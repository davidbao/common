//
//  ValueTypeTest.cpp
//  common
//
//  Created by baowei on 2022/10/8.
//  Copyright © 2022 com. All rights reserved.
//

#include <cfloat>
#include <limits.h>
#include "data/ValueType.h"
#include "data/WString.h"
#include "system/Math.h"
#include "IO/MemoryStream.h"
#include "data/Culture.h"

using namespace Data;
using namespace System;

bool testBooleanConstructor() {
    {
        Boolean test;
        if (test) {
            return false;
        }
    }

    {
        Boolean test(false);
        if (test) {
            return false;
        }
    }

    {
        Boolean test(true);
        if (!test) {
            return false;
        }
    }

    {
        Boolean test(true);
        Boolean test2(test);
        if (!test2) {
            return false;
        }
    }

    return true;
}

bool testBooleanEquals() {
    {
        Boolean test(true);
        if (!test.equals(true)) {
            return false;
        }
    }

    {
        Boolean test(true);
        if (!test.equals(Boolean(true))) {
            return false;
        }
    }

    {
        Boolean test(true);
        if (test != true) {
            return false;
        }
    }

    {
        Boolean test(true);
        if (test == false) {
            return false;
        }
    }

    return true;
}

bool testBooleanEvaluates() {
    {
        Boolean test(true);
        Boolean test2;
        test2.evaluates(test);
        if (!test) {
            return false;
        }
    }

    {
        Boolean test(true);
        Boolean test2 = test;
        if (!test) {
            return false;
        }
    }

    {
        bool test = true;
        Boolean test2 = test;
        if (!test) {
            return false;
        }
    }

    return true;
}

bool testBooleanOperator() {
    {
        Boolean test(true);
        bool test2 = test;
        if (!test2) {
            return false;
        }
    }

    return true;
}

