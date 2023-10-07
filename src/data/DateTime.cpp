//
//  DateTime.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "data/DateTime.h"
#include "data/ValueType.h"
#include "data/TimeSpan.h"
#include "data/TimeZone.h"
#include "exception/Exception.h"
#include "system/BCDProvider.h"
#include "system/Resources.h"
#include "system/Math.h"
#include "DateTimeFormat.h"
#include <assert.h>

using namespace System;

namespace Data {
    const int DateTime::DaysToMonth365[13] = {
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
    const int DateTime::DaysToMonth366[13] = {
            0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

    const DateTime DateTime::MinValue(MinTicks);
    const DateTime DateTime::MaxValue(MaxTicks);
    const DateTime DateTime::Min2010Value(2010, 1, 1);

    const DateTime DateTime::UtcPoint1900 = DateTime(1900, 1, 1, 0, 0, 0, 0, Kind::Utc);
    const DateTime DateTime::UtcPoint1970 = DateTime(1970, 1, 1, 0, 0, 0, 0, Kind::Utc);
    const DateTime DateTime::UtcPoint2010 = DateTime(2010, 1, 1, 0, 0, 0, 0, Kind::Utc);

    DateTime::DateTime(uint64_t ticks, Kind kind) {
        _dateData = ((uint64_t) ticks | ((uint64_t) kind << KindShift));
    }

    DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond, Kind kind) {
        _dateData = toTicks(year, month, day, hour, minute, second, millisecond, kind);
    }

    DateTime::DateTime(int year, int month, int day, Kind kind) : DateTime(year, month, day, 0, 0, 0, 0, kind) {
    }

    DateTime::DateTime(const DateTime &time) : _dateData(0) {
        this->operator=(time);
    }

    DateTime::~DateTime() = default;

    bool DateTime::equals(const DateTime &other) const {
        return (internalTicks() == other.internalTicks());
    }

    void DateTime::evaluates(const DateTime &other) {
        this->_dateData = other._dateData;
    }

    int DateTime::compareTo(const DateTime &other) const {
        if (internalTicks() > other.internalTicks()) return 1;
        if (internalTicks() < other.internalTicks()) return -1;
        return 0;
    }

    uint64_t
    DateTime::toTicks(int year, int month, int day, int hour, int minute, int second, int millisecond, Kind kind) {
        uint64_t ticks = dateToTicks(year, month, day) + timeToTicks(hour, minute, second);
        ticks += millisecond * TicksPerMillisecond;
        return ((uint64_t) ticks | ((uint64_t) kind << KindShift));
    }

    DateTime DateTime::now() {
        system_clock::time_point tp = system_clock::now();
        time_t tt = system_clock::to_time_t(tp);
        struct tm *tmp = localtime(&tt);
        auto currentTimeRounded = system_clock::from_time_t(tt);
        if (currentTimeRounded > tp) {
            currentTimeRounded -= std::chrono::seconds(1);
        }
        int milliseconds = duration_cast<duration<int, std::milli>>(tp - currentTimeRounded).count();
        DateTime dateTime(tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
                          milliseconds, Local);
        return dateTime;
    }

    DateTime DateTime::utcNow() {
        system_clock::time_point tp = system_clock::now();
        time_t tt = system_clock::to_time_t(tp);
        struct tm *tmp = gmtime(&tt);
        auto currentTimeRounded = system_clock::from_time_t(tt);
        if (currentTimeRounded > tp) {
            currentTimeRounded -= std::chrono::seconds(1);
        }
        int milliseconds = duration_cast<duration<int, std::milli>>(tp - currentTimeRounded).count();
        DateTime dateTime(tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
                          milliseconds, Utc);
        return dateTime;
    }

    DateTime DateTime::toLocalTime() const {
        return TimeZone::Local.toLocalTime(*this);
    }

    DateTime DateTime::toUtcTime() const {
        return TimeZone::Local.toUtcTime(*this);
    }

    bool DateTime::isValid(int year, int month, int day) {
        if (year >= 1 && year <= 9999 && month >= 1 && month <= 12) {
            const int *days = isLeapYear(year) ? DaysToMonth366 : DaysToMonth365;
            if (day >= 1 && day <= days[month] - days[month - 1]) {
                return true;
            }
        }
        return false;
    }

    uint64_t DateTime::dateToTicks(int year, int month, int day) {
        if (year >= 1 && year <= 9999 && month >= 1 && month <= 12) {
            const int *days = isLeapYear(year) ? DaysToMonth366 : DaysToMonth365;
            if (day >= 1 && day <= days[month] - days[month - 1]) {
                int y = year - 1;
                int n = y * 365 + y / 4 - y / 100 + y / 400 + days[month - 1] + day - 1;
                return n * TicksPerDay;
            }
        }
        throw ArgumentOutOfRangeException(String::Empty,
                                          "Year, Month, and Day parameters describe an un - representable DateTime.");
    }

    uint64_t DateTime::timeToTicks(int hour, int minute, int second) {
        //TimeSpan.TimeToTicks is a family access function which does no error checking, so
        //we need to put some error checking out here.
        if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60 && second >= 0 && second < 60) {
            return TimeSpan::timeToTicks(hour, minute, second);
        }
        throw ArgumentOutOfRangeException(String::Empty,
                                          "Hour, Minute, and Second parameters describe an un-representable DateTime.");
    }

