//
//  ConsulService.cpp
//  common
//
//  Created by baowei on 2020/2/18.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/ConsulService.h"
#include "json/JsonNode.h"
#include "diag/Trace.h"
#include "microservice/SummerApplication.h"
#include "configuration/ConfigService.h"

using namespace Config;

namespace Microservice {
    const char *ConsulService::DeregisterUrl = "v1/agent/service/deregister/%s";
    const char *ConsulService::RegisterUrl = "v1/agent/service/register";
    const char *ConsulService::GetServiceUrl = "v1/agent/service/%s";
    const char *ConsulService::GetAllServicesUrl = "v1/agent/services";
    const char *ConsulService::GetServicesUrl = "v1/agent/services?filter=%s";
    const char *ConsulService::CheckServiceHealthUrl = "v1/agent/checks?filter=%s";
    const char *ConsulService::GetLeaderUrl = "v1/status/leader";

    const HttpHeaders ConsulService::DefaultHeaders(new HttpHeader("Content-Type", "application/json"), nullptr);

    ConsulService::ConsulService() {
    }

    ConsulService::~ConsulService() {
    }

    const String ConsulService::serviceId() const {
        return _serviceId;
    }

    const String ConsulService::serviceName() const {
        return _serviceName;
    }

    const Url &ConsulService::baseUrl() const {
        return _baseUrl;
    }

    bool ConsulService::registerService() {
        Url baseUrl = createRegisterUrl();
        if (baseUrl.isEmpty()) {
            Trace::debug("The register url is incorrect!");
            return false;
        }
        _baseUrl = baseUrl;

        String payload = createPayload();
        JsonNode node;
        if (!JsonNode::parse(payload, node)) {
            Trace::debug("payload is incorrect!");
            return false;
        }

        if (!node.getAttribute("id", _serviceId)) {
            Trace::debug("service id is incorrect!");
            return false;
        }
        if (!node.getAttribute("name", _serviceName)) {
            Trace::debug("service name is incorrect!");
            return false;
        }

        Url url(baseUrl, RegisterUrl);
        if (_httpClient.put(url.toString(), DefaultHeaders, payload)) {
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            Port servicePort;
            if (cs->getProperty("management.server.port", servicePort)) {
                String serviceName = String::format("%s-management", _serviceName.c_str());
                String serviceId = String::format("%s-management", _serviceId.c_str());
                payload = createPayload(serviceName, serviceId, servicePort);
                return _httpClient.put(url.toString(), DefaultHeaders, payload);
            }
            return true;
        }
        return false;
    }

    bool ConsulService::deregisterService() {
        if (!_baseUrl.isEmpty() && !_serviceId.isNullOrEmpty()) {
            Url url(_baseUrl, String::format(DeregisterUrl, _serviceId.c_str()));
            _httpClient.put(url, DefaultHeaders);

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            if (cs->contains("management.server.port")) {
                String serviceId = String::format("%s-management", _serviceId.c_str());
                Url url(_baseUrl, String::format(DeregisterUrl, serviceId.c_str()));
                _httpClient.put(url, DefaultHeaders);
            }
            return true;
        }
        return false;
    }

    bool ConsulService::getServiceById(const Url &baseUrl, const String &serviceId, String &content) {
        if (baseUrl.isEmpty())
            return false;

        Url url(baseUrl, String::format(GetServiceUrl, serviceId.c_str()));
        if (_httpClient.get(url.toString(), DefaultHeaders, content)) {
            return true;
        }
        return false;
    }

