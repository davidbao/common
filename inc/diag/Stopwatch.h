//
//  Stopwatch.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Stopwatch_h
#define Stopwatch_h

#include "thread/TickTimeout.h"
#include "data/DateTime.h"
#include "data/String.h"
#include "diag/Trace.h"

namespace Diag {
    class Stopwatch {
    public:
        explicit Stopwatch(uint32_t deadTime = 0);

        explicit Stopwatch(const String &info, uint32_t deadTime = 0);

        ~Stopwatch();

        void reStart();

        void start(uint32_t deadTime = 0);

        void stop(bool showInfo = true);

        void setInfo(const String &info);

        uint32_t elapsed() const;

        TimeSpan elapsed2() const;

        uint32_t dateTime() const;

        TimeSpan dateTime2() const;

    private:
        uint32_t elapsedInner(bool currentTime = false) const;

    private:
        uint32_t _deadTime;
        uint32_t _startTime;
        uint32_t _endTime;
        String _info;
    };
}

#endif // Stopwatch_h
