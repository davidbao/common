//
//  Point.cpp
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#include "data/Point.h"
#include "system/Math.h"
#include "data/Convert.h"
#include "data/ValueType.h"

namespace Common {
    const PointF PointF::Empty;
    const PointF PointF::MinValue = PointF(Float::MinValue, Float::MinValue);
    const PointF PointF::MaxValue = PointF(Float::MaxValue, Float::MaxValue);

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

    void PointF::operator=(const PointF &value) {
        this->x = value.x;
        this->y = value.y;
    }

    bool PointF::operator==(const PointF &value) const {
        return this->x == value.x && this->y == value.y;
    }

    bool PointF::operator!=(const PointF &value) const {
        return !operator==(value);
    }

    void PointF::offset(const PointF &pos) {
        offset(pos.x, pos.y);
    }

    void PointF::offset(float dx, float dy) {
        x += dx;
        y += dy;
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

    Point PointF::round() const {
        return Point((int) Math::round(x), (int) Math::round(y));
    }

    PointFs::PointFs(uint capacity) : Vector<PointF>(capacity) {
    }

    PointFs::PointFs(const PointFs &array) : Vector<PointF>(array) {
    }

    // point:{0, 0};point:{300, 300}
    // 0, 0;300, 300
    bool PointFs::parse(const String &str, PointFs &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (uint i = 0; i < texts.count(); i++) {
                PointF point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (uint j = 0; j < pairs.count(); j++) {
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

    String PointFs::toString(const char split) const {
        String str;
        for (uint i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const PointF point = this->at(i);
            str.append(point.round().toString());
        }
        return str;
    }

    const Point Point::Empty;
    const Point Point::MinValue = Point(Int32::MinValue, Int32::MinValue);
    const Point Point::MaxValue = Point(Int32::MaxValue, Int32::MaxValue);

    Point::Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point::Point(const Point &point) {
        this->x = point.x;
        this->y = point.y;
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

    void Point::operator=(const Point &value) {
        this->x = value.x;
        this->y = value.y;
    }

    bool Point::operator==(const Point &value) const {
        return this->x == value.x && this->y == value.y;
    }

    bool Point::operator!=(const Point &value) const {
        return !operator==(value);
    }

    void Point::offset(const Point &pos) {
        offset(pos.x, pos.y);
    }

    void Point::offset(int dx, int dy) {
        x += dx;
        y += dy;
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

    Points::Points(uint capacity) : Vector<Point>(capacity) {
    }

    // point:{0, 0};point:{300, 300}
    // 0, 0;300, 300
    bool Points::parse(const String &str, Points &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (uint i = 0; i < texts.count(); i++) {
                Point point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (uint j = 0; j < pairs.count(); j++) {
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
}
