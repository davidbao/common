//
//  JsonNode.cpp
//  common
//
//  Created by baowei on 2015/7/15.
//  Copyright © 2015 com. All rights reserved.
//

#include "json/JsonNode.h"
#include "libjson/libjson.h"
#include "diag/Trace.h"

namespace Common {
    JsonNode::JsonNode(Type type) : _attach(false) {
        if (type == Type::TypeNode)
            _inner = new JSONNode(JSON_NODE);
        else if (type == Type::TypeArray)
            _inner = new JSONNode(JSON_ARRAY);
        else if (type == Type::TypeNumber)
            _inner = new JSONNode(JSON_NUMBER);
        else if (type == Type::TypeString)
            _inner = new JSONNode(JSON_STRING);
        else if (type == Type::TypeBoolean)
            _inner = new JSONNode(JSON_BOOL);
        else
            _inner = new JSONNode(JSON_NULL);
    }

    JsonNode::JsonNode(const JsonNode &node) : _attach(false) {
        if (node._attach) {
            _inner = node._inner;
            _attach = node._attach;
        } else {
            _inner = new JSONNode();
            _inner->operator=(*node._inner);
        }
    }

    JsonNode::JsonNode(const String &name, Type type) : JsonNode(type) {
        _inner->set_name(name.c_str());
    }

    JsonNode::JsonNode(const String &name, const char *value) : JsonNode(name, String(value)) {
    }

    JsonNode::JsonNode(const String &name, const String &value) : _attach(false) {
        if (value.find('{') == 0 ||
            value.find('[') == 0) {
            _inner = new JSONNode(JSON_NODE);
            _inner->set_name(name.c_str());
            JSONNode node = JSONWorker::parse(value.c_str());
            _inner->operator=(node);
            _inner->set_name(name.c_str());
        } else {
            _inner = new JSONNode(name.c_str(), value);
        }
    }

    JsonNode::JsonNode(const String &name, const bool &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const char &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const int8_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const uint8_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const int16_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const uint16_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const int32_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const uint32_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const int64_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const uint64_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const float &value) : _attach(false) {
        // Fixed bug: NaN and Infinity are unsupported in JavaScript.
        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON
        if (!::isnan(value))
            _inner = new JSONNode(name.c_str(), value);
        else
            _inner = new JSONNode(name.c_str(), String::Empty);
    }

    JsonNode::JsonNode(const String &name, const double &value) : _attach(false) {
        // Fixed bug: NaN and Infinity are unsupported in JavaScript.
        // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON
        if (!::isnan(value))
            _inner = new JSONNode(name.c_str(), value);
        else
            _inner = new JSONNode(name.c_str(), String::Empty);
    }

    JsonNode::JsonNode(const String &name, const StringArray &value) : JsonNode(name, Type::TypeArray) {
        for (size_t i = 0; i < value.count(); i++) {
            add(JsonNode("item", value[i]));
        }
    }

    JsonNode::JsonNode(std::initializer_list<JsonNode> list) : JsonNode(Type::TypeNode) {
        for (const auto &item: list) {
            add(item);
        }
    }

    JsonNode::JsonNode(const String &name, std::initializer_list<JsonNode> list) : JsonNode(name, Type::TypeNode) {
        for (const auto &item: list) {
            add(item);
        }
    }

    JsonNode::JsonNode(JSONNode *node) : _attach(true) {
        _inner = node;
    }

    JsonNode::~JsonNode() {
        if (!_attach) {
            delete _inner;
            _inner = nullptr;
        }
    }

    bool JsonNode::equals(const JsonNode &other) const {
        return _inner->operator==(*other._inner);
    }

    void JsonNode::evaluates(const JsonNode &other) {
        _attach = other._attach;
        if (other._attach) {
            _inner = other._inner;
        } else {
            _inner->operator=(*other._inner);
        }
    }

