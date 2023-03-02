//
//  ListTest.cpp
//  common
//
//  Created by baowei on 2022/10/27.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/List.h"
#include "data/ValueType.h"
#include "thread/Thread.h"

using namespace Data;

#define DefaultCapacity 125

typedef List<int> Integers;
typedef SortedList<int> SortedIntegers;

class Value : public IEquatable<Value>, public IEquatable<Value, int>, public IEquatable<Value, String>,
              public IComparable<Value>, public IEvaluation<Value> {
public:
    Value(int value = 0) : _iValue(value), _strValue(nullptr) {
    }

    Value(const String &value) : _iValue(0), _strValue(value) {
    }

    Value(const Value &value) : _iValue(0), _strValue(nullptr) {
        Value::evaluates(value);
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

typedef List<Value> Values;
typedef SortedList<Value> SortedValues;

void printValues(const Integers &v) {
    for (size_t i = 0; i < v.count(); ++i) {
        printf("%d; ", v[i]);
    }
    printf("\n");
}

void printValues(const Values &v) {
    for (size_t i = 0; i < v.count(); ++i) {
        printf("%s; ", v[i].toString().c_str());
    }
    printf("\n");
}

bool valuesEquals(const Values &x, const Values &y) {
    return x == y;
}

bool valueEquals(const Value &x, const Value &y) {
    return x == y;
}

bool valueEquals(const Value &x, int y) {
    return x.iValue() == y;
}

bool valueEquals(const Value &x, const String &y) {
    return x.strValue() == y;
}

int comparison(const int &x, const int &y) {
    if (x > y)
        return 1;
    else if (x == y)
        return 0;
    else
        return -1;
}

int comparison(const Value &x, const Value &y) {
    return x.compareTo(y);
}

template<class T>
class AscComparer : public IComparer<T> {
public:
    AscComparer() = default;

    int compare(const T &x, const T &y) const override {
        if (x > y)
            return 1;
        else if (x == y)
            return 0;
        else
            return -1;
    }
};

bool testIntConstructor() {
    {
        Integers test(DefaultCapacity);
        if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
            return false;
        }
    }

    {
        Integers test2(DefaultCapacity);
        test2.add(1);
        test2.add(2);
        Integers test3(test2);
        if (test2 != test3) {
            return false;
        }
    }

    {
        Integers test2_1(DefaultCapacity);
        test2_1.add(1);
        test2_1.add(2);
        Integers test3_1(test2_1, 1, 1);
        if (!(test3_1.count() == 1 && test3_1[0] == 2)) {
            return false;
        }
    }

    {
        Integers test2_2(DefaultCapacity);
        test2_2.add(1);
        test2_2.add(2);
        Integers test3_2(std::move(test2_2));
        if (!test2_2.isEmpty()) {
            return false;
        }
        if (!(test3_2.count() == 2 && test3_2[0] == 1 && test3_2[1] == 2)) {
            return false;
        }
    }

    {
        static const int count = 5;
        int array[count] = {1, 2, 3, 4, 5};
        Integers test4(array, count);
        if (test4.count() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            if (array[i] != test4[i]) {
                return false;
            }
        }
    }

    {
        static const int value = 6;
        static const int count2 = 120;
        Integers test5(value, count2);
        if (test5.count() != count2) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (value != test5[i]) {
                return false;
            }
        }
    }

    {
        static const int count = 5;
        Integers test{1, 2, 3, 4, 5};
        if(test.count() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            if (test[i] != i + 1) {
                return false;
            }
        }
    }

    return true;
}

bool testIntCount() {
    Integers test(DefaultCapacity);
    test.add(1);
    test.add(2);
    return test.count() == 2;
}

bool testIntSetCapacity() {
    Integers test(DefaultCapacity);
    // set capacity if test is empty.
    test.setCapacity(50);
    if (!(test.capacity() == 50 && test.count() == 0)) {
        return false;
    }

    // add some data in test.
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Integers test2(test);
    if (test != test2) {
        return false;
    }

    // set capacity if test is not empty and the capacity is less than count.
    test.setCapacity(25);
    if (test != test2) {
        return false;
    }

    // set capacity if test is not empty and the capacity is greater than count.
    test.setCapacity(DefaultCapacity);
    if (test != test2 && test.capacity() == DefaultCapacity) {
        return false;
    }
    test.setCapacity(255);
    if (test != test2 && test.capacity() == 255) {
        return false;
    }
    return true;
}

