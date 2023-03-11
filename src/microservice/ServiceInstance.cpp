//
//  ServiceInstance.cpp
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/ServiceInstance.h"
#include "system/Regex.h"

using namespace System;

namespace Microservice {
    const ServerProperty ServerProperty::Empty;
    const String ServerProperty::UNKNOWN_ZONE = "UNKNOWN";

    ServerProperty::ServerProperty(const Url &url) {
        this->_url = url;
        this->_isAliveFlag = false;
        this->_zone = UNKNOWN_ZONE;
        this->_readyToServe = true;
    }

    ServerProperty::ServerProperty(const ServerProperty &value) {
        this->operator=(value);
    }

    void ServerProperty::operator=(const ServerProperty &value) {
        this->_url = value._url;
        this->_isAliveFlag = value._isAliveFlag;
        this->_zone = value._zone;
        this->_readyToServe = value._readyToServe;
    }

    bool ServerProperty::operator==(const ServerProperty &value) const {
        return this->_url == value._url &&
               this->_isAliveFlag == value._isAliveFlag &&
               this->_zone == value._zone &&
               this->_readyToServe == value._readyToServe;
    }

    bool ServerProperty::operator!=(const ServerProperty &value) const {
        return !operator==(value);
    }

    bool ServerProperty::isAlive() const {
        return _isAliveFlag;
    }

    void ServerProperty::setAlive(bool alive) {
        _isAliveFlag = alive;
    }

    bool ServerProperty::isReadyToServe() const {
        return _readyToServe;
    }

    void ServerProperty::setReadyToServe(bool alive) {
        _readyToServe = alive;
    }

    String ServerProperty::id() const {
        return _url.endpoint().toString();
    }

    const String &ServerProperty::scheme() const {
        return _url.scheme();
    }

    const Endpoint &ServerProperty::endpoint() const {
        return _url.endpoint();
    }

    void ServerProperty::setEndpoint(const Endpoint &endpoint) {
        _url = Url(_url.scheme(), endpoint);
    }

    bool ServerProperty::isSecure() const {
        return _url.isSecure();
    }

    bool ServerProperty::isEmpty() const {
        return _url.isEmpty();
    }

    void ServerProperty::empty() {
        _url.empty();
    }

    const String &ServerProperty::zone() const {
        return _zone;
    }

    void ServerProperty::setZone(const String &zone) {
        _zone = zone;
    }

    ServerProperties::ServerProperties() {
    }

    void ServerProperties::add(ServerProperty *server) {
        _servers.add(server);
    }

    void ServerProperties::addRange(const ServerProperties &servers, bool onlyUp) {
        for (size_t i = 0; i < servers.count(); i++) {
            ServerProperty server = servers.at(i);
            if ((onlyUp && server.isAlive()) || !onlyUp)
                _servers.add(new ServerProperty(server));
        }
    }

    void ServerProperties::clear() {
        _servers.clear();
    }

    size_t ServerProperties::count() const {
        return _servers.count();
    }

//    bool ServerProperties::contains(const String& seviceId) const
//    {
//        for(uint32_t i=0; i<count(); i++)
//        {
//            const 
//        }
//    }
    ServerProperty ServerProperties::at(size_t i) const {
        ServerProperty *server = _servers.at(i);
        return server != nullptr ? *server : ServerProperty::Empty;
    }

    const ServiceInstance ServiceInstance::Empty;

    ServiceInstance::ServiceInstance() {
    }

    ServiceInstance::ServiceInstance(const String &serviceId, const ServerProperty &server) : _serviceId(serviceId),
                                                                                              _server(server) {
    }

    ServiceInstance::ServiceInstance(const ServiceInstance &value) {
        this->operator=(value);
    }

    ServiceInstance::~ServiceInstance() {
    }

    String ServiceInstance::instanceId() const {
        return _server.id();
    }

    String ServiceInstance::scheme() const {
        return _server.scheme();
    }

    const String &ServiceInstance::serviceId() const {
        return _serviceId;
    }

    void ServiceInstance::setServiceId(const String &serviceId) {
        _serviceId = serviceId;
    }

    String ServiceInstance::address() const {
        return _server.endpoint().address;
    }

