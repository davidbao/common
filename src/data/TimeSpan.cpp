//
//  TimeSpan::cpp
//  common
//
//  Created by baowei on 2015/8/7.
//  Copyright (c) 2015 com. All rights reserved.
//

#include <cmath>
#include "data/TimeSpan.h"
#include "system/Math.h"
#include "exception/Exception.h"
#include "data/StringArray.h"
#include "data/Culture.h"
#include "system/BCDUtilities.h"
#include "TimeSpanFormat.h"

namespace Common {
    const TimeSpan TimeSpan::MaxValue(LLONG_MAX);
    const TimeSpan TimeSpan::MinValue(LLONG_MIN);
    const TimeSpan TimeSpan::Zero(0);

    TimeSpan::TimeSpan(int64_t ticks) {
        _ticks = ticks;
    }

    TimeSpan::TimeSpan(int hours, int minutes, int seconds) {
        _ticks = timeToTicks(hours, minutes, seconds);
    }

    TimeSpan::TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds) {
        _ticks = timeToTicks(days, hours, minutes, seconds, milliseconds);
    }

    TimeSpan::TimeSpan(const TimeSpan &timeSpan) : _ticks(0) {
        this->operator=(timeSpan);
    }

    TimeSpan::~TimeSpan() = default;

    bool TimeSpan::equals(const TimeSpan &other) const {
        return (_ticks == other._ticks);
    }

    void TimeSpan::evaluates(const TimeSpan &other) {
        this->_ticks = other._ticks;
    }

    int TimeSpan::compareTo(const TimeSpan &other) const {
        if (_ticks > other._ticks) return 1;
        if (_ticks < other._ticks) return -1;
        return 0;
    }

    int64_t TimeSpan::timeToTicks(int hours, int minutes, int seconds) {
        // totalSeconds is bounded by 2^31 * 2^12 + 2^31 * 2^8 + 2^31,
        // which is less than 2^44, meaning we won't overflow totalSeconds.
        int64_t totalSeconds = (int64_t) hours * 3600 + (int64_t) minutes * 60 + (time_t) seconds;
        if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
            throw ArgumentOutOfRangeException(String::Empty,
                                              "TimeSpan overflowed because the duration is too long.");
        return totalSeconds * TicksPerSecond;
    }

    int64_t TimeSpan::timeToTicks(int days, int hours, int minutes, int seconds, int milliseconds) {
        int64_t totalMilliSeconds =
                ((int64_t) days * 3600 * 24 + (int64_t) hours * 3600 + (int64_t) minutes * 60 + seconds) * 1000 +
                milliseconds;
        if (totalMilliSeconds > MaxMilliSeconds || totalMilliSeconds < MinMilliSeconds)
            throw ArgumentOutOfRangeException(String::Empty,
                                              "TimeSpan overflowed because the duration is too long.");
        return totalMilliSeconds * TicksPerMillisecond;
    }

    bool TimeSpan::parse(const char *str, TimeSpan &timeSpan, const IFormatProvider<DateTimeFormatInfo> *provider) {
        return str != nullptr && parse(String(str), timeSpan, provider);
    }

    bool TimeSpan::parse(const String &str, TimeSpan &timeSpan, const IFormatProvider<DateTimeFormatInfo> *provider) {
        return TimeSpanParse::TryParse(str, provider, timeSpan);
    }

    String TimeSpan::toString(const String &format, const IFormatProvider<DateTimeFormatInfo> *provider) const {
        String fmtStr = !format.isNullOrEmpty() ? format : "c";

        // standard formats
        if (fmtStr.length() == 1) {
            char f = fmtStr[0];

            if (f == 'c' || f == 't' || f == 'T')
                return TimeSpanFormat::formatStandard(*this, true, fmtStr, TimeSpanFormat::Minimum);
            if (f == 'g' || f == 'G') {
                TimeSpanFormat::Pattern pattern;
                const DateTimeFormatInfo *dtfi = DateTimeFormatInfo::getInstance(provider);

                if (_ticks < 0)
                    fmtStr = dtfi->fullTimeSpanNegativePattern();
                else
                    fmtStr = dtfi->fullTimeSpanPositivePattern();
                if (f == 'g')
                    pattern = TimeSpanFormat::Minimum;
                else
                    pattern = TimeSpanFormat::Full;

                return TimeSpanFormat::formatStandard(*this, false, fmtStr, pattern);
            }
            return String::Empty;
        }

        return TimeSpanFormat::formatCustomized(*this, fmtStr, DateTimeFormatInfo::getInstance(provider));
    }

    int TimeSpan::days() const {
        return (int) (_ticks / TicksPerDay);
    }

    int TimeSpan::hours() const {
        return (int) (_ticks / TicksPerHour) % 24;
    }

    int TimeSpan::minutes() const {
        return (int) ((_ticks / TicksPerMinute) % 60);
    }

    int TimeSpan::seconds() const {
        return (int) ((_ticks / TicksPerSecond) % 60);
    }

    int TimeSpan::milliseconds() const {
        return (int) ((_ticks / TicksPerMillisecond) % 1000);
    }

    double TimeSpan::totalMilliseconds() const {
        double temp = (double) _ticks * MillisecondsPerTick;
        if (temp > MaxMilliSeconds)
            return (double) MaxMilliSeconds;

        if (temp < MinMilliSeconds)
            return (double) MinMilliSeconds;

        return temp;
    }

    double TimeSpan::totalSeconds() const {
        return (double) _ticks * SecondsPerTick;
    }

    double TimeSpan::totalDays() const {
        return ((double) _ticks) * DaysPerTick;
    }

    double TimeSpan::totalHours() const {
        return (double) _ticks * HoursPerTick;
    }

    double TimeSpan::totalMinutes() const {
        return (double) _ticks * MinutesPerTick;
    }

    int64_t TimeSpan::ticks() const {
        return _ticks;
    }

    void TimeSpan::writeBCDTime(Stream *stream) const {
        // such like HHmmss
        uint8_t buffer[3] = {0};
        off_t offset = 0;
        buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t) hours());
        buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t) minutes());
        buffer[offset++] = BCDUtilities::ByteToBCD((uint8_t) seconds());
        stream->write(buffer, 0, sizeof(buffer));
    }

    void TimeSpan::readBCDTime(Stream *stream) {
        // such like HHmmss
        uint8_t buffer[3] = {0};
        stream->read(buffer, 0, sizeof(buffer));
        off_t offset = 0;
        int hour = (int) BCDUtilities::BCDToInt64(buffer, offset++, 1);
        int minute = (int) BCDUtilities::BCDToInt64(buffer, offset++, 1);
        int second = (int) BCDUtilities::BCDToInt64(buffer, offset++, 1);
        _ticks = timeToTicks(hour, minute, second);
    }

    void TimeSpan::write(Stream *stream, bool bigEndian) const {
        stream->writeInt64(_ticks, bigEndian);
    }

    void TimeSpan::read(Stream *stream, bool bigEndian) {
        _ticks = stream->readInt64(bigEndian);
    }

    TimeSpan &TimeSpan::operator=(const TimeSpan &ts) {
        evaluates(ts);
        return *this;
    }

    TimeSpan TimeSpan::fromDays(double value) {
        return interval(value, MillisPerDay);
    }

    TimeSpan TimeSpan::fromHours(double value) {
        return interval(value, MillisPerHour);
    }

    TimeSpan TimeSpan::fromMinutes(double value) {
        return interval(value, MillisPerMinute);
    }

    TimeSpan TimeSpan::fromSeconds(double value) {
        return interval(value, MillisPerSecond);
    }

    TimeSpan TimeSpan::fromMilliseconds(double value) {
        return interval(value, 1);
    }

    TimeSpan TimeSpan::fromTicks(time_t value) {
        return TimeSpan(value);
    }

    TimeSpan TimeSpan::interval(double value, int scale) {
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

        return TimeSpan((time_t) millis * TicksPerMillisecond);
    }

    TimeSpan TimeSpan::add(const TimeSpan &ts) const {
        int64_t result = _ticks + ts._ticks;
        // Overflow if signs of operands was identical and result's
        // sign was opposite.
        // >> 63 gives the sign bit (either 64 1's or 64 0's).
        if ((_ticks >> 63 == ts._ticks >> 63) && (_ticks >> 63 != result >> 63))
            throw OverflowException("Overflow_TimeSpanTooLong");
        return TimeSpan(result);
    }

    TimeSpan TimeSpan::subtract(const TimeSpan &ts) const {
        int64_t result = _ticks - ts._ticks;
        // Overflow if signs of operands was different and result's
        // sign was opposite from the first argument's sign.
        // >> 63 gives the sign bit (either 64 1's or 64 0's).
        if ((_ticks >> 63 != ts._ticks >> 63) && (_ticks >> 63 != result >> 63))
            throw OverflowException("Overflow_TimeSpanTooLong");
        return TimeSpan(result);
    }

    TimeSpan TimeSpan::negate() const {
        if (ticks() == MinValue.ticks())
            return MinValue;
        return TimeSpan(-_ticks);
    }

    TimeSpan TimeSpan::operator+(const TimeSpan &ts) const {
        return this->add(ts);
    }

    TimeSpan TimeSpan::operator+=(const TimeSpan &ts) {
        *this = this->add(ts);
        return *this;
    }

    TimeSpan TimeSpan::operator-(const TimeSpan &ts) const {
        return this->subtract(ts);
    }

    TimeSpan TimeSpan::operator-=(const TimeSpan &ts) {
        *this = this->subtract(ts);
        return *this;
    }

    TimeSpan TimeSpan::operator+() const {
        return *this;
    }

    TimeSpan TimeSpan::operator-() const {
        if (_ticks == MinValue._ticks)
            throw OverflowException("Overflow_NegateTwosCompNum");
        return TimeSpan(-_ticks);
    }
}
