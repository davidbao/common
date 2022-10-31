//
//  WebStarter.cpp
//  common
//
//  Created by baowei on 2021/1/20.
//  Copyright © 2021 com. All rights reserved.
//

#ifdef __EMSCRIPTEN__
#include "system/WebStarter.h"
#include "diag/Trace.h"
#include "system/WebApplication.h"
#include "configuration/ConfigService.h"

namespace Common
{
    WebStarter::WebStarter(const String& name, int argc, const char * argv[])
    {
        _app = new WebApplication(name, argc, argv);
        _app->exitDelegates()->add(this, exited);
        _config = new ConfigService();
        _starter = nullptr;

        initialize();
    }
    WebStarter::~WebStarter()
    {
    	if(_starter != nullptr)
    	{
    		delete _starter;
    		_starter = nullptr;
    	}
        delete _config;
        _app->exitDelegates()->remove(this, exited);
        delete _app;
    }

    bool WebStarter::initialize()
    {
        _config->initialize();
        return true;
    }
    bool WebStarter::unInitialize()
    {
    	if(_starter != nullptr)
		{
    		_starter->unInitialize();
		}
        _config->unInitialize();
        return true;
    }

    void WebStarter::exited(void* owner, void* sender, EventArgs* args)
    {
    	WebStarter* starter = static_cast<WebStarter*>(owner);
    	assert(starter);
    	starter->unInitialize();
    	delete starter;
    }
}
#endif // __EMSCRIPTEN__
