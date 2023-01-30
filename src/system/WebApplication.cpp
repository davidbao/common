//
//  WebApplication.cpp
//  common
//
//  Created by baowei on 2020/5/1.
//  Copyright © 2020 com. All rights reserved.
//

#ifdef __EMSCRIPTEN__

#include "system/WebApplication.h"
#include "system/Regex.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "diag/MemoryTraceListener.h"
#include "diag/Trace.h"
#include "system/ServiceFactory.h"

#include <SDL/SDL.h>
#include <emscripten/emscripten.h>

namespace System {
    String WebApplication::_homePath = "/";
    TraceListenerContexts WebApplication::_traceContexts;

    String WebApplication::_url;

    static struct InitializeWebApplication {
        InitializeWebApplication() {
            WebApplication::_traceContexts.add(new MemoryTraceListenerContext());
        }
    } InitializeWebApplication;

//    static void __set_url_parameters(emscripten::val val) {
//    //	printf("set_url_parameters: %s\n", val.as<std::string>().c_str());
//    }
    void __set_url(emscripten::val val) {
        //	printf("set_url: %s\n", val.as<std::string>().c_str());
        String url = val.as<std::string>();

        WebApplication *app = WebApplication::instance();
        assert(app);

        Regex reg("([^&?]\\w+)=([^&]*)");
        reg.match(url, app->_arguments);

        WebApplication::_url = url;
    }

    EMSCRIPTEN_BINDINGS(URL) {
//    	emscripten::function("set_url_parameters", &__set_url_parameters);
        emscripten::function("set_url", &__set_url);
    }

    WebApplication::WebApplication(const String &name, int argc, const char *argv[]) : Application(argc, argv,
                                                                                                   WebApplication::_homePath,
                                                                                                   WebApplication::_traceContexts),
                                                                                       _name(name), _arguments(true) {
        Trace::writeLine(String::format("%s is starting.", this->name().c_str()), Trace::Info);

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

        EM_ASM({
                   Module['set_url'](decodeURIComponent(window.location.toString()));
               });

        String culture("zh-CN");    // default is zh-CN
        _arguments.at("culture", culture);
        setCulture(culture);
    }

    WebApplication::~WebApplication() {
        Trace::writeLine(String::format("%s is stopping.", name().c_str()), Trace::Info);
    }

    WebApplication *WebApplication::instance() {
        return dynamic_cast<WebApplication *>(Application::instance());
    }

    void WebApplication::runLoop(loop_callback callback) {
        emscripten_set_main_loop(callback, 0, 0);
    }

    String WebApplication::name() const {
        return _name;
    }

    const String &WebApplication::urlStr() const {
        return _url;
    }

    const Url WebApplication::url() const {
        Url url;
        Url::parse(_url, url);
        return url;
    }

    const WebApplication::Arguments &WebApplication::arguments() const {
        return _arguments;
    }

    void WebApplication::runLoop() {
        runLoop(proc);
    }

    void WebApplication::exit(int code) {
        Application::exit(code);

        SDL_Quit();
    }

    Delegates *WebApplication::exitDelegates() {
        return &_exitDelegates;
    }

    void WebApplication::proc() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    WebApplication *app = WebApplication::instance();
                    assert(app);
                    EventArgs args;
                    app->_exitDelegates.invoke(app, &args);
                    break;
                }
            }
        }
    }
}
#endif // __EMSCRIPTEN__
