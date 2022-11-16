//
//  PListTest.cpp
//  common
//
//  Created by baowei on 2022/11/11.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/Point.h"
#include "data/Size.h"
#include "data/ValueType.h"

using namespace Drawing;

bool testPointConstructor() {
    {
        Point test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Point test(1, 1);
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == 1 && test.y == 1)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Point test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPointEquals() {
    {
        Point test(1, 1);
        Point test2(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testPointEvaluates() {
    {
        Point test(1, 1);
        Point test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testPointCompareTo() {
    {
        Point test(1, 1);
        Point test2(test);
        if (test2.compareTo(test) != 0) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Point test2(2, 2);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Point test2(0, 0);
        if (test2.compareTo(test) >= 0) {
            return false;
        }
    }

    return true;
}

bool testPointIsEmpty() {
    {
        Point test;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        if (test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPointEmpty() {
    {
        Point test(1, 1);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }

    return true;
}

bool testPointToString() {
    {
        Point test(1, 1);
        if (test.toString() != "1,1") {
            return false;
        }
    }

    return true;
}

bool testPointOperator() {
    // operator=
    {
        Point test(1, 1);
        Point test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    // operator<
    {
        Point test(1, 1);
        Point test2(test);
        if (test2 < test) {
            return false;
        }
    }
    // operator<=
    {
        Point test(1, 1);
        Point test2(2, 2);
        if (test2 <= test) {
            return false;
        }
    }
    // operator>
    {
        Point test(1, 1);
        Point test2(0, 0);
        if (test2 > test) {
            return false;
        }
    }
    // operator>=
    {
        Point test(1, 1);
        Point test2(0, 0);
        if (test2 >= test) {
            return false;
        }
    }

    // operator PointF
    {
        Point test(1, 1);
        PointF test2 = test;
        if (!(test2.x == 1.0f && test2.y == 1.0f)) {
            return false;
        }
    }
    // operator Size
    {
        Point test(1, 1);
        Size size = test;
        if (!(size.width == 1 && size.height == 1)) {
            return false;
        }
    }

    // operator+
    {
        Point test(1, 1);
        Size size(3, 4);
        Point test2 = test + size;
        if (!(test2.x == 4 && test2.y == 5)) {
            return false;
        }
    }
    // operator+=
    {
        Point test(1, 1);
        Size size(3, 4);
        test += size;
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }

    // operator-
    {
        Point test(1, 1);
        Size size(3, 4);
        Point test2 = test - size;
        if (!(test2.x == -2 && test2.y == -3)) {
            return false;
        }
    }
    // operator-=
    {
        Point test(1, 1);
        Size size(3, 4);
        test -= size;
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }

    return true;
}

bool testPointOffset() {
    {
        Point test(1, 1);
        Point test2 = Point::offset(test, 1, 1);
        if (!(test2.x == 2 && test2.y == 2)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Point pos(2, 3);
        Point test2 = Point::offset(test, pos);
        if (!(test2.x == 3 && test2.y == 4)) {
            return false;
        }
    }

    {
        Point test(1, 1);
        test.offset(1, 1);
        if (!(test.x == 2 && test.y == 2)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Point pos(2, 3);
        test.offset(pos);
        if (!(test.x == 3 && test.y == 4)) {
            return false;
        }
    }

    return true;
}

bool testPointParse() {
    {
        String str = "1,2";
        Point test;
        if (!Point::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2)) {
            return false;
        }
    }
    {
        String str = "1, 2";
        Point test;
        if (!Point::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2)) {
            return false;
        }
    }
    {
        String str = " 1, 2 ";
        Point test;
        if (!Point::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2)) {
            return false;
        }
    }
    {
        String str = " x, 2 ";
        Point test;
        if (Point::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testPointAdd() {
    {
        Point test(1, 1);
        Size size(3, 4);
        test.add(size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Size size(3, 4);
        test = Point::add(test, size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }

    {
        Point test(1, 1);
        Size size(3, 4);
        test.subtract(size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }
    {
        Point test(1, 1);
        Size size(3, 4);
        test = Point::subtract(test, size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }

    return true;
}

bool testPointStaticVars() {
    {
        Point test = Point::Empty;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }
    {
        Point test = Point::MinValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == Int32::MinValue && test.y == Int32::MinValue)) {
            return false;
        }
    }
    {
        Point test = Point::MaxValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == Int32::MaxValue && test.y == Int32::MaxValue)) {
            return false;
        }
    }

    return true;
}

bool testPointsConstructor() {
    {
        Points test;
        test.add(Point(1, 2));
        test.add(Point(3, 4));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Point(1, 2)) {
            return false;
        }
        if (test[1] != Point(3, 4)) {
            return false;
        }
    }
    {
        Points test;
        test.add(Point(1, 2));
        test.add(Point(3, 4));
        Points test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != Point(1, 2)) {
            return false;
        }
        if (test2[1] != Point(3, 4)) {
            return false;
        }
    }
    {
        Points test{Point(1, 2), Point(3, 4)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Point(1, 2)) {
            return false;
        }
        if (test[1] != Point(3, 4)) {
            return false;
        }
    }

    return true;
}

bool testPointsToString() {
    {
        Points test{Point(1, 2), Point(3, 4)};
        if (test.toString() != "1,2;3,4") {
            return false;
        }
    }
    return true;
}

bool testPointsParse() {
    {
        String str = "1,2;3,4";
        Points test;
        if (!Points::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Point(1, 2)) {
            return false;
        }
        if (test[1] != Point(3, 4)) {
            return false;
        }
    }
    {
        String str = "1, 2; 3, 4";
        Points test;
        if (!Points::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Point(1, 2)) {
            return false;
        }
        if (test[1] != Point(3, 4)) {
            return false;
        }
    }
    {
        String str = " 1, 2; 3, 4 ";
        Points test;
        if (!Points::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Point(1, 2)) {
            return false;
        }
        if (test[1] != Point(3, 4)) {
            return false;
        }
    }
    {
        String str = "x, 2; 3, 4";
        Points test;
        if (Points::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testPointFConstructor() {
    {
        PointF test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == 1.1f && test.y == 1.1f)) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPointFEquals() {
    {
        PointF test(1.1f, 1.1f);
        PointF test2(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testPointFEvaluates() {
    {
        PointF test(1.1f, 1.1f);
        PointF test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }

    return true;
}

bool testPointFCompareTo() {
    {
        PointF test(1.1f, 1.1f);
        PointF test2(test);
        if (test2.compareTo(test) != 0) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(2.1f, 2.1f);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(0, 0);
        if (test2.compareTo(test) >= 0) {
            return false;
        }
    }

    return true;
}

bool testPointFIsEmpty() {
    {
        PointF test;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        if (test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPointFEmpty() {
    {
        PointF test(1.1f, 1.1f);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }

    return true;
}

bool testPointFToString() {
    {
        PointF test(1.1f, 1.1f);
        if (test.toString() != "1.1,1.1") {
            return false;
        }
    }

    return true;
}

bool testPointFOperator() {
    {
        PointF test(1.1f, 1.1f);
        PointF test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    {
        PointF test(1.1f, 1.1f);
        PointF test2(test);
        if (test2 < test) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(2.1f, 2.1f);
        if (test2 <= test) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(0, 0);
        if (test2 > test) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF test2(0, 0);
        if (test2 >= test) {
            return false;
        }
    }

    return true;
}

bool testPointFOffset() {
    {
        PointF test(1.1f, 1.1f);
        PointF test2 = PointF::offset(test, 1.1f, 1.1f);
        if (!(test2.x == 2.2f && test2.y == 2.2f)) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF pos(2.f, 3.f);
        PointF test2 = PointF::offset(test, pos);
        if (!(test2.x == 3.1f && test2.y == 4.1f)) {
            return false;
        }
    }

    {
        PointF test(1.1f, 1.1f);
        test.offset(1.f, 1.f);
        if (!(test.x == 2.1f && test.y == 2.1f)) {
            return false;
        }
    }
    {
        PointF test(1.1f, 1.1f);
        PointF pos(2.f, 3.f);
        test.offset(pos);
        if (!(test.x == 3.1f && test.y == 4.1f)) {
            return false;
        }
    }

    return true;
}

bool testPointFParse() {
    {
        String str = "1.1,2.1";
        PointF test;
        if (!PointF::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1.1f && test.y == 2.1f)) {
            return false;
        }
    }
    {
        String str = "1.1, 2.1";
        PointF test;
        if (!PointF::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1.1f && test.y == 2.1f)) {
            return false;
        }
    }
    {
        String str = " 1.1, 2.1 ";
        PointF test;
        if (!PointF::parse(str, test)) {
            return false;
        }
        if (!(test.x == 1.1f && test.y == 2.1f)) {
            return false;
        }
    }
    {
        String str = " x, 2.1 ";
        PointF test;
        if (PointF::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testPointFAdd() {
    {
        PointF test(1, 1);
        Size size(3, 4);
        test.add(size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }
    {
        PointF test(1, 1);
        Size size(3, 4);
        test = PointF::add(test, size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }

    {
        PointF test(1, 1);
        Size size(3, 4);
        test.subtract(size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }
    {
        PointF test(1, 1);
        Size size(3, 4);
        test = PointF::subtract(test, size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }

    {
        PointF test(1, 1);
        SizeF size(3, 4);
        test.add(size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }
    {
        PointF test(1, 1);
        SizeF size(3, 4);
        test = PointF::add(test, size);
        if (!(test.x == 4 && test.y == 5)) {
            return false;
        }
    }

    {
        PointF test(1, 1);
        SizeF size(3, 4);
        test.subtract(size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }
    {
        PointF test(1, 1);
        SizeF size(3, 4);
        test = PointF::subtract(test, size);
        if (!(test.x == -2 && test.y == -3)) {
            return false;
        }
    }

    return true;
}

bool testPointFStaticVars() {
    {
        PointF test = PointF::Empty;
        if (!test.isEmpty()) {
            return false;
        }
        if (!(test.x == 0 && test.y == 0)) {
            return false;
        }
    }
    {
        PointF test = PointF::MinValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == Float::MinValue && test.y == Float::MinValue)) {
            return false;
        }
    }
    {
        PointF test = PointF::MaxValue;
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.x == Float::MaxValue && test.y == Float::MaxValue)) {
            return false;
        }
    }

    return true;
}

bool testPointFRound() {
    {
        PointF test(1.1f, 2.1f);
        Point test2 = test.round();
        if (!(test2.x == 1 && test2.y == 2)) {
            return false;
        }
    }
    {
        PointF test(1.6f, 2.7f);
        Point test2 = test.round();
        if (!(test2.x == 2 && test2.y == 3)) {
            return false;
        }
    }

    return true;
}

bool testPointFCeiling() {
    {
        PointF test(1.1f, 2.1f);
        Point test2 = test.ceiling();
        if (!(test2.x == 2 && test2.y == 3)) {
            return false;
        }
    }
    {
        PointF test(1.6f, 2.7f);
        Point test2 = test.ceiling();
        if (!(test2.x == 2 && test2.y == 3)) {
            return false;
        }
    }

    return true;
}

bool testPointFTruncate() {
    {
        PointF test(1.1f, 2.1f);
        Point test2 = test.truncate();
        if (!(test2.x == 1 && test2.y == 2)) {
            return false;
        }
    }
    {
        PointF test(1.6f, 2.7f);
        Point test2 = test.truncate();
        if (!(test2.x == 1 && test2.y == 2)) {
            return false;
        }
    }

    return true;
}

bool testPointFsConstructor() {
    {
        PointFs test;
        test.add(PointF(1.1f, 2.1f));
        test.add(PointF(3.1f, 4.1f));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        PointFs test;
        test.add(PointF(1.1f, 2.1f));
        test.add(PointF(3.1f, 4.1f));
        PointFs test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test2[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        PointFs test{PointF(1.1f, 2.1f), PointF(3.1f, 4.1f)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }

    return true;
}

bool testPointFsToString() {
    {
        PointFs test{PointF(1.1f, 2.1f), PointF(3.1f, 4.1f)};
        if (test.toString() != "1.1,2.1;3.1,4.1") {
            return false;
        }
    }
    return true;
}

bool testPointFsParse() {
    {
        String str = "1.1,2.1;3.1,4.1";
        PointFs test;
        if (!PointFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = "1.1, 2.1; 3.1, 4.1";
        PointFs test;
        if (!PointFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = " 1.1, 2.1; 3.1, 4.1 ";
        PointFs test;
        if (!PointFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != PointF(1.1f, 2.1f)) {
            return false;
        }
        if (test[1] != PointF(3.1f, 4.1f)) {
            return false;
        }
    }
    {
        String str = "x, 2.1; 3.1f, 4.1";
        PointFs test;
        if (PointFs::parse(str, test)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testPointConstructor()) {
        return 1;
    }
    if (!testPointEquals()) {
        return 2;
    }
    if (!testPointEvaluates()) {
        return 3;
    }
    if (!testPointCompareTo()) {
        return 4;
    }
    if (!testPointIsEmpty()) {
        return 5;
    }
    if (!testPointEmpty()) {
        return 6;
    }
    if (!testPointToString()) {
        return 7;
    }
    if (!testPointOperator()) {
        return 8;
    }
    if (!testPointOffset()) {
        return 9;
    }
    if (!testPointParse()) {
        return 10;
    }
    if (!testPointAdd()) {
        return 11;
    }
    if (!testPointStaticVars()) {
        return 12;
    }
    if (!testPointsConstructor()) {
        return 13;
    }
    if (!testPointsToString()) {
        return 14;
    }
    if (!testPointsParse()) {
        return 15;
    }

    if (!testPointFConstructor()) {
        return 21;
    }
    if (!testPointFEquals()) {
        return 22;
    }
    if (!testPointFEvaluates()) {
        return 23;
    }
    if (!testPointFCompareTo()) {
        return 24;
    }
    if (!testPointFIsEmpty()) {
        return 25;
    }
    if (!testPointFEmpty()) {
        return 26;
    }
    if (!testPointFToString()) {
        return 27;
    }
    if (!testPointFOperator()) {
        return 28;
    }
    if (!testPointFOffset()) {
        return 29;
    }
    if (!testPointFParse()) {
        return 30;
    }
    if (!testPointFAdd()) {
        return 31;
    }
    if (!testPointFStaticVars()) {
        return 32;
    }
    if (!testPointFRound()) {
        return 33;
    }
    if (!testPointFCeiling()) {
        return 34;
    }
    if (!testPointFTruncate()) {
        return 35;
    }
    if (!testPointFsConstructor()) {
        return 36;
    }
    if (!testPointFsToString()) {
        return 37;
    }
    if (!testPointFsParse()) {
        return 38;
    }

    return 0;
}