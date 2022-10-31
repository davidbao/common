//
//  JsonTextWriter.cpp
//  common
//
//  Created by baowei on 2018-10-27.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "json/JsonTextWriter.h"

namespace Common
{
    JsonTextWriter::JsonTextWriter(const String& fileName) : _indent(true), _rootNode(nullptr)
    {
        _stream = new FileStream(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    }
    
    JsonTextWriter::~JsonTextWriter()
    {
        close();
        
        delete _stream;
        _stream = nullptr;
    }
    
    bool JsonTextWriter::isValid() const
    {
        return _stream != nullptr && _stream->isOpen();
    }
    
    bool JsonTextWriter::close()
    {
        if (isValid())
        {
            _stream->close();
            return true;
        }
        return false;
    }
    
    void JsonTextWriter::enableIndent(bool indent)
    {
        if (isValid())
        {
            _indent = indent;
        }
    }
    
    JsonNode* JsonTextWriter::currentNode() const
    {
        return _nodes.count() > 0 ? _nodes[_nodes.count() - 1] : _rootNode;
    }
    JsonNode* JsonTextWriter::parentNode() const
    {
        return _nodes.count() > 1 ? _nodes[_nodes.count() - 2] : _rootNode;
    }
    
    void JsonTextWriter::writeStartDocument()
    {
        assert(_rootNode == nullptr);
        _rootNode = new JsonNode(JsonNode::TypeNode);
    }
    void JsonTextWriter::writeEndDocument()
    {
        if (isValid())
        {
            if(_rootNode != nullptr)
            {
                String text = _rootNode->toString(_indent);
                _stream->writeText(text);
                
                delete _rootNode;
                _rootNode = nullptr;
            }
        }
    }
    
    void JsonTextWriter::writeStartElement(const String& name, JsonNode::Type type)
    {
        if (isValid() && _rootNode != nullptr)
        {
            JsonNode* node = new JsonNode(name, type);
            _nodes.add(node);
        }
    }
    void JsonTextWriter::writeEndElement()
    {
        if (isValid() && _rootNode != nullptr)
        {
            JsonNode* cnode = currentNode();
            JsonNode* pnode = parentNode();
            if(cnode != pnode)
            {
                pnode->add(*cnode);
            }
            if(_nodes.count() > 0)
            {
                _nodes.removeAt(_nodes.count() - 1);
            }
        }
    }
    
    void JsonTextWriter::writeAttributeString(const String& name, const String& value)
    {
        if (isValid())
        {
            currentNode()->add(JsonNode(name, value));
        }
    }
    void JsonTextWriter::writeAttributeInt32(const String& name, const Int32& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeUInt32(const String& name, const UInt32& value)
    {
        writeAttributeString(name, value.toString());
    }
    
    void JsonTextWriter::writeAttributeInt16(const String& name, const Int16& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeUInt16(const String& name, const UInt16& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeInt64(const String& name, const Int64& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeUInt64(const String& name, const UInt64& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeChar(const String& name, const Char& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeByte(const String& name, const Byte& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeBoolean(const String& name, const Boolean& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttributeFloat(const String& name, const Float& value)
    {
        writeAttributeString(name, value.isNaN() ? String::Empty : value.toString());
    }
    void JsonTextWriter::writeAttributeDouble(const String& name, const Double& value)
    {
        writeAttributeString(name, value.isNaN() ? String::Empty : value.toString());
    }
    void JsonTextWriter::writeAttributeDateTime(const String& name, const DateTime& value, DateTime::Format format)
    {
        writeAttributeString(name, value.toString(format));
    }
    void JsonTextWriter::writeAttributeTimeSpan(const String& name, const TimeSpan& value, TimeSpan::Format format)
    {
        writeAttributeString(name, value.toString(format));
    }
    void JsonTextWriter::writeAttributeVersion(const String& name, const Version& value)
    {
        writeAttributeString(name, value.toString());
    }
    
    void JsonTextWriter::writeAttributeInt32(const String& name, const int& value)
    {
        writeAttributeInt32(name, Int32(value));
    }
    void JsonTextWriter::writeAttributeUInt32(const String& name, const uint& value)
    {
        writeAttributeUInt32(name, UInt32(value));
    }
    
    void JsonTextWriter::writeAttributeInt16(const String& name, const short& value)
    {
        writeAttributeInt16(name, Int16(value));
    }
    void JsonTextWriter::writeAttributeUInt16(const String& name, const ushort& value)
    {
        writeAttributeUInt16(name, UInt16(value));
    }
    void JsonTextWriter::writeAttributeInt64(const String& name, const int64_t& value)
    {
        writeAttributeInt64(name, Int64(value));
    }
    void JsonTextWriter::writeAttributeUInt64(const String& name, const uint64_t& value)
    {
        writeAttributeUInt64(name, UInt64(value));
    }
    void JsonTextWriter::writeAttributeChar(const String& name, const char& value)
    {
        writeAttributeChar(name, Char(value));
    }
    void JsonTextWriter::writeAttributeByte(const String& name, const uint8_t& value)
    {
        writeAttributeByte(name, Byte(value));
    }
    void JsonTextWriter::writeAttributeBoolean(const String& name, const bool& value)
    {
        writeAttributeBoolean(name, Boolean(value));
    }
    void JsonTextWriter::writeAttributeFloat(const String& name, const float& value)
    {
        writeAttributeFloat(name, Float(value));
    }
    void JsonTextWriter::writeAttributeDouble(const String& name, const double& value)
    {
        writeAttributeDouble(name, Double(value));
    }
    
    void JsonTextWriter::writeAttribute(const String& name, const bool& value)
    {
        writeAttributeBoolean(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const char& value)
    {
        writeAttributeChar(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const uint8_t& value)
    {
        writeAttributeByte(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const short& value)
    {
        writeAttributeInt16(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const ushort& value)
    {
        writeAttributeUInt16(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const int& value)
    {
        writeAttributeInt32(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const uint& value)
    {
        writeAttributeUInt32(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const int64_t& value)
    {
        writeAttributeInt64(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const uint64_t& value)
    {
        writeAttributeUInt64(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const float& value)
    {
        writeAttributeFloat(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const double& value)
    {
        writeAttributeDouble(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const String& value)
    {
        writeAttributeString(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const char* value)
    {
        writeAttributeString(name, value != nullptr ? String(value) : String::Empty);
    }
    void JsonTextWriter::writeAttribute(const String& name, const DateTime& value, DateTime::Format format)
    {
        writeAttributeDateTime(name, value, format);
    }
    void JsonTextWriter::writeAttribute(const String& name, const TimeSpan& value, TimeSpan::Format format)
    {
        writeAttributeTimeSpan(name, value, format);
    }
    void JsonTextWriter::writeAttribute(const String& name, const Version& value)
    {
        writeAttributeVersion(name, value);
    }
    void JsonTextWriter::writeAttribute(const String& name, const IPAddress& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const MacAddress& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const Uuid& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const RectangleF& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const Rectangle& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const PointF& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const Point& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const SizeF& value)
    {
        writeAttributeString(name, value.toString());
    }
    void JsonTextWriter::writeAttribute(const String& name, const Size& value)
    {
        writeAttributeString(name, value.toString());
    }
}
