//
//  SummerApplication.h
//  common
//
//  Created by baowei on 2020/3/3.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef SummerApplication_h
#define SummerApplication_h

#include "data/String.h"
#include "data/Dictionary.h"
#include "system/Application.h"

using namespace Data;
using namespace System;

namespace Microservice {
    class SummerApplication : public Application {
    public:
        explicit SummerApplication(int argc = 0, const char *argv[] = nullptr,
                                   const TraceListenerContexts &contexts = TraceListenerContexts::Empty);

        explicit SummerApplication(const String &rootPath, int argc = 0, const char *argv[] = nullptr,
                                   const TraceListenerContexts &contexts = TraceListenerContexts::Empty);

        ~SummerApplication() override;

        static SummerApplication *instance();

    private:
        void create();

        void destroy();
    };
}

#endif // SummerApplication_h
