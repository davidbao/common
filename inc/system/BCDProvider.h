//
//  BCDProvider.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef BCDProvider_h
#define BCDProvider_h

#include "data/ByteArray.h"

using namespace Data;

namespace System {
    class BCDProvider {
    public:
        static const size_t MaxBCDCount = 8;

        static uint8_t bin2bcd(uint8_t value);

        template<class T>
        static T bin2bcd(T value, bool bigEndian = true) {
            static const int count = sizeof(T);
            BufferValue bv = {};
            bin2buffer(value, bv.buffer, bigEndian);
            if (isLittleEndian()) {
                reverse(bv.buffer, count);
            }

            T result;
            memcpy(&result, bv.buffer, count);
            return result;
        }

        static uint64_t bin2bcd(const uint8_t *buffer, off_t offset, size_t count, bool bigEndian = true);

        template<class T>
        static void bin2buffer(T value, uint8_t buffer[sizeof(T)], bool bigEndian = true) {
            static const int count = sizeof(T);
            memset(buffer, 0, count);
            int offset = 0;
            for (uint64_t multiple: Multiples) {
                uint64_t v = value / multiple;
                uint64_t hi = v - v / 10 * 10;
                uint64_t lo = (value - v * multiple) * 10 / multiple;
                buffer[offset++] = (uint8_t) ((hi << 4) + lo);
                if (offset >= count) {
                    break;
                }
            }

            if (bigEndian) {
                reverse(buffer, count);
            }
        }

        template<class T>
        static void bin2buffer(T value, ByteArray &buffer, bool bigEndian = true) {
            static const int count = sizeof(T);
            uint8_t buf[count];
            bin2buffer(value, buf, bigEndian);
            buffer = ByteArray(buf, sizeof(buf));
        }

        static uint8_t bcd2bin(uint8_t value);

        template<class T>
        static T bcd2bin(T value, bool bigEndian = true) {
            static const int count = sizeof(T);
            BufferValue bv(value);
            if (!isBigEndian()) {
                reverse(bv.buffer, count);
            }

            return bcd2bin(bv.buffer, 0, count, bigEndian);
        }

        static uint64_t bcd2bin(const uint8_t *buffer, off_t offset, size_t count, bool bigEndian = true);

        template<class T>
        static void bcd2buffer(T value, uint8_t buffer[sizeof(T)], bool bigEndian = true) {
            static const int count = sizeof(T);
            T result = bcd2bin(value, true);
            if ((isBigEndian() && !bigEndian) ||
                (isLittleEndian() && bigEndian)) {
                memcpy(buffer, &result, count);
                reverse(buffer, count);
            } else {
                memcpy(buffer, &result, count);
            }
        }

        template<class T>
        static void bcd2buffer(T value, ByteArray &buffer, bool bigEndian = true) {
            static const int count = sizeof(T);
            uint8_t temp[count] = {};
            bcd2buffer(value, temp, bigEndian);
            buffer = ByteArray(temp, count);
        }

    private:
        static bool isLittleEndian();

        static bool isBigEndian();

        static void reverse(uint8_t *buffer, size_t count) {
            auto temp = new uint8_t[count];
            for (size_t i = 0; i < count; ++i) {
                temp[i] = buffer[count - 1 - i];
            }
            memcpy(buffer, temp, count);
            delete[] temp;
        }

    private:
        union BufferValue {
            uint8_t buffer[MaxBCDCount];
            int64_t lValue;
            int32_t nValue;
            int16_t sValue;
            uint64_t ulValue;
            uint32_t unValue;
            uint16_t usValue;

            BufferValue();

            explicit BufferValue(int64_t value);

            explicit BufferValue(int32_t value);

            explicit BufferValue(int16_t value);

            explicit BufferValue(uint64_t value);

            explicit BufferValue(uint32_t value);

            explicit BufferValue(uint16_t value);
        };

    private:
        static const uint64_t Multiples[MaxBCDCount];
    };
}

#endif // BCDProvider_h
