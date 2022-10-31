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
#include "data/NetType.h"
#include "IO/Zip.h"
#include "configuration/ConfigFile.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"
#include "libjson/libjson.h"

namespace Common
{
	class JsonTextReader
	{
	public:
        JsonTextReader();
        JsonTextReader(const String& fileName);
//        JsonTextReader(const String& text, uint length);
        JsonTextReader(Zip* zip, const String& fileName);
//        JsonTextReader(const String& zipFileName, const String& fileName);
		~JsonTextReader();

		bool isValid() const;
        
        bool close();

        bool read(const String& name, JsonTextReader& reader) const;
        bool read(JsonTextReader& reader);

		String name() const;
        JsonNode::Type nodeType() const;

		const String getAttribute(const String& name);
        
        bool getAttribute(const String& name, bool& value);
        bool getAttribute(const String& name, char& value,
                          char minValue = Char::MinValue, char maxValue = Char::MaxValue);
        bool getAttribute(const String& name, uint8_t& value,
                          uint8_t minValue = Byte::MinValue, uint8_t maxValue = Byte::MaxValue);
        bool getAttribute(const String& name, short& value,
                          short minValue = Int16::MinValue, short maxValue = Int16::MaxValue);
        bool getAttribute(const String& name, ushort& value,
                          ushort minValue = UInt16::MinValue, ushort maxValue = UInt16::MaxValue);
        bool getAttribute(const String& name, int& value,
                          int minValue = Int32::MinValue, int maxValue = Int32::MaxValue);
        bool getAttribute(const String& name, uint& value,
                          uint minValue = UInt32::MinValue, uint maxValue = UInt32::MaxValue);
        bool getAttribute(const String& name, int64_t& value,
                          int64_t minValue = Int64::MinValue, int64_t maxValue = Int64::MaxValue);
        bool getAttribute(const String& name, uint64_t& value,
                          uint64_t minValue = UInt64::MinValue, uint64_t maxValue = UInt64::MaxValue);
        bool getAttribute(const String& name, float& value,
                          float minValue = Float::MinValue, float maxValue = Float::MaxValue);
        bool getAttribute(const String& name, double& value,
                          double minValue = Double::MinValue, double maxValue = Double::MaxValue);
        bool getAttribute(const String& name, String& value);
        bool getAttribute(const String& name, DateTime& value,
                          DateTime minValue = DateTime::MinValue, DateTime maxValue = DateTime::MaxValue);
        bool getAttribute(const String& name, TimeSpan& value,
                          const TimeSpan& minValue = TimeSpan::MinValue, const TimeSpan& maxValue = TimeSpan::MaxValue);
        bool getAttribute(const String& name, Version& value);
        bool getAttribute(const String& name, IPAddress& value);
        bool getAttribute(const String& name, MacAddress& value);
        bool getAttribute(const String& name, Uuid& value);
        
        template <class T>
        bool getAttribute(const String& name, T& value)
        {
            String text;
            if(getAttribute(name, text))
            {
                return T::parse(text, value);
            }
            return false;
        }
        
        const ConfigFile& configFile() const;
        
    private:
        JsonNode* currentNode() const;

	private:
        JsonNode _currentNode;
        int _iterator;
        
        ConfigFile _configFile;

		const int JsonSuccess = 1;
		const int JsonFailed = -1;
	};
}

#endif	// JsonTextReader_h
