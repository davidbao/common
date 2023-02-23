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

namespace System {
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

        static float round(float value);

        static double round(double value);

        static long double round(long double value);

        static float ceiling(float value);

        static double ceiling(double value);

        static long double ceiling(long double value);

        static float floor(float value);

        static double floor(double value);

        static long double floor(long double value);

        static float trunc(float value);

        static double trunc(double value);

        static long double trunc(long double value);

        static float sin(float value);

        static double sin(double value);

        static long double sin(long double value);

        static float cos(float value);

        static double cos(double value);

        static long double cos(long double value);

        static float tan(float value);

        static double tan(double value);

        static long double tan(long double value);

        static float asin(float value);

        static double asin(double value);

        static long double asin(long double value);

        static float acos(float value);

        static double acos(double value);

        static long double acos(long double value);

        static float atan(float value);

        static double atan(double value);

        static long double atan(long double value);

        static float atan2(float y, float x);

        static double atan2(double y, double x);

        static long double atan2(long double y, long double x);

        static float pow(float a, float b);

        static double pow(double a, double b);

        static long double pow(long double a, long double b);

        static float sqrt(float value);

        static double sqrt(double value);

        static long double sqrt(long double value);

        static float cbrt(float value);

        static double cbrt(double value);

        static long double cbrt(long double value);

        static float hypot(float x, float y);

        static double hypot(double x, double y);

        static long double hypot(long double x, long double y);

        static int abs(int value);

        static long abs(long value);

        static float abs(float value);

        static double abs(double value);

        static long double abs(long double value);

        static float exp(float value);

        static double exp(double value);

        static long double exp(long double value);

        static float exp2(float value);

        static double exp2(double value);

        static long double exp2(long double value);

        static float log(float value);

        static double log(double value);

        static long double log(long double value);

        static float log2(float value);

        static double log2(double value);

        static long double log2(long double value);

        static float log10(float value);

        static double log10(double value);

        static long double log10(long double value);

        static float mod(float x, float y);

        static double mod(double x, double y);

        static long double mod(long double x, long double y);

        static double toRadian(double angle);

        static double toAngle(double radian);

        // retrieved 'Greatest Common Divisor'.
        static bool getGCD(int a, int b, int &v);

        // retrieved 'Smallest Common Multiple'.
        static bool getSCM(int a, int b, int &v);

        static void calcCoord(float angle, float l, float &x, float &y);

        static void calcPolarCoord(float x, float y, float &angle, float &l);

    public:
        static const double PI;
        static const double RadianFactor;
        static const double AngleFactor;
        static const double E;
    };
}

#endif // Math_h