    bool ConsulService::getServiceInstances(const String &serviceId, ServiceInstances &instances) {
        if (_baseUrl.isEmpty()) {
            Url baseUrl = createRegisterUrl();
            if (baseUrl.isEmpty()) {
                Trace::debug("The register url is incorrect!");
                return false;
            }
            _baseUrl = baseUrl;
        }

        String filter1 = String::format("Service==\"%s\"", serviceId.c_str());
        Url url1(_baseUrl, String::format(GetServicesUrl, filter1.c_str()));
        String content;
        if (_httpClient.get(url1, DefaultHeaders, content) && parseInstanceJson(content, instances)) {
            String filter2 = String::format("ServiceName==\"%s\"", serviceId.c_str());
            Url url2(_baseUrl, String::format(CheckServiceHealthUrl, filter2.c_str()));
            if (_httpClient.get(url2, DefaultHeaders, content) && parseCheckJson(content, &instances))
                return true;
        }
        return false;
    }

    bool ConsulService::chooseNext() {
        for (size_t i = 0; i < _endpoints.count(); i++) {
            const Endpoint &endpoint = _endpoints[i];
            if (endpoint == _current) {
                size_t pos = i == _endpoints.count() - 1 ? 0 : i + 1;
                _current = _endpoints[pos];
                return true;
            }
        }
        return false;
    }

    bool ConsulService::isFirstEndpoint() const {
        return _endpoints.count() > 0 ? _current == _endpoints[0] : false;
    }

    bool ConsulService::getHealthNode(JsonNode &node) {
        if (_baseUrl.isEmpty())
            return false;

        node.setName("consul");

        String content;
        JsonNode tempNode;
        if (getAllServices(_baseUrl, content) && JsonNode::parse(content, tempNode)) {
            node.add(JsonNode("status", "UP"));
            JsonNode detailsNode("details");
            String leader;
            Url url(_baseUrl, GetLeaderUrl);
            _httpClient.get(url, DefaultHeaders, leader);
            leader = leader.trim('\"', '\r', '\n');
            detailsNode.add(JsonNode("leader", leader));

            JsonNode servicesNode("services");
            JsonNodes nodes;
            if (tempNode.subNodes(nodes)) {
                for (size_t i = 0; i < nodes.count(); i++) {
                    const JsonNode &subNode = nodes[i];
                    String serviceId;
                    StringArray tags;
                    if (subNode.getAttribute("Service", serviceId) && subNode.getAttribute("Tags", tags)) {
                        JsonNode serviceNode(serviceId, JsonNode::TypeArray);
                        for (size_t i = 0; i < tags.count(); i++) {
                            const String &tag = tags[i];
                            serviceNode.add(JsonNode("item", tag));
                        }
                        servicesNode.add(serviceNode);
                    }
                }
            }
            detailsNode.add(servicesNode);

            node.add(detailsNode);
        } else {
            node.add(JsonNode("status", "DOWN"));
        }

        return true;
    }

    bool ConsulService::checkServiceHealth() {
        if (_baseUrl.isEmpty())
            return false;

        Application *app = Application::instance();
        assert(app);
        String name = app->name();
        const String &serviceId = name;
        String filter = String::format("ServiceName==\"%s\"", serviceId.c_str());
        Url url(_baseUrl, String::format(CheckServiceHealthUrl, filter.c_str()));
        String content;
        if (_httpClient.get(url.toString(), DefaultHeaders, content)) {
            JsonNode node;
            if (JsonNode::parse(content, node)) {
                JsonNodes nodes;
                if (node.subNodes(nodes) && nodes.count() == 1) {
                    const JsonNode &serviceNode = nodes[0];
                    String serviceName;
                    if (serviceNode.getAttribute("ServiceName", serviceName)) {
                        return serviceName == serviceId;
                    }
                }
            }
        }
        return false;
    }

    bool ConsulService::getServiceIds(StringArray &serviceIds) {
        if (_baseUrl.isEmpty())
            return false;

        String content;
        JsonNode node;
        if (getAllServices(_baseUrl, content) && JsonNode::parse(content, node)) {
            JsonNodes nodes;
            if (node.subNodes(nodes)) {
                for (size_t i = 0; i < nodes.count(); i++) {
                    const JsonNode &subNode = nodes[i];
                    String serviceId;
                    if (subNode.getAttribute("Service", serviceId))
                        serviceIds.add(serviceId);
                }
                return nodes.count() > 0;
            }
        }
        return false;
    }

