//
//  WStringTest.cpp
//  common
//
//  Created by baowei on 2022/11/3.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/WString.h"
#include "IO/MemoryStream.h"

using namespace Common;

static const WString _text = L"ABC/abc123,)_中文";
static const wstring _text2 = L"ABC/abc123,)_中文";
static const wchar_t* _text3 = L"ABC/abc123,)_中文";
static const WString _tex4 = L"ABC/abc123,)_中文ABC/abc123,)_中文";

bool testConstructor() {
    WString str;
    if(str.length() != 0) {
        return false;
    }

    WString str2(_text);
    if(str2 != _text) {
        return false;
    }

    wstring text = _text.c_str();
    WString str3(text);
    if(str3 != _text) {
        return false;
    }

    WString str4(_text3);
    if(str4 != _text2) {
        return false;
    }

    WString str5(_text3, 5);
    if(str5 != L"ABC/a") {
        return false;
    }

    WString str6('a', 5);
    if(str6 != L"aaaaa") {
        return false;
    }

    WString str7(L'a');
    if(str7 != L"a") {
        return false;
    }

    return true;
}

bool testIsNullOrEmpty() {
    WString str;
    if(!str.isNullOrEmpty()) {
        return false;
    }

    return true;
}

bool testLength() {
    WString str = _text3;
    if(str.length() != wcslen(_text3)) {
        return false;
    }

    return true;
}

bool testEmpty() {
    WString str = _text;
    str.empty();
    if(str.length() != 0) {
        return false;
    }

    return true;
}

bool testOperators() {
    // operator const wchar_t*
    const wchar_t* str = _text;
    if(str != _text.c_str()) {
        return false;
    }

    // operator +=
    WString str2(_text);
    str2 += _text;
    if(str2 != _tex4) {
        return false;
    }

    WString str3(_text);
    str3 += _text2;
    if(str3 != _tex4) {
        return false;
    }

    WString str4(_text);
    str4 += _text3;
    if(str4 != _tex4) {
        return false;
    }

    // operator +
    WString str5(_text);
    str5 = str5 + _text;
    if(str5 != _tex4) {
        return false;
    }

    WString str6(_text);
    str6 = str6 + _text2;
    if(str6 != _tex4) {
        return false;
    }

    WString str7(_text);
    str7 = str7 + _text3;
    if(str7 != _tex4) {
        return false;
    }

    // operator =
    WString str8 = _text;
    if(str8 != _text) {
        return false;
    }

    WString str9 = _text2;
    if(str9 != _text2) {
        return false;
    }

    WString str10 = _text3;
    if(str10 != _text3) {
        return false;
    }

    // operator ==
    WString str11 = _text;
    if(!(str11 == _text)) {
        return false;
    }

    WString str12 = _text2;
    if(!(str12 == _text2)) {
        return false;
    }

    WString str13 = _text3;
    if(!(str13 == _text3)) {
        return false;
    }

    // operator !=
    WString str14 = _text;
    if(str14 != _text) {
        return false;
    }

    WString str15 = _text2;
    if(str15 != _text2) {
        return false;
    }

    WString str16 = _text3;
    if(str16 != _text3) {
        return false;
    }

    // operator >
    WString str17 = _text;
    str17.append('1');
    if(!(str17 > _text)) {
        return false;
    }

    WString str18 = _text2;
    str18.append('1');
    if(!(str18 > _text2)) {
        return false;
    }

    WString str19 = _text3;
    str19.append('1');
    if(!(str19 > _text3)) {
        return false;
    }

    // operator >=
    WString str20 = _text;
    str20.append('1');
    if(!(str20 >= _text)) {
        return false;
    }

    WString str21 = _text2;
    str21.append('1');
    if(!(str21 >= _text2)) {
        return false;
    }

    WString str22 = _text3;
    str22.append('1');
    if(!(str22 >= _text3)) {
        return false;
    }

    // operator <
    WString str23 = _text;
    str23[1] = 'A';
    if(!(str23 < _text)) {
        return false;
    }

    WString str24 = _text2;
    str24[1] = 'A';
    if(!(str24 < _text2)) {
        return false;
    }

    WString str25 = _text3;
    str25[1] = 'A';
    if(!(str25 < _text3)) {
        return false;
    }

    // operator <=
    WString str26 = _text;
    str26[1] = 'A';
    if(!(str26 <= _text)) {
        return false;
    }

    WString str27 = _text2;
    str27[1] = 'A';
    if(!(str27 <= _text2)) {
        return false;
    }

    WString str28 = _text3;
    str28[1] = 'A';
    if(!(str28 <= _text3)) {
        return false;
    }

    {
        WString str = _text3;
        String str2 = str;
        if(str2 != "ABC/abc123,)_中文") {
            return false;
        }
    }

    return true;
}

bool testGetter() {
    WString str = L"abc123";
    return str.at(3) == '1';
}

bool testSetter() {
    WString str = L"abc123";
    str[3] = L'2';
    if(str != L"abc223") {
        return false;
    }

    WString str2 = L"abc123";
    str2.set(3, L'2');
    if(str2 != L"abc223") {
        return false;
    }

    return true;
}

