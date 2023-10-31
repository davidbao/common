//
//  SummerStarterTest.cpp
//  common
//
//  Created by baowei on 2023/10/30.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "microservice/SummerStarter.h"
#include "IO/Path.h"
#include "IO/Directory.h"

using namespace IO;
using namespace Microservice;

void cleanUp(const Application * app) {
    String rootPath = app->rootPath();

    const String appPath = Path::getAppPath();
    if (appPath != rootPath) {
        if (Directory::exists(rootPath)) {
            Directory::deleteDirectory(rootPath);
        }
    }
}

bool testConstructor() {
    {
        SummerStarter starter;
        int result = starter.run();
        cleanUp(Application::instance());
        if (result != 0) {
            return false;
        }
    }

    {
        String rootPath = Path::getTempPath();
        SummerStarter starter(rootPath);
        int result = starter.run();
        if (Directory::exists(rootPath)) {
            Directory::deleteDirectory(rootPath);
        }
        if (result != 0) {
            return false;
        }
    }

    {
        auto runFunc = [] {
            Thread::msleep(200);
            Application::instance()->exit(1);
        };
        Thread test = Thread("test_thread", runFunc);

        SummerStarter starter;
        test.start();
        int result = starter.runLoop();
        cleanUp(Application::instance());
        if (result != 1) {
            return false;
        }
    }

    {
        auto runFunc = [] {
            Thread::msleep(200);
            SummerStarter::instance()->reInitialize();
            Thread::msleep(200);
            Application::instance()->exit(1);
        };
        Thread test = Thread("test_thread", runFunc);

        SummerStarter starter;
        test.start();
        int result = starter.runLoop();
        cleanUp(Application::instance());
        if (result != 1) {
            return false;
        }
    }

    return true;
}

int main() {
    if(!testConstructor()) {
        return 1;
    }

    return 0;
}