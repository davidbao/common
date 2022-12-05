//
//  XmlTextWriter.cpp
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#include "xml/XmlTextWriter.h"
#include "data/ValueType.h"
#include "libxml/xmlwriter.h"

#ifdef __arm_linux__
#include <unistd.h>
#endif

namespace Xml {
    class XmlTextWriterInner {
    public:
        xmlTextWriterPtr writer;

        XmlTextWriterInner() : writer(nullptr) {
        }
    };

    XmlTextWriter::XmlTextWriter(const String &fileName) {
        _writer = new XmlTextWriterInner();

        /*
        * this initializes the library and check potential ABI mismatches
        * between the version it was compiled for and the actual shared
        * library used.
        */
        LIBXML_TEST_VERSION

        _writer->writer = xmlNewTextWriterFilename(fileName, 0);
    }

    XmlTextWriter::~XmlTextWriter() {
        close();

        delete _writer;
    }

    bool XmlTextWriter::isValid() const {
        return _writer->writer != nullptr;
    }

    bool XmlTextWriter::close() {
        if (isValid()) {
            xmlTextWriterFlush(_writer->writer);
            xmlFreeTextWriter(_writer->writer);
            _writer->writer = nullptr;
#ifdef __arm_linux__
            sync();    // save all of files.
#endif
            return true;
        }
        return false;
    }

    void XmlTextWriter::enableIndent(bool indent) {
        if (isValid()) {
            xmlTextWriterSetIndent(_writer->writer, indent ? 1 : 0);
            xmlTextWriterSetIndentString(_writer->writer, (const xmlChar *) "\t");
        }
    }

    void XmlTextWriter::enableIndent(const String &str) {
        if (isValid()) {
            xmlTextWriterSetIndent(_writer->writer, 1);
            xmlTextWriterSetIndentString(_writer->writer, (const xmlChar *) str.c_str());
        }
    }

    void XmlTextWriter::enableIndent(const char *str) {
        enableIndent(String(str));
    }

    void XmlTextWriter::writeStartDocument() {
        writeStartDocument("1.0", "utf-8", String::Empty);
    }

    void XmlTextWriter::writeStartDocument(const String &version, const String &encoding, const String &standalone) {
        if (isValid()) {
            xmlTextWriterStartDocument(_writer->writer,
                                       version.isNullOrEmpty() ? nullptr : version.c_str(),
                                       encoding.isNullOrEmpty() ? nullptr : encoding.c_str(),
                                       standalone.isNullOrEmpty() ? nullptr : standalone.c_str());
        }
    }

    void XmlTextWriter::writeEndDocument() {
        if (isValid()) {
            xmlTextWriterEndDocument(_writer->writer);
        }
    }

    void XmlTextWriter::writeStartElement(const String &localName) {
        if (isValid()) {
            xmlTextWriterStartElement(_writer->writer, (const uint8_t *) (localName).c_str());
        }
    }

    void XmlTextWriter::writeStartElement(const String &prefix, const String &localName, const String &ns) {
        if (isValid()) {
            xmlTextWriterStartElementNS(_writer->writer,
                                        (const uint8_t *) (prefix.isNullOrEmpty() ? nullptr : (prefix).c_str()),
                                        (const uint8_t *) ((localName).c_str()),
                                        (const uint8_t *) (ns.isNullOrEmpty() ? nullptr : (ns).c_str()));
        }
    }

    void XmlTextWriter::writeEndElement() {
        if (isValid()) {
            xmlTextWriterEndElement(_writer->writer);
        }
    }

    void XmlTextWriter::writeAttributeString(const String &localName, const String &value) {
        if (isValid()) {
            xmlTextWriterWriteAttribute(_writer->writer, (const uint8_t *) (localName).c_str(),
                                        (const uint8_t *) (value).c_str());
        }
    }

    void XmlTextWriter::writeAttributeInt32(const String &localName, const Int32 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeUInt32(const String &localName, const UInt32 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeInt16(const String &localName, const Int16 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeUInt16(const String &localName, const UInt16 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeInt64(const String &localName, const Int64 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeUInt64(const String &localName, const UInt64 &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeChar(const String &localName, const Char &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeByte(const String &localName, const Byte &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeBoolean(const String &localName, const Boolean &value) {
        writeAttributeString(localName, value.toString());
    }

    void XmlTextWriter::writeAttributeFloat(const String &localName, const Float &value) {
        writeAttributeString(localName, value.isNaN() ? String::Empty : value.toString());
    }

    void XmlTextWriter::writeAttributeDouble(const String &localName, const Double &value) {
        writeAttributeString(localName, value.isNaN() ? String::Empty : value.toString());
    }

    void XmlTextWriter::writeAttributeInt32(const String &localName, const int &value) {
        writeAttributeInt32(localName, Int32(value));
    }

    void XmlTextWriter::writeAttributeUInt32(const String &localName, const uint32_t &value) {
        writeAttributeUInt32(localName, UInt32(value));
    }

    void XmlTextWriter::writeAttributeInt16(const String &localName, const short &value) {
        writeAttributeInt16(localName, Int16(value));
    }

    void XmlTextWriter::writeAttributeUInt16(const String &localName, const uint16_t &value) {
        writeAttributeUInt16(localName, UInt16(value));
    }

    void XmlTextWriter::writeAttributeInt64(const String &localName, const int64_t &value) {
        writeAttributeInt64(localName, Int64(value));
    }

    void XmlTextWriter::writeAttributeUInt64(const String &localName, const uint64_t &value) {
        writeAttributeUInt64(localName, UInt64(value));
    }

    void XmlTextWriter::writeAttributeChar(const String &localName, const char &value) {
        writeAttributeChar(localName, Char(value));
    }

    void XmlTextWriter::writeAttributeByte(const String &localName, const uint8_t &value) {
        writeAttributeByte(localName, Byte(value));
    }

    void XmlTextWriter::writeAttributeBoolean(const String &localName, const bool &value) {
        writeAttributeBoolean(localName, Boolean(value));
    }

    void XmlTextWriter::writeAttributeFloat(const String &localName, const float &value) {
        writeAttributeFloat(localName, Float(value));
    }

    void XmlTextWriter::writeAttributeDouble(const String &localName, const double &value) {
        writeAttributeDouble(localName, Double(value));
    }

    void XmlTextWriter::writeAttribute(const String &localName, const bool &value) {
        writeAttributeBoolean(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const char &value) {
        writeAttributeChar(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const uint8_t &value) {
        writeAttributeByte(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const short &value) {
        writeAttributeInt16(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const uint16_t &value) {
        writeAttributeUInt16(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const int &value) {
        writeAttributeInt32(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const uint32_t &value) {
        writeAttributeUInt32(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const int64_t &value) {
        writeAttributeInt64(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const uint64_t &value) {
        writeAttributeUInt64(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const float &value) {
        writeAttributeFloat(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const double &value) {
        writeAttributeDouble(localName, value);
    }

    void XmlTextWriter::writeAttribute(const String &localName, const String &value) {
        writeAttributeString(localName, value);
    }
}
