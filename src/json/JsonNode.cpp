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

    JsonNode::JsonNode(const String &name, const char *value) : JsonNode(name, (String) value) {
    }

    JsonNode::JsonNode(const String &name, const String &value) : _attach(false) {
        if (value.find('{') == 0 ||
            value.find('[') == 0) {
            try {
                _inner = new JSONNode(JSON_NODE);
                _inner->set_name(name.c_str());
                JSONNode node = JSONWorker::parse(value.c_str());
                _inner->operator=(node);
                _inner->set_name(name.c_str());
            }
            catch (...) {
                _inner = new JSONNode(name.c_str(), value);
            }
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

    JsonNode::JsonNode(const String &name, const uint8_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const short &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const uint16_t &value) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value);
    }

    JsonNode::JsonNode(const String &name, const int &value) : _attach(false) {
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
        for (uint32_t i = 0; i < value.count(); i++) {
            add(JsonNode("item", value[i]));
        }
    }

    JsonNode::JsonNode(const String &name, const DateTime &value, DateTime::Format format) : _attach(false) {
        _inner = new JSONNode(name.c_str(), value.toString(format));
    }

//    JsonNode::JsonNode(const String &name, const TimeSpan &value, TimeSpan::Format format) : _attach(false) {
//        _inner = new JSONNode(name.c_str(), value.toString(format));
//    }

    JsonNode::JsonNode(const KeyValue *item) : JsonNode(TypeNode) {
        const KeyValue *kv = item;
        while (kv != NULL && !kv->key.isNullOrEmpty()) {
            if (kv != NULL) {
                add(JsonNode(kv->key, kv->value));
            }
            kv++;
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

    void JsonNode::add(const JsonNode &node) {
        _inner->push_back(*node._inner);
    }

    String JsonNode::toString(bool format) const {
        return format ? _inner->write_formatted() : _inner->write();
    }

    bool JsonNode::parse(const String &str, JsonNode &node) {
        try {
            if (!str.isNullOrEmpty()) {
                JSONNode temp = JSONWorker::parse(str.c_str());
                node._inner->operator=(temp);
                return true;
            }
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool JsonNode::parse(const String &str, StringMap &values) {
        JsonNode node;
        if (JsonNode::parse(str, node)) {
            String value;
            StringArray names;
            if (node.getAttributeNames(names)) {
                for (uint32_t i = 0; i < names.count(); i++) {
                    const String &name = names[i];
                    if (node.getAttribute(name, value)) {
                        values.add(name, value);
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool JsonNode::getAttributeNames(StringArray &names) const {
        try {
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
            return true;
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, String &value) const {
        try {
            JSONNode &node = _inner->at(name.c_str());
            if (node.type() == JSON_NODE)
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
            } else
                value = node.as_string();
            return true;
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, bool &value) const {
        Boolean result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, uint8_t &value) const {
        Byte result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, short &value) const {
        Int16 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, uint16_t &value) const {
        UInt16 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, int &value) const {
        Int32 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, uint32_t &value) const {
        UInt32 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, int64_t &value) const {
        Int64 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, uint64_t &value) const {
        UInt64 result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, float &value) const {
        Float result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, double &value) const {
        Double result;
        if (getAttribute(name, result)) {
            value = result;
            return true;
        }
        return false;
    }

    bool JsonNode::getAttribute(const String &name, StringArray &value) const {
        try {
            JSONNode &node = _inner->at(name.c_str());
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
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
        return false;
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
        if (getAttributeNames(names)) {
            for (uint32_t i = 0; i < names.count(); i++) {
                const String &name = names[i];
                String v;
                if (getAttribute(name, v)) {
                    value.add(name, v);
                }
            }
            return value.count() > 0;
        }
        return false;
    }

    bool JsonNode::hasAttribute(const String &name) const {
        String value;
        return getAttribute(name, value);
    }

    bool JsonNode::hasAttribute() const {
        StringArray names;
        return getAttributeNames(names) && names.count() > 0;
    }

    bool JsonNode::subNodes(PList<JsonNode> &nodes) const {
        JSONNode::iterator iter = _inner->begin();
        for (; iter != _inner->end(); ++iter) {
            JSONNode &n = *iter;
            if (n.type() == JSON_NODE) {
                JsonNode *node = new JsonNode(&n);
                nodes.add(node);
            }
        }
        return nodes.count() > 0;
    }

    bool JsonNode::hasSubNodes() const {
        return _inner->size() > 0;
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

    uint32_t JsonNode::size() const {
        return _inner->size();
    }

    bool JsonNode::isEmpty() const {
        return type() == TypeNone;
    }

    bool JsonNode::at(int i, JsonNode &node) const {
        if (i < (int) size()) {
            try {
                JSONNode &temp = _inner->at(i);
                node._inner->operator=(temp);
                return true;
            }
            catch (const exception &e) {
                Trace::debug(e.what());
            }
            catch (...) {
            }
        }
        return false;
    }

    bool JsonNode::atByName(const String &name, JsonNode &node) const {
        try {
            JSONNode &temp = _inner->at(name.c_str());
            node._inner->operator=(temp);
            return true;
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
        return false;
    }

    void JsonNode::operator=(const JsonNode &value) {
        _attach = value._attach;
        if (value._attach) {
            _inner = value._inner;
        } else {
            _inner->operator=(*value._inner);
        }
    }

    bool JsonNode::operator==(const JsonNode &value) const {
        return _inner->operator==(*value._inner);
    }

    bool JsonNode::operator!=(const JsonNode &value) const {
        return !operator==(value);
    }

    void JsonNode::copyTo(YmlNode &node) const {
        try {
            if (_inner->type() == JSON_ARRAY) {
                copyArrayTo(_inner, "array", node);
            } else {
                JSONNode::const_iterator iter = _inner->begin();
                for (; iter != _inner->end(); ++iter) {
                    const JSONNode &n = *iter;
                    const String name = n.name();
                    JSONNode &jnode = _inner->at(name.c_str());
                    if (jnode.type() == JSON_NODE) {
                        JsonNode subNode(&jnode);
                        YmlNode ynode;
                        subNode.copyTo(ynode);
                        node.add(name, ynode);
                    } else if (jnode.type() == JSON_ARRAY) {
                        copyArrayTo(&jnode, name, node);
                    } else {
                        node.setAttribute(name, String(jnode.as_string()));
                    }
                }
            }
        }
        catch (const exception &e) {
            Trace::debug(e.what());
        }
        catch (...) {
        }
    }

    void JsonNode::copyArrayTo(JSONNode *jnode, const String &name, YmlNode &node) {
        YmlNode ysnode(YmlNode::TypeSequence);
        JSONNode::const_iterator iter = jnode->begin();
        for (; iter != jnode->end(); ++iter) {
            const JSONNode &n = *iter;
            if (n.type() == JSON_NODE) {
                JsonNode subNode((JSONNode *) &n);
                YmlNode ynode;
                subNode.copyTo(ynode);
                ysnode.add(ynode);
            } else if (n.type() == JSON_ARRAY) {
                copyArrayTo((JSONNode *) &n, "array", node);
            } else {
                ysnode.add(YmlNode(String(n.as_string())));
            }
        }
        node.add(name, ysnode);
    }
}
