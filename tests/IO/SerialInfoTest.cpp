//
//  SerialInfoTest.cpp
//  common
//
//  Created by baowei on 2023/1/20.
//  Copyright (c) 20123 com. All rights reserved.
//

#include "IO/SerialInfo.h"
#include "IO/MemoryStream.h"
#include "xml/XmlTextWriter.h"
#include "xml/XmlTextReader.h"

using namespace IO;
using namespace Xml;

bool testConstructor() {
    {
        SerialInfo test;
        if (!test.portName.isNullOrEmpty()) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1");
        if (test.portName.isNullOrEmpty()) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        if (test.portName != "/dev/ttyS1") {
            return false;
        }
        if (test.dataBits != SerialInfo::DATA_8) {
            return false;
        }
        if (test.stopBits != SerialInfo::STOP_1) {
            return false;
        }
        if (test.parity != SerialInfo::PAR_NONE) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1");
        SerialInfo test2 = test;
        if (test != test2) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1");
        SerialInfo test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testStaticMethods() {
    {
        SerialInfo::DataBitsType type = SerialInfo::parseDataBits("8");
        if (type != SerialInfo::DATA_8) {
            return false;
        }
    }
    {
        SerialInfo::DataBitsType type = SerialInfo::parseDataBits("7");
        if (type != SerialInfo::DATA_7) {
            return false;
        }
    }
    {
        SerialInfo::DataBitsType type = SerialInfo::parseDataBits("6");
        if (type != SerialInfo::DATA_6) {
            return false;
        }
    }
    {
        SerialInfo::DataBitsType type = SerialInfo::parseDataBits("5");
        if (type != SerialInfo::DATA_5) {
            return false;
        }
    }
    {
        SerialInfo::DataBitsType type = SerialInfo::parseDataBits("");
        if (type != SerialInfo::DATA_8) {
            return false;
        }
    }
    {
        if (SerialInfo::convertDataBitsStr(SerialInfo::DATA_8) != "8") {
            return false;
        }
    }
    {
        if (SerialInfo::convertDataBitsStr(SerialInfo::DATA_7) != "7") {
            return false;
        }
    }
    {
        if (SerialInfo::convertDataBitsStr(SerialInfo::DATA_6) != "6") {
            return false;
        }
    }
    {
        if (SerialInfo::convertDataBitsStr(SerialInfo::DATA_5) != "5") {
            return false;
        }
    }

    {
        if (SerialInfo::parseParity("n") != SerialInfo::PAR_NONE) {
            return false;
        }
        if (SerialInfo::parseParity("N") != SerialInfo::PAR_NONE) {
            return false;
        }
        if (SerialInfo::parseParity("NONE") != SerialInfo::PAR_NONE) {
            return false;
        }
        if (SerialInfo::parseParity("none") != SerialInfo::PAR_NONE) {
            return false;
        }
    }
    {
        if (SerialInfo::parseParity("o") != SerialInfo::PAR_ODD) {
            return false;
        }
        if (SerialInfo::parseParity("O") != SerialInfo::PAR_ODD) {
            return false;
        }
        if (SerialInfo::parseParity("ODD") != SerialInfo::PAR_ODD) {
            return false;
        }
        if (SerialInfo::parseParity("odd") != SerialInfo::PAR_ODD) {
            return false;
        }
    }
    {
        if (SerialInfo::parseParity("e") != SerialInfo::PAR_EVEN) {
            return false;
        }
        if (SerialInfo::parseParity("E") != SerialInfo::PAR_EVEN) {
            return false;
        }
        if (SerialInfo::parseParity("EVEN") != SerialInfo::PAR_EVEN) {
            return false;
        }
        if (SerialInfo::parseParity("even") != SerialInfo::PAR_EVEN) {
            return false;
        }
    }
    {
        if (SerialInfo::parseParity("s") != SerialInfo::PAR_SPACE) {
            return false;
        }
        if (SerialInfo::parseParity("S") != SerialInfo::PAR_SPACE) {
            return false;
        }
        if (SerialInfo::parseParity("SPACE") != SerialInfo::PAR_SPACE) {
            return false;
        }
        if (SerialInfo::parseParity("space") != SerialInfo::PAR_SPACE) {
            return false;
        }
    }
    {
        if (SerialInfo::convertParityStr(SerialInfo::PAR_NONE) != "N") {
            return false;
        }
        if (SerialInfo::convertParityStr(SerialInfo::PAR_ODD) != "O") {
            return false;
        }
        if (SerialInfo::convertParityStr(SerialInfo::PAR_EVEN) != "E") {
            return false;
        }
        if (SerialInfo::convertParityStr(SerialInfo::PAR_MARK) != "M") {
            return false;
        }
        if (SerialInfo::convertParityStr(SerialInfo::PAR_SPACE) != "S") {
            return false;
        }
    }

    {
        if (SerialInfo::parseStopBits("1") != SerialInfo::STOP_1) {
            return false;
        }
        if (SerialInfo::parseStopBits("1.5") != SerialInfo::STOP_1_5) {
            return false;
        }
        if (SerialInfo::parseStopBits("2") != SerialInfo::STOP_2) {
            return false;
        }
        if (SerialInfo::parseStopBits("") != SerialInfo::STOP_1) {
            return false;
        }
    }
    {
        if (SerialInfo::convertStopBitsStr(SerialInfo::STOP_1) != "1") {
            return false;
        }
        if (SerialInfo::convertStopBitsStr(SerialInfo::STOP_1_5) != "1.5") {
            return false;
        }
        if (SerialInfo::convertStopBitsStr(SerialInfo::STOP_2) != "2") {
            return false;
        }
    }

    {
        if (SerialInfo::parseHandshake("off") != SerialInfo::FLOW_OFF) {
            return false;
        }
        if (SerialInfo::parseHandshake("OFF") != SerialInfo::FLOW_OFF) {
            return false;
        }
        if (SerialInfo::parseHandshake("hardware") != SerialInfo::FLOW_HARDWARE) {
            return false;
        }
        if (SerialInfo::parseHandshake("HARDWARE") != SerialInfo::FLOW_HARDWARE) {
            return false;
        }
        if (SerialInfo::parseHandshake("xonxoff") != SerialInfo::FLOW_XONXOFF) {
            return false;
        }
        if (SerialInfo::parseHandshake("XONXOFF") != SerialInfo::FLOW_XONXOFF) {
            return false;
        }
    }
    {
        if (SerialInfo::convertHandshakeStr(SerialInfo::FLOW_OFF) != "OFF") {
            return false;
        }
        if (SerialInfo::convertHandshakeStr(SerialInfo::FLOW_HARDWARE) != "HARDWARE") {
            return false;
        }
        if (SerialInfo::convertHandshakeStr(SerialInfo::FLOW_XONXOFF) != "XONXOFF") {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        SerialInfo test2("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        if (!test.equals(test2)) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        SerialInfo test2("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        SerialInfo test2;
        test2.evaluates(test);
        if (test != test2) {
            return false;
        }
    }
    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        SerialInfo test2;
        test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testReadWrite() {
    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        String fileName = Path::combine(Path::getTempPath(), "test.xml");

        XmlTextWriter writer(fileName);
        writer.writeStartDocument();
        writer.writeStartElement("serial");
        test.write(writer);
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        XmlTextReader reader(fileName);
        SerialInfo test2;

        while (reader.read()) {
            if (reader.nodeType() == XmlNodeType::Element &&
                reader.localName() == "serial") {
                test2.read(reader);
            }
        }
        reader.close();

        File::deleteFile(fileName);

        if (test != test2) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        MemoryStream ms;
        test.write(&ms);
        ms.seek(0);
        SerialInfo test2;
        test2.read(&ms);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testToString() {
    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        if (test.dataBitsStr() != "8") {
            return false;
        }
        if (test.stopBitsStr() != "1") {
            return false;
        }
        if (test.parityStr() != "N") {
            return false;
        }
        if (test.handshakeStr() != "OFF") {
            return false;
        }

        JsonNode node;
        if (!JsonNode::parse(test.toString(), node)) {
            return false;
        }
        SerialInfo test2;
        test2.read(node);
        if (test != test2) {
            return false;
        }
    }

    {
        SerialInfo test("/dev/ttyS1", 9600, SerialInfo::DATA_8, SerialInfo::STOP_1, SerialInfo::PAR_NONE);
        if (test.isEmpty()) {
            return false;
        }
    }

    {
        SerialInfo test;
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    if (!testStaticMethods()) {
        return 2;
    }

    if (!testEquals()) {
        return 3;
    }

    if (!testEvaluates()) {
        return 4;
    }

    if (!testReadWrite()) {
        return 5;
    }

    if (!testToString()) {
        return 6;
    }

    return 0;
}