//
//  TimeZone.h
//  common
//
//  Created by baowei on 2016/10/19.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef TimeZone_h
#define TimeZone_h

#include "IO/Stream.h"
#include "DateTime.h"

namespace Common {
    class LocalTime;

    struct TimeZone : public IEquatable<TimeZone>, public IEvaluation<TimeZone>, public IComparable<TimeZone> {
    public:
        enum Type : uint8_t {
            GMT_B1200 = 0,
            GMT_B1100,
            GMT_B1000,
            GMT_B0900,
            GMT_B0800,
            GMT_B0700,
            GMT_B0600,
            GMT_B0500,
            GMT_B0400,
            GMT_B0330,
            GMT_B0300,
            GMT_B0200,
            GMT_B0100,
            GMT,
            GMT_A0100,
            GMT_A0200,
            GMT_A0300,
            GMT_A0330,
            GMT_A0400,
            GMT_A0430,
            GMT_A0500,
            GMT_A0530,
            GMT_A0545,
            GMT_A0600,
            GMT_A0630,
            GMT_A0700,
            GMT_A0800,
            GMT_A0900,
            GMT_A0930,
            GMT_A1000,
            GMT_A1100,
            GMT_A1200,
            GMT_A1300,
            Count = GMT_A1300 + 1
        };

        explicit TimeZone(int64_t offset = -1);

        explicit TimeZone(Type type);

        TimeZone(const TimeZone &timeZone);

        ~TimeZone() override;

        bool equals(const TimeZone &other) const override;

        void evaluates(const TimeZone &other) override;

        int compareTo(const TimeZone &other) const override;

        String toString() const;

        bool isEmpty() const;

        TimeZone &operator=(const TimeZone &tz);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        void writeSeconds(Stream *stream, bool bigEndian = true) const;

        void readSeconds(Stream *stream, bool bigEndian = true);

        DateTime toUtcTime(const DateTime &time) const;

        DateTime toLocalTime(const DateTime &time) const;

        int64_t ticksOffset() const;

        double hourOffset() const;

        double minuteOffset() const;

        double secondOffset() const;

        TimeSpan offset() const;

        Type type() const;

        String toTypeStr() const;

    public:
        static TimeSpan getUtcOffset(const DateTime &dateTime);

        static bool parse(const String &str, TimeZone &tz);

    private:
        int localTimeValue() const;

        void update();

    private:
        static bool parseTypeStr(const String &str, Type &type);

        static String toTypeStr(Type type);

        static void updateLocal(const TimeZone &tz);

    public:
        static TimeZone Local;
        static TimeZone Empty;

    private:
        friend LocalTime;

        int64_t _ticksOffset;

        static const int64_t TicksPerMillisecond = 10000;
        static const int64_t TicksPerSecond = TicksPerMillisecond * 1000;   // 10,000,000
        static const int64_t TicksPerMinute = TicksPerSecond * 60;         // 600,000,000
        static const int64_t TicksPerHour = TicksPerMinute * 60;
        static constexpr double SecondsPerTick = 1.0 / TicksPerSecond;         // 0.0001

    private:
        static const String FileNames[Count];
        static const int64_t Offsets[Count];
    };
}

#endif // TimeZone_h

