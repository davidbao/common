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

using namespace Common;

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

    public:
        static bool parse(const String &str, SizeF &size);

    public:
        float width;
        float height;

        static const SizeF Empty;
        static const SizeF MinValue;
        static const SizeF MaxValue;
    };

    typedef Vector<SizeF> SizeFs;

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

    typedef Vector<Size> Sizes;
}

#endif  // Size_h
