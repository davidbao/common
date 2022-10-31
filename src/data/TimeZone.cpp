#include <math.h>
#include<stdlib.h>
#include "data/TimeZone.h"
#include "system/LocalTime.h"
#include "exception/Exception.h"
#include "data/StringArray.h"
#include "system/BCDUtilities.h"
#include "diag/Trace.h"
#ifdef WIN32
#include <Windows.h>
#endif

namespace Common
{
    TimeZone TimeZone::Local = TimeZone();
    TimeZone TimeZone::Empty = TimeZone();
    const String TimeZone::FileNames[] =
    {
        "Etc/GMT+12", "Etc/GMT+11", "Etc/GMT+10", "Etc/GMT+9", "Etc/GMT+8", "Etc/GMT+7", "Etc/GMT+6",
        "Etc/GMT+5", "Etc/GMT+4", "Canada/Newfoundland", "Etc/GMT+3", "Etc/GMT+2", "Etc/GMT+1",
        "Europe/London",
        "Europe/Amsterdam", "Africa/Cairo", "Europe/Moscow", "Asia/Tehran", "Asia/Dubai", "Asia/Kabul", "Asia/Karachi", "Asia/Calcutta", "Asia/Kathmandu", "Asia/Dhaka", "Asia/Yangon", "Asia/Bangkok",
        "Asia/Shanghai", "Asia/Tokyo", "Australia/Adelaide", "Australia/Sydney", "Asia/Magadan", "Pacific/Auckland", "Pacific/Tongatapu"
    };
//    const String TimeZone::FileNames[] =
//    {
//        "Etc/GMT+12", "Etc/GMT+11", "Etc/GMT+10", "Etc/GMT+9", "Etc/GMT+8", "Etc/GMT+7", "Etc/GMT+6",
//        "Etc/GMT+5", "Etc/GMT+4", "Canada/Newfoundland", "Etc/GMT+3", "Etc/GMT+2", "Etc/GMT+1",
//        "Etc/GMT",
//        "Etc/GMT-1", "Etc/GMT-2", "Etc/GMT-3", "Iran", "Etc/GMT-4", "Asia/Kabul", "Etc/GMT-5", "Asia/Calcutta", "Asia/Kathmandu", "Etc/GMT-6", "Asia/Yangon", "Etc/GMT-7",
//        "Asia/Shanghai", "Etc/GMT-9", "Australia/Adelaide", "Etc/GMT-10", "Etc/GMT-11", "Etc/GMT-12", "Etc/GMT-13"
//    };

    const int64_t TimeZone::Offsets[] =
    {
        12*TicksPerHour, 11*TicksPerHour, 10*TicksPerHour, 9*TicksPerHour, 8*TicksPerHour, 7*TicksPerHour, 6*TicksPerHour,
        5*TicksPerHour, 4*TicksPerHour, 33*TicksPerHour/10, 3*TicksPerHour, 2*TicksPerHour, 1*TicksPerHour,
        0,
        -1*TicksPerHour, -2*TicksPerHour, -3*TicksPerHour, -33*TicksPerHour/10, -4*TicksPerHour, -43*TicksPerHour/10, -5*TicksPerHour, -53*TicksPerHour/10, -545*TicksPerHour/100, -6*TicksPerHour, -63*TicksPerHour/10, -7*TicksPerHour,
        -8*TicksPerHour, -9*TicksPerHour, -93*TicksPerHour/10, -10*TicksPerHour, -11*TicksPerHour, -12*TicksPerHour, -13*TicksPerHour
    };
    TimeZone::TimeZone(int64_t offset)
    {
        if(this == &Local)
        {
            update();
        }
        else
        {
            _ticksOffset = offset;
        }
    }
    TimeZone::TimeZone(Type type)
    {
        _ticksOffset = Offsets[type];
    }
    TimeZone::TimeZone(const TimeZone& timeZone)
    {
    	_ticksOffset = timeZone._ticksOffset;
    }
    
