//
//  SummerApplicationTest.cpp
//  common
//
//  Created by baowei on 2022/10/25.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "microservice/SummerApplication.h"
#include "IO/Path.h"
#include "IO/Directory.h"

using namespace IO;
using namespace Microservice;

static const char *Summer_default_ProdName = "easesuite";

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
        SummerApplication app;
        if (app.instance() != &app) {
            cleanUp(&app);
            return false;
        }

#ifdef __linux__
        const String homePath = Path::getAppPath();
#else
        const String homePath = Path::getDocumentPath(
                String::format("Documents/%s/%s", Summer_default_ProdName, app.name().c_str()));
#endif
        const String rootPath = SummerApplication::instance()->rootPath();
        if (homePath != rootPath) {
            cleanUp(&app);
            return false;
        }
        cleanUp(&app);
    }

    {
        const String homePath = Path::getDocumentPath(
                String::format("Documents/%s/%s", "test", "SummerApplicationTest"));
        SummerApplication app(homePath);
        if (app.instance() != &app) {
            cleanUp(&app);
            return false;
        }

        const String rootPath = SummerApplication::instance()->rootPath();
        if (homePath != rootPath) {
            cleanUp(&app);
            return false;
        }
        cleanUp(&app);
    }

    return true;
}

int main() {
    if(!testConstructor()) {
        return 1;
    }

    return 0;
}