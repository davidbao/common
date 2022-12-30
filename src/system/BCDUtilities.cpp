#include "system/BCDUtilities.h"
#include "exception/Exception.h"

namespace System {
    const int64_t BCDUtilities::OneHundreds[8] = {OneHundred1, OneHundred2, OneHundred3, OneHundred4, OneHundred5,
                                                  OneHundred6, OneHundred7, OneHundred8};

    uint8_t BCDUtilities::ByteToBCD(uint8_t value) {
        // 20 ====> 0x20
        if (value > 99) {
            throw ArgumentException("value must be between 0 to 99.", "value");
        }
        return (uint8_t) (value / 10 << 4 | value % 10);
    }

    uint8_t BCDUtilities::BCDToByte(uint8_t value) {
        // 0x20 ====> 20
        uint8_t buffer[1];
        buffer[0] = value;
        return (uint8_t) BCDToInt64(buffer, 0, 1);
    }

    void BCDUtilities::UInt16ToBCD(uint16_t value, uint8_t *buffer) {
        Int64ToBCD(value, buffer, 2);
    }

    uint16_t BCDUtilities::BCDToUInt16(const uint8_t *buffer) {
        return (uint16_t) BCDToInt64(buffer, 0, sizeof(uint16_t));
    }

    uint16_t BCDUtilities::BCDToUInt16(uint16_t value, bool bigEndian) {
        uint8_t buffer[2];
        if (bigEndian) {
            buffer[0] = ((value >> 8) & 0xFF);
            buffer[1] = ((value) & 0xFF);
        } else {
            buffer[1] = ((value >> 8) & 0xFF);
            buffer[0] = ((value) & 0xFF);
        }
        return BCDToUInt16(buffer);
    }

    void BCDUtilities::UInt32ToBCD(uint32_t value, uint8_t *buffer) {
        Int64ToBCD(value, buffer, 4);
    }

    uint32_t BCDUtilities::BCDToUInt32(const uint8_t *buffer) {
        return (uint32_t) BCDToInt64(buffer, 0, sizeof(uint32_t));
    }

    uint32_t BCDUtilities::BCDToUInt32(uint32_t value, bool bigEndian) {
        uint8_t buffer[4];
        if (bigEndian) {
            buffer[0] = (uint8_t) ((value & 0xFF000000) >> 24);
            buffer[1] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[2] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[3] = (uint8_t) ((value & 0x000000FF) >> 0);
        } else {
            buffer[3] = (uint8_t) ((value & 0xFF000000) >> 24);
            buffer[2] = (uint8_t) ((value & 0x00FF0000) >> 16);
            buffer[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
            buffer[0] = (uint8_t) ((value & 0x000000FF) >> 0);
        }
        return BCDToUInt32(buffer);
    }

    void BCDUtilities::Int64ToBCD(int64_t value, uint8_t *buffer, int length) {
        if (value >= OneHundreds[MaxBCDCount - 1]) {
            return;
        } else if (value == 0) {
            memset(buffer, 0, length);
            return;
        }

        uint8_t all[32];
        off_t offset = 0;
        for (int i = 0; i < MaxBCDCount; i++) {
            int64_t multiple = OneHundreds[MaxBCDCount - 1 - i] / 10;

            int64_t hi1 = (int64_t) (value / multiple);
            int64_t hi = hi1 - hi1 / 10 * 10;
            int64_t lo = (int64_t) ((value - hi1 * multiple) * 10 / multiple);

            all[offset++] = (uint8_t) ((hi << 4) + lo);
        }

        int index = 0;
        for (int i = 0; i < offset; i += 2) {
            if (i < offset - 1) {
                if (!(all[i] == 0 && all[i + 1] == 0)) {
                    index = i;
                    break;
                }
            }
        }

        int count = offset - index;
        if (count < length) {
            memset(buffer, 0, length - count);
            memcpy(buffer + (length - count), &*(all + index), count);
        } else {
            memcpy(buffer, &*(all + index + count - length), length);
        }
    }

    int64_t BCDUtilities::BCDToInt64(const uint8_t *buffer, off_t offset, int count) {
        int64_t value = 0;
        uint8_t *temp = nullptr;
        if (count % 2 != 0) {
            temp = new uint8_t[count + 1];
            temp[0] = 0x00;
            memcpy(&*(temp + 1), &*(buffer + offset), count);
            buffer = temp;
            offset = 0;
            count = count + 1;
        }

        int length = offset + count;
        for (int i = offset; i < length; i++) {
            int hi = buffer[i] >> 4;
            int lo = buffer[i] & 0x0F;

            int64_t multiple = OneHundreds[length - 1 - i] / 10;

            value += (int64_t) (hi * multiple + lo * multiple / 10);
        }

        delete[] temp;

        return value;
    }
}
