#include <math.h>
#include "data/TimeSpan.h"
#include "system/Convert.h"
#include "system/Math.h"
#include "exception/Exception.h"
#include "data/StringArray.h"
#include "system/BCDUtilities.h"

namespace Common
{
	const TimeSpan TimeSpan::MaxValue(LLONG_MAX);
	const TimeSpan TimeSpan::MinValue(LLONG_MIN);
    const TimeSpan TimeSpan::Zero(0);
    
	TimeSpan::TimeSpan(int64_t ticks)
	{
		_ticks = ticks;
	}
	TimeSpan::TimeSpan(int hours, int minutes, int seconds)
	{
		_ticks = timeToTicks(hours, minutes, seconds);
	}
	TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds)
	{
		_ticks = timeToTicks(days, hours, minutes, seconds, milliseconds);
	}
    TimeSpan::TimeSpan(const TimeSpan& timeSpan)
    {
        this->operator=(timeSpan);
    }

	TimeSpan::~TimeSpan()
	{
	}

	int64_t TimeSpan::timeToTicks(int hours, int minutes, int seconds)
	{
		// totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
		// which is less than 2^44, meaning we won't overflow totalSeconds.
		int64_t totalSeconds = (int64_t)hours * 3600 + (int64_t)minutes * 60 + (time_t)seconds;
		if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
            throw new ArgumentOutOfRangeException(String::Empty, "TimeSpan overflowed because the duration is too long.");
		return totalSeconds * TicksPerSecond;
	}
	int64_t TimeSpan::timeToTicks(int days, int hours, int minutes, int seconds, int milliseconds)
	{
		int64_t totalMilliSeconds = ((int64_t)days * 3600 * 24 + (int64_t)hours * 3600 + (int64_t)minutes * 60 + seconds) * 1000 + milliseconds;
		if (totalMilliSeconds > MaxMilliSeconds || totalMilliSeconds < MinMilliSeconds)
			throw new ArgumentOutOfRangeException(String::Empty, "TimeSpan overflowed because the duration is too long.");
		return totalMilliSeconds * TicksPerMillisecond;
	}

	bool TimeSpan::parse(const char* str, TimeSpan& timeSpan)
	{
		return str != NULL ? parse((String)str, timeSpan) : false;
	}
	bool TimeSpan::parse(const String& str, TimeSpan& timeSpan)
	{
		// (?<days>\d*)(\.|\A)(?<hours>\d+):(?<minutes>\d+):(?<seconds>\d+)(\.|\z)(?<milliseconds>\d*)
		if (str.isNullOrEmpty())
			return false;

		const int MaxCount = 16;
		int days = 0, hours = 0, minutes = 0, seconds = 0, milliseconds = 0;
		const char* buffer = str.c_str();
		int hoursIndex = (int)str.find(':');
		int daysIndex = (int)str.find('.');
		if (daysIndex >= 0 && daysIndex < hoursIndex)
		{
			// include days.
			char temp[MaxCount];
			memset(temp, 0, sizeof(temp));
			strncpy(temp, buffer, Math::min(daysIndex, MaxCount));
			Int32::parse(temp, days);

			buffer += daysIndex + 1;
		}
		if (buffer == NULL)
			return false;

		StringArray components;
		Convert::splitStr(buffer, ':', components);
		if (components.count() != 3)
		{
            int value;
            if(Int32::parse(str, value))
            {
                timeSpan._ticks = TimeSpan::fromMilliseconds(value)._ticks;
                return true;
            }
			return false;
		}
		String t1 = components.at(0);
		String t2 = components.at(1);
		String t3 = components.at(2);

		if (!Int32::parse(t1, hours))
			return false;
		if (!Int32::parse(t2, minutes))
			return false;

		buffer = t3.c_str();
		int millisecondsIndex = (int)t3.find('.');
		if (millisecondsIndex >= 0)
		{
			// include milliseconds.
			char temp[MaxCount];
			memset(temp, 0, sizeof(temp));
			strncpy(temp, buffer, Math::min(millisecondsIndex, MaxCount));
			if (!Int32::parse(temp, seconds))
				return false;

			buffer += millisecondsIndex + 1;
			if (!Int32::parse(buffer, milliseconds))
				return false;
		}
		else
		{
			if (!Int32::parse(buffer, seconds))
				return false;
		}

		timeSpan._ticks = timeToTicks(days, hours, minutes, seconds, milliseconds);

		return true;
	}

	String TimeSpan::toString(const Format format) const
	{
		switch (format)
		{
            case General:
            {
                if(totalMilliseconds() < 1000)  //Fn
                    return Int64((int64_t)totalMilliseconds()).toString();
                else if(days() > 1) // DnHHMMSS
                    return String::convert("%d.%02d:%02d:%02d", days(), hours(), minutes(), seconds());
                else if(totalMinutes() < 1) // HHMMSSfff
                    return String::convert("%02d:%02d:%02d.%03d", (int)totalHours(), minutes(), seconds(), milliseconds());
                else    // HHMMSS
                    return String::convert("%02d:%02d:%02d", (int)totalHours(), minutes(), seconds());
            }
                break;
            case HHMMSS:
                return String::convert("%02d:%02d:%02d", (int)totalHours(), minutes(), seconds());
            case DnHHMMSS:
                return String::convert("%d.%02d:%02d:%02d", days(), hours(), minutes(), seconds());
                break;
            case DnHHMMSSfff:
                return String::convert("%d.%02d:%02d:%02d.%03d", days(), hours(), minutes(), seconds(), milliseconds());
                break;
            case HHMMSSfff:
                return String::convert("%02d:%02d:%02d.%03d", (int)totalHours(), minutes(), seconds(), milliseconds());
            case Fn:
                return Int64((int64_t)totalMilliseconds()).toString();
            default:
                break;
		}
		return String::Empty;
	}

	int TimeSpan::days() const
	{
		return (int)(_ticks / TicksPerDay);
	}
	int TimeSpan::hours() const
	{
		return (int)(_ticks / TicksPerHour) % 24;
	}
	int TimeSpan::minutes() const
	{
		return (int)((_ticks / TicksPerMinute) % 60);
	}
	int TimeSpan::seconds() const
	{
		return (int)((_ticks / TicksPerSecond) % 60);
	}
	int TimeSpan::milliseconds() const
	{
		return (int)((_ticks / TicksPerMillisecond) % 1000);
	}

	double TimeSpan::totalMilliseconds() const
	{
		double temp = (double)_ticks * MillisecondsPerTick;
		if (temp > MaxMilliSeconds)
			return (double)MaxMilliSeconds;

		if (temp < MinMilliSeconds)
			return (double)MinMilliSeconds;

		return temp;
	}
	double TimeSpan::totalSeconds() const
	{
		return (double)_ticks * SecondsPerTick;
	}
	double TimeSpan::totalDays() const
	{
		return ((double)_ticks) * DaysPerTick;
	}
	double TimeSpan::totalHours() const
	{
		return (double)_ticks * HoursPerTick;
	}
	double TimeSpan::totalMinutes() const
	{
		return (double)_ticks * MinutesPerTick;
	}

	void TimeSpan::writeBCDTime(Stream* stream) const
	{
		// such like HHmmss
		uint8_t buffer[3];
		memset(buffer, 0, sizeof(buffer));

		off_t offset = 0;
		buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t)hours());
		buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t)minutes());
		buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t)seconds());

		stream->write(buffer, 0, sizeof(buffer));
	}
	void TimeSpan::readBCDTime(Stream* stream)
	{
		// such like HHmmss
		uint8_t buffer[3];
		memset(buffer, 0, sizeof(buffer));
		stream->read(buffer, 0, sizeof(buffer));
		bool isNull = true;
		for (uint i = 0; i < sizeof(buffer); i++)
		{
			if (buffer[i] != 0)
			{
				isNull = false;
				break;
			}
		}
		if (isNull)
		{
			_ticks = 0;
		}
		else
		{
			off_t offset = 0;
			int hour = (int)BCDUtilities::BCDToInt64(buffer, offset, 1);
			offset += 1;
			int minute = (int)BCDUtilities::BCDToInt64(buffer, offset, 1);
			offset += 1;
			int second = 0;
			second = (int)BCDUtilities::BCDToInt64(buffer, offset, 1);
//			offset += 1;

			_ticks = timeToTicks(hour, minute, second);
		}
	}
	void TimeSpan::write(Stream* stream, bool bigEndian) const
	{
		stream->writeUInt64(_ticks, bigEndian);
	}
	void TimeSpan::read(Stream* stream, bool bigEndian)
	{
		_ticks = stream->readUInt64(bigEndian);
	}

	void TimeSpan::operator=(const TimeSpan& timeSpan)
	{
		_ticks = timeSpan._ticks;
	}
	bool TimeSpan::operator==(const TimeSpan& timeSpan) const
	{
		return (_ticks == timeSpan._ticks);
	}
	bool TimeSpan::operator!=(const TimeSpan& timeSpan) const
	{
		return !operator==(timeSpan);
	}
	bool TimeSpan::operator>=(const TimeSpan& timeSpan) const
	{
		return (_ticks >= timeSpan._ticks);
	}
	bool TimeSpan::operator>(const TimeSpan& timeSpan) const
	{
		return (_ticks > timeSpan._ticks);
	}
	bool TimeSpan::operator<=(const TimeSpan& timeSpan) const
	{
		return (_ticks <= timeSpan._ticks);
	}
	bool TimeSpan::operator<(const TimeSpan& timeSpan) const
	{
		return (_ticks < timeSpan._ticks);
	}

	TimeSpan TimeSpan::fromDays(double value)
	{
		return interval(value, MillisPerDay);
	}
	TimeSpan TimeSpan::fromHours(double value)
	{
		return interval(value, MillisPerHour);
	}
	TimeSpan TimeSpan::fromMinutes(double value)
	{
		return interval(value, MillisPerMinute);
	}
	TimeSpan TimeSpan::fromSeconds(double value)
	{
		return interval(value, MillisPerSecond);
	}
	TimeSpan TimeSpan::fromMilliseconds(double value)
	{
		return interval(value, 1);
	}
	TimeSpan TimeSpan::fromTicks(time_t value)
	{
		return TimeSpan(value);
	}
	TimeSpan TimeSpan::interval(double value, int scale)
	{
#if !INT64_MAX
#define INT64_MAX        9223372036854775807LL
#endif
#if !INT64_MIN
#define INT64_MIN        (-INT64_MAX-1)
#endif
		if (value == NAN)
			throw ArgumentException("Arg_CannotBeNaN");

		double tmp = value * scale;
		double millis = tmp + (value >= 0 ? 0.5 : -0.5);
		if ((millis > INT64_MAX / TicksPerMillisecond) || (millis < INT64_MIN / TicksPerMillisecond))
			throw OverflowException("Overflow_TimeSpanTooLong");

		return TimeSpan((time_t)millis * TicksPerMillisecond);
	}
    
    TimeSpan TimeSpan::add(const TimeSpan& ts) const
    {
        int64_t result = _ticks + ts._ticks;
        // Overflow if signs of operands was identical and result's
        // sign was opposite.
        // >> 63 gives the sign bit (either 64 1's or 64 0's).
        if ((_ticks >> 63 == ts._ticks >> 63) && (_ticks >> 63 != result >> 63))
            throw OverflowException("Overflow_TimeSpanTooLong");
        return TimeSpan(result);
    }
    TimeSpan TimeSpan::subtract(const TimeSpan& ts) const
    {
        int64_t result = _ticks - ts._ticks;
        // Overflow if signs of operands was different and result's
        // sign was opposite from the first argument's sign.
        // >> 63 gives the sign bit (either 64 1's or 64 0's).
        if ((_ticks >> 63 != ts._ticks >> 63) && (_ticks >> 63 != result >> 63))
            throw OverflowException("Overflow_TimeSpanTooLong");
        return TimeSpan(result);
    }
    
    // Returns a value less than zero if this  object
    int TimeSpan::compareTo(TimeSpan value) const
    {
        int64_t t = value._ticks;
        if (_ticks > t) return 1;
        if (_ticks < t) return -1;
        return 0;
    }
    int TimeSpan::compare(TimeSpan t1, TimeSpan t2)
    {
        if (t1._ticks > t2._ticks) return 1;
        if (t1._ticks < t2._ticks) return -1;
        return 0;
    }
    
    TimeSpan TimeSpan::operator+(const TimeSpan& ts) const
    {
        return this->add(ts);
    }
    TimeSpan TimeSpan::operator-(const TimeSpan& ts) const
    {
        return this->subtract(ts);
    }
    
    TimeSpan TimeSpan::operator+() const
    {
        return *this;
    }
    TimeSpan TimeSpan::operator-() const
    {
        if (_ticks==MinValue._ticks)
            throw OverflowException("Overflow_NegateTwosCompNum");
        return TimeSpan(-_ticks);
    }
}