bool testIntAt() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    int v = test[99];
    if (v != 99) {
        return false;
    }
    int v2 = test[200];
    if (v2 != 0) {
        return false;
    }
    int v3 = test.at(99);
    if (v3 != 99) {
        return false;
    }
    int v4 = test.at(200);
    if (v4 != 0) {
        return false;
    }
    return true;
}

bool testIntAdd() {
    Integers test(DefaultCapacity);
    test.add(0);
    test.add(1);
    if (test[0] != 0 && test[1] != 1) {
        return false;
    }
    int count = DefaultCapacity + 32;
    for (int i = 2; i < count; ++i) {
        test.add(i);
    }
    if (test[2] != 2 && test[test.count() - 1] != count - 1) {
        return false;
    }
    return true;
}

bool testIntAddRange() {
    {
        Integers test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(i);
        }
        Integers test2;
        if (!test2.addRange(test)) {
            return false;
        }
        if (test != test2) {
            return false;
        }

        Integers test3;
        if (!test3.addRange(test, 50, test.count() - 50)) {
            return false;
        }
        if (!(test3[0] == 50 && test3[49] == 99)) {
            return false;
        }

        Integers test4;
        static const int count2 = 5;
        int array[count2] = {1, 2, 3, 4, 5};
        if (!test4.addRange(array, count2)) {
            return false;
        }
        if (test4.count() != count2) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (array[i] != test4[i]) {
                return false;
            }
        }
    }

    {
        Integers test;
        test.addRange({1, 2, 3, 4});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], i + 1)) {
                return false;
            }
        }
    }

    return true;
}

bool testIntInsertRange() {
    {
        Integers test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(i);
        }
        Integers test2;
        test2.addRange(test);
        Integers test3;
        for (int i = 0; i < 10; ++i) {
            test3.add(0);
        }
        if (!test2.insertRange(10, test3)) {
            return false;
        }
        if (!(test2[10] == 0 && test2[19] == 0 && test2.count() == test.count() + 10)) {
            return false;
        }

        test2.clear();
        test2.addRange(test);
        if (!test2.insertRange(10, test3, 0, 5)) {
            return false;
        }
        if (!(test2[10] == 0 && test2[14] == 0 && test2[15] != 0 && test2.count() == test.count() + 5)) {
            return false;
        }

        Integers test4;
        test4.addRange(test);
        static const int count2 = 5;
        int array[count2] = {1, 2, 3, 4, 5};
        if (!test4.insertRange(29, array, count2)) {
            return false;
        }
        if (test2.count() != test.count() + count2) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (test4[i + 29] != array[i]) {
                return false;
            }
        }
    }

    {
        Integers test;
        test.insertRange(0, {1, 2, 3, 4});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], i + 1)) {
                return false;
            }
        }
    }

    return true;
}

bool testIntInsert() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Integers test2;
    test2.addRange(test);
    if (!test2.insert(6, 111)) {
        return false;
    }
    if (!(test2[6] == 111 && test2.count() == test.count() + 1)) {
        return false;
    }

    // out of range.
    Integers test3;
    test3.addRange(test);
    if (test3.insert(200, 111)) {
        return false;
    }
    return true;
}

