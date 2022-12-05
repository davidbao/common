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
    fs.writeText("<?xml version='1.0' ?>\n"
                 "<book genre='novel' ISBN='1-861001-57-5'>\n"
                 "\t<title>Pride And Prejudice</title>\n"
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
        if(!reader.isValid()) {
            return false;
        }
    }
    {
        XmlTextReader reader("abc");
        if(reader.isValid()) {
            return false;
        }
    }

    {
        XmlTextReader reader(zipFullFileName, xmlFileName);
        if(!reader.isValid()) {
            return false;
        }
    }

    {
        Zip zip(zipFullFileName);
        XmlTextReader reader(&zip, xmlFileName);
        if(!reader.isValid()) {
            return false;
        }
    }

    return true;
}

bool testRead() {
    {
        XmlTextReader reader(xmlFullFileName);
        if(!reader.isValid()) {
            return false;
        }
        if(!reader.read()) {
            return false;
        }
        if(reader.nodeType() != XmlNodeType::Element) {
            return false;
        }
        if(reader.name() != "book") {
            return false;
        }
        if(!reader.value().isNullOrEmpty()) {
            return false;
        }
        if(reader.innerXml() != "\n\t<title>Pride And Prejudice</title>\n") {
            return false;
        }
        if(reader.outerXml() != "<book genre=\"novel\" ISBN=\"1-861001-57-5\">\n\t<title>Pride And Prejudice</title>\n</book>") {
            return false;
        }

        if(!reader.read()) {
            return false;
        }
        if(!reader.read()) {
            return false;
        }
        if(reader.nodeType() != XmlNodeType::Element) {
            return false;
        }
        if(reader.name() != "title") {
            return false;
        }
        if(!reader.value().isNullOrEmpty()) {
            return false;
        }
        if(reader.innerXml() != "Pride And Prejudice") {
            return false;
        }
        if(reader.outerXml() != "<title>Pride And Prejudice</title>") {
            return false;
        }
    }

    return true;
}

int main() {
    setUp();

    if (!testConstructor()) {
        return 1;
    }

    if (!testRead()) {
        return 1;
    }

    cleanUp();

    return 0;
}