bool testBooleanStream() {
    {
        MemoryStream stream;
        Boolean test(true);
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Boolean test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testBooleanString() {
    {
        Boolean test(true);
        String str = test.toString();
        if (str != "true") {
            return false;
        }
    }

    {
        Boolean test(false);
        String str = test.toString();
        if (str != "false") {
            return false;
        }
    }

    return true;
}

bool testBooleanParse() {
    {
        Boolean test;
        if (!(Boolean::parse("true", test) && test)) {
            return false;
        }
    }

    {
        Boolean test;
        if (!(Boolean::parse("false", test) && !test)) {
            return false;
        }
    }

    {
        Boolean test;
        if (!(Boolean::parse("1", test) && test)) {
            return false;
        }
    }

    {
        Boolean test;
        if (!(Boolean::parse("2", test) && test)) {
            return false;
        }
    }

    {
        Boolean test;
        if (!(Boolean::parse("0", test) && !test)) {
            return false;
        }
    }

    {
        Boolean test;
        if (Boolean::parse("abc", test)) {
            return false;
        }
    }

    {
        bool test;
        if (!(Boolean::parse("true", test) && test)) {
            return false;
        }
    }

    {
        bool test;
        if (!(Boolean::parse("false", test) && !test)) {
            return false;
        }
    }

    {
        bool test;
        if (!(Boolean::parse("1", test) && test)) {
            return false;
        }
    }

    {
        bool test;
        if (!(Boolean::parse("2", test) && test)) {
            return false;
        }
    }

    {
        bool test;
        if (!(Boolean::parse("0", test) && !test)) {
            return false;
        }
    }

    {
        bool test;
        if (Boolean::parse("abc", test)) {
            return false;
        }
    }

    return true;
}

bool testBooleanValue() {
    {
        if (!Boolean::TrueValue) {
            return false;
        }
    }

    {
        if (Boolean::FalseValue) {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueConstructor() {
    {
        name test;
        if (test != (type) 0) {
            return false;
        }
    }

    {
        type value = 1;
        name test(value);
        if (test != value) {
            return false;
        }
    }

    {
        name test(1);
        name test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testBooleanEquals() {
    {
        type value = 1;
        name test(value);
        if (!test.equals(value)) {
            return false;
        }
    }

    {
        type value = 1;
        name test(value);
        if (!test.equals(name(value))) {
            return false;
        }
    }

    {
        type value = 1;
        name test(value);
        if (test != value) {
            return false;
        }
    }

    {
        type value = 1;
        name test(value);
        if (test != value) {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueEvaluates() {
    {
        name test(1);
        name test2;
        test2.evaluates(test);
        if (!test) {
            return false;
        }
    }

    {
        name test(1);
        name test2 = test;
        if (!test) {
            return false;
        }
    }

    {
        type test = 1;
        name test2 = test;
        if (!test) {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueOperator() {
    {
        name test(1);
        type test2 = test;
        if (!test2) {
            return false;
        }
    }
    {
        name test(1);
        test += 1;
        if (test != (type) 2) {
            return false;
        }
    }
    {
        name test(1);
        test -= 1;
        if (test != (type) 0) {
            return false;
        }
    }
    {
        name test(1);
        type value = test + (type) 1;
        if (value != (type) 2) {
            return false;
        }
    }
    {
        name test(1);
        type value = test - (type) 1;
        if (value != 0) {
            return false;
        }
    }
    {
        if (!(name(2) > name(1))) {
            return false;
        }
        if (!(name(2) >= name(1))) {
            return false;
        }
        if (!(name(1) < name(2))) {
            return false;
        }
        if (!(name(1) <= name(2))) {
            return false;
        }
    }
    {
        if (!(name(2) > type(1))) {
            return false;
        }
        if (!(name(2) >= type(1))) {
            return false;
        }
        if (!(name(1) < type(2))) {
            return false;
        }
        if (!(name(1) <= type(2))) {
            return false;
        }
    }
    {
        name test(1);
        type value = test++;
        if (!(test == (type) 2 && value == 1)) {
            return false;
        }
    }
    {
        name test(1);
        type value = test--;
        if (!(test == (type) 0 && value == 1)) {
            return false;
        }
    }
    {
        name test(1);
        type value = ++test;
        if (!(test == (type) 2 && value == 2)) {
            return false;
        }
    }
    {
        name test(1);
        type value = --test;
        if (!(test == (type) 0 && value == 0)) {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueStream() {
    {
        MemoryStream stream;
        name test(1);
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        name test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testCharString() {
    {
        Char test('0');
        String str = test.toString();
        if (str != "0") {
            return false;
        }
    }
    {
        Char test('1');
        String str = test.toString();
        if (str != "1") {
            return false;
        }
    }

    return true;
}

bool testWCharString() {
    {
        WChar test(L'0');
        WString str = test.toString();
        if (str != L"0") {
            return false;
        }
    }
    {
        WChar test(L'1');
        WString str = test.toString();
        if (str != L"1") {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueString() {
    {
        name test;
        String str = test.toString();
        if (str != "0") {
            return false;
        }
    }
    {
        name test(1);
        String str = test.toString();
        if (str != "1") {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueParse() {
    {
        name test;
        if (!(name::parse("0", test) && test == (type) 0)) {
            return false;
        }
    }
    {
        name test;
        if (!(name::parse("1", test) && test == (type) 1)) {
            return false;
        }
    }
    {
        name test;
        if (!(name::parse("0x7F", test, NumberStyles::NSHexNumber) && test == (type) 127)) {
            return false;
        }
    }

    return true;
}

bool testFloatString() {
    {
        Float test(1.2345f);
        String str = test.toString();
        if (str != "1.2345") {
            return false;
        }
    }
    {
        Float test(1.2345f);
        String str = test.toString("0.0");
        if (str != "1.2") {
            return false;
        }
    }
    {
        Float test(1.2345f);
        String str = test.toString("00.00");
        if (str != "01.23") {
            return false;
        }
    }

    {
        Double test(1.2345f);
        String str = test.toString();
        if (str != "1.2345") {
            return false;
        }
    }
    {
        Double test(1.2345f);
        String str = test.toString("0.0");
        if (str != "1.2") {
            return false;
        }
    }
    {
        Double test(1.2345f);
        String str = test.toString("00.00");
        if (str != "01.23") {
            return false;
        }
    }

    return true;
}

template<typename name, typename type>
bool testValueComparison() {
    {
        name test(1);
        name test2(2);
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        name test(2);
        name test2(1);
        if (test.compareTo(test2) <= 0) {
            return false;
        }
    }
    {
        name test(1);
        name test2(1);
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    return true;
}

bool testFormatString() {
    // "C" or "c"
    {
        Double test(1234.56);
        NumberFormatInfo info;
        info.currencyPositivePattern = 0;   // $n
        String str = test.toString("C", &info);
        if (str != "¤1,234.56") {
            return false;
        }
    }
    {
        Double test(1234.56);
        NumberFormatInfo info;
        info.currencyPositivePattern = 1;   // n$
        String str = test.toString("C", &info);
        if (str != "1,234.56¤") {
            return false;
        }
    }
    {
        Double test(1234.56);
        NumberFormatInfo info;
        info.currencyPositivePattern = 2;   // $ n
        String str = test.toString("C", &info);
        if (str != "¤ 1,234.56") {
            return false;
        }
    }
    {
        Double test(1234.56);
        NumberFormatInfo info;
        info.currencyPositivePattern = 3;   // n $
        String str = test.toString("C", &info);
        if (str != "1,234.56 ¤") {
            return false;
        }
    }

    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 0;   // ($n)
        String str = test.toString("C", &info);
        if (str != "(¤1,234.56)") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 1;   // -$n
        String str = test.toString("C", &info);
        if (str != "-¤1,234.56") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 2;   // $-n
        String str = test.toString("C", &info);
        if (str != "¤-1,234.56") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 3;   // $n-
        String str = test.toString("C", &info);
        if (str != "¤1,234.56-") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 4;   // (n$)
        String str = test.toString("C", &info);
        if (str != "(1,234.56¤)") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 5;   // -n$
        String str = test.toString("C", &info);
        if (str != "-1,234.56¤") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 6;   // n-$
        String str = test.toString("C", &info);
        if (str != "1,234.56-¤") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 7;   // n$-
        String str = test.toString("C", &info);
        if (str != "1,234.56¤-") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 8;   // -n $
        String str = test.toString("C", &info);
        if (str != "-1,234.56 ¤") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 9;   // -$ n
        String str = test.toString("C", &info);
        if (str != "-¤ 1,234.56") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 10;   // n $-
        String str = test.toString("C", &info);
        if (str != "1,234.56 ¤-") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 11;   // $ n-
        String str = test.toString("C", &info);
        if (str != "¤ 1,234.56-") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 12;   // $ -n
        String str = test.toString("C", &info);
        if (str != "¤ -1,234.56") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 13;   // n- $
        String str = test.toString("C", &info);
        if (str != "1,234.56- ¤") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 14;   // ($ n)
        String str = test.toString("C", &info);
        if (str != "(¤ 1,234.56)") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        NumberFormatInfo info;
        info.currencyNegativePattern = 15;   // (n $)
        String str = test.toString("C", &info);
        if (str != "(1,234.56 ¤)") {
            return false;
        }
    }

    {
        Double test(123.456);
        String str = test.toString("C");
        if (str != "¤123.46") {
            return false;
        }
    }
    {
        Double test(123.456);
        String str = test.toString("c");
        if (str != "¤123.46") {
            return false;
        }
    }
    {
        Double test(123.456);
        String str = test.toString("C3");
        if (str != "¤123.456") {
            return false;
        }
    }
    {
        Double test(123.456);
        String str = test.toString("c3");
        if (str != "¤123.456") {
            return false;
        }
    }
    {
        Double test(-123.456);
        String str = test.toString("C");
        if (str != "-¤123.46") {
            return false;
        }
    }
    {
        Double test(-123.456);
        String str = test.toString("c");
        if (str != "-¤123.46") {
            return false;
        }
    }
    {
        Double test(-123.456);
        String str = test.toString("C3");
        if (str != "-¤123.456") {
            return false;
        }
    }
    {
        Double test(-123.456);
        String str = test.toString("c3");
        if (str != "-¤123.456") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        String str = test.toString("C");
        if (str != "-¤1,234.56") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        String str = test.toString("c");
        if (str != "-¤1,234.56") {
            return false;
        }
    }

//    // zh-CN
//    {
//        Double test(1234.56);
//        Culture culture("zh-CN");
//        String str = test.toString("c", &culture);
//        if (str != "￥1,234.56") {
//            return false;
//        }
//    }
//    {
//        Double test(-1234.56);
//        Culture culture("zh-CN");
//        String str = test.toString("c", &culture);
//        if (str != "￥-1,234.56") {
//            return false;
//        }
//    }

//    // fr-FR
//    {
//        Double test(1234.56);
//        Culture culture("fr-FR");
//        String str = test.toString("c", &culture);
//        if (str != "1 234,56 Eu") {
//            return false;
//        }
//    }
//    {
//
//        Double test(-1234.56);
//        Culture culture("fr-FR");
//        String str = test.toString("c", &culture);
//        if (str != "1 234,56 Eu-") {
//            return false;
//        }
//    }

//    // ja-JP
//    {
//        Double test(1234.56);
//        Culture culture("ja-JP");
//        String str = test.toString("c", &culture);
//        if (str != "¥1,234.56") {
//            return false;
//        }
//    }
//    {
//
//        Double test(-1234.56);
//        Culture culture("ja-JP");
//        String str = test.toString("c", &culture);
//        if (str != "¥-1,234.56") {
//            return false;
//        }
//    }

    // "D" or "d"
    {
        Int32 test(1234);
        String str = test.toString("D");
        if (str != "1234") {
            return false;
        }
    }
    {
        Int32 test(1234);
        String str = test.toString("d");
        if (str != "1234") {
            return false;
        }
    }
    {
        Int32 test(-1234);
        String str = test.toString("D6");
        if (str != "-001234") {
            return false;
        }
    }
    {
        Int32 test(-1234);
        String str = test.toString("d6");
        if (str != "-001234") {
            return false;
        }
    }

    // "E" or "e"
    {
        Double test(1052.0329112756);
        String str = test.toString("E");
        if (str != "1.052033E+03") {
            return false;
        }
    }
    {
        Double test(1052.0329112756);
        String str = test.toString("e");
        if (str != "1.052033e+03") {
            return false;
        }
    }
    {
        Double test(-1052.0329112756);
        String str = test.toString("E2");
        if (str != "-1.05E+03") {
            return false;
        }
    }
    {
        Double test(-1052.0329112756);
        String str = test.toString("e2");
        if (str != "-1.05e+03") {
            return false;
        }
    }

    // "F" or "f"
    {
        Double test(1234.567);
        String str = test.toString("F");
        if (str != "1234.57") {
            return false;
        }
    }
    {
        Double test(1234);
        String str = test.toString("F1");
        if (str != "1234.0") {
            return false;
        }
    }
    {
        Double test(1234);
        String str = test.toString("f1");
        if (str != "1234.0") {
            return false;
        }
    }
    {
        Double test(1234.56);
        String str = test.toString("F4");
        if (str != "1234.5600") {
            return false;
        }
    }
    {
        Double test(1234.56);
        String str = test.toString("f4");
        if (str != "1234.5600") {
            return false;
        }
    }

    // "G" or "g"
    {
        Double test(-123.456);
        String str = test.toString("G");
        if (str != "-123.456") {
            return false;
        }
    }
    {
        Double test(-123.456);
        String str = test.toString("g");
        if (str != "-123.456") {
            return false;
        }
    }
    {
        Double test(123.4546);
        String str = test.toString("G4");
        if (str != "123.5") {
            return false;
        }
    }
    {
        Double test(123.4546);
        String str = test.toString("g4");
        if (str != "123.5") {
            return false;
        }
    }
    {
        Double test(-1.234567890e-25);
        String str = test.toString("G");
        if (str != "-1.23457E-25") {
            return false;
        }
    }
    {
        Double test(-1.234567890e-25);
        String str = test.toString("g");
        if (str != "-1.23457e-25") {
            return false;
        }
    }

    // "N" or "n"
    {
        Double test(1234.567);
        String str = test.toString("N");
        if (str != "1,234.57") {
            return false;
        }
    }
    {
        Double test(1234.567);
        String str = test.toString("n");
        if (str != "1,234.57") {
            return false;
        }
    }
    {
        Double test(1234);
        String str = test.toString("N1");
        if (str != "1,234.0") {
            return false;
        }
    }
    {
        Double test(1234);
        String str = test.toString("n1");
        if (str != "1,234.0") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        String str = test.toString("N3");
        if (str != "-1,234.560") {
            return false;
        }
    }
    {
        Double test(-1234.56);
        String str = test.toString("n3");
        if (str != "-1,234.560") {
            return false;
        }
    }

    // "P" or "p"
    {
        Double test(1);
        String str = test.toString("P");
        if (str != "100.00 %") {
            return false;
        }
    }
    {
        Double test(1);
        String str = test.toString("p");
        if (str != "100.00 %") {
            return false;
        }
    }
    {
        Double test(-0.39678);
        String str = test.toString("P1");
        if (str != "-39.7 %") {
            return false;
        }
    }
    {
        Double test(-0.39678);
        String str = test.toString("p1");
        if (str != "-39.7 %") {
            return false;
        }
    }

    // "X" or "x"
    {
        Int32 test(255);
        String str = test.toString("X");
        if (str != "FF") {
            return false;
        }
    }
    {
        Int32 test(255);
        String str = test.toString("x");
        if (str != "ff") {
            return false;
        }
    }
    {
        Int8 test(-1);
        String str = test.toString("X");
        if (str != "FF") {
            return false;
        }
    }
    {
        Int8 test(-1);
        String str = test.toString("x");
        if (str != "ff") {
            return false;
        }
    }
    {
        Int32 test(255);
        String str = test.toString("X4");
        if (str != "00FF") {
            return false;
        }
    }
    {
        Int32 test(255);
        String str = test.toString("x4");
        if (str != "00ff") {
            return false;
        }
    }
    {
        Int8 test(-1);
        String str = test.toString("X4");
        if (str != "00FF") {
            return false;
        }
    }
    {
        Int8 test(-1);
        String str = test.toString("x4");
        if (str != "00ff") {
            return false;
        }
    }

    return true;
}

bool testMinMaxString() {
//    {
//        Char test(Char::MinValue);
//        String str = test.toString();
//        if (str != "\x80") {
//            return false;
//        }
//    }
//    {
//        Char test(Char::MaxValue);
//        String str = test.toString();
//        if (str != "\U0000007f") {
//            return false;
//        }
//    }
//
//    {
//        WChar test(WChar::MinValue);
//        String str = test.toString();
//        String str2 = Int64(WCHAR_MIN).toString("D");
//        if (str != str2) {
//            return false;
//        }
//    }
//    {
//        WChar test(WChar::MaxValue);
//        String str = test.toString();
//        String str2 = Int64(WCHAR_MAX).toString("D");
//        if (str != str2) {
//            return false;
//        }
//    }

    {
        Int8 test(Int8::MinValue);
        String str = test.toString("D");
        if (str != "-128") {
            return false;
        }
    }
    {
        Int8 test(Int8::MaxValue);
        String str = test.toString("D");
        if (str != "127") {
            return false;
        }
    }

    {
        UInt8 test(UInt8::MinValue);
        String str = test.toString("D");
        if (str != "0") {
            return false;
        }
    }
    {
        UInt8 test(UInt8::MaxValue);
        String str = test.toString("D");
        if (str != "255") {
            return false;
        }
    }

    {
        Int16 test(Int16::MinValue);
        String str = test.toString("D");
        if (str != "-32768") {
            return false;
        }
    }
    {
        Int16 test(Int16::MaxValue);
        String str = test.toString("D");
        if (str != "32767") {
            return false;
        }
    }

    {
        UInt16 test(UInt16::MinValue);
        String str = test.toString("D");
        if (str != "0") {
            return false;
        }
    }
    {
        UInt16 test(UInt16::MaxValue);
        String str = test.toString("D");
        if (str != "65535") {
            return false;
        }
    }

    {
        Int32 test(Int32::MinValue);
        String str = test.toString("D");
        if (str != "-2147483648") {
            return false;
        }
    }
    {
        Int32 test(Int32::MaxValue);
        String str = test.toString("D");
        if (str != "2147483647") {
            return false;
        }
    }

    {
        UInt32 test(UInt32::MinValue);
        String str = test.toString("D");
        if (str != "0") {
            return false;
        }
    }
    {
        UInt32 test(UInt32::MaxValue);
        String str = test.toString("D");
        if (str != "4294967295") {
            return false;
        }
    }

    {
        Int64 test(Int64::MinValue);
        String str = test.toString("D");
        if (str != "-9223372036854775808") {
            return false;
        }
    }
    {
        Int64 test(Int64::MaxValue);
        String str = test.toString("D");
        if (str != "9223372036854775807") {
            return false;
        }
    }

    {
        UInt64 test(UInt64::MinValue);
        String str = test.toString("D");
        if (str != "0") {
            return false;
        }
    }
    {
        UInt64 test(UInt64::MaxValue);
        String str = test.toString("D");
        if (str != "18446744073709551615") {
            return false;
        }
    }

    {
        Float test(Float::MinValue);
        String str = test.toString("G");
        if (str != "-3.40282E+38") {
            return false;
        }
    }
    {
        Float test(Float::MaxValue);
        String str = test.toString("G");
        if (str != "3.40282E+38") {
            return false;
        }
    }

    {
        Double test(Double::MinValue);
        String str = test.toString("G");
        if (str != "-1.79769E+308") {
            return false;
        }
    }
    {
        Double test(Double::MaxValue);
        String str = test.toString("G");
        if (str != "1.79769E+308") {
            return false;
        }
    }

    return true;
}

bool testCharParse() {
    {
        Char test;
        if (!(Char::parse("A", test) && test == 'A')) {
            return false;
        }
    }

    return true;
}

bool testWCharParse() {
    {
        WChar test;
        if (!(WChar::parse("A", test) && test == L'A')) {
            return false;
        }
    }

    return true;
}

bool testCharComparison() {
    // toLower
    {
        Char test('A');
        Char test2 = test.toLower();
        if (test2 != 'a') {
            return false;
        }
    }

    // toUpper
    {
        Char test('a');
        Char test2 = test.toUpper();
        if (test2 != 'A') {
            return false;
        }
    }

    // isDigit
    {
        Char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (const auto& test: digits) {
            if (!test.isDigit()) {
                return false;
            }
        }
    }
    {
        Char test('a');
        if (test.isDigit()) {
            return false;
        }
    }

    // isLetter
    {
        Char test('a');
        if (!test.isLetter()) {
            return false;
        }
    }
    {
        Char test('B');
        if (!test.isLetter()) {
            return false;
        }
    }
    {
        Char test(',');
        if (test.isLetter()) {
            return false;
        }
    }

    // isWhiteSpace
    {
        Char test(' ');
        if (!test.isWhiteSpace()) {
            return false;
        }
    }
    {
        Char test('1');
        if (test.isWhiteSpace()) {
            return false;
        }
    }

    // isUpper
    {
        Char test('A');
        if (!test.isUpper()) {
            return false;
        }
    }
    {
        Char test('a');
        if (test.isUpper()) {
            return false;
        }
    }

    // isLower
    {
        Char test('a');
        if (!test.isLower()) {
            return false;
        }
    }
    {
        Char test('A');
        if (test.isLower()) {
            return false;
        }
    }

    // isLetterOrDigit
    {
        Char test('A');
        if (!test.isLetterOrDigit()) {
            return false;
        }
    }
    {
        Char test('9');
        if (!test.isLetterOrDigit()) {
            return false;
        }
    }
    {
        Char test('.');
        if (test.isLetterOrDigit()) {
            return false;
        }
    }

    // isSymbol
    {
        Char test('.');
        if (!test.isSymbol()) {
            return false;
        }
    }
    {
        Char test('A');
        if (test.isSymbol()) {
            return false;
        }
    }

    // isAscii
    {
        Char test('A');
        if (!test.isAscii()) {
            return false;
        }
    }
    {
        Char test((char) 0xC0);
        if (test.isAscii()) {
            return false;
        }
    }

    // toChar
    {
        Char test = Char::toChar(9);
        if (test != '9') {
            return false;
        }
    }
    {
        Char test = Char::toChar(0x0F);
        if (test != 'F') {
            return false;
        }
    }
    {
        Char test = Char::toChar(0x11);
        if ((uint8_t) test != 0xFF) {
            return false;
        }
    }

    // toHex
    {
        uint8_t test = Char::toHex('9');
        if (test != 0x09) {
            return false;
        }
    }
    {
        uint8_t test = Char::toHex('A');
        if (test != 0x0A) {
            return false;
        }
    }
    {
        uint8_t test = Char::toHex('a');
        if (test != 0x0A) {
            return false;
        }
    }
    {
        Char test = Char::toHex('.');
        if ((uint8_t) test != 0xFF) {
            return false;
        }
    }

    return true;
}

bool testWCharComparison() {
    // toLower
    {
        WChar test('A');
        WChar test2 = test.toLower();
        if (test2 != L'a') {
            return false;
        }
    }

    // toUpper
    {
        WChar test(L'a');
        WChar test2 = test.toUpper();
        if (test2 != L'A') {
            return false;
        }
    }

    // isDigit
    {
        WChar digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
        for (const auto& test: digits) {
            if (!test.isDigit()) {
                return false;
            }
        }
    }
    {
        WChar test(L'a');
        if (test.isDigit()) {
            return false;
        }
    }

    // isLetter
    {
        WChar test(L'a');
        if (!test.isLetter()) {
            return false;
        }
    }
    {
        WChar test(L'B');
        if (!test.isLetter()) {
            return false;
        }
    }
    {
        WChar test(L',');
        if (test.isLetter()) {
            return false;
        }
    }

    // isWhiteSpace
    {
        WChar test(L' ');
        if (!test.isWhiteSpace()) {
            return false;
        }
    }
    {
        WChar test(L'1');
        if (test.isWhiteSpace()) {
            return false;
        }
    }

    // isUpper
    {
        WChar test(L'A');
        if (!test.isUpper()) {
            return false;
        }
    }
    {
        WChar test(L'a');
        if (test.isUpper()) {
            return false;
        }
    }

    // isLower
    {
        WChar test(L'a');
        if (!test.isLower()) {
            return false;
        }
    }
    {
        WChar test(L'A');
        if (test.isLower()) {
            return false;
        }
    }

    // isLetterOrDigit
    {
        WChar test(L'A');
        if (!test.isLetterOrDigit()) {
            return false;
        }
    }
    {
        WChar test(L'9');
        if (!test.isLetterOrDigit()) {
            return false;
        }
    }
    {
        WChar test(L'.');
        if (test.isLetterOrDigit()) {
            return false;
        }
    }

    // isSymbol
    {
        WChar test(L'.');
        if (!test.isSymbol()) {
            return false;
        }
    }
    {
        WChar test(L'A');
        if (test.isSymbol()) {
            return false;
        }
    }

    // isAscii
    {
        WChar test(L'A');
        if (!test.isAscii()) {
            return false;
        }
    }
    {
        WChar test(0xC0);
        if (test.isAscii()) {
            return false;
        }
    }

    // toChar
    {
        WChar test = WChar::toChar(9);
        if (test != L'9') {
            return false;
        }
    }
    {
        WChar test = WChar::toChar(0x0F);
        if (test != L'F') {
            return false;
        }
    }
    {
        WChar test = WChar::toChar(0x11);
        if ((uint8_t) test != 0xFF) {
            return false;
        }
    }

    // toHex
    {
        uint8_t test = WChar::toHex(L'9');
        if (test != 0x09) {
            return false;
        }
    }
    {
        uint8_t test = WChar::toHex(L'A');
        if (test != 0x0A) {
            return false;
        }
    }
    {
        uint8_t test = WChar::toHex(L'a');
        if (test != 0x0A) {
            return false;
        }
    }
    {
        WChar test = WChar::toHex(L'.');
        if ((uint8_t) test != 0xFF) {
            return false;
        }
    }

    {
        WChar test(L'汉');
        if (!test.isHanzi()) {
            return false;
        }
    }
    {
        WChar test(L'A');
        if (test.isHanzi()) {
            return false;
        }
    }

    return true;
}

bool testFloatComparison() {
    // isNaN
    {
        Float test = Float::NaN;
        if (!test.isNaN()) {
            return false;
        }
        if (!Float::isNaN(Float::NaN)) {
            return false;
        }
        if (Float::isNaN(Float::PositiveInfinity)) {
            return false;
        }
        if (Float::isNaN(Float::NegativeInfinity)) {
            return false;
        }
        if (Float::isNaN(0.0f)) {
            return false;
        }
        if (Float::isNaN(Float::MinValue / 2.0f)) {
            return false;
        }
        float v = 0.0f;
        if (!Float::isNaN(v / v)) {
            return false;
        }
        if (!Float::isNaN(Float::PositiveInfinity - Float::PositiveInfinity)) {
            return false;
        }
    }

    // isFinite
    {
        if (!Float(0.0f).isFinite()) {
            return false;
        }
        if (Float(Math::exp(800.0f)).isFinite()) {
            return false;
        }
        if (Float::isFinite(Float::NaN)) {
            return false;
        }
        if (Float::isFinite(Float::PositiveInfinity)) {
            return false;
        }
        if (Float::isFinite(Float::NegativeInfinity)) {
            return false;
        }
        if (!Float::isFinite(0.0f)) {
            return false;
        }
        if (Float::isFinite(Math::exp(800.0f))) {
            return false;
        }
        if (!Float::isFinite(Float::MinValue / 2.0f)) {
            return false;
        }
    }

    // isInfinity
    {
        if (Float(0.0f).isInfinity()) {
            return false;
        }
        if (Float::isInfinity(Float::NaN)) {
            return false;
        }
        if (!Float::isInfinity(Float::PositiveInfinity)) {
            return false;
        }
        if (!Float::isInfinity(Float::NegativeInfinity)) {
            return false;
        }
        if (Float::isInfinity(0.0f)) {
            return false;
        }
        if (!Float::isInfinity(Math::exp(800.0f))) {
            return false;
        }
        if (Float::isInfinity(Float::MinValue / 2.0f)) {
            return false;
        }
    }

    // isNegative
    {
        Float test(-0.0f);
        if (!test.isNegative()) {
            return false;
        }
        if (Float::isNegative(+0.0f)) {
            return false;
        }
        if (!Float::isNegative(-0.0f)) {
            return false;
        }
    }

    // isNegativeInfinity
    {
        if (Float(0.0f).isNegativeInfinity()) {
            return false;
        }
        if (Float::isNegativeInfinity(Float::NaN)) {
            return false;
        }
        float v = 0.0f;
        if (!Float::isNegativeInfinity(-5.0f / v)) {
            return false;
        }
        if (!Float::isNegativeInfinity(Float::NegativeInfinity)) {
            return false;
        }
        if (Float::isNegativeInfinity(0.0f)) {
            return false;
        }
        if (!Float::isNegativeInfinity(-Math::exp(800.0f))) {
            return false;
        }
        if (Float::isNegativeInfinity(Float::MinValue / 2.0f)) {
            return false;
        }
    }

    // isPositiveInfinity
    {
        if (!Float(Float::PositiveInfinity).isPositiveInfinity()) {
            return false;
        }
        if (Float::isPositiveInfinity(Float::NaN)) {
            return false;
        }
        float v = 0.0f;
        if (!Float::isPositiveInfinity(5.0f / v)) {
            return false;
        }
        if (!Float::isPositiveInfinity(Float::PositiveInfinity)) {
            return false;
        }
        if (Float::isPositiveInfinity(0.0f)) {
            return false;
        }
        if (!Float::isPositiveInfinity(Math::exp(800.0f))) {
            return false;
        }
        if (Float::isPositiveInfinity(Float::MinValue / 2.0f)) {
            return false;
        }
    }

    // isNormal
    {
        Float test = 1.0f;
        if (!test.isNormal()) {
            return false;
        }
        if (Float::isNormal(Float::NaN)) {
            return false;
        }
        if (Float::isNormal(Float::PositiveInfinity)) {
            return false;
        }
        if (Float::isNormal(Float::NegativeInfinity)) {
            return false;
        }
        if (Float::isNormal(0.0f)) {
            return false;
        }
        if (!Float::isNormal(1.0f)) {
            return false;
        }
        if (!Float::isNormal(Float::MinValue / 2.0f)) {
            return false;
        }
    }

    // isSubnormal
    {
        if (Float(0.0f).isSubnormal()) {
            return false;
        }
        if (Float::isSubnormal(Float::NaN)) {
            return false;
        }
        if (Float::isSubnormal(Float::PositiveInfinity)) {
            return false;
        }
        if (Float::isSubnormal(Float::NegativeInfinity)) {
            return false;
        }
        if (Float::isSubnormal(0.0f)) {
            return false;
        }
        if (Float::isSubnormal(1.0f)) {
            return false;
        }
        if (!Float::isSubnormal(FLT_MIN / 2.0f)) {
            return false;
        }
    }

    return true;
}

bool testDoubleComparison() {
    // isNaN
    {
        Double test = Double::NaN;
        if (!test.isNaN()) {
            return false;
        }
        if (!Double::isNaN(Double::NaN)) {
            return false;
        }
        if (Double::isNaN(Double::PositiveInfinity)) {
            return false;
        }
        if (Double::isNaN(Double::NegativeInfinity)) {
            return false;
        }
        if (Double::isNaN(0.0)) {
            return false;
        }
        if (Double::isNaN(Double::MinValue / 2.0)) {
            return false;
        }
        double v = 0.0;
        if (!Double::isNaN(v / v)) {
            return false;
        }
        if (!Double::isNaN(Double::PositiveInfinity - Double::PositiveInfinity)) {
            return false;
        }
    }

    // isFinite
    {
        if (!Double(0.0).isFinite()) {
            return false;
        }
        if (Double(Math::exp(800.0)).isFinite()) {
            return false;
        }
        if (Double::isFinite(Double::NaN)) {
            return false;
        }
        if (Double::isFinite(Double::PositiveInfinity)) {
            return false;
        }
        if (Double::isFinite(Double::NegativeInfinity)) {
            return false;
        }
        if (!Double::isFinite(0.0)) {
            return false;
        }
        if (Double::isFinite(Math::exp(800.0))) {
            return false;
        }
        if (!Double::isFinite(Double::MinValue / 2.0)) {
            return false;
        }
    }

    // isInfinity
    {
        if (Double(0.0).isInfinity()) {
            return false;
        }
        if (Double::isInfinity(Double::NaN)) {
            return false;
        }
        if (!Double::isInfinity(Double::PositiveInfinity)) {
            return false;
        }
        if (!Double::isInfinity(Double::NegativeInfinity)) {
            return false;
        }
        if (Double::isInfinity(0.0)) {
            return false;
        }
        if (!Double::isInfinity(Math::exp(800.0))) {
            return false;
        }
        if (Double::isInfinity(Double::MinValue / 2.0)) {
            return false;
        }
    }

    // isNegative
    {
        Double test(-0.0);
        if (!test.isNegative()) {
            return false;
        }
        if (Double::isNegative(+0.0)) {
            return false;
        }
        if (!Double::isNegative(-0.0)) {
            return false;
        }
    }

    // isNegativeInfinity
    {
        if (Double(0.0).isNegativeInfinity()) {
            return false;
        }
        if (Double::isNegativeInfinity(Double::NaN)) {
            return false;
        }
        double v = 0.0;
        if (!Double::isNegativeInfinity(-5.0 / v)) {
            return false;
        }
        if (!Double::isNegativeInfinity(Double::NegativeInfinity)) {
            return false;
        }
        if (Double::isNegativeInfinity(0.0)) {
            return false;
        }
        if (!Double::isNegativeInfinity(-Math::exp(800.0))) {
            return false;
        }
        if (Double::isNegativeInfinity(Double::MinValue / 2.0)) {
            return false;
        }
    }

    // isPositiveInfinity
    {
        if (!Double(Double::PositiveInfinity).isPositiveInfinity()) {
            return false;
        }
        if (Double::isPositiveInfinity(Double::NaN)) {
            return false;
        }
        double v = 0.0;
        if (!Double::isPositiveInfinity(5.0 / v)) {
            return false;
        }
        if (!Double::isPositiveInfinity(Double::PositiveInfinity)) {
            return false;
        }
        if (Double::isPositiveInfinity(0.0)) {
            return false;
        }
        if (!Double::isPositiveInfinity(Math::exp(800.0))) {
            return false;
        }
        if (Double::isPositiveInfinity(Double::MinValue / 2.0)) {
            return false;
        }
    }

    // isNormal
    {
        Double test = 1.0f;
        if (!test.isNormal()) {
            return false;
        }
        if (Double::isNormal(Double::NaN)) {
            return false;
        }
        if (Double::isNormal(Double::PositiveInfinity)) {
            return false;
        }
        if (Double::isNormal(Double::NegativeInfinity)) {
            return false;
        }
        if (Double::isNormal(0.0)) {
            return false;
        }
        if (!Double::isNormal(1.0)) {
            return false;
        }
        if (!Double::isNormal(Double::MinValue / 2.0)) {
            return false;
        }
    }

    // isSubnormal
    {
        if (Double(0.0).isSubnormal()) {
            return false;
        }
        if (Double::isSubnormal(Double::NaN)) {
            return false;
        }
        if (Double::isSubnormal(Double::PositiveInfinity)) {
            return false;
        }
        if (Double::isSubnormal(Double::NegativeInfinity)) {
            return false;
        }
        if (Double::isSubnormal(0.0)) {
            return false;
        }
        if (Double::isSubnormal(1.0)) {
            return false;
        }
        if (!Double::isSubnormal(DBL_MIN / 2.0)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testBooleanConstructor()) {
        return 1;
    }
    if (!testBooleanEquals()) {
        return 2;
    }
    if (!testBooleanEvaluates()) {
        return 3;
    }
    if (!testBooleanOperator()) {
        return 4;
    }
    if (!testBooleanStream()) {
        return 5;
    }
    if (!testBooleanString()) {
        return 6;
    }
    if (!testBooleanParse()) {
        return 7;
    }
    if (!testBooleanValue()) {
        return 8;
    }

    if (!testValueConstructor<Char, char>()) {
        return 11;
    }
    if (!testBooleanEquals<Char, char>()) {
        return 12;
    }
    if (!testValueEvaluates<Char, char>()) {
        return 13;
    }
    if (!testValueOperator<Char, char>()) {
        return 14;
    }
    if (!testValueStream<Char, char>()) {
        return 15;
    }
    if (!testCharString()) {
        return 16;
    }
    if (!testCharParse()) {
        return 17;
    }
    if (!testValueComparison<Char, char>()) {
        return 18;
    }
    if (!testCharComparison()) {
        return 19;
    }

    if (!testValueConstructor<WChar, wchar_t>()) {
        return 21;
    }
    if (!testBooleanEquals<WChar, wchar_t>()) {
        return 22;
    }
    if (!testValueEvaluates<WChar, wchar_t>()) {
        return 23;
    }
    if (!testValueOperator<WChar, wchar_t>()) {
        return 24;
    }
    if (!testValueStream<WChar, wchar_t>()) {
        return 25;
    }
    if (!testWCharString()) {
        return 26;
    }
    if (!testWCharParse()) {
        return 27;
    }
    if (!testValueComparison<WChar, wchar_t>()) {
        return 28;
    }
    if (!testWCharComparison()) {
        return 29;
    }

    if (!testValueConstructor<Int8, int8_t>()) {
        return 31;
    }
    if (!testBooleanEquals<Int8, int8_t>()) {
        return 32;
    }
    if (!testValueEvaluates<Int8, int8_t>()) {
        return 33;
    }
    if (!testValueOperator<Int8, int8_t>()) {
        return 34;
    }
    if (!testValueStream<Int8, int8_t>()) {
        return 35;
    }
    if (!testValueString<Int8, int8_t>()) {
        return 36;
    }
    if (!testValueParse<Int8, int8_t>()) {
        return 37;
    }
    if (!testValueComparison<Int8, int8_t>()) {
        return 38;
    }

    if (!testValueConstructor<UInt8, uint8_t>()) {
        return 41;
    }
    if (!testBooleanEquals<UInt8, uint8_t>()) {
        return 42;
    }
    if (!testValueEvaluates<UInt8, uint8_t>()) {
        return 43;
    }
    if (!testValueOperator<UInt8, uint8_t>()) {
        return 44;
    }
    if (!testValueStream<UInt8, uint8_t>()) {
        return 45;
    }
    if (!testValueString<UInt8, uint8_t>()) {
        return 46;
    }
    if (!testValueParse<UInt8, uint8_t>()) {
        return 47;
    }
    if (!testValueComparison<UInt8, uint8_t>()) {
        return 48;
    }

    if (!testValueConstructor<Int16, int16_t>()) {
        return 51;
    }
    if (!testBooleanEquals<Int16, int16_t>()) {
        return 52;
    }
    if (!testValueEvaluates<Int16, int16_t>()) {
        return 53;
    }
    if (!testValueOperator<Int16, int16_t>()) {
        return 54;
    }
    if (!testValueStream<Int16, int16_t>()) {
        return 55;
    }
    if (!testValueString<Int16, int16_t>()) {
        return 56;
    }
    if (!testValueParse<Int16, int16_t>()) {
        return 57;
    }
    if (!testValueComparison<Int16, int16_t>()) {
        return 58;
    }

    if (!testValueConstructor<UInt16, uint16_t>()) {
        return 61;
    }
    if (!testBooleanEquals<UInt16, uint16_t>()) {
        return 62;
    }
    if (!testValueEvaluates<UInt16, uint16_t>()) {
        return 63;
    }
    if (!testValueOperator<UInt16, uint16_t>()) {
        return 64;
    }
    if (!testValueStream<UInt16, uint16_t>()) {
        return 65;
    }
    if (!testValueString<UInt16, uint16_t>()) {
        return 66;
    }
    if (!testValueParse<UInt16, uint16_t>()) {
        return 67;
    }
    if (!testValueComparison<UInt16, uint16_t>()) {
        return 68;
    }

    if (!testValueConstructor<Int32, int32_t>()) {
        return 71;
    }
    if (!testBooleanEquals<Int32, int32_t>()) {
        return 72;
    }
    if (!testValueEvaluates<Int32, int32_t>()) {
        return 73;
    }
    if (!testValueOperator<Int32, int32_t>()) {
        return 74;
    }
    if (!testValueStream<Int32, int32_t>()) {
        return 75;
    }
    if (!testValueString<Int32, int32_t>()) {
        return 76;
    }
    if (!testValueParse<Int32, int32_t>()) {
        return 77;
    }
    if (!testValueComparison<Int32, int32_t>()) {
        return 78;
    }

    if (!testValueConstructor<UInt32, uint32_t>()) {
        return 81;
    }
    if (!testBooleanEquals<UInt32, uint32_t>()) {
        return 82;
    }
    if (!testValueEvaluates<UInt32, uint32_t>()) {
        return 83;
    }
    if (!testValueOperator<UInt32, uint32_t>()) {
        return 84;
    }
    if (!testValueStream<UInt32, uint32_t>()) {
        return 85;
    }
    if (!testValueString<UInt32, uint32_t>()) {
        return 86;
    }
    if (!testValueParse<UInt32, uint32_t>()) {
        return 87;
    }
    if (!testValueComparison<UInt32, uint32_t>()) {
        return 88;
    }

    if (!testValueConstructor<Int64, int64_t>()) {
        return 91;
    }
    if (!testBooleanEquals<Int64, int64_t>()) {
        return 92;
    }
    if (!testValueEvaluates<Int64, int64_t>()) {
        return 93;
    }
    if (!testValueOperator<Int64, int64_t>()) {
        return 94;
    }
    if (!testValueStream<Int64, int64_t>()) {
        return 95;
    }
    if (!testValueString<Int64, int64_t>()) {
        return 96;
    }
    if (!testValueParse<Int64, int64_t>()) {
        return 97;
    }
    if (!testValueComparison<Int64, int64_t>()) {
        return 98;
    }

    if (!testValueConstructor<UInt64, uint64_t>()) {
        return 101;
    }
    if (!testBooleanEquals<UInt64, uint64_t>()) {
        return 102;
    }
    if (!testValueEvaluates<UInt64, uint64_t>()) {
        return 103;
    }
    if (!testValueOperator<UInt64, uint64_t>()) {
        return 104;
    }
    if (!testValueStream<UInt64, uint64_t>()) {
        return 105;
    }
    if (!testValueString<UInt64, uint64_t>()) {
        return 106;
    }
    if (!testValueParse<UInt64, uint64_t>()) {
        return 107;
    }
    if (!testValueComparison<UInt64, uint64_t>()) {
        return 108;
    }

    if (!testValueConstructor<Float, float>()) {
        return 111;
    }
    if (!testBooleanEquals<Float, float>()) {
        return 112;
    }
    if (!testValueEvaluates<Float, float>()) {
        return 113;
    }
    if (!testValueOperator<Float, float>()) {
        return 114;
    }
    if (!testValueStream<Float, float>()) {
        return 115;
    }
    if (!testValueString<Float, float>()) {
        return 116;
    }
    if (!testValueParse<Float, float>()) {
        return 117;
    }
    if (!testValueComparison<Float, float>()) {
        return 118;
    }

    if (!testValueConstructor<Double, double>()) {
        return 121;
    }
    if (!testBooleanEquals<Double, double>()) {
        return 122;
    }
    if (!testValueEvaluates<Double, double>()) {
        return 123;
    }
    if (!testValueOperator<Double, double>()) {
        return 124;
    }
    if (!testValueStream<Double, double>()) {
        return 125;
    }
    if (!testValueString<Double, double>()) {
        return 126;
    }
    if (!testValueParse<Double, double>()) {
        return 127;
    }
    if (!testValueComparison<Double, double>()) {
        return 128;
    }

    if (!testFormatString()) {
        return 131;
    }
    if (!testMinMaxString()) {
        return 132;
    }
    if (!testFloatString()) {
        return 133;
    }
    if (!testFloatComparison()) {
        return 134;
    }
    if (!testDoubleComparison()) {
        return 135;
    }

    return 0;
}