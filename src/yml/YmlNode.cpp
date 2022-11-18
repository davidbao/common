//
//  YmlNode.cpp
//  common
//
//  Created by baowei on 2020/3/2.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "yml/YmlNode.h"
#include "diag/Trace.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace YAML;

namespace Common
{
    YmlNode::YmlNode(Type type) : _attach(false)
    {
        if(type == Type::TypeUndefined)
            _inner = new Node(NodeType::Undefined);
        else if(type == Type::TypeNull)
            _inner = new Node(NodeType::Null);
        else if(type == Type::TypeScalar)
            _inner = new Node(NodeType::Scalar);
        else if(type == Type::TypeSequence)
            _inner = new Node(NodeType::Sequence);
        else if(type == Type::TypeMap)
            _inner = new Node(NodeType::Map);
    }
    YmlNode::YmlNode(const YmlNode& node) : _attach(false)
    {
        if(node._attach)
        {
            _inner = node._inner;
            _attach = node._attach;
        }
        else
        {
            _inner = new Node();
            _inner->operator=(*node._inner);
        }
    }
    YmlNode::YmlNode(const char* value) : YmlNode((String)value)
	{
	}
    YmlNode::YmlNode(const String& value) : _attach(false)
    {
        _inner = new Node(value.c_str());
    }
    YmlNode::YmlNode(const bool& value) : YmlNode(((Boolean)value).toString())
    {
    }
    YmlNode::YmlNode(const char& value) : YmlNode(((Char)value).toString())
    {
    }
    YmlNode::YmlNode(const uint8_t& value) : YmlNode(((Byte)value).toString())
    {
    }
    YmlNode::YmlNode(const short& value) : YmlNode(((Int16)value).toString())
    {
    }
    YmlNode::YmlNode(const uint16_t& value) : YmlNode(((UInt16)value).toString())
    {
    }
    YmlNode::YmlNode(const int& value) : YmlNode(((Int32)value).toString())
    {
    }
    YmlNode::YmlNode(const uint32_t& value) : YmlNode(((UInt32)value).toString())
    {
    }
    YmlNode::YmlNode(const int64_t& value) : YmlNode(((Int64)value).toString())
    {
    }
    YmlNode::YmlNode(const uint64_t& value) : YmlNode(((UInt64)value).toString())
    {
    }
    YmlNode::YmlNode(const float& value) : YmlNode(((Float)value).toString())
    {
    }
    YmlNode::YmlNode(const double& value) : YmlNode(((Double)value).toString())
    {
    }
    YmlNode::YmlNode(const Uuid& value) : YmlNode(value.toString())
    {
    }
    YmlNode::YmlNode(const DateTime& value, DateTime::Format format) : _attach(false)
    {
        _inner = new Node(value.toString(format).c_str());
    }
    YmlNode::YmlNode(const TimeSpan& value, TimeSpan::Format format) : _attach(false)
    {
        _inner = new Node(value.toString(format).c_str());
    }
    YmlNode::YmlNode(Node* node) : _attach(true)
    {
        _inner = node;
    }
    YmlNode::~YmlNode()
    {
        if(!_attach)
        {
            delete _inner;
            _inner = nullptr;
        }
    }
    
