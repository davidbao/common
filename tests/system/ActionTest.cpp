//
//  ActionTest.cpp
//  common
//
//  Created by baowei on 2023/3/6.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/Action.h"
#include "data/String.h"

using namespace System;
using namespace Data;

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

class FooStr {
public:
    void func1() {
        _var += "1";
    }

    void func2(const String &var) {
        _var += var;
    }

    String func3(const String &x, const String &y) {
        return x + y;
    }

    String var() const {
        return _var;
    }

private:
    String _var;
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
        Action test = Action(func1);
        test.execute();
        if (func1Var != 1) {
            return false;
        }

        test = Action(&func2, 3);
        func2Var = 0;
        test.execute();
        if (func2Var != 3) {
            return false;
        }
    }
    {
        func1Var = 0;
        Action test(&func1);
        Action test2(std::move(test));
        test.execute();
        if (func1Var != 0) {
            return false;
        }
        test2.execute();
        if (func1Var != 1) {
            return false;
        }
    }

    return true;
}

bool testAction() {
    {
        func1Var = 0;
        Action test(&func1);
        test.execute();
        if (func1Var != 1) {
            return false;
        }
    }
    {
        func1Var = 0;
        Action test(&func1);
        test.execute();
        test.execute();
        if (func1Var != 2) {
            return false;
        }
    }
    {
        func2Var = 0;
        Action test(&func2, 3);
        test.execute();
        if (func2Var != 3) {
            return false;
        }
    }
    {
        func2Var = 0;
        Action test(&func2, 3);
        test.execute();
        test.execute();
        if (func2Var != 6) {
            return false;
        }
    }

    {
        Foo foo;
        Action test(&Foo::func1, &foo);
        test.execute();
        if (foo.var() != 1) {
            return false;
        }
    }
    {
        Foo foo;
        Action test(&Foo::func1, &foo);
        test.execute();
        test.execute();
        if (foo.var() != 2) {
            return false;
        }
    }
    {
        Foo foo;
        Action test(&Foo::func2, &foo, 3);
        test.execute();
        if (foo.var() != 3) {
            return false;
        }
    }
    {
        Foo foo;
        Action test(&Foo::func2, &foo, 3);
        test.execute();
        test.execute();
        if (foo.var() != 6) {
            return false;
        }
    }

    {
        FooStr foo;
        Action test(&FooStr::func1, &foo);
        test.execute();
        if (foo.var() != "1") {
            return false;
        }
    }
    {
        FooStr foo;
        Action test(&FooStr::func1, &foo);
        test.execute();
        test.execute();
        if (foo.var() != "11") {
            return false;
        }
    }
    {
        FooStr foo;
        Action test(&FooStr::func2, &foo, "3");
        test.execute();
        if (foo.var() != "3") {
            return false;
        }
    }
    {
        FooStr foo;
        Action test(&FooStr::func2, &foo, "3");
        test.execute();
        test.execute();
        if (foo.var() != "33") {
            return false;
        }
    }

    return true;
}

bool testFunc() {
    {
        Func<int> test(&func3, 1, 2);
        if (test.execute() != 3) {
            return false;
        }
    }
    {
        Func<int> test(&func3, 1, 2);
        if (test.execute() != 3) {
            return false;
        }
        if (test.execute() != 3) {
            return false;
        }
    }
    {
        Foo foo;
        Func<int> test(&Foo::func3, &foo, 1, 2);
        if (test.execute() != 3) {
            return false;
        }
    }
    {
        Foo foo;
        Func<int> test(&Foo::func3, &foo, 1, 2);
        if (test.execute() != 3) {
            return false;
        }
        if (test.execute() != 3) {
            return false;
        }
    }

    {
        FooStr foo;
        Func<String> test(&FooStr::func3, &foo, "1", "2");
        if (test.execute() != "12") {
            return false;
        }
    }
    {
        FooStr foo;
        Func<String> test(&FooStr::func3, &foo, "1", "2");
        if (test.execute() != "12") {
            return false;
        }
        if (test.execute() != "12") {
            return false;
        }
    }

    return true;
}

bool testInvoke() {
    {
        func1Var = 0;
        Action::invoke(&func1);
        if (func1Var != 1) {
            return false;
        }
    }
    {
        func1Var = 0;
        Action::invoke(&func1);
        Action::invoke(&func1);
        if (func1Var != 2) {
            return false;
        }
    }
    {
        func2Var = 0;
        Action::invoke(&func2, 3);
        if (func2Var != 3) {
            return false;
        }
    }
    {
        func2Var = 0;
        Action::invoke(&func2, 3);
        Action::invoke(&func2, 3);
        if (func2Var != 6) {
            return false;
        }
    }

    {
        Foo foo;
        Action::invoke(&Foo::func1, &foo);
        if (foo.var() != 1) {
            return false;
        }
    }
    {
        Foo foo;
        Action::invoke(&Foo::func1, &foo);
        Action::invoke(&Foo::func1, &foo);
        if (foo.var() != 2) {
            return false;
        }
    }
    {
        Foo foo;
        Action::invoke(&Foo::func2, &foo, 3);
        if (foo.var() != 3) {
            return false;
        }
    }
    {
        Foo foo;
        Action::invoke(&Foo::func2, &foo, 3);
        Action::invoke(&Foo::func2, &foo, 3);
        if (foo.var() != 6) {
            return false;
        }
    }

    {
        int result = Action::invoke(&func3, 1, 2);
        if (result != 3) {
            return false;
        }
    }
    {
        int result = Action::invoke(&func3, 1, 2);
        if (result != 3) {
            return false;
        }
        result = Action::invoke(&func3, 1, 2);
        if (result != 3) {
            return false;
        }
    }
    {
        Foo foo;
        int result = Action::invoke(&Foo::func3, &foo, 1, 2);
        if (result != 3) {
            return false;
        }
    }
    {
        Foo foo;
        int result = Action::invoke(&Foo::func3, &foo, 1, 2);
        if (result != 3) {
            return false;
        }
        result = Action::invoke(&Foo::func3, &foo, 1, 2);
        if (result != 3) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }
    if (!testAction()) {
        return 2;
    }
    if (!testFunc()) {
        return 3;
    }
    if (!testInvoke()) {
        return 4;
    }

    return 0;
}