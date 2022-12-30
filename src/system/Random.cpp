//
//  Random.cpp
//  common
//
//  Created by baowei on 2020/12/26.
//  Copyright © 2020 com. All rights reserved.
//

#include "system/Random.h"
#include "data/ValueType.h"

namespace System {
    int32_t Random::getRandValue(int32_t min, int32_t max) {
        return getIntegerRandValue<int32_t>(min, max);
    }

    uint32_t Random::getRandValue(uint32_t min, uint32_t max) {
        return getIntegerRandValue<uint32_t>(min, max);
    }

    int64_t Random::getRandValue(int64_t min, int64_t max) {
        return getIntegerRandValue<int64_t>(min, max);
    }

    uint64_t Random::getRandValue(uint64_t min, uint64_t max) {
        return getIntegerRandValue<uint64_t>(min, max);
    }

    float Random::getRandValue(float min, float max) {
        return getFloatRandValue<float>(min, max);
    }

    double Random::getRandValue(double min, double max) {
        return getFloatRandValue<double>(min, max);
    }

    template<class T>
    T Random::getFloatRandValue(T min, T max) {
        srand();

        if (min > max) {
            return (T) rand() / (T) RAND_MAX * (min - max) + max;
        } else {
            return (T) rand() / (T) RAND_MAX * (max - min) + min;
        }
    }

    template<class T>
    T Random::getIntegerRandValue(T min, T max) {
        srand();

        T range = min > max ? min - max : max - min;
        if (range <= (T) RAND_MAX) {
            int value = rand() % range;
            return value + (min > max ? max : min);
        } else {
            return (T) ((double) range / (double) RAND_MAX * (double) rand()) + (min > max ? max : min);
        }
    }

    void Random::srand() {
        static time_t curtime = 0;
        if (curtime == 0) {
            curtime = time(NULL);
            ::srand((unsigned) curtime);
        }
    }

    bool Random::getRandValues(int count, ByteArray &values) {
        if (count > 0 && count < 65535) {
            for (int i = 0; i < count; i++) {
                uint8_t value = (uint8_t) getRandValue(Byte::MinValue, Byte::MaxValue);
                values.add(value);
            }
            return true;
        }
        return false;
    }

    String Random::getRandValues(int count) {
        ByteArray array;
        getRandValues(count, array);
        return array.toHexString();
    }
}
