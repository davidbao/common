//
//  ServiceGovernance.h
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef ServiceGovernance_h
#define ServiceGovernance_h

#include "data/String.h"
#include "json/JsonNode.h"
#include "system/ServiceFactory.h"
#include "microservice/RestTemplate.h"
#include "microservice/ServiceInstance.h"

using namespace Data;

namespace Microservice
{
    class IServiceGovernance
    {
    public:
        IServiceGovernance();
        virtual ~IServiceGovernance();
        
        virtual bool registerService() = 0;
        virtual bool deregisterService() = 0;
        
        virtual bool getServiceInstances(const String& serviceId, ServiceInstances& instances) = 0;
        virtual bool checkServiceHealth() = 0;
        
        virtual bool getServiceIds(StringArray& serviceIds) = 0;
        
        virtual bool chooseNext() = 0;
        virtual bool isFirstEndpoint() const = 0;
        
        virtual bool getHealthNode(JsonNode& node) = 0;
    };

    class ServiceGovernanceFactory
    {
    public:
        typedef IServiceGovernance* (*Constructor)();
        
        ~ServiceGovernanceFactory();
        
        void registerService(const String& typeName, Constructor constructor);
        IServiceGovernance* create(const String& typeName);
        IServiceGovernance* createOrGetService(const String& typeName);
        
        void getNames(StringArray& names);
        
    public:
        static ServiceGovernanceFactory* instance();
        
    private:
        ServiceGovernanceFactory();
        
    private:
        Dictionary<String, Constructor> _constructors;
        
        Map<String, IServiceGovernance> _services;
        Mutex _servicesMutex;
        
    private:
        static ServiceGovernanceFactory* _instance;
    };
    
#define REGISTER_SERVICE(class_name) \
    class class_name##Helper { \
    public: \
        class_name##Helper() \
        { \
            ServiceGovernanceFactory* factory = ServiceGovernanceFactory::instance(); \
            factory->registerService(#class_name, class_name##Helper::creatObjFunc); \
        } \
        static IServiceGovernance* creatObjFunc() \
        { \
            return new class_name; \
        } \
    }; \
    class_name##Helper class_name##helper;
}

#endif /* ServiceGovernance_h */
