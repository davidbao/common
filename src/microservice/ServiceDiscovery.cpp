//
//  ServiceDiscovery.cpp
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "diag/Trace.h"
#include "microservice/ServiceDiscovery.h"
#include "microservice/SummerApplication.h"
#include "configuration/ConfigService.h"

using namespace Config;

namespace Microservice {
    ServiceDiscovery::Item::Item(const String &serviceId, ILoadBalancer *lb) : serviceId(serviceId), lb(lb) {
    }

    ServiceDiscovery::Items::Items() {
    }

    bool ServiceDiscovery::Items::contains(const String &serviceId) const {
        for (size_t i = 0; i < count(); i++) {
            const Item *item = at(i);
            if (item->serviceId == serviceId)
                return true;
        }
        return false;
    }

    ServiceDiscovery::ServiceDiscovery() : _serviceTimer(nullptr), _service(nullptr) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IServiceDiscovery>(this);
    }

    ServiceDiscovery::~ServiceDiscovery() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IServiceDiscovery>();
    }

    bool ServiceDiscovery::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        ServiceGovernanceFactory *sgfactory = ServiceGovernanceFactory::instance();
        assert(sgfactory);

        String name;
        StringArray names;
        sgfactory->getNames(names);
        for (size_t i = 0; i < names.count(); i++) {
            const String &n = names[i];

            bool enabled = false;
            cs->getProperty(String::format("summer.cloud.%s.enabled", n.c_str()), enabled);
            if (enabled) {
                name = n;
                break;
            }
        }
        if (name.isNullOrEmpty()) {
            return false;
        }

        if (_service == nullptr) {
            ServiceGovernanceFactory *factory = ServiceGovernanceFactory::instance();
            assert(factory);
            _service = dynamic_cast<IServiceGovernance *>(factory->createOrGetService(name));
        }

        if (_serviceTimer == nullptr) {
            auto serviceCallback = [](ServiceDiscovery *cs) {
                cs->updateServiceInstances();
            };
            _serviceTimer = new Timer("microservice.service.timer",
                                      TimeSpan::fromSeconds(5), serviceCallback, this);
        }

        return true;
    }

    bool ServiceDiscovery::unInitialize() {
        if (_serviceTimer != nullptr) {
            delete _serviceTimer;
            _serviceTimer = nullptr;
        }

        if (_service != nullptr) {
            _service = nullptr;
        }
        return true;
    }

    void ServiceDiscovery::updateServiceInstances(const String &serviceId) {
        if (_service != nullptr) {
            Locker locker(&_itemsMutex);

            for (size_t i = 0; i < _items.count(); i++) {
                Item *item = _items.at(i);
                if (serviceId.isNullOrEmpty() || item->serviceId == serviceId) {
                    if (_service->getServiceInstances(item->serviceId, item->instances)) {
                        if (item->lb != nullptr) {
                            ServerProperties servers;
                            item->instances.toServerProperties(servers);
                            item->lb->addServers(servers);
                        }
                    }
                }
            }
        }
    }

    void ServiceDiscovery::addServiceId(const String &serviceId, ILoadBalancer *lb) {
        _itemsMutex.lock();
        _items.add(new Item(serviceId, lb));
        _itemsMutex.unlock();

        updateServiceInstances(serviceId);
    }
}
