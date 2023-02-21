//
//  WebApplication.h
//  common
//
//  Created by baowei on 2020/5/1.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef WebApplication_h
#define WebApplication_h

#ifdef __EMSCRIPTEN__

#include "data/String.h"
#include "net/NetType.h"
#include "data/Dictionary.h"
#include "system/Application.h"

#include <emscripten/bind.h>

using namespace Net;

namespace System {
    struct InitializeWebApplication;

    class WebApplication : public Application {
    public:
        typedef StringMap Arguments;

        WebApplication(const String &name, int argc = 0, const char *argv[] = nullptr);

        ~WebApplication() override;

        static WebApplication *instance();

        bool useSSL() const;

        const String &urlStr() const;

        const Url url() const;

        const Arguments &arguments() const;

        void runLoop() override;

        void exit(int code = 0) override;

        String name() const override;

        Delegates *exitDelegates();

    protected:
        void runLoop(loop_callback callback) override;

    private:
        friend void __set_url(emscripten::val val);

        static void proc();

    private:
        friend InitializeWebApplication;

        Arguments _arguments;

        String _name;

        Delegates _exitDelegates;

    private:
        static String _homePath;
        static TraceListenerContexts _traceContexts;

        static String _url;
    };
}
#endif

#endif // WebApplication_h
