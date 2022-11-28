//
//  TimeSpanTest.cpp
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/TimeSpan.h"
#include "IO/MemoryStream.h"

using namespace Common;

bool testConstructor() {
    {
        TimeSpan test;
        if (test != TimeSpan::Zero) {
            return false;
        }
    }
    {
        TimeSpan test(1 * 10 * 1000 * 1000);
        if (test.totalSeconds() != 1) {
            return false;
        }
    }
    {
        TimeSpan test(-1 * 10 * 1000 * 1000);
        if (test.totalSeconds() != -1) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3);
        if (test.totalSeconds() != 1 * 3600 + 2 * 60 + 3) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3, 4);
        if (test.totalSeconds() != 1 * 86400 + 2 * 3600 + 3 * 60 + 4) {
            return false;
        }
    }

    {
        TimeSpan test(1 * 10 * 1000 * 1000);
        TimeSpan test2(test);
        if (test2.totalSeconds() != 1) {
            return false;
        }
    }

    {
        TimeSpan test = TimeSpan::fromDays(1);
        if (test.totalDays() != 1) {
            return false;
        }
    }
    {
        TimeSpan test = TimeSpan::fromHours(1);
        if (test.totalHours() != 1) {
            return false;
        }
    }
    {
        TimeSpan test = TimeSpan::fromMinutes(1);
        if (test.totalMinutes() != 1) {
            return false;
        }
    }
    {
        TimeSpan test = TimeSpan::fromSeconds(1);
        if (test.totalSeconds() != 1) {
            return false;
        }
    }
    {
        TimeSpan test = TimeSpan::fromMilliseconds(1);
        if (test.totalMilliseconds() != 1) {
            return false;
        }
    }
    {
        TimeSpan test = TimeSpan::fromTicks(10 * 1000);
        if (test.totalMilliseconds() != 1) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(1, 2, 3);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(0, 2, 3);
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(1, 2, 3);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(1, 2, 3);
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2;
        test2.evaluates(test);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2;
        test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testComparison() {
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(2, 2, 3);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(2, 2, 3);
        if (test2 <= test) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(2, 2, 3);
        if (test2 < test) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(2, 2, 3);
        if (!(test2 >= test)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        TimeSpan test2(2, 2, 3);
        if (!(test2 > test)) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        TimeSpan test(1, 2, 3, 4, 5);
        if (test.days() != 1) {
            return false;
        }
        if (test.hours() != 2) {
            return false;
        }
        if (test.minutes() != 3) {
            return false;
        }
        if (test.seconds() != 4) {
            return false;
        }
        if (test.milliseconds() != 5) {
            return false;
        }
        if (test.totalMilliseconds() != 93784005) {
            return false;
        }
        if (test.totalSeconds() != 93784.00499999999) {
            return false;
        }
        if (test.totalDays() != 1.0854630208333333) {
            return false;
        }
        if (test.totalHours() != 26.051112499999999) {
            return false;
        }
        if (test.totalMinutes() != 1563.06675) {
            return false;
        }
        int64_t milliSeconds = (1 * 86400000 + 2 * 3600000 + 3 * 60000 + 4 * 1000 + 5);
        if (test.ticks() != milliSeconds * 10L * 1000) {
            return false;
        }
    }

    {
        TimeSpan test(1, 0, 0, 0, 0);
        if (test.totalDays() != 1) {
            return false;
        }
    }
    {
        TimeSpan test(0, 1, 0, 0, 0);
        if (test.totalHours() != 1) {
            return false;
        }
    }
    {
        TimeSpan test(0, 0, 1, 0, 0);
        if (test.totalMinutes() != 1) {
            return false;
        }
    }
    {
        TimeSpan test(0, 0, 0, 1, 0);
        if (test.totalSeconds() != 1) {
            return false;
        }
    }
    {
        TimeSpan test(0, 0, 0, 0, 1);
        if (test.totalMilliseconds() != 1) {
            return false;
        }
    }

    return true;
}

bool testOperator() {
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        TimeSpan test3 = test + test2;
        if (!(test3.days() == 6 && test3.hours() == 6 && test3.minutes() == 6 && test3.seconds() == 6 &&
              test3.milliseconds() == 6)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        test += test2;
        if (!(test.days() == 6 && test.hours() == 6 && test.minutes() == 6 && test.seconds() == 6 &&
              test.milliseconds() == 6)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        TimeSpan test3 = test2 - test;
        if (!(test3.days() == 4 && test3.hours() == 1 && test3.minutes() == 59 && test3.seconds() == 57 &&
              test3.milliseconds() == 996)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        test2 -= test;
        if (!(test2.days() == 4 && test2.hours() == 1 && test2.minutes() == 59 && test2.seconds() == 57 &&
              test2.milliseconds() == 996)) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        TimeSpan test3 = test.add(test2);
        if (!(test3.days() == 6 && test3.hours() == 6 && test3.minutes() == 6 && test3.seconds() == 6 &&
              test3.milliseconds() == 6)) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2(5, 4, 3, 2, 1);
        TimeSpan test3 = test2.subtract(test);
        if (!(test3.days() == 4 && test3.hours() == 1 && test3.minutes() == 59 && test3.seconds() == 57 &&
              test3.milliseconds() == 996)) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2 = +test;
        if (test2 != +test) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        TimeSpan test2 = -test;
        if (test2 != -test) {
            return false;
        }
    }

    return true;
}

bool testStream() {
    {
        TimeSpan test(1, 2, 3, 4, 5);
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeSpan test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        MemoryStream stream;
        test.write(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeSpan test2;
        test2.read(&stream, true);
        if (test != test2) {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4, 5);
        MemoryStream stream;
        test.write(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeSpan test2;
        test2.read(&stream, false);
        if (test != test2) {
            return false;
        }
    }

    {
        TimeSpan test(1, 2, 3, 4, 5);
        MemoryStream stream;
        test.writeBCDTime(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        TimeSpan test2;
        test2.readBCDTime(&stream);
        if (!(test.hours() == test2.hours() && test.minutes() == test2.minutes() &&
              test.seconds() == test2.seconds())) {
            return false;
        }
    }

    return true;
}

bool testStatic() {
    if (TimeSpan::Zero.totalMilliseconds() != 0) {
        return false;
    }
    if (TimeSpan::MinValue.ticks() != LLONG_MIN) {
        return false;
    }
    if (TimeSpan::MaxValue.ticks() != LLONG_MAX) {
        return false;
    }

    return true;
}

bool testToString() {
    // c
    {
        TimeSpan test(1, 2, 3, 4, 5);
        String str = test.toString("c");
        if(str != "1.02:03:04.0050000") {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3, 4);
        String str = test.toString("c");
        if(str != "1.02:03:04") {
            return false;
        }
    }
    {
        TimeSpan test(1, 2, 3);
        String str = test.toString("c");
        if(str != "01:02:03") {
            return false;
        }
    }
    {
        TimeSpan test(0, 2, 3, 4);
        String str = test.toString("c");
        if(str != "02:03:04") {
            return false;
        }
    }

    // g
    {
        TimeSpan test(1, 2, 3, 4, 5);
        String str = test.toString("g");
        if(str != "1:2:03:04.005") {
            return false;
        }
    }

    // G
    {
        TimeSpan test(1, 2, 3);
        String str = test.toString("g");
        if(str != "1:02:03") {
            return false;
        }
    }

    // %d
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("%d");
        if(str != "6") {
            return false;
        }
    }
    // ddd
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("ddd");
        if(str != "006") {
            return false;
        }
    }
    // dd\.hh\:mm
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("dd\\.hh\\:mm");
        if(str != "06.14:32") {
            return false;
        }
    }

    // %h
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("%h");
        if(str != "14") {
            return false;
        }
    }
    // hh\:mm
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("hh\\:mm");
        if(str != "14:32") {
            return false;
        }
    }
    // hh
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("hh");
        if(str != "14") {
            return false;
        }
    }
    // hh
    {
        TimeSpan test(6, 8, 32, 17, 685);
        String str = test.toString("hh");
        if(str != "08") {
            return false;
        }
    }

    // %m
    {
        TimeSpan test(6, 14, 32, 17, 685);
        String str = test.toString("%m");
        if(str != "32") {
            return false;
        }
    }
    // hh\:mm
    {
        TimeSpan test(6, 8, 32, 17, 685);
        String str = test.toString("h\\:m");
        if(str != "8:32") {
            return false;
        }
    }
    // mm
    {
        TimeSpan test(6, 8, 32, 17, 685);
        String str = test.toString("mm");
        if(str != "32") {
            return false;
        }
    }
    // d\.hh\:mm\:ss
    {
        TimeSpan test(6, 8, 32, 17, 685);
        String str = test.toString("d\\.hh\\:mm\\:ss");
        if(str != "6.08:32:17") {
            return false;
        }
    }

    // %s
    {
        TimeSpan test = TimeSpan::fromSeconds(12.965);
        String str = test.toString("%s");
        if(str != "12") {
            return false;
        }
    }
    // ss
    {
        TimeSpan test = TimeSpan::fromSeconds(6.965);
        String str = test.toString("ss");
        if(str != "06") {
            return false;
        }
    }
    // %s
    {
        TimeSpan test = TimeSpan::fromSeconds(6.965);
        String str = test.toString("ss\\.fff");
        if(str != "06.965") {
            return false;
        }
    }

    // %f
    {
        TimeSpan test = TimeSpan::fromSeconds(6.895);
        String str = test.toString("%f");
        if(str != "8") {
            return false;
        }
    }
    // ff
    {
        TimeSpan test = TimeSpan::fromSeconds(6.895);
        String str = test.toString("ff");
        if(str != "89") {
            return false;
        }
    }
    // ss.ff
    {
        TimeSpan test = TimeSpan::fromSeconds(6.895);
        String str = test.toString("ss\\.ff");
        if(str != "06.89") {
            return false;
        }
    }
    // fff
    {
        TimeSpan test = TimeSpan::fromSeconds(6.895);
        String str = test.toString("fff");
        if(str != "895") {
            return false;
        }
    }
    // ffff
    {
        TimeSpan test;
        TimeSpan::parse("0:0:6.8954321", test);
        String str = test.toString("ffff");
        if(str != "8954") {
            return false;
        }
    }
    // fffff
    {
        TimeSpan test;
        TimeSpan::parse("0:0:6.8954321", test);
        String str = test.toString("fffff");
        if(str != "89543") {
            return false;
        }
    }
    // ffffff
    {
        TimeSpan test;
        TimeSpan::parse("0:0:6.8954321", test);
        String str = test.toString("ffffff");
        if(str != "895432") {
            return false;
        }
    }
    // fffffff
    {
        TimeSpan test;
        TimeSpan::parse("0:0:6.8954321", test);
        String str = test.toString("fffffff");
        if(str != "8954321") {
            return false;
        }
    }

    // %F
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.32", test);
        String str = test.toString("%F");
        if(str != "3") {
            return false;
        }
    }
    // ss\.F
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.091", test);
        String str = test.toString("ss\\.F");
        if(str != "03.") {
            return false;
        }
    }

    // FF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.329", test);
        String str = test.toString("FF");
        if(str != "32") {
            return false;
        }
    }
    // ss\.FF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.101", test);
        String str = test.toString("ss\\.FF");
        if(str != "03.1") {
            return false;
        }
    }

    // FFF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.3291", test);
        String str = test.toString("FFF");
        if(str != "329") {
            return false;
        }
    }
    // ss\.FFF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.1009", test);
        String str = test.toString("ss\\.FFF");
        if(str != "03.1") {
            return false;
        }
    }

    // FFFF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.32917", test);
        String str = test.toString("FFFF");
        if(str != "3291") {
            return false;
        }
    }
    // ss\.FFFF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.10009", test);
        String str = test.toString("ss\\.FFFF");
        if(str != "03.1") {
            return false;
        }
    }

    // FFFFF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.329179", test);
        String str = test.toString("FFFFF");
        if(str != "32917") {
            return false;
        }
    }
    // ss\.FFFFF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.100009", test);
        String str = test.toString("ss\\.FFFFF");
        if(str != "03.1") {
            return false;
        }
    }

    // FFFFFF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.3291791", test);
        String str = test.toString("FFFFFF");
        if(str != "329179") {
            return false;
        }
    }
    // ss\.FFFFFF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.1000009", test);
        String str = test.toString("ss\\.FFFFFF");
        if(str != "03.1") {
            return false;
        }
    }

    // FFFFFFF
    {
        TimeSpan test;
        TimeSpan::parse("00:00:06.3291791", test);
        String str = test.toString("FFFFFFF");
        if(str != "3291791") {
            return false;
        }
    }
    // ss\.FFFFFFF
    {
        TimeSpan test;
        TimeSpan::parse("0:0:3.1900000", test);
        String str = test.toString("ss\\.FFFFFFF");
        if(str != "03.19") {
            return false;
        }
    }

    // hh':'mm':'ss
    {
        TimeSpan test(14, 32, 17);
        String str = test.toString("hh':'mm':'ss");
        if(str != "14:32:17") {
            return false;
        }
    }
    // hh\:mm\:ss
    {
        TimeSpan test(14, 32, 17);
        String str = test.toString("hh\\:mm\\:ss");
        if(str != "14:32:17") {
            return false;
        }
    }

    return true;
}

