//
//  StringMapTest.cpp
//  common
//
//  Created by baowei on 2022/11/2.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/StringMap.h"

using namespace Common;

bool testConstructor() {
    {
        StringMap test;
        if (test.count() != 0) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        StringMap test2(test);
        if (test2 != test) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        StringMap test2(std::move(test));
        if (test2.count() != 3) {
            return false;
        }
        if (test.count() != 0) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        if (test.count() != 3) {
            return false;
        }
        StringMap test2{{"1", "1"},
                       {"2", "2"},
                       {"3", "3"},
                       {"4", "4"}};
        if (test2.count() != 4) {
            return false;
        }
        StringMap test3{{"1", "1"},
                       {"2", "2"},
                       {"3", "3"}};
        if (test3.count() != 3) {
            return false;
        }
    }

    {
        const StringMap test{
                {"1", "1"},
                {"2", "2"},
                {"3", "3"},
                {"4", "4"},
        };
        if (test.count() != 4) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};

        StringMap test2{{"1", "1"},
                       {"2", "2"},
                       {"3", "3"}};

        if (!test2.equals(test)) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};

        StringMap test2{{"1", "1"},
                       {"2", "2"},
                       {"3", "3"}};

        if (!(test2 == test)) {
            return false;
        }
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testEvaluates() {
    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};

        StringMap test2;
        test2.evaluates(test);

        if (test2 != test) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};

        StringMap test2;
        test2 = test;

        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testCompare() {
    StringMap alice{{"1", "1"},
                   {"2", "2"},
                   {"3", "3"}};
    StringMap bob{{"7",  "7"},
                 {"8",  "8"},
                 {"9",  "9"},
                 {"10", "10"}};
    if (!(bob.compareTo(alice) > 0)) {
        return false;
    }
    if (!(bob > alice)) {
        return false;
    }
    if (!(bob >= alice)) {
        return false;
    }
    if (bob < alice) {
        return false;
    }
    if (bob <= alice) {
        return false;
    }

    return true;
}

bool testAdd() {
    StringMap test;
    test.add("1", "1");
    if (test.count() != 1) {
        return false;
    }
    if (test.at("1") != "1") {
        return false;
    }

    test.add("1", "0");
    if (test.at("1") != "0") {
        return false;
    }

    return true;
}

bool testAddRange() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};

    StringMap test2;
    test2.addRange(test);
    if (test2 != test) {
        return false;
    }

    return true;
}

bool testContains() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};

    if (!test.contains("1")) {
        return false;
    }
    if (!test.contains("2", "2")) {
        return false;
    }
    if (test.contains("4")) {
        return false;
    }

    return true;
}

bool testClear() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    if (test.count() != 3) {
        return false;
    }

    test.clear();
    if (test.count() != 0) {
        return false;
    }
    return true;
}

bool testCount() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    if (test.count() != 3) {
        return false;
    }

    return true;
}

bool testEmpty() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    if (test.isEmpty()) {
        return false;
    }

    test.clear();
    if (!test.isEmpty()) {
        return false;
    }

    return true;
}

bool testAt() {
    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        String value;
        if (!test.at("1", value)) {
            return false;
        }
        if (test.at("4", value)) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        String value;
        value = test.at("1");
        if (value != "1") {
            return false;
        }
        value = test.at("4");
        if (!value.isNullOrEmpty()) {
            return false;
        }
    }

    {
        StringMap test{{"1", "1"},
                      {"2", "2"},
                      {"3", "3"}};
        String value;
        value = test["1"];
        if (value != "1") {
            return false;
        }
        value = test["4"];
        if (!value.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testRemove() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    if (!test.remove("2")) {
        return false;
    }
    if (test.contains("2")) {
        return false;
    }
    if (test.count() != 2) {
        return false;
    }

    return true;
}

bool testKeys() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    StringArray keys;
    test.keys(keys);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < keys.count(); ++i) {
        if (keys[i] != Int32(i + 1).toString()) {
            return false;
        }
    }

    return true;
}

bool testStrings() {
    StringMap test{{"1", "1"},
                  {"2", "2"},
                  {"3", "3"}};
    StringArray values;
    test.values(values);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < values.count(); ++i) {
        if (values[i] != Int32(i + 1).toString()) {
            return false;
        }
    }

    return true;
}

bool testIterator() {
    StringMap test;
    test.add("1", "1");
    test.add("2", "2");
    test.add("3", "3");

    int index = 1;
    for (auto it = test.begin(); it != test.end(); ++it) {
        const String &k = it.key();
        const String &v = it.value();
        if (k != Int32(index).toString()) {
            return false;
        }
        if (v != Int32(index).toString()) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        const String &k = it.key();
        const String &v = it.value();
        if (k != Int32(index).toString()) {
            return false;
        }
        if (v != Int32(index).toString()) {
            return false;
        }
        index--;
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
    if (!testEvaluates()) {
        return 3;
    }
    if (!testCompare()) {
        return 4;
    }
    if (!testAdd()) {
        return 5;
    }
    if (!testAddRange()) {
        return 6;
    }
    if (!testContains()) {
        return 7;
    }
    if (!testClear()) {
        return 8;
    }
    if (!testCount()) {
        return 9;
    }
    if (!testEmpty()) {
        return 10;
    }
    if (!testAt()) {
        return 11;
    }
    if (!testRemove()) {
        return 12;
    }
    if (!testKeys()) {
        return 13;
    }
    if (!testStrings()) {
        return 14;
    }
    if (!testIterator()) {
        return 15;
    }

    return 0;
}