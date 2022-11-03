//
//  MapTest.cpp
//  common
//
//  Created by baowei on 2022/11/1.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/Map.h"
#include "data/ValueType.h"

using namespace Common;

class Key : public IComparable<Key>, public IEquatable<Key>, public IEvaluation<Key> {
public:
    Key(int value = 0) : _iValue(value), _strValue(nullptr) {
    }

    Key(const String &value) : _iValue(0), _strValue(value) {
    }

    Key(const Key &value) {
        evaluates(value);
    }

    ~Key() override = default;

    int iValue() const {
        return _iValue;
    }

    String strValue() const {
        return _strValue;
    }

    String toString() const {
        if (_strValue.isNullOrEmpty()) {
            return String::format("%d", _iValue);
        } else {
            return String::format("iValue: %d, strValue: %s", _iValue, _strValue.c_str());
        }
    }

    int compareTo(const Key &other) const override {
        if (_iValue != other._iValue) {
            if (_iValue > other._iValue) {
                return 1;
            }
            return -1;
        }
        if (_strValue != other._strValue) {
            if (_strValue > other._strValue) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    void evaluates(const Key &other) override {
        _iValue = other._iValue;
        _strValue = other._strValue;
    }

    bool equals(const Key &other) const override {
        return compareTo(other) == 0;
    }

    Key &operator=(const Key &other) {
        evaluates(other);
        return *this;
    }

    bool operator==(const Key &other) const {
        return this->equals(other);
    }

    bool operator!=(const Key &other) const {
        return !this->operator==(other);
    }

    bool operator==(const int &other) const {
        return this->equals(Key(other));
    }

    bool operator!=(const int &other) const {
        return !this->operator==(other);
    }

    bool operator==(const String &other) const {
        return this->equals(Key(other));
    }

    bool operator!=(const String &other) const {
        return !this->operator==(other);
    }

private:
    int _iValue;
    String _strValue;
};

class Value : public IComparable<Value>, public IEquatable<Value>, public IEvaluation<Value> {
public:
    Value(int value = 0) : _iValue(value), _strValue(nullptr) {
    }

    Value(const String &value) : _iValue(0), _strValue(value) {
    }

    Value(const Value &value) {
        evaluates(value);
    }

    ~Value() override = default;

    int iValue() const {
        return _iValue;
    }

    String strValue() const {
        return _strValue;
    }

    String toString() const {
        if (_strValue.isNullOrEmpty()) {
            return String::format("%d", _iValue);
        } else {
            return String::format("iValue: %d, strValue: %s", _iValue, _strValue.c_str());
        }
    }

    int compareTo(const Value &other) const override {
        if (_iValue != other._iValue) {
            if (_iValue > other._iValue) {
                return 1;
            }
            return -1;
        }
        if (_strValue != other._strValue) {
            if (_strValue > other._strValue) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    void evaluates(const Value &other) override {
        _iValue = other._iValue;
        _strValue = other._strValue;
    }

    bool equals(const Value &other) const override {
        return compareTo(other) == 0;
    }

    Value &operator=(const Value &other) {
        evaluates(other);
        return *this;
    }

    bool operator==(const Value &other) const {
        return this->equals(other);
    }

    bool operator!=(const Value &other) const {
        return !this->operator==(other);
    }

    bool operator==(const int &other) const {
        return this->equals(Value(other));
    }

    bool operator!=(const int &other) const {
        return !this->operator==(other);
    }

    bool operator==(const String &other) const {
        return this->equals(Value(other));
    }

    bool operator!=(const String &other) const {
        return !this->operator==(other);
    }

private:
    int _iValue;
    String _strValue;
};

typedef Map<Key, Value> ValueMap;

bool testConstructor() {
    {
        ValueMap test;
        if (test.count() != 0) {
            return false;
        }
    }

    {
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        ValueMap test2(test);
        test2.setAutoDeleteValue(false);
        if (test.count() != 3) {
            return false;
        }
        if (test2.count() != 3) {
            return false;
        }
    }

    {
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        ValueMap test2(std::move(test));
        if (test2.count() != 3) {
            return false;
        }
        if (test.count() != 0) {
            return false;
        }
    }

    {
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        if (test.count() != 3) {
            return false;
        }
        ValueMap test2({{Key(1), new Value(1)},
                        {Key(2), new Value(2)},
                        {Key(3), new Value(3)},
                        {Key(4), new Value(4)}}, true);
        if (test2.count() != 4) {
            return false;
        }
        ValueMap test3({{Key(1), new Value(1)},
                        {Key(2), new Value(2)},
                        {Key(3), new Value(3)}}, true);
        if (test3.count() != 3) {
            return false;
        }
    }

    {
        const ValueMap test({{Key(1), new Value(1)},
                        {Key(2), new Value(2)},
                        {Key(3), new Value(3)},
                        {Key(4), new Value(4)}}, true);
        if (test.count() != 4) {
            return false;
        }
    }

    return true;
}

bool testAdd() {
    ValueMap test;
    test.add(Key(1), new Value(1));
    if (test.count() != 1) {
        return false;
    }
    Value *value = nullptr;
    if (!(test.at(Key(1), value) && value->equals(Value(1)))) {
        return false;
    }

    test.add(Key(1), new Value(0));
    if (!(test.at(Key(1), value) && value->equals(Value(0)))) {
        return false;
    }

    return true;
}

bool testContains() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
    if (!test.contains(Key(1))) {
        return false;
    }
    if (!test.contains(Key(2), Value(2))) {
        return false;
    }
    if (test.contains(Key(4))) {
        return false;
    }

    return true;
}

bool testClear() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
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
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
    if (test.count() != 3) {
        return false;
    }

    return true;
}

bool testEmpty() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
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
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        Value *value = nullptr;
        if (!test.at(Key(1), value)) {
            return false;
        }
        if (test.at(4, value)) {
            return false;
        }
    }

