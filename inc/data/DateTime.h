//
//  DateTime.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef DateTime_h
#define DateTime_h

#include <chrono>
#include "TimeSpan.h"
#include "IO/Stream.h"
#include "data/String.h"
#include "data/Culture.h"

using namespace std;
using namespace std::chrono;

namespace Data {
    class DateTimeFormat;

    struct DateTime : public IEquatable<DateTime>, public IEvaluation<DateTime>, public IComparable<DateTime> {
    public:
        enum Kind {
            Unspecified = 0,
            Utc = 1,
            Local = 2,
        };
        enum Resolutions : uint8_t {
            ResNone = 0,
            ResMillisecond = 1,
            ResSecond = 2,
            ResMinute = 3,
            ResHour = 4,
        };

        DateTime(uint64_t ticks = 0, Kind kind = Unspecified);

        DateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0,
                 Kind kind = Unspecified);

        DateTime(int year, int month, int day, Kind kind);

        DateTime(const DateTime &time);

        ~DateTime() override;

        bool equals(const DateTime &other) const override;

        void evaluates(const DateTime &other) override;

        int compareTo(const DateTime &other) const override;

        DateTime toLocalTime() const;

        DateTime toUtcTime() const;

        String toString(const String &format = String::Empty, const IFormatProvider<DateTimeFormatInfo> *provider = nullptr) const;

        // Returns the day-of-week part of this DateTime. The returned value
        // is an integer between 0 and 6, where 0 indicates Sunday, 1 indicates
        // Monday, 2 indicates Tuesday, 3 indicates Wednesday, 4 indicates
        // Thursday, 5 indicates Friday, and 6 indicates Saturday.
        //
        DayOfWeek dayOfWeek() const;

        // Returns the day-of-year part of this DateTime. The returned value
        // is an integer between 1 and 366.
        //
        int dayOfYear() const;

        // Returns the year part of this DateTime. The returned value is an
        // integer between 1 and 9999.
        //
        int year() const;

        // Returns the day-of-month part of this DateTime. The returned
        // value is an integer between 1 and 31.
        //
        int day() const;

        // Returns the hour part of this DateTime. The returned value is an
        // integer between 0 and 23.
        //
        int hour() const;

        // Returns the minute part of this DateTime. The returned value is
        // an integer between 0 and 59.
        //
        int minute() const;

        // Returns the date part of this DateTime. The resulting value
        // corresponds to this DateTime with the time-of-day part set to
        // zero (midnight).
        //
        DateTime date() const;

        Kind kind() const;

        // Returns the millisecond part of this DateTime. The returned value
        // is an integer between 0 and 999.
        //
        int millisecond() const;

        // Returns the month part of this DateTime. The returned value is an
        // integer between 1 and 12.
        //
        int month() const;

        // Returns the second part of this DateTime. The returned value is
        // an integer between 0 and 59.
        //
        int second() const;

        // Returns the tick count for this DateTime. The returned value is
        // the number of 100-nanosecond intervals that have elapsed since 1/1/0001
        // 12:00am.
        //
        uint64_t ticks() const;

        // Returns the time-of-day part of this DateTime. The returned value
        // is a TimeSpan that indicates the time elapsed since midnight.
        //
        TimeSpan timeOfDay() const;

        double total1970Milliseconds() const;

        double total2010Milliseconds() const;

        void writeBCDDateTime(Stream *stream, bool includedSec = true, bool includedMs = false) const;

        void readBCDDateTime(Stream *stream, bool includedSec = true, bool includedMs = false);

        void writeBCDDate(Stream *stream) const;

        void readBCDDate(Stream *stream);

        void writeBCDTime(Stream *stream) const;

        void readBCDTime(Stream *stream);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        void writeMilliseconds(Stream *stream, bool bigEndian = true) const;

        void readMilliseconds(Stream *stream, bool bigEndian = true);

        DateTime &operator=(const DateTime &value);

        DateTime operator+(const TimeSpan &value) const;

        DateTime operator+=(const TimeSpan &value);

        DateTime operator-(const TimeSpan &value) const;

        TimeSpan operator-(const DateTime &value) const;

        DateTime operator-=(const TimeSpan &value);

