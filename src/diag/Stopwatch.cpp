//
//  Stopwatch.cpp
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "diag/Stopwatch.h"
#include "system/Environment.h"
#include "diag/Trace.h"

using namespace Diag;
using namespace System;

namespace Diag {
    Stopwatch::Stopwatch(uint32_t deadTime) : Stopwatch(String::Empty, deadTime) {
    }

    Stopwatch::Stopwatch(const String &info, uint32_t deadTime)
            : Stopwatch(info, TimeSpan::fromMilliseconds(deadTime)) {
    }

    Stopwatch::Stopwatch(const TimeSpan &deadTime) : Stopwatch(String::Empty, deadTime) {
    }

    Stopwatch::Stopwatch(const String &info, const TimeSpan &deadTime) : _deadTime(deadTime), _startTime(0),
                                                                         _endTime(0), _info(info) {
        start(deadTime);
    }

    Stopwatch::~Stopwatch() {
        stop();
    }

    void Stopwatch::reStart() {
        start(_deadTime);
    }

    void Stopwatch::start(uint32_t deadTime) {
        start(TimeSpan::fromMilliseconds((double) deadTime));
    }

    void Stopwatch::start(const TimeSpan &deadTime) {
        _deadTime = deadTime;
        _startTime = Environment::getTickCount();
        _endTime = 0;
    }

    void Stopwatch::stop(bool showInfo) {
        if (_endTime == 0) {
            _endTime = Environment::getTickCount();

            if (showInfo && !_info.isNullOrEmpty()) {
                uint64_t e = elapsedInner();
                if (e >= (uint64_t) _deadTime.totalMilliseconds()) {
                    if (e <= 10 * 1000) {   // less than 10 seconds
                        Debug::writeFormatLine("%s, elapsed: %d ms", _info.c_str(), e);
                    } else {
                        Debug::writeFormatLine("%s, elapsed: %.3f s", _info.c_str(), (float) e / 1000.0f);
                    }
                }
            }
        }
    }

    void Stopwatch::setInfo(const String &info) {
        _info = info;
    }

    const String &Stopwatch::info() const {
        return _info;
    }

    uint32_t Stopwatch::elapsedMilliseconds() const {
        return elapsedInner(true);
    }

    TimeSpan Stopwatch::elapsed() const {
        return TimeSpan::fromMilliseconds((double) elapsedMilliseconds());
    }

    uint32_t Stopwatch::deadTimeMilliseconds() const {
        return (uint32_t) _deadTime.totalMilliseconds();
    }

    TimeSpan Stopwatch::deadTime() const {
        return _deadTime;
    }

    bool Stopwatch::isRunning() const {
        return _endTime == 0;
    }

    uint64_t Stopwatch::elapsedInner(bool currentTime) const {
        uint64_t endTime = currentTime ? Environment::getTickCount() : _endTime;
        return endTime - _startTime;
    }
}
