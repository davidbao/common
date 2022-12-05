//
//  XmlAttribute.cpp
//  common
//
//  Created by baowei on 2017/7/30.
//  Copyright © 2017 com. All rights reserved.
//

#include "xml/XmlAttribute.h"
#include "exception/Exception.h"
#include "xml/XmlNode.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace Xml {
    class XmlAttributeInner {
    public:
        xmlAttributePtr attr;

        XmlAttributeInner(xmlAttributePtr attr = nullptr) {
            this->attr = attr;
        }
    };

    XmlAttribute::XmlAttribute() {
        _node = nullptr;
        _attr = new XmlAttributeInner();
    }

    XmlAttribute::~XmlAttribute() {
        _node = nullptr;
        delete _attr;
    }

    String XmlAttribute::name() const {
        String result;
        if (isValid()) {
            result = (const char *) _attr->attr->name;
        }
        return result;
    }

    bool XmlAttribute::isValid() const {
        return _node != nullptr && _attr->attr != nullptr;
    }

    String XmlAttribute::value() const {
        String result;
        if (isValid()) {
            xmlChar *szAttr = xmlGetProp((xmlNodePtr) _node->nodePtr(), (const xmlChar *) name().c_str());
            result = (const char *) szAttr;
            xmlFree(szAttr);
        }
        return result;
    }

    bool XmlAttribute::setValue(const char *value) {
        if (isValid()) {
            xmlSetProp((xmlNodePtr) _node->nodePtr(), (const xmlChar *) name().c_str(), (const xmlChar *) value);
            return true;
        }
        return false;
    }

    bool XmlAttribute::setValue(const String &value) {
        return setValue(value.c_str());
    }

    void XmlAttribute::setXmlAttributeInner(const XmlNode *node, const void *attr) {
        _node = (XmlNode *) node;
        _attr->attr = (xmlAttributePtr) attr;
    }

    void XmlAttribute::copyFrom(const XmlAttribute *value) {
        _node = value->_node;
        _attr->attr = value->_attr->attr;
    }

    XmlAttributes::XmlAttributes(size_t capacity) : CopyPList<XmlAttribute>(true, capacity) {
    }

    XmlAttribute *XmlAttributes::at(const String &name) const {
        for (size_t i = 0; i < count(); i++) {
            XmlAttribute *attr = PList<XmlAttribute>::at(i);
            if (attr->name() == name) {
                return attr;
            }
        }
        return nullptr;
    }

    XmlAttribute *XmlAttributes::operator[](const String &name) const {
        return this->at(name);
    }

    const String XmlAttributes::value(const String &name) const {
        XmlAttribute *attr = at(name);
        return attr != nullptr ? attr->value() : String::Empty;
    }

    XmlAttribute *XmlAttributes::at(size_t pos) const {
        return CopyPList<XmlAttribute>::at(pos);
    }
}
