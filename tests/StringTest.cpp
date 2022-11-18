//
//  StringTest.cpp
//  common
//
//  Created by baowei on 2022/10/8.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/String.h"
#include "data/WString.h"
#include "IO/MemoryStream.h"

using namespace Common;

static const String _text = "ABC/abc123,)_中文";
static const string _text2 = "ABC/abc123,)_中文";
static const char *_text3 = "ABC/abc123,)_中文";
static const String _tex4 = "ABC/abc123,)_中文ABC/abc123,)_中文";

bool testConstructor() {
    String str;
    if (str.length() != 0) {
        return false;
    }

    String str2(_text);
    if (str2 != _text) {
        return false;
    }

    string text = _text.c_str();
    String str3(text);
    if (str3 != _text) {
        return false;
    }

    String str4(_text3);
    if (str4 != _text2) {
        return false;
    }

    String str5(_text3, 5);
    if (str5 != "ABC/a") {
        return false;
    }

    String str6('a', 5);
    if (str6 != "aaaaa") {
        return false;
    }

    String str7('a');
    if (str7 != "a") {
        return false;
    }

    return true;
}

bool testIsNullOrEmpty() {
    String str;
    if (!str.isNullOrEmpty()) {
        return false;
    }

    return true;
}

bool testLength() {
    String str = _text3;
    if (str.length() != strlen(_text3)) {
        return false;
    }

    return true;
}

bool testEmpty() {
    String str = _text;
    str.empty();
    if (str.length() != 0) {
        return false;
    }

    return true;
}

