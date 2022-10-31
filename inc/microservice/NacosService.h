//
//  NacosService.h
//  common
//
//  Created by baowei on 2022/5/5.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef NacosService_h
#define NacosService_h

#include "data/ValueType.h"
#include "http/HttpContent.h"
#include "microservice/ServiceGovernance.h"

using namespace Common;

namespace Microservice
{
    class NacosService : public IServiceGovernance
    {
    public:
        NacosService();
        ~NacosService() override;
        
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
        Url createRegisterUrl();
        void createInstanceProperties(StringMap& properties, const String& serviceName = String::Empty, Port servicePort = Port::Empty);
        bool registerService(const StringMap& properties);
        bool deregisterService(const StringMap& properties);
        bool checkServiceHealth(const StringMap& properties);
        
        bool checkBeat();
        
        bool getAllServices(StringArray& serviceIds, int pageNo = 1, int pageSize = 500);
        bool getLeaderInfo(String& ip);
        
        bool login();
        void loginInner();
        
        bool send(const Url& url, const HttpMethod& method, const StringMap& properties, HttpResponse& response);
        bool send(const Url& url, const HttpMethod& method, HttpResponse& response);
        
    private:
        static bool parseMeta(const String& value, JsonNode& node);
        
        static void beatTimeUp(void* state);
        static void loginTimeUp(void* state);
        
    private:
        HttpClient _httpClient;
        Mutex _httpClientMutex;
        
        StringMap _instanceInfo;
        StringMap _instanceMInfo;
        bool _registered;
        
        Url _baseUrl;
        
        Endpoints _endpoints;
        Endpoint _current;
        
        class AccessToken
        {
        public:
            String username;
            String accessToken;
            int64_t tokenTtl;
            bool globalAdmin;
            int64_t lastRefTime;
            
            AccessToken();
            
            static bool parse(const JsonNode& node, AccessToken& accessToken);
        };
        AccessToken _accessToken;
        Timer* _loginTimer;
            
    private:
        static const char* InstanceUrl;
        static const char* InstanceBeatUrl;
        static const char* InstanceListUrl;
        static const char* MetricsUrl;
        static const char* ServiceListUrl;
        static const char* ClusterNodesUrl;
        static const char* LoginUrl;
        
        static const HttpHeaders DefaultHeaders;
        
        static const char* DefaultClusterName;
        static const char* DefaultGroupName;
    };
}

#endif /* NacosService_h */
