//
//  JsonTextWriter.cpp
//  common
//
//  Created by baowei on 2018-10-27.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "json/JsonTextWriter.h"

namespace Json {
    JsonTextWriter::JsonTextWriter(const String &fileName) : _indent(true), _rootNode(nullptr) {
        _stream = new FileStream(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    }

    JsonTextWriter::~JsonTextWriter() {
        close();

        delete _stream;
        _stream = nullptr;
    }

    bool JsonTextWriter::isValid() const {
        return _stream != nullptr && _stream->isOpen();
    }

    bool JsonTextWriter::close() {
        if (isValid()) {
            if (_rootNode != nullptr) {
                writeEndDocument();
            }
            _stream->close();
            return true;
        }
        return false;
    }

    void JsonTextWriter::enableIndent(bool indent) {
        if (isValid()) {
            _indent = indent;
        }
    }

    JsonNode *JsonTextWriter::currentNode() const {
        return _nodes.count() > 0 ? _nodes[_nodes.count() - 1] : _rootNode;
    }

    JsonNode *JsonTextWriter::parentNode() const {
        return _nodes.count() > 1 ? _nodes[_nodes.count() - 2] : _rootNode;
    }

    void JsonTextWriter::writeStartDocument() {
        if (_rootNode == nullptr) {
            _rootNode = new JsonNode(JsonNode::TypeNode);
        }
    }

    void JsonTextWriter::writeEndDocument() {
        if (isValid()) {
            if (_rootNode != nullptr) {
                String text = _rootNode->toString(_indent);
                _stream->writeText(text);

                delete _rootNode;
                _rootNode = nullptr;
            }
        }
    }

    void JsonTextWriter::writeStartElement(const String &name, JsonNode::Type type) {
        if (isValid() && _rootNode != nullptr) {
            auto *node = new JsonNode(name, type);
            _nodes.add(node);
        }
    }

    void JsonTextWriter::writeEndElement() {
        if (isValid() && _rootNode != nullptr) {
            JsonNode *cNode = currentNode();
            JsonNode *pNode = parentNode();
            if (cNode != pNode) {
                pNode->add(*cNode);
            }
            if (_nodes.count() > 0) {
                _nodes.removeAt(_nodes.count() - 1);
            }
        }
    }

    void JsonTextWriter::writeString(const String &text) {
        if (isValid() && _rootNode != nullptr) {
            currentNode()->setValue(text);
        }
    }

    bool JsonTextWriter::setAttribute(const String &name, const String &value) {
        if (isValid()) {
            currentNode()->add(JsonNode(name, value));
            return true;
        }
        return false;
    }
}
