//
//  TimeZoneTest.cpp
//  common
//
//  Created by baowei on 2022/11/27.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/TimeZone.h"
#include "IO/MemoryStream.h"

using namespace Common;

bool testConstructor() {
    {
        TimeZone test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        TimeZone test(TimeSpan::fromHours(-8).ticks());
        if (test.type() != TimeZone::GMT_A0800) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.ticksOffset() != TimeSpan::fromHours(-8).ticks()) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(test);
        if (test != tz2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(test);
        if (!test.equals(tz2)) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2;
        tz2.evaluates(test);
        if (test != tz2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2;
        tz2 = test;
        if (test != tz2) {
            return false;
        }
    }

    return true;
}

bool testCompareTo() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(TimeZone::GMT_A0700);
        if (test.compareTo(tz2) >= 0) {
            return false;
        }
    }

    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(TimeZone::GMT_A0700);
        if (!(test <= tz2)) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(TimeZone::GMT_A0700);
        if (!(test < tz2)) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(TimeZone::GMT_A0700);
        if (test > tz2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        TimeZone tz2(TimeZone::GMT_A0700);
        if (test >= tz2) {
            return false;
        }
    }

    return true;
}

bool testToString() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.toString() != "GMT+08:00") {
            return false;
        }
    }
    {
        TimeZone test;
        if (!test.toString().isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        DateTime time = test.toUtcTime(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Utc));
        if (!(time.year() == 2010 && time.month() == 1 && time.day() == 2 && time.hour() == 3 && time.minute() == 4 &&
              time.second() == 5 && time.millisecond() == 6)) {
            return false;
        }
        if (time.kind() != DateTime::Utc) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        DateTime time = test.toUtcTime(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Local));
        if (!(time.year() == 2010 && time.month() == 1 && time.day() == 1 && time.hour() == 19 && time.minute() == 4 &&
              time.second() == 5 && time.millisecond() == 6)) {
            return false;
        }
        if (time.kind() != DateTime::Utc) {
            return false;
        }
    }

    {
        TimeZone test(TimeZone::GMT_A0800);
        DateTime time = test.toLocalTime(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Utc));
        if (!(time.year() == 2010 && time.month() == 1 && time.day() == 2 && time.hour() == 11 && time.minute() == 4 &&
              time.second() == 5 && time.millisecond() == 6)) {
            return false;
        }
        if (time.kind() != DateTime::Local) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        DateTime time = test.toLocalTime(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Local));
        if (!(time.year() == 2010 && time.month() == 1 && time.day() == 2 && time.hour() == 3 && time.minute() == 4 &&
              time.second() == 5 && time.millisecond() == 6)) {
            return false;
        }
        if (time.kind() != DateTime::Local) {
            return false;
        }
    }

    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.ticksOffset() != -8L * 3600 * 1000 * 10000) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.secondOffset() != -8L * 3600) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.minuteOffset() != -8L * 60) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.hourOffset() != -8L) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.offset() != TimeSpan::fromHours(-8)) {
            return false;
        }
    }

    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.type() != TimeZone::GMT_A0800) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        if (test.toTypeStr() != "GMT+08:00") {
            return false;
        }
    }

    {
        TimeSpan time = TimeZone::getUtcOffset(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Utc));
        if(time != TimeSpan::Zero) {
            return false;
        }
    }
    {
        TimeSpan time = TimeZone::getUtcOffset(DateTime(2010, 1, 2, 3, 4, 5, 6, DateTime::Local));
        if(time != TimeZone::Local.offset()) {
            return false;
        }
    }

    return true;
}

bool testStream() {
    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.write(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.read(&stream, true);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.write(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.read(&stream, false);
        if (test != test2) {
            return false;
        }
    }

    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.writeSeconds(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.readSeconds(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.writeSeconds(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.readSeconds(&stream, true);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeZone test(TimeZone::GMT_A0800);
        MemoryStream stream;
        test.writeSeconds(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeZone test2;
        test2.readSeconds(&stream, false);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        TimeZone test;
        if(!TimeZone::parse("GMT+08:00", test)) {
            return false;
        }
        if(test != TimeZone(TimeZone::GMT_A0800)) {
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
    if (!testCompareTo()) {
        return 4;
    }
    if (!testToString()) {
        return 5;
    }
    if (!testProperty()) {
        return 6;
    }
    if (!testStream()) {
        return 7;
    }
    if (!testParse()) {
        return 8;
    }

    return 0;
}