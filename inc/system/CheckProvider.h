//
//  CheckProvider.h
//  common
//
//  Created by baowei on 2021/1/13.
//  Copyright (c) 2021 com. All rights reserved.
//

#ifndef CheckProvider_h
#define CheckProvider_h

#include "data/Convert.h"
#include "data/ByteArray.h"
#include <cassert>

using namespace Data;

#ifdef WIN32
#pragma warning(disable: 4333)
#endif

namespace System {
    template<class Type>
    class CheckProvider {
    public:
        virtual ~CheckProvider() = default;

        virtual Type calc(const uint8_t *buffer, size_t count) = 0;

        Type calc(const uint8_t *buffer, off_t offset, size_t count) {
            return calc(buffer + offset, count);
        }

        Type calc(const ByteArray &buffer) {
            return calc(buffer.data(), 0, buffer.count());
        }

        Type calc(const String &buffer) {
            return calc((const uint8_t *) buffer.c_str(), 0, buffer.length());
        }
    };

    // Block Check Character
    class BccProvider : CheckProvider<uint8_t> {
    public:
        using CheckProvider<uint8_t>::calc;

        BccProvider();

        ~BccProvider() override;

        uint8_t calc(const uint8_t *buffer, size_t count) override;
    };

    // Longitudinal Redundancy Check
    class LrcProvider : CheckProvider<uint8_t> {
    public:
        using CheckProvider<uint8_t>::calc;

        LrcProvider();

        ~LrcProvider() override;

        uint8_t calc(const uint8_t *buffer, size_t count) override;
    };

    template<class Type>
    struct CrcConfig {
        Type poly;
        Type init;
        bool refIn;
        bool refOut;
        Type xorOut;
    };

    typedef uint8_t Crc8Type;
    typedef uint16_t Crc16Type;
    typedef uint32_t Crc32Type;

    typedef CrcConfig<Crc8Type> Crc8Config;
    typedef CrcConfig<Crc16Type> Crc16Config;
    typedef CrcConfig<Crc32Type> Crc32Config;

    // Cyclic Redundancy Check
    template<class Type>
    class CrcProvider : public CheckProvider<Type> {
    public:
        using CheckProvider<Type>::calc;

        explicit CrcProvider(const CrcConfig<Type> &config) : _config(config), _width(sizeof(Type) * 8) {
            init();
        }

        ~CrcProvider() override {
        }

        Type calc(const uint8_t *buffer, size_t count) override {
            Type pos;
            Type crc;
            if (_config.refIn) {
                assert(_config.refOut);
                crc = reverseValue(_config.init, _width);
                while (count--) {
                    pos = (0xFF & crc) ^ (*buffer++);
                    crc = _width == 8 ? _table[pos & 0xFF] : (crc >> 8) ^ _table[pos & 0xFF];
                }
            } else {
                crc = _config.init;
                while (count--) {
                    pos = (crc >> (_width - 8)) ^ (*buffer++);
                    crc = _width == 8 ? _table[pos & 0xFF] : (crc << 8) ^ _table[pos & 0xFF];
                }
            }
            return crc ^ _config.xorOut;
        }

        // calcDirectly from buffer.
        Type calcDirectly(const uint8_t *buffer, size_t count) {
            Type crc;
            if (_config.refIn) {
                assert(_config.refOut);
                crc = reverseValue(_config.init, _width);
                while (count--) {
                    crc ^= *buffer++;
                    auto reverse = reverseValue(_config.poly, _width);
                    for (uint8_t i = 0; i < 8; ++i) {
                        if (crc & 1)
                            crc = (crc >> 1) ^ reverse;
                        else
                            crc = (crc >> 1);
                    }
                }
            } else {
                crc = _config.init;
                Type bits = 1 << (_width - 1);
                while (count--) {
                    crc ^= (Type) ((*buffer++) << (_width - 8));
                    for (uint8_t i = 0; i < 8; ++i) {
                        if (crc & bits)
                            crc = (crc << 1) ^ _config.poly;
                        else
                            crc <<= 1;
                    }
                }
            }
            return crc ^ _config.xorOut;
        }

        // calcDirectly from buffer.
        Type calcDirectly(const uint8_t *buffer, off_t offset, size_t count) {
            return calcDirectly(buffer + offset, count);
        }

