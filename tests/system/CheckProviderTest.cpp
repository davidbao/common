//
//  CheckProviderTest.cpp
//  common
//
//  Created by baowei on 2023/9/22.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/CheckProvider.h"

using namespace System;

bool testBccProvider() {
    {
        BccProvider test;
        uint8_t actual = test.calc("abc");
        if (actual != 0x60) {
            return false;
        }
    }
    {
        BccProvider test;
        uint8_t actual = test.calc("中文ABCabc123~");
        if (actual != 0x68) {
            return false;
        }
    }
    {
        BccProvider test;
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        uint8_t actual = test.calc(ByteArray(buffer, count));
        if (actual != 0x11) {
            return false;
        }
    }
    {
        BccProvider test;
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        uint8_t actual = test.calc(buffer, count);
        if (actual != 0x11) {
            return false;
        }
    }

    return true;
}

bool testLrcProvider() {
    {
        LrcProvider test;
        uint8_t actual = test.calc("abc");
        if (actual != 0xDA) {
            return false;
        }
    }
    {
        LrcProvider test;
        uint8_t actual = test.calc("中文ABCabc123~");
        if (actual != 0xB4) {
            return false;
        }
    }
    {
        LrcProvider test;
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        uint8_t actual = test.calc(ByteArray(buffer, count));
        if (actual != 0x03) {
            return false;
        }
    }
    {
        LrcProvider test;
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        uint8_t actual = test.calc(buffer, count);
        if (actual != 0x03) {
            return false;
        }
    }

    return true;
}

template<class Type>
bool testCrcProvider(CrcProvider<Type> &provider, const Type expects[4]) {
    {
        Type actual = provider.calc("abc");
        if (actual != expects[0]) {
            return false;
        }
    }
    {
        Type actual = provider.calc("ABCabc123~");
        if (actual != expects[1]) {
            return false;
        }
    }
    {
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        Type actual = provider.calc(ByteArray(buffer, count));
        if (actual != expects[2]) {
            return false;
        }
    }
    {
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        Type actual = provider.calc(buffer, count);
        if (actual != expects[3]) {
            return false;
        }
    }

    {
        Type actual = provider.calcDirectly("abc");
        if (actual != expects[0]) {
            return false;
        }
    }
    {
        Type actual = provider.calcDirectly("ABCabc123~");
        if (actual != expects[1]) {
            return false;
        }
    }
    {
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        Type actual = provider.calcDirectly(ByteArray(buffer, count));
        if (actual != expects[2]) {
            return false;
        }
    }
    {
        static const int count = 9;
        uint8_t buffer[count] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
        Type actual = provider.calcDirectly(buffer, count);
        if (actual != expects[3]) {
            return false;
        }
    }

    return true;
}

