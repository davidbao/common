#ifndef CHECKUTILITIES_H
#define CHECKUTILITIES_H

#include "data/Convert.h"
#include "data/ByteArray.h"

using namespace Data;

namespace System {
    // Block Check Character
    class BccUtilities {
    public:
        static uint8_t checkByBit(const uint8_t *buffer, off_t offset, size_t count);

        static uint8_t checkByBit(const ByteArray &buffer);
    };

    class SumUtilities {
    public:
        static uint8_t checkByBit(const uint8_t *buffer, off_t offset, size_t count);
    };

    // Longitudinal Redundancy Check
    class LrcUtilities {
    public:
        static uint8_t checkByBit(const uint8_t *buffer, off_t offset, size_t count);
    };

    // Cyclic Redundancy Check 16
    class Crc16Utilities {
    public:
        static unsigned short
        checkByBit(const uint8_t *buffer, off_t offset, size_t count, unsigned short initValue = 0x0,
                   unsigned short polynomial = 0xA001);

        static unsigned short
        checkByBit(const ByteArray &buffer, unsigned short initValue = 0x0, unsigned short polynomial = 0xA001);

        static unsigned short
        quickCheckByBit(const uint8_t *buffer, off_t offset, size_t count, unsigned short initValue = 0x0);

        static unsigned short quickCheckByBit(const ByteArray &buffer, unsigned short initValue = 0x0);

    private:
        static const uint8_t s_aCRCHi[];
        static const uint8_t s_aCRCLo[];
    };

    // Cyclic Redundancy Check 8
    class Crc8Utilities {
    public:
        static uint8_t checkByBit(const uint8_t *buffer, off_t offset, size_t count, uint8_t initValue = 0x0,
                                  uint8_t polynomial = 0x8C);

        static uint8_t checkByBit(const ByteArray &buffer, uint8_t initValue = 0x0, uint8_t polynomial = 0x8C);

        static uint8_t quickCheckByBit(const uint8_t *buffer, off_t offset, size_t count, uint8_t initValue = 0x0);

        static uint8_t quickCheckByBit(const ByteArray &buffer, uint8_t initValue = 0x0);

        static uint8_t checkByBit(uint8_t data);

    private:
        static const uint8_t s_aCRC[];
    };
}

#endif // CHECKUTILITIES_H
