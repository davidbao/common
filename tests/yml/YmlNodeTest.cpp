//
//  YmlNodeTest.h
//  common
//
//  Created by baowei on 2022/12/7.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "yml/YmlNode.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"

using namespace Yml;

template<class parseType, class keyType>
bool testValueConstructor(const keyType &value) {
    YmlNode test(value);
    keyType v;
    if (!parseType::parse(test.toString(), v)) {
        return false;
    }
    if (v != value) {
        return false;
    }
    return true;
}

bool testConstructor() {
    {
        YmlNode test;
        if (test.type() != YmlNode::TypeUndefined) {
            return false;
        }
    }
    {
        YmlNode test(YmlNode::TypeUndefined);
        if (test.type() != YmlNode::TypeUndefined) {
            return false;
        }
    }
    {
        YmlNode test(YmlNode::TypeNull);
        if (test.type() != YmlNode::TypeNull) {
            return false;
        }
    }
    {
        YmlNode test(YmlNode::TypeScalar);
        if (test.type() != YmlNode::TypeScalar) {
            return false;
        }
    }
    {
        YmlNode test(YmlNode::TypeSequence);
        if (test.type() != YmlNode::TypeSequence) {
            return false;
        }
    }
    {
        YmlNode test(YmlNode::TypeMap);
        if (test.type() != YmlNode::TypeMap) {
            return false;
        }
    }

    if (!testValueConstructor<Boolean, bool>(true)) {
        return false;
    }
    if (!testValueConstructor<Char, char>('T')) {
        return false;
    }
    if (!testValueConstructor<Int8, int8_t>(0x14)) {
        return false;
    }
    if (!testValueConstructor<UInt8, uint8_t>(0x84)) {
        return false;
    }
    if (!testValueConstructor<Int16, int16_t>(0x114)) {
        return false;
    }
    if (!testValueConstructor<UInt16, uint16_t>(0x884)) {
        return false;
    }
    if (!testValueConstructor<Int32, int32_t>(0x188814)) {
        return false;
    }
    if (!testValueConstructor<UInt32, uint32_t>(0x888884)) {
        return false;
    }
    if (!testValueConstructor<Int64, int64_t>(0x188814)) {
        return false;
    }
    if (!testValueConstructor<UInt64, uint64_t>(0x888884)) {
        return false;
    }
    if (!testValueConstructor<Float, float>(1.2f)) {
        return false;
    }
    if (!testValueConstructor<Double, double>(1.2)) {
        return false;
    }

    {
        DateTime value = DateTime(2010, 1, 2);
        YmlNode test(value);
        DateTime v;
        if (!DateTime::parse(test.toString(), v)) {
            return false;
        }
        if (v != value) {
            return false;
        }
    }

    {
        Vector<String> value = {"1", "2", "3"};
        YmlNode test(value);
        String str = test.toString();
        if (str != "- 1\n- 2\n- 3") {
            return false;
        }
    }
    {
        List<String> value = {"1", "2", "3"};
        YmlNode test(value);
        String str = test.toString();
        if (str != "- 1\n- 2\n- 3") {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        YmlNode test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        YmlNode test2;
        test2.evaluates(test);
        if (test != test2) {
            return false;
        }
    }
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        YmlNode test2;
        test2.evaluates(test);
        if (!(test == test2)) {
            return false;
        }
    }

    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        YmlNode test2;
        test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testAt() {
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        if (test.at(0).toString() != "Brewers") {
            return false;
        }
        if (test.at(1).toString() != "Milwaukee") {
            return false;
        }

        if (test.at("name").toString() != "Brewers") {
            return false;
        }
        if (test.at("city").toString() != "Milwaukee") {
            return false;
        }
    }

    return true;
}

