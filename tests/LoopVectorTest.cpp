//
//  LoopVector.h
//  common
//
//  Created by baowei on 2022/11/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "data/LoopVector.h"
#include "thread/Thread.h"

using namespace Data;

#define DefaultSize 125

typedef LoopVector<int> Integers;

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

typedef LoopVector<Value> Values;

bool testIntConstructor() {
    {
        Integers test(0);
        if(test.size() != 1024) {
            return false;
        }
    }

    return true;
}

bool testIntEnqueue() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        if(test[0] != 1) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.count() != DefaultSize) {
            return false;
        }
        if(test.front() != 2) {
            return false;
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testIntDequeue() {
    {
        Integers test(DefaultSize);
        if(test.dequeue()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(!test.dequeue()) {
            return false;
        }
        if(test.front() != 2) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        for (int i = 0; i < count; ++i) {
            if (!test.dequeue()) {
                return false;
            }
        }
        if(test.count() != 0) {
            return false;
        }
    }

    return true;
}

bool testIntAt() {
    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        for (int i = 0; i < count; ++i) {
            int value = (i < count - 1) ? i + 2 : 0;
            if(test[i] != value) {
                return false;
            }
        }
    }

    return true;
}

bool testIntFront() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(test.front() != 1) {
            return false;
        }
    }
    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.front() != 2) {
            return false;
        }
    }

    return true;
}

bool testIntBack() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(test.back() != 2) {
            return false;
        }
    }
    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testIntIsEmpty() {
    {
        Integers test(DefaultSize);
        if(!test.isEmpty()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(test.isEmpty()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        test.clear();
        if(!test.isEmpty()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testIntIsFull() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(test.isFull()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize - 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.isFull()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(!test.isFull()) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(!test.isFull()) {
            return false;
        }
    }

    return true;
}

bool testIntCount() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        if(test.count() != 2) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize - 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.count() != count) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.count() != DefaultSize) {
            return false;
        }
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        if(test.count() != DefaultSize) {
            return false;
        }
    }

    return true;
}

bool testIntCopyTo() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        int* values = new int[test.count()];
        test.copyTo(values);
        if(!(values[0] == 1 && values[1] == 2)) {
            delete[] values;
            return false;
        }
        delete[] values;
    }

    {
        Integers test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(i + 1);
        }
        int* values = new int[test.count()];
        test.copyTo(values);
        if(!(values[0] == 2 && values[test.count() - 1] == count)) {
            delete[] values;
            return false;
        }
        delete[] values;
    }

    return true;
}

bool testIntClear() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        test.clear();
        if(test.count() != 0) {
            return false;
        }
    }

    return true;
}

bool testIntSize() {
    {
        Integers test;
        if(test.size() != 1024) {
            return false;
        }
    }
    {
        Integers test(DefaultSize);
        if(test.size() != DefaultSize) {
            return false;
        }
    }
    {
        Integers test(0);
        if(test.size() != 1024) {
            return false;
        }
    }
    {
        Integers test(20 * 1024 * 1204);
        if(test.size() != 1024) {
            return false;
        }
    }

    return true;
}

bool testIntReserve() {
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        size_t count = test.count();
        test.reserve(DefaultSize + 1);
        if(test.size() != DefaultSize + 1) {
            return false;
        }
        if(!(test.count() == count && test[0] == 1 && test[1] == 2)) {
            return false;
        }
    }
    {
        Integers test(DefaultSize);
        test.enqueue(1);
        test.enqueue(2);
        test.reserve(DefaultSize - 1);
        if(test.size() == DefaultSize - 1) {
            return false;
        }
    }

    return true;
}

#ifndef __EMSCRIPTEN__

// Do not support on web, so the mutex does not worked.
void lockIntAction(void *owner) {
    auto *test = static_cast<Integers *>(owner);
    Locker locker(test);
    Thread::msleep(500);
    test->clear();
}

bool testIntLock() {
    Integers test;
    test.enqueue(1);
    test.enqueue(2);
    test.enqueue(3);

    Thread thread("test lock thread");
    thread.start(lockIntAction, &test);
    if (test[1] != 2) {
        return false;
    }
    Thread::msleep(1000);
    if (test.count() != 0) {
        return false;
    }

    return true;
}

#endif  // __EMSCRIPTEN__

bool testValueConstructor() {
    {
        Values test(0);
        if(test.size() != 1024) {
            return false;
        }
    }

    return true;
}

bool testValueEnqueue() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        if(test[0] != 1) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.count() != DefaultSize) {
            return false;
        }
        if(test.front() != 2) {
            return false;
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testValueDequeue() {
    {
        Values test(DefaultSize);
        if(test.dequeue()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(!test.dequeue()) {
            return false;
        }
        if(test.front() != 2) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        for (int i = 0; i < count; ++i) {
            if (!test.dequeue()) {
                return false;
            }
        }
        if(test.count() != 0) {
            return false;
        }
    }

    return true;
}

bool testValueAt() {
    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        for (int i = 0; i < count; ++i) {
            int value = (i < count - 1) ? i + 2 : 0;
            if(test[i] != value) {
                return false;
            }
        }
    }

    return true;
}

bool testValueFront() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(test.front() != 1) {
            return false;
        }
    }
    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.front() != 2) {
            return false;
        }
    }

    return true;
}

