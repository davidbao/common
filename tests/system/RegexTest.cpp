//
//  RandomTest.cpp
//  common
//
//  Created by baowei on 2023/9/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/Regex.h"
#include "data/StringMap.h"

using namespace System;

bool testConstructor() {
    {
        Regex test("");
        if (test.isValid()) {
            return false;
        }
        if (!test.error().isNullOrEmpty()) {
            return false;
        }
    }
    {
        Regex test("^1[34578][0-9]{9}$");
        if (!test.isValid()) {
            return false;
        }
        if (!test.error().isNullOrEmpty()) {
            return false;
        }
    }
#ifndef __EMSCRIPTEN__
    {
        Regex test("**");
        if (test.isValid()) {
            return false;
        }
        if (test.error().isNullOrEmpty()) {
            return false;
        }
    }
#endif

    return true;
}

bool testMatch() {
    {
        String lines[] = {"Roses are #ff0000",
                          "violets are #0000ff",
                          "all of my base are belong to you"};
        Regex color_regex("#([a-f0-9]{2})"
                          "([a-f0-9]{2})"
                          "([a-f0-9]{2})");
        StringArray groups;
        for (const auto &line: lines) {
            color_regex.match(line, groups);
        }
        if (groups.count() != 6) {
            return false;
        }
        if (!((groups[0] + groups[1] + groups[2]) == "ff0000" && (groups[3] + groups[4] + groups[5]) == "0000ff")) {
            return false;
        }
    }
    {
        String str(R"(
                    Speed:	366
                    Mass:	35
                    Speed:	378
                    Mass:	32
                    Speed:	400
                    Mass:	30)"
        );
        StringArray lines;
        StringArray::parse(str, lines, '\n');
        Regex test(R"(Speed:\t\d*)");
        StringArray groups;
        for (const auto &line: lines) {
            test.match(line, groups);
        }
        if (groups.count() != 3) {
            return false;
        }
        if (groups[0].find("Speed:\t366") != 0) {
            return false;
        }
        if (groups[1].find("Speed:\t378") != 0) {
            return false;
        }
        if (groups[2].find("Speed:\t400") != 0) {
            return false;
        }
    }
    {
        String str = "host=192.168.1.1";
        Regex test("([^&?]\\w+)=([^&]*)");
        StringArray groups;
        StringMap maps;
        if (test.match(str, groups) && groups.count() == 2) {
            maps.add(groups[0], groups[1]);
        }
        if (maps.count() != 1) {
            return false;
        }
        if (maps["host"] != "192.168.1.1") {
            return false;
        }
    }
    {
        String str = "http://127.0.0.1:8081/login?name=admin &pass=admin' and '1=1";
        Regex test("\b(and|exec|insert|select|drop|grant|alter|delete|update|count|chr|mid|master|truncate|char|declare|or)\b|(\\*|;|\\+|'|%)");
        if (!test.isValid()) {
            return false;
        }
        if (!test.isMatch(str)) {
            return false;
        }
    }
    {
        String str = "test";
        Regex test("([^0-9]*)([0-9]+)");
        if (!test.isValid()) {
            return false;
        }
        if (test.isMatch(str)) {
            return false;
        }
    }
    {
        String str = "test1";
        Regex test("([^0-9]*)([0-9]+)");
        if (!test.isValid()) {
            return false;
        }
        if (!test.isMatch(str)) {
            return false;
        }
    }
    {
        String str = "test=1";
        Regex test("^(\\S+)=(.*)$");
        if (!test.isValid()) {
            return false;
        }
        if (!test.isMatch(str)) {
            return false;
        }
    }
    {
        String str = "1901:1";
        Regex test("(\\d+|\\s):(.+)");
        if (!test.isValid()) {
            return false;
        }
        if (!test.isMatch(str)) {
            return false;
        }
    }

    return true;
}

bool testSystemRegex() {
    {
        if (!SystemRegex::isPhoneNumber("13912345678")) {
            return false;
        }
        if (SystemRegex::isPhoneNumber("139123456789")) {
            return false;
        }
    }
    {
        if (!SystemRegex::isIdCardNo("110102200001018881")) {
            return false;
        }
        if (!SystemRegex::isIdCardNo("11010220000101888x")) {
            return false;
        }
        if (!SystemRegex::isIdCardNo("11010220000101888X")) {
            return false;
        }
    }
    {
        if (!SystemRegex::isMail("abc123@111.com")) {
            return false;
        }
        if (SystemRegex::isMail("abc123@@111.com")) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testMatch()) {
        return 2;
    }
    if (!testSystemRegex()) {
        return 3;
    }

    return 0;
}