bool parseTimeSpan(const String& intervalStr, const String& expectStr) {
    TimeSpan intervalVal;
    if (!TimeSpan::parse(intervalStr, intervalVal)) {
        return false;
    }
    if(intervalVal.toString() != expectStr) {
        return false;
    }

    return true;
}

bool testParse() {
    if(!parseTimeSpan("0","00:00:00")) {
        return false;
    }
    if(!parseTimeSpan("14","14.00:00:00")) {
        return false;
    }
    if(!parseTimeSpan("1:2:3","01:02:03")) {
        return false;
    }
    if(!parseTimeSpan("0:0:0.250","00:00:00.2500000")) {
        return false;
    }
    if(!parseTimeSpan("10.20:30:40.50","10.20:30:40.5000000")) {
        return false;
    }
    if(!parseTimeSpan("99.23:59:59.9999999","99.23:59:59.9999999")) {
        return false;
    }
    if(!parseTimeSpan("0023:0059:0059.0099","23:59:59.0099000")) {
        return false;
    }
    if(!parseTimeSpan("23:0:0","23:00:00")) {
        return false;
    }
    if(parseTimeSpan("24:0:0","")) {
        return false;
    }
    if(!parseTimeSpan("0:59:0","00:59:00")) {
        return false;
    }
    if(parseTimeSpan("0:60:0","")) {
        return false;
    }
    if(!parseTimeSpan("0:0:59","00:00:59")) {
        return false;
    }
    if(parseTimeSpan("0:0:60","")) {
        return false;
    }
    if(parseTimeSpan("10:","")) {
        return false;
    }
    if(!parseTimeSpan("10:0","10:00:00")) {
        return false;
    }
    if(parseTimeSpan(":10","")) {
        return false;
    }
    if(!parseTimeSpan("0:10","00:10:00")) {
        return false;
    }
    if(parseTimeSpan("10:20:","")) {
        return false;
    }
    if(!parseTimeSpan("10:20:0","10:20:00")) {
        return false;
    }
    if(parseTimeSpan(".123","")) {
        return false;
    }
    if(!parseTimeSpan("0.12:00","12:00:00")) {
        return false;
    }
    if(parseTimeSpan("10.","")) {
        return false;
    }
    if(parseTimeSpan("10.12","")) {
        return false;
    }
    if(!parseTimeSpan("10.12:00","10.12:00:00")) {
        return false;
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
    if (!testComparison()) {
        return 3;
    }
    if (!testProperty()) {
        return 4;
    }
    if (!testOperator()) {
        return 5;
    }
    if (!testStream()) {
        return 6;
    }
    if (!testStatic()) {
        return 7;
    }
    if (!testToString()) {
        return 8;
    }
    if (!testParse()) {
        return 9;
    }

    return 0;
}