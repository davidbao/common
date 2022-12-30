//
//  JsonTextReader.h
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef JsonTextReader_h
#define JsonTextReader_h

#include "data/ValueType.h"
#include "IO/Zip.h"
#include "configuration/ConfigFile.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"

using namespace Config;

namespace Json {
    class JsonTextReader {
    public:
        JsonTextReader();

        explicit JsonTextReader(const String &fileName);

//        JsonTextReader(const String& text, uint32_t length);
        JsonTextReader(Zip *zip, const String &fileName);

//        JsonTextReader(const String& zipFileName, const String& fileName);
        ~JsonTextReader();

        bool isValid() const;

        bool close();

        bool read(const String &name, JsonTextReader &reader) const;

        bool read(JsonTextReader &reader);

        String name() const;

        JsonNode::Type nodeType() const;

        String getAttribute(const String &name);

        bool getAttribute(const String &name, bool &value);

        bool getAttribute(const String &name, char &value,
                          char minValue = Char::MinValue, char maxValue = Char::MaxValue);

        bool getAttribute(const String &name, int8_t &value,
                          int8_t minValue = Int8::MinValue, int8_t maxValue = Int8::MaxValue);

        bool getAttribute(const String &name, uint8_t &value,
                          uint8_t minValue = UInt8::MinValue, uint8_t maxValue = UInt8::MaxValue);

        bool getAttribute(const String &name, int16_t &value,
                          int16_t minValue = Int16::MinValue, int16_t maxValue = Int16::MaxValue);

        bool getAttribute(const String &name, uint16_t &value,
                          uint16_t minValue = UInt16::MinValue, uint16_t maxValue = UInt16::MaxValue);

        bool getAttribute(const String &name, int32_t &value,
                          int32_t minValue = Int32::MinValue, int32_t maxValue = Int32::MaxValue);

        bool getAttribute(const String &name, uint32_t &value,
                          uint32_t minValue = UInt32::MinValue, uint32_t maxValue = UInt32::MaxValue);

        bool getAttribute(const String &name, int64_t &value,
                          int64_t minValue = Int64::MinValue, int64_t maxValue = Int64::MaxValue);

        bool getAttribute(const String &name, uint64_t &value,
                          uint64_t minValue = UInt64::MinValue, uint64_t maxValue = UInt64::MaxValue);

        bool getAttribute(const String &name, float &value,
                          float minValue = Float::MinValue, float maxValue = Float::MaxValue);

        bool getAttribute(const String &name, double &value,
                          double minValue = Double::MinValue, double maxValue = Double::MaxValue);

        bool getAttribute(const String &name, String &value);

        bool getAttribute(const String &name, DateTime &value,
                          const DateTime &minValue = DateTime::MinValue, const DateTime &maxValue = DateTime::MaxValue);

        bool getAttribute(const String &name, TimeSpan &value,
                          const TimeSpan &minValue = TimeSpan::MinValue, const TimeSpan &maxValue = TimeSpan::MaxValue);

        template<class T>
        bool getAttribute(const String &name, T &value) {
            String text;
            if (getAttribute(name, text)) {
                return T::parse(text, value);
            }
            return false;
        }

        const ConfigFile &configFile() const;

    private:
        JsonNode *currentNode() const;

    private:
        JsonNode _currentNode;
        int _iterator;

        ConfigFile _configFile;

        const int JsonSuccess = 1;
        const int JsonFailed = -1;
    };
}

#endif    // JsonTextReader_h
