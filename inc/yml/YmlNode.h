//
//  YmlNode.h
//  common
//
//  Created by baowei on 2020/3/2.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef YmlNode_h
#define YmlNode_h

#include "data/String.h"
#include "data/Vector.h"
#include "data/List.h"
#include "data/Dictionary.h"
#include "data/StringMap.h"
#include "data/IAttribute.h"

namespace Yml {
    class YmlNodeInner;

    class YmlNode
            : public IEquatable<YmlNode>,
              public IEvaluation<YmlNode>,
              public IIndexGetter<YmlNode>,
              public IPositionGetter<YmlNode, const String &>,
              public IAttributeGetter,
              public IAttributeSetter {
    public:
        using IAttributeGetter::getAttribute;
        using IIndexGetter<YmlNode>::operator[];
        using IPositionGetter<YmlNode, const String &>::operator[];

        enum Type {
            TypeUndefined, TypeNull, TypeScalar, TypeSequence, TypeMap
        };

        typedef StringMap Properties;

        explicit YmlNode(Type type = Type::TypeUndefined);

        YmlNode(const YmlNode &node);

        explicit YmlNode(const bool &value);

        explicit YmlNode(const char &value);

        explicit YmlNode(const int8_t &value);

        explicit YmlNode(const uint8_t &value);

        explicit YmlNode(const int16_t &value);

        explicit YmlNode(const uint16_t &value);

        explicit YmlNode(const int32_t &value);

        explicit YmlNode(const uint32_t &value);

        explicit YmlNode(const int64_t &value);

        explicit YmlNode(const uint64_t &value);

        explicit YmlNode(const float &value);

        explicit YmlNode(const double &value);

        explicit YmlNode(const char *value);

        explicit YmlNode(const String &value);

        template<class T>
        explicit YmlNode(const T &value) : YmlNode(value.toString()) {
        }

        template<class T>
        explicit YmlNode(const Vector<T> &value) : YmlNode(Type::TypeSequence) {
            for (size_t i = 0; i < value.count(); i++) {
                add(YmlNode(value[i]));
            }
        }

        template<class T>
        explicit YmlNode(const List<T> &value) : YmlNode(Type::TypeSequence) {
            for (size_t i = 0; i < value.count(); i++) {
                add(YmlNode(value[i]));
            }
        }

        ~YmlNode() override;

        bool equals(const YmlNode &other) const override;

        void evaluates(const YmlNode &other) override;

        YmlNode at(size_t pos) const override;

        YmlNode at(const String &name) const override;

        bool at(size_t pos, YmlNode &node) const;

        bool at(const String &name, YmlNode &node) const;

        bool getAttribute(const String &name, String &value) const override;

        bool setAttribute(const String &name, const String &value) override;

        void setAttributes(const StringMap &values);

        YmlNode &operator=(const YmlNode &value);

        String toString() const;

        Type type() const;

        size_t size() const;

        bool isEmpty() const;

        void add(const YmlNode &node);

        void add(const String &name, const YmlNode &node);

        bool getProperties(Properties &properties) const;

        operator String() const;

    private:
        void getProperties(const YmlNodeInner &node, String &levelStr, Properties &properties) const;

        void getValueProperties(const YmlNodeInner &node, String &levelStr, Properties &properties) const;

        void getValueProperties(const YmlNodeInner &node, size_t index, String &levelStr, Properties &properties) const;

    public:
        static bool parse(const String &str, YmlNode &node);

        static bool parseFile(const String &fileName, YmlNode &node);

        static bool updateFile(const String &fileName, const Properties &properties);

    private:
        YmlNodeInner *_node;
        bool _attach;
    };
}

#endif // YmlNode_h
