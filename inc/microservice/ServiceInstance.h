//
//  ServiceInstance.h
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef ServiceInstance_h
#define ServiceInstance_h

#include "data/ValueType.h"
#include "net/NetType.h"
#include "data/StringMap.h"
#include "data/PList.h"
#include "system/ServiceFactory.h"

using namespace Common;
using namespace Net;

namespace Microservice
{
    class ServerProperty
    {
    public:
        ServerProperty(const Url& url = Url::Empty);
        ServerProperty(const ServerProperty& value);
        
        void operator=(const ServerProperty& value);
        bool operator==(const ServerProperty& value) const;
        bool operator!=(const ServerProperty& value) const;
        
        bool isAlive() const;
        void setAlive(bool alive);
        
        bool isReadyToServe() const;
        void setReadyToServe(bool isReadyToServe);
        
        String id() const;
        const String& scheme() const;
        
        const Endpoint& endpoint() const;
        void setEndpoint(const Endpoint& endpoint);
        
        bool isSecure() const;
        
        bool isEmpty() const;
        void empty();
        
        const String& zone() const;
        void setZone(const String& zone);
        
    private:
        Url _url;
        String _zone;
        bool _isAliveFlag;
        bool _readyToServe;
        
    public:
        static const ServerProperty Empty;

    private:
        static const String UNKNOWN_ZONE;
    };
    class ServerProperties
    {
    public:
        ServerProperties();
        
        void add(ServerProperty* server);
        void addRange(const ServerProperties& servers, bool onlyUp = false);
        
        void clear();
        
        size_t count() const;
        
//        bool contains(const String& seviceId) const;
        
        ServerProperty at(uint i) const;
        
    private:
        PList<ServerProperty> _servers;
    };

    class ServiceInstance
    {
    public:
        ServiceInstance();
        ServiceInstance(const String& serviceId, const ServerProperty& server);
        ServiceInstance(const ServiceInstance& value);
        
        virtual ~ServiceInstance();
        
        String instanceId() const;
        String scheme() const;

        const String& serviceId() const;
        void setServiceId(const String& serviceId);

        String address() const;
        int port() const;
        const Endpoint& endpoint() const;
        void setEndpoint(const Endpoint& endpoint);

        bool isSecure() const;
        
        bool isAlive() const;
        void setAlive(bool alive);
        
        bool isEmpty() const;
        void empty();
        
        const ServerProperty& server() const;
        
        const StringArray& tags() const;
        void setTags(const StringArray& tags);
        
        bool getTag(const String& name, String& value) const;
        
        const StringMap& meta() const;
        void setMeta(const StringMap& meta);
        
        void operator=(const ServiceInstance& value);
        bool operator==(const ServiceInstance& value) const;
        bool operator!=(const ServiceInstance& value) const;
        
    public:
        static const ServiceInstance Empty;
        
    private:
        String _serviceId;
        ServerProperty _server;
        
        StringArray _tags;
        StringMap _meta;
    };
    class ServiceInstances
    {
    public:
        ServiceInstances();
        
        void add(ServiceInstance* instance);
        
        size_t count() const;
        
        ServiceInstance* at(uint i) const;
        ServiceInstance* at(const String& serviceId) const;
        
        void toServerProperties(ServerProperties& servers) const;
        
    private:
        PList<ServiceInstance> _instances;
    };

    class ServiceInstanceChooser
    {
    public:
        ServiceInstanceChooser();
        virtual ~ServiceInstanceChooser();
        
        virtual ServiceInstance choose(const String& serviceId) = 0;
        virtual ServiceInstance choose(const String& serviceId, int zoneNo) = 0;
    };
}

#endif /* ServiceInstance_h */