bool testCrc8Provider() {
    {
        Crc8Provider test;  // default: CRC8
        Crc8Type expects[] = {0x5F, 0x32, 0xED, 0xED};
        if (!testCrcProvider<uint8_t>(test, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x5F, 0x32, 0xED, 0xED};
        if (!testCrcProvider<uint8_t>(Crc8Provider::CRC8, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x33, 0x86, 0xAA, 0xAA};
        if (!testCrcProvider<uint8_t>(Crc8Provider::CDMA2000, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x0D, 0x00, 0x7F, 0x7F};
        if (!testCrcProvider<uint8_t>(Crc8Provider::DARC, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x5A, 0xA1, 0x6F, 0x6F};
        if (!testCrcProvider<uint8_t>(Crc8Provider::DVB_S2, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0xF7, 0xE4, 0x55, 0x55};
        if (!testCrcProvider<uint8_t>(Crc8Provider::EBU, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x66, 0x6C, 0x91, 0x91};
        if (!testCrcProvider<uint8_t>(Crc8Provider::I_CODE, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x0A, 0x67, 0xB8, 0xB8};
        if (!testCrcProvider<uint8_t>(Crc8Provider::ITU, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x42, 0xCE, 0x55, 0x55};
        if (!testCrcProvider<uint8_t>(Crc8Provider::MAXIM, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x24, 0xE3, 0x5C, 0x5C};
        if (!testCrcProvider<uint8_t>(Crc8Provider::ROHC, expects)) {
            return false;
        }
    }
    {
        Crc8Type expects[] = {0x2B, 0xF1, 0xA9, 0xA9};
        if (!testCrcProvider<uint8_t>(Crc8Provider::WCDMA, expects)) {
            return false;
        }
    }

    return true;
}

bool testCrc16Provider() {
    {
        Crc16Provider test;  // default: MODBUS
        Crc16Type expects[] = {0x5749, 0xFFC8, 0xD241, 0xD241};
        if (!testCrcProvider<uint16_t>(test, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x9738, 0xF8B8, 0x224B, 0x224B};
        if (!testCrcProvider<uint16_t>(Crc16Provider::ARC, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x8CDA, 0x2897, 0xE0B5, 0xE0B5};
        if (!testCrcProvider<uint16_t>(Crc16Provider::AUG_CCITT, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xCADB, 0x3A08, 0xC52B, 0xC52B};
        if (!testCrcProvider<uint16_t>(Crc16Provider::BUYPASS, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x514A, 0x4D57, 0x2CC8, 0x2CC8};
        if (!testCrcProvider<uint16_t>(Crc16Provider::CCITT_FALSE, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xF9C7, 0x38B3, 0x1FBB, 0x1FBB};
        if (!testCrcProvider<uint16_t>(Crc16Provider::CDMA2000, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xEEDB, 0x1C48, 0xA50C, 0xA50C};
        if (!testCrcProvider<uint16_t>(Crc16Provider::DDS_110, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x1A21, 0x3C3F, 0x2F09, 0x2F09};
        if (!testCrcProvider<uint16_t>(Crc16Provider::DECT_R, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x1A20, 0x3C3E, 0x2F08, 0x2F08};
        if (!testCrcProvider<uint16_t>(Crc16Provider::DECT_X, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xE957, 0x84C8, 0x0D85, 0x0D85};
        if (!testCrcProvider<uint16_t>(Crc16Provider::DNP, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x571C, 0x4D1A, 0xB11B, 0xB11B};
        if (!testCrcProvider<uint16_t>(Crc16Provider::EN_13757, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xAEB5, 0xB2A8, 0xD337, 0xD337};
        if (!testCrcProvider<uint16_t>(Crc16Provider::GENIBUS, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x58E9, 0x8240, 0xA57E, 0xA57E};
        if (!testCrcProvider<uint16_t>(Crc16Provider::KERMIT, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x68C7, 0x0747, 0xDDB4, 0xDDB4};
        if (!testCrcProvider<uint16_t>(Crc16Provider::MAXIM, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x61DA, 0x1EC7, 0xEB66, 0xEB66};
        if (!testCrcProvider<uint16_t>(Crc16Provider::MCRF4XX, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x5749, 0xFFC8, 0xD241, 0xD241};
        if (!testCrcProvider<uint16_t>(Crc16Provider::MODBUS, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x0CAF, 0x4D46, 0xE727, 0xE727};
        if (!testCrcProvider<uint16_t>(Crc16Provider::RIELLO, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x443B, 0x7B10, 0xA94C, 0xA94C};
        if (!testCrcProvider<uint16_t>(Crc16Provider::T10_DIF, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x8089, 0xB1E6, 0xC5D4, 0xC5D4};
        if (!testCrcProvider<uint16_t>(Crc16Provider::TELEDISK, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x70E6, 0x3F8C, 0xA246, 0xA246};
        if (!testCrcProvider<uint16_t>(Crc16Provider::TMS37157, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xA8B6, 0x0037, 0x2DBE, 0x2DBE};
        if (!testCrcProvider<uint16_t>(Crc16Provider::USB, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x9E25, 0xE138, 0x1499, 0x1499};
        if (!testCrcProvider<uint16_t>(Crc16Provider::X_25, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0x9DD6, 0xAC6E, 0x34BA, 0x34BA};
        if (!testCrcProvider<uint16_t>(Crc16Provider::XMODEM, expects)) {
            return false;
        }
    }
    {
        Crc16Type expects[] = {0xFDFD, 0xCCBA, 0x3BF2, 0x3BF2};
        if (!testCrcProvider<uint16_t>(Crc16Provider::A, expects)) {
            return false;
        }
    }

    return true;
}

bool testCrc32Provider() {
    {
        Crc32Provider test;  // default: CRC32
        Crc32Type expects[] = {0x352441C2, 0x5F4599CB, 0x2E2A7F10, 0x2E2A7F10};
        if (!testCrcProvider<uint32_t>(test, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x352441C2, 0x5F4599CB, 0x2E2A7F10, 0x2E2A7F10};
        if (!testCrcProvider<uint32_t>(Crc32Provider::CRC32, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x648CBB73, 0xB8124FC9, 0x798F6451, 0x798F6451};
        if (!testCrcProvider<uint32_t>(Crc32Provider::BZIP2, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0xCADBBE3D, 0xA0BA6634, 0xD1D580EF, 0xD1D580EF};
        if (!testCrcProvider<uint32_t>(Crc32Provider::JAMCRC, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x9B73448C, 0x47EDB036, 0x86709BAE, 0x86709BAE};
        if (!testCrcProvider<uint32_t>(Crc32Provider::MPEG_2, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0xD3E8C673, 0x29FBE1F1, 0xF3580BC9, 0xF3580BC9};
        if (!testCrcProvider<uint32_t>(Crc32Provider::POSIX, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x9E90110F, 0x2B22B4CB, 0x4A74D2A1, 0x4A74D2A1};
        if (!testCrcProvider<uint32_t>(Crc32Provider::SATA, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x3EB041D1, 0xA8737470, 0xEE697A44, 0xEE697A44};
        if (!testCrcProvider<uint32_t>(Crc32Provider::XFER, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x364B3FB7, 0x670E31E1, 0x76032DF7, 0x76032DF7};
        if (!testCrcProvider<uint32_t>(Crc32Provider::C, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x5832F57E, 0xA8801E52, 0xF8C17DBB, 0xF8C17DBB};
        if (!testCrcProvider<uint32_t>(Crc32Provider::D, expects)) {
            return false;
        }
    }
    {
        Crc32Type expects[] = {0x9590B5B0, 0xC08816A3, 0xF5C08080, 0xF5C08080};
        if (!testCrcProvider<uint32_t>(Crc32Provider::Q, expects)) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testBccProvider()) {
        return 1;
    }
    if (!testLrcProvider()) {
        return 2;
    }
    if (!testCrc8Provider()) {
        return 3;
    }
    if (!testCrc16Provider()) {
        return 4;
    }
    if (!testCrc32Provider()) {
        return 5;
    }

    return 0;
}