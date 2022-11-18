//
//  Math.h
//  common
//
//  Created by baowei on 2015/9/6.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Math_h
#define Math_h

#include <cstdlib>
#include <cmath>
#include <cstdint>
#include "system/OsDefine.h"
#if WIN32
#undef min
#undef max
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#endif // WIN32

namespace Common {
    // https://en.cppreference.com/w/c/numeric/math
    class Math {
    public:
        template<typename type>
        inline static type min(type a, type b) {
            return a < b ? a : b;
        }

        template<typename type>
        inline static type max(type a, type b) {
            return a < b ? b : a;
        }

        inline static float round(float value) {
            return ::roundf(value);
        }

        inline static double round(double value) {
            return ::round(value);
        }

        inline static long double round(long double value) {
            return ::roundl(value);
        }

        inline static float ceiling(float value) {
            return ::ceilf(value);
        }

        inline static double ceiling(double value) {
            return ::ceil(value);
        }

        inline static long double ceiling(long double value) {
            return ::ceill(value);
        }

        inline static float sin(float value) {
            return ::sinf(value);
        }

        inline static double sin(double value) {
            return ::sin(value);
        }

        inline static long double sin(long double value) {
            return ::sinl(value);
        }

        inline static float cos(float value) {
            return ::cosf(value);
        }

        inline static double cos(double value) {
            return ::cos(value);
        }

        inline static long double cos(long double value) {
            return ::cosl(value);
        }

        inline static float tan(float value) {
            return ::tanf(value);
        }

        inline static double tan(double value) {
            return ::tan(value);
        }

        inline static long double tan(long double value) {
            return ::tanl(value);
        }

        inline static long double asin(long double value) {
            return ::asinl(value);
        }

        inline static double asin(double value) {
            return ::asin(value);
        }

        inline static float acos(float value) {
            return ::acosf(value);
        }

        inline static double acos(double value) {
            return ::acos(value);
        }

        inline static long double acos(long double value) {
            return ::acosl(value);
        }

        inline static float atan(float value) {
            return ::atanf(value);
        }

        inline static double atan(double value) {
            return ::atan(value);
        }

        inline static long double atan(long double value) {
            return ::atanl(value);
        }

        inline static float atan2(float y, float x) {
            return ::atan2f(y, x);
        }

        inline static double atan2(double y, double x) {
            return ::atan2(y, x);
        }

        inline static long double atan2(long double y, long double x) {
            return ::atan2l(y, x);
        }

        inline static float pow(float a, float b) {
            return ::powf(a, b);
        }

        inline static double pow(double a, double b) {
            return ::pow(a, b);
        }

        inline static long double pow(long double a, long double b) {
            return ::powl(a, b);
        }

        inline static float sqrt(float value) {
            return ::sqrtf(value);
        }

        inline static double sqrt(double value) {
            return ::sqrt(value);
        }

        inline static long double sqrt(long double value) {
            return ::sqrtl(value);
        }

        inline static int abs(int value) {
            return value < 0 ? -value : value;
        }

        inline static float abs(float value) {
            return ::fabsf(value);
        }

        inline static double abs(double value) {
            return ::fabs(value);
        }

        inline static long double abs(long double value) {
            return ::fabsl(value);
        }

        inline static float exp(float value) {
            return ::expf(value);
        }

        inline static double exp(double value) {
            return ::exp(value);
        }

        inline static long double exp2(long double value) {
            return ::exp2l(value);
        }

        inline static float exp2(float value) {
            return ::exp2f(value);
        }

        inline static double exp2(double value) {
            return ::exp2(value);
        }

        inline static long double exp(long double value) {
            return ::expl(value);
        }

        inline static float log(float value) {
            return ::logf(value);
        }

        inline static double log(double value) {
            return ::log(value);
        }

        inline static long double log(long double value) {
            return ::logl(value);
        }

        inline static float log2(float value) {
            return ::log2f(value);
        }

        inline static double log2(double value) {
            return ::log2(value);
        }

        inline static long double log2(long double value) {
            return ::log2l(value);
        }

        inline static float log10(float value) {
            return ::log10f(value);
        }

        inline static double log10(double value) {
            return ::log10(value);
        }

        inline static long double log10(long double value) {
            return ::log10l(value);
        }

        inline static double toRadian(double angle) {
            return (double) (angle * RadianFactor);
        }

        inline static double toAngle(double radian) {
            return (double) (radian * AngleFactor);
        }

        template<typename type>
        inline static type getGreatestCommonDivisor(const type& a, const type& b) {
            type c;
            while (b != 0) {
                c = a % b;
                a = b;
                b = c;
            }
            return a;
        }

        template<typename type>
        inline static type getSmallestCommonMultiple(const type& a, const type& b) {
            type m, n, c;
            m = a;
            n = b;
            while (b != 0) {
                c = a % b;
                a = b;
                b = c;
            }
            return m * n / a;
        }

        static void calcCoordinate(float angle, float l, float &x, float &y);

        static bool calcPolarCoordinate(float x, float y, float &angle, float &l);

    public:
        static const double PI;
        static const double RadianFactor;
        static const double AngleFactor;
        static const double E;
    };
}

#endif // Math_h
