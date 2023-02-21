//
//  Rectangle.cpp
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "data/Rectangle.h"
#include "system/Math.h"
#include "data/Convert.h"

using namespace System;

namespace Drawing {
    const RectangleF RectangleF::Empty;

    RectangleF::RectangleF(float x, float y, float width, float height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    RectangleF::RectangleF(const PointF &location, const SizeF &size) {
        this->x = location.x;
        this->y = location.y;
        this->width = size.width;
        this->height = size.height;
    }

    RectangleF::RectangleF(const RectangleF &rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->width = rect.width;
        this->height = rect.height;
    }

    RectangleF::~RectangleF() = default;

    bool RectangleF::equals(const RectangleF &other) const {
        return x == other.x && y == other.y &&
               width == other.width && height == other.height;
    }

    void RectangleF::evaluates(const RectangleF &other) {
        this->x = other.x;
        this->y = other.y;
        this->width = other.width;
        this->height = other.height;
    }

    int RectangleF::compareTo(const RectangleF &other) const {
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
        if (width != other.width) {
            if (width > other.width) {
                return 1;
            }
            return -1;
        }
        if (height != other.height) {
            if (height > other.height) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool RectangleF::isEmpty() const {
        return x == 0.0f && y == 0.0f && width == 0.0f && height == 0.0f;
    }

    String RectangleF::toString() const {
        return String::format("%g,%g,%g,%g", x, y, width, height);
    }

    PointF RectangleF::location() const {
        return PointF(x, y);
    }

    void RectangleF::setLocation(float x, float y) {
        this->x = x;
        this->y = y;
    }

    void RectangleF::setLocation(const PointF &location) {
        setLocation(location.x, location.y);
    }

    SizeF RectangleF::size() const {
        return SizeF(width, height);
    }

    void RectangleF::setSize(float width, float height) {
        this->width = width;
        this->height = height;
    }

    void RectangleF::setSize(const SizeF &size) {
        setSize(size.width, size.height);
    }

    float RectangleF::left() const {
        return x;
    }

    float RectangleF::top() const {
        return y;
    }

    float RectangleF::right() const {
        return x + width;
    }

    float RectangleF::bottom() const {
        return y + height;
    }

    PointF RectangleF::center() const {
        if (size().isEmpty())
            return location();

        return PointF(x + width / 2.0f, y + height / 2.0f);
    }

    PointF RectangleF::leftTop() const {
        return location();
    }

    PointF RectangleF::rightTop() const {
        return PointF(right(), top());
    }

    PointF RectangleF::leftBottom() const {
        return PointF(left(), bottom());
    }

    PointF RectangleF::rightBottom() const {
        return PointF(right(), bottom());
    }

    RectangleF &RectangleF::operator=(const RectangleF &value) {
        this->x = value.x;
        this->y = value.y;
        this->width = value.width;
        this->height = value.height;
        return *this;
    }

    bool RectangleF::contains(float x, float y) const {
        return this->x <= x &&
               x < this->right()&&
               this->y <= y &&
               y < this->bottom();
    }

    bool RectangleF::contains(const PointF &point) const {
        return contains(point.x, point.y);
    }

    bool RectangleF::contains(const RectangleF &rect) const {
        return (this->x <= rect.x) &&
               (rect.right() <= this->right()) &&
               (this->y <= rect.y) &&
               (rect.bottom() <= this->bottom());
    }

    void RectangleF::inflate(float dx, float dy) {
        *this = inflate(*this, dx, dy);
    }

    void RectangleF::inflate(const SizeF &size) {
        inflate(size.width, size.height);
    }

    RectangleF RectangleF::offset(const RectangleF &rect, float dx, float dy) {
        return RectangleF(rect.x + dx, rect.y + dy, rect.width, rect.height);
    }

    RectangleF RectangleF::intersect(const RectangleF &a, const RectangleF &b) {
        float x1 = Math::max(a.x, b.x);
        float x2 = Math::min(a.x + a.width, b.x + b.width);
        float y1 = Math::max(a.y, b.y);
        float y2 = Math::min(a.y + a.height, b.y + b.height);

        if (x2 >= x1 && y2 >= y1) {
            return RectangleF(x1, y1, x2 - x1, y2 - y1);
        }
        return RectangleF::Empty;
    }

    void RectangleF::intersect(const RectangleF &rect) {
        RectangleF result = RectangleF::intersect(rect, *this);

        this->x = result.x;
        this->y = result.y;
        this->width = result.width;
        this->height = result.height;
    }

    bool RectangleF::intersectsWith(const RectangleF &rect) const {
        return (rect.x < this->x + this->width) &&
               (this->x < (rect.x + rect.width)) &&
               (rect.y < this->y + this->height) &&
               (this->y < rect.y + rect.height);
    }

    RectangleF RectangleF::unions(const RectangleF &a, const RectangleF &b) {
        if (a.size().isEmpty())
            return b;
        if (b.size().isEmpty())
            return a;

        float x1 = Math::min(a.x, b.x);
        float x2 = Math::max(a.x + a.width, b.x + b.width);
        float y1 = Math::min(a.y, b.y);
        float y2 = Math::max(a.y + a.height, b.y + b.height);

        return RectangleF(x1, y1, x2 - x1, y2 - y1);
    }

    void RectangleF::offset(const PointF &pos) {
        offset(pos.x, pos.y);
    }

    void RectangleF::offset(float dx, float dy) {
        *this = offset(*this, dx, dy);
    }

    void RectangleF::unions(const RectangleF &rect) {
        RectangleF value = unions(*this, rect);
        this->operator=(value);
    }

    bool RectangleF::parse(const String &str, RectangleF &rect) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 4) {
            RectangleF temp;
            if (Float::parse(texts[0].trim(), temp.x) &&
                Float::parse(texts[1].trim(), temp.y) &&
                Float::parse(texts[2].trim(), temp.width) &&
                Float::parse(texts[3].trim(), temp.height)) {
                rect = temp;
                return true;
            }
        }
        return false;
    }

    Rectangle RectangleF::ceiling() const {
        return Rectangle::ceiling(*this);
    }

    Rectangle RectangleF::round() const {
        return Rectangle::round(*this);
    }

    Rectangle RectangleF::truncate() const {
        return Rectangle::truncate(*this);
    }

    void RectangleF::empty() {
        this->operator=(Empty);
    }

    RectangleF RectangleF::makeLTRB(float left, float top, float right, float bottom) {
        return RectangleF(left, top, right - left, bottom - top);
    }

    RectangleF RectangleF::makeLTRB(const PointF &leftTop, const PointF &rightBottom) {
        return makeLTRB(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
    }

    RectangleF RectangleF::inflate(const RectangleF &rect, float width, float height) {
        RectangleF result = rect;
        result.x -= width;
        result.y -= height;
        result.width += 2.0f * width;
        result.height += 2.0f * height;
        return result;
    }

    RectangleFs::RectangleFs(size_t capacity) : Vector<RectangleF>(capacity) {
    }

    RectangleFs::RectangleFs(const RectangleFs &array) = default;

    RectangleFs::RectangleFs(std::initializer_list<RectangleF> list) : Vector<RectangleF>(list) {
    }

    // rect:{0, 0, 100, 100};rect:{300, 300, 100, 100}
    // 0, 0, 100, 100;300, 300, 100, 100
    bool RectangleFs::parse(const String &str, RectangleFs &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                RectangleF point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "rect", true) &&
                            RectangleF::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (RectangleF::parse(texts[i], point)) {
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

    String RectangleFs::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const RectangleF &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }

    const Rectangle Rectangle::Empty;

    Rectangle::Rectangle(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    Rectangle::Rectangle(const Point &location, const Size &size) {
        this->x = location.x;
        this->y = location.y;
        this->width = size.width;
        this->height = size.height;
    }

    Rectangle::Rectangle(const Rectangle &rect) {
        this->x = rect.x;
        this->y = rect.y;
        this->width = rect.width;
        this->height = rect.height;
    }

    Rectangle::~Rectangle() = default;

    bool Rectangle::equals(const Rectangle &other) const {
        return x == other.x && y == other.y &&
               width == other.width && height == other.height;
    }

    void Rectangle::evaluates(const Rectangle &other) {
        this->x = other.x;
        this->y = other.y;
        this->width = other.width;
        this->height = other.height;
    }

    int Rectangle::compareTo(const Rectangle &other) const {
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
        if (width != other.width) {
            if (width > other.width) {
                return 1;
            }
            return -1;
        }
        if (height != other.height) {
            if (height > other.height) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool Rectangle::isEmpty() const {
        return x == 0 && y == 0 && width == 0 && height == 0;
    }

    String Rectangle::toString() const {
        return String::format("%d,%d,%d,%d", x, y, width, height);
    }

    Point Rectangle::location() const {
        return Point(x, y);
    }

    void Rectangle::setLocation(const Point &location) {
        setLocation(location.x, location.y);
    }

    void Rectangle::setLocation(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Size Rectangle::size() const {
        return Size(width, height);
    }

    void Rectangle::setSize(int width, int height) {
        this->width = width;
        this->height = height;
    }

    void Rectangle::setSize(const Size &size) {
        setSize(size.width, size.height);
    }

    int Rectangle::left() const {
        return x;
    }

    int Rectangle::top() const {
        return y;
    }

    int Rectangle::right() const {
        return x + width;
    }

    int Rectangle::bottom() const {
        return y + height;
    }

    Point Rectangle::center() const {
        if (size().isEmpty())
            return location();

        return Point(x + width / 2, y + height / 2);
    }

    Point Rectangle::leftTop() const {
        return location();
    }

    Point Rectangle::rightTop() const {
        return PointF(right(), top());
    }

    Point Rectangle::leftBottom() const {
        return PointF(left(), bottom());
    }

    Point Rectangle::rightBottom() const {
        return PointF(right(), bottom());
    }

    Rectangle &Rectangle::operator=(const Rectangle &value) {
        evaluates(value);
        return *this;
    }

    bool Rectangle::contains(int x, int y) const {
        return this->x <= x &&
               x < this->right()&&
               this->y <= y &&
               y < this->bottom();
    }

    bool Rectangle::contains(const Point &point) const {
        return contains(point.x, point.y);
    }

    bool Rectangle::contains(const Rectangle &rect) const {
        return (this->x <= rect.x) &&
               (rect.right() <= this->right()) &&
               (this->y <= rect.y) &&
               (rect.bottom() <= this->bottom());
    }

    void Rectangle::inflate(int width, int height) {
        *this = inflate(*this, width, height);
    }

    void Rectangle::inflate(const Size &size) {
        inflate(size.width, size.height);
    }

    Rectangle Rectangle::offset(const Rectangle &rect, int dx, int dy) {
        return Rectangle(rect.x + dx, rect.y + dy, rect.width, rect.height);
    }

    Rectangle Rectangle::intersect(const Rectangle &a, const Rectangle &b) {
        int x1 = Math::max(a.x, b.x);
        int x2 = Math::min(a.x + a.width, b.x + b.width);
        int y1 = Math::max(a.y, b.y);
        int y2 = Math::min(a.y + a.height, b.y + b.height);

        if (x2 >= x1 && y2 >= y1) {
            return Rectangle(x1, y1, x2 - x1, y2 - y1);
        }
        return Rectangle::Empty;
    }

    void Rectangle::intersect(const Rectangle &rect) {
        *this = Rectangle::intersect(rect, *this);
    }

    bool Rectangle::intersectsWith(const Rectangle &rect) const {
        return rect.x < this->right() &&
               this->x < rect.right() &&
               rect.y < this->bottom() &&
               this->y < rect.bottom();
    }

    Rectangle Rectangle::unions(const Rectangle &a, const Rectangle &b) {
        if (a.size().isEmpty())
            return b;
        if (b.size().isEmpty())
            return a;

        int x1 = Math::min(a.x, b.x);
        int x2 = Math::max(a.x + a.width, b.x + b.width);
        int y1 = Math::min(a.y, b.y);
        int y2 = Math::max(a.y + a.height, b.y + b.height);

        return Rectangle(x1, y1, x2 - x1, y2 - y1);
    }

    void Rectangle::offset(const Point &pos) {
        offset(pos.x, pos.y);
    }

    void Rectangle::offset(int dx, int dy) {
        *this = offset(*this, dx, dy);
    }

    void Rectangle::unions(const Rectangle &rect) {
        Rectangle value = unions(*this, rect);
        this->operator=(value);
    }

    bool Rectangle::parse(const String &str, Rectangle &rect) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() == 4) {
            Rectangle temp;
            if (Int32::parse(texts[0].trim(), temp.x) &&
                Int32::parse(texts[1].trim(), temp.y) &&
                Int32::parse(texts[2].trim(), temp.width) &&
                Int32::parse(texts[3].trim(), temp.height)) {
                rect = temp;
                return true;
            }
        }
        return false;
    }

    void Rectangle::empty() {
        this->operator=(Empty);
    }

    Rectangle Rectangle::makeLTRB(int left, int top, int right, int bottom) {
        return Rectangle(left, top, right - left, bottom - top);
    }

    Rectangle Rectangle::makeLTRB(const Point &leftTop, const Point &rightBottom) {
        return makeLTRB(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y);
    }

    Rectangle Rectangle::inflate(const Rectangle &rect, int width, int height) {
        Rectangle result = rect;
        result.x -= width;
        result.y -= height;
        result.width += 2 * width;
        result.height += 2 * height;
        return result;
    }

    Rectangle Rectangle::ceiling(const RectangleF &rect) {
        return Rectangle((int) Math::ceiling(rect.x),
                         (int) Math::ceiling(rect.y),
                         (int) Math::ceiling(rect.width),
                         (int) Math::ceiling(rect.height));
    }

    Rectangle Rectangle::round(const RectangleF &rect) {
        return Rectangle((int) Math::round(rect.x),
                         (int) Math::round(rect.y),
                         (int) Math::round(rect.width),
                         (int) Math::round(rect.height));
    }

    Rectangle Rectangle::truncate(const RectangleF &rect) {
        return Rectangle((int) rect.x,
                         (int) rect.y,
                         (int) rect.width,
                         (int) rect.height);
    }

    Rectangles::Rectangles(size_t capacity) : Vector<Rectangle>(capacity) {
    }

    Rectangles::Rectangles(const Rectangles &array) = default;

    Rectangles::Rectangles(std::initializer_list<Rectangle> list) : Vector<Rectangle>(list) {
    }

    // rect:{0, 0, 100, 100};rect:{300, 300, 100, 100}
    // 0, 0, 100, 100;300, 300, 100, 100
    bool Rectangles::parse(const String &str, Rectangles &points) {
        StringArray texts;
        Convert::splitStr(str, ';', texts);
        if (texts.count() > 0) {
            for (size_t i = 0; i < texts.count(); i++) {
                Rectangle point;

                Convert::KeyPairs pairs;
                if (Convert::splitItems(texts[i], pairs)) {
                    for (size_t j = 0; j < pairs.count(); j++) {
                        const Convert::KeyPair *kp = pairs[j];
                        if (String::equals(kp->name, "rect", true) &&
                            Rectangle::parse(kp->value, point)) {
                            points.add(point);
                        } else {
                            return false;
                        }
                    }
                } else {
                    if (Rectangle::parse(texts[i], point)) {
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

    String Rectangles::toString(const char &split) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0) {
                str.append(split);
            }
            const Rectangle &point = this->at(i);
            str.append(point.toString());
        }
        return str;
    }
}
