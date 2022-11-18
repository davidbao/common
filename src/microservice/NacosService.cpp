//
//  NacosService.cpp
//  common
//
//  Created by baowei on 2022/5/5.
//  Copyright © 2022 com. All rights reserved.
//

#include "microservice/NacosService.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"
#include "diag/Trace.h"
#include "thread/Timer.h"
#include "thread/TickTimeout.h"
#include "microservice/SummerApplication.h"
#include "configuration/ConfigService.h"

namespace Microservice {
    const char *NacosService::InstanceUrl = "/nacos/v1/ns/instance";
    const char *NacosService::InstanceBeatUrl = "/nacos/v1/ns/instance/beat";
    const char *NacosService::InstanceListUrl = "/nacos/v1/ns/instance/list";
    const char *NacosService::MetricsUrl = "/nacos/v1/ns/operator/metrics";
    const char *NacosService::ServiceListUrl = "/nacos/v1/ns/service/list";
    const char *NacosService::ClusterNodesUrl = "nacos/v1/core/cluster/nodes";
    const char *NacosService::LoginUrl = "nacos/v1/auth/users/login";

    const HttpHeaders NacosService::DefaultHeaders(new HttpHeader("Content-Type", "application/json"), nullptr);

    const char *NacosService::DefaultClusterName = "DEFAULT";
    const char *NacosService::DefaultGroupName = "DEFAULT_GROUP";

    NacosService::NacosService() : _registered(false), _loginTimer(nullptr) {
    }

    NacosService::~NacosService() {
        if (_loginTimer != nullptr) {
            delete _loginTimer;
            _loginTimer = nullptr;
        }
    }

    const String NacosService::serviceName() const {
        String serviceName;
        _instanceInfo.at("serviceName", serviceName);
        return serviceName;
    }

    const Url &NacosService::baseUrl() const {
        return _baseUrl;
    }

    bool NacosService::registerService() {
        Url baseUrl = createRegisterUrl();
        if (baseUrl.isEmpty()) {
            Trace::debug("The register url is incorrect!");
            return false;
        }
        _baseUrl = baseUrl;

        login();

        createInstanceProperties(_instanceInfo);
        if (registerService(_instanceInfo)) {
            _registered = true;

            // start management service.
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            Port servicePort;
            if (cs->getProperty("management.server.port", servicePort)) {
                String serviceName;
                _instanceInfo.at("serviceName", serviceName);
                String mserviceName = String::format("%s-management", serviceName.c_str());
                StringMap properties;
                createInstanceProperties(_instanceMInfo, mserviceName, servicePort);
                return registerService(_instanceMInfo);
            }
        }
        return false;
    }

    bool NacosService::registerService(const StringMap &properties) {
        if (_baseUrl.isEmpty())
            return false;
        if (properties.count() == 0)
            return false;

        Url url(_baseUrl, InstanceUrl);
        HttpResponse response(new HttpStringContent());
        return send(url, HttpMethod::Post, properties, response);

    }

    bool NacosService::deregisterService() {
        if (_registered) {
            deregisterService(_instanceInfo);
            deregisterService(_instanceMInfo);
            return true;
        }
        return false;
    }

    bool NacosService::deregisterService(const StringMap &properties) {
        if (_baseUrl.isEmpty())
            return false;
        if (properties.count() == 0)
            return false;

        Url url(_baseUrl, InstanceUrl);
        String serviceName, groupName, ip, port, clusterName, ephemeral, namespaceId;
        properties.at("serviceName", serviceName);
        properties.at("groupName", groupName);
        properties.at("ip", ip);
        properties.at("port", port);
        properties.at("clusterName", clusterName);
        properties.at("ephemeral", ephemeral);
        properties.at("namespaceId", namespaceId);
        const StringMap::KeyValue kvs[] = {
                {"serviceName", serviceName},
                {"groupName", groupName},
                {"ip", ip},
                {"port", port},
                {"clusterName", clusterName},
                {"ephemeral", ephemeral},
                {"namespaceId", namespaceId},
                nullptr
        };
        HttpResponse response(new HttpStringContent());
        return send(url, HttpMethod::Delete, StringMap(kvs), response);
    }

