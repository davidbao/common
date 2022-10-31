//
//  ArrayTest.cpp
//  common
//
//  Created by baowei on 2022/10/13.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/ByteArray.h"
#include "IO/MemoryStream.h"

using namespace Common;

#define DefaultCapacity 125

bool testConstructor() {
    ByteArray test(DefaultCapacity);
    if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
        return false;
    }

    ByteArray test2(DefaultCapacity);
    test2.add(1);
    test2.add(2);
    ByteArray test3(test2);
    if (test2 != test3) {
        return false;
    }

    static const int count = 5;
    uint8_t array[count] = {1, 2, 3, 4, 5};
    ByteArray test4(array, count);
    if (test4.count() != count) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (array[i] != test4[i]) {
            return false;
        }
    }

    static const int value = 6;
    static const int count2 = 120;
    ByteArray test5(value, count2);
    if(test5.count() != count2) {
        return false;
    }
    for (int i = 0; i < count2; ++i) {
        if (value != test5[i]) {
            return false;
        }
    }

    return true;
}

bool testStream() {
    MemoryStream ms;
    ByteArray test;
    for (int i = 0; i < 100; ++i) {
        test.add(i);
    }
    test.write(&ms);

    ByteArray test2;
    ms.seek(0, SeekOrigin::SeekBegin);
    test2.read(&ms);
    if(test != test2) {
        return false;
    }

    return true;
}

bool testToString() {
    ByteArray test;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    test.add(0xAF);

    String str = test.toString();
    if(str != "00-01-02-03-04-AF") {
        return false;
    }

    String str2 = test.toString(ByteArray::HexFormat, "--");
    if(str2 != "00--01--02--03--04--AF") {
        return false;
    }

    String str3 = test.toString(ByteArray::DecFormat);
    printf("%s\n", str3.c_str());
    if(str3 != "00-01-02-03-04-175") {
        return false;
    }

    String str4 = test.toString(ByteArray::OtcFormat);
    printf("%s\n", str3.c_str());
    if(str4 != "00-01-02-03-04-257") {
        return false;
    }

    return true;
}

bool testToLimitString() {
    ByteArray test;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    test.add(0xAF);
    for (int i = 0xC0; i < count; ++i) {
        test.add(i);
    }

    String str = test.toLimitString(17);
    if(str != "00-01-02-03-04-AF") {
        return false;
    }

    String str2 = test.toLimitString(21, ByteArray::HexFormat, "--");
    if(str2 != "00--01--02--03--04--AF") {
        return false;
    }

    String str3 = test.toLimitString(18, ByteArray::DecFormat);
    if(str3 != "00-01-02-03-04-175") {
        return false;
    }

    String str4 = test.toLimitString(18, ByteArray::OtcFormat);
    if(str4 != "00-01-02-03-04-257") {
        return false;
    }

    return true;
}

bool testToHexString() {
    ByteArray test;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    test.add(0xAF);

    String str = test.toHexString();
    if(str != "0001020304AF") {
        return false;
    }

    return true;
}

bool testFind() {
    ByteArray test;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    ByteArray test2;
    for (int i = 2; i < count; ++i) {
        test2.add(i);
    }
    ssize_t pos = test.find(test2);
    if(pos != 2) {
        return false;
    }

    ByteArray test3;
    for (int i = 3; i < count; ++i) {
        test3.add(i);
    }
    int pos2 = ByteArray::find(test, test3);
    if(pos2 != 3) {
        return false;
    }

    ByteArray test4;
    for (int i = 10; i < 10 + count; ++i) {
        test4.add(i);
    }
    ssize_t pos3 = ByteArray::find(test, test4);
    if(pos3 != -1) {
        return false;
    }

    uint8_t array[count] = {1, 2, 3, 4, 5};
    ByteArray test5;
    test5.addRange(array, count);
    test5.addRange(array, count);
    test5.addRange(array, count);
    ssize_t pos4 = test5.find(ByteArray(5, 1));
    if(pos4 != 4) {
        return false;
    }
    ssize_t pos5 = test5.find(5, 10, ByteArray(5, 1));
    if(pos5 != 9) {
        return false;
    }
    ssize_t pos6 = test5.find(5, 2, ByteArray(5, 1));
    if(pos6 != -1) {
        return false;
    }

    return true;
}

