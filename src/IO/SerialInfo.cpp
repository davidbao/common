//
//  SerialInfo.cpp
//  common
//
//  Created by baowei on 15/7/31.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "IO/SerialInfo.h"
#include "system/Convert.h"
#include "data/ValueType.h"

namespace Common
{
    SerialInfo::SerialInfo(const String& portName)
    {
        this->portName = portName;
        this->baudRate = 9600;
        this->dataBits = DATA_8;
        this->stopBits = STOP_1;
        this->parity = PAR_NONE;
        this->handshake = FLOW_OFF;
        this->rtsEnable = false;
        this->dtrEnable = false;
        this->useSignal = false;
    }
    void SerialInfo::read(XmlTextReader& reader)
    {
        portName = reader.getAttribute("portname");
        Int32::parse(reader.getAttribute("baudrate"), baudRate);
        dataBits = parseDataBits(reader.getAttribute("databits"));
        stopBits = parseStopBits(reader.getAttribute("stopbits"));
        parity = parseParity(reader.getAttribute("parity"));
        handshake = parseHandshake(reader.getAttribute("handshake"));
        Boolean::parse(reader.getAttribute("rtsenable"), rtsEnable);
        Boolean::parse(reader.getAttribute("dtrenable"), dtrEnable);
        Boolean::parse(reader.getAttribute("usesignal"), useSignal);
    }
    void SerialInfo::write(XmlTextWriter& writer) const
    {
        writer.writeAttributeString("portname", portName);
        writer.writeAttribute("baudrate", baudRate);
        writer.writeAttributeString("databits", convertDataBitsStr(dataBits));
        writer.writeAttributeString("stopbits", convertStopBitsStr(stopBits));
        writer.writeAttributeString("parity", convertParityStr(parity));
        if(handshake != HandshakeType::FLOW_OFF)
            writer.writeAttributeString("handshake", convertHandshakeStr(handshake));
        if(rtsEnable)
            writer.writeAttribute("rtsenable", rtsEnable);
        if(dtrEnable)
            writer.writeAttribute("dtrenable", dtrEnable);
        if(useSignal)
            writer.writeAttribute("usesignal", useSignal);
    }
    void SerialInfo::read(JsonTextReader& reader)
    {
        portName = reader.getAttribute("portname");
        Int32::parse(reader.getAttribute("baudrate"), baudRate);
        dataBits = parseDataBits(reader.getAttribute("databits"));
        stopBits = parseStopBits(reader.getAttribute("stopbits"));
        parity = parseParity(reader.getAttribute("parity"));
        handshake = parseHandshake(reader.getAttribute("handshake"));
        Boolean::parse(reader.getAttribute("rtsenable"), rtsEnable);
        Boolean::parse(reader.getAttribute("dtrenable"), dtrEnable);
        Boolean::parse(reader.getAttribute("usesignal"), useSignal);
    }
    void SerialInfo::write(JsonTextWriter& writer) const
    {
        writer.writeAttribute("portname", portName);
        writer.writeAttribute("baudrate", baudRate);
        writer.writeAttribute("databits", convertDataBitsStr(dataBits));
        writer.writeAttribute("stopbits", convertStopBitsStr(stopBits));
        writer.writeAttribute("parity", convertParityStr(parity));
        if(handshake != HandshakeType::FLOW_OFF)
            writer.writeAttributeString("handshake", convertHandshakeStr(handshake));
        if(rtsEnable)
            writer.writeAttribute("rtsenable", rtsEnable);
        if(dtrEnable)
            writer.writeAttribute("dtrenable", dtrEnable);
        if(useSignal)
            writer.writeAttribute("usesignal", useSignal);
    }
    void SerialInfo::write(JsonNode& node) const
    {
        node.add(JsonNode("portname", portName));
        node.add(JsonNode("baudrate", baudRate));
        node.add(JsonNode("databits", convertDataBitsStr(dataBits)));
        node.add(JsonNode("stopbits", convertStopBitsStr(stopBits)));
        node.add(JsonNode("parity", convertParityStr(parity)));
        if(handshake != HandshakeType::FLOW_OFF)
            node.add(JsonNode("handshake", convertHandshakeStr(handshake)));
        if(rtsEnable)
            node.add(JsonNode("rtsenable", rtsEnable));
        if(dtrEnable)
            node.add(JsonNode("dtrenable", dtrEnable));
        if(useSignal)
            node.add(JsonNode("usesignal", useSignal));
    }
    
    void SerialInfo::read(Stream* stream)
    {
        portName = stream->readStr();
        baudRate = stream->readInt32();
        dataBits = (DataBitsType)stream->readByte();
        stopBits = (StopBitsType)stream->readByte();
        parity = (ParityType)stream->readByte();
        handshake = (HandshakeType)stream->readByte();
        rtsEnable = stream->readBoolean();
        dtrEnable = stream->readBoolean();
        useSignal = stream->readBoolean();
    }
    void SerialInfo::write(Stream* stream) const
    {
        stream->writeStr(portName);
        stream->writeInt32(baudRate);
        stream->writeByte(dataBits);
        stream->writeByte(stopBits);
        stream->writeByte(parity);
        stream->writeByte(handshake);
        stream->writeBoolean(rtsEnable);
        stream->writeBoolean(dtrEnable);
        stream->writeBoolean(useSignal);
    }
    
