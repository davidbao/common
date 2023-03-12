//
//  WebStarter.h
//  Common
//
//  Created by baowei on 2021/1/20.
//  Copyright (c) 2021 com. All rights reserved.
//

#ifndef WebStarter_h
#define WebStarter_h

#include "system/ServiceFactory.h"
#include "system/WebApplication.h"

namespace Config {
    class ConfigService;
}
using namespace Config;

namespace System {
    class IStarter : public IService {
    public:
        virtual bool initialize() = 0;

        virtual bool unInitialize() = 0;
    };

    class WebStarter : public IStarter {
    public:
        explicit WebStarter(const String &name, int argc = 0, const char *argv[] = nullptr);

        ~WebStarter() override;

        template<class T>
        int runLoop() {
            T *t = new T();
            if (t->initialize()) {
                _starter = t;

                _app->runLoop();
                return 0;
            } else {
                t->unInitialize();
                delete t;

                unInitialize();

                return -1;
            }
        }

    private:
        bool initialize() override;

        bool unInitialize() override;

    private:
        static void exited(void *owner, void *sender, EventArgs *args);

    private:
        WebApplication *_app;
        ConfigService *_config;

        IStarter *_starter;
    };
}

#endif /* WebStarter_h */
