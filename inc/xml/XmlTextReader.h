//
//  XmlTextReader.h
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef XmlTextReader_h
#define XmlTextReader_h

#include "data/ValueType.h"
#include "data/NetType.h"
#include "IO/Zip.h"
#include "configuration/ConfigFile.h"
#include "xml/XmlNode.h"

namespace Common
{
    class XmlTextReaderInner;
	class XmlTextReader
	{
	public:
        XmlTextReader(const String& fileName);
        XmlTextReader(const String& text, size_t length);
        XmlTextReader(Zip* zip, const String& fileName);
        XmlTextReader(const String& zipFileName, const String& fileName);
		~XmlTextReader();

		bool isValid() const;

		void clear();

		bool read();

		const String& name();
		const String& localName();
		const String& namespaceUri();
		XmlNodeType nodeType();
		const String& value();
        bool isEmptyElement() const;

		XmlNodeType moveToContent();
		bool moveToElement();

		const String getAttribute(const String& name) const;
        bool getAttribute(const String& name, String& value) const;
        
        bool getAttribute(const String& name, bool& value) const;
        bool getAttribute(const String& name, char& value,
                          char minValue = Char::MinValue, char maxValue = Char::MaxValue) const;
        bool getAttribute(const String& name, uint8_t& value,
                          uint8_t minValue = Byte::MinValue, uint8_t maxValue = Byte::MaxValue) const;
        bool getAttribute(const String& name, short& value,
                          short minValue = Int16::MinValue, short maxValue = Int16::MaxValue) const;
        bool getAttribute(const String& name, ushort& value,
                          ushort minValue = UInt16::MinValue, ushort maxValue = UInt16::MaxValue) const;
        bool getAttribute(const String& name, int& value,
                          int minValue = Int32::MinValue, int maxValue = Int32::MaxValue) const;
        bool getAttribute(const String& name, uint& value,
                          uint minValue = UInt32::MinValue, uint maxValue = UInt32::MaxValue) const;
        bool getAttribute(const String& name, int64_t& value,
                          int64_t minValue = Int64::MinValue, int64_t maxValue = Int64::MaxValue) const;
        bool getAttribute(const String& name, uint64_t& value,
                          uint64_t minValue = UInt64::MinValue, uint64_t maxValue = UInt64::MaxValue) const;
        bool getAttribute(const String& name, float& value,
                          float minValue = Float::MinValue, float maxValue = Float::MaxValue) const;
        bool getAttribute(const String& name, double& value,
                          double minValue = Double::MinValue, double maxValue = Double::MaxValue) const;
        bool getAttribute(const String& name, DateTime& value,
                          DateTime minValue = DateTime::MinValue, DateTime maxValue = DateTime::MaxValue) const;
        bool getAttribute(const String& name, TimeSpan& value,
                          const TimeSpan& minValue = TimeSpan::MinValue, const TimeSpan& maxValue = TimeSpan::MaxValue) const;
        bool getAttribute(const String& name, Version& value) const;
        bool getAttribute(const String& name, IPAddress& value) const;
        bool getAttribute(const String& name, MacAddress& value) const;
        bool getAttribute(const String& name, Uuid& value) const;
        template <class T>
        bool getAttribute(const String& name, T& value) const
        {
            return T::parse(getAttribute(name), value);
        }
        template <class T>
        bool getAttribute(const String& name, T& value, const T& minValue, const T& maxValue) const
        {
            return getAttribute(name, value) && (value >= minValue && value <= maxValue);
        }
        
        const ConfigFile& configFile() const;

	private:
		XmlTextReaderInner* _reader;

		String _name;
		String _localName;
		String _namespaceUri;
		XmlNodeType _nodeType;
		String _value;
        
        bool _deleteZip;
        ZipFile* _zipFile;
        ConfigFile _configFile;

		const int XmlSuccess = 1;
		const int XmlFailed = -1;
	};
}

#endif	// XmlTextReader_h
