//
//  ConsulService.h
//  common
//
//  Created by baowei on 2020/2/18.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef ConsulService_h
#define ConsulService_h

#include "data/String.h"
#include "http/HttpContent.h"
#include "microservice/ServiceGovernance.h"

using namespace Data;

namespace Microservice
{
    class ConsulService : public IServiceGovernance
    {
    public:
        ConsulService();
        ~ConsulService() override;
        
        const String serviceId() const;
        const String serviceName() const;
        
        const Url& baseUrl() const;
        
        bool registerService() override;
        bool deregisterService() override;
        
        bool getServiceInstances(const String& serviceId, ServiceInstances& instances) override;
        bool checkServiceHealth() override;
        
        bool getServiceIds(StringArray& serviceIds) override;
        
        bool chooseNext() override;
        bool isFirstEndpoint() const override;
        
        bool getHealthNode(JsonNode& node) override;
        
    private:
        bool getServiceById(const Url& baseUrl, const String& serviceId, String& content);
        bool getAllServices(const Url& baseUrl, String& content);
        
        Url createRegisterUrl();
        String createPayload(const String& serviceName = String::Empty, const String& serviceId = String::Empty, Port servicePort = Port::Empty);
        
    private:
        static bool parseInstanceJson(const String& content, ServiceInstances& instances);
        static bool parseCheckJson(const String& content, ServiceInstances* instances);
        
    private:
        HttpClient _httpClient;
        
    private:
        static const char* DeregisterUrl;
        static const char* RegisterUrl;
        static const char* GetServiceUrl;
        static const char* GetAllServicesUrl;
        static const char* GetServicesUrl;
        static const char* CheckServiceHealthUrl;
        static const char* GetLeaderUrl;
        
        static const HttpHeaders DefaultHeaders;
        
        String _serviceId;
        String _serviceName;
        
        Url _baseUrl;
        
        Endpoints _endpoints;
        Endpoint _current;
    };
}

#endif /* ConsulService_h */
