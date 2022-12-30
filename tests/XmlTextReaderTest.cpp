//
//  XmlTextReaderTest.cpp
//  common
//
//  Created by baowei on 2022/12/3.
//  Copyright © 2023 com. All rights reserved.
//

#include "xml/XmlTextReader.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "IO/Zip.h"

using namespace Xml;

static const String xmlPath = Path::combine(Directory::getTempPath(), "XmlTextReaderTest");
static const char *xmlFileName = "book.xml";
static const String xmlFullFileName = Path::combine(xmlPath, xmlFileName);
static const String zipFullFileName = Path::combine(xmlPath, "book.zip");

void setUp() {
    if (!Directory::exists(xmlPath)) {
        Directory::createDirectory(xmlPath);
    }

    FileStream fs(xmlFullFileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.writeText("<?xml version='1.0' encoding='utf-8' ?>\n"
                 "<!-- XML fragment -->\n"
                 "<book xmlns:bk='urn:samples' genre='novel' ISBN='1-861001-57-5'>\n"
                 "\t<title>Pride And Prejudice</title>\n"
                 "\t<bk:genre>novel</bk:genre>\n"
                 "</book>");
    fs.close();

    Zip::compressFile(xmlFullFileName, zipFullFileName);
}

void cleanUp() {
    if (Directory::exists(xmlPath)) {
        Directory::deleteDirectory(xmlPath);
    }
}

bool testConstructor() {
    {
        XmlTextReader reader(xmlFullFileName);
        if (!reader.isValid()) {
            return false;
        }
    }
    {
        XmlTextReader reader("abc");
        if (reader.isValid()) {
            return false;
        }
    }

    {
        XmlTextReader reader(zipFullFileName, xmlFileName);
        if (!reader.isValid()) {
            return false;
        }
    }

    {
        Zip zip(zipFullFileName);
        XmlTextReader reader(&zip, xmlFileName);
        if (!reader.isValid()) {
            return false;
        }
    }

    {
        static const String text = "<?xml version='1.0' encoding='utf-8' ?>\n"
                                   "<!-- XML fragment -->\n"
                                   "<book xmlns:bk='urn:samples' genre='novel' pages='1234' ISBN='1-861001-57-5'>\n"
                                   "\t<title>Pride And Prejudice</title>\n"
                                   "\t<bk:genre>novel</bk:genre>\n"
                                   "</book>";
        XmlTextReader reader(text, text.length());
        if (!reader.isValid()) {
            return false;
        }
    }

    return true;
}

bool testRead() {
    {
        XmlTextReader reader(xmlFullFileName);
        if (!reader.isValid()) {
            return false;
        }
        if (reader.readState() != XmlTextReader::Initial) {
            return false;
        }
        if (reader.isEOF()) {
            return false;
        }
        const ConfigFile &cf = reader.configFile();
        if (cf.rootPath != xmlPath) {
            return false;
        }
        if (cf.fileName != xmlFileName) {
            return false;
        }

        if (!reader.read()) {
            return false;
        }
        if (reader.nodeType() != XmlNodeType::Comments) {
            return false;
        }
        if (reader.encoding() != "utf-8") {
            return false;
        }
        if (reader.xmlVersion() != "1.0") {
            return false;
        }
        if (reader.quoteChar() != '\"') {
            return false;
        }
        if (reader.lineNumber() != 6) {
            return false;
        }
        if (reader.linePosition() != 8) {
            return false;
        }
        if (reader.xmlLang() != "") {
            return false;
        }

        if (!reader.read()) {
            return false;
        }
        if (reader.nodeType() != XmlNodeType::Element) {
            return false;
        }
        if (reader.name() != "book") {
            return false;
        }
        if (!reader.value().isNullOrEmpty()) {
            return false;
        }
        if (reader.innerXml() !=
            "\n\t<title>Pride And Prejudice</title>\n\t<bk:genre xmlns:bk=\"urn:samples\">novel</bk:genre>\n") {
            return false;
        }
        if (reader.outerXml() !=
            "<book xmlns:bk=\"urn:samples\" genre=\"novel\" ISBN=\"1-861001-57-5\">\n\t<title>Pride And Prejudice</title>\n\t<bk:genre>novel</bk:genre>\n</book>") {
            return false;
        }
        if (Path::getFileName(reader.baseUri()) != Path::getFileName(xmlFullFileName)) {
            return false;
        }

        if (!reader.read()) {
            return false;
        }
        if (!reader.read()) {
            return false;
        }
        if (reader.nodeType() != XmlNodeType::Element) {
            return false;
        }

        if (reader.name() != "title") {
            return false;
        }
        if (reader.depth() != 1) {
            return false;
        }
        if (!reader.value().isNullOrEmpty()) {
            return false;
        }
        if (reader.innerXml() != "Pride And Prejudice") {
            return false;
        }
        if (reader.outerXml() != "<title>Pride And Prejudice</title>") {
            return false;
        }
    }

    return true;
}

bool testAttribute() {
    {
        XmlTextReader reader(xmlFullFileName);
        if (!reader.moveToContent()) {
            return false;
        }
        if (!reader.hasAttributes()) {
            return false;
        }
        int count = reader.attributeCount();
        for (int i = 0; i < count; ++i) {
            if (!reader.moveToAttribute(i)) {
                return false;
            }
            if (reader.isEmptyElement()) {
                return false;
            }
            if (reader.isDefault()) {
                return false;
            }
            String name = reader.name();
            if (!(name == "xmlns:bk" || name == "genre" || name == "ISBN")) {
                return false;
            }
            String value = reader.value();
            if (!(value == "urn:samples" || value == "novel" || value == "1-861001-57-5")) {
                return false;
            }
            if (!reader.hasValue()) {
                return false;
            }

            String prefix = reader.prefix();
            if (name == "xmlns:bk") {
                if (prefix != "xmlns") {
                    return false;
                }
                if (reader.namespaceUri() != "http://www.w3.org/2000/xmlns/") {
                    return false;
                }
            } else {
                if (prefix != "") {
                    return false;
                }
            }
        }
    }

    {
        XmlTextReader reader(xmlFullFileName);
        if (!reader.moveToContent()) {
            return false;
        }
        if (reader.moveToAttribute("abc")) {
            return false;
        }
        if (!reader.moveToAttribute("genre")) {
            return false;
        }
        if (reader.name() != "genre") {
            return false;
        }
        if (reader.value() != "novel") {
            return false;
        }
    }

    {
        static const String text = "<?xml version='1.0' encoding='utf-8' ?>\n"
                                   "<!-- XML fragment -->\n"
                                   "<book xmlns:bk='urn:samples' genre='novel' pages='1234' ISBN='1-861001-57-5'>\n"
                                   "\t<title>Pride And Prejudice</title>\n"
                                   "\t<bk:genre>novel</bk:genre>\n"
                                   "</book>";
        XmlTextReader reader(text, text.length());
        if (!reader.moveToContent()) {
            return false;
        }

        int abc;
        if (reader.getAttribute("abc", abc)) {
            return false;
        }

        int pages;
        if (!reader.getAttribute("pages", pages)) {
            return false;
        }
        if (pages != 1234) {
            return false;
        }
        if (reader.getAttribute("pages", pages, 2000, 3000)) {
            return false;
        }
    }

    {
        static const String text = "<?xml version='1.0' encoding='utf-8' ?>\n"
                                   "<!-- XML fragment -->\n"
                                   "<book xmlns:bk='urn:samples' genre='novel &amp;' pages='1234' ISBN='1-861001-57-5'>\n"
                                   "\t<title>Pride And Prejudice</title>\n"
                                   "\t<bk:genre>novel</bk:genre>\n"
                                   "</book>";
        XmlTextReader reader(text, text.length());
        if (!reader.moveToContent()) {
            return false;
        }

        String genre;
        if (!reader.getAttribute("genre", genre)) {
            return false;
        }
        if (genre != "novel &") {
            return false;
        }
    }

    {
        static const String text = "<?xml version='1.0' encoding='utf-8' ?>\n"
                                   "<!-- XML fragment -->\n"
                                   "<book xmlns:bk='urn:samples' genre='novel &amp;' pages='1234' ISBN='1-861001-57-5'>\n"
                                   "\t<title>Pride And Prejudice</title>\n"
                                   "\t<bk:genre>novel</bk:genre>\n"
                                   "</book>";
        XmlTextReader reader(text, text.length());
        if (!reader.moveToContent()) {
            return false;
        }

        StringArray names;
        if (!reader.getAttributeNames(names)) {
            return false;
        }
        if (!(names.contains("xmlns:bk") && names.contains("genre") &&
              names.contains("pages") && names.contains("ISBN"))) {
            return false;
        }
    }

    return true;
}

int main() {
    setUp();

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testRead()) {
        result = 2;
    }
    if (!testAttribute()) {
        result = 3;
    }

    cleanUp();

    return result;
}