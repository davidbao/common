//
//  ThreadTest.cpp
//  common
//
//  Created by baowei on 2023/2/23.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "data/ValueType.h"
#include "thread/Thread.h"
#include "system/Environment.h"

#ifdef __linux__

#include <unistd.h>

#elif WIN_OS

#include "diag/Trace.h"
using namespace Diag;

#endif

using namespace Data;
using namespace Threading;
using namespace System;

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
        Thread test;
    }
    {
        static int a = 0;
        auto runFunc = [] {
            a++;
        };
        Thread test = Thread("test_thread", runFunc);
        if (test.name() != "test_thread") {
            return false;
        }
    }
    {
        static int a = 0;
        auto runFunc = [] {
            a++;
        };
        Thread test;
        test = Thread("test_thread", runFunc);
        if (test.name() != "test_thread") {
            return false;
        }
    }

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
        Thread test("test_thread", runFunc, nullptr);
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
        Thread test("test_thread", runFunc, &b);
        test.start();
        test.join();
        if (!(a == 1 && b == 1)) {
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

//class PriorityTest {
//public:
//    std::atomic<bool> loopSwitch;
//    int64_t threadCount;
//
//    PriorityTest() : loopSwitch(true), threadCount(0) {
//    }
//
//    void threadMethod() {
//        while (loopSwitch) {
//            threadCount++;
//        }
//    }
//};

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

//    {
//        bool flag = true;
//#ifdef __linux__
//        flag = getuid() == 0;
//#endif
//        if (flag) {
//            {
//                PriorityTest pt1;
//                Thread test1("test_thread1", &PriorityTest::threadMethod, &pt1);
//                test1.start();
//                test1.setPriority(ThreadPriority::Highest);
//
//                PriorityTest pt3;
//                Thread test3("test_thread3", &PriorityTest::threadMethod, &pt3);
//                test3.start();
//                test3.setPriority(ThreadPriority::Normal);
//
//                Thread::msleep(500);
//                pt3.loopSwitch = false;
//                pt1.loopSwitch = false;
//
//                if (pt1.threadCount <= pt3.threadCount) {
//                    return false;
//                }
//            }
//            {
//                PriorityTest pt2;
//                Thread test2("test_thread2", &PriorityTest::threadMethod, &pt2);
//                test2.start();
//                test2.setPriority(ThreadPriority::AboveNormal);
//
//                PriorityTest pt3;
//                Thread test3("test_thread3", &PriorityTest::threadMethod, &pt3);
//                test3.start();
//                test3.setPriority(ThreadPriority::Normal);
//
//                Thread::msleep(500);
//                pt3.loopSwitch = false;
//                pt2.loopSwitch = false;
//
//                if (pt2.threadCount <= pt3.threadCount) {
//                    return false;
//                }
//            }
//        }
//    }
//
//    {
//        PriorityTest pt3;
//        Thread test3("test_thread3", &PriorityTest::threadMethod, &pt3);
//        test3.start();
//        test3.setPriority(ThreadPriority::Normal);
//
//        PriorityTest pt4;
//        Thread test4("test_thread4", &PriorityTest::threadMethod, &pt4);
//        test4.start();
//        test4.setPriority(ThreadPriority::BelowNormal);
//
//        PriorityTest pt5;
//        Thread test5("test_thread5", &PriorityTest::threadMethod, &pt5);
//        test5.start();
//        test5.setPriority(ThreadPriority::Lowest);
//
//        Thread::msleep(1000);
//        pt3.loopSwitch = false;
//        pt4.loopSwitch = false;
//        pt5.loopSwitch = false;
//        if (pt3.threadCount <= pt5.threadCount) {
//            return false;
//        }
//        if (pt4.threadCount <= pt5.threadCount) {
//            return false;
//        }
//    }

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

bool testDelay() {
    {
        auto func = [] {
            Thread::sleep(100);
            return false;
        };
        uint64_t start = Environment::getTickCount();
        Thread::delay(200, Func<bool>(func));
        uint64_t end = Environment::getTickCount();
        int64_t elapsed = (int64_t) (end - start);
        if (elapsed < 200) {
            return false;
        }
    }
    {
        auto func = [] {
            Thread::sleep(100);
            return true;
        };
        uint64_t start = Environment::getTickCount();
        Thread::delay(200, Func<bool>(func));
        uint64_t end = Environment::getTickCount();
        int64_t elapsed = (int64_t) (end - start);
        if (!(elapsed >= 100 && elapsed <= 200)) {
            return false;
        }
    }

    return true;
}

int main() {
#ifdef WIN32
    Trace::enableFlushConsoleOutput();
#endif
    if (!testThreadId()) {
        return 1;
    }
    if (!testConstructor()) {
        return 2;
    }
    if (!testStart()) {
        return 3;
    }
    if (!testRunning()) {
        return 4;
    }
    if (!testId()) {
        return 5;
    }
    if (!testPriority()) {
        return 6;
    }
    if (!testCurrentThread()) {
        return 7;
    }
    if (!testDelay()) {
        return 8;
    }

    return 0;
}