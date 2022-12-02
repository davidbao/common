//
//  JsonTextReader.cpp
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright © 2017 com. All rights reserved.
//

#include "json/JsonTextReader.h"
#include "exception/Exception.h"
#include "data/Convert.h"
#include "IO/Path.h"
#include "data/ValueType.h"
#include "diag/Trace.h"

namespace Common {
    JsonTextReader::JsonTextReader() : _iterator(-1) {
    }

    JsonTextReader::JsonTextReader(const String &fileName) : JsonTextReader() {
        FileStream fs(fileName, FileMode::FileOpen, FileAccess::FileRead);
        if (fs.isOpen()) {
            _configFile.rootPath = Path::getDirectoryName(fileName);
            _configFile.fileName = Path::getFileName(fileName);

            String str;
            fs.readToEnd(str);
            JsonNode::parse(str, _currentNode);
        }

#ifdef DEBUG
        if (!isValid())
            Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
    }

//    JsonTextReader::JsonTextReader(const String& text, uint32_t length)
//    {
//        _reader = new JsonTextReaderInner();
//
//        clear();
//        /*
//         * this initialize the library and check potential ABI mismatches
//         * between the version it was compiled for and the actual shared
//         * library used.
//         */
//        LIBXML_TEST_VERSION
//
//        _reader->reader = xmlReaderForMemory(text.c_str(), (int)length, nullptr, nullptr, 0);
//
//        _zipFile = nullptr;
//    }
    JsonTextReader::JsonTextReader(Zip *zip, const String &fileName) : JsonTextReader() {
        if (zip == nullptr)
            throw ArgumentException("zip");

#ifdef WIN32
        // fixbug: '/' must be used instead of '\' in windows.
        _configFile.fileName = String::replace(fileName, "\\", "/");
#else
        _configFile.fileName = fileName;
#endif

        String str;
        if (zip->read(_configFile.fileName, str))
            JsonNode::parse(str, _currentNode);

#ifdef DEBUG
        if (!isValid())
            Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
    }
//    JsonTextReader::JsonTextReader(const String& zipFileName, const String& fileName)
//    {
//        _reader = new JsonTextReaderInner();
//
//        clear();
//        /*
//         * this initialize the library and check potential ABI mismatches
//         * between the version it was compiled for and the actual shared
//         * library used.
//         */
//        LIBXML_TEST_VERSION
//
//        Zip* zip = new Zip(zipFileName);
//        if(zip->isValid())
//        {
//            _deleteZip = true;
//            _configFile.zip = zip;
//#ifdef WIN32
//            // fixbug: '/' must be used instead of '\' in windows.
//            _configFile.fileName = String::replace(fileName, "\\", "/");
//#else
//            _configFile.fileName = fileName;
//#endif
//            _zipFile = zip->open(_configFile.fileName);
//            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
//        }
//        else
//        {
//            delete zip;
//        }
//    }

    JsonTextReader::~JsonTextReader() {
        close();

//        if(_configFile.isZip() && _zipFile != nullptr)
//        {
//            _configFile.zip->close(_zipFile);
//            if(_deleteZip)
//            {
//                delete _configFile.zip;
//            }
//            _configFile.zip = nullptr;
//            _zipFile = nullptr;
//        }
    }

    bool JsonTextReader::isValid() const {
        return !_currentNode.isEmpty();
    }

    bool JsonTextReader::close() {
        if (isValid()) {
            return true;
        }
        return false;
    }

    bool JsonTextReader::read(const String &name, JsonTextReader &reader) const {
        if (isValid()) {
            JsonNode *currentNode = this->currentNode();
            JsonNode node;
            if (currentNode->at(name, node)) {
                reader._currentNode = node;

                return true;
            }
        }
        return false;
    }

    bool JsonTextReader::read(JsonTextReader &reader) {
        if (isValid()) {
            JsonNode *currentNode = this->currentNode();
            JsonNode node;
            if (currentNode->at(++_iterator, node)) {
                reader._currentNode = node;

                return true;
            } else {
                _iterator = -1;
            }
        }
        return false;
    }

    JsonNode::Type JsonTextReader::nodeType() const {
        return this->currentNode()->type();
    }

    String JsonTextReader::name() const {
        return currentNode()->name();
    }

    JsonNode *JsonTextReader::currentNode() const {
        return (JsonNode *) &_currentNode;
    }

    String JsonTextReader::getAttribute(const String &name) {
        if (isValid()) {
            String value;
            if (currentNode()->getAttribute(name, value))
                return value;
        }
        return String::Empty;
    }

    bool JsonTextReader::getAttribute(const String &name, bool &value) {
        return Boolean::parse(getAttribute(name), value);
    }

    bool JsonTextReader::getAttribute(const String &name, char &value, char minValue, char maxValue) {
        return Char::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, int8_t &value, int8_t minValue, int8_t maxValue) {
        return Int8::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, uint8_t &value, uint8_t minValue, uint8_t maxValue) {
        return UInt8::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, int16_t &value, int16_t minValue, int16_t maxValue) {
        return Int16::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, uint16_t &value, uint16_t minValue, uint16_t maxValue) {
        return UInt16::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, int32_t &value, int32_t minValue, int32_t maxValue) {
        return Int32::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, uint32_t &value, uint32_t minValue, uint32_t maxValue) {
        return UInt32::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, int64_t &value, int64_t minValue, int64_t maxValue) {
        return Int64::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, uint64_t &value, uint64_t minValue, uint64_t maxValue) {
        return UInt64::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, float &value, float minValue, float maxValue) {
        return Float::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, double &value, double minValue, double maxValue) {
        return Double::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, String &value) {
        value = getAttribute(name);
        return true;
    }

    bool JsonTextReader::getAttribute(const String &name, DateTime &value, const DateTime &minValue,
                                      const DateTime &maxValue) {
        return DateTime::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool JsonTextReader::getAttribute(const String &name, TimeSpan &value, const TimeSpan &minValue,
                                      const TimeSpan &maxValue) {
        TimeSpan ts;
        if (TimeSpan::parse(getAttribute(name), ts) &&
            ts >= minValue && ts <= maxValue) {
            value = ts;
            return true;
        }
        return false;
    }

    const ConfigFile &JsonTextReader::configFile() const {
        return _configFile;
    }
}
