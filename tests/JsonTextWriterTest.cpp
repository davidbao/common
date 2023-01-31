//
//  JsonTextWriterTest.cpp
//  common
//
//  Created by baowei on 2023/1/30.
//  Copyright © 2023 com. All rights reserved.
//

#include "json/JsonTextWriter.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "IO/Path.h"

using namespace Json;

static const String jsonPath = Path::combine(Path::getTempPath(), "JsonTextWriterTest");
static const char *jsonFileName = "book.json";
static const String jsonFullFileName = Path::combine(jsonPath, jsonFileName);
//static const String zipFullFileName = Path::combine(jsonPath, "book.zip");

void setUp() {
    if (!Directory::exists(jsonPath)) {
        Directory::createDirectory(jsonPath);
    }
}

void cleanUp() {
    if (Directory::exists(jsonPath)) {
        Directory::deleteDirectory(jsonPath);
    }
}

bool testConstructor() {
    {
        JsonTextWriter writer(jsonFullFileName);
        if (!writer.isValid()) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
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
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(true);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 8) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(false);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 2) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(true);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
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
        if(index != 8) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(true);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
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
        if(index != 8) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(true);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 2) {
                if(text.find('\t') != 0) {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 8) {
            return false;
        }
    }

    {
        JsonTextWriter writer(jsonFullFileName);
        writer.enableIndent(true);
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

        FileStream stream(jsonFullFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
        String text;
        int index = 0;
        do {
            text = stream.readLine();
            if(index == 0) {
                if(text.find('{') < 0) {
                    return false;
                }
            }
            index++;
        } while (!text.isNullOrEmpty());
        if(index != 8) {
            return false;
        }
    }

    return true;
}

int main() {
    setUp();

    if (!testConstructor()) {
        cleanUp();
        return 1;
    }
    if (!testProperty()) {
        cleanUp();
        return 2;
    }

    cleanUp();

    return 0;
}
