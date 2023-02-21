//
//  RandomTest.cpp
//  common
//
//  Created by baowei on 2022/9/26.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "system/Random.h"

using namespace System;

bool testFloatValue() {
    {
        float min = 0.0f, max = 100.0f;
        for (int i = 0; i < 1000; i++) {
            float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }
    {
        float min = -100.0f, max = 100.0f;
        for (int i = 0; i < 1000; i++) {
            float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }
    {
        float min = 0.0f, max = 1.234f;
        for (int i = 0; i < 1000; i++) {
            float value = Random::getRandValue(min, max);
//        printf("value: %f\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }

    {
        double min = 0.0, max = 100.0;
        for (int i = 0; i < 1000; i++) {
            double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }
    {
        double min = -100.0, max = 100.0;
        for (int i = 0; i < 1000; i++) {
            double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }
    {
        double min = (double) -RAND_MAX - 100.0, max = (double) RAND_MAX + 100.0f;
        for (int i = 0; i < 1000; i++) {
            double value = Random::getRandValue(min, max);
//        printf("value: %lf\n", value);
            bool result = value >= min && value <= max;
            if (!result)
                return false;
        }
    }

    return true;
}

bool testIntValue() {
    {
        int min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            int value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        int min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            int value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }

    {
        uint32_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint32_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        uint32_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint32_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= max && value <= min))
                return false;
        }
    }

    {
        int64_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        int64_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        int64_t min = -(int64_t) RAND_MAX - (int64_t) 100, max = (int64_t) RAND_MAX + (int64_t) 100;
        for (int i = 0; i < 1000; i++) {
            int64_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }

    {
        uint64_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint64_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        uint64_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint64_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= max && value <= min))
                return false;
        }
    }

    {
        int16_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            int16_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        int16_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            int16_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }

    {
        uint16_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint16_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        uint16_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint16_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= max && value <= min))
                return false;
        }
    }

    {
        int8_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            int8_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        int8_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            int8_t value = Random::getRandValue(min, max);
//        printf("value: %lld\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }

    {
        uint8_t min = 0, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint8_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= min && value <= max))
                return false;
        }
    }
    {
        uint8_t min = -100, max = 100;
        for (int i = 0; i < 1000; i++) {
            uint8_t value = Random::getRandValue(min, max);
//        printf("value: %d\n", value);
            if (!(value >= max && value <= min))
                return false;
        }
    }

    return true;
}

bool testGetRandValues() {
    {
        ByteArray buffer;
        if (!Random::getRandValues(100, buffer)) {
            return false;
        }
    }
    {
        ByteArray buffer;
        if (Random::getRandValues(0, buffer)) {
            return false;
        }
    }
    {
        ByteArray buffer;
        if (Random::getRandValues(100000, buffer)) {
            return false;
        }
    }

    {
        String buffer = Random::getRandValues(100);
        if (buffer.length() != 200) {
            return false;
        }
    }
    {
        String buffer = Random::getRandValues(0);
        if (!buffer.isNullOrEmpty()) {
            return false;
        }
    }
    {
        String buffer = Random::getRandValues(100000);
        if (!buffer.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testBoolValue() {
    {
        int trueCount = 0;
        int falseCount = 0;
        for (int i = 0; i < 1000; i++) {
            bool value = Random::getRandValue();
            if (value) {
                trueCount++;
            } else {
                falseCount++;
            }
//            printf("value: %d\n", value);
        }
        if (trueCount == 0 || falseCount == 0) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testFloatValue())
        return 1;

    if (!testIntValue())
        return 2;

    if (!testBoolValue())
        return 3;

    if (!testGetRandValues())
        return 4;

    return 0;
}

