//
//  ValueTypeTest.cpp
//  common
//
//  Created by baowei on 2022/11/18.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/Version.h"
#include "IO/MemoryStream.h"

using namespace Data;

bool testConstructor() {
    {
        Version test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Version test(1);
        if (test.major() != 1) {
            return false;
        }
    }
    {
        Version test(1, 2);
        if (!(test.major() == 1 && test.minor() == 2)) {
            return false;
        }
    }
    {
        Version test(1, 2, 3);
        if (!(test.major() == 1 && test.minor() == 2 && test.build() == 3)) {
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        if (!(test.major() == 1 && test.minor() == 2 && test.build() == 3 && test.revision() == 4)) {
            return false;
        }
    }
    {
        Version test("1.2");
        if (!(test.major() == 1 && test.minor() == 2)) {
            return false;
        }
    }
    {
        Version test(String("1.2"));
        if (!(test.major() == 1 && test.minor() == 2)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2(test);
        if (!(test2.major() == 1 && test2.minor() == 2)) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        Version test("1.2");
        Version test2("1.2");
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2("1.2");
        if (test != test2) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2("1.2");
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testCompare() {
    {
        Version test("1.2");
        Version test2("1.3");
        if (!(test2.compareTo(test) > 0)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2("1.3");
        if (!(test2.compareTo(test) >= 0)) {
            return false;
        }
    }
    {
        Version test("1.3");
        Version test2("1.2");
        if (!(test2.compareTo(test) < 0)) {
            return false;
        }
    }
    {
        Version test("1.3");
        Version test2("1.2");
        if (!(test2.compareTo(test) <= 0)) {
            return false;
        }
    }

    {
        Version test("1.2");
        Version test2("1.3");
        if (!(test2 > test)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2("1.3");
        if (!(test2 >= test)) {
            return false;
        }
    }
    {
        Version test("1.3");
        Version test2("1.2");
        if (!(test2 < test)) {
            return false;
        }
    }
    {
        Version test("1.3");
        Version test2("1.2");
        if (!(test2 <= test)) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        Version test("1.2");
        Version test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2 = test;
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Version test("1.2");
        Version test2 = "1.2";
        if (!test.equals(test2)) {
            return false;
        }
    }

    return true;
}

bool testVersion() {
    {
        Version test("1.2.3.4");
        if (!(test.major() == 1 && test.minor() == 2 && test.build() == 3 && test.revision() == 4)) {
            return false;
        }
    }

    return true;
}

bool testToString() {
    {
        Version test(1, 2);
        if (test.toString() != "1.2") {
            return false;
        }
    }
    {
        Version test(1, 2);
        String str = test;
        if (str != "1.2") {
            return false;
        }
    }

    return true;
}

bool testStream() {
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.read(&stream);
        if (test2 != test) {
            printf("testStream.error1\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.write(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.read(&stream, true);
        if (test2 != test) {
            printf("testStream.error2\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.write(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.read(&stream, false);
        if (test2 != test) {
            printf("testStream.error3\n");
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt32(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt32(&stream);
        if (test2 != test) {
            printf("testStream.error4\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt32(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt32(&stream, true);
        if (test2 != test) {
            printf("testStream.error5\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt32(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt32(&stream, false);
        if (test2 != test) {
            printf("testStream.error6\n");
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt16(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt16(&stream);
        if (test2 != test) {
            printf("testStream.error7\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt16(&stream, true);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt16(&stream, true);
        if (test2 != test) {
            printf("testStream.error8\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeInt16(&stream, false);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readInt16(&stream, false);
        if (test2 != test) {
            printf("testStream.error9\n");
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readByte(&stream);
        if (test2 != test) {
            printf("testStream.error10\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readByte(&stream);
        if (test2 != test) {
            printf("testStream.error11\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readByte(&stream);
        if (test2 != test) {
            printf("testStream.error12\n");
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDInt32(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDInt32(&stream);
        if (test2 != test) {
            printf("testStream.error13\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDInt32(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDInt32(&stream);
        if (test2 != test) {
            printf("testStream.error14\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDInt32(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDInt32(&stream);
        if (test2 != test) {
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDInt16(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDInt16(&stream);
        if (test2 != test) {
            printf("testStream.error15\n");
            return false;
        }
    }

    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDByte(&stream);
        if (test2 != test) {
            printf("testStream.error16\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDByte(&stream);
        if (test2 != test) {
            printf("testStream.error17\n");
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        MemoryStream stream;
        test.writeBCDByte(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Version test2;
        test2.readBCDByte(&stream);
        if (test2 != test) {
            printf("testStream.error18\n");
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        Version test;
        if (!Version::parse("1.2.3.4", test)) {
            return false;
        }
        if (!(test.major() == 1 && test.minor() == 2 && test.build() == 3 && test.revision() == 4)) {
            return false;
        }
    }
    {
        Version test;
        if (Version::parse("x.2.3.4", test)) {
            return false;
        }
    }

    {
        Version test;
        if (!Version::parseByte("1.2.3.4", test)) {
            return false;
        }
        if (!(test.major() == 1 && test.minor() == 2 && test.build() == 3 && test.revision() == 4)) {
            return false;
        }
    }
    {
        Version test;
        if (Version::parseByte("x.2.3.4", test)) {
            return false;
        }
    }

    return true;
}

bool testIncrease() {
    {
        Version test(1, 2);
        Version test2 = test.increase(test);
        if (!(test2.major() == 1 && test2.minor() == 3)) {
            return false;
        }
    }
    {
        Version test(1, 255);
        Version test2 = test.increase(test);
        if (!(test2.major() == 2 && test2.minor() == 255)) {
            return false;
        }
    }
    {
        Version test(1, 2, 3);
        Version test2 = test.increase(test);
        if (!(test2.major() == 1 && test2.minor() == 2 && test2.build() == 4)) {
            return false;
        }
    }
    {
        Version test(1, 2, 255);
        Version test2 = test.increase(test);
        if (!(test2.major() == 1 && test2.minor() == 3 && test2.build() == 255)) {
            return false;
        }
    }
    {
        Version test(1, 2, 3, 4);
        Version test2 = test.increase(test);
        if (!(test2.major() == 1 && test2.minor() == 2 && test2.build() == 3 && test2.revision() == 5)) {
            return false;
        }
    }
    {
        Version test(1, 2, 3, 255);
        Version test2 = test.increase(test);
        if (!(test2.major() == 1 && test2.minor() == 2 && test2.build() == 4 && test2.revision() == 255)) {
            return false;
        }
    }

    return true;
}

bool testStatic() {
    {
        if(!Version::Empty.isEmpty()) {
            return false;
        }
    }
    {
        Version test = Version::Version1_0;
        if (!(test.major() == 1 && test.minor() == 0)) {
            return false;
        }
    }

    return true;
}

bool testVersions() {
    {
        Versions test;
        test.add(Version(1, 2, 3, 4));
        test.add(Version(1, 2, 3, 5));
        test.add(Version(1, 2, 3, 6));
        if(test.count() != 3) {
            return false;
        }
        if(!(test[0] == "1.2.3.4" && test[1] == "1.2.3.5" && test[2] == "1.2.3.6")) {
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
    if (!testCompare()) {
        return 3;
    }
    if (!testEvaluates()) {
        return 4;
    }
    if (!testVersion()) {
        return 5;
    }
    if (!testToString()) {
        return 6;
    }
    if (!testStream()) {
        return 7;
    }
    if (!testIncrease()) {
        return 8;
    }
    if (!testStatic()) {
        return 9;
    }
    if (!testVersions()) {
        return 10;
    }

    return 0;
}