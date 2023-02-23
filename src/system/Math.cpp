//
//  Math.cpp
//  common
//
//  Created by baowei on 2015/9/6.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Math.h"

namespace System {
    const double Math::PI = 3.14159265358979323846;
    const double Math::RadianFactor = PI / 180.0;
    const double Math::AngleFactor = 180.0 / PI;
    const double Math::E = 2.7182818284590452354;

    float Math::round(float value) {
        return ::roundf(value);
    }

    double Math::round(double value) {
        return ::round(value);
    }

    long double Math::round(long double value) {
        return ::roundl(value);
    }

    float Math::ceiling(float value) {
        return ::ceilf(value);
    }

    double Math::ceiling(double value) {
        return ::ceil(value);
    }

    long double Math::ceiling(long double value) {
        return ::ceill(value);
    }

    float Math::floor(float value) {
        return ::floorf(value);
    }

    double Math::floor(double value) {
        return ::floor(value);
    }

    long double Math::floor(long double value) {
        return ::floorl(value);
    }

    float Math::trunc(float value) {
        return ::truncf(value);
    }

    double Math::trunc(double value) {
        return ::trunc(value);
    }

    long double Math::trunc(long double value) {
        return ::truncl(value);
    }

    float Math::sin(float value) {
        return ::sinf(value);
    }

    double Math::sin(double value) {
        return ::sin(value);
    }

    long double Math::sin(long double value) {
        return ::sinl(value);
    }

    float Math::cos(float value) {
        return ::cosf(value);
    }

    double Math::cos(double value) {
        return ::cos(value);
    }

    long double Math::cos(long double value) {
        return ::cosl(value);
    }

    float Math::tan(float value) {
        return ::tanf(value);
    }

    double Math::tan(double value) {
        return ::tan(value);
    }

    long double Math::tan(long double value) {
        return ::tanl(value);
    }

    float Math::asin(float value) {
        return ::asinf(value);
    }

    double Math::asin(double value) {
        return ::asin(value);
    }

    long double Math::asin(long double value) {
        return ::asinl(value);
    }

    float Math::acos(float value) {
        return ::acosf(value);
    }

    double Math::acos(double value) {
        return ::acos(value);
    }

    long double Math::acos(long double value) {
        return ::acosl(value);
    }

    float Math::atan(float value) {
        return ::atanf(value);
    }

    double Math::atan(double value) {
        return ::atan(value);
    }

    long double Math::atan(long double value) {
        return ::atanl(value);
    }

    float Math::atan2(float y, float x) {
        return ::atan2f(y, x);
    }

    double Math::atan2(double y, double x) {
        return ::atan2(y, x);
    }

    long double Math::atan2(long double y, long double x) {
        return ::atan2l(y, x);
    }

    float Math::pow(float a, float b) {
        return ::powf(a, b);
    }

    double Math::pow(double a, double b) {
        return ::pow(a, b);
    }

    long double Math::pow(long double a, long double b) {
        return ::powl(a, b);
    }

    float Math::sqrt(float value) {
        return ::sqrtf(value);
    }

    double Math::sqrt(double value) {
        return ::sqrt(value);
    }

    long double Math::sqrt(long double value) {
        return ::sqrtl(value);
    }

    float Math::hypot(float x, float y) {
        return ::hypotf(x, y);
    }

    double Math::hypot(double x, double y) {
        return ::hypot(x, y);
    }

    long double Math::hypot(long double x, long double y) {
        return ::hypotl(x, y);
    }

    float Math::cbrt(float value) {
        return ::cbrtf(value);
    }

    double Math::cbrt(double value) {
        return ::cbrt(value);
    }

    long double Math::cbrt(long double value) {
        return ::cbrtl(value);
    }

    int Math::abs(int value) {
        return ::abs(value);
    }

    long Math::abs(long value) {
        return ::labs(value);
    }

    float Math::abs(float value) {
        return ::fabsf(value);
    }

