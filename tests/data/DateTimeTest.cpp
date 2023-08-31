//
//  DateTimeTest.cpp
//  common
//
//  Created by baowei on 2022/11/24.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/DateTime.h"
#include "IO/MemoryStream.h"

using namespace Data;

bool testConstructor() {
    {
        DateTime test;
        if (test != DateTime::MinValue) {
            return false;
        }
    }
    {
        DateTime test(0, DateTime::Utc);
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test(0, DateTime::Local);
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 2);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2)) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, 3, 4, 5, 6);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2 && test.hour() == 3 && test.minute() == 4 &&
              test.second() == 5 && test.millisecond() == 6)) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, 3, 4, 5, 6, DateTime::Utc);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2 && test.hour() == 3 && test.minute() == 4 &&
              test.second() == 5 && test.millisecond() == 6)) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, 3, 4, 5, 6, DateTime::Local);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2 && test.hour() == 3 && test.minute() == 4 &&
              test.second() == 5 && test.millisecond() == 6)) {
            return false;
        }
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 2);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2)) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, DateTime::Utc);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2)) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, DateTime::Local);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2)) {
            return false;
        }
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 2, 3, 4, 5, 6, DateTime::Local);
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 2 && test.hour() == 3 && test.minute() == 4 &&
              test.second() == 5 && test.millisecond() == 6)) {
            return false;
        }
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        DateTime test(2010, 1, 2);
        DateTime test2(2010, 1, 2);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2);
        DateTime test2(2010, 2, 2);
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 2);
        DateTime test2(2010, 1, 2);
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2);
        DateTime test2(2010, 1, 2);
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        DateTime test(2010, 1, 2);
        DateTime test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2);
        DateTime test2;
        test2 = test;
        if (!test.equals(test2)) {
            return false;
        }
    }

    return true;
}

bool testComparison() {
    {
        DateTime test(1, 2, 3);
        DateTime test2(2, 2, 3);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        DateTime test(1, 2, 3);
        DateTime test2(2, 2, 3);
        if (test2 <= test) {
            return false;
        }
    }
    {
        DateTime test(1, 2, 3);
        DateTime test2(2, 2, 3);
        if (test2 < test) {
            return false;
        }
    }
    {
        DateTime test(1, 2, 3);
        DateTime test2(2, 2, 3);
        if (!(test2 >= test)) {
            return false;
        }
    }
    {
        DateTime test(1, 2, 3);
        DateTime test2(2, 2, 3);
        if (!(test2 > test)) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        DateTime test(2010, 1, 2, 3, 4, 5);
        if (test.dayOfWeek() != DayOfWeek::Saturday) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        if (test.dayOfYear() != 61) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, 3, 4, 5, 6, DateTime::Local);
        String str = test.timeOfDay().toString();
        if (test.timeOfDay() != TimeSpan(0, 3, 4, 5, 6)) {
            return false;
        }
    }
    {
        DateTime test(1, 1, 1, 3, 4, 5);
        if (test.ticks() != ((int64_t)3 * 3600 + 4 * 60 + 5) * 10 * 1000 * 1000) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 2, 3, 4, 5);
        DateTime date = test.date();
        if (!(date.year() == test.year() && date.month() == test.month() && date.day() == test.day())) {
            return false;
        }
    }

    {
        DateTime test(1970, 1, 1, 3, 4, 5);
        if (test.total1970Milliseconds() != (3 * 3600 + 4 * 60 + 5) * 1000L) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 1, 3, 4, 5);
        if (test.total2010Milliseconds() != (3 * 3600 + 4 * 60 + 5) * 1000L) {
            return false;
        }
    }

    return true;
}

