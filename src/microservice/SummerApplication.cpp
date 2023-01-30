//
//  SummerApplication.cpp
//  common
//
//  Created by baowei on 2020/3/3.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/SummerApplication.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "diag/FileTraceListener.h"

using namespace Data;

namespace Microservice {
    static const char* __Summer_default_ProdName = "easesuite";
    static String __Summer_homePath;
    static TraceListenerContexts __Summer_traceContexts;

    static struct InitializeSummerApplication {
        InitializeSummerApplication() {
            String appFilePath = Application::startupPath();
            String appFileName = Path::getFileName(appFilePath);

            String appName;
            int pos = appFileName.find('.');
            if (pos > 0)
                appName = appFileName.substr(0, pos);
            else
                appName = appFileName;

#if __linux__
            const String homePath = Path::getAppPath();
#else
            const String homePath = Path::getDocumentPath(
                    String::format("Documents/%s/%s", __Summer_default_ProdName, appName.c_str()));
#endif
            __Summer_homePath = homePath;

            __Summer_traceContexts.add(new FileTraceListenerContext(Path::combine(homePath, "logs")));
        }
    } InitializeSummerApplication;

    SummerApplication::SummerApplication(int argc, const char *argv[]) : Application(argc, argv, __Summer_homePath,
                                                                                     __Summer_traceContexts) {
        if (_traceListeners.count() > 0) {
            Trace::writeLine(String::format("%s is starting.", name().c_str()), Trace::Info);
        }
    }

    SummerApplication::~SummerApplication() {
        if (_traceListeners.count() > 0) {
            Trace::writeLine(String::format("%s is stopping.", name().c_str()), Trace::Info);
        }
    }

    SummerApplication *SummerApplication::instance() {
        return dynamic_cast<SummerApplication *>(Application::instance());
    }
}