bool testIntSetRange() {
    {
        Integers test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(i);
        }
        Integers test2;
        test2.addRange(test);
        Integers test3;
        for (int i = 0; i < 10; ++i) {
            test3.add(0);
        }
        if (!test2.setRange(10, test3)) {
            return false;
        }
        if (!(test2[10] == 0 && test2[19] == 0 && test2.count() == test.count())) {
            return false;
        }

        test2.clear();
        test2.addRange(test);
        if (!test2.setRange(10, test3, 0, 5)) {
            return false;
        }
        if (!(test2[10] == 0 && test2[14] == 0 && test2[15] != 0 && test2.count() == test.count())) {
            return false;
        }

        Integers test4;
        test4.addRange(test);
        static const int count2 = 5;
        int array[count2] = {1, 2, 3, 4, 5};
        if (!test4.setRange(29, array, count2)) {
            return false;
        }
        if (test4.count() != test.count()) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (test4[i + 29] != array[i]) {
                return false;
            }
        }
    }

    {
        Integers test(0, 4);
        test.setRange(0, {1, 2, 3, 4});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], i + 1)) {
                return false;
            }
        }
    }

    return true;
}

bool testIntSet() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Integers test2;
    test2.addRange(test);
    if (!test2.set(6, 111)) {
        return false;
    }
    if (!(test2[6] == 111 && test2.count() == test.count())) {
        return false;
    }

    Integers test3;
    test3.addRange(test);
    test3[6] = 111;
    if (!(test3[6] == 111 && test3.count() == test.count())) {
        return false;
    }

    // out of range.
    Integers test4;
    test4.addRange(test);
    if (test4.set(200, 111)) {
        return false;
    }

    // out of range if it can be inserted empty.
    Integers test5;
    test5.addRange(test);
    if (test5.set(200, 111)) {
        return false;
    }

    return true;
}

bool testIntQuickSort() {
    SortedIntegers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }

    // sort desc.
    test.sort(false);
//    printValues(test);
    for (int i = 0; i < count; ++i) {
        if (test[count - 1 - i] != i) {
            return false;
        }
    }

    // sort asc.
    test.sort(true);
    for (int i = 0; i < count; ++i) {
        if (test[i] != i) {
            return false;
        }
    }

    // sort desc by comparison.
    test.sort(comparison, false);
    for (int i = 0; i < count; ++i) {
        if (test[count - 1 - i] != i) {
            return false;
        }
    }

    // sort asc by comparison.
    test.sort(comparison, true);
    for (int i = 0; i < count; ++i) {
        if (test[i] != i) {
            return false;
        }
    }

    // sort desc by comparer.
    test.sort(AscComparer<int>(), false);
//    printf("\n");
//    for (int i = 0; i < count; ++i) {
//        printf("%d, ", test[i]);
//    }
    for (int i = 0; i < count; ++i) {
        if (test[count - 1 - i] != i) {
            return false;
        }
    }

    return true;
}

bool testIntRemove() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    if (!test.remove(50)) {
        return false;
    }
    if (!(test.count() == count - 1 && test[50] == 51)) {
        return false;
    }

    // out of range.
    Integers test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test2.remove(count + 1)) {
        return false;
    }

    Integers test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    if (!test3.removeAt(50)) {
        return false;
    }
    if (!(test3.count() == count - 1 && test3[50] == 51)) {
        return false;
    }

    Integers test4;
    for (int i = 0; i < count; ++i) {
        test4.add(i);
    }

    if (!test4.removeRange(50, 2)) {
        return false;
    }
    if (!(test4.count() == count - 2 && test4[50] == 52)) {
        return false;
    }

    return true;
}

bool testIntClear() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    test.clear();

    return test.count() == 0;
}

bool testIntContains() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    if (!test.contains(50)) {
        return false;
    }

    Integers test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test.contains(150)) {
        return false;
    }

    return true;
}

bool testIntIndexOf() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    ssize_t index = test.indexOf(50);
    if (index != 50) {
        return false;
    }

    Integers test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    ssize_t index2 = test2.indexOf(150);
    if (index2 != -1) {
        return false;
    }

    Integers test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    ssize_t index3 = test3.lastIndexOf(50);
    if (index3 != 50) {
        return false;
    }

    Integers test4;
    for (int i = 0; i < count; ++i) {
        test4.add(i);
    }
    ssize_t index4 = test4.lastIndexOf(150);
    if (index4 != -1) {
        return false;
    }

    return true;
}

bool testIntData() {
    return true;
//    Integers test;
//    static const int count = 100;
//    for (int i = 0; i < count; ++i) {
//        test.add(i);
//    }
//    int *data = test.data();
//
//    return data != nullptr;
}

