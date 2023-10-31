//
//  SummerApplication.cpp
//  common
//
//  Created by baowei on 2020/3/3.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/SummerApplication.h"
#include "IO/Path.h"
#include "diag/FileTraceListener.h"

using namespace Data;

namespace Microservice {
    static const char *Summer_default_ProdName = "easesuite";
    static String Summer_rootPath;
    static TraceListenerContexts Summer_traceContexts(true, 32);

    static struct InitializeSummerApplication {
        InitializeSummerApplication() {
            String appFilePath = Application::startupPath();
            String appFileName = Path::getFileName(appFilePath);

            String appName;
            ssize_t pos = appFileName.find('.');
            if (pos > 0)
                appName = appFileName.substr(0, pos);
            else
                appName = appFileName;

#ifdef __linux__
            const String rootPath = Path::getAppPath();
#else
            const String rootPath = Path::getDocumentPath(
                    String::format("Documents/%s/%s", Summer_default_ProdName, appName.c_str()));
#endif
            Summer_rootPath = rootPath;

            Summer_traceContexts.add(new FileTraceListenerContext(Path::combine(rootPath, "logs")));
        }
    } InitializeSummerApplication;

    SummerApplication::SummerApplication(int argc, const char *argv[], const TraceListenerContexts &contexts) :
            Application(argc, argv, Summer_rootPath, !contexts.isEmpty() ? contexts : Summer_traceContexts) {
        create();
    }

    SummerApplication::SummerApplication(const String &rootPath, int argc, const char *argv[],
                                         const TraceListenerContexts &contexts) :
            Application(argc, argv,
                        !rootPath.isNullOrEmpty() ? rootPath : Summer_rootPath,
                        !contexts.isEmpty() ? contexts : Summer_traceContexts) {
        create();
    }

    SummerApplication::~SummerApplication() {
        destroy();
    }

    SummerApplication *SummerApplication::instance() {
        return dynamic_cast<SummerApplication *>(Application::instance());
    }

    void SummerApplication::create() {
#ifdef WIN32
        Trace::enableConsoleOutput();
        Trace::enableFlushConsoleOutput();
#endif

        if (_traceListeners.count() > 0) {
            Trace::writeLine(String::format("%s is starting.", Application::name().c_str()), Trace::Info);
        }
    }

    void SummerApplication::destroy() {
        if (_traceListeners.count() > 0) {
            Trace::writeLine(String::format("%s is stopping.", Application::name().c_str()), Trace::Info);
        }
    }
}