bool testAttribute() {
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        if (test.getAttribute("name") != "Brewers") {
            return false;
        }
        if (test.getAttribute("city") != "Milwaukee") {
            return false;
        }

        if (!test.setAttribute("name", "Mike")) {
            return false;
        }
        if (test.getAttribute("name") != "Mike") {
            return false;
        }
    }

    {
        YmlNode test;
        StringMap map{{"name", "Brewers"},
                      {"city", "Milwaukee"}};
        test.setAttributes(map);
        if (test.getAttribute("name") != "Brewers") {
            return false;
        }
        if (test.getAttribute("city") != "Milwaukee") {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        if (test.type() != YmlNode::TypeMap) {
            return false;
        }
    }
    {
        YmlNode test("abc");
        if (test.type() != YmlNode::TypeScalar) {
            return false;
        }
    }
    {
        Vector<int> value = {1, 2, 3};
        YmlNode test(value);
        if (test.type() != YmlNode::TypeSequence) {
            return false;
        }
    }
    {
        Vector<int> value = {1, 2, 3};
        YmlNode test(value);
        if (test.size() != 3) {
            return false;
        }
    }
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        if (test.size() != 2) {
            return false;
        }
    }

    {
        Vector<int> value = {1, 2, 3};
        YmlNode test(value);
        String str = test.toString();
        test.add(YmlNode(4));
        String str2 = test.toString();
        if (test.size() != 4) {
            return false;
        }
        if (test.at(3).toString() != "4") {
            return false;
        }
    }
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        test.add("car", YmlNode("Benz"));
        if (test.size() != 3) {
            return false;
        }
        if (test.at(2).toString() != "Benz") {
            return false;
        }
        if (test.at("car").toString() != "Benz") {
            return false;
        }
    }

    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        YmlNode::Properties properties;
        if (!test.getProperties(properties)) {
            return false;
        }
        if (properties["name"] != "Brewers") {
            return false;
        }
        if (properties["city"] != "Milwaukee") {
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        YmlNode test;
        if (!YmlNode::parse("{name: Brewers, city: Milwaukee}", test)) {
            return false;
        }
        if (test.at("name").toString() != "Brewers") {
            return false;
        }
        if (test.at("city").toString() != "Milwaukee") {
            return false;
        }
    }
    {
        String content = "key1: value1\n"
                         "key2: value2\n"
                         "key3: value3\n"
                         "seq:\n"
                         "  - first element\n"
                         "  - second element\n"
                         "map:\n"
                         "  m1:\n"
                         "    k1: 1\n"
                         "    k2: 2";
        String fileName = "test.yml";
        String path = Path::combine(Path::getTempPath(), "YmlNodeTest");
        String fullFileName = Path::combine(path, fileName);
        if (!Directory::exists(path))
            Directory::createDirectory(path);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(content);
        fs.close();
        YmlNode test;
        if (!YmlNode::parseFile(fullFileName, test)) {
            return false;
        }
        if (test.at("key1") != String("value1")) {
            return false;
        }
        if (test["seq"][0] != String("first element")) {
            return false;
        }
        if (test["map"]["m1"]["k1"].toString() != "1") {
            return false;
        }
        if (Directory::exists(path))
            Directory::deleteDirectory(path);
    }

    return true;
}

bool testUpdateFile() {
//    {
//        String content = "key1: value1\n"
//                         "key2: value2\n"
//                         "key3: value3\n"
//                         "seq:\n"
//                         "  - first element\n"
//                         "  - second element\n"
//                         "map:\n"
//                         "  m1:\n"
//                         "    k1: 1\n"
//                         "    k2: 2";
//        String fileName = "test.yml";
//        String path = Path::combine(Path::getTempPath(), "YmlNodeTest");
//        String fullFileName = Path::combine(path, fileName);
//        if (!Directory::exists(path))
//            Directory::createDirectory(path);
//        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileWrite);
//        fs.writeText(content);
//        fs.close();
//        YmlNode test;
//        if (!YmlNode::parseFile(fullFileName, test)) {
//            return false;
//        }
//        YmlNode::Properties properties;
//        if (!test.getProperties(properties)) {
//            return false;
//        }
//        String fileName2 = "test2.yml";
//        String fullFileName2 = Path::combine(path, fileName2);
//        if (!YmlNode::updateFile(fullFileName2, properties)) {
//            return false;
//        }
//
//        YmlNode test2;
//        if (!YmlNode::parseFile(fullFileName2, test2)) {
//            return false;
//        }
//        YmlNode::Properties properties2;
//        if (!test2.getProperties(properties2)) {
//            return false;
//        }
//        if(properties2["key1"] != "value1") {
//            return false;
//        }
//        if(properties2["map.m1.k1"] != "1") {
//            return false;
//        }
//        if(properties2["seq[0]"] != "first element") {
//            return false;
//        }
//        if (Directory::exists(path))
//            Directory::deleteDirectory(path);
//    }

    {
        String fileName = "test.yml";
        String path = Path::combine(Path::getTempPath(), "YmlNodeTest");
        String fullFileName = Path::combine(path, fileName);
        YmlNode::Properties properties;
        properties["key1"] = "value1";
        properties["map.m1.k1"] = "1";
        properties["map.m1.k2"] = "2";
        properties["seq[0]"] = "first element";
        properties["seq[1]"] = "second element";
        if (!YmlNode::updateFile(fullFileName, properties)) {
            return false;
        }
        YmlNode test;
        if (!YmlNode::parseFile(fullFileName, test)) {
            return false;
        }
//        YmlNode::Properties properties2;
//        if (!test.getProperties(properties2)) {
//            return false;
//        }
//        StringArray keys;
//        properties.keys(keys);
//        for (size_t i=0; i<keys.count(); i++)
//        {
//            const String &key = keys[i];
//            String value;
//            properties.at(key, value);
//            String str = String::format("%s=%s", key.c_str(), value.c_str());
//            Debug::writeLine(str);
//        }

//        if (test.at("key1") != String("value1")) {
//            return false;
//        }
//        if (test["seq"][0] != String("first element")) {
//            return false;
//        }
//        if (test["map"]["m1"]["k1"].toString() != "1") {
//            return false;
//        }
        if (Directory::exists(path))
            Directory::deleteDirectory(path);
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testEquals()) {
        return 2;
    }
    if (!testAt()) {
        return 3;
    }
    if (!testAttribute()) {
        return 4;
    }
    if (!testProperty()) {
        return 5;
    }
    if (!testParse()) {
        return 6;
    }
    if (!testUpdateFile()) {
        return 7;
    }

    return 0;
}