    int ServiceInstance::port() const {
        return _server.endpoint().port;
    }

    const Endpoint &ServiceInstance::endpoint() const {
        return _server.endpoint();
    }

    void ServiceInstance::setEndpoint(const Endpoint &endpoint) {
        _server.setEndpoint(endpoint);
    }

    bool ServiceInstance::isSecure() const {
        return _server.isSecure();
    }

    bool ServiceInstance::isAlive() const {
        return _server.isAlive();
    }

    void ServiceInstance::setAlive(bool alive) {
        _server.setAlive(alive);
    }

    bool ServiceInstance::isEmpty() const {
        return _serviceId.isNullOrEmpty() || _server.isEmpty();
    }

    void ServiceInstance::empty() {
        _serviceId.empty();
        _server.empty();
    }

    const ServerProperty &ServiceInstance::server() const {
        return _server;
    }

    const StringArray &ServiceInstance::tags() const {
        return _tags;
    }

    void ServiceInstance::setTags(const StringArray &tags) {
        _tags = tags;
    }

    bool ServiceInstance::getTag(const String &name, String &value) const {
        static const Regex expressionRegex("^(\\S+)=(.*)$");

        StringArray groups;
        for (size_t i = 0; i < _tags.count(); i++) {
            const String &tag = _tags[i];
            if (expressionRegex.match(tag, groups) && groups.count() == 2 &&
                groups[0] == name) {
                value = groups[1];
                return true;
            }
        }
        return false;
    }

    const StringMap &ServiceInstance::meta() const {
        return _meta;
    }

    void ServiceInstance::setMeta(const StringMap &meta) {
        _meta = meta;
    }

    void ServiceInstance::operator=(const ServiceInstance &value) {
        this->_serviceId = value._serviceId;
        this->_server = value._server;
        this->_tags = value._tags;
        this->_meta = value._meta;
    }

    bool ServiceInstance::operator==(const ServiceInstance &value) const {
        return this->_serviceId == value._serviceId &&
               this->_server == value._server &&
               this->_tags == value._tags &&
               this->_meta == value._meta;
    }

    bool ServiceInstance::operator!=(const ServiceInstance &value) const {
        return !operator==(value);
    }

    ServiceInstances::ServiceInstances() {
    }

    void ServiceInstances::add(ServiceInstance *instance) {
        _instances.add(instance);
    }

    size_t ServiceInstances::count() const {
        return _instances.count();
    }

    ServiceInstance *ServiceInstances::at(size_t i) const {
        return _instances.at(i);
//        ServiceInstance* instance = _instances.at(i);
//        return instance != nullptr ? *instance : ServiceInstance::Empty;
    }

    ServiceInstance *ServiceInstances::at(const String &serviceId) const {
        for (size_t i = 0; i < _instances.count(); i++) {
            ServiceInstance *instance = _instances[i];
            if (instance != nullptr && instance->serviceId() == serviceId)
                return instance;
        }
        return nullptr;
    }

    void ServiceInstances::toServerProperties(ServerProperties &servers) const {
        for (size_t i = 0; i < _instances.count(); i++) {
            const ServiceInstance *instance = _instances[i];
            ServerProperty *prop = new ServerProperty(instance->server());
            servers.add(prop);

            const StringArray &tags = instance->tags();
            if (tags.count() > 0) {
                static const String zoneNoStr = "zoneNo=";
                for (size_t i = 0; i < tags.count(); i++) {
                    const String &tag = tags[i];
                    if (tag.find(zoneNoStr) >= 0) {
                        String str = tag.substr((off_t) zoneNoStr.length(), tag.length() - zoneNoStr.length());
                        prop->setZone(str);
                        break;
                    }
                }
            } else {
                const StringMap &meta = instance->meta();
                if (meta.count() > 0) {
                    String zoneNoStr;
                    if (meta.at("zoneNo", zoneNoStr)) {
                        prop->setZone(zoneNoStr);
                    }
                }
            }
        }
    }

    ServiceInstanceChooser::ServiceInstanceChooser() {
    }

    ServiceInstanceChooser::~ServiceInstanceChooser() {
    }
}
