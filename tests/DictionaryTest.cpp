//
//  DictionaryTest.cpp
//  common
//
//  Created by baowei on 2022/11/1.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/Dictionary.h"
#include "data/ValueType.h"

using namespace Data;

class Key : public IEquatable<Key>, public IEquatable<Key, int>, public IEquatable<Key, String>,
            public IComparable<Key>, public IEvaluation<Key> {
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

    bool equals(const int &other) const override {
        return _iValue == other;
    }

    bool equals(const String &other) const override {
        return _strValue == other;
    }

    Key &operator=(const Key &other) {
        evaluates(other);
        return *this;
    }

private:
    int _iValue;
    String _strValue;
};

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

typedef Dictionary<int, int> IntMap;
typedef Dictionary<Key, Value> ValueMap;

bool testIntConstructor() {
    {
        IntMap test;
        if (test.count() != 0) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        IntMap test2(test);
        if (test2 != test) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        IntMap test2(std::move(test));
        if (test2.count() != 3) {
            return false;
        }
        if (test.count() != 0) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        if (test.count() != 3) {
            return false;
        }
        IntMap test2{{1, 1},
                     {2, 2},
                     {3, 3},
                     {4, 4}};
        if (test2.count() != 4) {
            return false;
        }
        IntMap test3{{1, 1},
                     {2, 2},
                     {3, 3}};
        if (test3.count() != 3) {
            return false;
        }
    }

    {
        const IntMap test{
                {1, 1},
                {2, 2},
                {3, 3},
                {4, 4},
        };
        if (test.count() != 4) {
            return false;
        }
    }

    return true;
}

bool testIntEquals() {
    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};

        IntMap test2{{1, 1},
                     {2, 2},
                     {3, 3}};

        if (!test2.equals(test)) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};

        IntMap test2{{1, 1},
                     {2, 2},
                     {3, 3}};

        if (!(test2 == test)) {
            return false;
        }
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testIntEvaluates() {
    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};

        IntMap test2;
        test2.evaluates(test);

        if (test2 != test) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};

        IntMap test2;
        test2 = test;

        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testIntCompare() {
    IntMap alice{{1, 1},
                 {2, 2},
                 {3, 3}};
    IntMap bob{{7,  7},
               {8,  8},
               {9,  9},
               {10, 10}};
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

bool testIntAdd() {
    IntMap test;
    test.add(1, 1);
    if (test.count() != 1) {
        return false;
    }
    if (test.at(1) != 1) {
        return false;
    }

    test.add(1, 0);
    if (test.at(1) != 0) {
        return false;
    }

    return true;
}

bool testIntAddRange() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};

    IntMap test2;
    test2.addRange(test);
    if (test2 != test) {
        return false;
    }

    return true;
}

bool testIntContains() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};

    if (!test.contains(1)) {
        return false;
    }
    if (!test.contains(2, 2)) {
        return false;
    }
    if (test.contains(4)) {
        return false;
    }

    return true;
}

bool testIntClear() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
    if (test.count() != 3) {
        return false;
    }

    test.clear();
    if (test.count() != 0) {
        return false;
    }
    return true;
}

bool testIntCount() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
    if (test.count() != 3) {
        return false;
    }

    return true;
}

bool testIntEmpty() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
    if (test.isEmpty()) {
        return false;
    }

    test.clear();
    if (!test.isEmpty()) {
        return false;
    }

    return true;
}

bool testIntAt() {
    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        int value;
        if (!test.at(1, value)) {
            return false;
        }
        if (test.at(4, value)) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        int value;
        value = test.at(1);
        if (value != 1) {
            return false;
        }
        value = test.at(4);
        if (value != 0) {
            return false;
        }
    }

    {
        IntMap test{{1, 1},
                    {2, 2},
                    {3, 3}};
        int value;
        value = test[1];
        if (value != 1) {
            return false;
        }
        value = test[4];
        if (value != 0) {
            return false;
        }
    }

    return true;
}

bool testIntSet() {
    ValueMap test{{1, 1},
                  {2, 2},
                  {3, 3}};
    if(!test.set(1, 0)) {
        return false;
    }
    if(test[1] != 0) {
        return false;
    }

    if(test.set(4, 4)) {
        return false;
    }

    return true;
}

bool testIntRemove() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
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

bool testIntKeys() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
    Vector<int> keys;
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

bool testIntValues() {
    IntMap test{{1, 1},
                {2, 2},
                {3, 3}};
    Vector<int> values;
    test.values(values);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < values.count(); ++i) {
        if (values[i] != i + 1) {
            return false;
        }
    }

    return true;
}