bool testStream() {
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, true, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readBCDDateTime(&stream, true, true);
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, true, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readBCDDateTime(&stream, true, false);
        if (test.addMilliseconds(-test.millisecond()) != test2) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, false, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readBCDDateTime(&stream, false, false);
        if (test.addMilliseconds(-(test.second() * 1000 + test.millisecond())) != test2) {
            return false;
        }
    }

    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, true, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        int year = stream.readInt16();
        if (year != 0x2018) {
            return false;
        }
        auto month = stream.readInt8();
        if (month != 0x12) {
            return false;
        }
        auto day = stream.readInt8();
        if (day != 0x30) {
            return false;
        }
        auto hour = stream.readInt8();
        if (hour != 0x23) {
            return false;
        }
        auto minute = stream.readInt8();
        if (minute != 0x14) {
            return false;
        }
        auto second = stream.readInt8();
        if (second != 0x25) {
            return false;
        }
        auto ms = stream.readInt16();
        if (ms != 0x646) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, true, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        int year = stream.readInt16();
        if (year != 0x2018) {
            return false;
        }
        auto month = stream.readInt8();
        if (month != 0x12) {
            return false;
        }
        auto day = stream.readInt8();
        if (day != 0x30) {
            return false;
        }
        auto hour = stream.readInt8();
        if (hour != 0x23) {
            return false;
        }
        auto minute = stream.readInt8();
        if (minute != 0x14) {
            return false;
        }
        auto second = stream.readInt8();
        if (second != 0x25) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDateTime(&stream, false, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        auto year = stream.readInt16();
        if (year != 0x2018) {
            return false;
        }
        auto month = stream.readInt8();
        if (month != 0x12) {
            return false;
        }
        auto day = stream.readInt8();
        if (day != 0x30) {
            return false;
        }
        auto hour = stream.readInt8();
        if (hour != 0x23) {
            return false;
        }
        auto minute = stream.readInt8();
        if (minute != 0x14) {
            return false;
        }
    }

    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDate(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readBCDDate(&stream);
        if (test.date() != test2) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDDate(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        auto year = stream.readInt16();
        if (year != 0x2018) {
            return false;
        }
        auto month = stream.readInt8();
        if (month != 0x12) {
            return false;
        }
        auto day = stream.readInt8();
        if (day != 0x30) {
            return false;
        }
    }

    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDTime(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readBCDTime(&stream);
        if (!(test.hour() == test2.hour() && test.minute() == test2.minute() && test.second() == test2.second())) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeBCDTime(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        auto hour = stream.readInt8();
        if (hour != 0x23) {
            return false;
        }
        auto minute = stream.readInt8();
        if (minute != 0x14) {
            return false;
        }
        auto second = stream.readInt8();
        if (second != 0x25) {
            return false;
        }
    }

    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeMilliseconds(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readMilliseconds(&stream, true);
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(2018, 12, 30, 23, 14, 25, 646);
        MemoryStream stream;
        test.writeMilliseconds(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        DateTime test2;
        test2.readMilliseconds(&stream, false);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testOperator() {
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test + TimeSpan(1, 2, 3);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 4 &&
              test2.minute() == 6 &&
              test2.second() == 8)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        test += TimeSpan(1, 2, 3);
        if (!(test.year() == 2010 && test.month() == 3 && test.day() == 2 && test.hour() == 4 && test.minute() == 6 &&
              test.second() == 8)) {
            return false;
        }
    }

    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test - TimeSpan(1, 2, 3);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 2 &&
              test2.minute() == 2 &&
              test2.second() == 2)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        test -= TimeSpan(1, 2, 3);
        if (!(test.year() == 2010 && test.month() == 3 && test.day() == 2 && test.hour() == 2 && test.minute() == 2 &&
              test.second() == 2)) {
            return false;
        }
    }

    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2(2010, 3, 1, 3, 4, 5);
        TimeSpan test3 = test - test2;
        if (test3 != TimeSpan(1, 0, 0, 0)) {
            return false;
        }
    }

    return true;
}

