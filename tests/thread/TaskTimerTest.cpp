//
//  TaskTimerTest.cpp
//  common
//
//  Created by baowei on 2023/3/11.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "thread/TaskTimer.h"

using namespace Threading;

static int func1Var = 0, func2Var = 0;

class Foo {
public:
    void func1() {
        _var++;
    }

    void func2(int var) {
        _var += var;
    }

    int func3(int x, int y) {
        return x + y;
    }

    int var() const {
        return _var;
    }

private:
    int _var = 0;
};

void func1() {
    func1Var++;
}

void func2(int var) {
    func2Var += var;
}

int func3(int x, int y) {
    return x + y;
}

bool testConstructor() {
    {
        TaskTimer timer;
        if (timer.running()) {
            return false;
        }
    }
    {
        TaskTimer timer("test");
        if (timer.running()) {
            return false;
        }
        if (timer.name() != "test") {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        TaskTimer timer("test");
        timer.add("test.1", TimeSpan::fromMilliseconds(100), func1);
        timer.add("test.2", TimeSpan::fromMilliseconds(100), func2, 2);
        timer.start();
        Thread::msleep(550);
        if (func1Var < 5) {
            return false;
        }
        if (func2Var < 10) {
            return false;
        }
    }

    return true;
}

bool testProperty() {
    {
        TaskTimer timer;
        if (timer.running()) {
            return false;
        }
    }
    {
        TaskTimer timer("test");
        if (timer.running()) {
            return false;
        }
        if (timer.name() != "test") {
            return false;
        }
    }
    {
        TaskTimer timer;
        if (timer.hasTimer()) {
            return false;
        }
    }
    {
        TaskTimer timer;
        timer.add("test.1", TimeSpan::fromMilliseconds(100), func1);
        if (!timer.hasTimer()) {
            return false;
        }
    }

    return true;
}

bool testRemove() {
    {
        TaskTimer timer("test");
        timer.add("test.1", TimeSpan::fromMilliseconds(100), func1);
        timer.add("test.2", TimeSpan::fromMilliseconds(100), func2, 2);
        timer.start();
        Thread::msleep(200);
        if (func1Var < 2) {
            return false;
        }
        if (func2Var < 4) {
            return false;
        }

        if (!timer.remove("test.2")) {
            return false;
        }
        int func2Var_old = func2Var;
        Thread::msleep(200);
        if (func1Var < 4) {
            return false;
        }
        if (func2Var_old != func2Var) {
            return false;
        }
    }
    {
        TaskTimer timer("test");
        timer.add("test.1", TimeSpan::fromMilliseconds(100), func1);
        timer.add("test.2", TimeSpan::fromMilliseconds(100), func2, 2);
        if (timer.remove("test.3")) {
            return false;
        }
        if (!timer.remove("test.2")) {
            return false;
        }
        if (timer.contains("test.2")) {
            return false;
        }
        if (!timer.contains("test.1")) {
            return false;
        }
        if (!timer.remove("test.1")) {
            return false;
        }
        if (timer.hasTimer()) {
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
    if (!testProperty()) {
        return 3;
    }
    if (!testRemove()) {
        return 4;
    }

    return 0;
}