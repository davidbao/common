//
//  StringArrayTest.cpp
//  common
//
//  Created by baowei on 2022/10/9.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/StringArray.h"
#include "thread/Thread.h"

using namespace Common;

#define DefaultCapacity 125

bool valueEquals(const String &x, const String &y) {
    return x == y;
}

bool valueEquals(const String &x, const int &y) {
    return x == Int32(y).toString();
}

int comparison(const String &x, const String &y) {
    return x.compareTo(y);
}

template<class T>
class AscComparer : public IComparer<T> {
public:
    AscComparer() {

    }

    int compare(const T &x, const T &y) const override {
        if (x > y)
            return 1;
        else if (x == y)
            return 0;
        else
            return -1;
    }
};

void printValues(const StringArray &v) {
    for (size_t i = 0; i < v.count(); ++i) {
        printf("%s; ", v[i].toString().c_str());
    }
    printf("\n");
}

bool testConstructor() {
    StringArray test(DefaultCapacity);
    if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
        return false;
    }

    StringArray test2(DefaultCapacity);
    test2.add(String("1"));
    test2.add(String("abc"));
    StringArray test3(test2);
    if (test3 != test2) {
        return false;
    }

    StringArray test2_1(DefaultCapacity);
    test2_1.add(String("1"));
    test2_1.add(String("abc"));
    StringArray test3_1(std::move(test2_1));
    if (!test2_1.isEmpty()) {
        return false;
    }
    if (!(test3_1.count() == 2 && test3_1[0] == "1" && test3_1[1] == "abc")) {
        return false;
    }

    static const int count = 5;
    String array[count] = {
            String("1"),
            String("2"),
            String("abc"),
            String("bcd"),
            String("128")
    };
    StringArray test4(array, count);
    if (test4.count() != count) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (array[i] != test4[i]) {
            return false;
        }
    }

    {
        static const int count = 5;
        StringArray test{String("1"),
                    String("2"),
                    String("abc"),
                    String("bcd"),
                    String("128")};
        if (test.count() != count) {
            return false;
        }
        for (int i = 0; i < count; ++i) {
            if (array[i] != test[i]) {
                return false;
            }
        }
    }

    {
        StringArray test("1", "2", "3", "4", nullptr);
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (test[i] != Int32(i + 1).toString()) {
                return false;
            }
        }
    }

    return true;
}

bool testCount() {
    StringArray test(DefaultCapacity);
    test.add(String("1"));
    test.add(String("abc"));
    return test.count() == 2;
}

bool testSetCapacity() {
    StringArray test(DefaultCapacity);
    // set capacity if test is empty.
    test.setCapacity(50);
    if (!(test.capacity() == 50 && test.count() == 0)) {
        return false;
    }

    // add some data in test.
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Int32(i).toString());
    }
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

bool testAt() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    const String &v = test[99];
    if (v != "99") {
        return false;
    }
    const String &v2 = test[200];
    if (v2 != nullptr) {
        return false;
    }
    const String &v3 = test.at(99);
    if (v3 != "99") {
        return false;
    }
    const String &v4 = test.at(200);
    if (v4 != nullptr) {
        return false;
    }
    return true;
}

bool testAdd() {
    StringArray test(DefaultCapacity);
    test.add(String("1"));
    test.add(String("abc"));
    if (!valueEquals(test[0], 1) && !valueEquals(test[1], "abc")) {
        return false;
    }
    int count = DefaultCapacity + 32;
    for (int i = 2; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    if (!valueEquals(test[2], 2) && !valueEquals(test[count - 1], count - 1)) {
        return false;
    }
    return true;
}

bool testAddRange() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    if (!test2.addRange(test)) {
        return false;
    }
    if (test != test2) {
        return false;
    }

    StringArray test3;
    if (!test3.addRange(test, 50, test.count() - 50)) {
        return false;
    }
    if (!(valueEquals(test3[0], 50) && valueEquals(test3[49], 99))) {
        return false;
    }

    StringArray test4;
    static const int count2 = 5;
    String array[count] = {
            String("1"),
            String("2"),
            String("abc"),
            String("bcd"),
            String("128")
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

    {
        StringArray test;
        test.addArray("1", "2", "3", "4", nullptr);
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (test[i] != Int32(i + 1).toString()) {
                return false;
            }
        }
    }

    {
        StringArray test;
        test.addRange({"1", "2", "3", "4"});
        if(test.count() != 4) {
            return false;
        }
        for (int i = 0; i < 4; ++i) {
            if (test[i] != Int32(i + 1).toString()) {
                return false;
            }
        }
    }

    return true;
}

bool testInsertRange() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    test2.addRange(test);
    StringArray test3;
    for (int i = 0; i < 10; ++i) {
        test3.add(String("0"));
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

    StringArray test4;
    test4.addRange(test);
    static const int count2 = 5;
    String array[count] = {
            String("1"),
            String("2"),
            String("abc"),
            String("bcd"),
            String("128")
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

    return true;
}

bool testInsert() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    test2.addRange(test);
    if (!test2.insert(6, String("111"))) {
        return false;
    }
    if (!(valueEquals(test2[6], 111) && test2.count() == test.count() + 1)) {
        return false;
    }

    // out of range.
    StringArray test3;
    test3.addRange(test);
    if (test3.insert(200, String("111"))) {
        return false;
    }
    return true;
}

