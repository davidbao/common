//
//  XmlTextReader.cpp
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "xml/XmlTextReader.h"
#include "exception/Exception.h"
#include "IO/Path.h"
#include "data/String.h"
#include "diag/Trace.h"
#include "libxml/xmlreader.h"

using namespace Diag;
using namespace System;

namespace Xml {
    class XmlTextReaderInner {
    public:
        xmlTextReaderPtr reader;

        XmlTextReaderInner() : reader(nullptr) {
        }
    };

    XmlTextReader::XmlTextReader() : _deleteZip(false), _zipFile(nullptr) {
    }

    XmlTextReader::XmlTextReader(const String &fileName) : XmlTextReader() {
        _reader = new XmlTextReaderInner();

        /*
        * this initializes the library and check potential ABI mismatches
        * between the version it was compiled for and the actual shared
        * library used.
        */
        LIBXML_TEST_VERSION

        _reader->reader = xmlReaderForFile(fileName, nullptr, 0);

        if (_reader->reader != nullptr) {
            _configFile.rootPath = Path::getDirectoryName(fileName);
            _configFile.fileName = Path::getFileName(fileName);
        } else {
            Debug::writeFormatLine("Failed to construct XmlTextReader! file name: %s", fileName.c_str());
        }
    }

    XmlTextReader::XmlTextReader(const String &text, size_t length) : XmlTextReader() {
        _reader = new XmlTextReaderInner();

        /*
         * this initializes the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION

        if (length == 0 || length > text.length())
            length = text.length();
        _reader->reader = xmlReaderForMemory(text.c_str(), (int) length, nullptr, nullptr, 0);

        _zipFile = nullptr;
        _configFile.text = String(text, length);
    }

    XmlTextReader::XmlTextReader(Zip *zip, const String &fileName) : XmlTextReader() {
        if (zip == nullptr) {
            throw ArgumentException("zip");
        }

        _reader = new XmlTextReaderInner();

        /*
         * this initializes the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION

        _configFile.zip = zip;
#ifdef WIN32
        // fixbug: '/' must be used instead of '\' in windows.
        _configFile.fileName = String::replace(fileName, "\\", "/");
#else
        _configFile.fileName = fileName;
#endif
        _zipFile = zip->openFile(_configFile.fileName);
        if (_zipFile != nullptr)
            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
    }

    XmlTextReader::XmlTextReader(const String &zipFileName, const String &fileName) : XmlTextReader() {
        _reader = new XmlTextReaderInner();

        /*
         * this initializes the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION

        Zip *zip = new Zip(zipFileName);
        if (zip->isValid()) {
            _deleteZip = true;
            _configFile.zip = zip;
#ifdef WIN32
            // fixbug: '/' must be used instead of '\' in windows.
            _configFile.fileName = String::replace(fileName, "\\", "/");
#else
            _configFile.fileName = fileName;
#endif
            _zipFile = zip->openFile(_configFile.fileName);
            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
        } else {
            delete zip;
        }
    }

    XmlTextReader::~XmlTextReader() {
        close();

        if (_configFile.isZip() && _zipFile != nullptr) {
            _configFile.zip->closeFile(_zipFile);
            if (_deleteZip) {
                delete _configFile.zip;
            }
            _configFile.zip = nullptr;
            _zipFile = nullptr;
        }

        delete _reader;
    }

    bool XmlTextReader::isValid() const {
        return _reader->reader != nullptr && xmlTextReaderIsValid(_reader->reader) != XmlFailed;
    }

    bool XmlTextReader::close() {
        if (isValid()) {

            xmlFreeTextReader(_reader->reader);
            _reader->reader = nullptr;

            return true;
        }
        return false;
    }

    bool XmlTextReader::read() {
        if (isValid()) {
            return xmlTextReaderRead(_reader->reader) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextReader::getAttribute(const String &name, String &value) const {
        if (isValid()) {
            xmlChar *result = xmlTextReaderGetAttribute(_reader->reader, (const xmlChar *) name.c_str());
            if (result == nullptr)
                return false;

            value = (const char *) result;
            xmlFree(result);

            return true;
        }
        return false;
    }

    bool XmlTextReader::getAttribute(const String &name, const String &ns, String &value) const {
        if (isValid()) {
            xmlChar *result = xmlTextReaderGetAttributeNs(_reader->reader, (const xmlChar *) name.c_str(),
                                                          (const xmlChar *) ns.c_str());
            if (result == nullptr)
                return false;

            value = (const char *) result;
            xmlFree(result);

            return true;
        }
        return false;
    }

    bool XmlTextReader::getAttributeNames(StringArray &names) const {
        if (isValid()) {
            int count = xmlTextReaderAttributeCount(_reader->reader);
            if (count == XmlFailed) {
                return false;
            }
            for (int i = 0; i < count; i++) {
                if (xmlTextReaderMoveToAttributeNo(_reader->reader, i) == XmlSuccess) {
                    const xmlChar *name = xmlTextReaderConstName(_reader->reader);
                    names.add((const char *) name);
                }
            }
            return true;
        }
        return false;
    }

    bool XmlTextReader::hasAttributes() const {
        if (isValid()) {
            return xmlTextReaderHasAttributes(_reader->reader) == XmlSuccess;
        }
        return false;
    }

    int XmlTextReader::attributeCount() const {
        if (isValid()) {
            int result = xmlTextReaderAttributeCount(_reader->reader);
            if (result == XmlFailed) {
                return 0;
            }
            return result;
        }
        return false;
    }

    XmlNodeType XmlTextReader::nodeType() const {
        if (isValid()) {
            return (XmlNodeType) xmlTextReaderNodeType(_reader->reader);
        }
        return NodeNone;
    }

    String XmlTextReader::localName() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstLocalName(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::baseUri() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstBaseUri(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::namespaceUri() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstNamespaceUri(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::encoding() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstEncoding(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::prefix() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstPrefix(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::xmlLang() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstXmlLang(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::xmlVersion() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstXmlVersion(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    bool XmlTextReader::hasValue() const {
        if (isValid()) {
            return xmlTextReaderHasValue(_reader->reader);
        }
        return false;
    }

    bool XmlTextReader::isDefault() const {
        if (isValid()) {
            return xmlTextReaderIsDefault(_reader->reader) == 1;
        }
        return false;
    }

    bool XmlTextReader::isEOF() const {
        return readState() == EndOfFile;
    }

    XmlTextReader::ReadState XmlTextReader::readState() const {
        if (isValid()) {
            int result = xmlTextReaderReadState(_reader->reader);
            if (result == XmlFailed) {
                return ReadState::Error;
            }
            return (XmlTextReader::ReadState) result;
        }
        return ReadState::Error;
    }

    char XmlTextReader::quoteChar() const {
        if (isValid()) {
            int result = xmlTextReaderQuoteChar(_reader->reader);
            if (result == XmlFailed) {
                return false;
            }
            return (char) result;
        }
        return false;
    }

    String XmlTextReader::name() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstName(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::value() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderConstValue(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::innerXml() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderReadInnerXml(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    String XmlTextReader::outerXml() const {
        if (isValid()) {
            const xmlChar *result = xmlTextReaderReadOuterXml(_reader->reader);
#if WIN32
            return result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
            return result != nullptr ? (char *) result : String::Empty;
#endif
        }
        return String::Empty;
    }

    int XmlTextReader::depth() const {
        if (isValid()) {
            return xmlTextReaderDepth(_reader->reader);
        }
        return -1;
    }

    int XmlTextReader::lineNumber() const {
        if (isValid()) {
            return xmlTextReaderGetParserLineNumber(_reader->reader);
        }
        return -1;
    }

    int XmlTextReader::linePosition() const {
        if (isValid()) {
            return xmlTextReaderGetParserColumnNumber(_reader->reader);
        }
        return -1;
    }

    bool XmlTextReader::isEmptyElement() const {
        if (isValid()) {
            return xmlTextReaderIsEmptyElement(_reader->reader) == XmlSuccess;
        }
        return false;
    }

    XmlNodeType XmlTextReader::moveToContent() {
        if (isValid()) {
            XmlNodeType type;

            LNext:
            type = nodeType();
            switch (type) {
                case Element:
                case Text:
                case CDATA:
                case EntityRef:
                case EntityDeclaration:
                case EndElement:
                case EndEntity:
                    break;

                case Attribute:
                    moveToElement();
                    break;

                default:
                    if (read()) {
                        goto LNext;
                    }
                    break;
            }
            return type;
        }
        return XmlNodeType::NodeNone;
    }

    bool XmlTextReader::moveToElement() {
        if (isValid()) {
            return xmlTextReaderMoveToElement(_reader->reader) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextReader::moveToAttribute(int no) {
        if (isValid()) {
            return xmlTextReaderMoveToAttributeNo(_reader->reader, no) == XmlSuccess;
        }
        return false;
    }

    bool XmlTextReader::moveToAttribute(const String &name, const String &ns) {
        if (isValid()) {
            int result;
            if (ns.isNullOrEmpty()) {
                result = xmlTextReaderMoveToAttribute(_reader->reader, (const xmlChar *) name.c_str());
            } else {
                result = xmlTextReaderMoveToAttributeNs(_reader->reader, (const xmlChar *) name.c_str(),
                                                        (const xmlChar *) ns.c_str());
            }
            return result == XmlSuccess;
        }
        return false;
    }

    const ConfigFile &XmlTextReader::configFile() const {
        return _configFile;
    }
}
