//
//  Point.cpp
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#include <cfloat>
#include "data/Point.h"
#include "system/Math.h"
#include "data/Convert.h"
#include "data/ValueType.h"
#include "data/Size.h"

using namespace System;

namespace Drawing {
    const PointF PointF::Empty;
    const PointF PointF::MinValue = PointF(-FLT_MAX, -FLT_MAX);
    const PointF PointF::MaxValue = PointF(FLT_MAX, FLT_MAX);

    PointF::PointF(float x, float y) {
        this->x = x;
        this->y = y;
    }

    PointF::PointF(int x, int y) {
        this->x = (float) x;
        this->y = (float) y;
    }

    PointF::PointF(long x, long y) {
        this->x = (float) x;
        this->y = (float) y;
    }

    PointF::PointF(const PointF &point) {
        this->x = point.x;
        this->y = point.y;
    }

    PointF::~PointF() = default;

    bool PointF::equals(const PointF &other) const {
        return this->x == other.x && this->y == other.y;
    }

    void PointF::evaluates(const PointF &other) {
        this->x = other.x;
        this->y = other.y;
    }

    int PointF::compareTo(const PointF &other) const {
        if (x != other.x) {
            if (x > other.x) {
                return 1;
            }
            return -1;
        }
        if (y != other.y) {
            if (y > other.y) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool PointF::isEmpty() const {
        return x == 0.0f && y == 0.0f;
    }

    void PointF::empty() {
        x = 0.0f;
        y = 0.0f;
    }

    String PointF::toString() const {
        return String::convert("%g,%g", x, y);
    }

    PointF &PointF::operator=(const PointF &value) {
        this->x = value.x;
        this->y = value.y;
        return *this;
    }

    PointF PointF::operator+=(const Size &other) {
        *this = add(*this, other);
        return *this;
    }

    PointF PointF::operator+(const Size &other) const {
        return add(*this, other);
    }

    PointF PointF::operator-=(const Size &other) {
        *this = subtract(*this, other);
        return *this;
    }

    PointF PointF::operator-(const Size &other) const {
        return subtract(*this, other);
    }

    PointF PointF::operator+=(const SizeF &other) {
        *this = add(*this, other);
        return *this;
    }

    PointF PointF::operator+(const SizeF &other) const {
        return add(*this, other);
    }

    PointF PointF::operator-=(const SizeF &other) {
        *this = subtract(*this, other);
        return *this;
    }

    PointF PointF::operator-(const SizeF &other) const {
        return subtract(*this, other);
    }

    PointF::operator Point() const {
        return Point::round(*this);
    }

    PointF::operator SizeF() const {
        return SizeF(x, y);
    }

    void PointF::offset(const PointF &pos) {
        *this = PointF::offset(*this, pos);
    }

    void PointF::offset(float dx, float dy) {
        *this = PointF::offset(*this, dx, dy);
    }

    void PointF::add(const Size &size) {
        *this = PointF::add(*this, size);
    }

    void PointF::subtract(const Size &size) {
        *this = PointF::subtract(*this, size);
    }

    void PointF::add(const SizeF &size) {
        *this = PointF::add(*this, size);
    }

    void PointF::subtract(const SizeF &size) {
        *this = PointF::subtract(*this, size);
    }

    void PointF::add(const PointF &point) {
        *this = PointF::add(*this, point);
    }

    void PointF::subtract(const PointF &point) {
        *this = PointF::subtract(*this, point);
    }

    void PointF::add(const Point &point) {
        PointF pt = point;
        add(pt);
    }

    void PointF::subtract(const Point &point) {
        PointF pt = point;
        subtract(pt);
    }

    Point PointF::ceiling() const {
        return Point::ceiling(*this);
    }

    Point PointF::round() const {
        return Point::round(*this);
    }

    Point PointF::truncate() const{
        return Point::truncate(*this);
    }

    bool PointF::parse(const String &str, PointF &point) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 2) {
            PointF temp;
            if (Float::parse(texts[0].trim(), temp.x) &&
                Float::parse(texts[1].trim(), temp.y)) {
                point = temp;
                return true;
            }
        }
        return false;
    }

    PointF PointF::offset(const PointF &point, const PointF &pos) {
        return offset(point, pos.x, pos.y);
    }

    PointF PointF::offset(const PointF &point, float dx, float dy) {
        return PointF(point.x + dx, point.y + dy);
    }

    PointF PointF::add(const PointF &point, const Size &size) {
        return PointF(point.x + (float)size.width, point.x + (float)size.height);
    }

    PointF PointF::subtract(const PointF &point, const Size &size) {
        return PointF(point.x - (float)size.width, point.x - (float)size.height);
    }

    PointF PointF::add(const PointF &pt1, const PointF &pt2) {
        return PointF(pt1.x + pt2.x, pt2.x + pt2.y);
    }

    PointF PointF::subtract(const PointF &pt1, const PointF &pt2) {
        return PointF(pt1.x - pt2.x, pt2.x - pt2.y);
    }

    PointF PointF::add(const PointF &point, const SizeF &size) {
        return PointF(point.x + size.width, point.x + size.height);
    }

