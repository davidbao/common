//
//  SerialInfo.cpp
//  common
//
//  Created by baowei on 2015/7/31.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "IO/SerialInfo.h"

namespace IO {
    SerialInfo::SerialInfo(const String &portName) {
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

    SerialInfo::SerialInfo(const String &portName, int baudRate, DataBitsType dataBits, StopBitsType stopBits,
                           ParityType parity) {
        this->portName = portName;
        this->baudRate = baudRate;
        this->dataBits = dataBits;
        this->stopBits = stopBits;
        this->parity = parity;
        this->handshake = FLOW_OFF;
        this->rtsEnable = false;
        this->dtrEnable = false;
        this->useSignal = false;
    }

    SerialInfo::SerialInfo(const SerialInfo &other) : SerialInfo(String::Empty) {
        SerialInfo::evaluates(other);
    }

    bool SerialInfo::equals(const SerialInfo &other) const {
        return this->portName == other.portName &&
               this->baudRate == other.baudRate &&
               this->dataBits == other.dataBits &&
               this->stopBits == other.stopBits &&
               this->handshake == other.handshake &&
               this->parity == other.parity &&
               this->rtsEnable == other.rtsEnable &&
               this->dtrEnable == other.dtrEnable &&
               this->useSignal == other.useSignal;
    }

    void SerialInfo::evaluates(const SerialInfo &other) {
        this->portName = other.portName;
        this->baudRate = other.baudRate;
        this->dataBits = other.dataBits;
        this->stopBits = other.stopBits;
        this->handshake = other.handshake;
        this->parity = other.parity;
        this->rtsEnable = other.rtsEnable;
        this->dtrEnable = other.dtrEnable;
        this->useSignal = other.useSignal;
    }

    void SerialInfo::read(IAttributeGetter &getter) {
        portName = getter.getAttribute("portname");
        Int32::parse(getter.getAttribute("baudrate"), baudRate);
        dataBits = parseDataBits(getter.getAttribute("databits"));
        stopBits = parseStopBits(getter.getAttribute("stopbits"));
        parity = parseParity(getter.getAttribute("parity"));
        handshake = parseHandshake(getter.getAttribute("handshake"));
        Boolean::parse(getter.getAttribute("rtsenable"), rtsEnable);
        Boolean::parse(getter.getAttribute("dtrenable"), dtrEnable);
        Boolean::parse(getter.getAttribute("usesignal"), useSignal);
    }

    void SerialInfo::write(IAttributeSetter &setter) const {
        setter.writeAttribute("portname", portName);
        setter.writeAttribute("baudrate", baudRate);
        setter.writeAttribute("databits", convertDataBitsStr(dataBits));
        setter.writeAttribute("stopbits", convertStopBitsStr(stopBits));
        setter.writeAttribute("parity", convertParityStr(parity));
        setter.writeAttribute("handshake", convertHandshakeStr(handshake));
        setter.writeAttribute("rtsenable", rtsEnable);
        setter.writeAttribute("dtrenable", dtrEnable);
        setter.writeAttribute("usesignal", useSignal);
    }

    void SerialInfo::read(Stream *stream) {
        portName = stream->readStr();
        baudRate = stream->readInt32();
        dataBits = (DataBitsType) stream->readByte();
        stopBits = (StopBitsType) stream->readByte();
        parity = (ParityType) stream->readByte();
        handshake = (HandshakeType) stream->readByte();
        rtsEnable = stream->readBoolean();
        dtrEnable = stream->readBoolean();
        useSignal = stream->readBoolean();
    }

    void SerialInfo::write(Stream *stream) const {
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

    String SerialInfo::dataBitsStr() const {
        return convertDataBitsStr(dataBits);
    }

    String SerialInfo::parityStr() const {
        return convertParityStr(parity);
    }

    String SerialInfo::stopBitsStr() const {
        return convertStopBitsStr(stopBits);
    }

    String SerialInfo::handshakeStr() const {
        return convertHandshakeStr(handshake);
    }

    SerialInfo &SerialInfo::operator=(const SerialInfo &value) {
        SerialInfo::evaluates(value);
        return *this;
    }

    String SerialInfo::toString() const {
        JsonNode node;
        write(node);
        return node.toString();
    }

    bool SerialInfo::isEmpty() const {
        return this->portName.isNullOrEmpty();
    }

    SerialInfo::DataBitsType SerialInfo::parseDataBits(const String &str) {
        if (str == "8")
            return DATA_8;
        else if (str == "7")
            return DATA_7;
        else if (str == "6")
            return DATA_6;
        else if (str == "5")
            return DATA_5;
        return DATA_8;
    }

    String SerialInfo::convertDataBitsStr(DataBitsType dataBits) {
        switch (dataBits) {
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

    SerialInfo::ParityType SerialInfo::parseParity(const String &str) {
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
        return PAR_NONE;
    }

    String SerialInfo::convertParityStr(ParityType parity) {
        switch (parity) {
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

    SerialInfo::StopBitsType SerialInfo::parseStopBits(const String &str) {
        if (str == "1")
            return STOP_1;
        else if (str == "1.5")
            return STOP_1_5;
        else if (str == "2")
            return STOP_2;
        return STOP_1;
    }

    String SerialInfo::convertStopBitsStr(StopBitsType stopBits) {
        switch (stopBits) {
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

    SerialInfo::HandshakeType SerialInfo::parseHandshake(const String &str) {
        if (String::equals("OFF", str, true))
            return FLOW_OFF;
        else if (String::equals("HARDWARE", str, true))
            return FLOW_HARDWARE;
        else if (String::equals("XONXOFF", str, true))
            return FLOW_XONXOFF;
        return FLOW_OFF;
    }

    String SerialInfo::convertHandshakeStr(HandshakeType handshake) {
        switch (handshake) {
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
