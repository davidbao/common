//
//  JsonNodeTest.cpp
//  common
//
//  Created by baowei on 2022/10/21.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "json/JsonNode.h"
#include "diag/Trace.h"
#include "data/DateTime.h"
#include "data/Version.h"

using namespace Json;

template<class parseType, class keyType>
bool testValueConstructor(JsonNode::Type type, const keyType &value) {
    JsonNode test("abc", value);
    if (test.type() != type) {
        return false;
    }
    if (test.name() != "abc") {
        return false;
    }
    keyType v;
    if (!parseType::parse(test.value(), v)) {
        return false;
    }
    if (v != value) {
        return false;
    }
    return true;
}

bool testConstructor() {
    {
        JsonNode test;
        if (test.type() != JsonNode::TypeNode) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeNone);
        if (test.type() != JsonNode::TypeNone) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeNode);
        if (test.type() != JsonNode::TypeNode) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeArray);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeString);
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeNumber);
        if (test.type() != JsonNode::TypeNumber) {
            return false;
        }
    }
    {
        JsonNode test(JsonNode::TypeBoolean);
        if (test.type() != JsonNode::TypeBoolean) {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2(test);
        if (test2.toString() != R"({"test1":"1","test2":"abc","test3":"true"})") {
            return false;
        }
    }
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2 = test;
        if (test2.toString() != R"({"test1":"1","test2":"abc","test3":"true"})") {
            return false;
        }
    }

    {
        JsonNode test("abc");
        if (test.type() != JsonNode::TypeNode) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeNone);
        if (test.type() != JsonNode::TypeNone) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeNode);
        if (test.type() != JsonNode::TypeNode) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeArray);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeString);
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeNumber);
        if (test.type() != JsonNode::TypeNumber) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }
    {
        JsonNode test("abc", JsonNode::TypeBoolean);
        if (test.type() != JsonNode::TypeBoolean) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
    }

    if (!testValueConstructor<Boolean, bool>(JsonNode::TypeBoolean, true)) {
        return false;
    }
    if (!testValueConstructor<Char, char>(JsonNode::TypeNumber, 'T')) {
        return false;
    }
    if (!testValueConstructor<Int8, int8_t>(JsonNode::TypeNumber, 0x14)) {
        return false;
    }
    if (!testValueConstructor<UInt8, uint8_t>(JsonNode::TypeNumber, 0x84)) {
        return false;
    }
    if (!testValueConstructor<Int16, int16_t>(JsonNode::TypeNumber, 0x114)) {
        return false;
    }
    if (!testValueConstructor<UInt16, uint16_t>(JsonNode::TypeNumber, 0x884)) {
        return false;
    }
    if (!testValueConstructor<Int32, int32_t>(JsonNode::TypeNumber, 0x188814)) {
        return false;
    }
    if (!testValueConstructor<UInt32, uint32_t>(JsonNode::TypeNumber, 0x888884)) {
        return false;
    }
    if (!testValueConstructor<Int64, int64_t>(JsonNode::TypeNumber, 0x188814)) {
        return false;
    }
    if (!testValueConstructor<UInt64, uint64_t>(JsonNode::TypeNumber, 0x888884)) {
        return false;
    }
    if (!testValueConstructor<Float, float>(JsonNode::TypeNumber, 1.2f)) {
        return false;
    }
    if (!testValueConstructor<Double, double>(JsonNode::TypeNumber, 1.2)) {
        return false;
    }
    {
        JsonNode test("abc", "abc123");
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.value() != "abc123") {
            return false;
        }
    }
    {
        JsonNode test("abc", String("abc123"));
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.value() != "abc123") {
            return false;
        }
    }

    {
        JsonNode test("abc", DateTime(2010, 1, 2));
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        DateTime v;
        if (!DateTime::parse(test.value(), v)) {
            return false;
        }
        if (v != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        JsonNode test("abc", Version(2010, 1, 2));
        if (test.type() != JsonNode::TypeString) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        Version v;
        if (!Version::parse(test.value(), v)) {
            return false;
        }
        if (v != Version(2010, 1, 2)) {
            return false;
        }
    }

    {
        Vector<String> texts{"abc", "bcd", "cde", "def"};
        JsonNode test("abc", texts);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.toString(false) != R"(["abc","bcd","cde","def"])") {
            return false;
        }
    }
    {
        List<String> texts{"abc", "bcd", "cde", "def"};
        JsonNode test("abc", texts);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.toString(false) != R"(["abc","bcd","cde","def"])") {
            return false;
        }
    }
    {
        StringArray texts{"abc", "bcd", "cde", "def"};
        JsonNode test("abc", texts);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.toString(false) != R"(["abc","bcd","cde","def"])") {
            return false;
        }
    }

    {
        StringArray texts{"abc", "bcd", "cde", "def"};
        JsonNode test("abc", texts);
        if (test.type() != JsonNode::TypeArray) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        StringArray texts2;
        if (!test.getAttribute(texts2)) {
            return false;
        }
        if (texts != texts2) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2;
        test2.evaluates(test);
        if (test != test2) {
            return false;
        }
    }
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2;
        test2.evaluates(test);
        if (!(test == test2)) {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode test2;
        test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }

    {
        StringMap test;
        JsonNode node("test", R"({"test1":1,"test2":"abc","test3":true})");
        node.getAttribute(test);
        if (!(test["test1"] == "1" && test["test2"] == "abc" && test["test3"] == "true")) {
            return false;
        }
    }

    {
        StringMap test;
        JsonNode node("test", R"({"test1":1,"test2":"abc","test3":true,})");
        node.getAttribute(test);
        if (test.count() > 0) {
            return false;
        }
    }

    {
        StringMap test;
        JsonNode node("test", R"({""test2":"abc","test3":true})");
        node.getAttribute(test);
        if (test.count() > 0) {
            return false;
        }
    }

    {
        StringMap test;
        JsonNode node("test", "{abc}");
        node.getAttribute(test);
        if (test.count() > 0) {
            return false;
        }
    }

    {
        StringMap test;
        JsonNode node("test", "{}");
        node.getAttribute(test);
        if (test.count() > 0) {
            return false;
        }
    }

    return true;
}