bool testIntEvaluates() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Integers test2;
    test2.evaluates(test);
    if (test2 != test) {
        return false;
    }

    Integers test3 = test;
    if (test3 != test) {
        return false;
    }

    Integers test4;
    test4 = test;
    if (test4 != test) {
        return false;
    }

    return true;
}

bool testIntEquals() {
    Integers test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Integers test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test2 != test) {
        return false;
    }

    Integers test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    if (!test3.equals(test)) {
        return false;
    }

    return true;
}

bool testIntIterator() {
    Integers test;
    test.add(1);
    test.add(2);
    test.add(3);

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

#ifndef __EMSCRIPTEN__

// Do not support on web, so the mutex does not worked.
void lockIntAction(void *owner) {
    auto *test = static_cast<Integers *>(owner);
    Locker locker(test);
    Thread::msleep(500);
    test->set(1, 4);
}

bool testIntLock() {
    Integers test;
    test.add(1);
    test.add(2);
    test.add(3);

    Thread thread("test lock thread", lockIntAction);
    thread.start(&test);
    if (test[1] != 2) {
        return false;
    }
    Thread::msleep(1000);
    if (test[1] == 2) {
        return false;
    }

    return true;
}

#endif  // __EMSCRIPTEN__

// complex value.
bool testConstructor() {
    {
        Values test(DefaultCapacity);
        if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
            return false;
        }

        Values test2(DefaultCapacity);
        test2.add(Value(1));
        test2.add(Value("abc"));
        Values test3(test2);
        if (test3 != test2) {
            return false;
        }

        Values test2_1(DefaultCapacity);
        test2_1.add(Value(1));
        test2_1.add(Value("abc"));
        Values test3_1(std::move(test2_1));
        if (!test2_1.isEmpty()) {
            return false;
        }
        if (!(test3_1.count() == 2 && test3_1[0] == 1 && test3_1[1] == "abc")) {
            return false;
        }

        static const int count = 5;
        Value array[count] = {
                Value(1),
                Value(2),
                Value("abc"),
                Value("bcd"),
                Value(128)
        };
        Values test4(array, count);
        if (test4.count() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            if (array[i] != test4[i]) {
                return false;
            }
        }
    }

    {
        static const int count = 5;
        Value array[count] = {
                Value(1),
                Value(2),
                Value("abc"),
                Value("bcd"),
                Value(128)
        };
        Values test{Value(1),
                    Value(2),
                    Value("abc"),
                    Value("bcd"),
                    Value(128)};
        if (test.count() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            if (array[i] != test[i]) {
                return false;
            }
        }
    }

    return true;
}

bool testCount() {
    Values test(DefaultCapacity);
    test.add(Value(1));
    test.add(Value("abc"));
    return test.count() == 2;
}

bool testSetCapacity() {
    Values test(DefaultCapacity);
    // set capacity if test is empty.
    test.setCapacity(50);
    if (!(test.capacity() == 50 && test.count() == 0)) {
        return false;
    }

    // add some data in test.
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Value(i));
    }
    if (!valuesEquals(test, test2)) {
        return false;
    }

    // set capacity if test is not empty and the capacity is less than count.
    test.setCapacity(25);
    if (!valuesEquals(test, test2)) {
        return false;
    }

    // set capacity if test is not empty and the capacity is greater than count.
    test.setCapacity(DefaultCapacity);
    if (!valuesEquals(test, test2) && test.capacity() == DefaultCapacity) {
        return false;
    }
    test.setCapacity(255);
    if (!valuesEquals(test, test2) && test.capacity() == 255) {
        return false;
    }

    return true;
}

bool testAt() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    const Value &v = test[99];
    if (v.iValue() != 99) {
        return false;
    }
    const Value &v2 = test[200];
    if (v2 != nullptr) {
        return false;
    }
    const Value &v3 = test.at(99);
    if (v3.iValue() != 99) {
        return false;
    }
    const Value &v4 = test.at(200);
    if (v4 != nullptr) {
        return false;
    }
    return true;
}

