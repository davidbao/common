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

        enum ReadState {
            // The Read method has not been called yet.
            Initial = 0,

            // Reading is in progress.
            Interactive = 1,

            // An error occurred that prevents the XmlReader from continuing.
            Error = 2,

            // The end of the stream has been reached successfully.
            EndOfFile = 3,

            // The Close method has been called and the XmlReader is closed.
            Closed = 4,
            Reading = 5
        };

        explicit XmlTextReader(const String &fileName);

        XmlTextReader(const String &text, size_t length);

        XmlTextReader(Zip *zip, const String &fileName);

        XmlTextReader(const String &zipFileName, const String &fileName);

        ~XmlTextReader() override;

        bool getAttribute(const String &name, String &value) const override;

        bool getAttribute(const String &name, const String &ns, String &value) const;

        bool getAttributeNames(StringArray &names) const;

        bool hasAttributes() const;

        int attributeCount() const;

        bool isValid() const;

        bool read();

        String name() const;

        String localName() const;

        String baseUri() const;

        String namespaceUri() const;

        String encoding() const;

        String prefix() const;

        String xmlLang() const;

        String xmlVersion() const;

        XmlNodeType nodeType() const;

        String value() const;

        bool hasValue() const;

        bool isDefault() const;

        bool isEOF() const;

        ReadState readState() const;

        char quoteChar() const;

        String innerXml() const;

        String outerXml() const;

        int depth() const;

        int lineNumber() const;

        int linePosition() const;

        const ConfigFile &configFile() const;

        bool isEmptyElement() const;

        XmlNodeType moveToContent();

        bool moveToElement();

        bool moveToAttribute(int no);

        bool moveToAttribute(const String &name, const String &ns = String::Empty);


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
