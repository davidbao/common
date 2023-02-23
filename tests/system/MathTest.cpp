//
//  MathTest.cpp
//  common
//
//  Created by baowei on 2023/2/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/Math.h"
#include <cfloat>

using namespace System;

static const double EpsilonValue = 0.00001;

bool testMinMax() {
    {
        if (Math::min(1, 2) != 1) {
            return false;
        }
        if (Math::min(-1, -2) != -2) {
            return false;
        }
        if (Math::min(1, 1) != 1) {
            return false;
        }
    }
    {
        if (Math::min(1.0f, 2.0f) != 1.0f) {
            return false;
        }
        if (Math::min(-1.0f, -2.0f) != -2.0f) {
            return false;
        }
        if (Math::min(1.0f, 1.0f) != 1.0f) {
            return false;
        }
    }
    {
        if (Math::min(1.0, 2.0) != 1.0) {
            return false;
        }
        if (Math::min(-1.0, -2.0) != -2.0) {
            return false;
        }
        if (Math::min(1.0, 1.0) != 1.0) {
            return false;
        }
    }

    {
        if (Math::max(1, 2) != 2) {
            return false;
        }
        if (Math::max(-1, -2) != -1) {
            return false;
        }
        if (Math::max(1, 1) != 1) {
            return false;
        }
    }
    {
        if (Math::max(1.0f, 2.0f) != 2.0f) {
            return false;
        }
        if (Math::max(-1.0f, -2.0f) != -1.0f) {
            return false;
        }
        if (Math::max(1.0f, 1.0f) != 1.0f) {
            return false;
        }
    }
    {
        if (Math::max(1.0f, 2.0f) != 2.0f) {
            return false;
        }
        if (Math::max(-1.0f, -2.0f) != -1.0f) {
            return false;
        }
        if (Math::max(1.0f, 1.0f) != 1.0f) {
            return false;
        }
    }

    return true;
}

bool testRound() {
    {
        if (Math::round(2.3) != 2) {
            return false;
        }
        if (Math::round(2.5) != 3) {
            return false;
        }
        if (Math::round(2.7) != 3) {
            return false;
        }
        if (Math::round(0.0) != 0) {
            return false;
        }
        if (Math::round(-INFINITY) != -INFINITY) {
            return false;
        }
    }
    {
        if (Math::round(2.3f) != 2) {
            return false;
        }
        if (Math::round(2.5f) != 3) {
            return false;
        }
        if (Math::round(2.7f) != 3) {
            return false;
        }
        if (Math::round(0.0f) != 0) {
            return false;
        }
        if (Math::round(-INFINITY) != -INFINITY) {
            return false;
        }
    }

    {
        if (Math::ceiling(2.4) != 3) {
            return false;
        }
        if (Math::ceiling(-2.4) != -2) {
            return false;
        }
        if (Math::ceiling(0.0) != 0) {
            return false;
        }
        if (Math::ceiling(-INFINITY) != -INFINITY) {
            return false;
        }
    }
    {
        if (Math::ceiling(2.4f) != 3) {
            return false;
        }
        if (Math::ceiling(-2.4f) != -2) {
            return false;
        }
        if (Math::ceiling(0.0f) != 0) {
            return false;
        }
        if (Math::ceiling(-INFINITY) != -INFINITY) {
            return false;
        }
    }

    {
        if (Math::floor(2.7) != 2) {
            return false;
        }
        if (Math::floor(-2.7) != -3) {
            return false;
        }
        if (Math::floor(0.0) != 0) {
            return false;
        }
        if (Math::floor(-INFINITY) != -INFINITY) {
            return false;
        }
    }
    {
        if (Math::floor(2.7f) != 2) {
            return false;
        }
        if (Math::floor(-2.7f) != -3) {
            return false;
        }
        if (Math::floor(0.0f) != 0) {
            return false;
        }
        if (Math::floor(-INFINITY) != -INFINITY) {
            return false;
        }
    }

    {
        if (Math::trunc(2.7) != 2) {
            return false;
        }
        if (Math::trunc(-2.7) != -2) {
            return false;
        }
        if (Math::trunc(0.0) != 0) {
            return false;
        }
        if (Math::trunc(-INFINITY) != -INFINITY) {
            return false;
        }
    }
    {
        if (Math::trunc(2.7f) != 2) {
            return false;
        }
        if (Math::trunc(-2.7f) != -2) {
            return false;
        }
        if (Math::trunc(0.0f) != 0) {
            return false;
        }
        if (Math::trunc(-INFINITY) != -INFINITY) {
            return false;
        }
    }
    {
        if (Math::trunc(2.7L) != 2) {
            return false;
        }
        if (Math::trunc(-2.7L) != -2) {
            return false;
        }
        if (Math::trunc(0.0L) != 0) {
            return false;
        }
        if (Math::trunc(-INFINITY) != -INFINITY) {
            return false;
        }
    }

    return true;
}

