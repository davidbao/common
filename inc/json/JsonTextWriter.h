//
//  JsonTextWriter.h
//  common
//
//  Created by baowei on 2018/5/30.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef JsonTextWriter_h
#define JsonTextWriter_h

#include "data/ValueType.h"
#include "net/NetType.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"
#include "data/IAttribute.h"

namespace Common {
    class JsonTextWriter : public IAttributeSetter {
    public:
        explicit JsonTextWriter(const String &fileName);

        ~JsonTextWriter() override;

        bool setAttribute(const String &name, const String &value) override;

        bool isValid() const;

        bool close();

        void enableIndent(bool indent = true);

        void writeStartDocument();

        void writeEndDocument();

        void writeStartElement(const String &name, JsonNode::Type type = JsonNode::Type::TypeNode);

        void writeEndElement();

    private:
        JsonNode *currentNode() const;

        JsonNode *parentNode() const;

    private:
        bool _indent;

        FileStream *_stream;
        JsonNode *_rootNode;
        PList<JsonNode> _nodes;
    };
}

#endif    // JsonTextWriter_h
