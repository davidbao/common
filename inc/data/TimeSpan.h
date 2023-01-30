//
//  TimeSpan.h
//  common
//
//  Created by baowei on 2015/8/7.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef TimeSpan_h
#define TimeSpan_h

#include <climits>
#include "data/String.h"
#include "IO/Stream.h"
#include "data/Culture.h"

namespace Data {
    class StringArray;

    class DateTimeFormat;

    struct TimeSpan : public IEquatable<TimeSpan>, public IEvaluation<TimeSpan>, public IComparable<TimeSpan> {
    public:
        TimeSpan(int64_t ticks = 0);

        TimeSpan(int hours, int minutes, int seconds);

        TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds = 0);

        TimeSpan(const TimeSpan &timeSpan);

        ~TimeSpan() override;

        bool equals(const TimeSpan &other) const override;

        void evaluates(const TimeSpan &other) override;

        int compareTo(const TimeSpan &other) const override;

        String toString(const String &format = String::Empty,
                        const IFormatProvider<DateTimeFormatInfo> *provider = nullptr) const;

        int days() const;

        int hours() const;

        int minutes() const;

        int seconds() const;

        int milliseconds() const;

        double totalMilliseconds() const;

        double totalSeconds() const;

        double totalDays() const;

        double totalHours() const;

        double totalMinutes() const;

        int64_t ticks() const;

        TimeSpan &operator=(const TimeSpan &ts);

        TimeSpan operator+(const TimeSpan &ts) const;

        TimeSpan operator+=(const TimeSpan &ts);

        TimeSpan operator-(const TimeSpan &ts) const;

        TimeSpan operator-=(const TimeSpan &ts);

        TimeSpan operator+() const;

        TimeSpan operator-() const;

        void writeBCDTime(Stream *stream) const;

        void readBCDTime(Stream *stream);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        TimeSpan add(const TimeSpan &ts) const;

        TimeSpan subtract(const TimeSpan &ts) const;

        TimeSpan negate() const;

    public:
        static bool
        parse(const char *str, TimeSpan &timeSpan, const IFormatProvider<DateTimeFormatInfo> *provider = nullptr);

        static bool
        parse(const String &str, TimeSpan &timeSpan, const IFormatProvider<DateTimeFormatInfo> *provider = nullptr);

        static TimeSpan fromDays(double value);

        static TimeSpan fromHours(double value);

        static TimeSpan fromMinutes(double value);

        static TimeSpan fromSeconds(double value);

        static TimeSpan fromMilliseconds(double value);

        static TimeSpan fromTicks(time_t value);

    private:
        static int64_t timeToTicks(int hours, int minutes, int seconds);

        static int64_t timeToTicks(int days, int hours, int minutes, int seconds, int milliseconds = 0);

        static TimeSpan interval(double value, int scale);

    public:
        static const TimeSpan Zero;

        static const TimeSpan MaxValue;
        static const TimeSpan MinValue;

    private:
        friend struct DateTime;

        friend struct TimeSpanFormat;

        friend class TimeSpanParse;

        friend class DateTimeFormat;

        int64_t _ticks;

    private:
        static const int64_t TicksPerMillisecond = 10000;
        static const int64_t TicksPerSecond = TicksPerMillisecond * 1000;   // 10,000,000
        static const int64_t TicksPerMinute = TicksPerSecond * 60;         // 600,000,000
        static const int64_t TicksPerHour = TicksPerMinute * 60;        // 36,000,000,000
        static const int64_t TicksPerDay = TicksPerHour * 24;          // 864,000,000,000

        static constexpr double MillisecondsPerTick = 1.0 / TicksPerMillisecond;
        static constexpr double SecondsPerTick = 1.0 / TicksPerSecond;         // 0.0001
        static constexpr double MinutesPerTick = 1.0 / TicksPerMinute; // 1.6666666666667e-9
        static constexpr double HoursPerTick = 1.0 / TicksPerHour; // 2.77777777777777778e-11
        static constexpr double DaysPerTick = 1.0 / TicksPerDay; // 1.1574074074074074074e-12

        static const int MillisPerSecond = 1000;
        static const int MillisPerMinute = MillisPerSecond * 60; //     60,000
        static const int MillisPerHour = MillisPerMinute * 60;   //  3,600,000
        static const int MillisPerDay = MillisPerHour * 24;      // 86,400,000

        static const int64_t MaxSeconds = LLONG_MAX / TicksPerSecond;
        static const int64_t MinSeconds = LLONG_MIN / TicksPerSecond;

        static const int64_t MaxMilliSeconds = LLONG_MAX / TicksPerMillisecond;
        static const int64_t MinMilliSeconds = LLONG_MIN / TicksPerMillisecond;

        static const int64_t TicksPerTenthSecond = TicksPerMillisecond * 100;
    };
}

#endif // TimeSpan_h

