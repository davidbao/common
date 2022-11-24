//
//  JsonNode.h
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef JsonNode_h
#define JsonNode_h

#include "data/ValueType.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Rectangle.h"
#include "data/Point.h"
#include "data/Size.h"
#include "data/Vector.h"
#include "data/PList.h"
#include "data/StringArray.h"
#include "yml/YmlNode.h"

class JSONNode;

namespace Common {
    class JsonTextReader;

    class JsonNode {
    public:
        enum Type {
            TypeNone,
            TypeNode,
            TypeArray,
            TypeString,
            TypeNumber,
            TypeBoolean
        };

        struct KeyValue {
            const String key;
            const String value;
        };

        JsonNode(Type type = Type::TypeNode);

        JsonNode(const JsonNode &node);

        JsonNode(const String &name, Type type = Type::TypeNode);

        JsonNode(const String &name, const bool &value);

        JsonNode(const String &name, const char &value);

        JsonNode(const String &name, const uint8_t &value);

        JsonNode(const String &name, const short &value);

        JsonNode(const String &name, const uint16_t &value);

        JsonNode(const String &name, const int &value);

        JsonNode(const String &name, const uint32_t &value);

        JsonNode(const String &name, const int64_t &value);

        JsonNode(const String &name, const uint64_t &value);

        JsonNode(const String &name, const float &value);

        JsonNode(const String &name, const double &value);

        JsonNode(const String &name, const char *value);

        JsonNode(const String &name, const String &value);

        JsonNode(const String &name, const DateTime &value, DateTime::Format format = DateTime::Format::YYYYMMDDHHMMSS);

//        JsonNode(const String &name, const TimeSpan &value, TimeSpan::Format format = TimeSpan::Format::HHMMSS);

        template<class T>
        JsonNode(const String &name, const T &value) : JsonNode(name, value.toString()) {
        }

        template<class T>
        JsonNode(const String &name, const Vector<T> &value) : JsonNode(name, Type::TypeArray) {
            for (uint32_t i = 0; i < value.count(); i++) {
                add(JsonNode("item", value[i]));
            }
        }

        template<class T>
        JsonNode(const String &name, const PList<T> &value) : JsonNode(name, Type::TypeArray) {
            for (uint32_t i = 0; i < value.count(); i++) {
                add(JsonNode("item", value[i]));
            }
        }

        JsonNode(const String &name, const StringArray &value);

        JsonNode(const KeyValue *item);

        ~JsonNode();

        void add(const JsonNode &node);

        String toString(bool format = false) const;

        bool getAttributeNames(StringArray &names) const;

        bool getAttribute(const String &name, String &value) const;

        template<class T>
        bool getAttribute(const String &name, T &value) const {
            String str;
            return getAttribute(name, str) && T::parse(str, value);
        }

        bool getAttribute(const String &name, bool &value) const;

        bool getAttribute(const String &name, uint8_t &value) const;

        bool getAttribute(const String &name, short &value) const;

        bool getAttribute(const String &name, uint16_t &value) const;

        bool getAttribute(const String &name, int &value) const;

        bool getAttribute(const String &name, uint32_t &value) const;

        bool getAttribute(const String &name, int64_t &value) const;

        bool getAttribute(const String &name, uint64_t &value) const;

        bool getAttribute(const String &name, float &value) const;

        bool getAttribute(const String &name, double &value) const;

        bool getAttribute(const String &name, StringArray &value) const;

        bool getAttribute(StringArray &value) const;

        bool getAttribute(StringMap &value) const;

        template<class T>
        bool getAttribute(const String &name, Vector<T> &value) const {
            StringArray texts;
            if (!getAttribute(name, texts))
                return false;

            for (uint32_t i = 0; i < texts.count(); i++) {
                T item;
                if (T::parse(texts[i], item)) {
                    value.add(item);
                }
            }
            return value.count() == texts.count();
        }

        bool hasAttribute(const String &name) const;

        bool hasAttribute() const;

        bool subNodes(PList<JsonNode> &nodes) const;

        bool hasSubNodes() const;

        bool atByName(const String &name, JsonNode &node) const;

        bool at(int i, JsonNode &node) const;

        String name() const;

        void setName(const String &name);

        Type type() const;

        uint32_t size() const;

        bool isEmpty() const;

        void operator=(const JsonNode &value);

        bool operator==(const JsonNode &value) const;

        bool operator!=(const JsonNode &value) const;

        void copyTo(YmlNode &node) const;

    private:
        JsonNode(JSONNode *node);

    public:
        static bool parse(const String &str, JsonNode &node);

        static bool parse(const String &str, StringMap &values);

    private:
        static void copyArrayTo(JSONNode *jnode, const String &name, YmlNode &node);

    private:
        friend JsonTextReader;

        JSONNode *_inner;
        bool _attach;
    };
}
#endif // JsonNode_h
