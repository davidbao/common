//
//  YmlNode.cpp
//  common
//
//  Created by baowei on 2020/3/2.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "yml/YmlNode.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include <map>

using namespace Diag;

namespace Yml {
    class YmlNodeInner {
    public:
        YAML::Node *node;

        explicit YmlNodeInner(YAML::Node *node = nullptr) {
            this->node = node;
        }

        explicit YmlNodeInner(const YAML::Node *node) {
            this->node = (YAML::Node *) node;
        }
    };

    YmlNode::YmlNode(Type type) : _attach(false) {
        YAML::Node *node;
        if (type == Type::TypeUndefined)
            node = new YAML::Node(YAML::NodeType::Undefined);
        else if (type == Type::TypeNull)
            node = new YAML::Node(YAML::NodeType::Null);
        else if (type == Type::TypeScalar)
            node = new YAML::Node(YAML::NodeType::Scalar);
        else if (type == Type::TypeSequence)
            node = new YAML::Node(YAML::NodeType::Sequence);
        else if (type == Type::TypeMap)
            node = new YAML::Node(YAML::NodeType::Map);
        else
            node = new YAML::Node();
        _node = new YmlNodeInner(node);
    }

    YmlNode::YmlNode(const YmlNode &node) : _attach(false) {
        if (node._attach) {
            _node = new YmlNodeInner(node._node->node);
            _attach = node._attach;
        } else {
            _node = new YmlNodeInner(new YAML::Node());
            _node->node->operator=(*node._node->node);
        }
    }

    YmlNode::YmlNode(const char *value) : YmlNode((String) value) {
    }

    YmlNode::YmlNode(const String &value) : _attach(false) {
        _node = new YmlNodeInner(new YAML::Node(value.c_str()));
    }

    YmlNode::YmlNode(const bool &value) : YmlNode(((Boolean) value).toString()) {
    }

    YmlNode::YmlNode(const char &value) : YmlNode(((Char) value).toString()) {
    }

    YmlNode::YmlNode(const int8_t &value) : YmlNode(((Int8) value).toString()) {
    }

    YmlNode::YmlNode(const uint8_t &value) : YmlNode(((UInt8) value).toString()) {
    }

    YmlNode::YmlNode(const int16_t &value) : YmlNode(((Int16) value).toString()) {
    }

    YmlNode::YmlNode(const uint16_t &value) : YmlNode(((UInt16) value).toString()) {
    }

    YmlNode::YmlNode(const int32_t &value) : YmlNode(((Int32) value).toString()) {
    }

    YmlNode::YmlNode(const uint32_t &value) : YmlNode(((UInt32) value).toString()) {
    }

    YmlNode::YmlNode(const int64_t &value) : YmlNode(((Int64) value).toString()) {
    }

    YmlNode::YmlNode(const uint64_t &value) : YmlNode(((UInt64) value).toString()) {
    }

    YmlNode::YmlNode(const float &value) : YmlNode(((Float) value).toString()) {
    }

    YmlNode::YmlNode(const double &value) : YmlNode(((Double) value).toString()) {
    }

//    YmlNode::YmlNode(Node *node) : _attach(true) {
//        _node->node = node;
//    }

    YmlNode::~YmlNode() {
        if (!_attach) {
            delete _node->node;
            _node->node = nullptr;
        }
        delete _node;
        _node = nullptr;
    }

    bool YmlNode::equals(const YmlNode &other) const {
        return _node->node->is(*other._node->node);
    }

    void YmlNode::evaluates(const YmlNode &other) {
        _attach = other._attach;
        if (other._attach) {
            _node->node = other._node->node;
        } else {
            _node->node->operator=(*other._node->node);
        }
    }

    YmlNode YmlNode::at(size_t pos) const {
        YmlNode node(Type::TypeUndefined);
        at(pos, node);
        return node;
    }

    YmlNode YmlNode::at(const String &name) const {
        YmlNode node(Type::TypeUndefined);
        at(name, node);
        return node;
    }