bool testStream() {
    MemoryStream ms;
    WString str(_text);
    str.write(&ms);
    ms.seek(0, SeekOrigin::SeekBegin);
    WString str2;
    str2.read(&ms);
    if(str != str2) {
        return false;
    }

    MemoryStream ms2;
    WString str3(_text);
    str3.write(&ms2, WString::StreamLength2);
    ms2.seek(0, SeekOrigin::SeekBegin);
    WString str4;
    str4.read(&ms2, WString::StreamLength2);
    if(str3 != str4) {
        return false;
    }

    MemoryStream ms3;
    WString str5(_text);
    str5.write(&ms3, WString::StreamLength4);
    ms3.seek(0, SeekOrigin::SeekBegin);
    WString str6;
    str6.read(&ms3, WString::StreamLength4);
    if(str5 != str6) {
        return false;
    }

    MemoryStream ms4;
    WString str7(_text);
    str7.writeFixedLengthStr(&ms4, str7.length());
    ms4.seek(0, SeekOrigin::SeekBegin);
    WString str8;
    str8.readFixedLengthStr(&ms4, str7.length());
    if(str7 != str8) {
        return false;
    }

    MemoryStream ms5;
    static const wchar_t text[] = L"0123456789";
    static const int count = 6554; // text.length * count greater than 64K
    WString str9;
    for (int i = 0; i < count; i++) {
        str9.append(text);
    }
    str9.writeFixedLengthStr(&ms5, str9.length());
    ms5.seek(0, SeekOrigin::SeekBegin);
    WString str10;
    str10.readFixedLengthStr(&ms5, str9.length());
    if(str9 != str10) {
        return false;
    }

    MemoryStream ms6;
    WString str11(_text);
    str11.writeFixedLengthStr(&ms6, str11.length() + 10);
    ms6.seek(0, SeekOrigin::SeekBegin);
    WString str12;
    str12.readFixedLengthStr(&ms6, str11.length() + 10);
    if(str11 != str12) {
        return false;
    }

    return true;
}

bool testStr() {
    WString str(_text);
    const wchar_t* text = str.c_str();
    if(_text != text) {
        return false;
    }

    return true;
}

bool testLowerAndUpper() {
    WString str(L"ABC123");
    WString str2 = str.toLower();
    if(str2 != L"abc123") {
        return false;
    }

    WString str3(L"abc123");
    WString str4 = str2.toUpper();
    if(str4 != L"ABC123") {
        return false;
    }

    return true;
}

bool testTrim() {
    WString str = L" abc 123 ";
    WString str2 = str.trim(L' ');
    if(str2 != L"abc 123") {
        return false;
    }

    WString banner = L"*** Much Ado About Nothing ***";
    WString result = banner.trim('*', ' ', '\'');
    if(result != L"Much Ado About Nothing") {
        return false;
    }

    WString lineWithLeadingSpaces = L"   Hello World!";
    WString lineAfterTrimStart = lineWithLeadingSpaces.trimStart(L' ');
    if(lineAfterTrimStart != L"Hello World!") {
        return false;
    }

    WString lineToBeTrimmed = L"__###__ John Smith";
    WString lineAfterTrimStart2 = lineToBeTrimmed.trimStart(L'_', L'#', L' ');
    if(lineAfterTrimStart2 != L"John Smith") {
        return false;
    }

    WString lineWithLeadingSpaces2 = L"Hello World!   ";
    WString lineAfterTrimEnd = lineWithLeadingSpaces2.trimEnd(L' ');
    if(lineAfterTrimEnd != L"Hello World!") {
        return false;
    }

    WString lineToBeTrimmed2 = L"John Smith__###__ ";
    WString lineAfterTrimEnd2 = lineToBeTrimmed2.trimEnd(L'_', L'#', L' ');
    if(lineAfterTrimEnd2 != L"John Smith") {
        return false;
    }

    return true;
}

bool testEncoding() {
    WString str(_text);
    WString str2;
    if(str.isUTF8()) {
        str2 = str.UTF8toGBK();
        str2 = str2.GBKtoUTF8();
    } else {
        str2 = str.GBKtoUTF8();
        str2 = str2.UTF8toGBK();
    }
    if(str2 != str) {
        return false;
    }

    return true;
}

bool testBase64() {
    WString str(_text);
    WString str2 = str.toBase64();
    if(str2 != L"QUJDL2FiYzEyMywpX+S4reaWhw==") {
        return false;
    }
    WString str3 = str2.fromBase64();
    if(str3 != str) {
        return false;
    }

    return true;
}

bool testFind() {
    WString str(_text);
    ssize_t pos = str.find(L"a");
    if(pos != 4) {
        return false;
    }
    ssize_t pos2 = str.find(L"a", 5);
    if(pos2 >= 0) {
        return false;
    }

    ssize_t pos3 = str.find(L'a');
    if(pos3 != 4) {
        return false;
    }
    ssize_t pos4 = str.find(L'a', 5);
    if(pos4 >= 0) {
        return false;
    }

    WString str2(L"abc1123");
    ssize_t pos5 = str2.find(L"1");
    if(pos5 != 3) {
        return false;
    }
    ssize_t pos6 = str2.findLastOf(L'1');
    if(pos6 != 4) {
        return false;
    }

    return true;
}

int main() {
    if(!testConstructor()) {
        return 1;
    }
    if(!testIsNullOrEmpty()) {
        return 2;
    }
    if(!testLength()) {
        return 3;
    }
    if(!testEmpty()) {
        return 4;
    }
    if(!testOperators()) {
        return 5;
    }
    if(!testGetter()) {
        return 6;
    }
    if(!testSetter()) {
        return 7;
    }
    if(!testStream()) {
        return 8;
    }
    if(!testStr()) {
        return 9;
    }
    if(!testLowerAndUpper()) {
        return 10;
    }
    if(!testTrim()) {
        return 11;
    }
    if(!testEncoding()) {
        return 12;
    }
    if(!testBase64()) {
        return 13;
    }
    if(!testFind()) {
        return 14;
    }

    return 0;
}