    JsonNode JsonNode::at(size_t pos) const {
        JsonNode node(JsonNode::TypeNone);
        if (pos < count()) {
            const JSONNode &temp = _inner->at(pos);
            if (temp.type() != JSON_NULL) {
                node._inner->operator=(temp);
            }
        }
        return node;
    }

    JsonNode JsonNode::at(const String &name) const {
        JsonNode node(JsonNode::TypeNone);
        JSONNode::const_iterator iter = _inner->begin();
        for (; iter != _inner->end(); ++iter) {
            const JSONNode &n = *iter;
            if (n.name() == name) {
                node._inner->operator=(n);
                break;
            }
        }
        return node;
    }

    bool JsonNode::getAttribute(const String &name, StringArray &value) const {
        const JSONNode &node = _inner->at(name.c_str());
        if (node.type() == JSON_ARRAY) {
            StringArray texts;
            JSONNode::const_iterator iter = node.begin();
            for (; iter != node.end(); ++iter) {
                const JSONNode &n = *iter;
                String str;
                if (n.type() == JSON_NODE)
                    str = n.write();
                else
                    str = n.as_string();
                texts.add(str);
            }
            value = texts.toString();
            return true;
        }
        return false;
    }

    bool JsonNode::at(size_t pos, JsonNode &node) const {
        node = at(pos);
        return !node.isEmpty();
    }

    bool JsonNode::at(const String &name, JsonNode &node) const {
        node = at(name);
        return !node.isEmpty();
    }

    void JsonNode::add(const JsonNode &node) {
        _inner->push_back(*node._inner);
    }

    String JsonNode::toString(bool format) const {
        return format ? _inner->write_formatted() : _inner->write();
    }

    bool JsonNode::parse(const String &str, JsonNode &node) {
        if (!str.isNullOrEmpty()) {
            JSONNode temp = JSONWorker::parse(str.c_str());
            node._inner->operator=(temp);
            return node.type() != TypeNone;
        }
        return false;
    }

    bool JsonNode::parse(const String &str, StringMap &values) {
        JsonNode node;
        if (JsonNode::parse(str, node)) {
            String value;
            StringArray names;
            node.getAttributeNames(names);
            for (size_t i = 0; i < names.count(); i++) {
                const String &name = names[i];
                if (node.getAttribute(name, value)) {
                    values.add(name, value);
                }
            }
            return true;
        }
        return false;
    }

    void JsonNode::getAttributeNames(StringArray &names) const {
        JSONNode::const_iterator iter = _inner->begin();
        for (; iter != _inner->end(); ++iter) {
            const JSONNode &n = *iter;
            String str;
            if (n.type() == JSON_NODE)
                str = n.write();
            else
                str = n.name();
            names.add(str);
        }
    }

    bool JsonNode::getAttribute(const String &name, String &value) const {
        const JSONNode &node = _inner->at(name.c_str());
        if (node.type() == JSON_NULL) {
            return false;
        } else if (node.type() == JSON_NODE)
            value = node.write();
        else if (node.type() == JSON_ARRAY) {
            StringArray texts;
            JSONNode::const_iterator iter = node.begin();
            for (; iter != node.end(); ++iter) {
                const JSONNode &n = *iter;
                String str;
                if (n.type() == JSON_NODE)
                    str = n.write();
                else
                    str = n.as_string();
                texts.add(str);
            }
            value = texts.toString();
        } else {
            value = node.as_string();
        }
        return true;
    }