    void YmlNode::add(const YmlNode& node)
    {
        try
        {
            _inner->push_back(*node._inner);
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
    }
    void YmlNode::add(const String& name, const YmlNode& value)
    {
        try
        {
            YAML::Node& node = *_inner;
            node[name.c_str()] = *value._inner;
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
    }
    
    String YmlNode::toString() const
    {
        String str;
        try
        {
            YAML::Emitter out;
            out << *_inner;
            str = out.c_str();
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        return str;
    }
    
    bool YmlNode::parse(const String& str, YmlNode& node)
    {
        try
        {
            YAML::Node temp = YAML::Load(str.c_str());
            if(temp.Type() != NodeType::Undefined)
            {
                node._inner->operator=(temp);
                return true;
            }
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }
    bool YmlNode::parseFile(const String& fileName, YmlNode& node)
    {
        try
        {
            YAML::Node temp = YAML::LoadFile(fileName.c_str());
            if(temp.Type() != NodeType::Undefined)
            {
                node._inner->operator=(temp);
                return true;
            }
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }

    bool YmlNode::updateFile(const String& fileName, const Properties& properties)
    {
        try
        {
            YAML::Node node;
            if(File::exists(fileName))
                node = YAML::LoadFile(fileName.c_str()); // gets the root node

            StringArray keys;
            properties.keys(keys);
            for (uint32_t i=0; i<keys.count(); i++)
            {
                String key = keys[i];
                String value;
                if(properties.at(key, value))
                {
                    StringArray texts;
                    StringArray::parse(key, texts, '.');
                    YAML::Node* subNode = nullptr;
                    for (uint32_t j=0; j<texts.count(); j++)
                    {
                        const String& text = texts[j];
                        YAML::Node* item = j == 0 ? &node : subNode;
                        const YAML::Node& n = item->operator[](text.c_str());
                        subNode = (YAML::Node*)&n;
                    }
                    if(!value.isNullOrEmpty())
                        subNode->operator=(value.c_str());
                }
            }

            String path = Path::getDirectoryName(fileName);
            if(!Directory::exists(path)) {
                Directory::createDirectory(path);
            }
            String name = Path::getFileName(fileName);
            String tempFileName = Path::combine(path, String("~") + name);
            std::ofstream fout(tempFileName);
            fout << node; // dump it back into the file
            fout.close();
            return File::move(tempFileName, fileName);
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }

    bool YmlNode::getAttribute(const String& name, String& value) const
    {
        try
        {
            const Node& node = _inner->operator[](name.c_str());
            value = node.as<std::string>();
            return true;
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, bool& value) const
    {
        Boolean result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, uint8_t& value) const
    {
        Byte result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, int& value) const
    {
        Int32 result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, uint32_t& value) const
    {
        UInt32 result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, int64_t& value) const
    {
        Int64 result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, uint64_t& value) const
    {
        UInt64 result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, float& value) const
    {
        Float result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }
    bool YmlNode::getAttribute(const String& name, double& value) const
    {
        Double result;
        if(getAttribute(name, result))
        {
            value = result;
            return true;
        }
        return false;
    }

    bool YmlNode::setAttribute(const String& name, const String& value)
    {
        try
        {
            YAML::Node& node = *_inner;
            node[name.c_str()] = value.c_str();
            return true;
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }
    bool YmlNode::setAttribute(const String& name, const bool& value)
    {
        return setAttribute<Boolean>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const uint8_t& value)
    {
        return setAttribute<Byte>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const int& value)
    {
        return setAttribute<Int32>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const uint32_t& value)
    {
        return setAttribute<UInt32>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const int64_t& value)
    {
        return setAttribute<Int64>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const uint64_t& value)
    {
        return setAttribute<UInt64>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const float& value)
    {
        return setAttribute<Float>(name, value);
    }
    bool YmlNode::setAttribute(const String& name, const double& value)
    {
        return setAttribute<Double>(name, value);
    }
    void YmlNode::setAttributes(const StringMap& values)
    {
        StringArray keys;
        values.keys(keys);
        for (uint32_t i=0; i<keys.count(); i++)
        {
            const String& key = keys[i];
            String value;
            if(values.at(key, value))
            {
                setAttribute(key, value);
            }
        }
    }

    YmlNode::Type YmlNode::type() const
    {
        switch (_inner->Type())
        {
            case NodeType::Undefined:
                return TypeUndefined;
            case NodeType::Null:
                return TypeNull;
            case NodeType::Sequence:
                return TypeSequence;
            case NodeType::Scalar:
                return TypeScalar;
            case NodeType::Map:
                return TypeMap;
            default:
                return TypeUndefined;
        }
    }
    size_t YmlNode::size() const
    {
        return _inner->size();
    }
    bool YmlNode::isEmpty() const
    {
        return type() == TypeUndefined || type() == TypeNull;
    }

    void YmlNode::getProperties(const Node& node, String& levelStr, Properties& properties) const
    {
        String tempLevelStr;
        auto it = node.begin();
        for (; it != node.end(); ++it)
        {
            tempLevelStr = levelStr;
            
            const Node& key = it->first;
            const Node& value = it->second;
            if (key.Type() == NodeType::Scalar) {
              // This should be true; do something here with the scalar key.
                if(!levelStr.isNullOrEmpty())
                    levelStr.append('.');
                levelStr.append(key.as<string>());
                properties.add(levelStr, String::Empty);
//#ifdef DEBUG
//                String str = String::format("%s=%s", levelStr.c_str(), String::Empty.c_str());
//                Debug::writeLine(str);
//#endif
            }
            
            getValueProperties(value, levelStr, properties);
            
            levelStr = tempLevelStr;
        }
//#ifdef DEBUG
//        PList<String> keys;
//        properties.keys(keys);
//        for (uint32_t i=0; i<keys.count(); i++)
//        {
//            const String *key = keys[i];
//            String value;
//            properties.at(*key, value);
//            String str = String::format("%s=%s", (*key).c_str(), value.c_str());
//            Debug::writeLine(str);
//        }
//#endif
    }
    void YmlNode::getValueProperties(const Node& node, String& levelStr, Properties& properties) const
    {
        if (node.Type() == NodeType::Map) {
          // This should be true; do something here with the map.
            getProperties(node, levelStr, properties);
            levelStr.empty();
        } else if (node.Type() == NodeType::Scalar) {
            properties.add(levelStr, node.as<string>());
//#ifdef DEBUG
//            String str = String::format("%s=%s", levelStr.c_str(), node.as<string>().c_str());
//            Debug::writeLine(str);
//#endif
        } else if (node.Type() == NodeType::Sequence) {
            for (std::size_t i=0;i<node.size();i++) {
                getValueProperties(node[i], i, levelStr, properties);
            }
        }
    }
    void YmlNode::getValueProperties(const Node& node, size_t index, String& levelStr, Properties& properties) const
    {
        if (node.Type() == NodeType::Map) {
          // This should be true; do something here with the map.
            String temp = levelStr;
            levelStr.append(String::format("[%d]", index));
            properties.add(levelStr, String::Empty);
            
            getProperties(node, levelStr, properties);
            levelStr = temp;
        } else if (node.Type() == NodeType::Scalar) {
            String temp = levelStr;
            levelStr.append(String::format("[%d]", index));
            properties.add(levelStr, node.as<string>());
            levelStr = temp;
//#ifdef DEBUG
//            String str = String::format("%s=%s", levelStr.c_str(), node.as<string>().c_str());
//            Debug::writeLine(str);
//#endif
        } else if (node.Type() == NodeType::Sequence) {
            for (std::size_t i=0;i<node.size();i++) {
                getValueProperties(node[i], i, levelStr, properties);
            }
        }
    }
    bool YmlNode::getProperties(Properties& properties) const
    {
        try
        {
            String levelStr;
            getProperties(*_inner, levelStr, properties);
            return true;
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }
    bool YmlNode::at(const String& name, YmlNode& node) const
    {
        try
        {
            const Node& temp = (*_inner)[name.c_str()];
            if(temp.Type() != NodeType::Undefined)
            {
                node._inner->operator=(temp);
                return true;
            }
        }
        catch (const exception& e)
        {
            Trace::error(e.what());
        }
        catch (...)
        {
        }
        return false;
    }
    
    void YmlNode::operator=(const YmlNode& value)
    {
        _attach = value._attach;
        if(value._attach)
        {
            _inner = value._inner;
        }
        else
        {
            _inner->operator=(*value._inner);
        }
    }
    bool YmlNode::operator==(const YmlNode& value) const
    {
        return _inner->is(*value._inner);
    }
    bool YmlNode::operator!=(const YmlNode& value) const
    {
        return !operator==(value);
    }
}