bool testAdd() {
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.add(TimeSpan(1, 0, 0));
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 4 &&
              test2.minute() == 4 &&
              test2.second() == 5)) {
            return false;
        }
    }

    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addYears(1);
        if (!(test2.year() == 2011 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 3 &&
              test2.minute() == 4 &&
              test2.second() == 5)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addMonths(1);
        if (!(test2.year() == 2010 && test2.month() == 4 && test2.day() == 2 && test2.hour() == 3 &&
              test2.minute() == 4 &&
              test2.second() == 5)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addDays(1);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 3 && test2.hour() == 3 &&
              test2.minute() == 4 &&
              test2.second() == 5)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addHours(1);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 4 &&
              test2.minute() == 4 &&
              test2.second() == 5)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addMinutes(58);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 4 &&
              test2.minute() == 2 &&
              test2.second() == 5)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addSeconds(1);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 3 &&
              test2.minute() == 4 &&
              test2.second() == 6)) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2 = test.addTicks(1L * 10 * 1000 * 1000);
        if (!(test2.year() == 2010 && test2.month() == 3 && test2.day() == 2 && test2.hour() == 3 &&
              test2.minute() == 4 &&
              test2.second() == 6)) {
            return false;
        }
    }

    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2(2010, 3, 2, 3, 3, 5);
        uint32_t time = test.subtract(test2, DateTime::ResSecond);
        if (time != 60) {
            return false;
        }
    }
    {
        DateTime test(2010, 3, 2, 3, 4, 5);
        DateTime test2(2010, 3, 2, 3, 3, 5);
        uint32_t time = test.subtract(test2, DateTime::ResMillisecond);
        if (time != 60 * 1000) {
            return false;
        }
    }

    {
        DateTime test(2010, 3, 2, 3, 3, 5);
        DateTime test2 = test.add(60, DateTime::ResSecond);
        if (test2.minute() != 4) {
            return false;
        }
    }

    return true;
}

bool testStaticProperty() {
    {
        DateTime test = DateTime::MinValue;
        if (!(test.year() == 1 && test.month() == 1 && test.day() == 1)) {
            return false;
        }
    }
    {
        DateTime test = DateTime::MaxValue;
        if (!(test.year() == 9999 && test.month() == 12 && test.day() == 31)) {
            return false;
        }
    }
    {
        DateTime test = DateTime::Min2010Value;
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 1)) {
            return false;
        }
    }
    {
        DateTime test = DateTime::UtcPoint1900;
        if (!(test.year() == 1900 && test.month() == 1 && test.day() == 1)) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test = DateTime::UtcPoint1970;
        if (!(test.year() == 1970 && test.month() == 1 && test.day() == 1)) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test = DateTime::UtcPoint2010;
        if (!(test.year() == 2010 && test.month() == 1 && test.day() == 1)) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }

    return true;
}

bool testStatic() {
    {
        DateTime test = DateTime::now();
        if (test == DateTime::MinValue) {
            return false;
        }
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }
    {
        DateTime test = DateTime::utcNow();
        if (test == DateTime::MinValue) {
            return false;
        }
        if (test.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        DateTime test = DateTime::today();
        if (test != DateTime::now().date()) {
            return false;
        }
        if (test.kind() != DateTime::Local) {
            return false;
        }
    }

    {
        if (!DateTime::isLeapYear(2012)) {
            return false;
        }
        if (DateTime::isLeapYear(2011)) {
            return false;
        }
    }

    {
        if (DateTime::fromResolutionStr("second") != DateTime::ResSecond) {
            return false;
        }
        if (DateTime::fromResolutionStr("millisecond") != DateTime::ResMillisecond) {
            return false;
        }
        if (DateTime::fromResolutionStr("hour") != DateTime::ResHour) {
            return false;
        }
        if (DateTime::fromResolutionStr("minute") != DateTime::ResMinute) {
            return false;
        }
        if (DateTime::fromResolutionStr("xxx") != DateTime::ResSecond) {
            return false;
        }
    }
    {
        if (!String::equals(DateTime::toResolutionStr(DateTime::ResSecond), "second", true)) {
            return false;
        }
        if (!String::equals(DateTime::toResolutionStr(DateTime::ResMillisecond), "millisecond", true)) {
            return false;
        }
        if (!String::equals(DateTime::toResolutionStr(DateTime::ResHour), "hour", true)) {
            return false;
        }
        if (!String::equals(DateTime::toResolutionStr(DateTime::ResMinute), "minute", true)) {
            return false;
        }
    }

    {
        DateTime test(1970, 1, 1, 3, 4, 5);
        DateTime test2 = DateTime::from1970Milliseconds(test.total1970Milliseconds());
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(1970, 1, 20, 15, 41, 12);
        DateTime test2 = DateTime::from1970Milliseconds(1669272000, false);
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(1970, 1, 20, 7, 41, 12, 0, DateTime::Utc);
        DateTime test2 = DateTime::from1970Milliseconds(1669272000, true);
        if (test != test2) {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 1, 3, 4, 5);
        DateTime test2 = DateTime::from2010Milliseconds(test.total2010Milliseconds());
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 20, 15, 41, 12);
        DateTime test2 = DateTime::from2010Milliseconds(1669272000, false);
        if (test != test2) {
            return false;
        }
    }
    {
        DateTime test(2010, 1, 20, 7, 41, 12, 0, DateTime::Utc);
        DateTime test2 = DateTime::from2010Milliseconds(1669272000, true);
        if (test != test2) {
            return false;
        }
    }

    {
        if (!DateTime::isValid(2010, 1, 20)) {
            return false;
        }
        if (DateTime::isValid(2010, 2, 31)) {
            return false;
        }
        if (DateTime::isValid(2010, 4, 31)) {
            return false;
        }
    }

    return true;
}

