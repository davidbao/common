//
//  ServiceGovernance.cpp
//  common
//
//  Created by baowei on 2020/2/27.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/ServiceGovernance.h"
#include "microservice/ConsulService.h"
#include "microservice/NacosService.h"

namespace Microservice {
    REGISTER_SERVICE(ConsulService);

    REGISTER_SERVICE(NacosService);

    IServiceGovernance::IServiceGovernance() = default;

    IServiceGovernance::~IServiceGovernance() = default;

    ServiceGovernanceFactory *ServiceGovernanceFactory::_instance = nullptr;

    ServiceGovernanceFactory::ServiceGovernanceFactory() : _services(false) {
    }

    ServiceGovernanceFactory::~ServiceGovernanceFactory() = default;

    void ServiceGovernanceFactory::registerService(const String &typeName, Constructor constructor) {
        if (typeName.isNullOrEmpty())
            return;

        String name;
        ssize_t index;
        if ((index = typeName.find("Service")) > 0) {
            name = typeName.substr(0, index).toLower();
        } else {
            name = typeName;
        }
        _constructors.add(name, constructor);
    }

    IServiceGovernance *ServiceGovernanceFactory::create(const String &typeName) {
        Constructor constructor = nullptr;
        if (_constructors.at(typeName, constructor)) {
            if (constructor != nullptr)
                return (*constructor)();
        }
        return nullptr;
    }

    IServiceGovernance *ServiceGovernanceFactory::createOrGetService(const String &typeName) {
        Locker locker(&_servicesMutex);

        IServiceGovernance *service = nullptr;
        if (_services.at(typeName, service))
            return service;

        service = create(typeName);
        if (service != nullptr) {
            _services.add(typeName, service);
            return service;
        }
        return nullptr;
    }

    ServiceGovernanceFactory *ServiceGovernanceFactory::instance() {
        if (_instance == nullptr)
            _instance = new ServiceGovernanceFactory();
        return _instance;
    }

    void ServiceGovernanceFactory::getNames(StringArray &names) {
        _constructors.keys(names);
    }
}
