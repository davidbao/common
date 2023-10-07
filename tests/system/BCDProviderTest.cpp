//
//  BCDProvider.cpp
//  common
//
//  Created by baowei on 2023/9/26.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/BCDProvider.h"

using namespace System;

template<class T>
bool test_bcd2bin_value(T bins[], T bcds[]) {
    {
        T actual = BCDProvider::bcd2bin(bcds[0]);
        if (actual != bins[0]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bcd2bin(bcds[1]);
        if (actual != bins[1]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bcd2bin(bcds[2]);
        if (actual != bins[2]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bcd2bin(bcds[3]);
        if (actual != bins[3]) {
            return false;
        }
    }

    {
        T actual = BCDProvider::bcd2bin(bcds[4], false);
        if (actual != bins[4]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bcd2bin(bcds[5], false);
        if (actual != bins[5]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bcd2bin(bcds[6], false);
        if (actual != bins[6]) {
            return false;
        }
    }
    {
        if (typeid(T) == typeid(uint8_t)) {
            T actual = BCDProvider::bcd2bin(bcds[7]);
            if (actual != bins[7]) {
                return false;
            }
        } else {
            T actual = BCDProvider::bcd2bin(bcds[7], false);
            if (actual != bins[7]) {
                return false;
            }
        }
    }

    return true;
}

bool test_bcd2bin() {
    {
        uint8_t bins[] = {19, 0, 99, 32, 19, 0, 99, 32};
        uint8_t bcds[] = {0x19, 0x00, 0x99, 0x32, 0x19, 0x00, 0x99, 0x32};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint16_t bins[] = {1965, 0, 9999, 3232, 1965, 0, 9999, 3232};
        uint16_t bcds[] = {0x1965, 0x0000, 0x9999, 0x3232, 0x6519, 0x0000, 0x9999, 0x3232};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint32_t bins[] = {19651331, 0, 99999999, 32323232, 19651331, 0, 99999999, 32323232};
        uint32_t bcds[] = {0x19651331, 0x00000000, 0x99999999, 0x32323232,
                           0x31136519, 0x00000000, 0x99999999, 0x32323232};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint64_t bins[] = {1965133124678765, 0x0000000000000000, 9999999999999999, 3232323232323232,
                           1965133124678765, 0x0000000000000000, 9999999999999999, 3232323232323232};
        uint64_t bcds[] = {0x1965133124678765, 0x0000000000000000, 0x9999999999999999, 0x3232323232323232,
                           0x6587672431136519, 0x0000000000000000, 0x9999999999999999, 0x3232323232323232};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint8_t actual[2];
        uint8_t expect[2] = {0x07, 0xAD};
        BCDProvider::bcd2buffer((int16_t) 0x1965, actual);
        if (memcmp(actual, expect, sizeof(actual)) != 0) {
            return false;
        }
    }
    {
        uint8_t actual[2];
        uint8_t expect[2] = {0xAD, 0x07};
        BCDProvider::bcd2buffer((int16_t) 0x1965, actual, false);
        if (memcmp(actual, expect, sizeof(actual)) != 0) {
            return false;
        }
    }

    {
        ByteArray actual;
        ByteArray expect = {0x07, 0xAD};
        BCDProvider::bcd2buffer((int16_t) 0x1965, actual);
        if (actual != expect) {
            return false;
        }
    }
    {
        ByteArray actual;
        ByteArray expect = {0xAD, 0x07};
        BCDProvider::bcd2buffer((int16_t) 0x1965, actual, false);
        if (actual != expect) {
            return false;
        }
    }

    {
        uint8_t buffer[2] = {0x19, 0x65};
        uint16_t actual = BCDProvider::bcd2bin(buffer, 0, 2);
        if (actual != 1965) {
            return false;
        }
    }
    {
        uint8_t buffer[2] = {0x65, 0x19};
        uint16_t actual = BCDProvider::bcd2bin(buffer, 0, 2, false);
        if (actual != 1965) {
            return false;
        }
    }

    return true;
}

template<class T>
bool test_bin2bcd_value(T bins[], T bcds[]) {
    {
        T actual = BCDProvider::bin2bcd(bins[0]);
        if (actual != bcds[0]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bin2bcd(bins[1]);
        if (actual != bcds[1]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bin2bcd(bins[2]);
        if (actual != bcds[2]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bin2bcd(bins[3]);
        if (actual != bcds[3]) {
            return false;
        }
    }

    {
        T actual = BCDProvider::bin2bcd(bins[4], false);
        if (actual != bcds[4]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bin2bcd(bins[5], false);
        if (actual != bcds[5]) {
            return false;
        }
    }
    {
        T actual = BCDProvider::bin2bcd(bins[6], false);
        if (actual != bcds[6]) {
            return false;
        }
    }
    {
        if (typeid(T) == typeid(uint8_t)) {
            T actual = BCDProvider::bin2bcd(bins[7]);
            if (actual != bcds[7]) {
                return false;
            }
        } else {
            T actual = BCDProvider::bin2bcd(bins[7], false);
            if (actual != bcds[7]) {
                return false;
            }
        }
    }

    return true;
}

bool test_bin2bcd() {
    {
        uint8_t bins[] = {19, 0, 99, 32, 19, 0, 99, 32};
        uint8_t bcds[] = {0x19, 0x00, 0x99, 0x32, 0x19, 0x00, 0x99, 0x32};
        if (!test_bin2bcd_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint16_t bins[] = {1965, 0, 9999, 4718, 1965, 0, 9999, 4718};
        uint16_t bcds[] = {0x1965, 0x0000, 0x9999, 0x4718, 0x6519, 0x0000, 0x9999, 0x1847};
        if (!test_bin2bcd_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint32_t bins[] = {19651331, 0, 99999999, 32323232, 19651331, 0, 99999999, 32323232};
        uint32_t bcds[] = {0x19651331, 0x00000000, 0x99999999, 0x32323232,
                           0x31136519, 0x00000000, 0x99999999, 0x32323232};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint64_t bins[] = {1965133124678765, 0x0000000000000000, 9999999999999999, 3232323232323232,
                           1965133124678765, 0x0000000000000000, 9999999999999999, 3232323232323232};
        uint64_t bcds[] = {0x1965133124678765, 0x0000000000000000, 0x9999999999999999, 0x3232323232323232,
                           0x6587672431136519, 0x0000000000000000, 0x9999999999999999, 0x3232323232323232};
        if (!test_bcd2bin_value(bins, bcds)) {
            return false;
        }
    }

    {
        uint8_t buffer[2] = {0x07, 0xAD};
        uint64_t actual = BCDProvider::bin2bcd(buffer, 0, sizeof(buffer));
        if (actual != 0x1965) {
            return false;
        }
    }
    {
        uint8_t buffer[2] = {0xAD, 0x07};
        uint64_t actual = BCDProvider::bin2bcd(buffer, 0, sizeof(buffer), false);
        if (actual != 0x1965) {
            return false;
        }
    }

    {
        ByteArray actual;
        ByteArray expert = {0x19, 0x65};
        BCDProvider::bin2buffer((uint16_t) 1965, actual);
        if (actual != expert) {
            return false;
        }
    }
    {
        ByteArray actual;
        ByteArray expert = {0x65, 0x19};
        BCDProvider::bin2buffer((uint16_t) 1965, actual, false);
        if (actual != expert) {
            return false;
        }
    }

    return true;
}

bool test_change() {
    {
        uint8_t buffer[9] = {};
        off_t offset = 0;
        int16_t expect = 2018;
        BCDProvider::bin2buffer(expect, buffer + offset);
        int16_t actual = (int16_t) BCDProvider::bcd2bin(buffer, offset, 2);
        if (actual != expect) {
            return false;
        }
    }

    {
        uint8_t buffer[9] = {};
        off_t offset = 0;
        int16_t expect = 646;
        BCDProvider::bin2buffer(expect, buffer + offset);
        int16_t actual = (int16_t) BCDProvider::bcd2bin(buffer, offset, 2);
        if (actual != expect) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!test_bcd2bin()) {
        return 1;
    }
    if (!test_bin2bcd()) {
        return 2;
    }
    if (!test_change()) {
        return 3;
    }

    return 0;
}