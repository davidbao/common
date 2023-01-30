//
//  XmlTextWriterTest.cpp
//  common
//
//  Created by baowei on 2022/12/5.
//  Copyright © 2023 com. All rights reserved.
//

#include "xml/XmlTextWriter.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "IO/Path.h"

using namespace Xml;

static const String xmlPath = Path::combine(Path::getTempPath(), "XmlTextWriterTest");
static const char *xmlFileName = "book.xml";
static const String xmlFullFileName = Path::combine(xmlPath, xmlFileName);
static const String zipFullFileName = Path::combine(xmlPath, "book.zip");

void setUp() {
    if (!Directory::exists(xmlPath)) {
        Directory::createDirectory(xmlPath);
    }
}

void cleanUp() {
    if (Directory::exists(xmlPath)) {
        Directory::deleteDirectory(xmlPath);
    }
}

bool testConstructor() {
    {
        XmlTextWriter writer(xmlFullFileName);
        if (!writer.isValid()) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
    }

    return true;
}

bool testProperty() {
    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::Indented);
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 5) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::None);
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 3) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::Indented);
        writer.setIndentString(' ');
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 2) {
                if(text[0] != ' ') {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 5) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::Indented);
        writer.setIndentString('\t');
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 2) {
                if(text[0] != '\t') {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 5) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::Indented);
        writer.setIndentString("    ");
        if(writer.indentString() != "    ") {
            return false;
        }
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 2) {
                if(text.find("    ") != 0) {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 5) {
            return false;
        }
    }

    {
        XmlTextWriter writer(xmlFullFileName);
        writer.setFormatting(XmlTextWriter::Indented);
        writer.setQuoteChar('\'');
        if(writer.quoteChar() != '\'') {
            return false;
        }
        writer.writeStartDocument();
        writer.writeStartElement("book");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 0) {
                if(text.find('\'') < 0) {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 5) {
            return false;
        }
    }

    return true;
}

bool testWrite() {
    {
        XmlTextWriter writer(xmlFullFileName);
        writer.writeStartDocument();

        //Write the ProcessingInstruction node.
        String PItext="type='text/xsl' href='book.xsl'";
        writer.writeProcessingInstruction("xml-stylesheet", PItext);

        //Write the DocumentType node.
        writer.writeDocType("book", "", "", "<!ENTITY h 'hardcover'>");

        writer.writeCData("cdata_value");

        //Write a Comment node.
        writer.writeComment("sample XML");

        writer.writeStartElement("xmlns", "book", "ns");
        writer.writeAttribute("genre", "novel");
        writer.writeAttribute("ISBN", "1-861001-57-5");
        writer.writeStartElement("title");
        writer.writeString("Pride And Prejudice");
        writer.writeEndElement();   // title
        writer.writeStartElement("base64");
        writer.writeBase64(ByteArray{1, 2, 3, 4});
        writer.writeEndElement();   // base64
        writer.writeStartElement("binHex");
        writer.writeBinHex(ByteArray{1, 2, 3, 4});
        writer.writeEndElement();   // binHex
        writer.writeComment("comment");
        writer.writeStartElement("Item");

        writer.writeStartAttribute("xmlns", "ISBN", "urn:samples");
        writer.writeString("1-861003-78");
        writer.writeEndAttribute();
        writer.writeStartAttribute("ISBN2");
        writer.writeString("1-861003-78");
        writer.writeEndAttribute();

        writer.writeString("Write unescaped text:  ");
        writer.writeRaw("this & that");
        writer.writeEndElement();   // Item
        writer.writeEndElement();   // book
        writer.writeFullEndElement();
        writer.writeEndDocument();
        writer.close();

        FileStream stream(xmlFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(text.find("<xmlns:book") > 0) {
                if(text.find("genre") < 0) {
                    return false;
                }
            }
            if(text.find("base64") >= 0) {
                text = String::replace(text, "<base64>", String::Empty);
                text = String::replace(text, "</base64>", String::Empty);
                String str = String::fromBase64(text);
                if (str != "01020304") {
                    return false;
                }
            } else if(text.find("binHex") >= 0) {
                text = text.trim('\t');
                text = String::replace(text, "<binHex>", String::Empty);
                text = String::replace(text, "</binHex>", String::Empty);
                if (text != "01020304") {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
    }

    return true;
}

int main() {
    setUp();

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testProperty()) {
        result = 2;
    }
    if (!testWrite()) {
        result = 3;
    }

    cleanUp();

    return result;
}