bool testSetRange() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    test2.addRange(test);
    StringArray test3;
    for (int i = 0; i < 10; ++i) {
        test3.add(String("0"));
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

    StringArray test4;
    test4.addRange(test);
    static const int count2 = 5;
    String array[count] = {
            String("1"),
            String("2"),
            String("abc"),
            String("bcd"),
            String("128")
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

    return true;
}

bool testQuickSort() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString("%02d"));
    }

    // sort desc.
    test.sort(false);
//    printValues(test);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], Int32(i).toString("%02d"))) {
            return false;
        }
    }

    // sort asc.
    test.sort(true);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[i], Int32(i).toString("%02d"))) {
            return false;
        }
    }

    // sort desc by comparison.
    test.sort(comparison, false);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], Int32(i).toString("%02d"))) {
            return false;
        }
    }

    // sort asc by comparison.
    test.sort(comparison, true);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[i], Int32(i).toString("%02d"))) {
            return false;
        }
    }

    // sort desc by comparer.
    test.sort(AscComparer<String>(), false);
    for (int i = 0; i < count; ++i) {
        if (!valueEquals(test[count - 1 - i], Int32(i).toString("%02d"))) {
            return false;
        }
    }

    return true;
}

bool testRemove() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    auto v50 = test[50];
    if (!test.remove(v50)) {
        return false;
    }
    if (!(test.count() == count - 1 && valueEquals(test[50], 51))) {
        return false;
    }

    // out of range.
    StringArray test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Int32(i + 1).toString());
    }
    auto count1 = test[count + 1];
    if (test2.remove(count1)) {
        return false;
    }

    StringArray test3;
    for (int i = 0; i < count; ++i) {
        test3.add(Int32(i).toString());
    }
    if (!test3.removeAt(50)) {
        return false;
    }
    if (!(test3.count() == count - 1 && valueEquals(test3[50], 51))) {
        return false;
    }

    StringArray test4;
    for (int i = 0; i < count; ++i) {
        test4.add(Int32(i).toString());
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
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    StringArray test2;
    test2.addRange(test);
    if (!test2.set(6, String("111"))) {
        return false;
    }
    if (!(valueEquals(test2[6], 111) && test2.count() == test.count())) {
        return false;
    }

    // out of range.
    StringArray test4;
    test4.addRange(test);
    if (test4.set(200, String("111"))) {
        return false;
    }

    // out of range if it can be inserted empty.
    StringArray test5;
    test5.addRange(test);
    if (test5.set(200, String("111"))) {
        return false;
    }

    return true;
}

bool testClear() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    test.clear();

    return test.count() == 0;
}

bool testReserve() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }

    test.reserve(300);
    if (test.count() != count) {
        return false;
    }
    if (!(test[0] == "0" && test[count - 1] == "99")) {
        return false;
    }

    return true;
}

bool testContains() {
    StringArray test;
    static const int count = 100;
    for (int i = 1; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    auto v50 = test[50];
    if (!test.contains(v50)) {
        return false;
    }

    StringArray test2;
    for (int i = 0; i < count; ++i) {
        test2.add(Int32(i).toString());
    }
    auto v150 = test[150];
    if (test.contains(v150)) {
        return false;
    }

    return true;
}

bool testIndexOf() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    auto v50 = test[50];
    ssize_t index = test.indexOf(v50);
    if (index != 50) {
        return false;
    }

    StringArray test2;
    for (int i = 1; i < count; ++i) {
        test2.add(Int32(i).toString());
    }
    auto v150 = test2[150];
    ssize_t index2 = test2.indexOf(v150);
    if (index2 != -1) {
        return false;
    }

    StringArray test3;
    for (int i = 0; i < count; ++i) {
        test3.add(Int32(i).toString());
    }
    auto v50_2 = test3[50];
    ssize_t index3 = test3.indexOf(v50_2);
    if (index3 != 50) {
        return false;
    }

    StringArray test4;
    for (int i = 1; i < count; ++i) {
        test4.add(Int32(i).toString());
    }
    auto v150_2 = test4[150];
    ssize_t index4 = test4.indexOf(v150_2);
    if (index4 != -1) {
        return false;
    }

    StringArray test5;
    for (int i = 1; i < count; ++i) {
        test5.add(Int32(i).toString());
    }
    auto v150_3 = test5[150];
    ssize_t index5 = test5.lastIndexOf(v150_3);
    if (index5 != -1) {
        return false;
    }

    return true;
}

bool testData() {
    StringArray test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(Int32(i).toString());
    }
    auto data = test.data();

    return data != nullptr;
}

bool testIterator() {
    StringArray test;
    test.add(String("1"));
    test.add(String("2"));
    test.add(String("3"));

    int index = 1;
    for (auto it: test) {
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
    auto *test = static_cast<StringArray *>(owner);
    Locker locker(test);
    Thread::msleep(500);
    test->set(1, String("4"));
}

bool testLock() {
    StringArray test;
    test.add(String("1"));
    test.add(String("2"));
    test.add(String("3"));

    Thread thread("test lock thread");
    thread.start(lockAction, &test);
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
    if (!testReserve()) {
        return 14;
    }
    if (!testContains()) {
        return 15;
    }
    if (!testIndexOf()) {
        return 16;
    }
    if (!testData()) {
        return 17;
    }
    if (!testIterator()) {
        return 18;
    }
#ifndef __EMSCRIPTEN__
    if (!testLock()) {
        return 19;
    }
#endif // __EMSCRIPTEN__

    return 0;
}