    bool ConsulService::getAllServices(const Url &baseUrl, String &content) {
        if (baseUrl.isEmpty())
            return false;

        Url url(baseUrl, GetAllServicesUrl);
        if (_httpClient.get(url, DefaultHeaders, content)) {
            return true;
        }
        return false;
    }

    bool ConsulService::parseInstanceJson(const String &content, ServiceInstances &instances) {
        if (content.isNullOrEmpty())
            return false;

        JsonNode root;
        if (!JsonNode::parse(content, root) && root.type() == JsonNode::TypeArray)
            return false;

        for (size_t i = 0; i < instances.count(); i++) {
            ServiceInstance *instance = instances.at(i);
            instance->setAlive(false);
        }

        ServiceInstance *instance = nullptr;
        for (size_t i = 0; i < root.count(); i++) {
            JsonNode serviceNode;
            if (root.at(i, serviceNode)) {
                String serviceId;
                if (serviceNode.getAttribute("ID", serviceId)) {
                    if ((instance = instances.at(serviceId)) == nullptr) {
                        instance = new ServiceInstance();
                        instances.add(instance);
                        instance->setServiceId(serviceId);
                    }

                    Endpoint endpoint;
                    if (serviceNode.getAttribute("Address", endpoint.address) &&
                        serviceNode.getAttribute("Port", endpoint.port))
                        instance->setEndpoint(endpoint);

                    StringArray tags;
                    if (serviceNode.getAttribute("Tags", tags))
                        instance->setTags(tags);

                    JsonNode metaNode;
                    if (serviceNode.at("Meta", metaNode)) {
                        StringMap meta;
                        StringArray names;
                        metaNode.getAttributeNames(names);
                        for (size_t i = 0; i < names.count(); i++) {
                            String value;
                            if (metaNode.getAttribute(names[i], value))
                                meta.add(names[i], value);
                        }
                        instance->setMeta(meta);
                    }
                }
            }
        }
        return true;
    }

    bool ConsulService::parseCheckJson(const String &content, ServiceInstances *instances) {
        JsonNode root;
        if (!JsonNode::parse(content, root) && root.type() == JsonNode::TypeArray)
            return false;

        ServiceInstance *instance = nullptr;
        for (size_t i = 0; i < root.count(); i++) {
            JsonNode serviceNode;
            if (root.at(i, serviceNode)) {
                String serviceId;
                if (serviceNode.getAttribute("CheckID", serviceId)) {
                    serviceId = String::replace(serviceId, "service:", String::Empty);
                    if ((instance = instances->at(serviceId)) != nullptr) {
                        String status;
                        if (serviceNode.getAttribute("Status", status))
                            instance->setAlive(status == "passing");
                    }
                }
            }
        }
        return true;
    }

    Url ConsulService::createRegisterUrl() {
        if (_current.isEmpty()) {
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            String host;
            Port port;
            cs->getProperty("summer.cloud.consul.host", host);
            cs->getProperty("summer.cloud.consul.port", port);

            Endpoint current;
            StringArray texts;
            StringArray::parseMultiSymbol(host, texts, ',', ';', ' ');
            _endpoints.clear();
            if (texts.count() > 1) {
                for (size_t i = 0; i < texts.count(); i++) {
                    const String &text = texts[i];
                    Endpoint endpoint;
                    if (Endpoint::parse(text, endpoint)) {
                        _endpoints.add(endpoint);
                    }
                }
                if (_endpoints.count() > 0)
                    _current = _endpoints[0];
            }

            if (_current.isEmpty()) {
                _current = Endpoint(host, port);
                _endpoints.add(_current);
            }
        }

        return Url(Url::SchemeHttp, _current);
    }

