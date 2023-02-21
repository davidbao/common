//
//  RectangleTest.cpp
//  common
//
//  Created by baowei on 2022/11/26.
//  Copyright (c) 20122 com. All rights reserved.
//

#include "data/Rectangle.h"

using namespace Drawing;

bool testRectangleConstructor() {
    {
        Rectangle test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 3, 4);
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 3, 4);
        Rectangle test2(test);
        if (!(test2.x == 1 && test2.y == 2 && test2.width == 3 && test2.height == 4)) {
            return false;
        }
    }

    {
        Rectangle test = Rectangle::makeLTRB(1, 2, 4, 6);
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test = Rectangle::makeLTRB(Point(1, 2), Point(4, 6));
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    return true;
}

bool testRectangleEmpty() {
    {
        Rectangle test(1, 2, 3, 4);
        if (test.isEmpty()) {
            return false;
        }
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testRectangleProperty() {
    {
        Rectangle test(1, 2, 3, 4);
        if (test.x != 1) {
            return false;
        }
        if (test.y != 2) {
            return false;
        }
        if (test.width != 3) {
            return false;
        }
        if (test.height != 4) {
            return false;
        }

        if (test.right() != 4) {
            return false;
        }
        if (test.bottom() != 6) {
            return false;
        }

        if (test.leftTop() != Point(1, 2)) {
            return false;
        }
        if (test.rightTop() != Point(4, 2)) {
            return false;
        }
        if (test.leftBottom() != Point(1, 6)) {
            return false;
        }
        if (test.rightBottom() != Point(4, 6)) {
            return false;
        }

        test.setLocation(5, 6);
        if (!(test.x == 5 && test.y == 6)) {
            return false;
        }

        test.setLocation(Point(7, 8));
        if (!(test.x == 7 && test.y == 8)) {
            return false;
        }

        test.setSize(5, 6);
        if (!(test.width == 5 && test.height == 6)) {
            return false;
        }

        test.setSize(Size(7, 8));
        if (!(test.width == 7 && test.height == 8)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        if (test.center() != Point(51, 42)) {
            return false;
        }
    }

    return true;
}

bool testRectangleContains() {
    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(1, 2)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(100, 81)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (test.contains(101, 82)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(Point(1, 2))) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(Point(100, 81))) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (test.contains(Point(101, 82))) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(Rectangle(1, 2, 2, 2))) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (!test.contains(Rectangle(100, 81, 1, 1))) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        if (test.contains(Rectangle(101, 82, 2, 2))) {
            return false;
        }
    }

    return true;
}

bool testRectangleInflate() {
    {
        Rectangle test(1, 2, 100, 80);
        test.inflate(1, 1);
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        test.inflate(Size(1, 1));
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        test = Rectangle::inflate(test, 1, 1);
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }

    return true;
}

bool testRectangleIntersect() {
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(200, 200, 100, 80);
        Rectangle test3 = Rectangle::intersect(test, test2);
        if (!test3.isEmpty()) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(1, 2, 10, 10);
        Rectangle test3 = Rectangle::intersect(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 10 && test3.height == 10)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(50, 40, 10, 10);
        Rectangle test3 = Rectangle::intersect(test, test2);
        if (!(test3.x == 50 && test3.y == 40 && test3.width == 10 && test3.height == 10)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(200, 200, 100, 80);
        test.intersect(test2);
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(1, 2, 10, 10);
        test.intersect(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 10 && test.height == 10)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(50, 40, 10, 10);
        test.intersect(test2);
        if (!(test.x == 50 && test.y == 40 && test.width == 10 && test.height == 10)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(200, 200, 100, 80);
        if (test.intersectsWith(test2)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(1, 2, 10, 10);
        if (!test.intersectsWith(test2)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(50, 40, 10, 10);
        if (!test.intersectsWith(test2)) {
            return false;
        }
    }

    return true;
}

bool testRectangleOffset() {
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2 = Rectangle::offset(test, 1, 2);
        if (!(test2.x == 2 && test2.y == 4 && test2.width == 100 && test2.height == 80)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        test.offset(1, 2);
        if (!(test.x == 2 && test.y == 4 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        test.offset(Point(1, 2));
        if (!(test.x == 2 && test.y == 4 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }

    return true;
}

bool testRectangleUnions() {
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(200, 200, 100, 80);
        Rectangle test3 = Rectangle::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 299 && test3.height == 278)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(1, 2, 10, 10);
        Rectangle test3 = Rectangle::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 100 && test3.height == 80)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(50, 40, 10, 10);
        Rectangle test3 = Rectangle::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 100 && test3.height == 80)) {
            return false;
        }
    }

    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(200, 200, 100, 80);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 299 && test.height == 278)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(1, 2, 10, 10);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }
    {
        Rectangle test(1, 2, 100, 80);
        Rectangle test2(50, 40, 10, 10);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }

    return true;
}

