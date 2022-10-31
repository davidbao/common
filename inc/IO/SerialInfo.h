//
//  SerialInfo.h
//  common
//
//  Created by baowei on 15/7/31.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef __common__SerialInfo__
#define __common__SerialInfo__

#include "data/ValueType.h"
#include "xml/XmlTextReader.h"
#include "xml/XmlTextWriter.h"
#include "json/JsonTextWriter.h"
#include "json/JsonTextReader.h"

namespace Common
{
    struct SerialInfo
    {
    public:
        enum DataBitsType : uint8_t
        {
            DATA_5 = 5,
            DATA_6,
            DATA_7,
            DATA_8
        };
        
        enum ParityType : uint8_t
        {
            PAR_NONE = 0,
            PAR_ODD,
            PAR_EVEN,
            PAR_MARK,               //WINDOWS ONLY
            PAR_SPACE
        };
        
        enum StopBitsType : uint8_t
        {
            STOP_1 = 1,
            STOP_1_5,               //WINDOWS ONLY
            STOP_2
        };
        
        enum HandshakeType : uint8_t
        {
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
        
        SerialInfo(const String& portName = String::Empty);
        
        void read(XmlTextReader& reader);
        void write(XmlTextWriter& writer) const;
        
        void read(JsonTextReader& reader);
        void write(JsonTextWriter& writer) const;
        
        void write(JsonNode& node) const;
        
        void read(Stream* stream);
        void write(Stream* stream) const;

		String dataBitsStr() const;
		String parityStr() const;
		String stopBitsStr() const;
		String handshakeStr() const;
        
        void operator=(const SerialInfo& value);
        bool operator==(const SerialInfo& value) const;
        bool operator!=(const SerialInfo& value) const;
        
        const String toString() const;
        
        bool isEmpty() const;
        
        static DataBitsType parseDataBits(const String& str);
        static String convertDataBitsStr(DataBitsType dataBits);
        
        static ParityType parseParity(const String& str);
        static String convertParityStr(ParityType parity);
        
        static StopBitsType parseStopBits(const String& str);
        static String convertStopBitsStr(StopBitsType stopBits);
        
        static HandshakeType parseHandshake(const String& str);
        static String convertHandshakeStr(HandshakeType handshake);
    };
}

#endif /* defined(__common__SerialInfo__) */
