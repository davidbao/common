//
//  ThreadTest.cpp
//  common
//
//  Created by baowei on 2023/2/23.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "data/ValueType.h"
#include "thread/Thread.h"
#ifdef __linux__
#include <unistd.h>
#endif

using namespace Data;
using namespace Threading;

class TestHolderValue : public ThreadHolder::Value {
public:
    String str;

    explicit TestHolderValue(const String &str) : str(str) {
    }
};

class TestCallback {
public:
    int value = 0;

    TestCallback() = default;

    void execute() {
        value = 2;
    }

    void execute(ThreadHolder *holder) {
        auto th = dynamic_cast<TestHolderValue *>(holder->value);
        Int32::parse(th->str, value);
        delete holder;
    }
};

class Foo {
public:
    explicit Foo(int value) : _value(value) {
    }

    void bar() {
        _value++;
    }

    int value() const {
        return _value;
    }

private:
    int _value;
};

bool testThreadHolder() {
    {
        String str = "abc123";
        auto value = new TestHolderValue(str);
        ThreadHolder holder(&str, value, true);
        if (!(holder.owner == &str && holder.value != nullptr && holder.autoDeleteValue)) {
            return false;
        }
        auto tv = dynamic_cast<const TestHolderValue *>(holder.value);
        if (tv->str != str) {
            return false;
        }
    }
    {
        String str = "abc123";
        TestHolderValue value(str);
        ThreadHolder holder(&str, &value, false);
        if (!(holder.owner == &str && holder.value == &value && !holder.autoDeleteValue)) {
            return false;
        }
    }

    return true;
}

bool testThreadCallback() {
    {
        ObjectThreadStart<TestCallback> callback;
        if (callback.instance() != nullptr) {
            return false;
        }
    }
    {
        TestCallback tc;
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute);
        ObjectThreadStart<TestCallback> callback2(callback);
        if (!callback.equals(callback2)) {
            return false;
        }
    }
    {
        TestCallback tc;
        TestHolderValue value("4");
        auto holder = new ThreadHolder(nullptr, &value, false);
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute, holder);
        ObjectThreadStart<TestCallback> callback2(callback);
        if (!callback.equals(callback2)) {
            delete holder;
            return false;
        }
        delete holder;
    }

    {
        TestCallback tc;
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute);
        ObjectThreadStart<TestCallback> callback2;
        callback2 = callback;
        if (!callback.equals(callback2)) {
            return false;
        }
    }
    {
        TestCallback tc;
        TestHolderValue value("4");
        auto holder = new ThreadHolder(nullptr, &value, false);
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute, holder);
        ObjectThreadStart<TestCallback> callback2;
        callback2 = callback;
        if (!callback.equals(callback2)) {
            delete holder;
            return false;
        }
        delete holder;
    }

    {
        TestCallback tc;
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute);
        if (callback.instance() == nullptr) {
            return false;
        }
        if (!callback.canExecution()) {
            return false;
        }
        if (tc.value != 0) {
            return false;
        }
        callback.execute();
        if (tc.value != 2) {
            return false;
        }
    }
    {
        TestCallback tc;
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute);
        if (callback.instance() == nullptr) {
            return false;
        }
        if (!callback.canExecution()) {
            return false;
        }
        if (tc.value != 0) {
            return false;
        }
        callback();
        if (tc.value != 2) {
            return false;
        }
    }

    {
        TestCallback tc;
        TestHolderValue value("4");
        auto holder = new ThreadHolder(nullptr, &value, false);
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute, holder);
        if (callback.instance() == nullptr) {
            return false;
        }
        if (!callback.canExecution()) {
            return false;
        }
        if (tc.value != 0) {
            return false;
        }
        callback();
        if (tc.value != 4) {
            return false;
        }
    }

    {
        TestCallback tc;
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute);
        auto cloned = callback.clone();
        if (!cloned->equals(callback)) {
            delete cloned;
            return false;
        }
        delete cloned;
    }
    {
        TestCallback tc;
        TestHolderValue value("4");
        auto holder = new ThreadHolder(nullptr, &value, false);
        ObjectThreadStart<TestCallback> callback(&tc, &TestCallback::execute, holder);
        auto cloned = callback.clone();
        if (!cloned->equals(callback)) {
            delete cloned;
            delete holder;
            return false;
        }
        delete cloned;
        delete holder;
    }

    return true;
}

bool testThreadId() {
    {
        ThreadId id = Thread::currentThreadId();
        ThreadId id2(id);
        if (id != id2) {
            return false;
        }
    }
    {
        ThreadId id = Thread::currentThreadId();
        ThreadId id2;
        id2 = id;
        if (id != id2) {
            return false;
        }
    }

    return true;
}

