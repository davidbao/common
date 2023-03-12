//
//  TaskTimer.h
//  common
//
//  Created by baowei on 2017/4/3.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "system/ServiceFactory.h"

namespace System {
    IService::IService() = default;

    IService::~IService() = default;

    ServiceFactory::ServiceFactory() = default;

    ServiceFactory::~ServiceFactory() = default;

    void ServiceFactory::addServiceInner(const String &name, IService *service) {
        _services.add(name, service);
    }

    void ServiceFactory::removeServiceInner(const String &name) {
        _services.remove(name);
    }

    IService *ServiceFactory::getServiceInner(const String &name) const {
        IService *value = nullptr;
        if (_services.at(name, value))
            return value;
        return nullptr;
    }

    ServiceFactory *ServiceFactory::instance() {
        return Singleton<ServiceFactory>::instance();
    }
}
