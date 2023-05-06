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
        SummerApplication(int argc = 0, const char *argv[] = nullptr,
                          const String &rootPath = String::Empty,
                          const TraceListenerContexts &contexts = TraceListenerContexts::Empty);

        ~SummerApplication() override;

        static SummerApplication *instance();
    };
}

#endif // SummerApplication_h
