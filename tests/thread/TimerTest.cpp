//
//  TimerTest.cpp
//  common
//
//  Created by baowei on 2023/3/1.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "data/ValueType.h"
#include "thread/Timer.h"
#include "system/OsDefine.h"
#include "diag/Trace.h"

#ifdef WEB_OS
#include <emscripten.h>
#endif

using namespace Data;
using namespace Threading;

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

#ifdef WEB_OS
std::unique_ptr<Timer> timer;

bool testConstructor() {
//    {
//        static int a = 0;
//        auto timerProc = []() {
////            printf("timerProc\n");
//            a++;
//        };
//        timer = std::unique_ptr<Timer>(new Timer("t1", 100, timerProc));
//
//        auto mainProc = []() {
//            Diag::Debug::writeLine("testConstructor1");
//            Thread::msleep(200);
//            printf("testConstructor2\n");
//            Timer *test = timer.get();
//            if (test->name() != "t1") {
//                printf("testConstructor3\n");
//                exit(1);
//            }
//            printf("testConstructor4\n");
//            if (a < 2) {
//                printf("testConstructor5\n");
//                exit(1);
//            }
//            printf("testConstructor6\n");
//            exit(0);
//            printf("testConstructor7\n");
//        };
//        emscripten_set_main_loop(mainProc, 0, 0);
//    }

    return true;
}

bool testStart() {
    {
    }

    return true;
}

bool testRunning() {
    {
    }

    return true;
}

bool testChange() {
    {
    }

    return true;
}
#else
bool testConstructor() {
    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", 100, timerProc);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (a < 2) {
            return false;
        }
    }
    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", 200, 100, timerProc);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (a > 1) {
            return false;
        }
    }
    {
        int a = 0;
        auto timerProc = [](void *state) {
            int *v = (int *) state;
            (*v)++;
        };
        Timer test("t1", 100, timerProc, &a);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (a < 2) {
            return false;
        }
    }
    {
        int a = 0;
        auto timerProc = [](void *state) {
            int *v = (int *) state;
            (*v)++;
        };
        Timer test("t1", 200, 100, timerProc, &a);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (a > 1) {
            return false;
        }
    }

    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", TimeSpan::fromMilliseconds(100), timerProc);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (a < 2) {
            return false;
        }
    }
    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100), timerProc);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (a > 1) {
            return false;
        }
    }
    {
        int a = 0;
        auto timerProc = [](void *state) {
            int *v = (int *) state;
            (*v)++;
        };
        Timer test("t1", TimeSpan::fromMilliseconds(100), timerProc, &a);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (a < 2) {
            return false;
        }
    }
    {
        int a = 0;
        auto timerProc = [](void *state) {
            int *v = (int *) state;
            (*v)++;
        };
        Timer test("t1", TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100), timerProc, &a);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (a > 1) {
            return false;
        }
    }

    {
        Foo foo(0);
        Timer test("t1", 100, &Foo::bar, &foo);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (foo.value() < 2) {
            return false;
        }
    }
    {
        Foo foo(0);
        Timer test("t1", 200, 100, &Foo::bar, &foo);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (foo.value() > 1) {
            return false;
        }
    }
    {
        Foo foo(0);
        Timer test("t1", TimeSpan::fromMilliseconds(100), &Foo::bar, &foo);
        Thread::msleep(200);
        if (test.name() != "t1") {
            return false;
        }
        if (foo.value() < 2) {
            return false;
        }
    }
    {
        Foo foo(0);
        Timer test("t1", TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100), &Foo::bar, &foo);
        Thread::msleep(250);
        if (test.name() != "t1") {
            return false;
        }
        if (foo.value() > 1) {
            return false;
        }
    }

    {
        static int a = 0;
        struct Entry {
            Timer *timer;
        };
        Entry entry;
        auto timerProc = [](Entry *entry) {
            entry->timer->stop();
            a++;
        };
        entry.timer = new Timer("t1", 100, timerProc, &entry);
        Thread::msleep(250);
        if (a != 1) {
            delete entry.timer;
            return false;
        }
        delete entry.timer;
    }

    return true;
}

bool testStart() {
    {
        static int a = 0;
        auto timerProc = []() {
//            Diag::Debug::writeLine("testStart.timer.invoke!");
            a++;
        };
        Timer test("t1", 1000, timerProc);
        Thread::msleep(100);
        test.stop();
        test.start();
        Thread::msleep(100);
        test.stop();
        if (a != 2) {
            return false;
        }
    }

    return true;
}

bool testRunning() {
    {
        auto timerProc = []() {
            Thread::msleep(100);
        };
        Timer test("t1", 100, timerProc);
        if (!test.running()) {
            return false;
        }
    }

    return true;
}

bool testChange() {
    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", 100, timerProc);
        Thread::msleep(200);
        if (a < 2) {
            return false;
        }
        test.change(200);
        Thread::msleep(200);
        if (a < 3) {
            return false;
        }
    }
    {
        static int a = 0;
        auto timerProc = []() {
            a++;
        };
        Timer test("t1", 100, timerProc);
        Thread::msleep(200);
        if (a < 2) {
            return false;
        }
        test.change(1000, 200);
        Thread::msleep(200);
        if (a < 2) {
            return false;
        }
    }

    return true;
}
#endif // WEB_OS

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testStart()) {
        return 2;
    }
    if (!testRunning()) {
        return 3;
    }
    if (!testChange()) {
        return 4;
    }

    return 0;
}