//
//  BCDUtilities.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef BCDUtilities_h
#define BCDUtilities_h

#include "data/String.h"

namespace System {
    class BCDUtilities {
    public:
        static uint8_t ByteToBCD(uint8_t value);

        static uint8_t BCDToByte(uint8_t value);

        static void UInt16ToBCD(uint16_t value, uint8_t *buffer);

        static uint16_t BCDToUInt16(const uint8_t *buffer);

        static uint16_t BCDToUInt16(uint16_t value, bool bigEndian = true);

        static void UInt32ToBCD(uint32_t value, uint8_t *buffer);

        static uint32_t BCDToUInt32(const uint8_t *buffer);

        static uint32_t BCDToUInt32(uint32_t value, bool bigEndian = true);

        static void Int64ToBCD(int64_t value, uint8_t *buffer, int length);

        static int64_t BCDToInt64(const uint8_t *buffer, off_t offset, int count);

    private:
        static const int64_t OneHundred1 = (int64_t) 100;
        static const int64_t OneHundred2 = (int64_t) 100 * 100;
        static const int64_t OneHundred3 = (int64_t) 100 * 100 * 100;
        static const int64_t OneHundred4 = (int64_t) 100 * 100 * 100 * 100;
        static const int64_t OneHundred5 = (int64_t) 100 * 100 * 100 * 100 * 100;
        static const int64_t OneHundred6 = (int64_t) 100 * 100 * 100 * 100 * 100 * 100;
        static const int64_t OneHundred7 = (int64_t) 100 * 100 * 100 * 100 * 100 * 100 * 100;
        static const int64_t OneHundred8 = (int64_t) 100 * 100 * 100 * 100 * 100 * 100 * 100 * 100;

        static const int MaxBCDCount = 8;
        static const int64_t OneHundreds[MaxBCDCount];
    };
}

#endif // BCDUtilities_h