        // Returns the DateTime resulting from adding the given
        // TimeSpan to this DateTime.
        //
        DateTime add(const TimeSpan &value) const;

        // Returns the DateTime resulting from adding a fractional number of
        // time units to this DateTime.
        DateTime add(double value, int scale) const;

        // Returns the DateTime resulting from adding a fractional number of
        // days to this DateTime. The result is computed by rounding the
        // fractional number of days given by value to the nearest
        // millisecond, and adding that interval to this DateTime. The
        // value argument is permitted to be negative.
        //
        DateTime addDays(double value) const;

        // Returns the DateTime resulting from adding a fractional number of
        // hours to this DateTime. The result is computed by rounding the
        // fractional number of hours given by value to the nearest
        // millisecond, and adding that interval to this DateTime. The
        // value argument is permitted to be negative.
        //
        DateTime addHours(double value) const;

        // Returns the DateTime resulting from the given number of
        // milliseconds to this DateTime. The result is computed by rounding
        // the number of milliseconds given by value to the nearest integer,
        // and adding that interval to this DateTime. The value
        // argument is permitted to be negative.
        //
        DateTime addMilliseconds(double value) const;

        // Returns the DateTime resulting from adding a fractional number of
        // minutes to this DateTime. The result is computed by rounding the
        // fractional number of minutes given by value to the nearest
        // millisecond, and adding that interval to this DateTime. The
        // value argument is permitted to be negative.
        //
        DateTime addMinutes(double value) const;

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
        DateTime addMonths(int months) const;

        // Returns the DateTime resulting from adding a fractional number of
        // seconds to this DateTime. The result is computed by rounding the
        // fractional number of seconds given by value to the nearest
        // millisecond, and adding that interval to this DateTime. The
        // value argument is permitted to be negative.
        //
        DateTime addSeconds(double value) const;

        // Returns the DateTime resulting from adding the given number of
        // 100-nanosecond ticks to this DateTime. The value argument
        // is permitted to be negative.
        //
        DateTime addTicks(int64_t value) const;

        // Returns the DateTime resulting from adding the given number of
        // years to this DateTime. The result is computed by incrementing
        // (or decrementing) the year part of this DateTime by value
        // years. If the month and day of this DateTime is 2/29, and if the
        // resulting year is not a leap year, the month and day of the resulting
        // DateTime becomes 2/28. Otherwise, the month, day, and time-of-day
        // parts of the result are the same as those of this DateTime.
        //
        DateTime addYears(int value) const;

        uint32_t subtract(DateTime prev, Resolutions tr) const;

        DateTime add(uint32_t time, Resolutions tr) const;

    public:
        static DateTime now();

        static DateTime utcNow();

        // Returns a DateTime representing the current date. The date part
        // of the returned value is the current date, and the time-of-day part of
        // the returned value is zero (midnight).
        //
        static DateTime today();

        static bool parse(const String &str, DateTime &dateTime);

        // Checks whether a given year is a leap year. This method returns true if
        // year is a leap year, or false if not.
        //
        static bool isLeapYear(int year);

        // Returns the number of days in the month given by the year and
        // month arguments.
        //
        static int daysInMonth(int year, int month);

        static Resolutions fromResolutionStr(const String &str);

        static String toResolutionStr(Resolutions tr);

        static DateTime fromLocalTime(time_t time);

        static time_t toLocalTime(const DateTime &time);

        static double total1970Milliseconds(const DateTime &time);

        static DateTime from1970Milliseconds(double time, bool utc = true);

        static double total2010Milliseconds(const DateTime &time);

        static DateTime from2010Milliseconds(double time, bool utc = true);

        static bool isValid(int year, int month, int day);

    private:
        // Returns a given date part of this DateTime. This method is used
        // to compute the year, day-of-year, month, or day part.
        int getDatePart(int part) const;

        uint64_t internalTicks() const;

        uint64_t internalKind() const;

    private:
        static uint64_t dateToTicks(int year, int month, int day);

        static uint64_t timeToTicks(int hour, int minute, int second);

        static uint64_t toTicks(int year, int month, int day, int hour = 0, int minute = 0, int second = 0,
                                int millisecond = 0, Kind kind = Unspecified);

