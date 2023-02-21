//
//  JsonTextReaderTest.cpp
//  common
//
//  Created by baowei on 2023/1/30.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "json/JsonTextReader.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "IO/Zip.h"

using namespace Json;

static const String jsonPath = Path::combine(Path::getTempPath(), "jsonTextReaderTest");
static const char *jsonFileName = "book.json";
static const String jsonFullFileName = Path::combine(jsonPath, jsonFileName);
static const String zipFullFileName = Path::combine(jsonPath, "book.zip");

void setUp() {
    if (!Directory::exists(jsonPath)) {
        Directory::createDirectory(jsonPath);
    }

    FileStream fs(jsonFullFileName, FileMode::FileCreate, FileAccess::FileWrite);
//    {
//        "book":{
//            "bk":"urn:samples",
//                    "genre":"novel",
//                    "pages":"1234",
//                    "ISBN":"1-861001-57-5",
//                    "title":"Pride And Prejudice",
//                    "bk:genre":"novel",
//                    "pages1":[
//            {
//                "page1.1":"1",
//                        "page1.2":"2"
//            },
//            {
//                "page1.3":"3",
//                        "page1.4":"4"
//            }
//            ],
//            "pages2":[
//            {
//                "page2.1":"1",
//                        "page2.2":"2"
//            },
//            {
//                "page2.3":"3",
//                        "page2.4":"4"
//            }
//            ]
//        }
//    }
    fs.writeText("{\n"
                 "    \"book\":{\n"
                 "        \"bk\":\"urn:samples\",\n"
                 "        \"genre\":\"novel\",\n"
                 "        \"pages\":\"1234\",\n"
                 "        \"ISBN\":\"1-861001-57-5\",\n"
                 "        \"title\":\"Pride And Prejudice\",\n"
                 "        \"bk:genre\":\"novel\",\n"
                 "        \"pages1\":[\n"
                 "            {\n"
                 "                \"page1.1\":\"1\",\n"
                 "                \"page1.2\":\"2\"\n"
                 "            },\n"
                 "            {\n"
                 "                \"page1.3\":\"3\",\n"
                 "                \"page1.4\":\"4\"\n"
                 "            }\n"
                 "        ],\n"
                 "        \"pages2\":[\n"
                 "            {\n"
                 "                \"page2.1\":\"1\",\n"
                 "                \"page2.2\":\"2\"\n"
                 "            },\n"
                 "            {\n"
                 "                \"page2.3\":\"3\",\n"
                 "                \"page2.4\":\"4\"\n"
                 "            }\n"
                 "        ]\n"
                 "    }\n"
                 "}");
    fs.close();

    Zip::compressFile(jsonFullFileName, zipFullFileName);
}

void cleanUp() {
    if (Directory::exists(jsonPath)) {
        Directory::deleteDirectory(jsonPath);
    }
}

bool testConstructor() {
    {
        JsonTextReader reader(jsonFullFileName);
        if (!reader.isValid()) {
            return false;
        }
    }

    {
        JsonTextReader reader(zipFullFileName, jsonFileName);
        if (!reader.isValid()) {
            return false;
        }
    }

    {
        Zip zip(zipFullFileName);
        JsonTextReader reader(&zip, jsonFileName);
        if (!reader.isValid()) {
            return false;
        }
    }

    {
//        {
//            "book":{
//                "bk":"urn:samples",
//                        "genre":"novel",
//                        "pages":"1234",
//                        "ISBN":"1-861001-57-5",
//                        "title":"Pride And Prejudice",
//                        "bk:genre":"novel"
//            }
//        }
        static const String text = "{\n"
                                   "    \"book\":{\n"
                                   "        \"bk\":\"urn:samples\",\n"
                                   "        \"genre\":\"novel\",\n"
                                   "        \"pages\":\"1234\",\n"
                                   "        \"ISBN\":\"1-861001-57-5\",\n"
                                   "        \"title\":\"Pride And Prejudice\",\n"
                                   "        \"bk:genre\":\"novel\"\n"
                                   "    }\n"
                                   "}";
        JsonTextReader reader(text, text.length());
        if (!reader.isValid()) {
            return false;
        }
    }

    return true;
}

bool testRead() {
    {
        JsonTextReader reader(jsonFullFileName);
        if (!reader.isValid()) {
            return false;
        }

        const ConfigFile &cf = reader.configFile();
        if (cf.rootPath != jsonPath) {
            return false;
        }
        if (cf.fileName != jsonFileName) {
            return false;
        }
        if (reader.nodeType() != JsonNodeType::Document) {
            return false;
        }
        if (!reader.read()) {
            return false;
        }
        if (reader.name() != "book") {
            return false;
        }
        if (reader.nodeType() != JsonNodeType::TypeNode) {
            return false;
        }
        if (!reader.read()) {
            return false;
        }
        if (reader.nodeType() != JsonNodeType::TypeNode) {
            return false;
        }
        if (reader.name() != "bk") {
            return false;
        }
        if (reader.value() != "urn:samples") {
            return false;
        }

        if (!reader.read()) {
            return false;
        }
        if (!reader.read()) {
            return false;
        }
        if (reader.nodeType() != JsonNodeType::TypeNode) {
            return false;
        }

        if (reader.name() != "genre") {
            return false;
        }
        if (reader.value() != "novel") {
            return false;
        }

        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        if (reader.name() != "page1.1") {
            return false;
        }
        if (reader.value() != "1") {
            return false;
        }

        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        reader.read();
        if (reader.name() != "page2.4") {
            return false;
        }
        if (reader.value() != "4") {
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

    if (!testRead()) {
        cleanUp();
        return 2;
    }

    cleanUp();

    return 0;
}