bool testAt() {
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        if (test.at(0).value() != "1") {
            return false;
        }
        if (test.at(1).value() != "abc") {
            return false;
        }
        if (test.at(2).value() != "true") {
            return false;
        }
        if (!test.at(4).isEmpty()) {
            return false;
        }

        if (test.at("test1").value() != "1") {
            return false;
        }
        if (test.at("test2").value() != "abc") {
            return false;
        }
        if (test.at("test3").value() != "true") {
            return false;
        }
        if (!test.at("test4").isEmpty()) {
            return false;
        }
        String value;
        if (test.getAttribute("test4", value)) {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v;
        if (!test.at(0, v)) {
            return false;
        }
        if (v.name() != "test1") {
            return false;
        }
        if (v.value() != "1") {
            return false;
        }
    }
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v;
        if (test.at(4, v)) {
            return false;
        }
        if (!v.isEmpty()) {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v;
        if (!test.at("test1", v)) {
            return false;
        }
        if (v.name() != "test1") {
            return false;
        }
        if (v.value() != "1") {
            return false;
        }
    }
    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v;
        if (test.at("test4", v)) {
            return false;
        }
        if (!v.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        JsonNode test;
        test.add(JsonNode("test1", 1));
        test.add(JsonNode("test2", "abc"));
        test.add(JsonNode("test3", true));
        if (test.toString() != R"({"test1":1,"test2":"abc","test3":true})") {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v = test.at("test1");
        if (v.value() != "1") {
            return false;
        }
        v.setValue("2");
        if (v.value() != "2") {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        JsonNode v = test.at("test1");
        v.setComment("test1.comment");
        if (v.comment() != "test1.comment") {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        if (test.count() != 3) {
            return false;
        }
    }

    {
        JsonNode test{
                {"test1", "1"},
                {"test2", "abc"},
                {"test3", "true"},
        };
        if (test.isEmpty()) {
            return false;
        }
    }

    {
        StringMap value{{"test1", "1"},
                       {"test2", "2"},
                       {"test3", "3"}};
        JsonNode test;
        test.addRange(value);
        if (test.toString() != R"({"test1":"1","test2":"2","test3":"3"})") {
            return false;
        }
    }

    return true;
}

template<class type>
bool getAttribute(const JsonNode &node, const String &name, const type &value) {
    type v;
    if (!node.getAttribute(name, v)) {
        return false;
    }
    if (v != value) {
        return false;
    }
    return true;
}

bool testAttribute() {
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        StringArray names;
        test.getAttributeNames(names);
        if (names.count() != 3) {
            return false;
        }
        if (!(names[0] == "test1" && names[1] == "test2" && names[2] == "test3")) {
            return false;
        }
    }

    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        String value;
        if (!test.getAttribute("test1", value)) {
            return false;
        }
        if (value != "1") {
            return false;
        }
    }
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
                {"test4", 'T'},
                {"test5", 1.4f},
                {"test6", 1.5},
                {"test7", DateTime(2010, 1, 2)},
        });
        if (!getAttribute(test, "test3", true)) {
            return false;
        }
        if (!getAttribute(test, "test4", 'T')) {
            return false;
        }
        if (!getAttribute(test, "test1", (int8_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (uint8_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (int16_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (uint16_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (int32_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (uint32_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (int64_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test1", (uint64_t) 1)) {
            return false;
        }
        if (!getAttribute(test, "test5", 1.4f)) {
            return false;
        }
        if (!getAttribute(test, "test6", 1.5)) {
            return false;
        }
        if (!getAttribute(test, "test7", DateTime(2010, 1, 2))) {
            return false;
        }

        StringMap values;
        if (!test.getAttribute(values)) {
            return false;
        }
        if (values.count() != 7) {
            return false;
        }
//        for (auto it = values.begin(); it != values.end(); ++it) {
//            const String &k = it.key();
//            const String &v = it.value();
//            printf("key: %s, value: %s\n", k.c_str(), v.c_str());
//        }
        if (!(values["test1"] == "1" && values["test2"] == "abc" && values["test3"] == "true" &&
              values["test4"] == "84" &&
              values["test5"] == "1.4" && values["test6"] == "1.5" && values["test7"] == "2010-01-02 00:00:00")) {
            return false;
        }
    }

    {
        JsonNode test("abc", {"1", "abc", "true", "T", "1.4", "1.5", DateTime(2010, 1, 2).toString("d")});
        StringArray values;
        if (!test.getAttribute(values)) {
            return false;
        }
        if (values.count() != 7) {
            return false;
        }
        if (!(values[0] == "1" && values[1] == "abc" && values[2] == "true" && values[3] == "T" &&
              values[4] == "1.4" && values[5] == "1.5" && values[6] == "2010-01-02")) {
            return false;
        }
    }

    {
        JsonNode test;
        JsonNode test2("abc", {"1", "2", "3", "4"});
        test.add(test2);
        Vector<Int32> values;
        if (!test.getAttribute("abc", values)) {
            return false;
        }
        if (values.count() != 4) {
            return false;
        }
        if (!(values[0] == 1 && values[1] == 2 && values[2] == 3 && values[3] == 4)) {
            return false;
        }
    }
    {
        JsonNode test;
        JsonNode test2("abc", {"1", "2", "3", "4"});
        test.add(test2);
        List<Int32> values;
        if (!test.getAttribute("abc", values)) {
            return false;
        }
        if (values.count() != 4) {
            return false;
        }
        if (!(values[0] == 1 && values[1] == 2 && values[2] == 3 && values[3] == 4)) {
            return false;
        }
    }

    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        if (!test.hasAttribute()) {
            return false;
        }
    }
    {
        JsonNode test;
        JsonNode test2("abc", {"1", "2", "3", "4"});
        test.add(test2);
        if (!test.hasAttribute("abc")) {
            return false;
        }
    }
    {
        JsonNode test;
        if (test.hasAttribute()) {
            return false;
        }
    }

    {
        JsonNode test;
        JsonNode test2("abc", {"1", "2", "3", "4"});
        test.add(test2);
        if (!test.hasSubNodes()) {
            return false;
        }
    }
    {
        JsonNode test;
        JsonNode::parse(R"({"test":{"v1":1,"v2":"2","v3":{"v4":5,"v5":"6"}}})", test);
        JsonNodes nodes;
        test.subNodes(nodes);
        if (nodes.count() != 1) {
            return false;
        }
        if (nodes[0].name() != "test") {
            return false;
        }
    }

    return true;
}

bool testToString() {
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        if (test.toString() != R"({"test1":1,"test2":"abc","test3":true})") {
            return false;
        }
    }
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        if (test.toString(false) != R"({"test1":1,"test2":"abc","test3":true})") {
            return false;
        }
    }
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        String str = test.toString(true);
        if (test.toString(true) != "{\n\t\"test1\" : 1,\n\t\"test2\" : \"abc\",\n\t\"test3\" : true\n}") {
            return false;
        }
    }

    return true;
}

bool testParse() {
    {
        JsonNode test;
        if (!JsonNode::parse(R"({"test1":1,"test2":"abc","test3":true})", test)) {
            return false;
        }
        if (!(test["test1"].value() == "1" && test["test2"].value() == "abc" && test["test3"].value() == "true")) {
            return false;
        }
    }
    {
        JsonNode test;
        if (!JsonNode::parse(R"({"test1":1,"test1":"abc","test3":true})", test)) {
            return false;
        }
        String str = test.toString();
        if (!(test["test1"].value() == "1" && test["test3"].value() == "true")) {
            return false;
        }
    }
    {
        JsonNode test;
        if (JsonNode::parse(R"("test1":1,"test1":"abc","test3":true})", test)) {
            return false;
        }
    }
    {
        JsonNode test;
        if (JsonNode::parse("abc", test)) {
            return false;
        }
    }

    {
        StringMap test;
        if (!JsonNode::parse(R"({"test1":1,"test2":"abc","test3":true})", test)) {
            return false;
        }
        if (!(test["test1"] == "1" && test["test2"] == "abc" && test["test3"] == "true")) {
            return false;
        }
    }

    {
        StringMap test;
        if (JsonNode::parse(R"({"test1":1,"test2":"abc","test3":true,})", test)) {
            return false;
        }
    }

    {
        StringMap test;
        if (JsonNode::parse(R"({""test2":"abc","test3":true})", test)) {
            return false;
        }
    }

    {
        StringMap test;
        if (JsonNode::parse("abc", test)) {
            return false;
        }
    }

    {
        StringMap test;
        if (JsonNode::parse("", test)) {
            return false;
        }
    }

    return true;
}

bool testCopyTo() {
    {
        JsonNode test("abc", {
                {"test1", 1},
                {"test2", "abc"},
                {"test3", true},
        });
        YmlNode node;
        if (!test.copyTo(node)) {
            return false;
        }
        int v1;
        if (!node.getAttribute("test1", v1)) {
            return false;
        }
        if (v1 != 1) {
            return false;
        }
        String v2;
        if (!node.getAttribute("test2", v2)) {
            return false;
        }
        if (v2 != "abc") {
            return false;
        }
        bool v3;
        if (!node.getAttribute("test3", v3)) {
            return false;
        }
        if (v3 != true) {
            return false;
        }
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
    if (!testProperty()) {
        return 4;
    }
    if (!testAttribute()) {
        return 5;
    }
    if (!testToString()) {
        return 6;
    }
    if (!testParse()) {
        return 7;
    }
    if (!testCopyTo()) {
        return 8;
    }

    return 0;
}