//
//  SummerStarter.h
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef SummerStarter_h
#define SummerStarter_h

#include "data/String.h"
#include "thread/Thread.h"
#include "diag/Trace.h"
#include "microservice/SummerApplication.h"

using namespace Data;

namespace Config {
    class ConfigService;
}
using namespace Config;

namespace Microservice {
    class SummerApplication;

    class ServiceRegister;

    class ServiceDiscovery;

    class RibbonLoadBalancerClient;

    class Actuator;

    class HttpService;

    class TcpService;

    class DataSourceService;

    class ClusterService;

    class NotificationService;

    class SummerStarter {
    public:
        typedef void (*ProcessAction)();

        SummerStarter(int argc = 0, const char *argv[] = nullptr,
                      const String &rootPath = String::Empty,
                      const TraceListenerContexts &contexts = TraceListenerContexts::Empty);

        virtual ~SummerStarter();

        void add(const String &key, ProcessAction action);

        int runLoop();

        template<class T>
        int runLoop() {
            T t;
            if (t.initialize()) {
                runLoopInner();
            }
            t.unInitialize();

            unInitialize();

            return _app->exitCode();
        }

        template<class T>
        int run() {
            T t;
            if (t.initialize()) {
                t.unInitialize();
            }
            unInitialize();

            return _app->exitCode();
        }

        void unInitialize();

    private:
        void initialize();

        void runLoopInner();

        void runInConsole();

    private:
        SummerApplication *_app;
        ConfigService *_config;
        ServiceRegister *_register;
        ServiceDiscovery *_discovery;
        RibbonLoadBalancerClient *_lbc;
        Actuator *_actuator;
        HttpService *_http;
        TcpService *_tcp;
        DataSourceService *_dataSource;
        ClusterService *_cluster;
        NotificationService *_notification;

        Dictionary<String, ProcessAction> _actions;
    };
}

#endif // SummerStarter_h
