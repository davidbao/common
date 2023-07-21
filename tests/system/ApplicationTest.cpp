//
//  ApplicationTest.cpp
//  common
//
//  Created by baowei on 2023/7/20.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/Application.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/FileStream.h"
#include "diag/Process.h"

using namespace System;
using namespace IO;
using namespace Diag;

void createFlagName() {
    String fileName = Path::combine(Path::getAppPath(), "ApplicationTest_flag.txt");
    FileStream fs(fileName, FileMode::FileAppend, FileAccess::FileWrite);
    fs.writeText("abc123");
    fs.close();
}

bool testConstructor() {
    {
        Application app;
        if (app.name() != "ApplicationTest") {
            return false;
        }
        if (app.rootPath() != Path::getAppPath()) {
            return false;
        }
        if (app.culture() != Culture("en")) {
            return false;
        }
        if (!app.logPath().isNullOrEmpty()) {
            return false;
        }
        if (!app.logFileFilter().isNullOrEmpty()) {
            return false;
        }
        if (Application::instance() != &app) {
            return false;
        }
    }

    {
        int argc = 1;
        const char *argv[] = {"ApplicationTest"};
        Application app(argc, argv);
        if (app.argv().count() != 1) {
            return false;
        }
        if (app.argv()[0] != "ApplicationTest") {
            return false;
        }
    }
    {
        int argc = 2;
        const char *argv[] = {"ApplicationTest", "-h"};
        Application app(argc, argv);
        if (app.argv().count() != 2) {
            return false;
        }
        if (app.argv()[0] != "ApplicationTest") {
            return false;
        }
        if (app.argv()[1] != "-h") {
            return false;
        }
    }

    {
        Process::start(Application::startupPath(), "-s &");
        Process::start(Application::startupPath(), "-s &");

#ifdef __arm_linux__
        Thread::sleep(2000);
#else
        Thread::sleep(200);
#endif
        String fileName = Path::combine(Path::getAppPath(), "ApplicationTest_flag.txt");
        String content = File::readAllText(fileName);
        if (content != "abc123") {
            File::deleteFile(fileName);
            return false;
        }
        File::deleteFile(fileName);
    }

    return true;
}

void setUp() {
    String fileName = Path::combine(Path::getAppPath(), "ApplicationTest_flag.txt");
    if (File::exists(fileName)) {
        File::deleteFile(fileName);
    }
}

int main(int argc, const char *argv[]) {
    Application app(argc, argv);
    const Application::Arguments &arguments = app.arguments();
    if(arguments.contains("single") || arguments.contains("s")) {
        Application app2(argc, argv, String::Empty, TraceListenerContexts::Empty, "single_app");
//        if (ProcessMutex::exists("single_app")) {
//            exit(0);
//        }

        createFlagName();
        Thread::sleep(5 * 1000);
    } else {
        setUp();

        if (!testConstructor()) {
            return 1;
        }
    }

    return 0;
}