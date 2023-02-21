//
//  SummerStarter.cpp
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/SummerStarter.h"
#include "system/Console.h"
#include "microservice/SummerApplication.h"
#include "configuration/ConfigService.h"
#include "microservice/ServiceRegister.h"
#include "microservice/ServiceDiscovery.h"
#include "microservice/LoadBalancerClient.h"
#include "microservice/Actuator.h"
#include "microservice/HttpService.h"
#include "microservice/DataSourceService.h"
#include "microservice/ClusterService.h"
#include "microservice/TcpService.h"

#ifdef HAS_NOTIFICATION
#include "microservice/NotificationService.h"
#endif

namespace Microservice {
    SummerStarter::SummerStarter(int argc, const char *argv[]) {
        _app = new SummerApplication(argc, argv);
        _config = new ConfigService();
        _register = new ServiceRegister();
        _discovery = new ServiceDiscovery();
        _lbc = new RibbonLoadBalancerClient();
        _actuator = new Actuator();
        _http = new HttpService();
        _tcp = new TcpService();
        _dataSource = new DataSourceService();
        _cluster = new ClusterService();
#ifdef HAS_NOTIFICATION
        _notification = new NotificationService();
#endif

        initialize();
    }

    SummerStarter::~SummerStarter() {
        delete _http;
        delete _tcp;
        delete _actuator;
        delete _lbc;
        delete _discovery;
        delete _register;
        delete _config;
        delete _dataSource;
        delete _cluster;
#ifdef HAS_NOTIFICATION
        delete _notification;
#endif
        delete _app;
    }

    void SummerStarter::add(const String &key, action_callback action) {
        _actions.add(key.toLower(), action);
    }

    int SummerStarter::runLoop() {
        runLoopInner();

        unInitialize();

        return _app->exitCode();
    }

    void SummerStarter::runLoopInner() {
        if (_app->isExiting())
            return;

        if (_app->withQuit())
            runInConsole();
        else
            _app->runLoop();
    }

    void SummerStarter::runInConsole() {
        bool quit = false;
        do {
            printf("Please type 'q' to exit this application.\r\n");

            String line = Console::readLine();
            String command = line.trim('\r', '\n');
            if (String::equals("q", command, true))
                quit = true;
            else {
                action_callback action = nullptr;
                if (_actions.at(command.toLower(), action)) {
                    action();
                }
            }
        } while (!quit);
    }

    void SummerStarter::initialize() {
        _config->initialize();
        _dataSource->initialize();
        _register->initialize();
        _discovery->initialize();
        _lbc->initialize();
        _http->initialize();
        _tcp->initialize();
        _actuator->initialize();
        _cluster->initialize();
#ifdef HAS_NOTIFICATION
        _notification->initialize();
#endif
    }

    void SummerStarter::unInitialize() {
#ifdef HAS_NOTIFICATION
        _notification->unInitialize();
#endif
        _cluster->unInitialize();
        _lbc->unInitialize();
        _discovery->unInitialize();
        _register->unInitialize();
        _actuator->unInitialize();
        _http->unInitialize();
        _tcp->unInitialize();
        _dataSource->unInitialize();
        _config->unInitialize();
    }
}
