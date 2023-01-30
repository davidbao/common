//
//  SerialInfo.h
//  common
//
//  Created by baowei on 2015/7/31.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef SerialInfo_h
#define SerialInfo_h

#include "data/String.h"
#include "data/IAttribute.h"
#include "json/JsonNode.h"

using namespace Data;
using namespace Json;

namespace IO {
    struct SerialInfo : public IEquatable<SerialInfo>, public IEvaluation<SerialInfo> {
    public:
        enum DataBitsType : uint8_t {
            DATA_5 = 5,
            DATA_6,
            DATA_7,
            DATA_8
        };

        enum ParityType : uint8_t {
            PAR_NONE = 0,
            PAR_ODD,
            PAR_EVEN,
            PAR_MARK,               //WINDOWS ONLY
            PAR_SPACE
        };

        enum StopBitsType : uint8_t {
            STOP_1 = 1,
            STOP_1_5,               //WINDOWS ONLY
            STOP_2
        };

        enum HandshakeType : uint8_t {
            FLOW_OFF = 0,
            FLOW_HARDWARE,
            FLOW_XONXOFF
        };

        String portName;
        int baudRate;
        DataBitsType dataBits;
        StopBitsType stopBits;
        HandshakeType handshake;
        ParityType parity;
        bool rtsEnable;
        bool dtrEnable;
        bool useSignal;

        explicit SerialInfo(const String &portName = String::Empty);

        SerialInfo(const String &portName, int baudRate, DataBitsType dataBits, StopBitsType stopBits, ParityType parity);

        SerialInfo(const SerialInfo &other);

        bool equals(const SerialInfo &other) const override;

        void evaluates(const SerialInfo &other) override;

        void read(IAttributeGetter &getter);

        void write(IAttributeSetter &setter) const;

        void read(Stream *stream);

        void write(Stream *stream) const;

        String dataBitsStr() const;

        String parityStr() const;

        String stopBitsStr() const;

        String handshakeStr() const;

        SerialInfo &operator=(const SerialInfo &value);

        String toString() const;

        bool isEmpty() const;

    public:
        static DataBitsType parseDataBits(const String &str);

        static String convertDataBitsStr(DataBitsType dataBits);

        static ParityType parseParity(const String &str);

        static String convertParityStr(ParityType parity);

        static StopBitsType parseStopBits(const String &str);

        static String convertStopBitsStr(StopBitsType stopBits);

        static HandshakeType parseHandshake(const String &str);

        static String convertHandshakeStr(HandshakeType handshake);
    };
}

#endif // SerialInfo_h