bool testRectangleRound() {
    {
        Rectangle test = Rectangle::ceiling(RectangleF(1.1f, 2.1f, 3.1f, 4.1f));
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }
    {
        Rectangle test = Rectangle::ceiling(RectangleF(1.6f, 2.6f, 3.6f, 4.6f));
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        Rectangle test = Rectangle::round(RectangleF(1.1f, 2.1f, 3.1f, 4.1f));
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test = Rectangle::round(RectangleF(1.6f, 2.6f, 3.6f, 4.6f));
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        Rectangle test = Rectangle::truncate(RectangleF(1.1f, 2.1f, 3.1f, 4.1f));
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test = Rectangle::truncate(RectangleF(1.6f, 2.6f, 3.6f, 4.6f));
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    return true;
}

bool testRectangleStaticVars() {
    {
        if (!Rectangle::Empty.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testRectangleToString() {
    {
        Rectangle test(1, 2, 3, 4);
        if (test.toString() != "1,2,3,4") {
            return false;
        }
    }

    return true;
}

bool testRectangleParse() {
    {
        Rectangle test;
        if (!Rectangle::parse("1,2,3,4", test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test;
        if (!Rectangle::parse("1, 2, 3, 4", test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    {
        Rectangle test;
        if (Rectangle::parse("x,2,3,4", test)) {
            return false;
        }
    }

    return true;
}

bool testRectanglesConstructor() {
    {
        Rectangles test;
        test.add(Rectangle(1, 2, 3, 4));
        test.add(Rectangle(3, 4, 5, 6));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        Rectangles test;
        test.add(Rectangle(1, 2, 3, 4));
        test.add(Rectangle(3, 4, 5, 6));
        Rectangles test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test2[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        Rectangles test{Rectangle(1, 2, 3, 4), Rectangle(3, 4, 5, 6)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }

    return true;
}

bool testRectanglesToString() {
    {
        Rectangles test{Rectangle(1, 2, 3, 4), Rectangle(3, 4, 5, 6)};
        if (test.toString() != "1,2,3,4;3,4,5,6") {
            return false;
        }
    }
    return true;
}

bool testRectanglesParse() {
    {
        String str = "1,2,3,4;3,4,5,6";
        Rectangles test;
        if (!Rectangles::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = " 1, 2, 3, 4; 3, 4, 5, 6";
        Rectangles test;
        if (!Rectangles::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = " 1, 2, 3, 4; 3, 4, 5, 6 ";
        Rectangles test;
        if (!Rectangles::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != Rectangle(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != Rectangle(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = "x, 2, 3, 4; 3, 4, 5, 6";
        Rectangles test;
        if (Rectangles::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFConstructor() {
    {
        RectangleF test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 3, 4);
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 3, 4);
        RectangleF test2(test);
        if (!(test2.x == 1 && test2.y == 2 && test2.width == 3 && test2.height == 4)) {
            return false;
        }
    }

    {
        RectangleF test = RectangleF::makeLTRB(1, 2, 4, 6);
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        RectangleF test = RectangleF::makeLTRB(PointF(1, 2), PointF(4, 6));
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFEmpty() {
    {
        RectangleF test(1, 2, 3, 4);
        if (test.isEmpty()) {
            return false;
        }
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testRectangleFProperty() {
    {
        RectangleF test(1, 2, 3, 4);
        if (test.x != 1) {
            return false;
        }
        if (test.y != 2) {
            return false;
        }
        if (test.width != 3) {
            return false;
        }
        if (test.height != 4) {
            return false;
        }

        if (test.right() != 4) {
            return false;
        }
        if (test.bottom() != 6) {
            return false;
        }

        if (test.leftTop() != PointF(1, 2)) {
            return false;
        }
        if (test.rightTop() != PointF(4, 2)) {
            return false;
        }
        if (test.leftBottom() != PointF(1, 6)) {
            return false;
        }
        if (test.rightBottom() != PointF(4, 6)) {
            return false;
        }

        test.setLocation(5, 6);
        if (!(test.x == 5 && test.y == 6)) {
            return false;
        }

        test.setLocation(Point(7, 8));
        if (!(test.x == 7 && test.y == 8)) {
            return false;
        }

        test.setSize(5, 6);
        if (!(test.width == 5 && test.height == 6)) {
            return false;
        }

        test.setSize(Size(7, 8));
        if (!(test.width == 7 && test.height == 8)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        if (test.center() != PointF(51, 42)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFContains() {
    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(1, 2)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(100, 81)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (test.contains(101, 82)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(Point(1, 2))) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(Point(100, 81))) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (test.contains(Point(101, 82))) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(RectangleF(1, 2, 2, 2))) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (!test.contains(RectangleF(100, 81, 1, 1))) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        if (test.contains(RectangleF(101, 82, 2, 2))) {
            return false;
        }
    }

    return true;
}

bool testRectangleFInflate() {
    {
        RectangleF test(1, 2, 100, 80);
        test.inflate(1, 1);
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        test.inflate(Size(1, 1));
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        test = RectangleF::inflate(test, 1, 1);
        if (!(test.x == 0 && test.y == 1 && test.width == 102 && test.height == 82)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFIntersect() {
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(200, 200, 100, 80);
        RectangleF test3 = RectangleF::intersect(test, test2);
        if (!test3.isEmpty()) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(1, 2, 10, 10);
        RectangleF test3 = RectangleF::intersect(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 10 && test3.height == 10)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(50, 40, 10, 10);
        RectangleF test3 = RectangleF::intersect(test, test2);
        if (!(test3.x == 50 && test3.y == 40 && test3.width == 10 && test3.height == 10)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(200, 200, 100, 80);
        test.intersect(test2);
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(1, 2, 10, 10);
        test.intersect(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 10 && test.height == 10)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(50, 40, 10, 10);
        test.intersect(test2);
        if (!(test.x == 50 && test.y == 40 && test.width == 10 && test.height == 10)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(200, 200, 100, 80);
        if (test.intersectsWith(test2)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(1, 2, 10, 10);
        if (!test.intersectsWith(test2)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(50, 40, 10, 10);
        if (!test.intersectsWith(test2)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFOffset() {
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2 = RectangleF::offset(test, 1, 2);
        if (!(test2.x == 2 && test2.y == 4 && test2.width == 100 && test2.height == 80)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        test.offset(1, 2);
        if (!(test.x == 2 && test.y == 4 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        test.offset(PointF(1, 2));
        if (!(test.x == 2 && test.y == 4 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFUnions() {
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(200, 200, 100, 80);
        RectangleF test3 = RectangleF::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 299 && test3.height == 278)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(1, 2, 10, 10);
        RectangleF test3 = RectangleF::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 100 && test3.height == 80)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(50, 40, 10, 10);
        RectangleF test3 = RectangleF::unions(test, test2);
        if (!(test3.x == 1 && test3.y == 2 && test3.width == 100 && test3.height == 80)) {
            return false;
        }
    }

    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(200, 200, 100, 80);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 299 && test.height == 278)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(1, 2, 10, 10);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }
    {
        RectangleF test(1, 2, 100, 80);
        RectangleF test2(50, 40, 10, 10);
        test.unions(test2);
        if (!(test.x == 1 && test.y == 2 && test.width == 100 && test.height == 80)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFRound() {
    {
        Rectangle test = RectangleF(1.1f, 2.1f, 3.1f, 4.1f).ceiling();
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }
    {
        Rectangle test = RectangleF(1.6f, 2.6f, 3.6f, 4.6f).ceiling();
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        Rectangle test = RectangleF(1.1f, 2.1f, 3.1f, 4.1f).round();
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test = RectangleF(1.6f, 2.6f, 3.6f, 4.6f).round();
        if (!(test.x == 2 && test.y == 3 && test.width == 4 && test.height == 5)) {
            return false;
        }
    }

    {
        Rectangle test = RectangleF(1.1f, 2.1f, 3.1f, 4.1f).truncate();
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        Rectangle test = RectangleF(1.6f, 2.6f, 3.6f, 4.6f).truncate();
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFStaticVars() {
    {
        if (!RectangleF::Empty.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testRectangleFToString() {
    {
        RectangleF test(1, 2, 3, 4);
        if (test.toString() != "1,2,3,4") {
            return false;
        }
    }

    return true;
}

bool testRectangleFParse() {
    {
        RectangleF test;
        if (!RectangleF::parse("1,2,3,4", test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }
    {
        RectangleF test;
        if (!RectangleF::parse("1, 2, 3, 4", test)) {
            return false;
        }
        if (!(test.x == 1 && test.y == 2 && test.width == 3 && test.height == 4)) {
            return false;
        }
    }

    {
        RectangleF test;
        if (RectangleF::parse("x,2,3,4", test)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFsConstructor() {
    {
        RectangleFs test;
        test.add(RectangleF(1, 2, 3, 4));
        test.add(RectangleF(3, 4, 5, 6));
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        RectangleFs test;
        test.add(RectangleF(1, 2, 3, 4));
        test.add(RectangleF(3, 4, 5, 6));
        RectangleFs test2(test);
        if (test2.count() != 2) {
            return false;
        }
        if (test2[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test2[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        RectangleFs test{RectangleF(1, 2, 3, 4), RectangleF(3, 4, 5, 6)};
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }

    return true;
}

bool testRectangleFsToString() {
    {
        RectangleFs test{RectangleF(1, 2, 3, 4), RectangleF(3, 4, 5, 6)};
        if (test.toString() != "1,2,3,4;3,4,5,6") {
            return false;
        }
    }
    return true;
}

bool testRectangleFsParse() {
    {
        String str = "1,2,3,4;3,4,5,6";
        RectangleFs test;
        if (!RectangleFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = " 1, 2, 3, 4; 3, 4, 5, 6";
        RectangleFs test;
        if (!RectangleFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = " 1, 2, 3, 4; 3, 4, 5, 6 ";
        RectangleFs test;
        if (!RectangleFs::parse(str, test)) {
            return false;
        }
        if (test.count() != 2) {
            return false;
        }
        if (test[0] != RectangleF(1, 2, 3, 4)) {
            return false;
        }
        if (test[1] != RectangleF(3, 4, 5, 6)) {
            return false;
        }
    }
    {
        String str = "x, 2, 3, 4; 3, 4, 5, 6";
        RectangleFs test;
        if (RectangleFs::parse(str, test)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testRectangleConstructor()) {
        return 1;
    }
    if (!testRectangleEmpty()) {
        return 2;
    }
    if (!testRectangleProperty()) {
        return 3;
    }
    if (!testRectangleContains()) {
        return 4;
    }
    if (!testRectangleInflate()) {
        return 5;
    }
    if (!testRectangleIntersect()) {
        return 6;
    }
    if (!testRectangleOffset()) {
        return 7;
    }
    if (!testRectangleUnions()) {
        return 8;
    }
    if (!testRectangleRound()) {
        return 9;
    }
    if (!testRectangleStaticVars()) {
        return 10;
    }
    if (!testRectangleToString()) {
        return 11;
    }
    if (!testRectangleParse()) {
        return 12;
    }

    if (!testRectanglesConstructor()) {
        return 13;
    }
    if (!testRectanglesToString()) {
        return 14;
    }
    if (!testRectanglesParse()) {
        return 15;
    }

    if (!testRectangleFConstructor()) {
        return 21;
    }
    if (!testRectangleFEmpty()) {
        return 22;
    }
    if (!testRectangleFProperty()) {
        return 23;
    }
    if (!testRectangleFContains()) {
        return 24;
    }
    if (!testRectangleFInflate()) {
        return 25;
    }
    if (!testRectangleFIntersect()) {
        return 26;
    }
    if (!testRectangleFOffset()) {
        return 27;
    }
    if (!testRectangleFUnions()) {
        return 28;
    }
    if (!testRectangleFRound()) {
        return 29;
    }
    if (!testRectangleFStaticVars()) {
        return 30;
    }
    if (!testRectangleFToString()) {
        return 31;
    }
    if (!testRectangleFParse()) {
        return 32;
    }

    if (!testRectangleFsConstructor()) {
        return 33;
    }
    if (!testRectangleFsToString()) {
        return 34;
    }
    if (!testRectangleFsParse()) {
        return 35;
    }

    return 0;
}
