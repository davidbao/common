//
//  Stopwatch.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Stopwatch_h
#define Stopwatch_h

#include "data/TimeSpan.h"
#include "data/String.h"

using namespace Data;

namespace Diag {
    class Stopwatch {
    public:
        explicit Stopwatch(uint32_t deadTime);

        Stopwatch(const String &info, uint32_t deadTime);

        explicit Stopwatch(const TimeSpan &deadTime = TimeSpan::Zero);

        explicit Stopwatch(const String &info, const TimeSpan &deadTime = TimeSpan::Zero);

        ~Stopwatch();

        void reStart();

        void start(uint32_t deadTime);

        void start(const TimeSpan &deadTime = TimeSpan::Zero);

        void stop(bool showInfo = true);

        void setInfo(const String &info);

        const String &info() const;

        uint32_t elapsedMilliseconds() const;

        TimeSpan elapsed() const;

        uint32_t deadTimeMilliseconds() const;

        TimeSpan deadTime() const;

        bool isRunning() const;

    private:
        uint64_t elapsedInner(bool currentTime = false) const;

    private:
        TimeSpan _deadTime;
        uint64_t _startTime;
        uint64_t _endTime;
        String _info;
    };
}

#endif // Stopwatch_h
