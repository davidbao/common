//
//  XmlTextReader.h
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef XmlTextReader_h
#define XmlTextReader_h

#include "data/ValueType.h"
#include "IO/Zip.h"
#include "configuration/ConfigFile.h"
#include "xml/XmlNode.h"
#include "data/IAttribute.h"

namespace Xml {
    class XmlTextReaderInner;

    class XmlTextReader : public IAttributeGetter {
    public:
        using IAttributeGetter::getAttribute;

        explicit XmlTextReader(const String &fileName);

        XmlTextReader(const String &text, size_t length);

        XmlTextReader(Zip *zip, const String &fileName);

        XmlTextReader(const String &zipFileName, const String &fileName);

        ~XmlTextReader() override;

        bool getAttribute(const String &name, String &value) const override;

        bool isValid() const;

        bool read();

        String name() const;

        String localName() const;

        String baseUri() const;

        String namespaceUri() const;

        XmlNodeType nodeType() const;

        String value() const;

        String innerXml() const;

        String outerXml() const;

        int depth() const;

        int lineNumber() const;

        int linePosition() const;

        const ConfigFile &configFile() const;

        bool isEmptyElement() const;

        XmlNodeType moveToContent();

        bool moveToElement();

    private:
        XmlTextReaderInner *_reader;

        bool _deleteZip;
        ZipFile *_zipFile;
        ConfigFile _configFile;

    private:
        static const int XmlSuccess = 1;
        static const int XmlFailed = -1;
    };
}

#endif    // XmlTextReader_h
