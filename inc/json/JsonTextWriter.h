//
//  JsonTextWriter.h
//  common
//
//  Created by baowei on 2018/5/30.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef JsonTextWriter_h
#define JsonTextWriter_h

#include "data/ValueType.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "net/NetType.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"

namespace Common
{
    class JsonTextWriter
    {
    public:
        JsonTextWriter(const String& fileName);
        ~JsonTextWriter();
        
        bool isValid() const;
        
        bool close();
        
        void enableIndent(bool indent = true);
        
        void writeStartDocument();
        void writeEndDocument();
        
        void writeStartElement(const String& name, JsonNode::Type type = JsonNode::Type::TypeNode);
        void writeEndElement();
        
        void writeAttributeBoolean(const String& name, const Boolean& value);
        void writeAttributeChar(const String& name, const Char& value);
        void writeAttributeByte(const String& name, const Byte& value);
        void writeAttributeInt16(const String& name, const Int16& value);
        void writeAttributeUInt16(const String& name, const UInt16& value);
        void writeAttributeInt32(const String& name, const Int32& value);
        void writeAttributeUInt32(const String& name, const UInt32& value);
        void writeAttributeInt64(const String& name, const Int64& value);
        void writeAttributeUInt64(const String& name, const UInt64& value);
        void writeAttributeFloat(const String& name, const Float& value);
        void writeAttributeDouble(const String& name, const Double& value);
        void writeAttributeString(const String& name, const String& value);
        void writeAttributeDateTime(const String& name, const DateTime& value, DateTime::Format format = DateTime::Format::YYYYMMDDHHMMSS);
//        void writeAttributeTimeSpan(const String& name, const TimeSpan& value, TimeSpan::Format format = TimeSpan::Format::HHMMSS);
        void writeAttributeVersion(const String& name, const Version& value);
        
        void writeAttributeBoolean(const String& name, const bool& value);
        void writeAttributeChar(const String& name, const char& value);
        void writeAttributeByte(const String& name, const uint8_t& value);
        void writeAttributeInt16(const String& name, const short& value);
        void writeAttributeUInt16(const String& name, const uint16_t& value);
        void writeAttributeInt32(const String& name, const int& value);
        void writeAttributeUInt32(const String& name, const uint32_t& value);
        void writeAttributeInt64(const String& name, const int64_t& value);
        void writeAttributeUInt64(const String& name, const uint64_t& value);
        void writeAttributeFloat(const String& name, const float& value);
        void writeAttributeDouble(const String& name, const double& value);
        
        void writeAttribute(const String& name, const bool& value);
        void writeAttribute(const String& name, const char& value);
        void writeAttribute(const String& name, const uint8_t& value);
        void writeAttribute(const String& name, const short& value);
        void writeAttribute(const String& name, const uint16_t& value);
        void writeAttribute(const String& name, const int& value);
        void writeAttribute(const String& name, const uint32_t& value);
        void writeAttribute(const String& name, const int64_t& value);
        void writeAttribute(const String& name, const uint64_t& value);
        void writeAttribute(const String& name, const float& value);
        void writeAttribute(const String& name, const double& value);
        void writeAttribute(const String& name, const String& value);
        void writeAttribute(const String& name, const char* value);
        void writeAttribute(const String& name, const DateTime& value, DateTime::Format format = DateTime::Format::YYYYMMDDHHMMSS);
//        void writeAttribute(const String& name, const TimeSpan& value, TimeSpan::Format format = TimeSpan::Format::HHMMSS);

        template <class T>
        void writeAttribute(const String& name, const T& value)
        {
            writeAttribute(name, value.toString());
        }
        
    private:
        JsonNode* currentNode() const;
        JsonNode* parentNode() const;
        
    private:
        bool _indent;
        
        FileStream* _stream;
        JsonNode* _rootNode;
        PList<JsonNode> _nodes;
    };
}

#endif	// JsonTextWriter_h
