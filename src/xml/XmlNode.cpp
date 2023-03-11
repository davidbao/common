//
//  XmlNode.cpp
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "xml/XmlNode.h"
#include "data/ValueType.h"
#include "exception/Exception.h"
#include "xml/XmlDocument.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace Xml {
    class XmlNodeInner {
    public:
        xmlNodePtr node;

        XmlNodeInner(xmlNodePtr node = nullptr) {
            this->node = node;
        }
    };

    XmlNode::XmlNode() : XmlNode(String::Empty) {
    }

    XmlNode::XmlNode(const String &name, const String &content) {
        _doc = nullptr;

        if (!name.isNullOrEmpty()) {
            _node = new XmlNodeInner(xmlNewNode(nullptr, (const xmlChar *) name.c_str()));
            if (!content.isNullOrEmpty())
                xmlNodeSetContent(_node->node, (const xmlChar *) content.c_str());
        } else {
            _node = new XmlNodeInner();
        }
    }

    XmlNode::XmlNode(XmlDocument *doc, const void *node) {
        _doc = doc;
        _node = new XmlNodeInner();

        setXmlNodeInner(node);
    }

    XmlNode::~XmlNode() {
        _doc = nullptr;
        delete _node;
    }

    bool XmlNode::isValid() const {
        return _node->node != nullptr;
    }

    const XmlAttributes &XmlNode::attributes() const {
        return _attributes;
    }

    const String XmlNode::name() const {
        return isValid() ? (const String) ((const char *) _node->node->name) : String::Empty;
    }

    XmlNodeType XmlNode::nodeType() const {
        return isValid() ? (XmlNodeType) _node->node->type : XmlNodeType::NodeNone;
    }

    const String XmlNode::content() const {
        return isValid() ? (const String) ((const char *) _node->node->content) : String::Empty;
    }

    void XmlNode::setContent(const String &content) {
        if (isValid()) {
            xmlNodeSetContent(_node->node, (const xmlChar *) content.c_str());
        }
    }

    void XmlNode::setXmlNodeInner(const void *node) {
        _node->node = (xmlNodePtr) node;

        if (_node->node != nullptr) {
            _attributes.clear();
            xmlAttrPtr attrPtr = _node->node->properties;
            while (attrPtr != nullptr) {
                XmlAttribute *attr = new XmlAttribute();
                attr->setXmlAttributeInner(this, attrPtr);
                _attributes.add(attr);

                attrPtr = attrPtr->next;
            }
        }
    }

    bool XmlNode::subNodes(XmlNodes &nodes) const {
        if (hasSubNodes()) {
            xmlNodePtr nodePtr = _node->node->children;
            while (nodePtr != nullptr) {
                if (nodePtr->type == XML_ELEMENT_NODE) {
                    XmlNode *node = new XmlNode(_doc, nodePtr);
                    nodes.add(node);
                }
                nodePtr = nodePtr->next;
            }
        }
        return nodes.count() > 0;
    }

    bool XmlNode::hasSubNodes() const {
        if (isValid()) {
            xmlNodePtr nodePtr = _node->node->children;
            while (nodePtr != nullptr) {
                if (nodePtr->type == XML_ELEMENT_NODE) {
                    return true;
                }
                nodePtr = nodePtr->next;
            }
        }
        return false;
    }

    void XmlNode::selectSingleNode(const char *xpath, XmlNode &node) {
        if (isValid()) {
            xmlNodePtr nodePtr = _node->node->children;
            while (nodePtr != nullptr) {
                if (nodePtr->type == XML_ELEMENT_NODE &&
                    strcmp((const char *) nodePtr->name, xpath) == 0) {
                    node.setXmlNodeInner(nodePtr);
                    break;
                }
                nodePtr = nodePtr->next;
            }
        }
    }

    bool XmlNode::containsNode(const String &name) const {
        if (isValid()) {
            xmlNodePtr nodePtr = _node->node->children;
            while (nodePtr != nullptr) {
                if (nodePtr->type == XML_ELEMENT_NODE &&
                    strcmp((const char *) nodePtr->name, name) == 0) {
                    return true;
                }
                nodePtr = nodePtr->next;
            }
        }
        return false;
    }

    bool XmlNode::getSubNode(const String &name, XmlNode &node) const {
        if (isValid()) {
            xmlNodePtr nodePtr = _node->node->children;
            while (nodePtr != nullptr) {
                if (nodePtr->type == XML_ELEMENT_NODE &&
                    strcmp((const char *) nodePtr->name, name) == 0) {
                    node.setXmlNodeInner(nodePtr);
                    return true;
                }
                nodePtr = nodePtr->next;
            }
        }
        return false;
    }

    bool XmlNode::getSubNode(const String &name, const String &value, XmlNode &node) const {
        if (isValid()) {
            PList<XmlNode> nodes;
            subNodes(nodes);
            for (size_t i = 0; i < nodes.count(); i++) {
                const XmlNode *subNode = nodes[i];
                if (subNode->getAttribute(name) == value) {
                    node.copyFrom(subNode);
                    return true;
                }
            }
        }
        return false;
    }

    bool XmlNode::getOrNewSubNode(const String &name, XmlNode &node) {
        if (name.isNullOrEmpty())
            return false;

        if (getSubNode(name, node))
            return true;

        XmlNode subNode(name);
        node.copyFrom(&subNode);
        return appendNode(node);
    }

    bool XmlNode::appendAttribute(const String &name, const String &value) {
        XmlAttribute attr;
        return appendAttribute(name, value, attr);
    }

    bool XmlNode::appendAttribute(const String &name, const char *value) {
        return appendAttribute(name, String(value));
    }

    bool XmlNode::appendAttribute(const String &name, const String &value, XmlAttribute &attr) {
        if (isValid()) {
            xmlAttrPtr attrPtr = xmlNewProp(_node->node, (const xmlChar *) name.c_str(),
                                            (const xmlChar *) value.c_str());
            if (attrPtr != nullptr) {
                attr.setXmlAttributeInner(this, attrPtr);
                return true;
            }
        }
        return false;
    }

    bool XmlNode::appendAttribute(const String &name, const bool &value) {
        return appendAttribute(name, Boolean(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const char &value) {
        return appendAttribute(name, Int8(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const uint8_t &value) {
        return appendAttribute(name, UInt8(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const short &value) {
        return appendAttribute(name, Int16(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const uint16_t &value) {
        return appendAttribute(name, UInt16(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const int &value) {
        return appendAttribute(name, Int32(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const uint32_t &value) {
        return appendAttribute(name, UInt32(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const int64_t &value) {
        return appendAttribute(name, Int64(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const uint64_t &value) {
        return appendAttribute(name, UInt64(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const float &value) {
        return appendAttribute(name, Float(value).toString());
    }

    bool XmlNode::appendAttribute(const String &name, const double &value) {
        return appendAttribute(name, Double(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const String &value) {
        const XmlAttributes &attrs = attributes();
        XmlAttribute *attr = attrs[name];
        if (attr != nullptr)
            return attr->setValue(value);
        else
            return appendAttribute(name, value);
    }

    bool XmlNode::updateAttribute(const String &name, const char *&value) {
        return updateAttribute(name, String(value));
    }

    bool XmlNode::updateAttribute(const String &name, const bool &value) {
        return updateAttribute(name, Boolean(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const char &value) {
        return updateAttribute(name, Char(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const uint8_t &value) {
        return updateAttribute(name, Byte(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const short &value) {
        return updateAttribute(name, Int16(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const uint16_t &value) {
        return updateAttribute(name, UInt16(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const int &value) {
        return updateAttribute(name, Int32(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const uint32_t &value) {
        return updateAttribute(name, UInt32(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const int64_t &value) {
        return updateAttribute(name, Int64(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const uint64_t &value) {
        return updateAttribute(name, UInt64(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const float &value) {
        return updateAttribute(name, Float(value).toString());
    }

    bool XmlNode::updateAttribute(const String &name, const double &value) {
        return updateAttribute(name, Double(value).toString());
    }

    bool XmlNode::appendNode(XmlNode &node) {
        if (isValid()) {
            if (xmlAddChild(_node->node, node._node->node) != nullptr) {
                node._doc = this->_doc;
                return true;
            }
        }
        return false;
    }

    XmlDocument *XmlNode::document() const {
        return _doc;
    }

    void XmlNode::copyFrom(const XmlNode *value) {
        _doc = value->_doc;
        _node->node = value->_node->node;

        _attributes.copyFrom(&value->_attributes);
        for (size_t i = 0; i < _attributes.count(); i++) {
            _attributes.PList<XmlAttribute>::at(i)->_node = this;
        }
    }

    void *XmlNode::nodePtr() const {
        return _node->node;
    }

    void XmlNode::setNodePtr(void *node) {
        _node->node = (xmlNodePtr) node;
    }

    const String XmlNode::getAttribute(const String &name) const {
        return _attributes.value(name);
    }

    bool XmlNode::getAttribute(const String &name, String &value) const {
        value = getAttribute(name);
        return true;
    }

    bool XmlNode::getAttribute(const String &name, bool &value) const {
        return Boolean::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, char &value) const {
        return Char::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, uint8_t &value) const {
        return Byte::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, short &value) const {
        return Int16::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, uint16_t &value) const {
        return UInt16::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, int &value) const {
        return Int32::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, uint32_t &value) const {
        return UInt32::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, int64_t &value) const {
        return Int64::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, uint64_t &value) const {
        return UInt64::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, float &value) const {
        return Float::parse(getAttribute(name), value);
    }

    bool XmlNode::getAttribute(const String &name, double &value) const {
        return Double::parse(getAttribute(name), value);
    }

    String XmlNode::toString() const {
        if (isValid()) {
            xmlBufferPtr buffer = xmlBufferCreate();
            xmlNodeDump(buffer, nullptr, (xmlNodePtr) _node->node, 0, 1);
            const char *content = strdup((const char *) buffer->content);
//            Debug::writeFormatLine("content: \n%s", content);
            String result = String::unicode2String(content);
            xmlBufferFree(buffer);
            return result;
        }
        return String::Empty;
    }
}