    void TimeZone::update()
    {
#if WIN32
        TIME_ZONE_INFORMATION tz;
        GetTimeZoneInformation(&tz);
        _ticksOffset = TicksPerSecond * tz.Bias * 60;
#else
        time_t currtime;
        struct tm * timeinfo;
        time ( &currtime );
        timeinfo = gmtime ( &currtime );
        time_t utc = mktime( timeinfo );
        timeinfo = localtime ( &currtime );
        time_t local = mktime( timeinfo );
        // Get offset in hours from UTC
        
        _ticksOffset = difftime(utc, local) * TicksPerSecond;
#endif
    }
    void TimeZone::updateLocal(const TimeZone& tz)
    {
        TimeZone::Local.update();
        
        // fixed bug.
        if(TimeZone::Local._ticksOffset != tz._ticksOffset)
        {
            TimeZone::Local._ticksOffset = tz._ticksOffset;
        }
    }
    
    DateTime TimeZone::toUtcTime(DateTime time) const
    {
        if(time.kind() == DateTime::Kind::Utc)
            return time;
        return DateTime(time.ticks() + _ticksOffset, DateTime::Utc);
    }
    DateTime TimeZone::toLocalTime(DateTime time) const
    {
        if(time.kind() == DateTime::Kind::Local)
            return time;
        return DateTime(time.ticks() - _ticksOffset, DateTime::Local);
    }
    
    void TimeZone::operator=(const TimeZone& tz)
    {
        this->_ticksOffset = tz._ticksOffset;
    }
    bool TimeZone::operator==(const TimeZone& tz) const
    {
        return this->_ticksOffset == tz._ticksOffset;
    }
    bool TimeZone::operator!=(const TimeZone& tz) const
    {
        return !operator==(tz);
    }
    
    void TimeZone::write(Stream* stream, bool bigEndian) const
    {
        stream->writeInt64(_ticksOffset, bigEndian);
    }
    void TimeZone::read(Stream* stream, bool bigEndian)
    {
        _ticksOffset = stream->readInt64(bigEndian);
    }
    void TimeZone::writeSeconds(Stream* stream, bool bigEndian) const
    {
        stream->writeInt32((int)(_ticksOffset * SecondsPerTick), bigEndian);
    }
    void TimeZone::readSeconds(Stream* stream, bool bigEndian)
    {
        _ticksOffset = stream->readInt32(bigEndian) * TicksPerSecond;
    }
    
    int TimeZone::localTimeValue() const
    {
#if WIN32
        return (int)(_ticksOffset / TicksPerSecond / 60);
#else
        return (int)(_ticksOffset / TicksPerMinute);
#endif
    }
    int TimeZone::hourOffset() const
    {
        return (int)(_ticksOffset / TicksPerHour);
    }
    
    const String TimeZone::toString() const
    {
        if(isEmpty())
            return String::Empty;
        return toTypeStr(type());
//        return String::convert("STD%d", hourOffset());
    }
    bool TimeZone::isEmpty() const
    {
        return _ticksOffset == -1;
    }
    int64_t TimeZone::ticksOffset() const
    {
        return _ticksOffset;
    }
    
