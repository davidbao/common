//
//  CheckProvider.cpp
//  common
//
//  Created by baowei on 2021/1/13.
//  Copyright (c) 2021 com. All rights reserved.
//

#include "system/CheckProvider.h"

namespace System {
    // Block Check Character
    BccProvider::BccProvider() = default;

    BccProvider::~BccProvider() = default;

    uint8_t BccProvider::calc(const uint8_t *buffer, size_t count) {
        uint8_t result = 0;
        for (size_t i = 0; i < count; i++) {
            result ^= buffer[i];
        }
        return result;
    }

    // Longitudinal Redundancy Check
    LrcProvider::LrcProvider() = default;

    LrcProvider::~LrcProvider() = default;

    uint8_t LrcProvider::calc(const uint8_t *buffer, size_t count) {
        uint8_t result = 0;
        for (size_t i = 0; i < count; i++) {
            result += buffer[i];
        }
        result = -result;
        return result;
    }

    Crc8Provider Crc8Provider::CRC8 = Crc8Provider({0x07, 0x00, false, false, 0x00});
    Crc8Provider Crc8Provider::CDMA2000 = Crc8Provider({0x9B, 0xFF, false, false, 0x00});
    Crc8Provider Crc8Provider::DARC = Crc8Provider({0x39, 0x00, true, true, 0x00});
    Crc8Provider Crc8Provider::DVB_S2 = Crc8Provider({0xD5, 0x00, false, false, 0x00});
    Crc8Provider Crc8Provider::EBU = Crc8Provider({0x1D, 0xFF, true, true, 0x00});
    Crc8Provider Crc8Provider::I_CODE = Crc8Provider({0x1D, 0xFD, false, false, 0x00});
    Crc8Provider Crc8Provider::ITU = Crc8Provider({0x07, 0x00, false, false, 0x55});
    Crc8Provider Crc8Provider::MAXIM = Crc8Provider({0x31, 0x00, true, true, 0x00});
    Crc8Provider Crc8Provider::ROHC = Crc8Provider({0x07, 0xFF, true, true, 0x00});
    Crc8Provider Crc8Provider::WCDMA = Crc8Provider({0x9B, 0x00, true, true, 0x00});

    Crc8Provider::Crc8Provider(const Crc8Config &config) : CrcProvider<Crc8Type>(config) {
    }

    Crc8Provider::~Crc8Provider() = default;

    Crc16Provider Crc16Provider::ARC = Crc16Provider({0x8005, 0x0000, true, true, 0x0000});
    Crc16Provider Crc16Provider::AUG_CCITT = Crc16Provider({0x1021, 0x1D0F, false, false, 0x0000});
    Crc16Provider Crc16Provider::BUYPASS = Crc16Provider({0x8005, 0x0000, false, false, 0x0000});
    Crc16Provider Crc16Provider::CCITT_FALSE = Crc16Provider({0x1021, 0xFFFF, false, false, 0x0000});
    Crc16Provider Crc16Provider::CDMA2000 = Crc16Provider({0xC867, 0xFFFF, false, false, 0x0000});
    Crc16Provider Crc16Provider::DDS_110 = Crc16Provider({0x8005, 0x800D, false, false, 0x0000});
    Crc16Provider Crc16Provider::DECT_R = Crc16Provider({0x0589, 0x0000, false, false, 0x0001});
    Crc16Provider Crc16Provider::DECT_X = Crc16Provider({0x0589, 0x0000, false, false, 0x0000});
    Crc16Provider Crc16Provider::DNP = Crc16Provider({0x3D65, 0x0000, true, true, 0xFFFF});
    Crc16Provider Crc16Provider::EN_13757 = Crc16Provider({0x3D65, 0x0000, false, false, 0xFFFF});
    Crc16Provider Crc16Provider::GENIBUS = Crc16Provider({0x1021, 0xFFFF, false, false, 0xFFFF});
    Crc16Provider Crc16Provider::KERMIT = Crc16Provider({0x1021, 0x0000, true, true, 0x0000});
    Crc16Provider Crc16Provider::MAXIM = Crc16Provider({0x8005, 0x0000, true, true, 0xFFFF});
    Crc16Provider Crc16Provider::MCRF4XX = Crc16Provider({0x1021, 0xFFFF, true, true, 0x0000});
    Crc16Provider Crc16Provider::MODBUS = Crc16Provider({0x8005, 0xFFFF, true, true, 0x0000});
    Crc16Provider Crc16Provider::RIELLO = Crc16Provider({0x1021, 0xB2AA, true, true, 0x0000});
    Crc16Provider Crc16Provider::T10_DIF = Crc16Provider({0x8BB7, 0x0000, false, false, 0x0000});
    Crc16Provider Crc16Provider::TELEDISK = Crc16Provider({0xA097, 0x0000, false, false, 0x0000});
    Crc16Provider Crc16Provider::TMS37157 = Crc16Provider({0x1021, 0x89EC, true, true, 0x0000});
    Crc16Provider Crc16Provider::USB = Crc16Provider({0x8005, 0xFFFF, true, true, 0xFFFF});
    Crc16Provider Crc16Provider::X_25 = Crc16Provider({0x1021, 0xFFFF, true, true, 0xFFFF});
    Crc16Provider Crc16Provider::XMODEM = Crc16Provider({0x1021, 0x0000, false, false, 0x0000});
    Crc16Provider Crc16Provider::A = Crc16Provider({0x1021, 0xC6C6, true, true, 0x0000});

    Crc16Provider::Crc16Provider(const Crc16Config &config) : CrcProvider<Crc16Type>(config) {
    }

    Crc16Provider::~Crc16Provider() = default;

    Crc32Provider Crc32Provider::CRC32 = Crc32Provider({0x04C11DB7, 0xFFFFFFFF, true, true, 0xFFFFFFFF});
    Crc32Provider Crc32Provider::BZIP2 = Crc32Provider({0x04C11DB7, 0xFFFFFFFF, false, false, 0xFFFFFFFF});
    Crc32Provider Crc32Provider::JAMCRC = Crc32Provider({0x04C11DB7, 0xFFFFFFFF, true, true, 0x00000000});
    Crc32Provider Crc32Provider::MPEG_2 = Crc32Provider({0x04C11DB7, 0xFFFFFFFF, false, false, 0x00000000});
    Crc32Provider Crc32Provider::POSIX = Crc32Provider({0x04C11DB7, 0x00000000, false, false, 0xFFFFFFFF});
    Crc32Provider Crc32Provider::SATA = Crc32Provider({0x04C11DB7, 0x52325032, false, false, 0x00000000});
    Crc32Provider Crc32Provider::XFER = Crc32Provider({0x000000AF, 0x00000000, false, false, 0x00000000});
    Crc32Provider Crc32Provider::C = Crc32Provider({0x1EDC6F41, 0xFFFFFFFF, true, true, 0xFFFFFFFF});
    Crc32Provider Crc32Provider::D = Crc32Provider({0xA833982B, 0xFFFFFFFF, true, true, 0xFFFFFFFF});
    Crc32Provider Crc32Provider::Q = Crc32Provider({0x814141AB, 0x00000000, false, false, 0x00000000});

    Crc32Provider::Crc32Provider(const Crc32Config &config) : CrcProvider<Crc32Type>(config) {
    }

    Crc32Provider::~Crc32Provider() = default;
}
