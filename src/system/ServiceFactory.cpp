#include "system/ServiceFactory.h"
#include "diag/Trace.h"

namespace System {
    IService::IService() {
    }

    IService::~IService() {
    }

    ServiceFactory::ServiceFactory() {
    }

    ServiceFactory::~ServiceFactory() {
    }

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
