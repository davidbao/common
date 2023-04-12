//
//  PListTest.cpp
//  common
//
//  Created by baowei on 2022/10/12.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/PList.h"
#include "data/String.h"
#include "thread/Thread.h"

using namespace Data;

#define DefaultCapacity 125

class Value : public IComparable<Value> {
public:
    explicit Value(int value = 0) : _iValue(value), _strValue(nullptr) {
    }

    explicit Value(const String &value) : _iValue(0), _strValue(value) {
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

private:
    int _iValue;
    String _strValue;
};

typedef PList<Value> Values;

bool valueEquals(const Value *x, const Value *y) {
    return x->iValue() == y->iValue() && x->strValue() == y->strValue();
}

bool valueEquals(const Value *x, int y) {
    return x->iValue() == y;
}

bool valueEquals(const Value *x, const String &y) {
    return x->strValue() == y;
}

bool valuesEquals(const Values &x, const Values &y) {
    if (x.count() != y.count()) {
        return false;
    }
    for (size_t i = 0; i < x.count(); ++i) {
        const Value *v1 = x[i];
        const Value *v2 = y[i];
        if (!valueEquals(v1, v2)) {
            return false;
        }
    }
    return true;
}

void printValues(const Values &v) {
    for (size_t i = 0; i < v.count(); ++i) {
        printf("%s; ", v[i]->toString().c_str());
    }
    printf("\n");
}

bool testConstructor() {
    Values test(true, DefaultCapacity);
    if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
        return false;
    }

    Values test2(true, DefaultCapacity);
    test2.add(new Value(1));
    test2.add(new Value("abc"));
    Values test3(test2);
    if (!valuesEquals(test3, test2)) {
        return false;
    }

    Values test2_1(true, DefaultCapacity);
    test2_1.add(new Value(1));
    test2_1.add(new Value("abc"));
    Values test3_1(std::move(test2_1));
    if (!test2_1.isEmpty()) {
        return false;
    }
    if (!(test3_1.count() == 2 && valueEquals(test3_1[0], 1) && valueEquals(test3_1[1], "abc"))) {
        return false;
    }

    static const int count = 5;
    Value *array[count] = {
            new Value(1),
            new Value(2),
            new Value("abc"),
            new Value("bcd"),
            new Value(128)
    };
    Values test4(array, count);
    if (test4.count() != count) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(array[i], test4[i])) {
            return false;
        }
    }

    return true;
}

bool testCount() {
    Values test(true, DefaultCapacity);
    test.add(new Value(1));
    test.add(new Value("abc"));
    return test.count() == 2;
}

bool testSetCapacity() {
    Values test(true, DefaultCapacity);
    // set capacity if test is empty.
    test.setCapacity(50);
    if (!(test.capacity() == 50 && test.count() == 0)) {
        return false;
    }

    // add some data in test.
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(new Value(i));
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
        test.add(new Value(i));
    }
    const Value *v = test[99];
    if (v->iValue() != 99) {
        return false;
    }
    const Value *v2 = test[200];
    if (v2 != nullptr) {
        return false;
    }
    const Value *v3 = test.at(99);
    if (v3->iValue() != 99) {
        return false;
    }
    const Value *v4 = test.at(200);
    if (v4 != nullptr) {
        return false;
    }
    return true;
}

bool testAdd() {
    Values test(true, DefaultCapacity);
    test.add(new Value(1));
    test.add(new Value("abc"));
    if (!valueEquals(test[0], 1) && !valueEquals(test[1], "abc")) {
        return false;
    }
    int count = DefaultCapacity + 32;
    for (int i = 2; i < count; ++i) {
        test.add(new Value(i));
    }
    if (!valueEquals(test[2], 2) && !valueEquals(test[count - 1], count - 1)) {
        return false;
    }
    return true;
}

