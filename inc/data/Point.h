//
//  Point.h
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Point_h
#define Point_h

#include "data/Vector.h"
#include "data/String.h"
#include "data/DataInterface.h"

using namespace Data;

namespace Drawing {
    struct Point;
    struct Size;
    struct SizeF;

    struct PointF : public IEquatable<PointF>, public IEvaluation<PointF>, public IComparable<PointF> {
    public:
        explicit PointF(float x = 0.0f, float y = 0.0f);

        PointF(int x, int y);

        PointF(long x, long y);

        PointF(const PointF &point);

        ~PointF() override;

        bool equals(const PointF &other) const override;

        void evaluates(const PointF &other) override;

        int compareTo(const PointF &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        PointF &operator=(const PointF &other);

        PointF operator+=(const Size &other);

        PointF operator+(const Size &other) const;

        PointF operator-=(const Size &other);

        PointF operator-(const Size &other) const;

        PointF operator+=(const SizeF &other);

        PointF operator+(const SizeF &other) const;

        PointF operator-=(const SizeF &other);

        PointF operator-(const SizeF &other) const;

        operator Point() const;

        operator SizeF() const;

        void offset(const PointF &pos);

        void offset(float dx, float dy);

        void add(const Size &size);

        void subtract(const Size &size);

        void add(const SizeF &size);

        void subtract(const SizeF &size);

        void add(const PointF &point);

        void subtract(const PointF &point);

        void add(const Point &point);

        void subtract(const Point &point);

        Point ceiling() const;

        Point round() const;

        Point truncate() const;

    public:
        static bool parse(const String &str, PointF &point);

        static PointF offset(const PointF &point, const PointF &pos);

        static PointF offset(const PointF &point, float dx, float dy);

        static PointF add(const PointF &point, const Size &size);

        static PointF subtract(const PointF &point, const Size &size);

        static PointF add(const PointF &pt1, const PointF &pt2);

        static PointF subtract(const PointF &pt1, const PointF &pt2);

        static PointF add(const PointF &point, const SizeF &size);

        static PointF subtract(const PointF &point, const SizeF &size);

    public:
        float x;
        float y;

        static const PointF Empty;
        static const PointF MinValue;
        static const PointF MaxValue;
    };

    class PointFs : public Vector<PointF> {
    public:
        explicit PointFs(size_t capacity = Vector<PointF>::DefaultCapacity);

        PointFs(const PointFs &array);

        PointFs(std::initializer_list<PointF> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, PointFs &points);
    };

    struct Point : public IEquatable<Point>, public IEvaluation<Point>, public IComparable<Point> {
    public:
        explicit Point(int x = 0, int y = 0);

        Point(const Point &point);

        Point(const Size &size);

        ~Point() override;

        bool equals(const Point &other) const override;

        void evaluates(const Point &other) override;

        int compareTo(const Point &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        Point &operator=(const Point &other);

        Point operator+=(const Size &other);

        Point operator+(const Size &other) const;

        Point operator-=(const Size &other);

        Point operator-(const Size &other) const;

        Point operator+=(const Point &other);

        Point operator+(const Point &other) const;

        Point operator-=(const Point &other);

        Point operator-(const Point &other) const;

        operator PointF() const;

        operator Size() const;

        void offset(const Point &pos);

        void offset(int dx, int dy);

        void add(const Size &size);

        void subtract(const Size &size);

    public:
        static bool parse(const String &str, Point &point);

        static Point offset(const Point &point, const Point &pos);

        static Point offset(const Point &point, int dx, int dy);

        static Point add(const Point &point, const Size &size);

        static Point subtract(const Point &point, const Size &size);

        static Point add(const Point &pt1, const Point &pt2);

        static Point subtract(const Point &pt1, const Point &pt2);

        static Point ceiling(const PointF &point);

        static Point round(const PointF &point);

        static Point truncate(const PointF &point);

    public:
        int x;
        int y;

        static const Point Empty;
        static const Point MinValue;
        static const Point MaxValue;
    };

    class Points : public Vector<Point> {
    public:
        explicit Points(size_t capacity = Vector<Point>::DefaultCapacity);

        Points(const Points &array);

        Points(std::initializer_list<Point> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, Points &points);
    };
}

#endif  // Point_h