bool testIntIterator() {
    IntMap test;
    test.add(1, 1);
    test.add(2, 2);
    test.add(3, 3);

    int index = 1;
    for (auto it = test.begin(); it != test.end(); ++it) {
        int k = it.key();
        int v = it.value();
        if (k != index) {
            return false;
        }
        if (v != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        int k = it.key();
        int v = it.value();
        if (k != index) {
            return false;
        }
        if (v != index) {
            return false;
        }
        index--;
    }

    return true;
}

bool testConstructor() {
    {
        ValueMap test;
        if (test.count() != 0) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        ValueMap test2(test);
        if (test2 != test) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        ValueMap test2(std::move(test));
        if (test2.count() != 3) {
            return false;
        }
        if (test.count() != 0) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        if (test.count() != 3) {
            return false;
        }
        ValueMap test2{{Key(1), Value(1)},
                       {Key(2), Value(2)},
                       {Key(3), Value(3)},
                       {Key(4), Value(4)}};
        if (test2.count() != 4) {
            return false;
        }
        ValueMap test3{{Key(1), Value(1)},
                       {Key(2), Value(2)},
                       {Key(3), Value(3)}};
        if (test3.count() != 3) {
            return false;
        }
    }

    {
        const ValueMap test{
                {Key(1), Value(1)},
                {Key(2), Value(2)},
                {Key(3), Value(3)},
                {Key(4), Value(4)},
        };
        if (test.count() != 4) {
            return false;
        }
    }

    return true;
}

bool testEquals() {
    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};

        ValueMap test2{{Key(1), Value(1)},
                       {Key(2), Value(2)},
                       {Key(3), Value(3)}};

        if (!test2.equals(test)) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};

        ValueMap test2{{Key(1), Value(1)},
                       {Key(2), Value(2)},
                       {Key(3), Value(3)}};

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
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};

        ValueMap test2;
        test2.evaluates(test);

        if (test2 != test) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};

        ValueMap test2;
        test2 = test;

        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testCompare() {
    ValueMap alice{{Key(1), Value(1)},
                   {Key(2), Value(2)},
                   {Key(3), Value(3)}};
    ValueMap bob{{7,  7},
                 {8,  8},
                 {9,  9},
                 {10, 10}};
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
    ValueMap test;
    test.add(1, 1);
    if (test.count() != 1) {
        return false;
    }
    if (test.at(1) != 1) {
        return false;
    }

    test.add(1, 0);
    if (test.at(1) != 0) {
        return false;
    }

    return true;
}

bool testAddRange() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};

    ValueMap test2;
    test2.addRange(test);
    if (test2 != test) {
        return false;
    }

    return true;
}

bool testContains() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};

    if (!test.contains(1)) {
        return false;
    }
    if (!test.contains(2, 2)) {
        return false;
    }
    if (test.contains(4)) {
        return false;
    }

    return true;
}

bool testClear() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
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
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
    if (test.count() != 3) {
        return false;
    }

    return true;
}

bool testEmpty() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
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
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        Value value;
        if (!test.at(Key(1), value)) {
            return false;
        }
        if (test.at(4, value)) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        Value value;
        value = test.at(Key(1));
        if (value != 1) {
            return false;
        }
        value = test.at(Key(4));
        if (value != 0) {
            return false;
        }
    }

    {
        ValueMap test{{Key(1), Value(1)},
                      {Key(2), Value(2)},
                      {Key(3), Value(3)}};
        Value value;
        value = test[Key(1)];
        if (value != 1) {
            return false;
        }
        value = test[Key(4)];
        if (value != 0) {
            return false;
        }
    }

    return true;
}

bool testSet() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
    if (!test.set(Key(1), Value(0))) {
        return false;
    }
    if (test[Key(1)] != Value(0)) {
        return false;
    }

    if (test.set(Key(4), Value(4))) {
        return false;
    }

    return true;
}

bool testRemove() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
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
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
    Vector<Key> keys;
    test.keys(keys);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < keys.count(); ++i) {
        if (keys[i] != (int)(i + 1)) {
            return false;
        }
    }

    return true;
}

bool testValues() {
    ValueMap test{{Key(1), Value(1)},
                  {Key(2), Value(2)},
                  {Key(3), Value(3)}};
    Vector<Value> values;
    test.values(values);
    if (test.count() != 3) {
        return false;
    }
    for (size_t i = 0; i < values.count(); ++i) {
        if (values[i] != (int)(i + 1)) {
            return false;
        }
    }

    return true;
}

bool testIterator() {
    ValueMap test;
    test.add(1, 1);
    test.add(2, 2);
    test.add(3, 3);

    int index = 1;
    for (auto it = test.begin(); it != test.end(); ++it) {
        const Key &k = it.key();
        const Value &v = it.value();
        if (k != index) {
            return false;
        }
        if (v != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        const Key &k = it.key();
        const Value &v = it.value();
        if (k != index) {
            return false;
        }
        if (v != index) {
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
    if (!testIntEquals()) {
        return 2;
    }
    if (!testIntEvaluates()) {
        return 3;
    }
    if (!testIntCompare()) {
        return 4;
    }
    if (!testIntAdd()) {
        return 5;
    }
    if (!testIntAddRange()) {
        return 6;
    }
    if (!testIntContains()) {
        return 7;
    }
    if (!testIntClear()) {
        return 8;
    }
    if (!testIntCount()) {
        return 9;
    }
    if (!testIntEmpty()) {
        return 10;
    }
    if (!testIntAt()) {
        return 11;
    }
    if (!testIntSet()) {
        return 12;
    }
    if (!testIntRemove()) {
        return 13;
    }
    if (!testIntKeys()) {
        return 14;
    }
    if (!testIntValues()) {
        return 15;
    }
    if (!testIntIterator()) {
        return 16;
    }

    if (!testConstructor()) {
        return 21;
    }
    if (!testEquals()) {
        return 22;
    }
    if (!testEvaluates()) {
        return 23;
    }
    if (!testCompare()) {
        return 24;
    }
    if (!testAdd()) {
        return 25;
    }
    if (!testAddRange()) {
        return 26;
    }
    if (!testContains()) {
        return 27;
    }
    if (!testClear()) {
        return 28;
    }
    if (!testCount()) {
        return 29;
    }
    if (!testEmpty()) {
        return 30;
    }
    if (!testAt()) {
        return 31;
    }
    if(! testSet()) {
        return 32;
    }
    if (!testRemove()) {
        return 33;
    }
    if (!testKeys()) {
        return 34;
    }
    if (!testValues()) {
        return 35;
    }
    if (!testIterator()) {
        return 36;
    }

    return 0;
}