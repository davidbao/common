//
//  DelegateTest.cpp
//  common
//
//  Created by baowei on 2023/3/12.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/Delegate.h"
#include "data/String.h"

using namespace System;
using namespace Data;

class TestEventArgs : public EventArgs {
public:
    int nValue;
    String strValue;

    TestEventArgs(int value) : nValue(value) {
    }

    TestEventArgs(const String &value) : strValue(value) {
    }
};

class Foo {
public:
    void func1(void *sender, EventArgs *e) {
        auto args = static_cast<TestEventArgs *>(e);
        args->nValue++;
    }

    void func2(void *sender, EventArgs *e) {
        auto args = static_cast<TestEventArgs *>(e);
        args->strValue += args->strValue;
    }
};

bool testConstructor() {
    {
        auto func = [] (void *owner, void *sender, EventArgs *e) {
            auto args = static_cast<TestEventArgs *>(e);
            args->nValue++;
        };
        Delegate test(nullptr, func);
        TestEventArgs args(1);
        test.invoke(nullptr, &args);
        if (args.nValue != 2) {
            return false;
        }
    }
//    {
//        Foo foo;
//        TestEventArgs args(1);
//        typename Delegate::Method<Foo>::Handler handler = &Foo::func1;
////        Foo::handler(&foo)
////        foo.*handler(nullptr, &args);
//        Delegate test(handler);
//        test.invoke(nullptr, &args);
//        if (args.nValue != 2) {
//            return false;
//        }
//    }

    return true;
}

int main() {
    if (!testConstructor()) {
        return 1;
    }

    return 0;
}