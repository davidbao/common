//
//  RandomTest.cpp
//  common
//
//  Created by baowei on 2022/9/26.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "system/Random.h"

using namespace Common;

bool testFloat32_1() {
    float min = 0.0f, max = 100.0f;
    for (uint i = 0; i < 1000; i++) {
        float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testFloat32_2() {
    float min = -100.0f, max = 100.0f;
    for (uint i = 0; i < 1000; i++) {
        float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testFloat32_3() {
    float min = 0.0f, max = 1.234f;
    for (uint i = 0; i < 1000; i++) {
        float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testFloat64_1() {
    double min = 0.0, max = 100.0;
    for (uint i = 0; i < 1000; i++) {
        double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testFloat64_2() {
    double min = -100.0, max = 100.0;
    for (uint i = 0; i < 1000; i++) {
        double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testFloat64_3() {
    double min = (double)-RAND_MAX - 100.0, max = (double)RAND_MAX + 100.0f;
    for (uint i = 0; i < 1000; i++) {
        double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testInt32_1() {
    int min = 0, max = 100;
    for (uint i = 0; i < 1000; i++) {
        int value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testInt32_2() {
    int min = -100, max = 100;
    for (uint i = 0; i < 1000; i++) {
        int value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testUInt32_1() {
    uint min = 0, max = 100;
    for (uint i = 0; i < 1000; i++) {
        uint value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testUInt32_2() {
    uint min = -100, max = 100;
    for (uint i = 0; i < 1000; i++) {
        uint value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
        if(!((value >= max) && (value <= min)))
            return false;
    }
    return true;
}

bool testInt64_1() {
    int64_t min = 0, max = 100;
    for (uint i = 0; i < 1000; i++) {
        int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testInt64_2() {
    int64_t min = -100, max = 100;
    for (uint i = 0; i < 1000; i++) {
        int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

bool testInt64_3() {
    int64_t min = -(int64_t) RAND_MAX - (int64_t) 100, max = (int64_t) RAND_MAX + (int64_t) 100;
    for (uint i = 0; i < 1000; i++) {
        int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
        if(!((value >= min) && (value <= max)))
            return false;
    }
    return true;
}

int main() {
    if(!testFloat32_1())
        return 1;
    if(!testFloat32_2())
        return 2;
    if(!testFloat32_3())
        return 3;
    if(!testFloat64_1())
        return 4;
    if(!testFloat64_2())
        return 5;
    if(!testFloat64_3())
        return 6;
    if(!testInt32_1())
        return 7;
    if(!testInt32_2())
        return 8;
    if(!testUInt32_1())
        return 9;
    if(!testUInt32_2())
        return 10;
    if(!testInt64_1())
        return 11;
    if(!testInt64_2())
        return 12;
    if(!testInt64_3())
        return 13;

    return 0;
}

