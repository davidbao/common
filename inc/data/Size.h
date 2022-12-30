//
//  Size.h
//  common
//
//  Created by baowei on 2016/9/6.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Size_h
#define Size_h

#include "data/Vector.h"
#include "data/String.h"

using namespace Data;

namespace Drawing {
    struct Size;

    struct Point;

    struct SizeF : public IEquatable<SizeF>, public IEvaluation<SizeF>, public IComparable<SizeF> {
    public:
        explicit SizeF(float width = 0.0f, float height = 0.0f);

        SizeF(int width, int height);

        SizeF(long width, long height);

        SizeF(const SizeF &size);

        ~SizeF() override;

        bool equals(const SizeF &other) const override;

        void evaluates(const SizeF &other) override;

        int compareTo(const SizeF &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        SizeF &operator=(const SizeF &value);

        SizeF operator+=(const SizeF &other);

        SizeF operator+(const SizeF &other) const;

        SizeF operator-=(const SizeF &other);

        SizeF operator-(const SizeF &other) const;

        SizeF operator*=(float value);

        SizeF operator*(float value) const;

        SizeF operator*=(int value);

        SizeF operator*(int value) const;

        SizeF operator/=(float value);

        SizeF operator/(float value) const;

        SizeF operator/=(int value);

        SizeF operator/(int value) const;

        void add(const SizeF &size);

        void subtract(const SizeF &size);

        void multiply(float value);

        void multiply(int value);

        void division(float value);

        void division(int value);

        Size ceiling() const;

        Size round() const;

        Size truncate() const;

    public:
        static bool parse(const String &str, SizeF &size);

        static SizeF add(const SizeF &sz1, const SizeF &sz2);

        static SizeF subtract(const SizeF &sz1, const SizeF &sz2);

        static SizeF multiply(const SizeF& size, float value);

        static SizeF multiply(const SizeF& size, int value);

        static SizeF division(const SizeF& size, float value);

        static SizeF division(const SizeF& size, int value);

    public:
        float width;
        float height;

        static const SizeF Empty;
        static const SizeF MinValue;
        static const SizeF MaxValue;
    };

    class SizeFs : public Vector<SizeF> {
    public:
        explicit SizeFs(size_t capacity = Vector<SizeF>::DefaultCapacity);

        SizeFs(const SizeFs &array);

        SizeFs(std::initializer_list<SizeF> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, SizeFs &points);
    };

    struct Size : public IEquatable<Size>, public IEvaluation<Size>, public IComparable<Size> {
    public:
        explicit Size(int width = 0, int height = 0);

        Size(const Size &size);

        ~Size() override;

        bool equals(const Size &other) const override;

        void evaluates(const Size &other) override;

        int compareTo(const Size &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        Size &operator=(const Size &value);

        Size operator+=(const Size &other);

        Size operator+(const Size &other) const;

        Size operator-=(const Size &other);

        Size operator-(const Size &other) const;

        Size operator*=(float value);

        Size operator*(float value) const;

        Size operator*=(int value);

        Size operator*(int value) const;

        Size operator/=(float value);

        Size operator/(float value) const;

        Size operator/=(int value);

        Size operator/(int value) const;

        operator SizeF() const;

        void add(const Size &size);

        void subtract(const Size &size);

        void multiply(float value);

        void multiply(int value);

        void division(float value);

        void division(int value);

    public:
        static bool parse(const String &str, Size &size);

        static Size add(const Size &sz1, const Size &sz2);

        static Size subtract(const Size &sz1, const Size &sz2);

        static Size multiply(const Size& size, float value);

        static Size multiply(const Size& size, int value);

        static Size division(const Size& size, float value);

        static Size division(const Size& size, int value);

        static Size ceiling(const SizeF &size);

        static Size round(const SizeF &size);

        static Size truncate(const SizeF &size);

    public:
        int width;
        int height;

        static const Size Empty;
        static const Size MinValue;
        static const Size MaxValue;
    };

    class Sizes : public Vector<Size> {
    public:
        explicit Sizes(size_t capacity = Vector<Size>::DefaultCapacity);

        Sizes(const Sizes &array);

        Sizes(std::initializer_list<Size> list);

        String toString(const char &split = ';') const;

        static bool parse(const String &str, Sizes &points);
    };
}

#endif  // Size_h