    bool DateTime::isLeapYear(int year) {
        if (year < 1 || year > 9999) {
            throw ArgumentOutOfRangeException("year", "Year must be between 1 and 9999.");
        }
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    // Returns a given date part of this DateTime. This method is used
    // to compute the year, day-of-year, month, or day part.
    int DateTime::getDatePart(int part) const {
        uint64_t ticks = internalTicks();
        // n = number of days since 1/1/0001
        int n = (int) (ticks / TicksPerDay);
        // y400 = number of whole 400-year periods since 1/1/0001
        int y400 = n / DaysPer400Years;
        // n = day number within 400-year period
        n -= y400 * DaysPer400Years;
        // y100 = number of whole 100-year periods within 400-year period
        int y100 = n / DaysPer100Years;
        // Last 100-year period has an extra day, so decrement result if 4
        if (y100 == 4) y100 = 3;
        // n = day number within 100-year period
        n -= y100 * DaysPer100Years;
        // y4 = number of whole 4-year periods within 100-year period
        int y4 = n / DaysPer4Years;
        // n = day number within 4-year period
        n -= y4 * DaysPer4Years;
        // y1 = number of whole years within 4-year period
        int y1 = n / DaysPerYear;
        // Last year has an extra day, so decrement result if 4
        if (y1 == 4) y1 = 3;
        // If year was requested, compute and return it
        if (part == DatePartYear) {
            return y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1;
        }
        // n = day number within year
        n -= y1 * DaysPerYear;
        // If day-of-year was requested, return it
        if (part == DatePartDayOfYear) return n + 1;
        // Leap year calculation looks different from IsLeapYear since y1, y4,
        // and y100 are relative to year 1, not year 0
        bool leapYear = y1 == 3 && (y4 != 24 || y100 == 3);
        const int *days = leapYear ? DaysToMonth366 : DaysToMonth365;
        // All months have less than 32 days, so n >> 5 is a good conservative
        // estimate for the month
        int m = (n >> 5) + 1;
        // m = 1-based month number
        while (n >= days[m]) m++;
        // If month was requested, return it
        if (part == DatePartMonth) return m;
        // Return 1-based day-of-month
        return n - days[m - 1] + 1;
    }

    uint64_t DateTime::internalTicks() const {
        return (uint64_t) (_dateData & TicksMask);
    }

    uint64_t DateTime::internalKind() const {
        return (_dateData & FlagsMask);
    }

    bool DateTime::parse(const String &str, DateTime &dateTime) {
        if (str.isNullOrEmpty())
            return false;

        const DateTimeFormatInfo *dtfi = &DateTimeFormatInfo::currentInfo();

        const int MaxCount = 16;
        char temp[MaxCount];

        int year = 1, month = 1, day = 1, hour = 0, minute = 0, second = 0, millisecond = 0;
        const char *buffer = str.c_str();
        int yearIndex = (int) ((String) buffer).find(dtfi->dateSeparator);
        if (yearIndex > 0) {
            // include year.
            memset(temp, 0, sizeof(temp));
            strncpy(temp, buffer, Math::min(yearIndex, MaxCount));
            Int32::parse(temp, year);

            buffer += yearIndex + 1;

            int monthIndex = (int) ((String) buffer).find(dtfi->dateSeparator);
            if (monthIndex > 0) {
                // include month.
                memset(temp, 0, sizeof(temp));
                strncpy(temp, buffer, Math::min(monthIndex, MaxCount));
                Int32::parse(temp, month);

                buffer += monthIndex + 1;

                int dayIndex = (int) ((String) buffer).find(' ');
                if (dayIndex > 0) {
                    // include day.
                    memset(temp, 0, sizeof(temp));
                    strncpy(temp, buffer, Math::min(dayIndex, MaxCount));
                    Int32::parse(temp, day);

                    buffer += dayIndex + 1;
                } else {
                    // can not find space.
                    Int32::parse(buffer, day);

                    buffer += strlen(buffer);
                }
            }
        }
        if (buffer == nullptr)
            return false;

        if (strlen(buffer) > 0) {
            TimeSpan timeSpan;
            if (!TimeSpan::parse(buffer, timeSpan))
                return false;

            if (timeSpan.days() >= 1) {
                day = timeSpan.days();
            }
            hour = timeSpan.hours();
            minute = timeSpan.minutes();
            second = timeSpan.seconds();
            millisecond = timeSpan.milliseconds();
        }

        try {
            dateTime._dateData = toTicks(year, month, day, hour, minute, second, millisecond);
        } catch (...) {
            return false;
        }

        return true;
    }

    String DateTime::toString(const String &format, const IFormatProvider<DateTimeFormatInfo> *provider) const {
        const DateTimeFormatInfo *dtfi = DateTimeFormatInfo::getInstance(provider);
        return DateTimeFormat::Format(*this, format, dtfi);
    }

    // Returns the day-of-week part of this DateTime. The returned value
    // is an integer between 0 and 6, where 0 indicates Sunday, 1 indicates
    // Monday, 2 indicates Tuesday, 3 indicates Wednesday, 4 indicates
    // Thursday, 5 indicates Friday, and 6 indicates Saturday.
    //
    DayOfWeek DateTime::dayOfWeek() const {
        return (DayOfWeek) ((internalTicks() / TicksPerDay + 1) % 7);
    }

    // Returns the day-of-year part of this DateTime. The returned value
    // is an integer between 1 and 366.
    //
    int DateTime::dayOfYear() const {
        return getDatePart(DatePartDayOfYear);
    }

    // Returns the year part of this DateTime. The returned value is an
    // integer between 1 and 9999.
    //
    int DateTime::year() const {
        return getDatePart(DatePartYear);
    }

    // Returns the day-of-month part of this DateTime. The returned
    // value is an integer between 1 and 31.
    //
    int DateTime::day() const {
        return getDatePart(DatePartDay);
    }

    // Returns the hour part of this DateTime. The returned value is an
    // integer between 0 and 23.
    //
    int DateTime::hour() const {
        return (int) ((internalTicks() / TicksPerHour) % 24);
    }

    // Returns the minute part of this DateTime. The returned value is
    // an integer between 0 and 59.
    //
    int DateTime::minute() const {
        return (int) ((internalTicks() / TicksPerMinute) % 60);
    }

    // Returns the date part of this DateTime. The resulting value
    // corresponds to this DateTime with the time-of-day part set to
    // zero (midnight).
    //
    DateTime DateTime::date() const {
        uint64_t ticks = internalTicks();
        return {(uint64_t) (ticks - ticks % TicksPerDay) | internalKind()};
    }

    DateTime::Kind DateTime::kind() const {
        switch (internalKind()) {
            case Kind::Unspecified:
                return Kind::Unspecified;
            case KindUtc:
                return Kind::Utc;
            default:
                return Kind::Local;
        }
    }

    // Returns the millisecond part of this DateTime. The returned value
    // is an integer between 0 and 999.
    //
    int DateTime::millisecond() const {
        return (int) ((internalTicks() / TicksPerMillisecond) % 1000);
    }

    // Returns the month part of this DateTime. The returned value is an
    // integer between 1 and 12.
    //
    int DateTime::month() const {
        return getDatePart(DatePartMonth);
    }

    // Returns the second part of this DateTime. The returned value is
    // an integer between 0 and 59.
    //
    int DateTime::second() const {
        return (int) ((internalTicks() / TicksPerSecond) % 60);
    }

    // Returns the tick count for this DateTime. The returned value is
    // the number of 100-nanosecond intervals that have elapsed since 1/1/0001
    // 12:00am.
    //
    uint64_t DateTime::ticks() const {
        return internalTicks();
    }

    // Returns the time-of-day part of this DateTime. The returned value
    // is a TimeSpan that indicates the time elapsed since midnight.
    //
    TimeSpan DateTime::timeOfDay() const {
        return TimeSpan(internalTicks() % TicksPerDay);
    }

    double DateTime::total1970Milliseconds() const {
        return DateTime::total1970Milliseconds(*this);
    }

    double DateTime::total2010Milliseconds() const {
        return DateTime::total2010Milliseconds(*this);
    }

    // Returns a DateTime representing the current date. The date part
    // of the returned value is the current date, and the time-of-day part of
    // the returned value is zero (midnight).
    //
    DateTime DateTime::today() {
        return DateTime::now().date();
    }

    void DateTime::writeBCDDateTime(Stream *stream, bool includedSec, bool includedMs) const {
        // such like YYYYMMDDHHmmss or YYYYMMDDHHSS
        int len = includedSec ? 7 : 6;
        if (includedMs) {
            len += 2;
        }
        uint8_t *buffer = new uint8_t[len];
        memset(buffer, 0, len);

        off_t offset = 0;
        BCDProvider::bin2buffer((uint16_t) year(), buffer + offset);
        offset += 2;
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) month());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) day());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) hour());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) minute());
        if (includedSec) {
            buffer[offset++] = BCDProvider::bin2bcd((uint8_t) second());
        }
        if (includedMs) {
            BCDProvider::bin2buffer((uint16_t) millisecond(), buffer + offset);
//			offset += 2;
        }

        stream->write(buffer, 0, len);
        delete[] buffer;
    }

    void DateTime::readBCDDateTime(Stream *stream, bool includedSec, bool includedMs) {
        // such like YYYYMMDDHHmmss or YYYYMMDDHHSS
        int len = includedSec ? 7 : 6;
        if (includedMs) {
            len += 2;
        }
        uint8_t *buffer = new uint8_t[len];
        memset(buffer, 0, len);
        stream->read(buffer, 0, len);
        bool isNull = true;
        for (size_t i = 0; i < sizeof(buffer); i++) {
            if (buffer[i] != 0) {
                isNull = false;
                break;
            }
        }

        if (isNull) {
            _dateData = 0;
        } else {
            off_t offset = 0;
            int year = (int) BCDProvider::bcd2bin(buffer, offset, 2);
            offset += 2;
            int month = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int day = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;

            int hour = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int minute = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int second = 0;
            if (includedSec) {
                second = (int) BCDProvider::bcd2bin(buffer, offset, 1);
                offset += 1;
            }
            int millisecond = 0;
            if (includedMs) {
                millisecond = (int) BCDProvider::bcd2bin(buffer, offset, 2);
//				offset += 2;
            }
            _dateData = toTicks(year, month, day, hour, minute, second, millisecond);
        }
        delete[] buffer;
    }

    void DateTime::writeBCDDate(Stream *stream) const {
        // such like YYYYMMDD
        uint8_t buffer[4];
        memset(buffer, 0, sizeof(buffer));

        off_t offset = 0;
        BCDProvider::bin2buffer((int16_t) year(), buffer + offset);
        offset += 2;
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) month());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) day());

        stream->write(buffer, 0, sizeof(buffer));
    }

    void DateTime::readBCDDate(Stream *stream) {
        uint8_t buffer[4];
        memset(buffer, 0, sizeof(buffer));
        stream->read(buffer, 0, sizeof(buffer));
        bool isNull = true;
        for (size_t i = 0; i < sizeof(buffer); i++) {
            if (buffer[i] != 0) {
                isNull = false;
                break;
            }
        }
        if (isNull) {
            _dateData = 0;
        } else {
            off_t offset = 0;
            int year = (int) BCDProvider::bcd2bin(buffer, offset, 2);
            offset += 2;
            int month = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int day = (int) BCDProvider::bcd2bin(buffer, offset, 1);
//			offset += 1;

            _dateData = toTicks(year, month, day);
        }
    }

    void DateTime::writeBCDTime(Stream *stream) const {
        // such like HHmmss
        uint8_t buffer[3];
        memset(buffer, 0, sizeof(buffer));

        off_t offset = 0;
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) hour());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) minute());
        buffer[offset++] = BCDProvider::bin2bcd((uint8_t) second());

        stream->write(buffer, 0, sizeof(buffer));
    }

    void DateTime::readBCDTime(Stream *stream) {
        // such like HHmmss
        uint8_t buffer[3];
        memset(buffer, 0, sizeof(buffer));
        stream->read(buffer, 0, sizeof(buffer));
        bool isNull = true;
        for (size_t i = 0; i < sizeof(buffer); i++) {
            if (buffer[i] != 0) {
                isNull = false;
                break;
            }
        }
        if (isNull) {
            _dateData = 0;
        } else {
            off_t offset = 0;
            int hour = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int minute = (int) BCDProvider::bcd2bin(buffer, offset, 1);
            offset += 1;
            int second = 0;
            second = (int) BCDProvider::bcd2bin(buffer, offset, 1);
//			offset += 1;

            _dateData = toTicks(1, 1, 1, hour, minute, second);
        }
    }

    void DateTime::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt64(_dateData, bigEndian);
    }

    void DateTime::read(Stream *stream, bool bigEndian) {
        _dateData = stream->readUInt64(bigEndian);
    }

    void DateTime::writeMilliseconds(Stream *stream, bool bigEndian) const {
        int64_t value = (this->operator-(DateTime::Min2010Value)).totalMilliseconds();
        stream->writeInt48(value, bigEndian);
    }

    void DateTime::readMilliseconds(Stream *stream, bool bigEndian) {
        int64_t value = stream->readInt48(bigEndian);
        _dateData = (DateTime::Min2010Value + TimeSpan::fromMilliseconds(value))._dateData;
    }

    DateTime &DateTime::operator=(const DateTime &value) {
        evaluates(value);
        return *this;
    }

    DateTime DateTime::operator+(const TimeSpan &value) const {
        int64_t ticks = (int64_t) internalTicks();
        int64_t valueTicks = (int64_t) value.ticks();
        if (valueTicks > MaxTicks - ticks || valueTicks < MinTicks - ticks) {
            throw ArgumentOutOfRangeException("timeSpan", "ArgumentOutOfRange_DateArithmetic");
        }
        return DateTime((ticks + valueTicks) | internalKind());
    }

    DateTime DateTime::operator-(const TimeSpan &value) const {
        int64_t ticks = (int64_t) internalTicks();
        int64_t valueTicks = (int64_t) value.ticks();
        if (ticks - MinTicks < valueTicks || ticks - MaxTicks > valueTicks) {
            throw ArgumentOutOfRangeException("timeSpan", "ArgumentOutOfRange_DateArithmetic");
        }
        return DateTime((ticks - valueTicks) | internalKind());
    }

    TimeSpan DateTime::operator-(const DateTime &dateTime) const {
        return TimeSpan(internalTicks() - dateTime.internalTicks());
    }

    DateTime DateTime::operator+=(const TimeSpan &value) {
        this->operator=(this->add(value));
        return *this;
    }

    DateTime DateTime::operator-=(const TimeSpan &value) {
        this->operator=(this->add(-value));
        return *this;
    }

    // Returns the number of days in the month given by the year and
    // month arguments.
    //
    int DateTime::daysInMonth(int year, int month) {
        if (month < 1 || month > 12) throw ArgumentOutOfRangeException("month", "ArgumentOutOfRange_Month");
        // IsLeapYear checks the year argument
        const int *days = isLeapYear(year) ? DaysToMonth366 : DaysToMonth365;
        return days[month] - days[month - 1];
    }

    // Returns the DateTime resulting from adding the given
    // TimeSpan to this DateTime.
    //
    DateTime DateTime::add(const TimeSpan &value) const {
        return addTicks(value._ticks);
    }

    // Returns the DateTime resulting from adding a fractional number of
    // time units to this DateTime.
    DateTime DateTime::add(double value, int scale) const {
        int64_t millis = (int64_t) (value * scale + (value >= 0 ? 0.5 : -0.5));
        if (millis <= -MaxMillis || millis >= MaxMillis)
            throw ArgumentOutOfRangeException("value", "ArgumentOutOfRange_addValue");
        return addTicks(millis * TicksPerMillisecond);
    }

    // Returns the DateTime resulting from adding a fractional number of
    // days to this DateTime. The result is computed by rounding the
    // fractional number of days given by value to the nearest
    // millisecond, and adding that interval to this DateTime. The
    // value argument is permitted to be negative.
    //
    DateTime DateTime::addDays(double value) const {
        return add(value, MillisPerDay);
    }

    // Returns the DateTime resulting from adding a fractional number of
    // hours to this DateTime. The result is computed by rounding the
    // fractional number of hours given by value to the nearest
    // millisecond, and adding that interval to this DateTime. The
    // value argument is permitted to be negative.
    //
    DateTime DateTime::addHours(double value) const {
        return add(value, MillisPerHour);
    }

    // Returns the DateTime resulting from the given number of
    // milliseconds to this DateTime. The result is computed by rounding
    // the number of milliseconds given by value to the nearest integer,
    // and adding that interval to this DateTime. The value
    // argument is permitted to be negative.
    //
    DateTime DateTime::addMilliseconds(double value) const {
        return add(value, 1);
    }

    // Returns the DateTime resulting from adding a fractional number of
    // minutes to this DateTime. The result is computed by rounding the
    // fractional number of minutes given by value to the nearest
    // millisecond, and adding that interval to this DateTime. The
    // value argument is permitted to be negative.
    //
    DateTime DateTime::addMinutes(double value) const {
        return add(value, MillisPerMinute);
    }

    // Returns the DateTime resulting from adding the given number of
    // months to this DateTime. The result is computed by incrementing
    // (or decrementing) the year and month parts of this DateTime by
    // months months, and, if required, adjusting the day part of the
    // resulting date downwards to the last day of the resulting month in the
    // resulting year. The time-of-day part of the result is the same as the
    // time-of-day part of this DateTime.
    //
    // In more precise terms, considering this DateTime to be of the
    // form y / m / d + t, where y is the
    // year, m is the month, d is the day, and t is the
    // time-of-day, the result is y1 / m1 / d1 + t,
    // where y1 and m1 are computed by adding months months
    // to y and m, and d1 is the largest value less than
    // or equal to d that denotes a valid day in month m1 of year
    // y1.
    //
    DateTime DateTime::addMonths(int months) const {
        if (months < -120000 || months > 120000)
            throw ArgumentOutOfRangeException("months", "ArgumentOutOfRange_DateTimeBadMonths");
        int y = getDatePart(DatePartYear);
        int m = getDatePart(DatePartMonth);
        int d = getDatePart(DatePartDay);
        int i = m - 1 + months;
        if (i >= 0) {
            m = i % 12 + 1;
            y = y + i / 12;
        } else {
            m = 12 + (i + 1) % 12;
            y = y + (i - 11) / 12;
        }
        if (y < 1 || y > 9999) {
            throw ArgumentOutOfRangeException("months", "ArgumentOutOfRange_DateArithmetic");
        }
        int days = daysInMonth(y, m);
        if (d > days) d = days;
        return DateTime((uint64_t) (dateToTicks(y, m, d) + internalTicks() % TicksPerDay) | internalKind());
    }

    // Returns the DateTime resulting from adding a fractional number of
    // seconds to this DateTime. The result is computed by rounding the
    // fractional number of seconds given by value to the nearest
    // millisecond, and adding that interval to this DateTime. The
    // value argument is permitted to be negative.
    //
    DateTime DateTime::addSeconds(double value) const {
        return add(value, MillisPerSecond);
    }

    // Returns the DateTime resulting from adding the given number of
    // 100-nanosecond ticks to this DateTime. The value argument
    // is permitted to be negative.
    //
    DateTime DateTime::addTicks(int64_t value) const {
        int64_t ticks = (int64_t) internalTicks();
        if (value > MaxTicks - ticks || value < MinTicks - ticks) {
            throw ArgumentOutOfRangeException("value", "ArgumentOutOfRange_DateArithmetic");
        }
        return DateTime((uint64_t) (ticks + value) | internalKind());
    }

    // Returns the DateTime resulting from adding the given number of
    // years to this DateTime. The result is computed by incrementing
    // (or decrementing) the year part of this DateTime by value
    // years. If the month and day of this DateTime is 2/29, and if the
    // resulting year is not a leap year, the month and day of the resulting
    // DateTime becomes 2/28. Otherwise, the month, day, and time-of-day
    // parts of the result are the same as those of this DateTime.
    //
    DateTime DateTime::addYears(int value) const {
        if (value < -10000 || value > 10000)
            throw ArgumentOutOfRangeException("years", "ArgumentOutOfRange_DateTimeBadYears");
        return addMonths(value * 12);
    }

    uint32_t DateTime::subtract(DateTime prev, Resolutions tr) const {
        DateTime value = *this;
        if (value < prev) {
//            throw ArgumentException("value must be greater than prev", "prevTime");
            return 0;
        }

        if (tr == Resolutions::ResNone) {
//            throw ArgumentException("Resolutions must not be equal to None", "tr");
            return 0;
        }

        uint32_t time = 0;
        switch (tr) {
            case Resolutions::ResMillisecond:
                time = (uint32_t) (value - prev).totalMilliseconds();
                break;
            case Resolutions::ResSecond:
                time = (uint32_t) (value - prev).totalSeconds();
                break;
            case Resolutions::ResMinute:
                time = (uint32_t) (value - prev).totalMinutes();
                break;
            case Resolutions::ResHour:
                time = (uint32_t) (value - prev).totalHours();
                break;
            default:
                assert(false);
                break;
        }
        return time;
    }

    DateTime DateTime::add(uint32_t time, Resolutions tr) const {
        DateTime value = *this;
        if (tr == Resolutions::ResNone) {
            throw ArgumentException("Resolutions must not be equal to None", "tr");
        }

        DateTime result = MinValue;
        switch (tr) {
            case Resolutions::ResMillisecond:
                result = value.addMilliseconds(time);
                break;
            case Resolutions::ResSecond:
                result = value.addSeconds(time);
                break;
            case Resolutions::ResMinute:
                result = value.addMinutes(time);
                break;
            case Resolutions::ResHour:
                result = value.addHours(time);
                break;
            default:
                assert(false);
                break;
        }
        return result;
    }

    DateTime::Resolutions DateTime::fromResolutionStr(const String &str) {
        Resolutions tr = Resolutions::ResSecond;
        if (String::equals(str, "Second", true)) {
            tr = Resolutions::ResSecond;
        } else if (String::equals(str, "Millisecond", true)) {
            tr = Resolutions::ResMillisecond;
        } else if (String::equals(str, "Minute", true)) {
            tr = Resolutions::ResMinute;
        } else if (String::equals(str, "Hour", true)) {
            tr = Resolutions::ResHour;
        }
        return tr;
    }

    String DateTime::toResolutionStr(Resolutions tr) {
        switch (tr) {
            case Resolutions::ResMillisecond:
                return "Millisecond";
            case Resolutions::ResSecond:
                return "Second";
            case Resolutions::ResMinute:
                return "Minute";
            case Resolutions::ResHour:
                return "Hour";
            default:
                return "None";
        }
    }

    DateTime DateTime::fromLocalTime(time_t time) {
        if (time > 0) {
            struct tm *tmp = localtime(&time);
            system_clock::time_point timec = system_clock::from_time_t(time);
            auto currentTimeRounded = system_clock::from_time_t(time);
            if (currentTimeRounded > timec) {
                currentTimeRounded -= std::chrono::seconds(1);
            }
            int milliseconds = duration_cast<duration<int, std::milli> >(timec - currentTimeRounded).count();

            return DateTime(tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                            tmp->tm_hour, tmp->tm_min, tmp->tm_sec, milliseconds, Kind::Local);
        }
        return DateTime::MinValue;
    }

    time_t DateTime::toLocalTime(const DateTime &time) {
        return (time_t) (total1970Milliseconds(time) / 1000);
    }

    double DateTime::total1970Milliseconds(const DateTime &time) {
        static DateTime LocalPoint1970 = TimeZone::Local.toLocalTime(DateTime::UtcPoint1970);
        DateTime local = TimeZone::Local.toLocalTime(time);
        return (local - LocalPoint1970).totalMilliseconds();
    }

    DateTime DateTime::from1970Milliseconds(double time, bool utc) {
        static DateTime LocalPoint1970 = TimeZone::Local.toLocalTime(DateTime::UtcPoint1970);
        DateTime result = (utc ? UtcPoint1970 : LocalPoint1970) + TimeSpan::fromMilliseconds(time);
        return result;
    }

    double DateTime::total2010Milliseconds(const DateTime &time) {
        static DateTime LocalPoint2010 = TimeZone::Local.toLocalTime(DateTime::UtcPoint2010);
        DateTime local = TimeZone::Local.toLocalTime(time);
        return (local - LocalPoint2010).totalMilliseconds();
    }

    DateTime DateTime::from2010Milliseconds(double time, bool utc) {
        static DateTime LocalPoint2010 = TimeZone::Local.toLocalTime(DateTime::UtcPoint2010);
        DateTime result = (utc ? UtcPoint2010 : LocalPoint2010) + TimeSpan::fromMilliseconds(time);
        return result;
    }
}
