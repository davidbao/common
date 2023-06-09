//
//  XmlDocument.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef XmlDocument_h
#define XmlDocument_h

#include "data/String.h"
#include "XmlNode.h"

namespace Xml {
    class XmlDocumentInner;

    class XmlDocument {
    public:
        XmlDocument();

        ~XmlDocument();

        bool load(const String &fileName);

        bool loadXml(const String &xml);

        bool save(const String &fileName);

        bool documentElement(XmlNode &node);

        bool newDocument(const String &encoding = "utf-8");

        bool isValid() const;

        bool createNode(const String &name, XmlNode &node);

        bool setRootNode(const XmlNode &rootNode);

    private:
        XmlDocumentInner *_doc;

        XmlNode _rootNode;
    };
}

#endif // XmlDocument_h
