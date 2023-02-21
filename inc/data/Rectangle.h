//
//  Rectangle.h
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Rectangle_h
#define Rectangle_h

#include "data/String.h"
#include "data/Point.h"
#include "data/Size.h"

using namespace Data;

namespace Drawing {
    struct Rectangle;

    struct RectangleF : public IEquatable<RectangleF>, public IEvaluation<RectangleF>, public IComparable<RectangleF> {
    public:
        explicit RectangleF(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f);

        RectangleF(const PointF &location, const SizeF &size);

        RectangleF(const RectangleF &size);

        ~RectangleF() override;

        bool equals(const RectangleF &other) const override;

        void evaluates(const RectangleF &other) override;

        int compareTo(const RectangleF &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        PointF location() const;

        void setLocation(float x, float y);

        void setLocation(const PointF &location);

        SizeF size() const;

        void setSize(float width, float height);

        void setSize(const SizeF &size);

        float left() const;

        float top() const;

        float right() const;

        float bottom() const;

        PointF center() const;

        PointF leftTop() const;

        PointF rightTop() const;

        PointF leftBottom() const;

        PointF rightBottom() const;

        RectangleF &operator=(const RectangleF &value);

        bool contains(float x, float y) const;

        bool contains(const PointF &point) const;

        bool contains(const RectangleF &rect) const;

        void inflate(float width, float height);

        void inflate(const SizeF &size);

        void intersect(const RectangleF &rect);

        bool intersectsWith(const RectangleF &rect) const;

        void offset(const PointF &pos);

        void offset(float dx, float dy);

        void unions(const RectangleF &rect);

        Rectangle ceiling() const;

        Rectangle round() const;

        Rectangle truncate() const;

    public:
        static RectangleF offset(const RectangleF &rect, float dx, float dy);

        static RectangleF intersect(const RectangleF &a, const RectangleF &b);

        // Creates a rectangle that represents the union between a and b.
        static RectangleF unions(const RectangleF &a, const RectangleF &b);

        static bool parse(const String &str, RectangleF &rect);

        static RectangleF makeLTRB(float left, float top, float right, float bottom);

        static RectangleF makeLTRB(const PointF &leftTop, const PointF &rightBottom);

        static RectangleF inflate(const RectangleF &rect, float width, float height);

    public:
        float x;
        float y;
        float width;
        float height;

        static const RectangleF Empty;
    };

    class RectangleFs : public Vector<RectangleF> {
    public:
        explicit RectangleFs(size_t capacity = Vector<RectangleF>::DefaultCapacity);

        RectangleFs(const RectangleFs &array);

        RectangleFs(std::initializer_list<RectangleF> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, RectangleFs &points);
    };

    struct Rectangle : public IEquatable<Rectangle>, public IEvaluation<Rectangle>, public IComparable<Rectangle> {
    public:
        explicit Rectangle(int x = 0, int y = 0, int width = 0, int height = 0);

        Rectangle(const Point &location, const Size &size);

        Rectangle(const Rectangle &rect);

        ~Rectangle() override;

        bool equals(const Rectangle &other) const override;

        void evaluates(const Rectangle &other) override;

        int compareTo(const Rectangle &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        Point location() const;

        void setLocation(int x, int y);

        void setLocation(const Point &location);

        Size size() const;

        void setSize(int width, int height);

        void setSize(const Size &size);

        int left() const;

        int top() const;

        int right() const;

        int bottom() const;

        Point center() const;

        Point leftTop() const;

        Point rightTop() const;

        Point leftBottom() const;

        Point rightBottom() const;

        Rectangle &operator=(const Rectangle &value);

        bool contains(int x, int y) const;

        bool contains(const Point &point) const;

        bool contains(const Rectangle &rect) const;

        void inflate(int width, int height);

        void inflate(const Size &size);

        void intersect(const Rectangle &rect);

        bool intersectsWith(const Rectangle &rect) const;

        void offset(const Point &pos);

        void offset(int dx, int dy);

        void unions(const Rectangle &rect);

    public:
        static Rectangle offset(const Rectangle &rect, int dx, int dy);

        static Rectangle intersect(const Rectangle &a, const Rectangle &b);

        // Creates a rectangle that represents the union between a and b.
        static Rectangle unions(const Rectangle &a, const Rectangle &b);

        static bool parse(const String &str, Rectangle &rect);

        static Rectangle makeLTRB(int left, int top, int right, int bottom);

        static Rectangle makeLTRB(const Point &leftTop, const Point &rightBottom);

        static Rectangle inflate(const Rectangle &rect, int width, int height);

        static Rectangle ceiling(const RectangleF &rect);

        static Rectangle round(const RectangleF &rect);

        static Rectangle truncate(const RectangleF &rect);

    public:
        int x;
        int y;
        int width;
        int height;

    public:
        static const Rectangle Empty;
    };

    class Rectangles : public Vector<Rectangle> {
    public:
        explicit Rectangles(size_t capacity = Vector<Rectangle>::DefaultCapacity);

        Rectangles(const Rectangles &array);

        Rectangles(std::initializer_list<Rectangle> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, Rectangles &points);
    };
}

#endif  // Rectangle_h
