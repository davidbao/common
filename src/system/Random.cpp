//
//  Random.cpp
//  common
//
//  Created by baowei on 2020/12/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "system/Random.h"
#include "data/ValueType.h"
#include <random>

using namespace std;

namespace System {
    bool Random::getRandValue() {
        return getIntRandValue(0, 1) == 1;
    }

    int8_t Random::getRandValue(int8_t min, int8_t max) {
        return (int8_t) getIntRandValue((int16_t) min, (int16_t) max);
    }

    uint8_t Random::getRandValue(uint8_t min, uint8_t max) {
        return (uint8_t) getIntRandValue((uint16_t) min, (uint16_t) max);
    }

    int16_t Random::getRandValue(int16_t min, int16_t max) {
        return getIntRandValue(min, max);
    }

    uint16_t Random::getRandValue(uint16_t min, uint16_t max) {
        return getIntRandValue(min, max);
    }

    int32_t Random::getRandValue(int32_t min, int32_t max) {
        return getIntRandValue<int32_t>(min, max);
    }

    uint32_t Random::getRandValue(uint32_t min, uint32_t max) {
        return getIntRandValue<uint32_t>(min, max);
    }

    int64_t Random::getRandValue(int64_t min, int64_t max) {
        return getIntRandValue<int64_t>(min, max);
    }

    uint64_t Random::getRandValue(uint64_t min, uint64_t max) {
        return getIntRandValue<uint64_t>(min, max);
    }

    float Random::getRandValue(float min, float max) {
        return getRealRandValue<float>(min, max);
    }

    double Random::getRandValue(double min, double max) {
        return getRealRandValue<double>(min, max);
    }

    template<class T>
    T Random::getIntRandValue(T min, T max) {
        std::random_device rd;
        if (max >= min) {
            std::uniform_int_distribution<T> dist(min, max);
            return dist(rd);
        } else {
            std::uniform_int_distribution<T> dist(max, min);
            return dist(rd);
        }
    }

    template<class T>
    T Random::getRealRandValue(T min, T max) {
        std::random_device rd;
        if (max >= min) {
            std::uniform_real_distribution<T> dist(min, max);
            return dist(rd);
        } else {
            std::uniform_real_distribution<T> dist(max, min);
            return dist(rd);
        }
    }

    bool Random::getRandValues(int count, ByteArray &values) {
        if (count > 0 && count < 65535) {
            for (int i = 0; i < count; i++) {
                values.add(getRandValue(Byte::MinValue, Byte::MaxValue));
            }
            return true;
        }
        return false;
    }

    String Random::getRandValues(int count) {
        ByteArray array;
        if (getRandValues(count, array)) {
            return array.toHexString();
        }
        return String::Empty;
    }
}