    bool NacosService::getServiceInstances(const String &serviceId, ServiceInstances &instances) {
        if (_baseUrl.isEmpty())
            return false;

        Url url(_baseUrl, InstanceListUrl);
        const StringMap::KeyValue kvs[] = {
                {"serviceName", serviceId},
                {"healthyOnly", "false"},
                nullptr
        };
        HttpResponse response(new HttpJsonContent());
        if (send(url, HttpMethod::Get, StringMap(kvs), response)) {
            for (uint32_t i = 0; i < instances.count(); i++) {
                ServiceInstance *instance = instances.at(i);
                instance->setAlive(false);
            }

            const HttpJsonContent *content = dynamic_cast<const HttpJsonContent *>(response.content);
            assert(content);
            JsonNode node;
            if (content->node().atByName("hosts", node)) {
                ServiceInstance *instance = nullptr;
                for (uint32_t i = 0; i < node.size(); i++) {
                    JsonNode serviceNode;
                    if (node.at(i, serviceNode)) {
                        if ((instance = instances.at(serviceId)) == nullptr) {
                            instance = new ServiceInstance();
                            instances.add(instance);
                            instance->setServiceId(serviceId);
                        }

                        String ip;
                        int port;
                        if (serviceNode.getAttribute("ip", ip) && serviceNode.getAttribute("port", port))
                            instance->setEndpoint(Endpoint(ip, port));

                        JsonNode metaNode;
                        if (serviceNode.atByName("metadata", metaNode)) {
                            StringMap meta;
                            StringArray names;
                            metaNode.getAttributeNames(names);
                            for (uint32_t i = 0; i < names.count(); i++) {
                                String value;
                                if (metaNode.getAttribute(names[i], value))
                                    meta.add(names[i], value);
                            }
                            instance->setMeta(meta);
                        }

                        bool healthy;
                        if (serviceNode.getAttribute("healthy", healthy))
                            instance->setAlive(healthy);
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool NacosService::chooseNext() {
        for (uint32_t i = 0; i < _endpoints.count(); i++) {
            const Endpoint &endpoint = _endpoints[i];
            if (endpoint == _current) {
                uint32_t pos = i == _endpoints.count() - 1 ? 0 : i + 1;
                _current = _endpoints[pos];
                return true;
            }
        }
        return false;
    }

    bool NacosService::isFirstEndpoint() const {
        return _endpoints.count() > 0 ? _current == _endpoints[0] : false;
    }

    bool NacosService::getHealthNode(JsonNode &node) {
        if (!_registered)
            return false;

        node.setName("nacos");

        String leader;
        StringArray serviceIds;
        if (getLeaderInfo(leader) && getAllServices(serviceIds)) {
            node.add(JsonNode("status", "UP"));
            JsonNode detailsNode("details");
            detailsNode.add(JsonNode("leader", leader));

            JsonNode servicesNode("services");
            for (uint32_t i = 0; i < serviceIds.count(); i++) {
                const String &serviceId = serviceIds[i];
                JsonNode serviceNode(serviceId, JsonNode::TypeArray);
                servicesNode.add(serviceNode);
            }
            detailsNode.add(servicesNode);

            node.add(detailsNode);
        } else {
            node.add(JsonNode("status", "DOWN"));
        }

        return true;
    }

    bool NacosService::checkServiceHealth() {
        if (_registered) {
            if (!checkServiceHealth(_instanceInfo))
                return false;
            return checkServiceHealth(_instanceMInfo);
        }
        return false;
    }

    bool NacosService::checkServiceHealth(const StringMap &properties) {
        if (_baseUrl.isEmpty())
            return false;
        if (properties.count() == 0)
            return false;

        Url url(_baseUrl, InstanceBeatUrl);
        String serviceName, groupName, ip, port, clusterName, weight, namespaceId, metadata;
        properties.at("serviceName", serviceName);
        properties.at("groupName", groupName);
        properties.at("ip", ip);
        properties.at("port", port);
        properties.at("clusterName", clusterName);
        properties.at("namespaceId", namespaceId);
        properties.at("weight", weight);
        properties.at("metadata", metadata);
        JsonNode beatNode;
        beatNode.add(JsonNode("port", port));
        beatNode.add(JsonNode("ip", ip));
        beatNode.add(JsonNode("weight", weight));
        beatNode.add(JsonNode("serviceName", serviceName));
        beatNode.add(JsonNode("cluster", clusterName));
        beatNode.add(JsonNode("scheduled", true));
        JsonNode metaNode;
        JsonNode::parse(metadata, metaNode);
        beatNode.add(JsonNode("metadata", metaNode));

        const StringMap::KeyValue kvs[] = {
                {"serviceName", serviceName},
                {"groupName", groupName},
                {"beat", beatNode.toString()},
                nullptr
        };
        HttpResponse response(new HttpStringContent());
        return send(url, HttpMethod::Put, StringMap(kvs), response);
    }

    bool NacosService::getServiceIds(StringArray &serviceIds) {
        return getAllServices(serviceIds);
    }

    Url NacosService::createRegisterUrl() {
        if (_current.isEmpty()) {
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            String host;
            Port port;
            cs->getProperty("summer.cloud.nacos.host", host);
            cs->getProperty("summer.cloud.nacos.port", port);

            Endpoint current;
            StringArray texts;
            StringArray::parseMultiSymbol(host, texts, ',', ';', ' ');
            _endpoints.clear();
            if (texts.count() > 1) {
                for (uint32_t i = 0; i < texts.count(); i++) {
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

    bool NacosService::parseMeta(const String &value, JsonNode &node) {
        if (!value.isNullOrEmpty()) {
            if (JsonNode::parse(value, node))
                return true;

            StringArray texts;
            StringArray::parseMultiSymbol(value, texts, ',', ';', ' ');
            for (uint32_t i = 0; i < texts.count(); i++) {
                const String &text = texts[i];
                StringArray texts2;
                StringArray::parse(text, texts2, '=');
                String key, value;
                if (texts2.count() == 2) {
                    key = texts2[0].trim();
                    value = texts2[1].trim();
                } else if (texts2.count() == 1) {
                    key = texts2[0].trim();
                    value = String::Empty;
                }

                if (!key.isNullOrEmpty()) {
                    node.add(JsonNode(key, value));
                }
            }
            return true;
        }
        return false;
    }

    void NacosService::createInstanceProperties(StringMap &properties, const String &serviceName, Port servicePort) {
        Application *app = Application::instance();
        assert(app);
        String name = app->name();

        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        String instanceId;
        if (!cs->getProperty("summer.cloud.nacos.discovery.instanceId", instanceId))
            instanceId = String::format("%s-%s", name.c_str(), Uuid::generate().toString().toLower().c_str());
        instanceId = String::replace(instanceId, ".", "-");
        instanceId = String::replace(instanceId, ":", "-");

        String namespaceId;
        cs->getProperty("summer.cloud.nacos.namespaceId", namespaceId);

        String groupName;
        cs->getProperty("summer.cloud.nacos.groupName", groupName);
        if (groupName.isNullOrEmpty())
            groupName = DefaultGroupName;
        name = String::format("%s@@%s", groupName.c_str(), name.c_str());

        String clusterName;
        cs->getProperty("summer.cloud.nacos.clusterName", clusterName);
        if (clusterName.isNullOrEmpty())
            clusterName = DefaultClusterName;

        String weight;
        cs->getProperty("summer.cloud.nacos.weight", weight);

        bool enabled = true;
        cs->getProperty("summer.cloud.nacos.enabled", enabled);

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
        cs->getProperty("summer.cloud.nacos.discovery.hostname", actuatorHost);
        if (actuatorHost.isNullOrEmpty()) {
            if (firstIpAddress.isNullOrEmpty())
                cs->getProperty("summer.cloud.client.ip-address", firstIpAddress);
            actuatorHost = firstIpAddress;
        }

        bool healthy = true;
        cs->getProperty("summer.cloud.nacos.healthy", healthy);

        bool ephemeral = true;
        cs->getProperty("summer.cloud.nacos.ephemeral", ephemeral);

        JsonNode metaNode;
        String tags;
        if (cs->getProperty("summer.cloud.nacos.discovery.tags", tags) && !tags.isNullOrEmpty()) {
            parseMeta(tags, metaNode);
        }

        String meta;
        if (cs->getProperty("summer.cloud.nacos.discovery.meta", meta) && !meta.isNullOrEmpty()) {
            parseMeta(meta, metaNode);
        }
        bool hasMeta = metaNode.hasAttribute();

        const StringMap::KeyValue kvs[] = {
                {"instanceId", instanceId},
                {"namespaceId", namespaceId},
                {"serviceName", serviceName.isNullOrEmpty() ? name.c_str() : serviceName.c_str()},
                {"groupName", groupName},
                {"clusterName", clusterName},
                {"ip", serverHost},
                {"port", UInt16(servicePort.isEmpty() ? (uint16_t) port : (uint16_t) servicePort).toString()},
                {"enable", Boolean(enabled).toString()},
                {"healthy", Boolean(healthy).toString()},
                {"ephemeral", Boolean(ephemeral).toString()},
                nullptr
        };
        properties = StringMap(kvs);
        if (!weight.isNullOrEmpty()) {
            double w = 1.0;
            if (Double::parse(weight, w)) {
                properties.add("weight", weight);
            }
        }
        if (hasMeta) {
            properties.add("metadata", metaNode.toString());
        }
    }

    bool NacosService::getAllServices(StringArray &serviceIds, int pageNo, int pageSize) {
        if (!_registered)
            return false;

        Url url(_baseUrl, ServiceListUrl);
        const StringMap::KeyValue kvs[] = {
                {"pageNo", Int32(pageNo).toString()},
                {"pageSize", Int32(pageSize).toString()},
                nullptr
        };
        HttpResponse response(new HttpJsonContent());
        if (send(url, HttpMethod::Get, StringMap(kvs), response)) {
            const HttpJsonContent *content = dynamic_cast<const HttpJsonContent *>(response.content);
            assert(content);
            return content->node().getAttribute("doms", serviceIds);
        }
        return false;
    }

    bool NacosService::getLeaderInfo(String &ip) {
        if (!_registered)
            return false;

        Url url(_baseUrl, ClusterNodesUrl);
        const StringMap::KeyValue kvs[] = {
                {"withInstances", "false"},
                {"pageNo", "1"},
                {"pageSize", "10"},
                nullptr
        };
        StringMap properties(kvs);
        HttpRequest request(url, HttpMethod::Get, DefaultHeaders, properties);
        HttpResponse response(new HttpJsonContent());
        if (send(url, HttpMethod::Get, response)) {
            const HttpJsonContent *content = dynamic_cast<const HttpJsonContent *>(response.content);
            assert(content);
            JsonNode node = content->value();
            JsonNode dataNode, serviceNode;
            node.atByName("data", dataNode);
            if (dataNode.size() > 0 && dataNode.at(0, serviceNode)) {
                JsonNode eNode, rNode, mNode, nNode;
                serviceNode.atByName("extendInfo", eNode);
                eNode.atByName("raftMetaData", rNode);
                rNode.atByName("metaDataMap", mNode);
                mNode.atByName("naming_service_metadata", nNode);
                Endpoint leader;
                if (nNode.getAttribute("leader", leader)) {
                    ip = leader.address;
                    return true;
                }
            }
        }
        return false;
    }

    bool NacosService::login() {
        if (!_baseUrl.isEmpty()) {
            Locker locker(&_httpClientMutex);

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);

            Url url(_baseUrl, LoginUrl);
            HttpRequest request(url, HttpMethod::Post, DefaultHeaders);
            String username, password;
            cs->getProperty("summer.cloud.nacos.username", username);
            cs->getProperty("summer.cloud.nacos.password", password);
            if (!username.isNullOrEmpty() && !password.isNullOrEmpty()) {
                const StringMap::KeyValue kvs[] = {
                        {"username", username},
                        {"password", password},
                        nullptr
                };
                request.properties = StringMap(kvs);
                HttpResponse response(new HttpJsonContent());
                if (_httpClient.send(request, response) && response.status == HttpOk) {
                    const HttpJsonContent *content = dynamic_cast<const HttpJsonContent *>(response.content);
                    assert(content);
                    if (AccessToken::parse(content->value(), _accessToken)) {
                        if (_loginTimer == nullptr) {
                            _loginTimer = new Timer("nacos.login.timer", loginTimeUp, this,
                                                    _accessToken.tokenTtl * 1000, _accessToken.tokenTtl * 1000);
                        } else {
                            _loginTimer->change(_accessToken.tokenTtl * 1000);
                        }
                    } else {
                        Trace::error(String::format("Failed to parse access token'%s'!", LoginUrl,
                                                    content->value().toString().c_str()));
                    }
                } else {
                    const HttpStringContent *content = dynamic_cast<const HttpStringContent *>(response.content);
                    assert(content);
                    Trace::error(String::format("Failed to send url'%s' to nocas! status: %d, response: %s",
                                                url.toString().c_str(), response.status, content->value().c_str()));
                }
            }
        }
        return false;
    }

    bool
    NacosService::send(const Url &url, const HttpMethod &method, const StringMap &properties, HttpResponse &response) {
        Locker locker(&_httpClientMutex);

        HttpRequest request(url, method, DefaultHeaders);
        request.properties = properties;
        request.properties.add("accessToken", _accessToken.accessToken);
        if (_httpClient.send(request, response) && response.status == HttpOk) {
            return true;
        } else {
            Trace::error(String::format("Failed to send url'%s' to nocas! status: %d", url.toString().c_str(),
                                        response.status));
            return false;
        }
    }

    bool NacosService::send(const Url &url, const HttpMethod &method, HttpResponse &response) {
        StringMap properties;
        return send(url, method, properties, response);
    }

    void NacosService::loginTimeUp(void *state) {
        NacosService *ns = (NacosService *) state;
        assert(ns);
        ns->login();
    }

    NacosService::AccessToken::AccessToken() {
        tokenTtl = 0;
        globalAdmin = false;
        lastRefTime = 0;
    }

    bool NacosService::AccessToken::parse(const JsonNode &node, AccessToken &accessToken) {
        if (node.getAttribute("username", accessToken.username) &&
            node.getAttribute("accessToken", accessToken.accessToken) &&
            node.getAttribute("tokenTtl", accessToken.tokenTtl) &&
            node.getAttribute("globalAdmin", accessToken.globalAdmin)) {
            return true;
        }
        return false;
    }
}
