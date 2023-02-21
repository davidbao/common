//
//  JsonTextReader.cpp
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "json/JsonTextReader.h"
#include "exception/Exception.h"
#include "IO/Path.h"
#include "data/String.h"
#include "diag/Trace.h"

using namespace Diag;
using namespace System;

namespace Json {
    JsonTextReader::Node::Node() : parent(nullptr), iterator(-1), type(JsonNodeType::NodeNone) {
    }

    JsonTextReader::Node::Node(const JsonNode &node, JsonNodeType type) : Node(nullptr, node, type) {
    }

    JsonTextReader::Node::Node(Node *parent, const JsonNode &node, JsonNodeType type) : parent(parent), node(node),
                                                                                        iterator(-1),
                                                                                        type(type) {
    }

    bool JsonTextReader::Node::isValid() const {
        return !node.isEmpty();
    }

    String JsonTextReader::Node::name() const {
        return node.name();
    }

    String JsonTextReader::Node::value() const {
        return node.value();
    }

    String JsonTextReader::Node::typeStr() const {
        switch (type) {
            case JsonNodeType::NodeNone:
                return "None";
            case JsonNodeType::TypeNode:
                return "Node";
            case JsonNodeType::TypeEndNode:
                return "EndNode";
            case JsonNodeType::Document:
                return "Document";
            case JsonNodeType::TypeArray:
                return "Array";
            case JsonNodeType::TypeEndArray:
                return "EndArray";
            default:
                return "None";
        }
    }

    JsonTextReader::Node *JsonTextReader::Node::read() {
        if (isValid()) {
            Node *pNode;
            JsonNode n;
            if (node.at(++iterator, n)) {
                pNode = new Node(this, n, n.type() == JsonNode::TypeArray ? JsonNodeType::TypeArray
                                                                          : JsonNodeType::TypeNode);
            } else {
                pNode = new Node(parent, node, node.type() == JsonNode::TypeArray ? JsonNodeType::TypeEndArray
                                                                                  : JsonNodeType::TypeEndNode);
            }
            // Attention: need to delete it.
            return pNode;
        }
        return nullptr;
    }

    JsonTextReader::JsonTextReader() : _currentNode(nullptr), _deleteZip(false) {
    }

    JsonTextReader::JsonTextReader(const String &fileName) : JsonTextReader() {
        FileStream fs(fileName, FileMode::FileOpen, FileAccess::FileRead);
        if (fs.isOpen()) {
            _configFile.rootPath = Path::getDirectoryName(fileName);
            _configFile.fileName = Path::getFileName(fileName);

            String str;
            if (fs.readToEnd(str)) {
                initRootNode(str);
            }
        }

#ifdef DEBUG
        if (!isValid())
            Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
    }

    JsonTextReader::JsonTextReader(const String &text, size_t length) : JsonTextReader() {
        initRootNode(text.substr(0, length));
    }

    JsonTextReader::JsonTextReader(Zip *zip, const String &fileName) : JsonTextReader() {
        if (zip == nullptr)
            throw ArgumentException("zip");

#ifdef WIN32
        // fixbug: '/' must be used instead of '\' in windows.
        _configFile.fileName = String::replace(fileName, "\\", "/");
#else
        _configFile.fileName = fileName;
#endif

        String str;
        if (zip->read(_configFile.fileName, str))
            initRootNode(str);

#ifdef DEBUG
        if (!isValid())
            Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
    }

    JsonTextReader::JsonTextReader(const String &zipFileName, const String &fileName) : JsonTextReader() {
        Zip *zip = new Zip(zipFileName);
        if (zip->isValid()) {
            _deleteZip = true;
            _configFile.zip = zip;
#ifdef WIN32
            // fixbug: '/' must be used instead of '\' in windows.
            _configFile.fileName = String::replace(fileName, "\\", "/");
#else
            _configFile.fileName = fileName;
#endif
            String str;
            if (zip->read(_configFile.fileName, str)) {
                initRootNode(str);
                initRootNode(str);
            }
        } else {
            delete zip;
        }
    }

    JsonTextReader::~JsonTextReader() {
        close();

        if (_configFile.isZip()) {
            if (_deleteZip) {
                delete _configFile.zip;
            }
            _configFile.zip = nullptr;
        }
    }

    void JsonTextReader::initRootNode(const String &str) {
        JsonNode node;
        if (JsonNode::parse(str, node)) {
            Node *pNode = new Node(node);
            _nodes.add(pNode);
            _currentNode = pNode;
        }
    }

    bool JsonTextReader::isValid() const {
        return _currentNode != nullptr && _currentNode->isValid();
    }

    bool JsonTextReader::close() {
        if (isValid()) {
            _currentNode = nullptr;
            _nodes.clear();
            return true;
        }
        return false;
    }

    bool JsonTextReader::read() {
        if (isValid()) {
            Node *curNode = currentNode();
            if (curNode->type == JsonNodeType::TypeEndNode ||
                curNode->type == JsonNodeType::TypeEndArray) {
                curNode = curNode->parent;
            }
            if (curNode != nullptr && curNode->isValid()) {
                Node *subNode = curNode->read();
                if (subNode != nullptr) {
                    _nodes.add(subNode);
                    _currentNode = subNode;
//                    Trace::info(String::format("current node name: '%s', value: '%s', type: '%s'",
//                                               subNode->name().c_str(),
//                                               subNode->value().c_str(),
//                                               subNode->typeStr().c_str()));
                }
                return true;
            }
        }
        return false;
    }

    JsonNodeType JsonTextReader::nodeType() const {
        return isValid() ? currentNode()->type : JsonNodeType::NodeNone;
    }

    String JsonTextReader::name() const {
        return currentNode()->node.name();
    }

    String JsonTextReader::value() const {
        return currentNode()->node.value();
    }

    JsonTextReader::Node *JsonTextReader::currentNode() const {
        return _currentNode;
    }

    bool JsonTextReader::getAttribute(const String &name, String &value) const {
        if (isValid()) {
            String str;
            if (currentNode()->node.getAttribute(name, str)) {
                value = str;
                return true;
            }
        }
        return false;
    }

    const ConfigFile &JsonTextReader::configFile() const {
        return _configFile;
    }
}