bool testConstructor() {
    {
        static int a = 0;
        auto runFunc = [] {
            a++;
        };
        Thread test("test_thread", runFunc);
        if (test.name() != "test_thread") {
            return false;
        }
    }
    {
        static int a = 0;
        auto runFunc = [](void *parameter) {
            a++;
        };
        Thread test("test_thread", runFunc);
        if (test.name() != "test_thread") {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        static int a = 0;
        auto runFunc = [] {
            a++;
        };
        Thread test("test_thread", runFunc);
        test.start();
        test.join();
        if (a != 1) {
            return false;
        }
    }
    {
        static int a = 0;
        auto runFunc = [](void *parameter) {
            a++;
            int *b = (int *) parameter;
            (*b)++;
        };
        int b = 0;
        Thread test("test_thread", runFunc);
        test.start(&b);
        test.join();
        if (!(a == 1 && b == 1)) {
            return false;
        }
    }

    {
        Foo foo(1);
        Thread test("test_thread", ObjectThreadStart<Foo>(&foo, &Foo::bar));
        test.start();
        test.join();
        if (foo.value() != 2) {
            return false;
        }
    }

    return true;
}

bool testRunning() {
    {
        auto runFunc = [] {
            while (true) {
                Thread::msleep(100);
            }
        };
        Thread test("test_thread", runFunc);
        test.start();
        for (int i = 0; i < 100; ++i) {
            if (!test.isAlive()) {
                return false;
            }
            Thread::msleep(10);
        }
        test.detach();
    }

    return true;
}

bool testId() {
    {
        static ThreadId tid;
        auto runFunc = [] {
            tid = Thread::currentThreadId();
            Thread::msleep(100);
        };
        Thread test("test_thread", runFunc);
        test.start();
        ThreadId id = test.id();
        test.join();
        if (tid != id) {
            return false;
        }
    }

    return true;
}

class PriorityTest {
public:
    bool loopSwitch;
    int64_t threadCount;

    PriorityTest() : loopSwitch(true), threadCount(0) {
    }

    void threadMethod() {
        while (loopSwitch) {
            threadCount++;
        }
    }
};

bool testPriority() {
    {
        static ThreadId tid;
        auto runFunc = [] {
            tid = Thread::currentThreadId();
            Thread::msleep(200);
        };
        Thread test("test_thread", runFunc);
        test.start();

        ThreadPriority priority, priority2;
        bool flag = true;
#ifdef __linux__
        flag = getuid() == 0;
#endif
        if (flag) {
            priority = ThreadPriority::Highest;
            test.setPriority(priority);
            priority2 = test.priority();
            if (priority2 != priority) {
                test.join();
                return false;
            }

            priority = ThreadPriority::AboveNormal;
            test.setPriority(priority);
            priority2 = test.priority();
            if (priority2 != priority) {
                test.join();
                return false;
            }
        }

        priority = ThreadPriority::Normal;
        test.setPriority(priority);
        priority2 = test.priority();
        if (priority2 != priority) {
            test.join();
            return false;
        }

        priority = ThreadPriority::BelowNormal;
        test.setPriority(priority);
        priority2 = test.priority();
        if (priority2 != priority) {
            test.join();
            return false;
        }

        priority = ThreadPriority::Lowest;
        test.setPriority(priority);
        priority2 = test.priority();
        if (priority2 != priority) {
            test.join();
            return false;
        }

        test.join();
    }

    {
        bool flag = true;
#ifdef __linux__
        flag = getuid() == 0;
#endif
        PriorityTest pt1;
        Thread test1("test_thread1", ObjectThreadStart<PriorityTest>(&pt1, &PriorityTest::threadMethod));
        test1.start();
        if (flag) {
            test1.setPriority(ThreadPriority::Highest);
        }

        PriorityTest pt2;
        Thread test2("test_thread2", ObjectThreadStart<PriorityTest>(&pt2, &PriorityTest::threadMethod));
        test2.start();
        if (flag) {
            test2.setPriority(ThreadPriority::AboveNormal);
        }

        PriorityTest pt3;
        Thread test3("test_thread3", ObjectThreadStart<PriorityTest>(&pt3, &PriorityTest::threadMethod));
        test3.start();
        test3.setPriority(ThreadPriority::Normal);

        PriorityTest pt4;
        Thread test4("test_thread4", ObjectThreadStart<PriorityTest>(&pt4, &PriorityTest::threadMethod));
        test4.start();
        test4.setPriority(ThreadPriority::BelowNormal);

        PriorityTest pt5;
        Thread test5("test_thread5", ObjectThreadStart<PriorityTest>(&pt5, &PriorityTest::threadMethod));
        test5.start();
        test5.setPriority(ThreadPriority::Lowest);

        Thread::msleep(3000);
        pt1.loopSwitch = false;
        pt2.loopSwitch = false;
        pt3.loopSwitch = false;
        pt4.loopSwitch = false;
        pt5.loopSwitch = false;
        if (flag) {
            if (pt1.threadCount <= pt3.threadCount) {
                return false;
            }
            if (pt2.threadCount <= pt3.threadCount) {
                return false;
            }
        }
        if (pt3.threadCount <= pt4.threadCount) {
            return false;
        }
        if (pt4.threadCount <= pt5.threadCount) {
            return false;
        }
    }

    return true;
}

bool testCurrentThread() {
    {
        static Thread *t = nullptr;
        auto runFunc = [] {
            t = Thread::currentThread();
            Thread::msleep(100);
        };
        Thread test("test_thread", runFunc);
        test.start();
        Thread::msleep(100);
        test.join();
        if (t != &test) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testThreadHolder()) {
        return 1;
    }
    if (!testThreadCallback()) {
        return 2;
    }
    if (!testThreadId()) {
        return 3;
    }

    if (!testConstructor()) {
        return 4;
    }
    if (!testStart()) {
        return 5;
    }
    if (!testRunning()) {
        return 6;
    }
    if (!testId()) {
        return 7;
    }
    if (!testPriority()) {
        return 8;
    }
    if (!testCurrentThread()) {
        return 9;
    }

    return 0;
}