bool testSinCos() {
    {
        double pi = Math::acos(-1.0);
        // typical usage
        if (Math::sin(pi / 6) - 0.5 > EpsilonValue) {
            return false;
        }
        if (Math::sin(pi / 2) - 1.0 > EpsilonValue) {
            return false;
        }
        if (Math::sin(-3.0 * pi / 4) - (-0.707106769) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::sin(0.0) != 0.0) {
            return false;
        }
        if (Math::sin(-0.0) != 0.0) {
            return false;
        }
        if (!isnan(Math::sin(INFINITY))) {
            return false;
        }
    }

    {
        if (Math::asin(1.0) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::asin(-0.5) - (-0.523599) > EpsilonValue) {
            return false;
        }
        if (Math::asin(0.0) != 0.0) {
            return false;
        }
        if (Math::asin(-0.0) != 0.0) {
            return false;
        }
        if (!isnan(Math::asin(1.1))) {
            return false;
        }
    }
    {
        if (Math::asin(1.0L) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::asin(-0.5L) - (-0.523599) > EpsilonValue) {
            return false;
        }
        if (Math::asin(0.0L) != 0.0) {
            return false;
        }
        if (Math::asin(-0.0L) != 0.0) {
            return false;
        }
        if (!isnan(Math::asin(1.1L))) {
            return false;
        }
    }

    {
        double pi = Math::acos(-1.0);
        // typical usage
        if (Math::cos(pi / 3) - 0.5 > EpsilonValue) {
            return false;
        }
        if (Math::cos(pi / 2) - 0.0 > EpsilonValue) {
            return false;
        }
        if (Math::cos(-3 * pi / 4) - (-0.707106769) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::cos(0.0) != 1.0) {
            return false;
        }
        if (Math::cos(-0.0) != 1.0) {
            return false;
        }
        if (!isnan(Math::cos(INFINITY))) {
            return false;
        }
    }
    {
        long double pi = Math::acos(-1.0L);
        // typical usage
        if (Math::cos(pi / 3) - 0.5 > EpsilonValue) {
            return false;
        }
        if (Math::cos(pi / 2) - 0.0 > EpsilonValue) {
            return false;
        }
        if (Math::cos(-3 * pi / 4) - (-0.707106769) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::cos(0.0) != 1.0) {
            return false;
        }
        if (Math::cos(-0.0) != 1.0) {
            return false;
        }
        if (!isnan(Math::cos(INFINITY))) {
            return false;
        }
    }

    {
        if (Math::acos(-1.0) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.0) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.5) - 1.047198 > EpsilonValue) {
            return false;
        }
        if (Math::acos(1.0) - 0.0 > EpsilonValue) {
            return false;
        }
        if (!isnan(Math::acos(1.1))) {
            return false;
        }
    }
    {
        if (Math::acos(-1.0f) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.0f) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.5f) - 1.047198 > EpsilonValue) {
            return false;
        }
        if (Math::acos(1.0f) - 0.0 > EpsilonValue) {
            return false;
        }
        if (!isnan(Math::acos(1.1f))) {
            return false;
        }
    }
    {
        if (Math::acos(-1.0L) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.0L) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::acos(0.5L) - 1.047198 > EpsilonValue) {
            return false;
        }
        if (Math::acos(1.0L) - 0.0 > EpsilonValue) {
            return false;
        }
        if (!isnan(Math::acos(1.1L))) {
            return false;
        }
    }

    return true;
}