    double Math::abs(double value) {
        return ::fabs(value);
    }

    long double Math::abs(long double value) {
        return ::fabsl(value);
    }

    float Math::exp(float value) {
        return ::expf(value);
    }

    double Math::exp(double value) {
        return ::exp(value);
    }

    long double Math::exp2(long double value) {
        return ::exp2l(value);
    }

    float Math::exp2(float value) {
        return ::exp2f(value);
    }

    double Math::exp2(double value) {
        return ::exp2(value);
    }

    long double Math::exp(long double value) {
        return ::expl(value);
    }

    float Math::log(float value) {
        return ::logf(value);
    }

    double Math::log(double value) {
        return ::log(value);
    }

    long double Math::log(long double value) {
        return ::logl(value);
    }

    float Math::log2(float value) {
        return ::log2f(value);
    }

    double Math::log2(double value) {
        return ::log2(value);
    }

    long double Math::log2(long double value) {
        return ::log2l(value);
    }

    float Math::log10(float value) {
        return ::log10f(value);
    }

    double Math::log10(double value) {
        return ::log10(value);
    }

    long double Math::mod(long double x, long double y) {
        return ::fmodl(x, y);
    }

    float Math::mod(float x, float y) {
        return ::fmodf(x, y);
    }

    double Math::mod(double x, double y) {
        return ::fmod(x, y);
    }

    long double Math::log10(long double value) {
        return ::log10l(value);
    }

    double Math::toRadian(double angle) {
        return angle * RadianFactor;
    }

    double Math::toAngle(double radian) {
        return radian * AngleFactor;
    }

    bool Math::getGCD(int a, int b, int &v) {
        if (a <= 0 || b <= 0) {
            return false;
        }

        while (a % b) {
            v = a % b;
            a = b;
            b = v;
        }
        v = b;
        return true;
    }

    bool Math::getSCM(int a, int b, int &v) {
        if (a <= 0 || b <= 0) {
            return false;
        }

        int m, n, c;
        m = a;
        n = b;
        while (b != 0) {
            c = a % b;
            a = b;
            b = c;
        }
        v = m * n / a;
        return true;
    }

    void Math::calcCoord(float angle, float l, float &x, float &y) {
        if (l == 0.0f) {
            x = y = 0;
        } else {
            angle = mod(angle, 360.0f);
            if (angle == 90.0f) {
                x = 0;
                y = l;
            } else if (angle == 180.0f) {
                x = -l;
                y = 0;
            } else if (angle == 270.0f) {
                x = 0;
                y = -l;
            } else {
                auto radian = (float) toRadian(angle);
                x = l * cos(radian);
                y = l * sin(radian);
            }
        }
    }

    void Math::calcPolarCoord(float x, float y, float &angle, float &l) {
        if (x == 0.0f && y == 0.0f) {
            angle = l = 0;
        } else {
            float r;
            auto length = hypot(x, y);
            if (y == 0) {
                angle = x > 0 ? 0.0f : 180.0f;  // 0 or 180
                l = abs(x);
            } else if (y > 0) {
                r = asin(y / length);           // from 0 to 90
                if (x < 0) {
                    r = (float) PI - r;         // from 90 to 180
                    angle = (float) toAngle(r);
                    l = length;
                } else if (x == 0) {
                    angle = 90.0f;              // 90
                    l = y;
                } else {
                    angle = (float) toAngle(r);
                    l = length;
                }
            } else {
                // y < 0
                y = -y;
                r = asin(y / length);           // from -90 to 0
                if (x < 0) {
                    r = (float) PI - r;         // from 180 to 270
                    angle = 360.0f - (float) toAngle(r);
                    l = length;
                } else if (x == 0) {
                    angle = 270.0f;             // 270
                    l = y;
                } else {                        // from 270 to 360
                    angle = 360.0f - (float) toAngle(r);
                    l = length;
                }
            }
        }
    }
}
