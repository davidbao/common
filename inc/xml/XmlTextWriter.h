//
//  XmlTextWriter.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef XmlTextWriter_h
#define XmlTextWriter_h

#include "data/ValueType.h"
#include "net/NetType.h"

namespace Common
{
    class XmlTextWriterInner;
	class XmlTextWriter
	{
	public:
        XmlTextWriter(const String& fileName);
		~XmlTextWriter();

		bool isValid() const;

		bool close();
        
        void enableIndent(bool indent = true);
        void enableIndent(const String& str);
        void enableIndent(const char* str);
        
        void writeStartDocument();
        void writeStartDocument(const String& version, const String& encoding, const String& standalone);
        void writeEndDocument();
        
        void writeStartElement(const String& localName);
        void writeStartElement(const String& prefix, const String& localName, const String& ns);
        void writeEndElement();

        void writeAttributeBoolean(const String& localName, const Boolean& value);
        void writeAttributeChar(const String& localName, const Char& value);
        void writeAttributeByte(const String& localName, const Byte& value);
        void writeAttributeInt16(const String& localName, const Int16& value);
        void writeAttributeUInt16(const String& localName, const UInt16& value);
        void writeAttributeInt32(const String& localName, const Int32& value);
        void writeAttributeUInt32(const String& localName, const UInt32& value);
        void writeAttributeInt64(const String& localName, const Int64& value);
        void writeAttributeUInt64(const String& localName, const UInt64& value);
        void writeAttributeFloat(const String& localName, const Float& value);
        void writeAttributeDouble(const String& localName, const Double& value);
        void writeAttributeString(const String& localName, const String& value);

        void writeAttributeBoolean(const String& localName, const bool& value);
        void writeAttributeChar(const String& localName, const char& value);
        void writeAttributeByte(const String& localName, const uint8_t& value);
        void writeAttributeInt16(const String& localName, const short& value);
        void writeAttributeUInt16(const String& localName, const uint16_t& value);
        void writeAttributeInt32(const String& localName, const int& value);
        void writeAttributeUInt32(const String& localName, const uint32_t& value);
        void writeAttributeInt64(const String& localName, const int64_t& value);
        void writeAttributeUInt64(const String& localName, const uint64_t& value);
        void writeAttributeFloat(const String& localName, const float& value);
        void writeAttributeDouble(const String& localName, const double& value);

        void writeAttribute(const String& localName, const bool& value);
        void writeAttribute(const String& localName, const char& value);
        void writeAttribute(const String& localName, const uint8_t& value);
        void writeAttribute(const String& localName, const short& value);
        void writeAttribute(const String& localName, const uint16_t& value);
        void writeAttribute(const String& localName, const int& value);
        void writeAttribute(const String& localName, const uint32_t& value);
        void writeAttribute(const String& localName, const int64_t& value);
        void writeAttribute(const String& localName, const uint64_t& value);
        void writeAttribute(const String& localName, const float& value);
        void writeAttribute(const String& localName, const double& value);
        void writeAttribute(const String& localName, const String& value);

        template <class T>
        void writeAttribute(const String& localName, const T& value)
        {
            writeAttributeString(localName, value.toString());
        }
        
	private:
		XmlTextWriterInner* _writer;
	};
}

#endif	// XmlTextWriter_h