bool testAdd() {
    Values test(DefaultCapacity);
    test.add(Value(1));
    test.add(Value("abc"));
    if (!valueEquals(test[0], 1) && !valueEquals(test[1], "abc")) {
        return false;
    }
    int count = DefaultCapacity + 32;
    for (int i = 2; i < count; ++i) {
        test.add(Value(i));
    }
    if (!valueEquals(test[2], 2) && !valueEquals(test[count - 1], count - 1)) {
        return false;
    }
    return true;
}

bool testAddRange() {
    {
        Values test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(Value(i));
        }
        Values test2;
        if (!test2.addRange(test)) {
            return false;
        }
        if (!valuesEquals(test, test2)) {
            return false;
        }

        Values test3;
        if (!test3.addRange(test, 50, test.count() - 50)) {
            return false;
        }
        if (!(valueEquals(test3[0], 50) && valueEquals(test3[49], 99))) {
            return false;
        }

        Values test4;
        static const int count2 = 5;
        Value array[count] = {
                Value(1),
                Value(2),
                Value("abc"),
                Value("bcd"),
                Value(128)
        };
        if (!test4.addRange(array, count2)) {
            return false;
        }
        if (test4.count() != count2) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (!valueEquals(array[i], test4[i])) {
                return false;
            }
        }
    }

    {
        Values test;
        test.addRange({Value("1"), Value("2"), Value("3"), Value("4")});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], Int32(i + 1).toString())) {
                return false;
            }
        }
    }

    return true;
}

bool testInsertRange() {
    {
        Values test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(Value(i));
        }
        Values test2;
        test2.addRange(test);
        Values test3;
        for (int i = 0; i < 10; ++i) {
            test3.add(Value(0));
        }
        if (!test2.insertRange(10, test3)) {
            return false;
        }
        if (!(valueEquals(test2[10], 0) && valueEquals(test2[19], 0) && test2.count() == test.count() + 10)) {
            return false;
        }

        test2.clear();
        test2.addRange(test);
        if (!test2.insertRange(10, test3, 0, 5)) {
            return false;
        }
        if (!(valueEquals(test2[10], 0) && valueEquals(test2[14], 0) && !valueEquals(test2[15], 0) &&
              test2.count() == test.count() + 5)) {
            return false;
        }

        Values test4;
        test4.addRange(test);
        static const int count2 = 5;
        Value array[count] = {
                Value(1),
                Value(2),
                Value("abc"),
                Value("bcd"),
                Value(128)
        };
        if (!test4.insertRange(29, array, count2)) {
            return false;
        }
        if (test4.count() != test.count() + count2) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (!valueEquals(test4[i + 29], array[i])) {
                return false;
            }
        }
    }

    {
        Values test;
        test.insertRange(0, {Value(1), Value(2), Value(3), Value(4)});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], i + 1)) {
                return false;
            }
        }
    }

    return true;
}

bool testInsert() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    Values test2;
    test2.addRange(test);
    if (!test2.insert(6, Value(111))) {
        return false;
    }
    if (!(valueEquals(test2[6], 111) && test2.count() == test.count() + 1)) {
        return false;
    }

    // out of range.
    Values test3;
    test3.addRange(test);
    if (test3.insert(200, Value(111))) {
        return false;
    }
    return true;
}

bool testSetRange() {
    {
        Values test;
        static const int count = 100;
        for (int i = 0; i < count; ++i) {
            test.add(Value(i));
        }
        Values test2;
        test2.addRange(test);
        Values test3;
        for (int i = 0; i < 10; ++i) {
            test3.add(Value(0));
        }
        if (!test2.setRange(10, test3)) {
            return false;
        }
        if (!(valueEquals(test2[10], 0) && valueEquals(test2[19], 0) && test2.count() == test.count())) {
            return false;
        }

        test2.clear();
        test2.addRange(test);
        if (!test2.setRange(10, test3, 0, 5)) {
            return false;
        }
        if (!(valueEquals(test2[10], 0) && valueEquals(test2[14], 0) && !valueEquals(test2[15], 0) &&
              test2.count() == test.count())) {
            return false;
        }

        Values test4;
        test4.addRange(test);
        static const int count2 = 5;
        Value array[count] = {
                Value(1),
                Value(2),
                Value("abc"),
                Value("bcd"),
                Value(128)
        };
        if (!test4.setRange(29, array, count2)) {
            return false;
        }
        if (test4.count() != test.count()) {
            return false;
        }
        for (int i = 0; i < count2; ++i) {
            if (!valueEquals(test4[i + 29], array[i])) {
                return false;
            }
        }
    }

    {
        Values test(Value(0), 4);
        test.setRange(0, {Value(1), Value(2), Value(3), Value(4)});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (!valueEquals(test[i], i + 1)) {
                return false;
            }
        }
    }

    return true;
}