bool testToString() {
    {
        DateTime test(2010, 1, 20, 15, 41, 12);
        String formats[] = {"d", "D", "f", "F",
                            "g", "G", "m", "o",
                            "r","s", "t", "T",
                            "u", "U", "Y"};
        String expects[] = {"2010-01-20", "2010-01-20", "2010-01-20 15:41", "2010-01-20 15:41:12",
                            "2010-01-20 15:41", "2010-01-20 15:41:12", "01-20", "2010-01-20T15:41:12.0000000",
                            "Wed, 20 Jan 2010 15:41:12 GMT", "2010-01-20T15:41:12", "15:41", "15:41:12",
                            "2010-01-20 15:41:12Z", "2010-01-20 15:41:12", "2010-01"};
        for (int i = 0; i < 15; ++i) {
            String str = test.toString(formats[i]);
            if (str != expects[i]) {
                printf("toString test error, format: %s\n", formats[i].c_str());
                return false;
            }
        }
    }

    // d
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if (test.toString("d, M") != "29, 8") {
            return false;
        }
    }
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if (test.toString("d MMMM") != "29 August") {
            return false;
        }
    }

    // dd
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if (test.toString("dd, MM") != "29, 08") {
            return false;
        }
    }

    // ddd
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        String str = test.toString("ddd d MMM");
        if (test.toString("ddd d MMM") != "Fri 29 Aug") {
            return false;
        }
    }

    // dddd
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if (test.toString("dddd dd MMMM") != "Friday 29 August") {
            return false;
        }
    }

    // f & F
    {
        DateTime test(2008, 8, 29, 19, 27, 15, 18);

        if(test.toString("hh:mm:ss.f") != "07:27:15.0") {
            return false;
        }
        if(test.toString("hh:mm:ss.F") != "07:27:15") {
            return false;
        }
        if(test.toString("hh:mm:ss.ff") != "07:27:15.01") {
            return false;
        }
        if(test.toString("hh:mm:ss.FF") != "07:27:15.01") {
            return false;
        }
        if(test.toString("hh:mm:ss.fff") != "07:27:15.018") {
            return false;
        }
        if(test.toString("hh:mm:ss.FFF") != "07:27:15.018") {
            return false;
        }
        if(test.toString("hh:mm:ss.ffff") != "07:27:15.0180") {
            return false;
        }
        if(test.toString("hh:mm:ss.FFFF") != "07:27:15.018") {
            return false;
        }
        if(test.toString("hh:mm:ss.fffff") != "07:27:15.01800") {
            return false;
        }
        if(test.toString("hh:mm:ss.FFFFF") != "07:27:15.018") {
            return false;
        }
        if(test.toString("hh:mm:ss.ffffff") != "07:27:15.018000") {
            return false;
        }
        if(test.toString("hh:mm:ss.FFFFFF") != "07:27:15.018") {
            return false;
        }
        if(test.toString("hh:mm:ss.fffffff") != "07:27:15.0180000") {
            return false;
        }
        if(test.toString("hh:mm:ss.FFFFFFF") != "07:27:15.018") {
            return false;
        }
    }

    // h
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("h:m:s.F t") != "6:9:1 P") {
            return false;
        }
    }

    // hh
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("hh:mm:ss tt") != "06:09:01 PM") {
            return false;
        }
    }

    // H
    {
        DateTime test(2008, 1, 1, 6, 9, 1);
        String str = test.toString("H:mm:ss");
        if(test.toString("H:mm:ss") != "6:09:01") {
            return false;
        }
    }

    // HH
    {
        DateTime test(2008, 1, 1, 6, 9, 1);
        if(test.toString("HH:mm:ss") != "06:09:01") {
            return false;
        }
    }

    // m
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("h:m:s.F t") != "6:9:1 P") {
            return false;
        }
    }

    // mm
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("hh:mm:ss tt") != "06:09:01 PM") {
            return false;
        }
    }

    // M
    {
        DateTime test(2008, 8, 18);
        if(test.toString("(M) MMM, MMMM") != "(8) Aug, August") {
            return false;
        }
    }

    // MM
    {
        DateTime test(2008, 1, 2, 6, 30, 15);
        if(test.toString("dd, MM") != "02, 01") {
            return false;
        }
    }

    // MMM
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if(test.toString("ddd d MMM") != "Fri 29 Aug") {
            return false;
        }
    }

    // MMMM
    {
        DateTime test(2008, 8, 29, 19, 27, 15);
        if(test.toString("dddd dd MMMM") != "Friday 29 August") {
            return false;
        }
    }

    // s
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("h:m:s.F t") != "6:9:1 P") {
            return false;
        }
    }

    // ss
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("hh:mm:ss tt") != "06:09:01 PM") {
            return false;
        }
    }

    // t
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("h:m:s.F t") != "6:9:1 P") {
            return false;
        }
    }

    // tt
    {
        DateTime test(2008, 1, 1, 18, 9, 1);
        if(test.toString("hh:mm:ss tt") != "06:09:01 PM") {
            return false;
        }
    }

    // y
    {
        DateTime date1(1, 12, 1);
        DateTime date2(2010, 1, 1);
        if(date1.toString("%y") != "1") {
            return false;
        }
        if(date1.toString("yy") != "01") {
            return false;
        }
        if(date1.toString("yyy") != "001") {
            return false;
        }
        if(date1.toString("yyyy") != "0001") {
            return false;
        }
        if(date1.toString("yyyyy") != "00001") {
            return false;
        }
        if(date2.toString("%y") != "10") {
            return false;
        }
        if(date2.toString("yy") != "10") {
            return false;
        }
        if(date2.toString("yyy") != "2010") {
            return false;
        }
        if(date2.toString("yyyy") != "2010") {
            return false;
        }
        if(date2.toString("yyyyy") != "02010") {
            return false;
        }
    }

    {
        DateTime test(2010, 1, 20, 15, 41, 12);
        if(test.toString("yyyy-MM-dd HH:mm:ss.fff") != "2010-01-20 15:41:12.000") {
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        DateTime test;
        if(!DateTime::parse("2010-01-20 15:41:12.000", test)) {
            return false;
        }
        DateTime test2(2010, 1, 20, 15, 41, 12);
        if(test != test2) {
            return false;
        }
    }

    {
        DateTime test;
        if(!DateTime::parse("6.00:00:36", test)) {
            return false;
        }
        if (!(test.day() == 6 && test.hour() == 0 && test.minute() == 0 && test.second() == 36)) {
            return false;
        }
    }

    {
        DateTime test;
        if(!DateTime::parse("06.00:00:36", test)) {
            return false;
        }
//        printf("time: %s\n", test.toString().c_str());
        if (!(test.day() == 6 && test.hour() == 0 && test.minute() == 0 && test.second() == 36)) {
            return false;
        }
    }

#ifndef __EMSCRIPTEN__
    {
        DateTime test;
        if(DateTime::parse("32.00:00:36", test)) {
            return false;
        }
    }
#endif

    {
        DateTime test;
        if(!DateTime::parse("00:00:36", test)) {
            return false;
        }
        if (!(test.hour() == 0 && test.minute() == 0 && test.second() == 36)) {
            return false;
        }
    }

    {
        DateTime test;
        if(!DateTime::parse("00:36", test)) {
            return false;
        }
        if (!(test.hour() == 0 && test.minute() == 36 && test.second() == 0)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testEquals()) {
        return 2;
    }
    if (!testEvaluates()) {
        return 3;
    }
    if (!testComparison()) {
        return 4;
    }
    if (!testProperty()) {
        return 5;
    }
    if (!testStream()) {
        return 6;
    }
    if (!testOperator()) {
        return 7;
    }
    if (!testAdd()) {
        return 8;
    }
    if (!testStaticProperty()) {
        return 9;
    }
    if (!testStatic()) {
        return 10;
    }
    if (!testToString()) {
        return 11;
    }
    if (!testParse()) {
        return 12;
    }

    return 0;
}