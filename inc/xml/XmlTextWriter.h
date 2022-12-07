//
//  XmlTextWriter.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef XmlTextWriter_h
#define XmlTextWriter_h

#include "data/ValueType.h"
#include "data/ByteArray.h"
#include "net/NetType.h"
#include "data/IAttribute.h"

namespace Xml {
    class XmlTextWriterInner;

    class XmlTextWriter : public IAttributeSetter {
    public:
        using IAttributeSetter::setAttribute;

        enum Formatting {
            // No special formatting is done.
            None,

            //This option causes child elements to be indented using the Indentation and IndentChar properties.
            // It only indents Element Content (http://www.w3.org/TR/1998/REC-xml-19980210#sec-element-content)
            // and not Mixed Content (http://www.w3.org/TR/1998/REC-xml-19980210#sec-mixed-content)
            // according to the XML 1.0 definitions of these terms.
            Indented,
        };

        explicit XmlTextWriter(const String &fileName);

        ~XmlTextWriter() override;

        bool setAttribute(const String &name, const String &value) override;

        bool isValid() const;

        bool close();

        void flush();

        Formatting formatting() const;

        void setFormatting(Formatting formatting);

        const String &indentString() const;

        void setIndentString(const String &indentString);

        char quoteChar() const;

        void setQuoteChar(char quoteChar);

        void writeStartDocument();

        void writeStartDocument(const String &version, const String &encoding, const String &standalone);

        void writeEndDocument();

        void writeStartElement(const String &localName);

        void writeStartElement(const String &prefix, const String &localName, const String &ns);

        void writeEndElement();

        void writeFullEndElement();

        void writeStartAttribute(const String &localName);

        void writeStartAttribute(const String &prefix, const String &name, const String &ns);

        void writeEndAttribute();

        bool writeString(const String &text);

        bool writeBase64(const ByteArray &buffer);

        bool writeBinHex(const ByteArray &buffer);

        bool writeCData(const String &text);

        bool writeComment(const String &text);

        bool writeDocType(const String &name, const String &pubid, const String &sysid, const String &subset);

        bool writeRaw(const String &text);

        bool writeProcessingInstruction(const String &name, const String &text);

    private:
        XmlTextWriterInner *_writer;

        Formatting _formatting;
        String _indentString;
        char _quoteChar;

    private:
        static const int XmlSuccess = 1;
        static const int XmlFailed = -1;
    };
}

#endif // XmlTextWriter_h
