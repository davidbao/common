#ifndef RANDOM_H
#define RANDOM_H

#include "data/ByteArray.h"

using namespace Data;

namespace System {
    class Random {
    public:
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
        static T getIntegerRandValue(T min, T max);

        template<class T>
        static T getFloatRandValue(T min, T max);

        static void srand();
    };
}

#endif // RANDOM_H