bool testQuickSort() {
    SortedValues test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }

    // sort desc.
    test.sort(false);
//    printValues(test);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], i)) {
            return false;
        }
    }

    // sort asc.
    test.sort(true);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[i], i)) {
            return false;
        }
    }

    // sort desc by comparison.
    test.sort(comparison, false);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], i)) {
            return false;
        }
    }

    // sort asc by comparison.
    test.sort(comparison, true);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[i], i)) {
            return false;
        }
    }

    // sort desc by comparer.
    test.sort(AscComparer<Value>(), false);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], i)) {
            return false;
        }
    }

    return true;
}

bool testRemove() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    auto v50 = test[50];
    if (!test.remove(v50)) {
        return false;
    }
    if (!(test.count() == count - 1 && valueEquals(test[50], 51))) {
        return false;
    }

    // out of range.
    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Value(i + 1));
    }
    auto count1 = test[count + 1];
    if (test2.remove(count1)) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(Value(i));
    }
    if (!test3.removeAt(50)) {
        return false;
    }
    if (!(test3.count() == count - 1 && valueEquals(test3[50], 51))) {
        return false;
    }

    Values test4;
    for (int i = 0; i < count; ++i) {
        test4.add(Value(i));
    }

    if (!test4.removeRange(50, 2)) {
        return false;
    }
    if (!(test4.count() == count - 2 && valueEquals(test4[50], 52))) {
        return false;
    }

    return true;
}

bool testSet() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    Values test2;
    test2.addRange(test);
    if (!test2.set(6, Value(111))) {
        return false;
    }
    if (!(valueEquals(test2[6], 111) && test2.count() == test.count())) {
        return false;
    }

    // out of range.
    Values test4;
    test4.addRange(test);
    if (test4.set(200, Value(111))) {
        return false;
    }

    // out of range if it can be inserted empty.
    Values test5;
    test5.addRange(test);
    if (test5.set(200, Value(111))) {
        return false;
    }

    return true;
}

bool testClear() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    test.clear();

    return test.count() == 0;
}

bool testContains() {
    Values test;
    static const int count = 100;
    for (int i = 1; i < count; ++i) {
        test.add(Value(i));
    }
    auto v50 = test[50];
    if (!test.contains(v50)) {
        return false;
    }

    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Value(i));
    }
    auto v150 = test[150];
    if (test.contains(v150)) {
        return false;
    }

    return true;
}

bool testIndexOf() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Value(i));
    }
    auto v50 = test[50];
    ssize_t index = test.indexOf(v50);
    if (index != 50) {
        return false;
    }

    Values test2;
    for (int i = 1; i < count; ++i) {
        test2.add(Value(i));
    }
    auto v150 = test2[150];
    ssize_t index2 = test2.indexOf(v150);
    if (index2 != -1) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(Value(i));
    }
    auto v50_2 = test3[50];
    ssize_t index3 = test3.indexOf(v50_2);
    if (index3 != 50) {
        return false;
    }

    Values test4;
    for (int i = 1; i < count; ++i) {
        test4.add(Value(i));
    }
    auto v150_2 = test4[150];
    ssize_t index4 = test4.indexOf(v150_2);
    if (index4 != -1) {
        return false;
    }

    Values test5;
    for (int i = 1; i < count; ++i) {
        test5.add(Value(i));
    }
    auto v150_3 = test5[150];
    ssize_t index5 = test5.lastIndexOf(v150_3);
    if (index5 != -1) {
        return false;
    }

    return true;
}

