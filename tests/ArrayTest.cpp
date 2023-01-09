//
//  ArrayTest.cpp
//  common
//
//  Created by baowei on 2022/11/26.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/Array.h"
#include "data/String.h"

using namespace Data;

typedef Array<int, 3> Integers;

class Value : public IEquatable<Value>, public IEquatable<Value, int>, public IEquatable<Value, String>,
              public IComparable<Value>, public IEvaluation<Value> {
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

    bool equals(const int &other) const override {
        return _iValue == other;
    }

    bool equals(const String &other) const override {
        return _strValue == other;
    }

    Value &operator=(const Value &other) {
        evaluates(other);
        return *this;
    }

private:
    int _iValue;
    String _strValue;
};

typedef Array<Value, 3> Values;

bool testIntConstructor() {
    {
        Integers test = {1, 2, 3};
        if (test.count() != 3) {
            return false;
        }
    }

    {
        int array[] = {1, 2, 3};
        Integers test(array, 3);
        if (test.count() != 3) {
            return false;
        }
    }

    {
        Integers test = {1, 2, 3};
        Integers test2(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        Integers test = {1, 2, 3};
        Integers test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testIntCount() {
    Integers test = {1, 2, 3};
    return test.count() == 3;
}

bool testIntAt() {
    Integers test = {1, 2, 3};
    int v = test[2];
    if (v != 3) {
        return false;
    }
    int v2 = test[3];
    if (v2 != 0) {
        return false;
    }
    int v3 = test.at(2);
    if (v3 != 3) {
        return false;
    }
    int v4 = test.at(3);
    if (v4 != 0) {
        return false;
    }
    return true;
}

bool testIntIndexOf() {
    {
        Integers test = {1, 2, 3};
        ssize_t index = test.indexOf(3);
        if (index != 2) {
            return false;
        }
    }

    {
        Integers test = {1, 2, 3};
        ssize_t index = test.indexOf(4);
        if (index != -1) {
            return false;
        }
    }

    {
        Integers test = {1, 2, 3};
        ssize_t index = test.lastIndexOf(3);
        if (index != 2) {
            return false;
        }
    }

    {
        Integers test = {1, 2, 3};
        ssize_t index = test.lastIndexOf(4);
        if (index != -1) {
            return false;
        }
    }

    return true;
}

bool testIntData() {
    Integers test = {1, 2, 3};
    int *data = test.data();

    return data != nullptr;
}

bool testIntEquals() {
    Integers test = {1, 2, 3};
    Integers test2 = {1, 2, 3};
    if (test2 != test) {
        return false;
    }

    Integers test3 = {1, 2, 3};
    if (!test3.equals(test)) {
        return false;
    }

    return true;
}

bool testIntIterator() {
    Integers test = {1, 2, 3};

    int index = 1;
    for (int it: test) {
        if (it != index) {
            return false;
        }
        index++;
    }

    index = 1;
    for (int &it: test) {
        if (it != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (*it != index) {
            return false;
        }
        index--;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (*it != index) {
            return false;
        }
        index--;
    }

    return true;
}

bool testValueConstructor() {
    {
        Values test = {1, 2, 3};
        if (test.count() != 3) {
            return false;
        }
    }

    {
        Value array[] = {1, 2, 3};
        Values test(array, 3);
        if (test.count() != 3) {
            return false;
        }
    }

    {
        Values test = {1, 2, 3};
        Values test2(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        Values test = {1, 2, 3};
        Values test2 = test;
        if (test2 != test) {
            return false;
        }
    }
    {
        Values test = {1, 2, 3};
        Values test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testValueCount() {
    Values test = {1, 2, 3};
    return test.count() == 3;
}

bool testValueAt() {
    Values test = {1, 2, 3};
    auto v = test[2];
    if (v != 3) {
        return false;
    }
    auto v2 = test[3];
    if (v2 != 0) {
        return false;
    }
    auto v3 = test.at(2);
    if (v3 != 3) {
        return false;
    }
    auto v4 = test.at(3);
    if (v4 != 0) {
        return false;
    }
    return true;
}

bool testValueIndexOf() {
    {
        Values test = {1, 2, 3};
        ssize_t index = test.indexOf(3);
        if (index != 2) {
            return false;
        }
    }

    {
        Values test = {1, 2, 3};
        ssize_t index = test.indexOf(4);
        if (index != -1) {
            return false;
        }
    }

    {
        Values test = {1, 2, 3};
        ssize_t index = test.lastIndexOf(3);
        if (index != 2) {
            return false;
        }
    }

    {
        Values test = {1, 2, 3};
        ssize_t index = test.lastIndexOf(4);
        if (index != -1) {
            return false;
        }
    }

    return true;
}

bool testValueData() {
    Values test = {1, 2, 3};
    auto *data = test.data();

    return data != nullptr;
}

bool testValueEquals() {
    Values test = {1, 2, 3};
    Values test2 = {1, 2, 3};
    if (test2 != test) {
        return false;
    }

    Values test3 = {1, 2, 3};
    if (!test3.equals(test)) {
        return false;
    }

    return true;
}

bool testValueIterator() {
    Values test = {1, 2, 3};

    int index = 1;
    for (auto it: test) {
        if (it != index) {
            return false;
        }
        index++;
    }

    index = 1;
    for (auto &it: test) {
        if (it != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (*it != index) {
            return false;
        }
        index--;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (*it != index) {
            return false;
        }
        index--;
    }

    return true;
}

int main() {
    if (!testIntConstructor()) {
        return 1;
    }
    if (!testIntCount()) {
        return 2;
    }
    if (!testIntAt()) {
        return 3;
    }
    if (!testIntIndexOf()) {
        return 4;
    }
    if (!testIntData()) {
        return 5;
    }
    if (!testIntEquals()) {
        return 6;
    }
    if (!testIntIterator()) {
        return 7;
    }

    if (!testValueConstructor()) {
        return 11;
    }
    if (!testValueCount()) {
        return 12;
    }
    if (!testValueAt()) {
        return 13;
    }
    if (!testValueIndexOf()) {
        return 14;
    }
    if (!testValueData()) {
        return 15;
    }
    if (!testValueEquals()) {
        return 16;
    }
    if (!testValueIterator()) {
        return 17;
    }

    return 0;
}