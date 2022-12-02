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
#include "data/Vector.h"
#include "data/List.h"
#include "data/StringArray.h"
#include "yml/YmlNode.h"

class JSONNode;

namespace Common {
    class JsonTextReader;

    class JsonNode;

    typedef List<JsonNode> JsonNodes;

    class JsonNode
            : public IEquatable<JsonNode>,
              public IEvaluation<JsonNode>,
              public IIndexGetter<JsonNode>,
              public IPositionGetter<JsonNode, const String &> {
    public:
        enum Type {
            TypeNone = 0,
            TypeString = 1,
            TypeNumber = 2,
            TypeBoolean = 3,
            TypeArray = 4,
            TypeNode = 5
        };

        JsonNode(Type type = Type::TypeNode);

        JsonNode(const JsonNode &node);

        JsonNode(const String &name, Type type = Type::TypeNode);

        JsonNode(const String &name, const bool &value);

        JsonNode(const String &name, const char &value);

        JsonNode(const String &name, const int8_t &value);

        JsonNode(const String &name, const uint8_t &value);

        JsonNode(const String &name, const int16_t &value);

        JsonNode(const String &name, const uint16_t &value);

        JsonNode(const String &name, const int32_t &value);

        JsonNode(const String &name, const uint32_t &value);

        JsonNode(const String &name, const int64_t &value);

        JsonNode(const String &name, const uint64_t &value);

        JsonNode(const String &name, const float &value);

        JsonNode(const String &name, const double &value);

        JsonNode(const String &name, const char *value);

        JsonNode(const String &name, const String &value);

        template<class T>
        JsonNode(const String &name, const T &value) : JsonNode(name, value.toString()) {
        }

        template<class T>
        JsonNode(const String &name, const Vector<T> &value) : JsonNode(name, Type::TypeArray) {
            for (size_t i = 0; i < value.count(); i++) {
                add(JsonNode("item", value[i]));
            }
        }

        template<class T>
        JsonNode(const String &name, const List<T> &value) : JsonNode(name, Type::TypeArray) {
            for (size_t i = 0; i < value.count(); i++) {
                add(JsonNode("item", value[i]));
            }
        }

        JsonNode(const String &name, const StringArray &value);

        JsonNode(std::initializer_list<JsonNode> list);

        JsonNode(const String &name, std::initializer_list<JsonNode> list);

        ~JsonNode() override;

        bool equals(const JsonNode &other) const override;

        void evaluates(const JsonNode &other) override;

        JsonNode at(size_t pos) const override;

        JsonNode at(const String &name) const override;

        bool at(size_t pos, JsonNode &node) const;

        bool at(const String &name, JsonNode &node) const;

        void add(const JsonNode &node);

        String toString(bool format = false) const;

        void getAttributeNames(StringArray &names) const;

        bool getAttribute(const String &name, String &value) const;

        template<class T>
        bool getAttribute(const String &name, T &value) const {
            String str;
            return getAttribute(name, str) && T::parse(str, value);
        }

        bool getAttribute(const String &name, bool &value) const;

        bool getAttribute(const String &name, char &value) const;

        bool getAttribute(const String &name, int8_t &value) const;

        bool getAttribute(const String &name, uint8_t &value) const;

        bool getAttribute(const String &name, int16_t &value) const;

        bool getAttribute(const String &name, uint16_t &value) const;

        bool getAttribute(const String &name, int32_t &value) const;

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

            for (size_t i = 0; i < texts.count(); i++) {
                T item;
                if (T::parse(texts[i], item)) {
                    value.add(item);
                }
            }
            return value.count() == texts.count();
        }

        template<class T>
        bool getAttribute(const String &name, List<T> &value) const {
            StringArray texts;
            if (!getAttribute(name, texts))
                return false;

            for (size_t i = 0; i < texts.count(); i++) {
                T item;
                if (T::parse(texts[i], item)) {
                    value.add(item);
                }
            }
            return value.count() == texts.count();
        }

        bool hasAttribute(const String &name) const;

        bool hasAttribute() const;

        bool subNodes(JsonNodes &nodes) const;

        bool hasSubNodes() const;

        String name() const;

        void setName(const String &name);

        Type type() const;

        String value() const;

        void setValue(const String &value);

        String comment() const;

        void setComment(const String &comment);

        size_t count() const;

        bool isEmpty() const;

        JsonNode &operator=(const JsonNode &value);

        bool copyTo(YmlNode &node) const;

    private:
        explicit JsonNode(JSONNode *node);

    public:
        static bool parse(const String &str, JsonNode &node);

        static bool parse(const String &str, StringMap &values);

    private:
        static void copyArrayTo(JSONNode *jsonNode, const String &name, YmlNode &node);

    private:
        friend JsonTextReader;

        JSONNode *_inner;
        bool _attach;
    };
}
#endif // JsonNode_h
