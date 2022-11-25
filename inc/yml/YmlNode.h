//
//  YmlNode.h
//  common
//
//  Created by baowei on 2020/3/2.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef YmlNode_h
#define YmlNode_h

#include "data/ValueType.h"
#include "data/Uuid.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Rectangle.h"
#include "data/Point.h"
#include "data/Size.h"
#include "data/Vector.h"
#include "data/PList.h"
#include "data/Dictionary.h"
#include "data/StringMap.h"
#include "yaml-cpp/yaml.h"

namespace Common
{
    class YmlNode
    {
    public:
        enum Type
        {
            TypeUndefined, TypeNull, TypeScalar, TypeSequence, TypeMap
        };
        
        typedef StringMap Properties;
        
        YmlNode(Type type = Type::TypeUndefined);
        YmlNode(const YmlNode& node);
        YmlNode(const bool& value);
        YmlNode(const char& value);
        YmlNode(const uint8_t& value);
        YmlNode(const short& value);
        YmlNode(const uint16_t& value);
        YmlNode(const int& value);
        YmlNode(const uint32_t& value);
        YmlNode(const int64_t& value);
        YmlNode(const uint64_t& value);
        YmlNode(const float& value);
        YmlNode(const double& value);
        YmlNode(const char* value);
        YmlNode(const String& value);

        template <class T>
        YmlNode(const T& value) : YmlNode(value.toString())
        {
        }
        template <class T>
        YmlNode(const Vector<T>& value) : YmlNode(Type::TypeMap)
        {
            for (uint32_t i=0; i<value.count(); i++)
            {
                add(YmlNode("item", value[i]));
            }
        }
        template <class T>
        YmlNode(const PList<T>& value) : YmlNode(Type::TypeMap)
        {
            for (uint32_t i=0; i<value.count(); i++)
            {
                add(YmlNode("item", value[i]));
            }
        }
        
        ~YmlNode();
        
        void add(const YmlNode& node);
        void add(const String& name, const YmlNode& value);
        
        String toString() const;

        bool getAttribute(const String& name, String& value) const;
        template <class T>
        bool getAttribute(const String& name, T& value) const
        {
            String str;
            return getAttribute(name, str) && T::parse(str, value);
        }
        bool getAttribute(const String& name, bool& value) const;
        bool getAttribute(const String& name, uint8_t& value) const;
        bool getAttribute(const String& name, int& value) const;
        bool getAttribute(const String& name, uint32_t& value) const;
        bool getAttribute(const String& name, int64_t& value) const;
        bool getAttribute(const String& name, uint64_t& value) const;
        bool getAttribute(const String& name, float& value) const;
        bool getAttribute(const String& name, double& value) const;
        template <class T>
        bool getAttribute(const String& name, Vector<T>& value) const
        {
            StringArray texts;
            if(!getAttribute(name, texts))
                return false;
            
            for (uint32_t i=0; i<texts.count(); i++)
            {
                T item;
                if(T::parse(texts[i], item))
                {
                    value.add(item);
                }
            }
            return value.count() == texts.count();
        }
        
        bool setAttribute(const String& name, const String& value);
        template <class T>
        bool setAttribute(const String& name, const T& value)
        {
            return setAttribute(name, value.toString());
        }
        bool setAttribute(const String& name, const bool& value);
        bool setAttribute(const String& name, const uint8_t& value);
        bool setAttribute(const String& name, const int& value);
        bool setAttribute(const String& name, const uint32_t& value);
        bool setAttribute(const String& name, const int64_t& value);
        bool setAttribute(const String& name, const uint64_t& value);
        bool setAttribute(const String& name, const float& value);
        bool setAttribute(const String& name, const double& value);
        void setAttributes(const StringMap& values);

        bool getProperties(Properties& properties) const;
        bool at(const String& name, YmlNode& node) const;
        
        Type type() const;
        size_t size() const;
        bool isEmpty() const;
        
        void operator=(const YmlNode& value);
        bool operator==(const YmlNode& value) const;
        bool operator!=(const YmlNode& value) const;
        
    private:
        YmlNode(YAML::Node* node);
        
        void getProperties(const YAML::Node& node, String& levelStr, Properties& properties) const;
        void getValueProperties(const YAML::Node& node, String& levelStr, Properties& properties) const;
        void getValueProperties(const YAML::Node& node, size_t index, String& levelStr, Properties& properties) const;
        
    public:
        static bool parse(const String& str, YmlNode& node);
        static bool parseFile(const String& fileName, YmlNode& node);
        
        static bool updateFile(const String& fileName, const Properties& properties);
        
    private:
        YAML::Node* _inner;
        bool _attach;
    };
}

#endif /* YmlNode_h */