        // calcDirectly from buffer.
        Type calcDirectly(const ByteArray &buffer) {
            return calcDirectly(buffer.data(), 0, buffer.count());
        }

        // calcDirectly from buffer.
        Type calcDirectly(const String &buffer) {
            return calcDirectly((const uint8_t *) buffer.c_str(), 0, buffer.length());
        }

    protected:
        void init() {
            Type crc;
            if (_config.refIn) {
                assert(_config.refOut);
                Type bits = 1;
                auto reversePoly = reverseValue(_config.poly, _width);
                for (int i = 0; i < Count; ++i) {
                    crc = (Type) (i);
                    for (int j = 0; j < 8; ++j) {
                        if (crc & bits)
                            crc = (crc >> 1) ^ reversePoly;
                        else
                            crc = (crc >> 1);
                    }
                    _table[i] = crc;
                }
            } else {
                Type bits = 1 << (_width - 1);
                for (int i = 0; i < Count; ++i) {
                    crc = (Type) (i << (_width - 8));
                    for (int j = 0; j < 8; ++j) {
                        if (crc & bits)
                            crc = (crc << 1) ^ _config.poly;
                        else
                            crc = (crc << 1);
                    }
                    _table[i] = crc;
                }
            }
        }

        const CrcConfig<Type> &config() const {
            return _config;
        }

    private:
        Type reverseValue(Type val, int width) {
            Type rv = 0;
            for (int bit = 0; bit < width; bit++) {
                int digit = (val >> bit) & 1;       // extract a digit
                rv |= digit << (width - 1 - bit);   // stick it in the result
            }
            return rv;
        };

    protected:
        CrcConfig<Type> _config;

        Type _width;

        static const int Count = 256;
        Type _table[Count];
    };

    class Crc8Provider : public CrcProvider<Crc8Type> {
    public:
        explicit Crc8Provider(const Crc8Config &config = Crc8Provider::CRC8.config());

        ~Crc8Provider() override;

    public:
        static Crc8Provider CRC8;
        static Crc8Provider CDMA2000;
        static Crc8Provider DARC;
        static Crc8Provider DVB_S2;
        static Crc8Provider EBU;
        static Crc8Provider I_CODE;
        static Crc8Provider ITU;
        static Crc8Provider MAXIM;
        static Crc8Provider ROHC;
        static Crc8Provider WCDMA;
    };

    class Crc16Provider : public CrcProvider<Crc16Type> {
    public:
        explicit Crc16Provider(const Crc16Config &config = Crc16Provider::MODBUS.config());

        ~Crc16Provider() override;

    public:
        static Crc16Provider ARC;
        static Crc16Provider AUG_CCITT;
        static Crc16Provider BUYPASS;
        static Crc16Provider CCITT_FALSE;
        static Crc16Provider CDMA2000;
        static Crc16Provider DDS_110;
        static Crc16Provider DECT_R;
        static Crc16Provider DECT_X;
        static Crc16Provider DNP;
        static Crc16Provider EN_13757;
        static Crc16Provider GENIBUS;
        static Crc16Provider KERMIT;
        static Crc16Provider MAXIM;
        static Crc16Provider MCRF4XX;
        static Crc16Provider MODBUS;
        static Crc16Provider RIELLO;
        static Crc16Provider T10_DIF;
        static Crc16Provider TELEDISK;
        static Crc16Provider TMS37157;
        static Crc16Provider USB;
        static Crc16Provider X_25;
        static Crc16Provider XMODEM;
        static Crc16Provider A;
    };

    class Crc32Provider : public CrcProvider<Crc32Type> {
    public:
        explicit Crc32Provider(const Crc32Config &config = Crc32Provider::CRC32.config());

        ~Crc32Provider() override;

    public:
        static Crc32Provider CRC32;
        static Crc32Provider BZIP2;
        static Crc32Provider JAMCRC;
        static Crc32Provider MPEG_2;
        static Crc32Provider POSIX;
        static Crc32Provider SATA;
        static Crc32Provider XFER;
        static Crc32Provider C;
        static Crc32Provider D;
        static Crc32Provider Q;
    };
}

#endif // CheckProvider_h