bool testValueBack() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(test.back() != 2) {
            return false;
        }
    }
    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testValueIsEmpty() {
    {
        Values test(DefaultSize);
        if(!test.isEmpty()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(test.isEmpty()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        test.clear();
        if(!test.isEmpty()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.back() != count) {
            return false;
        }
    }

    return true;
}

bool testValueIsFull() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(test.isFull()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize - 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.isFull()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(!test.isFull()) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(!test.isFull()) {
            return false;
        }
    }

    return true;
}

bool testValueCount() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        if(test.count() != 2) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize - 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.count() != count) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.count() != DefaultSize) {
            return false;
        }
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        if(test.count() != DefaultSize) {
            return false;
        }
    }

    return true;
}

bool testValueCopyTo() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        Value* values = new Value[test.count()];
        test.copyTo(values);
        if(!(values[0] == 1 && values[1] == 2)) {
            delete[] values;
            return false;
        }
        delete[] values;
    }

    {
        Values test(DefaultSize);
        static int count = DefaultSize + 1;
        for (int i = 0; i < count; ++i) {
            test.enqueue(Value(i + 1));
        }
        Value* values = new Value[test.count()];
        test.copyTo(values);
        if(!(values[0] == 2 && values[test.count() - 1] == count)) {
            delete[] values;
            return false;
        }
        delete[] values;
    }

    return true;
}

bool testValueClear() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        test.clear();
        if(test.count() != 0) {
            return false;
        }
    }

    return true;
}

bool testValueSize() {
    {
        Values test;
        if(test.size() != 1024) {
            return false;
        }
    }
    {
        Values test(DefaultSize);
        if(test.size() != DefaultSize) {
            return false;
        }
    }
    {
        Values test(0);
        if(test.size() != 1024) {
            return false;
        }
    }
    {
        Values test(20 * 1024 * 1204);
        if(test.size() != 1024) {
            return false;
        }
    }

    return true;
}

bool testValueReserve() {
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        size_t count = test.count();
        test.reserve(DefaultSize + 1);
        if(test.size() != DefaultSize + 1) {
            return false;
        }
        if(!(test.count() == count && test[0] == 1 && test[1] == 2)) {
            return false;
        }
    }
    {
        Values test(DefaultSize);
        test.enqueue(Value(1));
        test.enqueue(Value(2));
        test.reserve(DefaultSize - 1);
        if(test.size() == DefaultSize - 1) {
            return false;
        }
    }

    return true;
}

#ifndef __EMSCRIPTEN__

// Do not support on web, so the mutex does not worked.
void lockValueAction(void *owner) {
    auto *test = static_cast<Values *>(owner);
    Locker locker(test);
    Thread::msleep(500);
    test->clear();
}

bool testValueLock() {
    Values test;
    test.enqueue(Value(1));
    test.enqueue(Value(2));
    test.enqueue(Value(3));

    Thread thread("test lock thread");
    thread.start(lockValueAction, &test);
    if (test[1] != 2) {
        return false;
    }
    Thread::msleep(1000);
    if (test.count() != 0) {
        return false;
    }

    return true;
}

#endif  // __EMSCRIPTEN__

int main() {
    if(!testIntConstructor()) {
        return 1;
    }
    if(!testIntEnqueue()) {
        return 2;
    }
    if(!testIntDequeue()) {
        return 3;
    }
    if(!testIntAt()) {
        return 4;
    }
    if(!testIntFront()) {
        return 5;
    }
    if(!testIntBack()) {
        return 6;
    }
    if(!testIntIsEmpty()) {
        return 7;
    }
    if(!testIntIsFull()) {
        return 8;
    }
    if(!testIntCount()) {
        return 9;
    }
    if(!testIntCopyTo()) {
        return 10;
    }
    if(!testIntClear()) {
        return 11;
    }
    if(!testIntSize()) {
        return 12;
    }
    if(!testIntReserve()) {
        return 13;
    }
#ifndef __EMSCRIPTEN__
    if(!testIntLock()) {
        return 14;
    }
#endif

    if(!testValueConstructor()) {
        return 21;
    }
    if(!testValueEnqueue()) {
        return 22;
    }
    if(!testValueDequeue()) {
        return 23;
    }
    if(!testValueAt()) {
        return 24;
    }
    if(!testValueFront()) {
        return 25;
    }
    if(!testValueBack()) {
        return 26;
    }
    if(!testValueIsEmpty()) {
        return 27;
    }
    if(!testValueIsFull()) {
        return 28;
    }
    if(!testValueCount()) {
        return 29;
    }
    if(!testValueCopyTo()) {
        return 30;
    }
    if(!testValueClear()) {
        return 31;
    }
    if(!testValueSize()) {
        return 32;
    }
    if(!testValueReserve()) {
        return 33;
    }
#ifndef __EMSCRIPTEN__
    if(!testValueLock()) {
        return 34;
    }
#endif

    return 0;
}
