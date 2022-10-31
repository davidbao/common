//
//  ArrayTest.cpp
//  common
//
//  Created by baowei on 2022/10/9.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/Array.h"
#include "thread/Thread.h"

using namespace Common;

#define DefaultCapacity 125

typedef Array<int> Values;

bool testConstructor() {
    Values test(DefaultCapacity);
    if (!(test.count() == 0 && test.capacity() == DefaultCapacity)) {
        return false;
    }

    Values test2(DefaultCapacity);
    test2.add(1);
    test2.add(2);
    Values test3(test2);
    if (test2 != test3) {
        return false;
    }

    Values test2_1(DefaultCapacity);
    test2_1.add(1);
    test2_1.add(2);
    Values test3_1(test2_1, 1, 1);
    if (!(test3_1.count() == 1 && test3_1[0] == 2)) {
        return false;
    }

    Values test2_2(DefaultCapacity);
    test2_2.add(1);
    test2_2.add(2);
    Values test3_2(std::move(test2_2));
    if (!test2_2.isEmpty()) {
        return false;
    }
    if (!(test3_2.count() == 2 && test3_2[0] == 1 && test3_2[1] == 2)) {
        return false;
    }

    static const int count = 5;
    int array[count] = {1, 2, 3, 4, 5};
    Values test4(array, count);
    if (test4.count() != count) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (array[i] != test4[i]) {
            return false;
        }
    }

    static const int value = 6;
    static const int count2 = 120;
    Values test5(value, count2);
    if (test5.count() != count2) {
        return false;
    }
    for (int i = 0; i < count2; ++i) {
        if (value != test5[i]) {
            return false;
        }
    }

    return true;
}

bool testCount() {
    Values test(DefaultCapacity);
    test.add(1);
    test.add(2);
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
        test.add(i);
    }
    Values test2(test);
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
    Values test;
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

bool testAdd() {
    Values test(DefaultCapacity);
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

bool testAddRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    if (!test2.addRange(test)) {
        return false;
    }
    if (test != test2) {
        return false;
    }

    Values test3;
    if (!test3.addRange(test, 50, test.count() - 50)) {
        return false;
    }
    if (!(test3[0] == 50 && test3[49] == 99)) {
        return false;
    }

    Values test4;
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
    return true;
}

bool testInsertRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    test2.addRange(test);
    Values test3;
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

    Values test4;
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
    return true;
}

bool testInsert() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    test2.addRange(test);
    if (!test2.insert(6, 111)) {
        return false;
    }
    if (!(test2[6] == 111 && test2.count() == test.count() + 1)) {
        return false;
    }

    // out of range.
    Values test3;
    test3.addRange(test);
    if (test3.insert(200, 111)) {
        return false;
    }
    return true;
}

bool testSetRange() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    test2.addRange(test);
    Values test3;
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

    Values test4;
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
    return true;
}

bool testSet() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    test2.addRange(test);
    if (!test2.set(6, 111)) {
        return false;
    }
    if (!(test2[6] == 111 && test2.count() == test.count())) {
        return false;
    }

    Values test3;
    test3.addRange(test);
    test3[6] = 111;
    if (!(test3[6] == 111 && test3.count() == test.count())) {
        return false;
    }

    // out of range.
    Values test4;
    test4.addRange(test);
    if (test4.set(200, 111)) {
        return false;
    }

    // out of range if it can be inserted empty.
    Values test5;
    test5.addRange(test);
    if (!test5.set(200, 111, true)) {
        return false;
    }
    if (!(test5[200] == 111 && test5.count() == 200 + 1)) {
        return false;
    }
    return true;
}

int comparison(const int &x, const int &y) {
    if (x > y)
        return 1;
    else if (x == y)
        return 0;
    else
        return -1;
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

bool testQuickSort() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }

    // sort desc.
    test.sort(false);
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

bool testRemove() {
    Values test;
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
    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test2.remove(count + 1)) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    if (!test3.removeAt(50)) {
        return false;
    }
    if (!(test3.count() == count - 1 && test3[50] == 51)) {
        return false;
    }

    Values test4;
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

bool testClear() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    test.clear();

    return test.count() == 0;
}

bool testContains() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    if (!test.contains(50)) {
        return false;
    }

    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test.contains(150)) {
        return false;
    }

    return true;
}

bool testIndexOf() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    ssize_t index = test.indexOf(50);
    if (index != 50) {
        return false;
    }

    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    ssize_t index2 = test2.indexOf(150);
    if (index2 != -1) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    ssize_t index3 = test3.lastIndexOf(50);
    if (index3 != 50) {
        return false;
    }

    Values test4;
    for (int i = 0; i < count; ++i) {
        test4.add(i);
    }
    ssize_t index4 = test4.lastIndexOf(150);
    if (index4 != -1) {
        return false;
    }

    return true;
}

bool testData() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    int *data = test.data();

    return data != nullptr;
}

bool testEvaluates() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    test2.evaluates(test);
    if (test2 != test) {
        return false;
    }

    Values test3 = test;
    if (test3 != test) {
        return false;
    }

    Values test4;
    test4 = test;
    if (test4 != test) {
        return false;
    }

    return true;
}

bool testEquals() {
    Values test;
    static const int count = 100;
    for (int i = 0; i < count; ++i) {
        test.add(i);
    }
    Values test2;
    for (int i = 0; i < count; ++i) {
        test2.add(i);
    }
    if (test2 != test) {
        return false;
    }

    Values test3;
    for (int i = 0; i < count; ++i) {
        test3.add(i);
    }
    if (!test3.equals(test)) {
        return false;
    }

    return true;
}

bool testIterator() {
    Values test;
    test.add(1);
    test.add(2);
    test.add(3);

    int index = 1;
    for (int iter: test) {
        if (iter != index) {
            return false;
        }
        index++;
    }

    index = 1;
    for (int &iter: test) {
        if (iter != index) {
            return false;
        }
        index++;
    }

    index = 3;
    for (Values::reverse_iterator iter = test.rbegin(); iter != test.rend(); ++iter) {
        if (*iter != index) {
            return false;
        }
        index--;
    }

    index = 3;
    for (Values::reverse_const_iterator iter = test.rbegin(); iter != test.rend(); ++iter) {
        if (*iter != index) {
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
    test->set(1, 4);
}

bool testLock() {
    Values test;
    test.add(1);
    test.add(2);
    test.add(3);

    Thread thread("test lock thread");
    thread.start(lockAction, &test);
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
    if (!testEvaluates()) {
        return 17;
    }
    if (!testEquals()) {
        return 18;
    }
    if (!testIterator()) {
        return 19;
    }
#ifndef __EMSCRIPTEN__
    if (!testLock()) {
        return 20;
    }
#endif // __EMSCRIPTEN__

    return 0;
}