    String ConsulService::createPayload(const String &serviceName, const String &serviceId, Port servicePort) {
        Application *app = Application::instance();
        assert(app);
        String name = app->name();

        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        if (!cs->getProperty("summer.cloud.consul.discovery.instanceId", _serviceId))
            _serviceId = String::format("%s-%s", name.c_str(), Uuid::generate().toString().toLower().c_str());
        _serviceId = String::replace(_serviceId, ".", "-");
        _serviceId = String::replace(_serviceId, ":", "-");

        String firstIpAddress;
        String serverHost;
        cs->getProperty("server.host", serverHost);
        if (serverHost.isNullOrEmpty()) {
            cs->getProperty("summer.cloud.client.ip-address", firstIpAddress);
            serverHost = firstIpAddress;
        }
        Port port;
        cs->getProperty("server.port", port);
        Port actuatorPort = port;
        cs->getProperty("management.server.port", actuatorPort);

        String actuatorHost;
        cs->getProperty("summer.cloud.consul.discovery.hostname", actuatorHost);
        if (actuatorHost.isNullOrEmpty()) {
            if (firstIpAddress.isNullOrEmpty())
                cs->getProperty("summer.cloud.client.ip-address", firstIpAddress);
            actuatorHost = firstIpAddress;
        }

        String healthCheckPath = "/actuator/health";
        cs->getProperty("summer.cloud.consul.discovery.healthCheckPath", healthCheckPath);
        Url checkUrl = Url(String::format("http://%s:%d/", actuatorHost.c_str(), (uint16_t) actuatorPort),
                           healthCheckPath);

        String value;
        cs->getProperty("summer.cloud.consul.discovery.healthCheckInterval", value);
        int checkInterval = 10;
        Int32::parse(value.trim('S', 's'), checkInterval);

        String tags;
        if (cs->getProperty("summer.cloud.consul.discovery.tags", value) && !value.isNullOrEmpty()) {
            StringArray texts;
            StringArray::parseMultiSymbol(value, texts, ',', ';', ' ');
            for (size_t i = 0; i < texts.count(); i++) {
                const String &text = texts[i];
                if (tags.length() > 0)
                    tags.append(',');
                tags.append(String::format("\"%s\"", text.c_str()));
            }
        }

        String meta;
        if (cs->getProperty("summer.cloud.consul.discovery.meta", value) && !value.isNullOrEmpty()) {
            StringArray texts;
            StringArray::parseMultiSymbol(value, texts, ',', ';', ' ');
            for (size_t i = 0; i < texts.count(); i++) {
                const String &text = texts[i];
                if (meta.length() > 0)
                    meta.append(',');
                StringArray texts2;
                StringArray::parse(text, texts2, '=');
                String key, value;
                if (texts2.count() == 2) {
                    key = texts2[0].trim();
                    value = texts2[1].trim();
                } else if (texts2.count() == 1) {
                    key = texts2[0].trim();
                    value = "0";
                }
                if (!key.isNullOrEmpty())
                    meta = String::format("\"%s\":\"%s\"", key.c_str(), value.c_str());
            }
        }

        String payload = String::format(
                "{\
                          \"id\": \"%s\",\
                          \"name\": \"%s\",\
                          \"address\": \"%s\",\
                          \"port\": %d,\
                          \"Tags\": [\
                              %s\
                          ],\
                          \"Meta\": {\
                              %s\
                          },\
                          \"check\": {\
                            \"http\": \"%s\",\
                            \"interval\": \"%ds\",\
                            \"DeregisterCriticalServiceAfter\": \"30s\"\
                          }\
                        }",
                serviceId.isNullOrEmpty() ? _serviceId.c_str() : serviceId.c_str(),
                serviceName.isNullOrEmpty() ? name.c_str() : serviceName.c_str(),
                serverHost.c_str(),
                servicePort.isEmpty() ? (uint16_t) port : (uint16_t) servicePort,
                tags.c_str(),
                meta.c_str(),
                checkUrl.toString().c_str(),
                checkInterval);
        return payload;
    }
}