bool testTan() {
    {
        double pi = acos(-1);
        // typical usage
        if (Math::tan(pi / 4) - 1.0 > EpsilonValue) { //   45 deg
            return false;
        }
        if (Math::tan(3 * pi / 4) - (-1.0) > EpsilonValue) { //  135 deg
            return false;
        }
        if (Math::tan(5 * pi / 4) - 1.0 > EpsilonValue) { // -135 deg
            return false;
        }
        if (Math::tan(7 * pi / 4) - (-1.0) > EpsilonValue) { //  -45 deg
            return false;
        }
        // special values
        if (Math::tan(0.0) != 0.0) {
            return false;
        }
        if (Math::tan(-0.0) != 0.0) {
            return false;
        }
        // error handling
        if (!isnan(Math::tan(INFINITY))) {
            return false;
        }
    }
    {
        long double pi = acosl(-1);
        // typical usage
        if (Math::tan(pi / 4) - 1.0 > EpsilonValue) { //   45 deg
            return false;
        }
        if (Math::tan(3 * pi / 4) - (-1.0) > EpsilonValue) { //  135 deg
            return false;
        }
        if (Math::tan(5 * pi / 4) - 1.0 > EpsilonValue) { // -135 deg
            return false;
        }
        if (Math::tan(7 * pi / 4) - (-1.0) > EpsilonValue) { //  -45 deg
            return false;
        }
        // special values
        if (Math::tan(0.0) != 0.0) {
            return false;
        }
        if (Math::tan(-0.0) != 0.0) {
            return false;
        }
        // error handling
        if (!isnan(Math::tan(INFINITY))) {
            return false;
        }
    }

    {
        if (Math::atan(1.0) - 0.785398 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::atan(INFINITY) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::atan(0.0) != 0.0) {
            return false;
        }
        if (Math::atan(-0.0) != 0.0) {
            return false;
        }
    }
    {
        if (Math::atan(1.0L) - 0.785398 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::atan(INFINITY) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::atan(0.0L) != 0.0) {
            return false;
        }
        if (Math::atan(-0.0L) != 0.0) {
            return false;
        }
    }

    {
        // atan2(1,1) = +pi/4, Quad I
        if (Math::atan2(1.0f, 1) - 0.785398 > EpsilonValue) {
            return false;
        }
        // atan2(1, -1) = +3pi/4, Quad II
        if (Math::atan2(1.0f, -1) - 2.356194 > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -3pi/4, Quad III
        if (Math::atan2(-1.0f, -1) - (-2.356194) > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -pi/4, Quad IV
        if (Math::atan2(-1.0f, 1) - (-0.785398) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::atan2(0.0f, 0) - 0.0 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(0.0f, -0) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(7.0f, 0) - 1.570796 > EpsilonValue) {
            return false;
        }
    }
    {
        // atan2(1,1) = +pi/4, Quad I
        if (Math::atan2(1.0, 1) - 0.785398 > EpsilonValue) {
            return false;
        }
        // atan2(1, -1) = +3pi/4, Quad II
        if (Math::atan2(1.0, -1) - 2.356194 > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -3pi/4, Quad III
        if (Math::atan2(-1.0, -1) - (-2.356194) > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -pi/4, Quad IV
        if (Math::atan2(-1.0, 1) - (-0.785398) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::atan2(0.0, 0) - 0.0 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(0.0, -0) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(7.0, 0) - 1.570796 > EpsilonValue) {
            return false;
        }
    }
    {
        // atan2(1,1) = +pi/4, Quad I
        if (Math::atan2(1.0L, 1) - 0.785398 > EpsilonValue) {
            return false;
        }
        // atan2(1, -1) = +3pi/4, Quad II
        if (Math::atan2(1.0L, -1) - 2.356194 > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -3pi/4, Quad III
        if (Math::atan2(-1.0L, -1) - (-2.356194) > EpsilonValue) {
            return false;
        }
        // atan2(-1,-1) = -pi/4, Quad IV
        if (Math::atan2(-1.0L, 1) - (-0.785398) > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::atan2(0.0L, 0) - 0.0 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(0.0L, -0) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::atan2(7.0L, 0) - 1.570796 > EpsilonValue) {
            return false;
        }
    }

    return true;
}

bool testPower() {
    {
        // typical usage
        if (Math::pow(2.0, 10) - 1024.0 > EpsilonValue) {
            return false;
        }
        if (Math::pow(2.0, 0.5) - 1.414214 > EpsilonValue) {
            return false;
        }
        if (Math::pow(-2.0, -3) - (-0.125) > EpsilonValue) {
            return false;
        }
        // special values
        if (!isnan(Math::pow(-1, NAN))) {
            return false;
        }
        if (Math::pow(+1, NAN) - 1.0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::pow(INFINITY, 2))) {
            return false;
        }
        if (Math::pow(INFINITY, -1) - 0.0 > EpsilonValue) {
            return false;
        }
        // error handling
        if (!isnan(Math::pow(-1, 1.0 / 3))) {
            return false;
        }
        if (!isinf(Math::pow(-0.0, -3))) {
            return false;
        }
    }
    {
        // typical usage
        if (Math::pow(2.0L, 10) - 1024.0 > EpsilonValue) {
            return false;
        }
        if (Math::pow(2.0L, 0.5L) - 1.414214 > EpsilonValue) {
            return false;
        }
        if (Math::pow(-2.0L, -3) - (-0.125) > EpsilonValue) {
            return false;
        }
        // special values
        if (!isnan(Math::pow(-1L, NAN))) {
            return false;
        }
        if (Math::pow(+1L, NAN) - 1.0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::pow(INFINITY, 2))) {
            return false;
        }
        if (Math::pow(INFINITY, -1) - 0.0 > EpsilonValue) {
            return false;
        }
        // error handling
        if (!isnan(Math::pow(-1L, 1.0 / 3))) {
            return false;
        }
        if (!isinf(Math::pow(-0.0L, -3))) {
            return false;
        }
    }

    {
        // normal use
        if (Math::sqrt(100.0f) - 10 > EpsilonValue) {
            return false;
        }
        if (Math::sqrt(2.0f) - 1.414214 > EpsilonValue) {
            return false;
        }
        if ((1 + Math::sqrt(5.0f)) / 2 - 1.618034 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::sqrt(-0.0f) - 0.0 > EpsilonValue) {
            return false;
        }
        // error handling
        if (!isnan(Math::sqrt(-1.0f))) {
            return false;
        }
    }
    {
        // normal use
        if (Math::sqrt(100.0) - 10 > EpsilonValue) {
            return false;
        }
        if (Math::sqrt(2.0) - 1.414214 > EpsilonValue) {
            return false;
        }
        if ((1 + Math::sqrt(5.0)) / 2 - 1.618034 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::sqrt(-0.0) - 0.0 > EpsilonValue) {
            return false;
        }
        // error handling
        if (!isnan(Math::sqrt(-1.0))) {
            return false;
        }
    }
    {
        // normal use
        if (Math::sqrt(100.0L) - 10 > EpsilonValue) {
            return false;
        }
        if (Math::sqrt(2.0L) - 1.414214 > EpsilonValue) {
            return false;
        }
        if ((1 + Math::sqrt(5.0L)) / 2 - 1.618034 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::sqrt(-0.0L) - 0.0 > EpsilonValue) {
            return false;
        }
        // error handling
        if (!isnan(Math::sqrt(-1.0L))) {
            return false;
        }
    }

    {
        if (Math::cbrt(729.0) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.125) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.0) - 9.0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::cbrt(INFINITY))) {
            return false;
        }
        if (Math::cbrt(343.0) - Math::pow(343.0, 1.0 / 3) > EpsilonValue) {
            return false;
        }
    }
    {
        if (Math::cbrt(729.0f) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.125f) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.0f) - 9.0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::cbrt(INFINITY))) {
            return false;
        }
        if (Math::cbrt(343.0f) - Math::pow(343.0f, 1.0f / 3) > EpsilonValue) {
            return false;
        }
    }
    {
        if (Math::cbrt(729.0L) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.125L) - 9.0 > EpsilonValue) {
            return false;
        }
        if (Math::cbrt(-0.0L) - 9.0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::cbrt(INFINITY))) {
            return false;
        }
        if (Math::cbrt(343.0L) - Math::pow(343.0L, 1.0L / 3) > EpsilonValue) {
            return false;
        }
    }

    {
        // typical usage
        if (Math::hypot(1.0, 1) - 1.414214 > EpsilonValue) {
            return false;
        }
        // special values
        if (!isinf(hypot(NAN, INFINITY))) {
            return false;
        }
        // error handling
        if (!isinf(hypot(DBL_MAX, DBL_MAX))) {
            return false;
        }
    }
    {
        // typical usage
        if (Math::hypot(1.0f, 1) - 1.414214 > EpsilonValue) {
            return false;
        }
        // special values
        if (!isinf(hypot(NAN, INFINITY))) {
            return false;
        }
        // error handling
        if (!isinf(hypot(DBL_MAX, DBL_MAX))) {
            return false;
        }
    }
    {
        // typical usage
        if (Math::hypot(1.0L, 1) - 1.414214 > EpsilonValue) {
            return false;
        }
        // special values
        if (!isinf(hypot(NAN, INFINITY))) {
            return false;
        }
        // error handling
        if (!isinf(hypot(DBL_MAX, DBL_MAX))) {
            return false;
        }
    }

    return true;
}