    public:
        static const DateTime MinValue;
        static const DateTime MaxValue;
        static const DateTime Min2010Value;
        static const DateTime UtcPoint1900;
        static const DateTime UtcPoint1970;
        static const DateTime UtcPoint2010;

    private:
        friend class DateTimeFormat;

        // The data is stored as an unsigned 64-bit integer
        //   Bits 01-62: The value of 100-nanosecond ticks where 0 represents 1/1/0001 12:00am, up until the value
        //               12/31/9999 23:59:59.9999999
        //   Bits 63-64: A four-state value that describes the DateTimeKind value of the date time, with a 2nd
        //               value for the rare case where the date time is local, but is in an overlapped daylight
        //               savings time hour and it is in daylight savings time. This allows distinction of these
        //               otherwise ambiguous local times and prevents data loss when round tripping from Local to
        //               UTC time.
        uint64_t _dateData;

    private:
        // Number of 100ns ticks per time unit
        static const int64_t TicksPerMillisecond = 10000;
        static const int64_t TicksPerSecond = TicksPerMillisecond * 1000;
        static const int64_t TicksPerMinute = TicksPerSecond * 60;
        static const int64_t TicksPerHour = TicksPerMinute * 60;
        static const int64_t TicksPerDay = TicksPerHour * 24;

        // Number of milliseconds per time unit
        static const int MillisPerSecond = 1000;
        static const int MillisPerMinute = MillisPerSecond * 60;
        static const int MillisPerHour = MillisPerMinute * 60;
        static const int MillisPerDay = MillisPerHour * 24;

        // Number of days in a non-leap year
        static const int DaysPerYear = 365;
        // Number of days in 4 years
        static const int DaysPer4Years = DaysPerYear * 4 + 1;       // 1461
        // Number of days in 100 years
        static const int DaysPer100Years = DaysPer4Years * 25 - 1;  // 36524
        // Number of days in 400 years
        static const int DaysPer400Years = DaysPer100Years * 4 + 1; // 146097

        // Number of days from 1/1/0001 to 12/31/1600
        static const int DaysTo1601 = DaysPer400Years * 4;          // 584388
        // Number of days from 1/1/0001 to 12/30/1899
        static const int DaysTo1899 = DaysPer400Years * 4 + DaysPer100Years * 3 - 367;
        // Number of days from 1/1/0001 to 12/31/9999
        static const int DaysTo10000 = DaysPer400Years * 25 - 366;  // 3652059

        static const int64_t MinTicks = 0;
        static const int64_t MaxTicks = DaysTo10000 * TicksPerDay - 1;
        static const int64_t MaxMillis = (int64_t) DaysTo10000 * MillisPerDay;

        static const int64_t FileTimeOffset = DaysTo1601 * TicksPerDay;
        static const int64_t DoubleDateOffset = DaysTo1899 * TicksPerDay;
        // The minimum OA date is 0100/01/01 (Note it's year 100).
        // The maximum OA date is 9999/12/31
        static const int64_t OADateMinAsTicks = (DaysPer100Years - DaysPerYear) * TicksPerDay;
        // All OA dates must be greater than (not >=) OADateMinAsDouble
        static const int64_t OADateMinAsDouble = -657435;
        // All OA dates must be less than (not <=) OADateMaxAsDouble
        static const int64_t OADateMaxAsDouble = 2958466;

        static const int DatePartYear = 0;
        static const int DatePartDayOfYear = 1;
        static const int DatePartMonth = 2;
        static const int DatePartDay = 3;

        static const int DaysToMonth365[13];
        static const int DaysToMonth366[13];

        static const uint64_t TicksMask = 0x3FFFFFFFFFFFFFFF;
        static const uint64_t FlagsMask = 0xC000000000000000;
        static const uint64_t LocalMask = 0x8000000000000000;
        static const int64_t TicksCeiling = 0x4000000000000000;
        static const uint64_t KindUnspecified = 0x0000000000000000;
        static const uint64_t KindUtc = 0x4000000000000000;
        static const uint64_t KindLocal = 0x8000000000000000;
        static const uint64_t KindLocalAmbiguousDst = 0xC000000000000000;
        static const int KindShift = 62;
    };
}

#endif // DateTime_h

