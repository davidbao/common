#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <stdio.h>
#include <limits.h>
#include "IO/Stream.h"
#include "DateTime.h"

namespace Common
{
    class LocalTime;
	struct TimeZone
	{
	public:
        enum Type : uint8_t
        {
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
        
        TimeZone(int64_t offset = -1);
        TimeZone(Type type);
        TimeZone(const TimeZone& timeZone);
        
        DateTime toUtcTime(DateTime time) const;
        DateTime toLocalTime(DateTime time) const;
        
        void operator=(const TimeZone& tz);
        bool operator==(const TimeZone& tz) const;
        bool operator!=(const TimeZone& tz) const;
        
        void write(Stream* stream, bool bigEndian = true) const;
        void read(Stream* stream, bool bigEndian = true);

        void writeSeconds(Stream* stream, bool bigEndian = true) const;
        void readSeconds(Stream* stream, bool bigEndian = true);
        
        const String toString() const;
        bool isEmpty() const;
        
        int64_t ticksOffset() const;
        
        Type type() const;
        const String toTypeStr() const;
       
    public:
        static bool parseTypeStr(const String& str, Type& type);
        static const String toTypeStr(Type type);
        
    private:
        int localTimeValue() const;
        int hourOffset() const;
        
        void update();
        static void updateLocal(const TimeZone& tz);
        
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
        const double SecondsPerTick = 1.0 / TicksPerSecond;         // 0.0001
        
    private:
        static const String FileNames[Count];
        static const int64_t Offsets[Count];
	};
}

#endif // TIMEZONE_H

