//
//  XmlTextWriter.cpp
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "xml/XmlTextWriter.h"
#include "data/String.h"
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

    XmlTextWriter::XmlTextWriter(const String &fileName) : _formatting(Formatting::None), _indentString(),
                                                           _quoteChar('\0') {
        _writer = new XmlTextWriterInner();

        /*
        * this initializes the library and check potential ABI mismatches
        * between the version it was compiled for and the actual shared
        * library used.
        */
        LIBXML_TEST_VERSION

        _writer->writer = xmlNewTextWriterFilename(fileName, 0);

        setIndentString('\t');
        setFormatting(Formatting::Indented);
        setQuoteChar('\"');
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
            flush();

            xmlFreeTextWriter(_writer->writer);
            _writer->writer = nullptr;

            return true;
        }
        return false;
    }

    void XmlTextWriter::flush() {
        if (isValid()) {
            xmlTextWriterFlush(_writer->writer);

#ifdef __arm_linux__
            sync();    // save all of files.
#endif
        }
    }

    XmlTextWriter::Formatting XmlTextWriter::formatting() const {
        return _formatting;
    }

    void XmlTextWriter::setFormatting(Formatting formatting) {
        if (formatting != _formatting) {
            _formatting = formatting;

            if (isValid()) {
                xmlTextWriterSetIndent(_writer->writer, formatting == Formatting::Indented ? 1 : 0);
            }
        }
    }

    const String &XmlTextWriter::indentString() const {
        return _indentString;
    }

    void XmlTextWriter::setIndentString(const String &indentString) {
        if (indentString != _indentString) {
            _indentString = indentString;

            if (isValid()) {
                xmlTextWriterSetIndentString(_writer->writer, (const xmlChar *) indentString.c_str());
            }
        }
    }

    char XmlTextWriter::quoteChar() const {
        return _quoteChar;
    }

    void XmlTextWriter::setQuoteChar(char quoteChar) {
        if ((quoteChar == '\'' || quoteChar == '\"') && quoteChar != _quoteChar) {
            _quoteChar = quoteChar;

            if (isValid()) {
                xmlTextWriterSetQuoteChar(_writer->writer, quoteChar);
            }
        }
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
            xmlTextWriterStartElement(_writer->writer, (const xmlChar *) (localName).c_str());
        }
    }

    void XmlTextWriter::writeStartElement(const String &prefix, const String &localName, const String &ns) {
        if (isValid()) {
            xmlTextWriterStartElementNS(_writer->writer,
                                        (const xmlChar *) (prefix.isNullOrEmpty() ? nullptr : (prefix).c_str()),
                                        (const xmlChar *) ((localName).c_str()),
                                        (const xmlChar *) (ns.isNullOrEmpty() ? nullptr : (ns).c_str()));
        }
    }

    void XmlTextWriter::writeEndElement() {
        if (isValid()) {
            xmlTextWriterEndElement(_writer->writer);
        }
    }

    void XmlTextWriter::writeFullEndElement() {
        if (isValid()) {
            xmlTextWriterFullEndElement(_writer->writer);
        }
    }

    void XmlTextWriter::writeStartAttribute(const String &name) {
        if (isValid()) {
            xmlTextWriterStartAttribute(_writer->writer, (const xmlChar *) (name).c_str());
        }
    }

    void XmlTextWriter::writeStartAttribute(const String &prefix, const String &name, const String &ns) {
        if (isValid()) {
            xmlTextWriterStartAttributeNS(_writer->writer,
                                          (const xmlChar *) (prefix.isNullOrEmpty() ? nullptr : (prefix).c_str()),
                                          (const xmlChar *) ((name).c_str()),
                                          (const xmlChar *) (ns.isNullOrEmpty() ? nullptr : (ns).c_str()));
        }
    }

    void XmlTextWriter::writeEndAttribute() {
        if (isValid()) {
            xmlTextWriterEndAttribute(_writer->writer);
        }
    }

    bool XmlTextWriter::setAttribute(const String &name, const String &value) {
        if (isValid()) {
            return xmlTextWriterWriteAttribute(_writer->writer, (const xmlChar *) name.c_str(),
                                               (const xmlChar *) value.c_str()) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeString(const String &text) {
        if (isValid()) {
            return xmlTextWriterWriteString(_writer->writer, (const xmlChar *) text.c_str()) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeBase64(const ByteArray &buffer) {
        if (isValid()) {
            String str = buffer.toHexString();
            return xmlTextWriterWriteBase64(_writer->writer, str, 0, (int) str.length()) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeBinHex(const ByteArray &buffer) {
        if (isValid()) {
            return xmlTextWriterWriteBinHex(_writer->writer, (const char *) buffer.data(), 0, (int) buffer.count()) ==
                   XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeCData(const String &text) {
        if (isValid()) {
            return xmlTextWriterWriteCDATA(_writer->writer, (const xmlChar *) text.c_str()) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeComment(const String &text) {
        if (isValid()) {
            return xmlTextWriterWriteComment(_writer->writer, (const xmlChar *) text.c_str()) == XmlSuccess;
        }
        return false;
    }

    bool
    XmlTextWriter::writeDocType(const String &name, const String &pubid, const String &sysid, const String &subset) {
        if (isValid()) {
            return xmlTextWriterWriteDocType(_writer->writer,
                                             (const xmlChar *) name.c_str(), (const xmlChar *) pubid.c_str(),
                                             (const xmlChar *) sysid.c_str(), (const xmlChar *) subset.c_str()) ==
                   XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeRaw(const String &text) {
        if (isValid()) {
            return xmlTextWriterWriteRaw(_writer->writer, (const xmlChar *) text.c_str()) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextWriter::writeProcessingInstruction(const String &name, const String &text) {
        if (isValid()) {
            return xmlTextWriterWriteProcessingInstruction(_writer->writer, (const xmlChar *) name.c_str(),
                                                           (const xmlChar *) text.c_str()) == XmlSuccess;
        }
        return false;
    }
}
