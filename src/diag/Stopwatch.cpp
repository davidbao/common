//
//  Stopwatch.cpp
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#include "diag/Stopwatch.h"

namespace Common
{
    Stopwatch::Stopwatch(uint32_t deadTime) : Stopwatch(String::Empty, deadTime)
    {
    }
    Stopwatch::Stopwatch(const String& info, uint32_t deadTime) : _deadTime(deadTime), _startTime(0), _endTime(0), _info(info)
    {
#ifdef DEBUG
        start(deadTime);
#endif
    }
    Stopwatch::~Stopwatch()
    {
#ifdef DEBUG
        stop();
#endif
    }
    
    void Stopwatch::reStart()
    {
#ifdef DEBUG
        start(_deadTime);
#endif
    }
    void Stopwatch::start(uint32_t deadTime)
    {
#ifdef DEBUG
        _deadTime = deadTime;
        _startTime = TickTimeout::getCurrentTickCount();
        _endTime = 0;
#endif
    }
    void Stopwatch::stop(bool showInfo)
    {
#ifdef DEBUG
        if (_endTime == 0)
        {
            _endTime = TickTimeout::getCurrentTickCount();
            if (showInfo && !_info.isNullOrEmpty())
            {
                uint32_t e = elapsedInner();
                if (e >= _deadTime)
                {
                    if (e <= 10 * 1000)	// less than 10 seconds
                    {
                        Debug::writeFormatLine("%s, elapsed: %d ms", _info.c_str(), e);
                    }
                    else
                    {
                        Debug::writeFormatLine("%s, elapsed: %.3f s", _info.c_str(), e / 1000.0f);
                    }
                }
            }
        }
#endif
    }
    void Stopwatch::setInfo(const String& info)
    {
        _info = info;
    }
    void Stopwatch::setInfo(const char* info)
    {
        _info = info;
    }
    uint32_t Stopwatch::elapsed() const
    {
        return elapsedInner(true);
    }
    TimeSpan Stopwatch::elapsed2() const
    {
        uint32_t e = elapsed();
        return TimeSpan::fromMilliseconds((double)e);
    }
    
    uint32_t Stopwatch::dateTime() const
    {
        return _deadTime;
    }
    TimeSpan Stopwatch::dateTime2() const
    {
        return TimeSpan::fromMilliseconds((double)dateTime());
    }
    
    uint32_t Stopwatch::elapsedInner(bool currentTime) const
    {
#ifdef DEBUG
        uint32_t endTime = currentTime ? TickTimeout::getCurrentTickCount() : _endTime;
        uint32_t elapsed = TickTimeout::elapsed(_startTime, endTime);
        return elapsed;
#else
		return 0;
#endif
    }
}