    bool JsonNode::getAttribute(StringArray &value) const {
        if (_inner->type() == JSON_ARRAY) {
            JSONNode::const_iterator iter = _inner->begin();
            for (; iter != _inner->end(); ++iter) {
                const JSONNode &n = *iter;
                String str;
                if (n.type() == JSON_NODE)
                    str = n.write();
                else
                    str = n.as_string();
                value.add(str);
            }
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(StringMap &value) const {
        StringArray names;
        getAttributeNames(names);
        for (size_t i = 0; i < names.count(); i++) {
            const String &name = names[i];
            String v;
            if (getAttribute(name, v)) {
                value.add(name, v);
            }
        }
        return value.count() > 0;
    }

    bool JsonNode::hasAttribute(const String &name) const {
        String value;
        return getAttribute(name, value);
    }

    bool JsonNode::hasAttribute() const {
        StringArray names;
        getAttributeNames(names);
        return names.count() > 0;
    }

    bool JsonNode::subNodes(JsonNodes &nodes) const {
        JSONNode::iterator iter = _inner->begin();
        for (; iter != _inner->end(); ++iter) {
            JSONNode &n = *iter;
            if (n.type() == JSON_NODE) {
                nodes.add(JsonNode(&n));
            }
        }
        return nodes.count() > 0;
    }

    bool JsonNode::hasSubNodes() const {
        return !_inner->empty();
    }

    String JsonNode::name() const {
        return _inner->name();
    }

    void JsonNode::setName(const String &name) {
        _inner->set_name(name.c_str());
    }

    JsonNode::Type JsonNode::type() const {
        switch (_inner->type()) {
            case JSON_NODE:
                return TypeNode;
            case JSON_ARRAY:
                return TypeArray;
            case JSON_NULL:
                return TypeNone;
            case JSON_STRING:
                return TypeString;
            case JSON_NUMBER:
                return TypeNumber;
            case JSON_BOOL:
                return TypeBoolean;
            default:
                return TypeNone;
        }
    }

    String JsonNode::value() const {
        return _inner->as_string();
    }

    void JsonNode::setValue(const String &value) {
        _inner->operator=(value.c_str());
    }

    String JsonNode::comment() const {
        return _inner->get_comment();
    }

    void JsonNode::setComment(const String &comment) {
        _inner->set_comment(comment.c_str());
    }

    size_t JsonNode::count() const {
        return (size_t) _inner->size();
    }

    bool JsonNode::isEmpty() const {
        return type() == TypeNone;
    }

    JsonNode &JsonNode::operator=(const JsonNode &value) {
        if (this != &value) {
            JsonNode::evaluates(value);
        }
        return *this;
    }

    bool JsonNode::copyTo(YmlNode &node) const {
        if (_inner->type() == JSON_ARRAY) {
            copyArrayTo(_inner, "array", node);
        } else {
            JSONNode::const_iterator iter = _inner->begin();
            for (; iter != _inner->end(); ++iter) {
                const JSONNode &n = *iter;
                const String name = n.name();
                JSONNode &jsonNode = _inner->at(name.c_str());
                if (jsonNode.type() == JSON_NODE) {
                    JsonNode subNode(&jsonNode);
                    YmlNode ymlNode;
                    subNode.copyTo(ymlNode);
                    node.add(name, ymlNode);
                } else if (jsonNode.type() == JSON_ARRAY) {
                    copyArrayTo(&jsonNode, name, node);
                } else {
                    node.setAttribute(name, String(jsonNode.as_string()));
                }
            }
        }
        return true;
    }

    void JsonNode::copyArrayTo(JSONNode *jsonNode, const String &name, YmlNode &node) {
        YmlNode ymlNodes(YmlNode::TypeSequence);
        JSONNode::const_iterator iter = jsonNode->begin();
        for (; iter != jsonNode->end(); ++iter) {
            const JSONNode &n = *iter;
            if (n.type() == JSON_NODE) {
                JsonNode subNode((JSONNode *) &n);
                YmlNode ymlNode;
                subNode.copyTo(ymlNode);
                ymlNodes.add(ymlNode);
            } else if (n.type() == JSON_ARRAY) {
                copyArrayTo((JSONNode *) &n, "array", node);
            } else {
                ymlNodes.add(YmlNode(String(n.as_string())));
            }
        }
        node.add(name, ymlNodes);
    }
}
