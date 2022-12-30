#ifndef LOCALTIME_H
#define LOCALTIME_H

#include <time.h>
#include "data/DateTime.h"
#include "data/TimeZone.h"

namespace System {
    class LocalTime {
    public:
        COMMON_ATTR_DEPRECATED("use LocalTime::setTime with DateTime & TimeZone")
        static bool setTime(const time_t &time);

        static bool setTime(const DateTime &time, const TimeZone &tz = TimeZone::Empty);

        static bool setTimeZone(const TimeZone &tz);

    private:
#if __linux__ && !__ANDROID__
        static bool hwclock(const DateTime& time);
#endif
    };
}

#endif // LOCALTIME_H
