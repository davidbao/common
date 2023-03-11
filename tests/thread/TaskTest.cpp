//
//  TaskTest.cpp
//  common
//
//  Created by baowei on 2023/3/8.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "thread/Task.h"
#include "system/Environment.h"

using namespace Threading;
using namespace System;

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
        func1Var = 0;
        Task task(func1);
        task.wait(TimeSpan::fromMilliseconds(500));
        if (func1Var != 1) {
            return false;
        }
    }

    {
        Task task(func3, 1, 2);
        task.wait();
        if (task.result<int>() != 3) {
            return false;
        }
    }

    {
        func1Var = 0;
        Task task = Task(func1);
        task.wait(TimeSpan::fromMilliseconds(500));
        if (func1Var != 1) {
            return false;
        }
    }

    {
        Task task = Task(func3, 1, 2);
        task.wait();
        if (task.result<int>() != 3) {
            return false;
        }
    }

    {
        func1Var = 0;
        Task task = Task::run(func1);
        task.wait(TimeSpan::fromMilliseconds(500));
        if (func1Var != 1) {
            return false;
        }
    }

    {
        Task task = Task::run(func3, 1, 2);
        task.wait();
        if (task.result<int>() != 3) {
            return false;
        }
    }

    return true;
}

bool testResult() {
    {
        Task task(func3, 1, 2);
        task.wait();
        if (task.result<int>() != 3) {
            return false;
        }
    }
    {
        Task task = Task(func3, 1, 2);
        if (task.result<int>() != 3) {
            return false;
        }
    }
    {
        Task task = Task::run(func3, 1, 2);
        if (task.result<int>() != 3) {
            return false;
        }
    }
    {
        Task task(func3, 1, 2);
        if (task.result<int>() != 3) {
            return false;
        }
    }

    {
        auto func = [](const String &x, const String &y) {
            return x + y;
        };
        Task task(func, "1", "2");
        task.wait();
        if (task.result<String>() != "12") {
            return false;
        }
    }
    {
        auto func = [](const String &x, const String &y) {
            return x + y;
        };
        Task task = Task(func, "1", "2");
        task.wait();
        if (task.result<String>() != "12") {
            return false;
        }
    }
    {
        auto func = [](const String &x, const String &y) {
            return x + y;
        };
        Task task = Task::run(func, "1", "2");
        task.wait();
        if (task.result<String>() != "12") {
            return false;
        }
    }
    {
        auto func = [](const String &x, const String &y) {
            return x + y;
        };
        Task task(func, "1", "2");
        if (task.result<String>() != "12") {
            return false;
        }
    }

    return true;
}

bool testStatus() {
    {
        Task task;
        if (task.status() != Threading::TaskCreated) {
            return false;
        }
    }

    {
        func1Var = 0;
        Task task(func1);
        task.wait();
        if (task.status() != Threading::TaskCompletion) {
            return false;
        }
    }

    {
        auto func = [] {
            Thread::msleep(200);
        };
        Task task(func);
        if (task.status() != Threading::TaskRunning) {
            return false;
        }
    }

    {
        auto func = [] {
            Thread::msleep(3000);
        };
        Task task(func);
        if (task.wait(TimeSpan::fromMilliseconds(100))) {
            return false;
        }
        if (task.isCompleted()) {
            return false;
        }
        uint64_t start = Environment::getTickCount();
        task.cancel();
        uint64_t end = Environment::getTickCount();
        int64_t elapsed = (int64_t) (end - start);
        if (elapsed >= 3000) {
            return false;
        }
        if (task.status() != Threading::TaskCanceled) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testResult()) {
        return 2;
    }
    if (!testStatus()) {
        return 3;
    }

    return 0;
}