    {
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        Value *value = nullptr;
        value = test.at(Key(1));
        if (!(value != nullptr && value->equals(Value(1)))) {
            return false;
        }
        value = test.at(Key(4));
        if (value != nullptr) {
            return false;
        }
    }

    {
        ValueMap test({{Key(1), new Value(1)},
                       {Key(2), new Value(2)},
                       {Key(3), new Value(3)}}, true);
        Value *value = test[Key(1)];
        if (!(value != nullptr && value->equals(Value(1)))) {
            return false;
        }
        value = test[Key(4)];
        if (value != nullptr) {
            return false;
        }
    }

    return true;
}

bool testRemove() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
    if (!test.remove(2)) {
        return false;
    }
    if (test.contains(2)) {
        return false;
    }
    if (test.count() != 2) {
        return false;
    }

    return true;
}

bool testKeys() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
    Vector<Key> keys;
    test.keys(keys);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < keys.count(); ++i) {
        if (keys[i] != i + 1) {
            return false;
        }
    }

    return true;
}

bool testValues() {
    ValueMap test({{Key(1), new Value(1)},
                   {Key(2), new Value(2)},
                   {Key(3), new Value(3)}}, true);
    Vector<Value *> values;
    test.values(values);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < values.count(); ++i) {
        if (!values[i]->equals(i + 1)) {
            return false;
        }
    }

    return true;
}

bool testIterator() {
    ValueMap test;
    test.add(Key(1), new Value(1));
    test.add(Key(2), new Value(2));
    test.add(Key(3), new Value(3));

    int index = 1;
    for (auto it = test.begin(); it != test.end(); ++it) {
        const Key &k = it.key();
        const Value *v = it.value();
        if (k != index) {
            return false;
        }
        if (*v != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        const Key &k = it.key();
        const Value *v = it.value();
        if (k != index) {
            return false;
        }
        if (*v != index) {
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
    if (!testAdd()) {
        return 5;
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
    if (!testValues()) {
        return 14;
    }
    if (!testIterator()) {
        return 15;
    }

    return 0;
}