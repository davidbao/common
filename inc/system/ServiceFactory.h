#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include "data/String.h"
#include "data/Dictionary.h"
#include "system/Singleton.h"

using namespace Data;

namespace System {
    class IService {
    public:
        IService();

        virtual ~IService();
    };

    class ServiceFactory {
    public:
        ~ServiceFactory();

        template<class T>
        void addService(const String &name, T *service) {
            addServiceInner(getTypeName<T>(name), service);
        }

        template<class T>
        void addService(T *service) {
            addService<T>(String::Empty, service);
        }

        template<class T>
        void removeService(const String &name) {
            removeServiceInner(getTypeName<T>(name));
        }

        template<class T>
        void removeService() {
            removeService<T>(String::Empty);
        }

        template<class T>
        T *getService(const String &name) const {
            return dynamic_cast<T *>(getServiceInner(getTypeName<T>(name)));
        }

        template<class T>
        T *getService() const {
            return getService<T>(String::Empty);
        }

    private:
        ServiceFactory();

        DECLARE_SINGLETON_CLASS(ServiceFactory);

        template<class T>
        String getTypeName(const String &name) const {
            return name.isNullOrEmpty() ? String(typeid(T).name()) : String::format("%s.%s", typeid(T).name(),
                                                                                    name.c_str());
        }

        void addServiceInner(const String &name, IService *service);

        void removeServiceInner(const String &name);

        IService *getServiceInner(const String &name) const;

    public:
        static ServiceFactory *instance();

    private:
        Dictionary<String, IService *> _services;
    };
}
#endif // SERVICEFACTORY_H