    bool TimeZone::parseTypeStr(const String& str, Type& type)
    {
        if(String::equals(str, "GMT-12:00", true))
        {
            type = GMT_B1200;
            return true;
        }
        else if(String::equals(str, "GMT-11:00", true))
        {
            type = GMT_B1100;
            return true;
        }
        else if(String::equals(str, "GMT-10:00", true))
        {
            type = GMT_B1000;
            return true;
        }
        else if(String::equals(str, "GMT-09:00", true))
        {
            type = GMT_B0900;
            return true;
        }
        else if(String::equals(str, "GMT-08:00", true))
        {
            type = GMT_B0800;
            return true;
        }
        else if(String::equals(str, "GMT-07:00", true))
        {
            type = GMT_B0700;
            return true;
        }
        else if(String::equals(str, "GMT-06:00", true))
        {
            type = GMT_B0600;
            return true;
        }
        else if(String::equals(str, "GMT-05:00", true))
        {
            type = GMT_B0500;
            return true;
        }
        else if(String::equals(str, "GMT-04:00", true))
        {
            type = GMT_B0400;
            return true;
        }
        else if(String::equals(str, "GMT-03:30", true))
        {
            type = GMT_B0330;
            return true;
        }
        else if(String::equals(str, "GMT-03:00", true))
        {
            type = GMT_B0300;
            return true;
        }
        else if(String::equals(str, "GMT-02:00", true))
        {
            type = GMT_B0200;
            return true;
        }
        else if(String::equals(str, "GMT-01:00", true))
        {
            type = GMT_B0100;
            return true;
        }
        else if(String::equals(str, "GMT", true))
        {
            type = GMT;
            return true;
        }
        else if(String::equals(str, "GMT+01:00", true))
        {
            type = GMT_A0100;
            return true;
        }
        else if(String::equals(str, "GMT+02:00", true))
        {
            type = GMT_A0200;
            return true;
        }
        else if(String::equals(str, "GMT+03:00", true))
        {
            type = GMT_A0300;
            return true;
        }
        else if(String::equals(str, "GMT+03:30", true))
        {
            type = GMT_A0330;
            return true;
        }
        else if(String::equals(str, "GMT+04:00", true))
        {
            type = GMT_A0400;
            return true;
        }
        else if(String::equals(str, "GMT+04:30", true))
        {
            type = GMT_A0430;
            return true;
        }
        else if(String::equals(str, "GMT+05:00", true))
        {
            type = GMT_A0500;
            return true;
        }
        else if(String::equals(str, "GMT+05:30", true))
        {
            type = GMT_A0530;
            return true;
        }
        else if(String::equals(str, "GMT+05:45", true))
        {
            type = GMT_A0545;
            return true;
        }
        else if(String::equals(str, "GMT+06:00", true))
        {
            type = GMT_A0600;
            return true;
        }
        else if(String::equals(str, "GMT+06:30", true))
        {
            type = GMT_A0630;
            return true;
        }
        else if(String::equals(str, "GMT+07:00", true))
        {
            type = GMT_A0700;
            return true;
        }
        else if(String::equals(str, "GMT+08:00", true))
        {
            type = GMT_A0800;
            return true;
        }
        else if(String::equals(str, "GMT+09:00", true))
        {
            type = GMT_A0900;
            return true;
        }
        else if(String::equals(str, "GMT+09:30", true))
        {
            type = GMT_A0930;
            return true;
        }
        else if(String::equals(str, "GMT+10:00", true))
        {
            type = GMT_A1000;
            return true;
        }
        else if(String::equals(str, "GMT+11:00", true))
        {
            type = GMT_A1100;
            return true;
        }
        else if(String::equals(str, "GMT+12:00", true))
        {
            type = GMT_A1200;
            return true;
        }
        else if(String::equals(str, "GMT+13:00", true))
        {
            type = GMT_A1300;
            return true;
        }
        return false;
    }
    const String TimeZone::toTypeStr(Type type)
    {
        switch (type)
        {
            case GMT_B1200:
                return "GMT-12:00";
            case GMT_B1100:
                return "GMT-11:00";
            case GMT_B1000:
                return "GMT-10:00";
            case GMT_B0900:
                return "GMT-09:00";
            case GMT_B0800:
                return "GMT-08:00";
            case GMT_B0700:
                return "GMT-07:00";
            case GMT_B0600:
                return "GMT-06:00";
            case GMT_B0500:
                return "GMT-05:00";
            case GMT_B0400:
                return "GMT-04:00";
            case GMT_B0330:
                return "GMT-03:30";
            case GMT_B0300:
                return "GMT-03:00";
            case GMT_B0200:
                return "GMT-02:00";
            case GMT_B0100:
                return "GMT-01:00";
            case GMT:
                return "GMT";
            case GMT_A0100:
                return "GMT+01:00";
            case GMT_A0200:
                return "GMT+02:00";
            case GMT_A0300:
                return "GMT+03:00";
            case GMT_A0330:
                return "GMT+03:30";
            case GMT_A0400:
                return "GMT+04:00";
            case GMT_A0430:
                return "GMT+04:30";
            case GMT_A0500:
                return "GMT+05:00";
            case GMT_A0530:
                return "GMT+05:30";
            case GMT_A0545:
                return "GMT+05:45";
            case GMT_A0600:
                return "GMT+06:00";
            case GMT_A0630:
                return "GMT+06:30";
            case GMT_A0700:
                return "GMT+07:00";
            case GMT_A0800:
                return "GMT+08:00";
            case GMT_A0900:
                return "GMT+09:00";
            case GMT_A0930:
                return "GMT+09:30";
            case GMT_A1000:
                return "GMT+10:00";
            case GMT_A1100:
                return "GMT+11:00";
            case GMT_A1200:
                return "GMT+12:00";
            case GMT_A1300:
                return "GMT+13:00";
            default:
                assert(false);
                return String::Empty;
        }
    }