bool testOperators() {
    {
        // operator const char*
        const char *str = _text;
        if (str != _text.c_str()) {
            return false;
        }
    }

    {
        // operator +=
        String str2(_text);
        str2 += _text;
        if (str2 != _tex4) {
            return false;
        }
    }

    String str3(_text);
    str3 += _text2;
    if (str3 != _tex4) {
        return false;
    }

    {
        String str4(_text);
        str4 += _text3;
        if (str4 != _tex4) {
            return false;
        }
    }

    {
        // operator +
        String str5(_text);
        str5 = str5 + _text;
        if (str5 != _tex4) {
            return false;
        }
    }

    {
        String str6(_text);
        str6 = str6 + _text2;
        if (str6 != _tex4) {
            return false;
        }
    }

    {
        String str7(_text);
        str7 = str7 + _text3;
        if (str7 != _tex4) {
            return false;
        }
    }

    {
        // operator =
        String str8 = _text;
        if (str8 != _text) {
            return false;
        }
    }

    {
        String str9 = _text2;
        if (str9 != _text2) {
            return false;
        }
    }

    {
        String str10 = _text3;
        if (str10 != _text3) {
            return false;
        }
    }

    {
        // operator ==
        String str11 = _text;
        if (!(str11 == _text)) {
            return false;
        }
    }

    {
        String str12 = _text2;
        if (!(str12 == _text2)) {
            return false;
        }
    }

    {
        String str13 = _text3;
        if (!(str13 == _text3)) {
            return false;
        }
    }

    {
        // operator !=
        String str14 = _text;
        if (str14 != _text) {
            return false;
        }
    }

    {
        String str15 = _text2;
        if (str15 != _text2) {
            return false;
        }
    }

    {
        String str16 = _text3;
        if (str16 != _text3) {
            return false;
        }
    }

    {
        // operator >
        String str17 = _text;
        str17.append('1');
        if (!(str17 > _text)) {
            return false;
        }
    }

    {
        String str18 = _text2;
        str18.append('1');
        if (!(str18 > _text2)) {
            return false;
        }
    }

    {
        String str19 = _text3;
        str19.append('1');
        if (!(str19 > _text3)) {
            return false;
        }
    }

    {
        // operator >=
        String str20 = _text;
        str20.append('1');
        if (!(str20 >= _text)) {
            return false;
        }
    }

    {
        String str21 = _text2;
        str21.append('1');
        if (!(str21 >= _text2)) {
            return false;
        }
    }

    {
        String str22 = _text3;
        str22.append('1');
        if (!(str22 >= _text3)) {
            return false;
        }
    }

    {
        // operator <
        String str23 = _text;
        str23[1] = 'A';
        if (!(str23 < _text)) {
            return false;
        }
    }

    {
        String str24 = _text2;
        str24[1] = 'A';
        if (!(str24 < _text2)) {
            return false;
        }
    }

    {
        String str25 = _text3;
        str25[1] = 'A';
        if (!(str25 < _text3)) {
            return false;
        }
    }

    {
        // operator <=
        String str26 = _text;
        str26[1] = 'A';
        if (!(str26 <= _text)) {
            return false;
        }
    }

    {
        String str27 = _text2;
        str27[1] = 'A';
        if (!(str27 <= _text2)) {
            return false;
        }
    }

    {
        String str28 = _text3;
        str28[1] = 'A';
        if (!(str28 <= _text3)) {
            return false;
        }
    }

#ifndef __arm_linux__
    {
        String str = _text3;
        WString str2 = str;
        printf("String to WString: %ls\n", str2.c_str());
        if (str2 != L"ABC/abc123,)_中文") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetter() {
    String str = "abc123";
    return str.at(3) == '1';
}

bool testSetter() {
    String str = "abc123";
    str[3] = '2';
    if (str != "abc223") {
        return false;
    }

    String str2 = "abc123";
    str2.set(3, '2');
    if (str2 != "abc223") {
        return false;
    }

    return true;
}

bool testStream() {
    MemoryStream ms;
    String str(_text);
    str.write(&ms);
    ms.seek(0, SeekOrigin::SeekBegin);
    String str2;
    str2.read(&ms);
    if (str != str2) {
        return false;
    }

    MemoryStream ms2;
    String str3(_text);
    str3.write(&ms2, String::StreamLength2);
    ms2.seek(0, SeekOrigin::SeekBegin);
    String str4;
    str4.read(&ms2, String::StreamLength2);
    if (str3 != str4) {
        return false;
    }

    MemoryStream ms3;
    String str5(_text);
    str5.write(&ms3, String::StreamLength4);
    ms3.seek(0, SeekOrigin::SeekBegin);
    String str6;
    str6.read(&ms3, String::StreamLength4);
    if (str5 != str6) {
        return false;
    }

    MemoryStream ms4;
    String str7(_text);
    str7.writeFixedLengthStr(&ms4, str7.length());
    ms4.seek(0, SeekOrigin::SeekBegin);
    String str8;
    str8.readFixedLengthStr(&ms4, str7.length());
    if (str7 != str8) {
        return false;
    }

    MemoryStream ms5;
    static const char text[] = "0123456789";
    static const int count = 6554; // text.length * count greater than 64K
    String str9;
    for (int i = 0; i < count; i++) {
        str9.append(text);
    }
    str9.writeFixedLengthStr(&ms5, str9.length());
    ms5.seek(0, SeekOrigin::SeekBegin);
    String str10;
    str10.readFixedLengthStr(&ms5, str9.length());
    if (str9 != str10) {
        return false;
    }

    MemoryStream ms6;
    String str11(_text);
    str11.writeFixedLengthStr(&ms6, str11.length() + 10);
    ms6.seek(0, SeekOrigin::SeekBegin);
    String str12;
    str12.readFixedLengthStr(&ms6, str11.length() + 10);
    if (str11 != str12) {
        return false;
    }

    return true;
}

bool testStr() {
    String str(_text);
    const char *text = str.c_str();
    if (_text != text) {
        return false;
    }

    return true;
}

bool testLowerAndUpper() {
    String str("ABC123");
    String str2 = str.toLower();
    if (str2 != "abc123") {
        return false;
    }

    String str3("abc123");
    String str4 = str2.toUpper();
    if (str4 != "ABC123") {
        return false;
    }

    return true;
}

bool testTrim() {
    String str = " abc 123 ";
    String str2 = str.trim(' ');
    if (str2 != "abc 123") {
        return false;
    }

    String banner = "*** Much Ado About Nothing ***";
    String result = banner.trim('*', ' ', '\'');
    if (result != "Much Ado About Nothing") {
        return false;
    }

    String lineWithLeadingSpaces = "   Hello World!";
    String lineAfterTrimStart = lineWithLeadingSpaces.trimStart(' ');
    if (lineAfterTrimStart != "Hello World!") {
        return false;
    }

    String lineToBeTrimmed = "__###__ John Smith";
    String lineAfterTrimStart2 = lineToBeTrimmed.trimStart('_', '#', ' ');
    if (lineAfterTrimStart2 != "John Smith") {
        return false;
    }

    String lineWithLeadingSpaces2 = "Hello World!   ";
    String lineAfterTrimEnd = lineWithLeadingSpaces2.trimEnd(' ');
    if (lineAfterTrimEnd != "Hello World!") {
        return false;
    }

    String lineToBeTrimmed2 = "John Smith__###__ ";
    String lineAfterTrimEnd2 = lineToBeTrimmed2.trimEnd('_', '#', ' ');
    if (lineAfterTrimEnd2 != "John Smith") {
        return false;
    }

    return true;
}

bool testEncoding() {
    String str(_text);
    String str2;
    if (str.isUTF8()) {
        str2 = str.UTF8toGBK();
        str2 = str2.GBKtoUTF8();
    } else {
        str2 = str.GBKtoUTF8();
        str2 = str2.UTF8toGBK();
    }
    if (str2 != str) {
        return false;
    }

    return true;
}

bool testBase64() {
    String str(_text);
    String str2 = str.toBase64();
    if (str2 != "QUJDL2FiYzEyMywpX+S4reaWhw==") {
        return false;
    }
    String str3 = str2.fromBase64();
    if (str3 != str) {
        return false;
    }

    return true;
}

bool testFind() {
    String str(_text);
    ssize_t pos = str.find("a");
    if (pos != 4) {
        return false;
    }
    ssize_t pos2 = str.find("a", 5);
    if (pos2 >= 0) {
        return false;
    }

    ssize_t pos3 = str.find('a');
    if (pos3 != 4) {
        return false;
    }
    ssize_t pos4 = str.find('a', 5);
    if (pos4 >= 0) {
        return false;
    }

    String str2("abc1123");
    ssize_t pos5 = str2.find("1");
    if (pos5 != 3) {
        return false;
    }
    ssize_t pos6 = str2.findLastOf('1');
    if (pos6 != 4) {
        return false;
    }

    return true;
}

bool testAppend() {
    {
        String test = "abc";
        test.append('1');
        if(test != "abc1") {
            return false;
        }
    }
    {
        String test = "abc";
        test.append("1");
        if(test != "abc1") {
            return false;
        }
    }
    {
        String test = "abc";
        test.append("123", 2);
        if(test != "abc12") {
            return false;
        }
    }
    {
        String test = "abc";
        String test2 = "123";
        test.append(test2, 1, 1);
        if(test != "abc2") {
            return false;
        }
    }

    {
        String test = "abc";
        test.appendLine('1');
        if(test != "abc1\n") {
            return false;
        }
    }
    {
        String test = "abc";
        test.appendLine("1");
        if(test != "abc1\n") {
            return false;
        }
    }
    {
        String test = "abc";
        test.appendLine("123", 2);
        if(test != "abc12\n") {
            return false;
        }
    }
    {
        String test = "abc";
        String test2 = "123";
        test.appendLine(test2, 1, 1);
        if(test != "abc2\n") {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testIsNullOrEmpty()) {
        return 2;
    }
    if (!testLength()) {
        return 3;
    }
    if (!testEmpty()) {
        return 4;
    }
    if (!testOperators()) {
        return 5;
    }
    if (!testGetter()) {
        return 6;
    }
    if (!testSetter()) {
        return 7;
    }
    if (!testStream()) {
        return 8;
    }
    if (!testStr()) {
        return 9;
    }
    if (!testLowerAndUpper()) {
        return 10;
    }
    if (!testTrim()) {
        return 11;
    }
    if (!testEncoding()) {
        return 12;
    }
    if (!testBase64()) {
        return 13;
    }
    if (!testFind()) {
        return 14;
    }
    if (!testAppend()) {
        return 15;
    }

    return 0;
}