bool testAbs() {
    {
        if (Math::abs(3) != 3) {
            return false;
        }
        if (Math::abs(-3) != 3) {
            return false;
        }
        if (Math::abs(0) != 0) {
            return false;
        }
    }
    {
        if (Math::abs(3L) != 3) {
            return false;
        }
        if (Math::abs(-3L) != 3) {
            return false;
        }
        if (Math::abs(0L) != 0) {
            return false;
        }
    }
    {
        if (Math::abs(3.0) != 3) {
            return false;
        }
        if (Math::abs(-3.0) != 3) {
            return false;
        }
        if (Math::abs(0.0) != 0) {
            return false;
        }
    }
    {
        if (Math::abs(3.0f) != 3) {
            return false;
        }
        if (Math::abs(-3.0f) != 3) {
            return false;
        }
        if (Math::abs(0.0f) != 0) {
            return false;
        }
    }
    {
        if (Math::abs(3.0L) != 3) {
            return false;
        }
        if (Math::abs(-3.0L) != 3) {
            return false;
        }
        if (Math::abs(0.0L) != 0) {
            return false;
        }
    }

    return true;
}

bool testExp() {
    {
        if (Math::exp(1.0) - 2.718282 > EpsilonValue) {
            return false;
        }
        if (Math::exp(0.03) - 1.03045453 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp(-0.0) - 1 > EpsilonValue) {
            return false;
        }
        if (Math::exp(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp(710.0))) {
            return false;
        }
    }
    {
        if (Math::exp(1.0L) - 2.718282 > EpsilonValue) {
            return false;
        }
        if (Math::exp(0.03L) - 1.03045453 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp(-0.0L) - 1 > EpsilonValue) {
            return false;
        }
        if (Math::exp(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp(710000.0L))) {
            return false;
        }
    }
    {
        if (Math::exp(1.0f) - 2.718282 > EpsilonValue) {
            return false;
        }
        if (Math::exp(0.03f) - 1.03045453 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp(-0.0f) - 1 > EpsilonValue) {
            return false;
        }
        if (Math::exp(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp(710.0f))) {
            return false;
        }
    }

    {
        if (Math::exp2(5.0) - 32 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(0.5) - 1.414214 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-4.0) - 0.062500 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp2(-0.9) - 0.535887 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp2(1024.0))) {
            return false;
        }
    }
    {
        if (Math::exp2(5.0f) - 32 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(0.5f) - 1.414214 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-4.0f) - 0.062500 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp2(-0.9f) - 0.535887 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp2(1024.0f))) {
            return false;
        }
    }
    {
        if (Math::exp2(5.0L) - 32 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(0.5L) - 1.414214 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-4.0L) - 0.062500 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::exp2(-0.9L) - 0.535887 > EpsilonValue) {
            return false;
        }
        if (Math::exp2(-INFINITY) - 0 > EpsilonValue) {
            return false;
        }
        //error handling
        if (!isinf(Math::exp2(81920.0L))) {
            return false;
        }
    }

    {
        if (Math::log(1.0) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::log(125.0) / Math::log(5.0) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log(1.0) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log(0.0))) {
            return false;
        }
    }
    {
        if (Math::log(1.0f) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::log(125.0f) / Math::log(5.0f) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log(1.0f) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log(0.0f))) {
            return false;
        }
    }
    {
        if (Math::log(1.0L) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::log(125.0L) / Math::log(5.0) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log(1.0L) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log(0.0L))) {
            return false;
        }
    }

    {
        if (Math::log2(65536.0) - 16 > EpsilonValue) {
            return false;
        }
        if (Math::log2(0.125) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log2((double) 0x020) - 9.041659 > EpsilonValue) {
            return false;
        }
        if (Math::log2(125.0) / Math::log2(5.0) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log2(1.0) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log2(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log2(0.0))) {
            return false;
        }
    }
    {
        if (Math::log2(65536.0f) - 16 > EpsilonValue) {
            return false;
        }
        if (Math::log2(0.125f) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log2((float) 0x020) - 9.041659 > EpsilonValue) {
            return false;
        }
        if (Math::log2(125.0f) / Math::log2(5.0f) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log2(1.0f) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log2(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log2(0.0f))) {
            return false;
        }
    }
    {
        if (Math::log2(65536.0L) - 16 > EpsilonValue) {
            return false;
        }
        if (Math::log2(0.125L) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log2((long double) 0x020) - 9.041659 > EpsilonValue) {
            return false;
        }
        if (Math::log2(125.0L) / Math::log2(5.0L) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log2(1.0L) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log2(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log2(0.0L))) {
            return false;
        }
    }

    {
        if (Math::log10(1000.0) - 3 > EpsilonValue) {
            return false;
        }
        if (Math::log10(0.001) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log10(125.0) / Math::log10(5.0) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log10(1.0) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log10(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log10(0.0))) {
            return false;
        }
    }
    {
        if (Math::log10(1000.0f) - 3 > EpsilonValue) {
            return false;
        }
        if (Math::log10(0.001f) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log10(125.0f) / Math::log10(5.0f) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log10(1.0f) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log10(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log10(0.0f))) {
            return false;
        }
    }
    {
        if (Math::log10(1000.0L) - 3 > EpsilonValue) {
            return false;
        }
        if (Math::log10(0.001L) - (-3) > EpsilonValue) {
            return false;
        }
        if (Math::log10(125.0L) / Math::log10(5.0L) - 3 > EpsilonValue) {
            return false;
        }
        // special values
        if (Math::log10(1.0L) - 0 > EpsilonValue) {
            return false;
        }
        if (!isinf(Math::log10(INFINITY))) {
            return false;
        }
        //error handling
        if (!isinf(Math::log10(0.0L))) {
            return false;
        }
    }

    return true;
}