    TimeZone::Type TimeZone::type() const
    {
        Type type = Type::GMT;
        int minute = localTimeValue();
        if(minute == 12 * 60)
        {
            type = GMT_B1200;
        }
        else if(minute == 11 * 60)
        {
            type = GMT_B1100;
        }
        else if(minute == 10 * 60)
        {
            type = GMT_B1000;
        }
        else if(minute == 9 * 60)
        {
            type = GMT_B0900;
        }
        else if(minute == 8 * 60)
        {
            type = GMT_B0800;
        }
        else if(minute == 7 * 60)
        {
            type = GMT_B0700;
        }
        else if(minute == 6 * 60)
        {
            type = GMT_B0600;
        }
        else if(minute == 5 * 60)
        {
            type = GMT_B0500;
        }
        else if(minute == 4 * 60)
        {
            type = GMT_B0400;
        }
        else if(minute == 3.5 * 60)
        {
            type = GMT_B0330;
        }
        else if(minute == 3 * 60)
        {
            type = GMT_B0300;
        }
        else if(minute == 2 * 60)
        {
            type = GMT_B0200;
        }
        else if(minute == 1 * 60)
        {
            type = GMT_B0100;
        }
        else if(minute == 0)
        {
            type = GMT;
        }
        else if(minute == -1 * 60)
        {
            type = GMT_A0100;
        }
        else if(minute == -2 * 60)
        {
            type = GMT_A0200;
        }
        else if(minute == -3 * 60)
        {
            type = GMT_A0300;
        }
        else if(minute == -3.5 * 60)
        {
            type = GMT_A0330;
        }
        else if(minute == -4 * 60)
        {
            type = GMT_A0400;
        }
        else if(minute == -4.5 * 60)
        {
            type = GMT_A0430;
        }
        else if(minute == -5 * 60)
        {
            type = GMT_A0500;
        }
        else if(minute == -5.5 * 60)
        {
            type = GMT_A0530;
        }
        else if(minute == -5.45 * 60)
        {
            type = GMT_A0545;
        }
        else if(minute == -6 * 60)
        {
            type = GMT_A0600;
        }
        else if(minute == -6.5 * 60)
        {
            type = GMT_A0630;
        }
        else if(minute == -7 * 60)
        {
            type = GMT_A0700;
        }
        else if(minute == -8 * 60)
        {
            type = GMT_A0800;
        }
        else if(minute == -9 * 60)
        {
            type = GMT_A0900;
        }
        else if(minute == -9.5 * 60)
        {
            type = GMT_A0930;
        }
        else if(minute == -10 * 60)
        {
            type = GMT_A1000;
        }
        else if(minute == -11 * 60)
        {
            type = GMT_A1100;
        }
        else if(minute == -12 * 60)
        {
            type = GMT_A1200;
        }
        else if(minute == -13 * 60)
        {
            type = GMT_A1300;
        }
        return type;
    }

    const String TimeZone::toTypeStr() const
    {
        return toTypeStr(type());
    }
}
