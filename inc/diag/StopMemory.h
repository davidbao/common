//
//  StopMemory.h
//  common
//
//  Created by baowei on 2020/12/14.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef StopMemory_h
#define StopMemory_h

#include "thread/TickTimeout.h"
#include "data/DateTime.h"
#include "data/String.h"
#include "diag/Trace.h"
#include "Trace.h"

namespace Diag {
    class StopMemory {
    public:
        StopMemory(int64_t deadMemory = 0);

        StopMemory(const String &info, int64_t deadMemory = 0);

        ~StopMemory();

        void reStart();

        void start(int64_t deadMemory = 0);

        void stop(bool showInfo = true);

        void setInfo(const String &info);

        void setInfo(const char *info);

        int64_t used() const;

    public:
        static void logUsed();

    private:
        int64_t _deadMemory;
        int64_t _startMemory;
        int64_t _endMemory;
        String _info;
    };
}

#endif /* StopMemory_h */