    bool YmlNode::at(size_t pos, YmlNode &node) const {
        try {
            YAML::Node *yNode = _node->node;
            if (pos < size()) {
                if (yNode->IsMap()) {
                    int index = 0;
                    auto it = yNode->begin();
                    for (; it != yNode->end(); ++it) {
                        const YAML::Node &key = it->first;
                        const YAML::Node &value = it->second;
                        if (index == pos) {
                            node._node->node->operator=(value);
                            return true;
                        }
                        index++;
                    }
                } else if (yNode->IsSequence()) {
                    node._node->node->operator=((*yNode)[pos]);
                    return true;
                }
            }
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool YmlNode::at(const String &name, YmlNode &node) const {
        try {
            YAML::Node *yNode = _node->node;
            if (yNode->IsMap()) {
                const YAML::Node &temp = (*yNode)[name.c_str()];
                if (temp.Type() != YAML::NodeType::Undefined) {
                    node._node->node->operator=(temp);
                    return true;
                }
            }
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool YmlNode::getAttribute(const String &name, String &value) const {
        try {
            const YAML::Node &node = _node->node->operator[](name.c_str());
            value = node.as<std::string>();
            return true;
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool YmlNode::setAttribute(const String &name, const String &value) {
        try {
            YAML::Node &node = *_node->node;
            node[name.c_str()] = value.c_str();
            return true;
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    void YmlNode::setAttributes(const StringMap &values) {
        StringArray keys;
        values.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (values.at(key, value)) {
                setAttribute(key, value);
            }
        }
    }

    YmlNode &YmlNode::operator=(const YmlNode &value) {
        if (this != &value) {
            YmlNode::evaluates(value);
        }
        return *this;
    }

    String YmlNode::toString() const {
        String str;
        try {
            YAML::Emitter out;
            out << *_node->node;
            str = out.c_str();
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        return str;
    }

    YmlNode::Type YmlNode::type() const {
        switch (_node->node->Type()) {
            case YAML::NodeType::Undefined:
                return TypeUndefined;
            case YAML::NodeType::Null:
                return TypeNull;
            case YAML::NodeType::Sequence:
                return TypeSequence;
            case YAML::NodeType::Scalar:
                return TypeScalar;
            case YAML::NodeType::Map:
                return TypeMap;
            default:
                return TypeUndefined;
        }
    }

    size_t YmlNode::size() const {
        return _node->node->size();
    }

    bool YmlNode::isEmpty() const {
        return type() == TypeUndefined || type() == TypeNull;
    }

    void YmlNode::add(const YmlNode &node) {
        try {
            _node->node->push_back(*node._node->node);
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
    }

    void YmlNode::add(const String &name, const YmlNode &node) {
        try {
            YAML::Node &yNode = *_node->node;
            yNode[name.c_str()] = *node._node->node;
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
    }

    bool YmlNode::getProperties(Properties &properties) const {
        try {
            String levelStr;
            getProperties(*_node, levelStr, properties);
            return true;
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    void YmlNode::getProperties(const YmlNodeInner &node, String &levelStr, Properties &properties) const {
        String tempLevelStr;
        auto it = node.node->begin();
        for (; it != node.node->end(); ++it) {
            tempLevelStr = levelStr;

            const YAML::Node key = it->first;
            const YAML::Node value = it->second;
            if (key.Type() == YAML::NodeType::Scalar) {
                // This should be true; do something here with the scalar key.
                if (!levelStr.isNullOrEmpty())
                    levelStr.append('.');
                levelStr.append(key.as<string>());
                properties.add(levelStr, String::Empty);
                if (value.Type() == YAML::NodeType::Scalar) {
                    value.as<string>(); // Fixed BUG: crash without value invoked.
                }
//#ifdef DEBUG
//                String str = String::format("%s=%s", levelStr.c_str(), String::Empty.c_str());
//                Debug::writeLine(str);
//#endif
            }

            const YmlNodeInner innerNode(&value);
            getValueProperties(innerNode, levelStr, properties);

            levelStr = tempLevelStr;
        }
//#ifdef DEBUG
//        StringArray keys;
//        properties.keys(keys);
//        for (size_t i=0; i<keys.count(); i++)
//        {
//            const String &key = keys[i];
//            String value;
//            properties.at(key, value);
//            String str = String::format("%s=%s", key.c_str(), value.c_str());
//            Debug::writeLine(str);
//        }
//#endif
    }

    void YmlNode::getValueProperties(const YmlNodeInner &node, String &levelStr, Properties &properties) const {
        if (node.node->Type() == YAML::NodeType::Map) {
            // This should be true; do something here with the map.
            getProperties(node, levelStr, properties);
            levelStr.empty();
        } else if (node.node->Type() == YAML::NodeType::Scalar) {
            properties.add(levelStr, node.node->as<string>());
//#ifdef DEBUG
//            String str = String::format("%s=%s", levelStr.c_str(), node.as<string>().c_str());
//            Debug::writeLine(str);
//#endif
        } else if (node.node->Type() == YAML::NodeType::Sequence) {
            for (std::size_t i = 0; i < node.node->size(); i++) {
                const YAML::Node &subNode = (*node.node)[i];
                const YmlNodeInner innerNode(&subNode);
                getValueProperties(innerNode, i, levelStr, properties);
            }
        }
    }

    void YmlNode::getValueProperties(const YmlNodeInner &node, size_t index, String &levelStr,
                                     Properties &properties) const {
        if (node.node->Type() == YAML::NodeType::Map) {
            // This should be true; do something here with the map.
            String temp = levelStr;
            levelStr.append(String::format("[%d]", index));
            properties.add(levelStr, String::Empty);

            getProperties(node, levelStr, properties);
            levelStr = temp;
        } else if (node.node->Type() == YAML::NodeType::Scalar) {
            String temp = levelStr;
            levelStr.append(String::format("[%d]", index));
            properties.add(levelStr, node.node->as<string>());
            levelStr = temp;
//#ifdef DEBUG
//            String str = String::format("%s=%s", levelStr.c_str(), node.as<string>().c_str());
//            Debug::writeLine(str);
//#endif
        } else if (node.node->Type() == YAML::NodeType::Sequence) {
            for (std::size_t i = 0; i < node.node->size(); i++) {
                const YAML::Node &subNode = (*node.node)[i];
                const YmlNodeInner innerNode(&subNode);
                getValueProperties(innerNode, i, levelStr, properties);
            }
        }
    }

    bool YmlNode::parse(const String &str, YmlNode &node) {
        try {
            YAML::Node temp = YAML::Load(str.c_str());
            if (temp.Type() != YAML::NodeType::Undefined) {
                node._node->node->operator=(temp);
                return true;
            }
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool YmlNode::parseFile(const String &fileName, YmlNode &node) {
        try {
            YAML::Node temp = YAML::LoadFile(fileName.c_str());
            if (temp.Type() != YAML::NodeType::Undefined) {
                node._node->node->operator=(temp);
                return true;
            }
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    bool YmlNode::loadFile(const String &fileName, Properties &properties) {
        if (File::exists(fileName)) {
            YmlNode node;
            if (parseFile(fileName, node)) {
                return node.getProperties(properties);
            }
        }
        return false;
    }

    bool YmlNode::updateFile(const String &fileName, const Properties &properties) {
        Properties all;
        if (loadFile(fileName, all)) {
            all.addRange(properties);   // replace if contained.
            return saveFile(fileName, all);
        } else {
            return saveFile(fileName, properties);
        }
        return false;
    }

    bool YmlNode::saveFile(const String &fileName, const Properties &properties) {
        try {
            YAML::Node node;
            StringArray keys;
            properties.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                String key = keys[i];
                String value;
                if (properties.at(key, value)) {
                    StringArray texts;
                    StringArray::parse(key, texts, '.');
                    YAML::Node *subNode = nullptr;
                    for (size_t j = 0; j < texts.count(); j++) {
                        const String &text = texts[j];
                        YAML::Node *item = j == 0 ? &node : subNode;
//                        int seq = -1;
//                        String seqKey;
//                        ssize_t seqStart = text.find('[');
//                        if(seqStart > 0) {
//                            ssize_t seqEnd = text.find(']');
//                            if(Int32::parse(text.substr(seqStart + 1, seqEnd - seqStart - 1), seq)) {
//                                seqKey = text.substr(0, seqStart);
//                            }
//                        }
//                        if(seq >= 0) {
//                            const YAML::Node &n = item->operator[](seqKey.c_str());
//                            subNode = (YAML::Node *) &n;
//                            subNode->push_back(value.c_str());
//                        } else {
//                            const YAML::Node &n = item->operator[](text.c_str());
//                            subNode = (YAML::Node *) &n;
//                        }
                        const YAML::Node &n = item->operator[](text.c_str());
                        subNode = (YAML::Node *) &n;
                    }
                    if (!value.isNullOrEmpty())
                        subNode->operator=(value.c_str());
                }
            }

            String path = Path::getDirectoryName(fileName);
            if (!Directory::exists(path)) {
                Directory::createDirectory(path);
            }
            String name = Path::getFileName(fileName);
            String tempFileName = Path::combine(path, String("~") + name);
            std::ofstream out(tempFileName);
            out << node; // dump it back into the file
            out.close();
            return File::move(tempFileName, fileName);
        }
        catch (const exception &e) {
            Trace::error(e.what());
        }
        catch (...) {
        }
        return false;
    }

    YmlNode::operator String() const {
        return toString();
    }
}
