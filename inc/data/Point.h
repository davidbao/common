//
//  Point.h
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Point_h
#define Point_h

#include "data/Vector.h"
#include "data/ValueType.h"

namespace Common {
    struct Point;

    struct PointF {
    public:
        PointF(float x = 0.0f, float y = 0.0f);

        PointF(int x, int y);

        PointF(long x, long y);

        PointF(const PointF &point);

        bool isEmpty() const;

        void empty();

        String toString() const;

        void operator=(const PointF &value);

        bool operator==(const PointF &value) const;

        bool operator!=(const PointF &value) const;

        void offset(const PointF &pos);

        void offset(float dx, float dy);

        Point round() const;

    public:
        static bool parse(const String &str, PointF &point);

    public:
        float x;
        float y;

        static const PointF Empty;
        static const PointF MinValue;
        static const PointF MaxValue;
    };

    class PointFs : public Vector<PointF> {
    public:
        PointFs(uint capacity = Vector<PointF>::DefaultCapacity);

        PointFs(const PointFs &array);

        String toString(const char split = ';') const;

        static bool parse(const String &str, PointFs &points);
    };

    struct Point {
    public:
        Point(int x = 0, int y = 0);

        Point(const Point &point);

        bool isEmpty() const;

        void empty();

        String toString() const;

        void operator=(const Point &value);

        bool operator==(const Point &value) const;

        bool operator!=(const Point &value) const;

        void offset(const Point &pos);

        void offset(int dx, int dy);

    public:
        static bool parse(const String &str, Point &point);

    public:
        int x;
        int y;

        static const Point Empty;
        static const Point MinValue;
        static const Point MaxValue;
    };

    class Points : public Vector<Point> {
    public:
        Points(uint capacity = Vector<Point>::DefaultCapacity);

        static bool parse(const String &str, Points &points);
    };
}

#endif  // Point_h