bool testAddRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    Values test2(false);
    if (!test2.addRange(test)) {
        return false;
    }
    if (!valuesEquals(test, test2)) {
        return false;
    }

    Values test3(false);
    if (!test3.addRange(test, 50, test.count() - 50)) {
        return false;
    }
    if (!(valueEquals(test3[0], 50) && valueEquals(test3[49], 99))) {
        return false;
    }

    Values test4;
    static const int count2 = 5;
    Value *array[count] = {
            new Value(1),
            new Value(2),
            new Value("abc"),
            new Value("bcd"),
            new Value(128)
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
    return true;
}

bool testInsertRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    Values test2(false);
    test2.addRange(test);
    Values test3;
    for (int i = 0; i < 10; ++i) {
        test3.add(new Value(0));
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

    Values test4(false);
    test4.addRange(test);
    static const int count2 = 5;
    Value *array[count] = {
            new Value(1),
            new Value(2),
            new Value("abc"),
            new Value("bcd"),
            new Value(128)
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
    for (int i = 0; i < count2; ++i) {
        delete array[i];
    }
    return true;
}

bool testInsert() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }

    {
        Values test2(false);
        test2.addRange(test);
        std::unique_ptr<Value> v(new Value(111));
        if (!test2.insert(6, v.get())) {
            return false;
        }
        if (!(valueEquals(test2[6], 111) && test2.count() == test.count() + 1)) {
            return false;
        }
    }

    {
        // out of range.
        Values test3(false);
        test3.addRange(test);
        std::unique_ptr<Value> v(new Value(111));
        if (test3.insert(200, v.get())) {
            return false;
        }
    }

    return true;
}

bool testSetRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    Values test2(false);
    test2.addRange(test);
    Values test3;
    for (int i = 0; i < 10; ++i) {
        test3.add(new Value(0));
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

    Values test4(false);
    test4.addRange(test);
    static const int count2 = 5;
    Value *array[count] = {
            new Value(1),
            new Value(2),
            new Value("abc"),
            new Value("bcd"),
            new Value(128)
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
    for (int i = 0; i < count2; ++i) {
        delete array[i];
    }
    return true;
}

int comparison(const Value &x, const Value &y) {
    return x.compareTo(y);
}

template<class T>
class AscComparer : public IComparer<T> {
public:
    AscComparer() = default;

    int compare(const T &x, const T &y) const override {
        return x.compareTo(y);
    }
};

bool testQuickSort() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
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
        test.add(new Value(i));
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
        test2.add(new Value(i));
    }
    auto count1 = test[count + 1];
    if (test2.remove(count1)) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(new Value(i));
    }
    if (!test3.removeAt(50)) {
        return false;
    }
    if (!(test3.count() == count - 1 && valueEquals(test3[50], 51))) {
        return false;
    }

    Values test4;
    for (int i = 0; i < count; ++i) {
        test4.add(new Value(i));
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
        test.add(new Value(i));
    }

    {
        Values test2(false);
        test2.addRange(test);
        std::unique_ptr<Value> v(new Value(111));
        if (!test2.set(6, v.get())) {
            return false;
        }
        if (!(valueEquals(test2[6], 111) && test2.count() == test.count())) {
            return false;
        }
    }

    {
        // out of range.
        Values test4(false);
        test4.addRange(test);
        std::unique_ptr<Value> v(new Value(111));
        if (test4.set(200, v.get())) {
            return false;
        }
    }

    {
        // out of range if it can be inserted empty.
        Values test5(false);
        test5.addRange(test);
        std::unique_ptr<Value> v(new Value(111));
        if (!test5.set(200, v.get(), true)) {
            return false;
        }
        if (!(valueEquals(test5[200], 111) && test5.count() == 200 + 1)) {
            return false;
        }
    }

    return true;
}

bool testClear() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    test.clear();

    return test.count() == 0;
}