bool testAngle() {
    {
        if (Math::toRadian(0) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(45) - 0.785398 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(90) - 1.570796 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(120) - 2.094395 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(180) - 3.141593 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(210) - 3.665191 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(270) - 4.712389 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(360) - 6.283185 > EpsilonValue) {
            return false;
        }
        if (Math::toRadian(405) - 7.068583 > EpsilonValue) {
            return false;
        }
    }

    {
        if (Math::toAngle(0) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(0.785398) - 45 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(1.570796) - 90 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(2.094395) - 120 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(3.14159) - 180 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(3.66519) - 210 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(4.712389) - 270 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(6.283185) - 360 > EpsilonValue) {
            return false;
        }
        if (Math::toAngle(7.06858) - 405 > EpsilonValue) {
            return false;
        }
    }

    return true;
}

bool getCommonValue() {
    {
        int value;
        if (!Math::getGCD(10, 5, value) && value != 5) {
            return false;
        }
        if (!Math::getGCD(12, 30, value) && value != 6) {
            return false;
        }
        if (!Math::getGCD(3, 5, value) && value != 1) {
            return false;
        }
        if (Math::getGCD(0, 5, value)) {
            return false;
        }
        if (Math::getGCD(5, 0, value)) {
            return false;
        }
        if (Math::getGCD(-5, 7, value)) {
            return false;
        }
        if (Math::getGCD(7, -5, value)) {
            return false;
        }
    }

    {
        int value;
        if (!Math::getSCM(10, 5, value) && value != 10) {
            return false;
        }
        if (!Math::getSCM(7, 5, value) && value != 35) {
            return false;
        }
        if (Math::getSCM(0, 5, value)) {
            return false;
        }
        if (Math::getSCM(5, 0, value)) {
            return false;
        }
        if (Math::getSCM(-5, 7, value)) {
            return false;
        }
        if (Math::getSCM(7, -5, value)) {
            return false;
        }
    }

    return true;
}

