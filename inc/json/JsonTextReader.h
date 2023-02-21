//
//  JsonTextReader.h
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef JsonTextReader_h
#define JsonTextReader_h

#include "data/String.h"
#include "IO/Zip.h"
#include "configuration/ConfigFile.h"
#include "json/JsonNode.h"
#include "IO/FileStream.h"

using namespace IO;
using namespace Config;

namespace Json {
    enum JsonNodeType {
        NodeNone = 0,
        TypeNode = 1,
        TypeEndNode = 2,
        Document = 3,
        TypeArray = 4,
        TypeEndArray = 5
    };

    class JsonTextReader : public IAttributeGetter {
    public:
        using IAttributeGetter::getAttribute;

        explicit JsonTextReader(const String &fileName);

        JsonTextReader(const String &text, size_t length);

        JsonTextReader(Zip *zip, const String &fileName);

        JsonTextReader(const String &zipFileName, const String &fileName);

        ~JsonTextReader() override;

        bool getAttribute(const String &name, String &value) const override;

        bool isValid() const;

        bool close();

        bool read();

        String name() const;

        JsonNodeType nodeType() const;

        String value() const;

        const ConfigFile &configFile() const;

    private:
        class Node {
        public:
            Node *parent;
            JsonNode node;
            int iterator;

            JsonNodeType type;

            Node();

            explicit Node(const JsonNode &node, JsonNodeType type = JsonNodeType::Document);

            Node(Node *parent, const JsonNode &node, JsonNodeType type);

            bool isValid() const;

            String name() const;

            String value() const;

            String typeStr() const;

            Node *read();
        };

        typedef PList<Node> Nodes;

    private:
        JsonTextReader();

        Node *currentNode() const;

        void initRootNode(const String &str);

    private:
        Node *_currentNode;
        Nodes _nodes;

        bool _deleteZip;
        ConfigFile _configFile;
    };
}

#endif    // JsonTextReader_h
