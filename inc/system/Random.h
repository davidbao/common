//
//  Random.h
//  common
//
//  Created by baowei on 2020/12/26.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef Random_h
#define Random_h

#include "data/String.h"
#include "data/ByteArray.h"

using namespace Data;

namespace System {
    class Random {
    public:
        static bool getRandValue();

        static int8_t getRandValue(int8_t min, int8_t max);

        static uint8_t getRandValue(uint8_t min, uint8_t max);

        static int16_t getRandValue(int16_t min, int16_t max);

        static uint16_t getRandValue(uint16_t min, uint16_t max);

        static int32_t getRandValue(int32_t min, int32_t max);

        static uint32_t getRandValue(uint32_t min, uint32_t max);

        static int64_t getRandValue(int64_t min, int64_t max);

        static uint64_t getRandValue(uint64_t min, uint64_t max);

        static float getRandValue(float min, float max);

        static double getRandValue(double min, double max);

        static bool getRandValues(int count, ByteArray &values);

        static String getRandValues(int count);

    private:
        template<class T>
        static T getIntRandValue(T min, T max);

        template<class T>
        static T getRealRandValue(T min, T max);
    };
}

#endif // Random_h