bool testCoord() {
    {
        float x, y;
        Math::calcCoord(0, 0, x, y);
        if (!(x == 0 && y == 0)) {
            return false;
        }
        Math::calcCoord(60, 0, x, y);
        if (!(x == 0 && y == 0)) {
            return false;
        }
        Math::calcCoord(0, 100, x, y);
        if (!(x == 100 && y == 0)) {
            return false;
        }
        Math::calcCoord(45, 100, x, y);
        if (!(x - 70.7107 < EpsilonValue && y - 70.7107 < EpsilonValue)) {
            return false;
        }
        Math::calcCoord(90, 100, x, y);
        if (!(x == 0 && y == 100)) {
            return false;
        }
        Math::calcCoord(120, 100, x, y);
        if (!(x - (-50) < EpsilonValue && y - 86.602539 < EpsilonValue)) {
            return false;
        }
        Math::calcCoord(180, 100, x, y);
        if (!(x == -100 && y == 0)) {
            return false;
        }
        Math::calcCoord(210, 100, x, y);
        if (!(x - (-86.6025) < EpsilonValue && y - (-50) < EpsilonValue)) {
            return false;
        }
        Math::calcCoord(270, 100, x, y);
        if (!(x == 0 && y == -100)) {
            return false;
        }
        Math::calcCoord(315, 100, x, y);
        if (!(x - 70.7106628 < EpsilonValue && y - (-70.7106857) < EpsilonValue)) {
            return false;
        }
        Math::calcCoord(360, 100, x, y);
        if (!(x == 100 && y == 0)) {
            return false;
        }
        Math::calcCoord(405, 100, x, y);
        if (!(x - 70.7107 < EpsilonValue && y - 70.7107 < EpsilonValue)) {
            return false;
        }
    }

    {
        float angle, l;
        Math::calcPolarCoord(0, 0, angle, l);
        if (!(angle == 0 && l == 0)) {
            return false;
        }
        Math::calcPolarCoord(0, 0, angle, l);
        if (!(angle == 0 && l == 0)) {
            return false;
        }
        Math::calcPolarCoord(100, 0, angle, l);
        if (!(angle == 0 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(70.7106781f, 70.7106781f, angle, l);
        if (!(angle - 45 < EpsilonValue && l - 100 < EpsilonValue)) {
            return false;
        }
        Math::calcPolarCoord(0, 100, angle, l);
        if (!(angle == 90 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(-50, 86.602539f, angle, l);
        if (!(angle == 120 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(-100, 0, angle, l);
        if (!(angle == 180 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(-86.602539f, -49.9999962f, angle, l);
        if (!(angle - 210 < EpsilonValue && l - 100 < EpsilonValue)) {
            return false;
        }
        Math::calcPolarCoord(0, -100, angle, l);
        if (!(angle == 270 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(70.7106781f, -70.7106781f, angle, l);
        if (!(angle == 315 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(100, 0, angle, l);
        if (!(angle == 0 && l == 100)) {
            return false;
        }
        Math::calcPolarCoord(70.7106781f, 70.7106781f, angle, l);
        if (!(angle - 45 < EpsilonValue && l - 100 < EpsilonValue)) {
            return false;
        }
    }

    return true;
}

bool testMod() {
    {
        if (Math::mod(5.1, 3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1, 3) - (-2.1) > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1, -3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1, -3) - (-2.1) > EpsilonValue) {
            return false;
        }

        // special values
        if (Math::mod(0.0, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-0.0, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1, (double) INFINITY) - 5.1 > EpsilonValue) {
            return false;
        }

        // error handling
        if (!isnan(Math::mod(5.1, 0))) {
            return false;
        }
    }
    {
        if (Math::mod(5.1f, 3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1f, 3) - (-2.1) > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1f, -3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1f, -3) - (-2.1) > EpsilonValue) {
            return false;
        }

        // special values
        if (Math::mod(0.0f, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-0.0f, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1f, (float) INFINITY) - 5.1 > EpsilonValue) {
            return false;
        }

        // error handling
        if (!isnan(Math::mod(5.1, 0))) {
            return false;
        }
    }
    {
        if (Math::mod(5.1L, 3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1L, 3) - (-2.1) > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1L, -3) - 2.1 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-5.1L, -3) - (-2.1) > EpsilonValue) {
            return false;
        }

        // special values
        if (Math::mod(0.0L, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(-0.0L, 1) - 0 > EpsilonValue) {
            return false;
        }
        if (Math::mod(5.1L, (long double) INFINITY) - 5.1 > EpsilonValue) {
            return false;
        }

        // error handling
        if (!isnan(Math::mod(5.1L, 0))) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testMinMax()) {
        return 1;
    }
    if (!testRound()) {
        return 2;
    }
    if (!testSinCos()) {
        return 3;
    }
    if (!testTan()) {
        return 4;
    }
    if (!testPower()) {
        return 5;
    }
    if (!testAbs()) {
        return 6;
    }
    if (!testExp()) {
        return 7;
    }
    if (!testAngle()) {
        return 8;
    }
    if (!getCommonValue()) {
        return 9;
    }
    if (!testCoord()) {
        return 10;
    }
    if (!testMod()) {
        return 11;
    }

    return 0;
}

