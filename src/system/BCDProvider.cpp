//
//  BCDProvider.cpp
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/BCDProvider.h"
#include "system/Math.h"

namespace System {
    const uint64_t BCDProvider::Multiples[8] = {
            (uint64_t) Math::pow(10.0, 1.0), (uint64_t) Math::pow(10.0, 3.0),
            (uint64_t) Math::pow(10.0, 5.0), (uint64_t) Math::pow(10.0, 7.0),
            (uint64_t) Math::pow(10.0, 9.0), (uint64_t) Math::pow(10.0, 11.0),
            (uint64_t) Math::pow(10.0, 13.0), (uint64_t) Math::pow(10.0, 15.0),
    };

    uint8_t BCDProvider::bin2bcd(uint8_t value) {
        return ((value / 10) << 4) + value % 10;
    }

    uint64_t BCDProvider::bin2bcd(const uint8_t *buffer, off_t offset, size_t count, bool bigEndian) {
        if (count > MaxBCDCount) {
            return 0;
        }

        BufferValue bv = {};
        memcpy(bv.buffer, buffer + offset, count);
        if ((isBigEndian() && !bigEndian) ||
            (isLittleEndian() && bigEndian)) {
            reverse(bv.buffer, count);
        }

        if (count == sizeof(uint16_t)) {
            return bin2bcd(bv.usValue, true);
        } else if (count == sizeof(uint32_t)) {
            return bin2bcd(bv.unValue, true);
        } else if (count == sizeof(uint64_t)) {
            return bin2bcd(bv.ulValue, true);
        } else if (count == sizeof(uint8_t)) {
            return bin2bcd(buffer[offset]);
        }
        return 0;
    }

    uint8_t BCDProvider::bcd2bin(uint8_t value) {
        return (value & 0x0f) + (value >> 4) * 10;
    }

    uint64_t BCDProvider::bcd2bin(const uint8_t *buffer, off_t offset, size_t count, bool bigEndian) {
        if (count > MaxBCDCount) {
            return 0;
        }

        uint64_t result = 0;
        for (size_t i = 0; i < count; ++i) {
            auto pos = bigEndian ? count - 1 + offset - i : offset + i;
            result += bcd2bin(buffer[pos]) * Multiples[i] / 10;
        }
        return result;
    }

    bool BCDProvider::isLittleEndian() {
        int x = 1;
        return *(char *) &x == '\1';
    }

    bool BCDProvider::isBigEndian() {
        return !isLittleEndian();
    }

    BCDProvider::BufferValue::BufferValue() : lValue(0) {
    }

    BCDProvider::BufferValue::BufferValue(int64_t value) : BufferValue() {
        lValue = value;
    }

    BCDProvider::BufferValue::BufferValue(int32_t value) : BufferValue() {
        nValue = value;
    }

    BCDProvider::BufferValue::BufferValue(int16_t value) : BufferValue() {
        sValue = value;
    }

    BCDProvider::BufferValue::BufferValue(uint64_t value) : BufferValue() {
        ulValue = value;
    }

    BCDProvider::BufferValue::BufferValue(uint32_t value) : BufferValue() {
        unValue = value;
    }

    BCDProvider::BufferValue::BufferValue(uint16_t value) : BufferValue() {
        usValue = value;
    }
}