bool testContains() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    auto v50 = test[50];
    if (!test.contains(v50)) {
        return false;
    }

    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(new Value(i));
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
        test.add(new Value(i));
    }
    auto v50 = test[50];
    ssize_t index = test.indexOf(v50);
    if (index != 50) {
        return false;
    }

    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(new Value(i));
    }
    auto v150 = test2[150];
    ssize_t index2 = test2.indexOf(v150);
    if (index2 != -1) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(new Value(i));
    }
    auto v50_2 = test3[50];
    ssize_t index3 = test3.indexOf(v50_2);
    if (index3 != 50) {
        return false;
    }

    Values test4;
    for (int i = 0; i < count; ++i) {
        test4.add(new Value(i));
    }
    auto v150_2 = test4[150];
    ssize_t index4 = test4.indexOf(v150_2);
    if (index4 != -1) {
        return false;
    }

    Values test5;
    for (int i = 0; i < count; ++i) {
        test5.add(new Value(i));
    }
    auto v150_3 = test5[150];
    ssize_t index5 = test5.lastIndexOf(v150_3);
    if (index5 != -1) {
        return false;
    }

    return true;
}

bool testData() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }
    auto data = test.data();

    return data != nullptr;
}

bool testIterator() {
    Values test;
    test.add(new Value(1));
    test.add(new Value(2));
    test.add(new Value(3));

    int index = 1;
    for (const auto& iter: test) {
        if (!valueEquals(&iter, index)) {
            return false;
        }
        index++;
    }

    index = 1;
    for (auto &iter: test) {
        if (!valueEquals(&iter, index)) {
            return false;
        }
        index++;
    }

    index = 3;
    for (Values::reverse_iterator iter = test.rbegin(); iter != test.rend(); ++iter) {
        if (!valueEquals(&*iter, index)) {
            return false;
        }
        index--;
    }

    index = 3;
    for (Values::const_reverse_iterator iter = test.rbegin(); iter != test.rend(); ++iter) {
        if (!valueEquals(&*iter, index)) {
            return false;
        }
        index--;
    }

    return true;
}

bool testAutoDelete() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(new Value(i));
    }

    if (!test.autoDelete()) {
        return false;
    }

    test.setAutoDelete(false);
    if (test.autoDelete()) {
        return false;
    }

    auto data = test.data();
    for (int i = 0; i < count; ++i) {
        delete data[i];
    }

    return true;
}

#ifndef __EMSCRIPTEN__

// Do not support on web, so the mutex does not worked.
void lockAction(Values *test) {
    Locker locker(test);
    Thread::msleep(500);
    test->set(1, new Value(4));
}

bool testLock() {
    Values test;
    test.add(new Value(1));
    test.add(new Value(2));
    test.add(new Value(3));

    Thread thread("test lock thread", lockAction, &test);
    thread.start();
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
    if (!testConstructor()) {
        return 1;
    }
    if (!testCount()) {
        return 2;
    }
    if (!testSetCapacity()) {
        return 3;
    }
    if (!testAt()) {
        return 4;
    }
    if (!testAdd()) {
        return 5;
    }
    if (!testAddRange()) {
        return 6;
    }
    if (!testInsertRange()) {
        return 7;
    }
    if (!testInsert()) {
        return 8;
    }
    if (!testSetRange()) {
        return 9;
    }
    if (!testSet()) {
        return 10;
    }
    if (!testQuickSort()) {
        return 11;
    }
    if (!testRemove()) {
        return 12;
    }
    if (!testClear()) {
        return 13;
    }
    if (!testContains()) {
        return 14;
    }
    if (!testIndexOf()) {
        return 15;
    }
    if (!testData()) {
        return 16;
    }
    if (!testIterator()) {
        return 17;
    }
    if (!testAutoDelete()) {
        return 18;
    }
#ifndef __EMSCRIPTEN__
    if (!testLock()) {
        return 19;
    }
#endif // __EMSCRIPTEN__

    return 0;
}