    PointF PointF::subtract(const PointF &point, const SizeF &size) {
        return PointF(point.x - size.width, point.x - size.height);
    }

    PointFs::PointFs(size_t capacity) : Vector<PointF>(capacity) {
    }

    PointFs::PointFs(const PointFs &array) = default;

    PointFs::PointFs(std::initializer_list<PointF> list) : Vector<PointF>(list) {
    }

    // point:{0, 0};point:{300, 300}
    // 0, 0;300, 300
    bool PointFs::parse(const String &str, PointFs &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                PointF point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "point", true) &&
                            PointF::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (PointF::parse(texts[i], point)) {
                        points.add(point);
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    String PointFs::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const PointF &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }

    const Point Point::Empty;
    const Point Point::MinValue = Point(INT32_MIN, INT32_MIN);
    const Point Point::MaxValue = Point(INT32_MAX, INT32_MAX);

    Point::Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point::Point(const Point &point) {
        this->x = point.x;
        this->y = point.y;
    }

    Point::Point(const Size &size) {
        this->x = size.width;
        this->y = size.height;
    }

    Point::~Point() = default;

    bool Point::equals(const Point &other) const {
        return this->x == other.x && this->y == other.y;
    }

    void Point::evaluates(const Point &other) {
        this->x = other.x;
        this->y = other.y;
    }

    int Point::compareTo(const Point &other) const {
        if (x != other.x) {
            if (x > other.x) {
                return 1;
            }
            return -1;
        }
        if (y != other.y) {
            if (y > other.y) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool Point::isEmpty() const {
        return x == 0 && y == 0;
    }

    void Point::empty() {
        x = 0;
        y = 0;
    }

    String Point::toString() const {
        return String::convert("%d,%d", x, y);
    }

    Point &Point::operator=(const Point &other) {
        this->x = other.x;
        this->y = other.y;
        return *this;
    }

    Point Point::operator+=(const Size &other) {
        *this = add(*this, other);
        return *this;
    }

    Point Point::operator+(const Size &other) const {
        return add(*this, other);
    }

    Point Point::operator-=(const Size &other) {
        *this = subtract(*this, other);
        return *this;
    }

    Point Point::operator-(const Size &other) const {
        return subtract(*this, other);
    }

    Point Point::operator+=(const Point &other) {
        *this = add(*this, other);
        return *this;
    }

    Point Point::operator+(const Point &other) const {
        return add(*this, other);
    }

    Point Point::operator-=(const Point &other) {
        *this = subtract(*this, other);
        return *this;
    }

    Point Point::operator-(const Point &other) const {
        return subtract(*this, other);
    }

    Point::operator PointF() const {
        return PointF((float)x, (float)y);
    }

    Point::operator Size() const {
        return Size(x, y);
    }

    void Point::offset(const Point &pos) {
        *this = Point::offset(*this, pos);
    }

    void Point::offset(int dx, int dy) {
        *this = Point::offset(*this, dx, dy);
    }

    void Point::add(const Size &size) {
        *this = Point::add(*this, size);
    }

    void Point::subtract(const Size &size) {
        *this = Point::subtract(*this, size);
    }

    bool Point::parse(const String &str, Point &point) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 2) {
            Point temp;
            if (Int32::parse(texts[0].trim(), temp.x) &&
                Int32::parse(texts[1].trim(), temp.y)) {
                point = temp;
                return true;
            }
        }
        return false;
    }

    Point Point::offset(const Point &point, const Point &pos) {
        return offset(point, pos.x, pos.y);
    }

    Point Point::offset(const Point &point, int dx, int dy) {
        return Point(point.x + dx, point.y + dy);
    }

    Point Point::add(const Point &point, const Size &size) {
        return Point(point.x + size.width, point.y + size.height);
    }

    Point Point::subtract(const Point &point, const Size &size) {
        return Point(point.x - size.width, point.y - size.height);
    }

    Point Point::add(const Point &pt1, const Point &pt2) {
        return Point(pt1.x + pt2.x, pt1.y + pt2.y);
    }

    Point Point::subtract(const Point &pt1, const Point &pt2) {
        return Point(pt1.x - pt2.x, pt1.y - pt2.y);
    }

    Point Point::ceiling(const PointF &point) {
        return Point((int) Math::ceiling(point.x), (int) Math::ceiling(point.y));
    }

    Point Point::round(const PointF &point) {
        return Point((int) Math::round(point.x), (int) Math::round(point.y));
    }

    Point Point::truncate(const PointF &point) {
        return Point((int) point.x, (int) point.y);
    }

    Points::Points(size_t capacity) : Vector<Point>(capacity) {
    }

    Points::Points(const Points &array) = default;

    Points::Points(std::initializer_list<Point> list) : Vector<Point>(list) {
    }

    // point:{0, 0};point:{300, 300}
    // 0, 0;300, 300
    bool Points::parse(const String &str, Points &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                Point point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "point", true) &&
                            Point::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (Point::parse(texts[i], point)) {
                        points.add(point);
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }
        return false;
    }

    String Points::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const Point &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }
}
