//
//  Actuator.h
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef Actuator_h
#define Actuator_h

#include "data/String.h"
#include "microservice/HttpService.h"

using namespace Data;

namespace Microservice
{
    class IActuator
    {
    public:
        IActuator();
        virtual ~IActuator();
        
    protected:
        HttpStatus onActuator(const HttpRequest& request, HttpResponse& response);
        HttpStatus onInfo(const HttpRequest& request, HttpResponse& response);
        HttpStatus onHealth(const HttpRequest& request, HttpResponse& response);
        HttpStatus onMetrics(const HttpRequest& request, HttpResponse& response);
        HttpStatus onProcessUptime(const HttpRequest& request, HttpResponse& response);
        HttpStatus onSystemCPUCount(const HttpRequest& request, HttpResponse& response);
        HttpStatus onProcessCPUUsage(const HttpRequest& request, HttpResponse& response);
        HttpStatus onSystemCPUUsage(const HttpRequest& request, HttpResponse& response);
        HttpStatus onThreadsLive(const HttpRequest& request, HttpResponse& response);
        HttpStatus onThreadsPeak(const HttpRequest& request, HttpResponse& response);
        HttpStatus onThreadsDaemon(const HttpRequest& request, HttpResponse& response);
        HttpStatus onMemoryMax(const HttpRequest& request, HttpResponse& response);
        HttpStatus onMemoryUsed(const HttpRequest& request, HttpResponse& response);
        HttpStatus onMemoryCommitted(const HttpRequest& request, HttpResponse& response);
        HttpStatus onEnvironment(const HttpRequest& request, HttpResponse& response);
        HttpStatus onProcessId(const HttpRequest& request, HttpResponse& response);
    };

    class Actuator : public IHttpAction, public IActuator
    {
    public:
        Actuator();
        ~Actuator() override;
        
        bool initialize();
        bool unInitialize();
        
        HttpStatus onAction(const HttpRequest& request, HttpResponse& response) override;
        
    private:
        static HttpStatus onAction(void* parameter, const HttpRequest& request, HttpResponse& response);

    private:
        HttpServer _httpServer;
        
    private:
        static const HttpHeader ContentTypeHeader;
    };
}

#endif /* Actuator_h */
