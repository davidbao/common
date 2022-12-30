//
//  LoadBalancerClient.cpp
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright © 2020 com. All rights reserved.
//

#include "diag/Trace.h"
#include "thread/Thread.h"
#include "microservice/LoadBalancerClient.h"
#include "microservice/ServiceDiscovery.h"

using namespace Diag;

namespace Microservice {
    LoadBalancers::LoadBalancers() : _lbs(true) {
    }

    ILoadBalancer *LoadBalancers::at(const String &serviceId) const {
        ILoadBalancer *lb = nullptr;
        if (_lbs.at(serviceId, lb))
            return lb;
        return nullptr;
    }

    void LoadBalancers::add(const String &serviceId, ILoadBalancer *lb) {
        if (!serviceId.isNullOrEmpty() && lb != nullptr)
            _lbs.add(serviceId, lb);
    }

    IRule::IRule() {
    }

    IRule::~IRule() {
    }

    BaseRule::BaseRule(ILoadBalancer *lb) {
        setLoadBalancer(lb);
    }

    void BaseRule::setLoadBalancer(ILoadBalancer *lb) {
        _lb = lb;
    }

    ILoadBalancer *BaseRule::getLoadBalancer() {
        return _lb;
    }

    RoundRobinRule::RoundRobinRule(ILoadBalancer *lb) : BaseRule(lb), _nextServerCyclicCounter(0) {
    }

    ServerProperty RoundRobinRule::choose(const String &zone) {
        return choose(getLoadBalancer(), zone);
    }

    ServerProperty RoundRobinRule::choose(ILoadBalancer *lb, const String &zone) {
        if (lb == nullptr) {
            Trace::writeLine("no load balancer", Trace::Warning);
            return ServerProperty::Empty;
        } else {
            ServerProperty server;
            int count = 0;

            while (true) {
                if (server.isEmpty() && count++ < 10) {
                    const ServerProperties &reachableServers = lb->getReachableServers();
                    const ServerProperties &allServers = lb->getAllServers();
                    size_t upCount = reachableServers.count();
                    size_t serverCount = allServers.count();
                    if (upCount != 0 && serverCount != 0) {
                        int nextServerIndex = incrementAndGetModulo(serverCount);
                        server = allServers.at(nextServerIndex);
                        if (server.isEmpty()) {
                            Thread::msleep(100);
                        } else {
                            if (server.isAlive() && server.isReadyToServe()) {
                                if (zone.isNullOrEmpty() || zone == server.zone()) {
                                    return server;
                                }
                            }

                            server.empty();
                        }
                        continue;
                    }

//                    Trace::warn(String::format("No up servers available from load balancer: %s", lb->toString().c_str()));
                    return ServerProperty::Empty;
                }

                if (count >= 10) {
                    Trace::warn(String::format("No available alive servers after 10 tries from load balancer: %s",
                                               lb->toString().c_str()));
                }

                return server;
            }
        }
    }

    int RoundRobinRule::incrementAndGetModulo(int modulo) {
        int current;
        int next;

        current = _nextServerCyclicCounter;
        next = (current + 1) % modulo;
        _nextServerCyclicCounter = next;

        return next;
    }

    IRule *BaseLoadBalancer::DEFAULT_RULE = new RoundRobinRule();

    BaseLoadBalancer::BaseLoadBalancer(const String &lbName, IRule *rule) : _name(lbName), _rule(rule) {
        BaseRule *br = dynamic_cast<BaseRule *>(rule);
        if (br != nullptr)
            br->setLoadBalancer(this);
    }

    BaseLoadBalancer::~BaseLoadBalancer() {
    }

    void BaseLoadBalancer::addServers(const ServerProperties &servers) {
        _allServersMutex.lock();
        _allServers.clear();
        _allServers.addRange(servers);
        _allServersMutex.unlock();

        _upServersMutex.lock();
        _upServers.clear();
        _upServers.addRange(servers, true);
        _upServersMutex.unlock();
    }

    ServerProperty BaseLoadBalancer::chooseServer(const String &zone) {
        return _rule != nullptr ? _rule->choose(zone) : ServerProperty();
    }

    void BaseLoadBalancer::markServerDown(ServerProperty *server) {
        if (server != nullptr && server->isAlive()) {
            Trace::writeLine(String::format("LoadBalancer [%s]: markServerDown called on [%s]", name().c_str(),
                                            server->id().c_str()));
            server->setAlive(false);
        }
    }

    const ServerProperties &BaseLoadBalancer::getReachableServers() const {
        return _upServers;
    }

    const ServerProperties &BaseLoadBalancer::getAllServers() const {
        return _allServers;
    }

    String BaseLoadBalancer::toString() const {
        return name();
    }

    String BaseLoadBalancer::name() const {
        return _name;
    }

    ZoneAwareLoadBalancer::ZoneAwareLoadBalancer(const String &lbName, IRule *rule) : BaseLoadBalancer(lbName, rule) {
    }

    LoadBalancerClient::LoadBalancerClient() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<LoadBalancerClient>(this);
    }

    LoadBalancerClient::~LoadBalancerClient() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<LoadBalancerClient>();
    }

    bool LoadBalancerClient::initialize() {
        return true;
    }

    bool LoadBalancerClient::unInitialize() {
        return true;
    }

    Url LoadBalancerClient::reconstructUrl(const String &originalUrl) {
        // http://consul-provider/hello
        Url original;
        if (Url::parse(originalUrl, original)) {
            String serviceId = original.address();
            ServiceInstance instance = choose(serviceId);
            if (!instance.isEmpty()) {
                Url url(original.scheme(), instance.endpoint(), original.relativeUrl());
                return url;
            }
        }
        return Url::Empty;
    }

    ILoadBalancer *LoadBalancerClient::getLoadBalancer(const String &serviceId) {
        Locker locker(&_lbsMutex);
        ILoadBalancer *lb = _lbs.at(serviceId);
        if (lb == nullptr) {
            lb = new ZoneAwareLoadBalancer("ZoneAware", new RoundRobinRule());
            _lbs.add(serviceId, lb);

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IServiceDiscovery *service = factory->getService<IServiceDiscovery>();
            assert(service);
            service->addServiceId(serviceId, lb);
        }
        return lb;
    }

    ServerProperty LoadBalancerClient::getServer(ILoadBalancer *loadBalancer, const String &zone) {
        return loadBalancer != nullptr ? loadBalancer->chooseServer(zone) : ServerProperty();
    }

    ServiceInstance LoadBalancerClient::choose(const String &serviceId) {
        ServerProperty server = this->getServer(this->getLoadBalancer(serviceId));
        return server.isEmpty() ? ServiceInstance::Empty : ServiceInstance(serviceId, server);
    }

    ServiceInstance LoadBalancerClient::choose(const String &serviceId, int zoneNo) {
        String zone = ((Int32) zoneNo).toString();
        ServerProperty server = this->getServer(this->getLoadBalancer(serviceId), zone);
        return server.isEmpty() ? ServiceInstance::Empty : ServiceInstance(serviceId, server);
    }

    RibbonLoadBalancerClient::RibbonLoadBalancerClient() {
    }

    RibbonLoadBalancerClient::~RibbonLoadBalancerClient() {
    }
}