bool testData() {
    return true;
//    Values test;
//    static const int count = 100;
//    for (int i = 0; i < count; ++i) {
//        test.add(Value(i));
//    }
//    auto data = test.data();
//
//    return data != nullptr;
}

bool testIterator() {
    Values test;
    test.add(Value(1));
    test.add(Value(2));
    test.add(Value(3));

    int index = 1;
    for (const auto& it: test) {
        if (!valueEquals(it, index)) {
            return false;
        }
        index++;
    }

    index = 1;
    for (auto &it: test) {
        if (!valueEquals(it, index)) {
            return false;
        }
        index++;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (!valueEquals(*it, index)) {
            return false;
        }
        index--;
    }

    index = 3;
    for (auto it = test.rbegin(); it != test.rend(); ++it) {
        if (!valueEquals(*it, index)) {
            return false;
        }
        index--;
    }

    return true;
}

#ifndef __EMSCRIPTEN__

// Do not support on web, so the mutex does not worked.
void lockAction(void *owner) {
    auto *test = static_cast<Values *>(owner);
    Locker locker(test);
    Thread::msleep(500);
    test->set(1, Value(4));
}

bool testLock() {
    Values test;
    test.add(Value(1));
    test.add(Value(2));
    test.add(Value(3));

    Thread thread("test lock thread", lockAction);
    thread.start(&test);
    if (!valueEquals(test[1], 2)) {
        return false;
    }
    Thread::msleep(1000);
    if (valueEquals(test[1], 2)) {
        return false;
    }

    return true;
}

#endif  // __EMSCRIPTEN__

int main() {
    if (!testIntConstructor()) {
        return 1;
    }

    if (!testIntCount()) {
        return 2;
    }
    if (!testIntSetCapacity()) {
        return 3;
    }
    if (!testIntAt()) {
        return 4;
    }
    if (!testIntAdd()) {
        return 5;
    }
    if (!testIntAddRange()) {
        return 6;
    }
    if (!testIntInsertRange()) {
        return 7;
    }
    if (!testIntInsert()) {
        return 8;
    }
    if (!testIntSetRange()) {
        return 9;
    }
    if (!testIntSet()) {
        return 10;
    }
    if (!testIntQuickSort()) {
        return 11;
    }
    if (!testIntRemove()) {
        return 12;
    }
    if (!testIntClear()) {
        return 13;
    }
    if (!testIntContains()) {
        return 14;
    }
    if (!testIntIndexOf()) {
        return 15;
    }
    if (!testIntData()) {
        return 16;
    }
    if (!testIntEvaluates()) {
        return 17;
    }
    if (!testIntEquals()) {
        return 18;
    }
    if (!testIntIterator()) {
        return 19;
    }
#ifndef __EMSCRIPTEN__
    if (!testIntLock()) {
        return 20;
    }
#endif // __EMSCRIPTEN__

    if (!testConstructor()) {
        return 30;
    }
    if (!testCount()) {
        return 31;
    }
    if (!testSetCapacity()) {
        return 32;
    }
    if (!testAt()) {
        return 33;
    }
    if (!testAdd()) {
        return 34;
    }
    if (!testAddRange()) {
        return 35;
    }
    if (!testInsertRange()) {
        return 36;
    }
    if (!testInsert()) {
        return 37;
    }
    if (!testSetRange()) {
        return 38;
    }
    if (!testSet()) {
        return 39;
    }
    if (!testQuickSort()) {
        return 40;
    }
    if (!testRemove()) {
        return 41;
    }
    if (!testClear()) {
        return 42;
    }
    if (!testContains()) {
        return 43;
    }
    if (!testIndexOf()) {
        return 44;
    }
    if (!testData()) {
        return 45;
    }
    if (!testIterator()) {
        return 46;
    }
#ifndef __EMSCRIPTEN__
    if (!testLock()) {
        return 47;
    }
#endif // __EMSCRIPTEN__

    return 0;
}