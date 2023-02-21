//
//  SizeTest.cpp
//  common
//
//  Created by baowei on 2022/11/12.
//  Copyright (c) 20122 com. All rights reserved.
//

#include "data/Size.h"
#include "data/ValueType.h"

using namespace Drawing;

bool testSizeConstructor() {
    {
        Size test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Size test(1, 1);
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == 1 && test.height == 1)) {
            return false;
        }
    }
    {
        Size test(1, 1);
        Size test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testSizeEquals() {
    {
        Size test(1, 1);
        Size test2(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testSizeEvaluates() {
    {
        Size test(1, 1);
        Size test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testSizeCompareTo() {
    {
        Size test(1, 1);
        Size test2(test);
        if (test2.compareTo(test) != 0) {
            return false;
        }
    }
    {
        Size test(1, 1);
        Size test2(2, 2);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }
    {
        Size test(1, 1);
        Size test2(0, 0);
        if (test2.compareTo(test) >= 0) {
            return false;
        }
    }

    return true;
}

bool testSizeIsEmpty() {
    {
        Size test;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }
    {
        Size test(1, 1);
        if (test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testSizeEmpty() {
    {
        Size test(1, 1);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }

    return true;
}

bool testSizeToString() {
    {
        Size test(1, 1);
        if (test.toString() != "1,1") {
            return false;
        }
    }

    return true;
}

bool testSizeOperator() {
    // operator=
    {
        Size test(1, 1);
        Size test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    // operator<
    {
        Size test(1, 1);
        Size test2(test);
        if (test2 < test) {
            return false;
        }
    }
    // operator<=
    {
        Size test(1, 1);
        Size test2(2, 2);
        if (test2 <= test) {
            return false;
        }
    }
    // operator>
    {
        Size test(1, 1);
        Size test2(0, 0);
        if (test2 > test) {
            return false;
        }
    }
    // operator>=
    {
        Size test(1, 1);
        Size test2(0, 0);
        if (test2 >= test) {
            return false;
        }
    }

    // operator SizeF
    {
        Size test(1, 1);
        SizeF test2 = test;
        if (!(test2.width == 1.0f && test2.height == 1.0f)) {
            return false;
        }
    }
    // operator Size
    {
        Size test(1, 1);
        Size size = test;
        if (!(size.width == 1 && size.height == 1)) {
            return false;
        }
    }

    // operator+
    {
        Size test(1, 1);
        Size size(3, 4);
        Size test2 = test + size;
        if (!(test2.width == 4 && test2.height == 5)) {
            return false;
        }
    }
    // operator+=
    {
        Size test(1, 1);
        Size size(3, 4);
        test += size;
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    // operator-
    {
        Size test(1, 1);
        Size size(3, 4);
        Size test2 = test - size;
        if (!(test2.width == -2 && test2.height == -3)) {
            return false;
        }
    }
    // operator-=
    {
        Size test(1, 1);
        Size size(3, 4);
        test -= size;
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }

    return true;
}

bool testSizeParse() {
    {
        String str = "1,2";
        Size test;
        if (!Size::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        String str = "1, 2";
        Size test;
        if (!Size::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        String str = " 1, 2 ";
        Size test;
        if (!Size::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        String str = " width, 2 ";
        Size test;
        if (Size::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testSizeAdd() {
    {
        Size test(1, 1);
        Size size(3, 4);
        test.add(size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }
    {
        Size test(1, 1);
        Size size(3, 4);
        test = Size::add(test, size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        Size test(1, 1);
        Size size(3, 4);
        test.subtract(size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }
    {
        Size test(1, 1);
        Size size(3, 4);
        test = Size::subtract(test, size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }

    {
        Size test(1, 2);
        test.multiply(2);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }
    {
        Size test(1, 2);
        test = Size::multiply(test, 2);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }

    {
        Size test(1, 2);
        test.multiply(2.1f);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }
    {
        Size test(1, 2);
        test = Size::multiply(test, 2.1f);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }

    {
        Size test(2, 4);
        test.division(2);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        Size test(2, 4);
        test = Size::division(test, 2);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }

    {
        Size test(2, 4);
        test.division(2.1f);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        Size test(2, 4);
        test = Size::division(test, 2.1f);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }

    return true;
}

bool testSizeStaticVars() {
    {
        Size test = Size::Empty;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }
    {
        Size test = Size::MinValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == Int32::MinValue && test.height == Int32::MinValue)) {
            return false;
        }
    }
    {
        Size test = Size::MaxValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == Int32::MaxValue && test.height == Int32::MaxValue)) {
            return false;
        }
    }

    return true;
}

bool testSizesConstructor() {
    {
        Sizes test;
        test.add(Size(1, 2));
        test.add(Size(3, 4));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Size(1, 2)) {
            return false;
        }
        if (test[1] != Size(3, 4)) {
            return false;
        }
    }
    {
        Sizes test;
        test.add(Size(1, 2));
        test.add(Size(3, 4));
        Sizes test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != Size(1, 2)) {
            return false;
        }
        if (test2[1] != Size(3, 4)) {
            return false;
        }
    }
    {
        Sizes test{Size(1, 2), Size(3, 4)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Size(1, 2)) {
            return false;
        }
        if (test[1] != Size(3, 4)) {
            return false;
        }
    }

    return true;
}

bool testSizesToString() {
    {
        Sizes test{Size(1, 2), Size(3, 4)};
        if (test.toString() != "1,2;3,4") {
            return false;
        }
    }
    return true;
}

bool testSizesParse() {
    {
        String str = "1,2;3,4";
        Sizes test;
        if (!Sizes::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Size(1, 2)) {
            return false;
        }
        if (test[1] != Size(3, 4)) {
            return false;
        }
    }
    {
        String str = "1, 2; 3, 4";
        Sizes test;
        if (!Sizes::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Size(1, 2)) {
            return false;
        }
        if (test[1] != Size(3, 4)) {
            return false;
        }
    }
    {
        String str = " 1, 2; 3, 4 ";
        Sizes test;
        if (!Sizes::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Size(1, 2)) {
            return false;
        }
        if (test[1] != Size(3, 4)) {
            return false;
        }
    }
    {
        String str = "width, 2; 3, 4";
        Sizes test;
        if (Sizes::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testSizeFConstructor() {
    {
        SizeF test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == 1.1f && test.height == 1.1f)) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testSizeFEquals() {
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testSizeFEvaluates() {
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testSizeFCompareTo() {
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(test);
        if (test2.compareTo(test) != 0) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(2.1f, 2.1f);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(0, 0);
        if (test2.compareTo(test) >= 0) {
            return false;
        }
    }

    return true;
}

bool testSizeFIsEmpty() {
    {
        SizeF test;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        if (test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testSizeFEmpty() {
    {
        SizeF test(1.1f, 1.1f);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }

    return true;
}

bool testSizeFToString() {
    {
        SizeF test(1.1f, 1.1f);
        if (test.toString() != "1.1,1.1") {
            return false;
        }
    }

    return true;
}

bool testSizeFOperator() {
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(test);
        if (test2 < test) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(2.1f, 2.1f);
        if (test2 <= test) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(0, 0);
        if (test2 > test) {
            return false;
        }
    }
    {
        SizeF test(1.1f, 1.1f);
        SizeF test2(0, 0);
        if (test2 >= test) {
            return false;
        }
    }

    return true;
}

bool testSizeFParse() {
    {
        String str = "1.1,2.1";
        SizeF test;
        if (!SizeF::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1.1f && test.height == 2.1f)) {
            return false;
        }
    }
    {
        String str = "1.1, 2.1";
        SizeF test;
        if (!SizeF::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1.1f && test.height == 2.1f)) {
            return false;
        }
    }
    {
        String str = " 1.1, 2.1 ";
        SizeF test;
        if (!SizeF::parse(str, test)) {
            return false;
        }
        if (!(test.width == 1.1f && test.height == 2.1f)) {
            return false;
        }
    }
    {
        String str = " width, 2.1 ";
        SizeF test;
        if (SizeF::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testSizeFAdd() {
    {
        SizeF test(1, 1);
        Size size(3, 4);
        test.add(size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }
    {
        SizeF test(1, 1);
        Size size(3, 4);
        test = SizeF::add(test, size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        SizeF test(1, 1);
        Size size(3, 4);
        test.subtract(size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }
    {
        SizeF test(1, 1);
        Size size(3, 4);
        test = SizeF::subtract(test, size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }

    {
        SizeF test(1, 1);
        SizeF size(3, 4);
        test.add(size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }
    {
        SizeF test(1, 1);
        SizeF size(3, 4);
        test = SizeF::add(test, size);
        if (!(test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        SizeF test(1, 1);
        SizeF size(3, 4);
        test.subtract(size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }
    {
        SizeF test(1, 1);
        SizeF size(3, 4);
        test = SizeF::subtract(test, size);
        if (!(test.width == -2 && test.height == -3)) {
            return false;
        }
    }

    {
        SizeF test(1, 2);
        test.multiply(2);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }
    {
        SizeF test(1, 2);
        test = SizeF::multiply(test, 2);
        if (!(test.width == 2 && test.height == 4)) {
            return false;
        }
    }

    {
        SizeF test(1, 2);
        test.multiply(2.1f);
        if (!(test.width == 2.1f && test.height == 4.2f)) {
            return false;
        }
    }
    {
        SizeF test(1, 2);
        test = SizeF::multiply(test, 2.1f);
        if (!(test.width == 2.1f && test.height == 4.2f)) {
            return false;
        }
    }

    {
        SizeF test(2, 4);
        test.division(2);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        SizeF test(2, 4);
        test = SizeF::division(test, 2);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }

    {
        SizeF test(2, 4);
        test.division(2.0f);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }
    {
        SizeF test(2, 4);
        test = SizeF::division(test, 2.0f);
        if (!(test.width == 1 && test.height == 2)) {
            return false;
        }
    }

    return true;
}

bool testSizeFStaticVars() {
    {
        SizeF test = SizeF::Empty;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.width == 0 && test.height == 0)) {
            return false;
        }
    }
    {
        SizeF test = SizeF::MinValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == Float::MinValue && test.height == Float::MinValue)) {
            return false;
        }
    }
    {
        SizeF test = SizeF::MaxValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.width == Float::MaxValue && test.height == Float::MaxValue)) {
            return false;
        }
    }

    return true;
}

bool testSizeFRound() {
    {
        SizeF test(1.1f, 2.1f);
        Size test2 = test.round();
        if (!(test2.width == 1 && test2.height == 2)) {
            return false;
        }
    }
    {
        SizeF test(1.6f, 2.7f);
        Size test2 = test.round();
        if (!(test2.width == 2 && test2.height == 3)) {
            return false;
        }
    }

    return true;
}

bool testSizeFCeiling() {
    {
        SizeF test(1.1f, 2.1f);
        Size test2 = test.ceiling();
        if (!(test2.width == 2 && test2.height == 3)) {
            return false;
        }
    }
    {
        SizeF test(1.6f, 2.7f);
        Size test2 = test.ceiling();
        if (!(test2.width == 2 && test2.height == 3)) {
            return false;
        }
    }

    return true;
}

bool testSizeFTruncate() {
    {
        SizeF test(1.1f, 2.1f);
        Size test2 = test.truncate();
        if (!(test2.width == 1 && test2.height == 2)) {
            return false;
        }
    }
    {
        SizeF test(1.6f, 2.7f);
        Size test2 = test.truncate();
        if (!(test2.width == 1 && test2.height == 2)) {
            return false;
        }
    }

    return true;
}

bool testSizeFsConstructor() {
    {
        SizeFs test;
        test.add(SizeF(1.1f, 2.1f));
        test.add(SizeF(3.1f, 4.1f));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        SizeFs test;
        test.add(SizeF(1.1f, 2.1f));
        test.add(SizeF(3.1f, 4.1f));
        SizeFs test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test2[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        SizeFs test{SizeF(1.1f, 2.1f), SizeF(3.1f, 4.1f)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }

    return true;
}

bool testSizeFsToString() {
    {
        SizeFs test{SizeF(1.1f, 2.1f), SizeF(3.1f, 4.1f)};
        if (test.toString() != "1.1,2.1;3.1,4.1") {
            return false;
        }
    }
    return true;
}

bool testSizeFsParse() {
    {
        String str = "1.1,2.1;3.1,4.1";
        SizeFs test;
        if (!SizeFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = "1.1, 2.1; 3.1, 4.1";
        SizeFs test;
        if (!SizeFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = " 1.1, 2.1; 3.1, 4.1 ";
        SizeFs test;
        if (!SizeFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != SizeF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != SizeF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = "width, 2.1; 3.1f, 4.1";
        SizeFs test;
        if (SizeFs::parse(str, test)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testSizeConstructor()) {
        return 1;
    }
    if (!testSizeEquals()) {
        return 2;
    }
    if (!testSizeEvaluates()) {
        return 3;
    }
    if (!testSizeCompareTo()) {
        return 4;
    }
    if (!testSizeIsEmpty()) {
        return 5;
    }
    if (!testSizeEmpty()) {
        return 6;
    }
    if (!testSizeToString()) {
        return 7;
    }
    if (!testSizeOperator()) {
        return 8;
    }
    if (!testSizeParse()) {
        return 10;
    }
    if (!testSizeAdd()) {
        return 11;
    }
    if (!testSizeStaticVars()) {
        return 12;
    }
    if (!testSizesConstructor()) {
        return 13;
    }
    if (!testSizesToString()) {
        return 14;
    }
    if (!testSizesParse()) {
        return 15;
    }

    if (!testSizeFConstructor()) {
        return 21;
    }
    if (!testSizeFEquals()) {
        return 22;
    }
    if (!testSizeFEvaluates()) {
        return 23;
    }
    if (!testSizeFCompareTo()) {
        return 24;
    }
    if (!testSizeFIsEmpty()) {
        return 25;
    }
    if (!testSizeFEmpty()) {
        return 26;
    }
    if (!testSizeFToString()) {
        return 27;
    }
    if (!testSizeFOperator()) {
        return 28;
    }
    if (!testSizeFParse()) {
        return 30;
    }
    if (!testSizeFAdd()) {
        return 31;
    }
    if (!testSizeFStaticVars()) {
        return 32;
    }
    if (!testSizeFRound()) {
        return 33;
    }
    if (!testSizeFCeiling()) {
        return 34;
    }
    if (!testSizeFTruncate()) {
        return 35;
    }
    if (!testSizeFsConstructor()) {
        return 36;
    }
    if (!testSizeFsToString()) {
        return 37;
    }
    if (!testSizeFsParse()) {
        return 38;
    }

    return 0;
}
