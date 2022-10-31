//
//  ServiceRegister.h
//  common
//
//  Created by baowei on 2020/2/26.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef ServiceRegister_h
#define ServiceRegister_h

#include "data/Dictionary.h"
#include "thread/Timer.h"
#include "microservice/ServiceGovernance.h"

using namespace Common;

namespace Microservice
{
    class IServiceRegister : public IService
    {
    public:
        virtual bool getServiceIds(StringArray& serviceIds) = 0;
        virtual bool getHealthNode(JsonNode& node) const = 0;
    };

    class ServiceRegister : public IServiceRegister
    {
    public:
        ServiceRegister();
        ~ServiceRegister() override;
        
        bool initialize();
        bool unInitialize();
        
        bool getServiceIds(StringArray& serviceIds) override;
        bool getHealthNode(JsonNode& node) const override;
        
    private:
        static void registerCallback(void* parameter);
        void registerProcInner();
        
    private:
        bool _registered;
        Timer* _registerTimer;
        
        String _name;
        IServiceGovernance* _service;
    };
}

#endif /* ServiceRegister_h */