bool testReplace() {
    ByteArray source;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        source.add(i);
    }
//    printf("%s\n", test.toString().c_str());
    ByteArray src;
    for (int i = 2; i < count; ++i) {
        src.add(i);
    }
//    printf("%s\n", src.toString().c_str());
    ByteArray dst;
    for (int i = 0x0a; i < 0x0f; ++i) {
        dst.add(i);
    }
//    printf("%s\n", dst.toString().c_str());

    ByteArray test(source);
//    printf("%s\n", test.toString().c_str());
    test = test.replace(ByteArray(1, 3), dst);
//    printf("%s\n", test.toString().c_str());
    if(test.toString() != "00-01-02-03-04") {
        return false;
    }

    test = test.replace(src, dst);
//    printf("%s\n", test.toString().c_str());
    if(test.toString() != "00-01-0A-0B-0C-0D-0E") {
        return false;
    }

    ByteArray test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    test2 = ByteArray::replace(test2, src, dst);
    if(test.toString() != "00-01-0A-0B-0C-0D-0E") {
        return false;
    }

    ByteArray test3(0xFF, count);
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
//    printf("%s\n", test3.toString().c_str());
    test3 = test3.replace(2, 2, ByteArray(0xFF, 1), ByteArray(0xEE, 2));
//    printf("%s\n", test3.toString().c_str());
    if(test3.toString() != "FF-FF-EE-EE-EE-EE-FF-00-01-02-03-04") {
        return false;
    }

    ByteArray test4(0xFF, count);
    for (int i = 0; i < count; ++i) {
        test4.add(i);
    }
    for (int i = 0; i < count; ++i) {
        test4.add(0xFF);
    }
//    printf("%s\n", test4.toString().c_str());
    test4 = test4.replace(2, 10, ByteArray(0xFF, 1), ByteArray(0xEE, 2));
//    printf("%s\n", test4.toString().c_str());
    if(test4.toString() != "FF-FF-EE-EE-EE-EE-EE-EE-00-01-02-03-04-EE-EE-EE-EE-FF-FF-FF") {
        return false;
    }

    return true;
}

bool testEquals() {
    ByteArray test;
    static const int count = 5;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    ByteArray test2;
    test2 = test;
    if(test != test2) {
        return false;
    }

    return true;
}

bool testParse() {
    ByteArray test;
    static const int count = 5;
    if(!ByteArray::parse("00-01-02-03-04", test)) {
        return false;
    }
    if(test.count() != 5) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if(test[i] != i) {
            return false;
        }
    }

    ByteArray test2;
    if(!ByteArray::parse("00--01--02--03--04", test2, "--")) {
        return false;
    }
    if(test2.count() != 5) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if(test2[i] != i) {
            return false;
        }
    }

    ByteArray test3;
    if(!ByteArray::parse("00-01-02-03-04abc", test3)) {
        return false;
    }
    if(!test3.isEmpty()) {
        return false;
    }

    ByteArray test4;
    if(!ByteArray::parseHexString("ACADAEAFB0", test4)) {
        return false;
    }
    if(test4.count() != 5) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if(test4[i] != i + 0xAC) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testStream()) {
        return 2;
    }
    if (!testToString()) {
        return 3;
    }
    if (!testToLimitString()) {
        return 4;
    }
    if (!testToHexString()) {
        return 5;
    }
    if (!testFind()) {
        return 6;
    }
    if (!testReplace()) {
        return 7;
    }
    if(!testEquals()) {
        return 8;
    }
    return 0;
}