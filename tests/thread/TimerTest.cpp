//
//  TimerTest.cpp
//  common
//
//  Created by baowei on 2023/3/1.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "data/ValueType.h"
#include "thread/Timer.h"

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

bool testConstructor() {
    {
        static int a = 0;
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, 100);
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
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, 200, 100);
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
        Timer test("t1", timerProc, &a, 100);
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
        Timer test("t1", timerProc, &a, 200, 100);
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
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, TimeSpan::fromMilliseconds(100));
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
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100));
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
        Timer test("t1", timerProc, &a, TimeSpan::fromMilliseconds(100));
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
        Timer test("t1", timerProc, &a, TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100));
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
        Timer test("t1", ObjectTimerCallback<Foo>(&foo, &Foo::bar), 100);
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
        Timer test("t1", ObjectTimerCallback<Foo>(&foo, &Foo::bar), 200, 100);
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
        Timer test("t1", ObjectTimerCallback<Foo>(&foo, &Foo::bar), TimeSpan::fromMilliseconds(100));
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
        Timer test("t1", ObjectTimerCallback<Foo>(&foo, &Foo::bar),
                TimeSpan::fromMilliseconds(200), TimeSpan::fromMilliseconds(100));
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
        auto timerProc = [](void *state) {
            auto timer = (Timer*) state;
            timer->stop();
            a++;
        };
        Timer test("t1", timerProc, 100);
        if (test.name() != "t1") {
            return false;
        }
        Thread::msleep(250);
        if (a != 1) {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        static int a = 0;
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, 1000);
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
        auto timerProc = [](void *state) {
            Thread::msleep(100);
        };
        Timer test("t1", timerProc, 100);
        if (!test.running()) {
            return false;
        }
    }

    return true;
}

bool testChange() {
    {
        static int a = 0;
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, 100);
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
        auto timerProc = [](void *state) {
            a++;
        };
        Timer test("t1", timerProc, 100);
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