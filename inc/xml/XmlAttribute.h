//
//  XmlAttribute.h
//  common
//
//  Created by baowei on 2015/7/30.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef XmlAttribute_h
#define XmlAttribute_h

#include "data/String.h"
#include "data/PList.h"

using namespace Data;

namespace Xml {
    class XmlNode;

    class XmlAttributeInner;

    class XmlAttribute {
    public:
        XmlAttribute();

        ~XmlAttribute();

        bool isValid() const;

        String name() const;

        String value() const;

        bool setValue(const char *value);

        bool setValue(const String &value);

        void copyFrom(const XmlAttribute *value);

    private:
        void setXmlAttributeInner(const XmlNode *node, const void *attr);

    private:
        friend class XmlNode;

        XmlNode *_node;
        XmlAttributeInner *_attr;
    };

    //typedef PList<XmlAttribute> XmlAttributes;
    class XmlAttributes : public CopyPList<XmlAttribute> {
    public:
        XmlAttributes(size_t capacity = PList<XmlAttribute>::DefaultCapacity);

        XmlAttribute *at(const String &name) const;

        XmlAttribute *operator[](const String &name) const;

        const String value(const String &name) const;

        XmlAttribute *at(size_t pos) const override;
    };
}

#endif // XmlAttribute_h
