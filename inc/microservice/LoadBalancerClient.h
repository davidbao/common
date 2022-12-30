//
//  LoadBalancerClient.h
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef LoadBalancerClient_h
#define LoadBalancerClient_h

#include "data/ValueType.h"
#include "data/Map.h"
#include "system/ServiceFactory.h"
#include "microservice/ServiceInstance.h"

using namespace Data;
using namespace System;

namespace Microservice {
    class IPing {
    public:
        IPing();

        virtual ~IPing();

        virtual bool isAlive(ServerProperty *server) = 0;
    };

    class ILoadBalancer : public IService {
    public:
        virtual void addServers(const ServerProperties &servers) = 0;

        virtual ServerProperty chooseServer(const String &zone) = 0;

        virtual void markServerDown(ServerProperty *server) = 0;

        virtual const ServerProperties &getReachableServers() const = 0;

        virtual const ServerProperties &getAllServers() const = 0;

        virtual String toString() const = 0;
    };

    class LoadBalancers {
    public:
        LoadBalancers();

        void add(const String &serviceId, ILoadBalancer *lb);

        ILoadBalancer *at(const String &serviceId) const;

    private:
        Map<String, ILoadBalancer> _lbs;
    };

    class IRule {
    public:
        IRule();

        virtual ~IRule();

        virtual ServerProperty choose(const String &zone) = 0;
    };

    class BaseRule : public IRule {
    public:
        BaseRule(ILoadBalancer *lb = nullptr);

        void setLoadBalancer(ILoadBalancer *lb);

        ILoadBalancer *getLoadBalancer();

    private:
        ILoadBalancer *_lb;
    };

    class RoundRobinRule : public BaseRule {
    public:
        RoundRobinRule(ILoadBalancer *lb = nullptr);

        ServerProperty choose(const String &zone) override;

        ServerProperty choose(ILoadBalancer *lb, const String &zone);

    private:
        int incrementAndGetModulo(int modulo);

    private:
        int _nextServerCyclicCounter;
    };

    class BaseLoadBalancer : public ILoadBalancer {
    public:
        BaseLoadBalancer(const String &lbName, IRule *rule = DEFAULT_RULE);

        ~BaseLoadBalancer() override;

        void addServers(const ServerProperties &servers) override;

        ServerProperty chooseServer(const String &zone) override;

        void markServerDown(ServerProperty *server) override;

        const ServerProperties &getReachableServers() const override;

        const ServerProperties &getAllServers() const override;

        String toString() const override;

        String name() const;

    protected:
        String _name;

        IRule *_rule;

        ServerProperties _allServers;
        Mutex _allServersMutex;

        ServerProperties _upServers;
        Mutex _upServersMutex;

    protected:
        static IRule *DEFAULT_RULE;
    };

    class ZoneAwareLoadBalancer : public BaseLoadBalancer {
    public:
        ZoneAwareLoadBalancer(const String &lbName, IRule *rule = DEFAULT_RULE);
    };

    class LoadBalancerClient : public IService, public ServiceInstanceChooser {
    public:
        LoadBalancerClient();

        ~LoadBalancerClient() override;

        Url reconstructUrl(const String &originalUrl);

        ServiceInstance choose(const String &serviceId) override;

        ServiceInstance choose(const String &serviceId, int zoneNo) override;

        virtual bool initialize();

        virtual bool unInitialize();

    protected:
        ILoadBalancer *getLoadBalancer(const String &serviceId);

        ServerProperty getServer(ILoadBalancer *loadBalancer, const String &zone = String::Empty);

    private:
        Mutex _lbsMutex;
        LoadBalancers _lbs;
    };

    class RibbonLoadBalancerClient : public LoadBalancerClient {
    public:
        RibbonLoadBalancerClient();

        ~RibbonLoadBalancerClient() override;
    };
}

#endif /* LoadBalancerClient_h */
