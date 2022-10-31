//
//  Stopwatch.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef Stopwatch_h
#define Stopwatch_h

#include "thread/TickTimeout.h"
#include "data/DateTime.h"
#include "data/ValueType.h"
#include "diag/Trace.h"
#include "Trace.h"

namespace Common
{
	class Stopwatch
	{
	public:
        Stopwatch(uint deadTime = 0);
        Stopwatch(const String& info, uint deadTime = 0);
        ~Stopwatch();

        void reStart();
        void start(uint deadTime = 0);
        void stop(bool showInfo = true);
        void setInfo(const String& info);
        void setInfo(const char* info);
        uint elapsed() const;
        TimeSpan elapsed2() const;
        uint dateTime() const;
        TimeSpan dateTime2() const;

	private:
        uint elapsedInner(bool currentTime = false) const;

	private:
		uint _deadTime;
		uint _startTime;
		uint _endTime;
        String _info;
	};
}

#endif // Stopwatch_h
