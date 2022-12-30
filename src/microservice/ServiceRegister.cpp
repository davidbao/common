//
//  ServiceRegister.cpp
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/ServiceRegister.h"
#include "microservice/SummerApplication.h"
#include "configuration/ConfigService.h"

using namespace Config;

namespace Microservice {
    ServiceRegister::ServiceRegister() : _registered(false), _registerTimer(nullptr), _service(nullptr) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IServiceRegister>(this);
    }

    ServiceRegister::~ServiceRegister() {
        unInitialize();

        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IServiceRegister>();
    }

    bool ServiceRegister::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        ServiceGovernanceFactory *sgfactory = ServiceGovernanceFactory::instance();
        assert(sgfactory);

        StringArray names;
        sgfactory->getNames(names);
        for (uint32_t i = 0; i < names.count(); i++) {
            const String &name = names[i];

            bool enable = true;
            if (cs->getProperty(String::format("summer.cloud.%s.enabled", name.c_str()), enable) && !enable) {
                continue;
            }
            bool allowRegister = false;
            if (!(cs->getProperty(String::format("summer.cloud.%s.discovery.register", name.c_str()), allowRegister) &&
                  allowRegister)) {
                continue;
            }

            _name = name;

            String value;
            cs->getProperty(String::format("summer.cloud.%s.discovery.healthCheckInterval", name.c_str()), value);
            int checkInterval = 10;
            Int32::parse(value.trim('S', 's'), checkInterval);
            _registerTimer = new Timer("microservice.registertimer", registerCallback, this,
                                       TimeSpan::fromSeconds(checkInterval));
        }

        return true;
    }

    bool ServiceRegister::unInitialize() {
        if (_registerTimer != nullptr) {
            delete _registerTimer;
            _registerTimer = nullptr;
        }

        if (_service != nullptr) {
            _service->deregisterService();
            delete _service;
            _service = nullptr;
        }

        return true;
    }

    bool ServiceRegister::getServiceIds(StringArray &serviceIds) {
        if (_service != nullptr) {
            return _service->getServiceIds(serviceIds);
        }
        return false;
    }

    bool ServiceRegister::getHealthNode(JsonNode &node) const {
        if (_service != nullptr) {
            return _service->getHealthNode(node);
        }
        return false;
    }

    void ServiceRegister::registerCallback(void *parameter) {
        ServiceRegister *pr = (ServiceRegister *) parameter;
        assert(pr);
        pr->registerProcInner();
    }

    void ServiceRegister::registerProcInner() {
        if (!_registered) {
            assert(!_name.isNullOrEmpty());
            ServiceGovernanceFactory *factory = ServiceGovernanceFactory::instance();
            assert(factory);
            if (_service == nullptr)
                _service = dynamic_cast<IServiceGovernance *>(factory->createOrGetService(_name));

            if (_service->registerService())
                _registered = true;
            else {
                _service->chooseNext();
                if (!_service->isFirstEndpoint())
                    registerProcInner();
            }
        } else {
            static int count = 0;
            if (!_service->checkServiceHealth()) {
                count++;
                if (count >= 2) {
                    _registered = false;
                    _service->chooseNext();
                    if (!_service->isFirstEndpoint())
                        registerProcInner();
                }
            }
        }
    }
}
