//
//  ServiceDiscovery.h
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef ServiceDiscovery_h
#define ServiceDiscovery_h

#include "thread/Timer.h"
#include "data/Dictionary.h"
#include "system/ServiceFactory.h"
#include "microservice/ServiceGovernance.h"
#include "microservice/ServiceInstance.h"
#include "microservice/LoadBalancerClient.h"

using namespace Data;

namespace Microservice {
    class IServiceDiscovery : public IService {
    public:
        virtual void addServiceId(const String &serviceId, ILoadBalancer *lb) = 0;
    };

    class ServiceDiscovery : public IServiceDiscovery {
    public:
        class Item {
        public:
            String serviceId;
            ILoadBalancer *lb;
            ServiceInstances instances;

            Item(const String &serviceId, ILoadBalancer *lb);
        };

        class Items : public PList<Item> {
        public:
            Items();

            bool contains(const String &serviceId) const;
        };

        ServiceDiscovery();

        ~ServiceDiscovery() override;

        bool initialize();

        bool unInitialize();

        void addServiceId(const String &serviceId, ILoadBalancer *lb) override;

    private:
        void updateServiceInstances(const String &serviceId = String::Empty);

    private:
        Timer *_serviceTimer;

        IServiceGovernance *_service;

        Mutex _itemsMutex;
        Items _items;
    };
}

#endif /* ServiceDiscovery_h */