	String SerialInfo::dataBitsStr() const
	{
		return convertDataBitsStr(dataBits);
	}
	String SerialInfo::parityStr() const
	{
		return convertParityStr(parity);
	}
	String SerialInfo::stopBitsStr() const
	{
		return convertStopBitsStr(stopBits);
	}
	String SerialInfo::handshakeStr() const
	{
		return convertHandshakeStr(handshake);
	}
    
    void SerialInfo::operator=(const SerialInfo& value)
    {
        this->portName = value.portName;
        this->baudRate = value.baudRate;
        this->dataBits = value.dataBits;
        this->stopBits = value.stopBits;
        this->handshake = value.handshake;
        this->parity = value.parity;
        this->rtsEnable = value.rtsEnable;
        this->dtrEnable = value.dtrEnable;
        this->useSignal = value.useSignal;
    }
    bool SerialInfo::operator==(const SerialInfo& value) const
    {
        return this->portName == value.portName &&
            this->baudRate == value.baudRate &&
            this->dataBits == value.dataBits &&
            this->stopBits == value.stopBits &&
            this->handshake == value.handshake &&
            this->parity == value.parity &&
            this->rtsEnable == value.rtsEnable &&
            this->dtrEnable == value.dtrEnable &&
            this->useSignal == value.useSignal;
    }
    bool SerialInfo::operator!=(const SerialInfo& value) const
    {
        return !operator==(value);
    }
    
    const String SerialInfo::toString() const
    {
        return String::convert("port: %s, baudrate: %d, databits: %s, stopbits: %s, parity: %s",
                               portName.c_str(),
                               baudRate,
                               convertDataBitsStr(dataBits).c_str(),
                               convertStopBitsStr(stopBits).c_str(),
                               convertParityStr(parity).c_str());
    }
    
    bool SerialInfo::isEmpty() const
    {
        return this->portName.isNullOrEmpty();
    }
    
    SerialInfo::DataBitsType SerialInfo::parseDataBits(const String& str)
    {
        if (str == "8")
            return DATA_8;
        else if (str == "7")
            return DATA_7;
        else if (str == "6")
            return DATA_6;
        else if (str == "5")
            return DATA_5;
        else
        {
            return DATA_8;
        }
    }
    String SerialInfo::convertDataBitsStr(DataBitsType dataBits)
    {
        switch (dataBits)
        {
            case SerialInfo::DATA_5:
                return "5";
            case SerialInfo::DATA_6:
                return "6";
            case SerialInfo::DATA_7:
                return "7";
            case SerialInfo::DATA_8:
            default:
                return "8";
        }
    }
    
    SerialInfo::ParityType SerialInfo::parseParity(const String& str)
    {
        if (str == "N" || str == "n" || str.toLower() == "none")
            return PAR_NONE;
        else if (str == "O" || str == "o" || str.toLower() == "odd")
            return PAR_ODD;
        else if (str == "E" || str == "e" || str.toLower() == "even")
            return PAR_EVEN;
        else if (str == "M" || str == "m" || str.toLower() == "mark")
            return PAR_MARK;
        else if (str == "S" || str == "s" || str.toLower() == "space")
            return PAR_SPACE;
        else
        {
            return PAR_NONE;
        }
    }
    String SerialInfo::convertParityStr(ParityType parity)
    {
        switch (parity)
        {
            case SerialInfo::PAR_NONE:
                return "N";
            case SerialInfo::PAR_ODD:
                return "O";
            case SerialInfo::PAR_EVEN:
                return "E";
            case SerialInfo::PAR_MARK:
                return "M";
            case SerialInfo::PAR_SPACE:
                return "S";
            default:
                return "N";
        }
    }
    
    SerialInfo::StopBitsType SerialInfo::parseStopBits(const String& str)
    {
        if (str == "1")
            return STOP_1;
        else if (str == "1.5")
            return STOP_1_5;
        else if (str == "2")
            return STOP_2;
        else
        {
            return STOP_1;
        }
    }
    String SerialInfo::convertStopBitsStr(StopBitsType stopBits)
    {
        switch (stopBits)
        {
            case SerialInfo::STOP_1:
                return "1";
            case SerialInfo::STOP_1_5:
                return "1.5";
            case SerialInfo::STOP_2:
                return "2";
            default:
                return "1";
        }
    }
    
    SerialInfo::HandshakeType SerialInfo::parseHandshake(const String& str)
    {
        if (String::equals("OFF", str, true))
            return FLOW_OFF;
        else if (String::equals("HARDWARE", str, true))
            return FLOW_HARDWARE;
        else if (String::equals("XONXOFF", str, true))
            return FLOW_XONXOFF;
        else
        {
            return FLOW_OFF;
        }
    }
    String SerialInfo::convertHandshakeStr(HandshakeType handshake)
    {
        switch (handshake)
        {
            case SerialInfo::FLOW_OFF:
                return "OFF";
            case SerialInfo::FLOW_HARDWARE:
                return "HARDWARE";
            case SerialInfo::FLOW_XONXOFF:
                return "XONXOFF";
            default:
                return "OFF";
        }
    }
}
