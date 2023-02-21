//
//  MemoryTraceListenerTest.cpp
//  common
//
//  Created by baowei on 2023/2/17.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "diag/MemoryTraceListener.h"
#include "diag/FileTraceListener.h"
#include "diag/MemoryTraceListener.h"
#include "system/Application.h"

using namespace Diag;
using namespace System;

class TraceListenerContextTest {
public:
    static bool testConstructor() {
        {
            TraceListenerContext test;
            if (!test.enable) {
                return false;
            }
        }
        {
            TraceListenerContext test;
            test.enable = false;
            if (test.enable) {
                return false;
            }
        }
        {
            TraceListenerContext test;
            TraceListenerContext test2 = test;
            if (!test2.enable) {
                return false;
            }
        }

        return true;
    }

    static bool testEquals() {
        {
            TraceListenerContext test;
            test.enable = false;
            TraceListenerContext test2;
            test2.enable = false;
            if (test != test2) {
                return false;
            }
        }
        {
            TraceListenerContext test;
            test.enable = false;
            TraceListenerContext test2;
            test2.enable = true;
            if (test == test2) {
                return false;
            }
        }
        {
            TraceListenerContext test;
            test.enable = false;
            TraceListenerContext test2;
            test2.enable = false;
            if (!test.equals(test2)) {
                return false;
            }
        }

        return true;
    }

    static bool testEvaluates() {
        {
            TraceListenerContext test;
            test.enable = false;
            TraceListenerContext test2;
            test2.evaluates(test);
            if (test != test2) {
                return false;
            }
        }

        return true;
    }

    static int runTest() {
        if (!testConstructor()) {
            return 1;
        }
        if (!testEquals()) {
            return 2;
        }
        if (!testEvaluates()) {
            return 3;
        }

        return 0;
    }
};

class TraceListenerContextsTest {
public:
    static bool testConstructor() {
        {
            TraceListenerContexts test;
            if (!test.autoDelete()) {
                return false;
            }
        }
        {
            TraceListenerContext test;
            TraceListenerContext test2 = test;
            if (!test2.enable) {
                return false;
            }
        }
        {
            TraceListenerContexts test = TraceListenerContexts::Default;
            if (test.count() != 1) {
                return false;
            }
            auto context = dynamic_cast<const FileTraceListenerContext *>(test[0]);
            if (context == nullptr) {
                return false;
            }
        }
        {
            TraceListenerContexts test = TraceListenerContexts::Empty;
            if (test.count() != 0) {
                return false;
            }
        }

        return true;
    }

    static int runTest() {
        if (!testConstructor()) {
            return 11;
        }

        return 0;
    }
};

class TraceUpdatedEventArgsTest {
public:
    static bool testConstructor() {
        {
            TraceUpdatedEventArgs test("message", "category");
            if (test.message != "message") {
                return false;
            }
            if (test.category != "category") {
                return false;
            }
        }

        return true;
    }

    static int runTest() {
        if (!testConstructor()) {
            return 21;
        }

        return 0;
    }
};

class MemoryTraceListenerContextTest {
public:
    static bool testConstructor() {
        {
            MemoryTraceListenerContext test;
            if (test.maxMessageCount != 10000) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test(12000);
            if (test.maxMessageCount != 12000) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test(10);
            if (test.maxMessageCount != 10000) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test(256 * 1024);
            if (test.maxMessageCount != 10000) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test = MemoryTraceListenerContext::Default;
            if (test.maxMessageCount != 10000) {
                return false;
            }
        }

        return true;
    }

    static bool testEquals() {
        {
            MemoryTraceListenerContext test(12000);
            test.enable = false;
            MemoryTraceListenerContext test2(12000);
            test2.enable = false;
            if (test != test2) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test(12000);
            test.enable = false;
            MemoryTraceListenerContext test2(13000);
            test2.enable = true;
            if (test == test2) {
                return false;
            }
        }
        {
            MemoryTraceListenerContext test(12000);
            test.enable = false;
            MemoryTraceListenerContext test2(12000);
            test2.enable = false;
            if (!test.equals(test2)) {
                return false;
            }
        }

        return true;
    }

    static bool testEvaluates() {
        {
            MemoryTraceListenerContext test(12000);
            test.enable = false;
            MemoryTraceListenerContext test2;
            test2.evaluates(test);
            if (test != test2) {
                return false;
            }
        }

        return true;
    }

    static int runTest() {
        if (!testConstructor()) {
            return 31;
        }
        if (!testEquals()) {
            return 32;
        }
        if (!testEvaluates()) {
            return 33;
        }

        return 0;
    }
};

class MemoryTraceListenerTest {
public:
    static bool testConstructor() {
        {
            MemoryTraceListener test;
            const MemoryTraceListenerContext &context = test.context();
            if (context != MemoryTraceListenerContext::Default) {
                return false;
            }
        }
        {
            MemoryTraceListener test(MemoryTraceListenerContext(12000));
            const MemoryTraceListenerContext &context = test.context();
            if (context.maxMessageCount != 12000) {
                return false;
            }
        }

        return true;
    }

    static bool testGetAllMessages() {
        {
            TraceListenerContexts contexts;
            contexts.add(new MemoryTraceListenerContext());
            Application app(String::Empty, contexts);
            Trace::write("error");
            StringArray messages;
            app.getAllMessages(messages);
            if (messages.count() != 1) {
                return false;
            }
            if (messages[0].find("error") <= 0) {
                return false;
            }
        }

        return true;
    }

    static bool testUpdatedDelegates() {
        {
            TraceListenerContexts contexts;
            contexts.add(new MemoryTraceListenerContext());
            Application app(String::Empty, contexts);
            static int result = false;
            auto function = [](void *owner, void *sender, EventArgs *e) {
                auto args = dynamic_cast<TraceUpdatedEventArgs *>(e);
                if (args != nullptr) {
                    const String &message = args->message;
                    const String &category = args->category;
                    if (message.find("error") > 0 && category == Trace::Info) {
                        result = true;
                    } else {
                        result = false;
                    }
                }
            };
            app.logUpdatedDelegates()->add(Delegate(nullptr, function));
            Trace::write("error");
            return result;
        }

        return true;
    }

    static int runTest() {
        if (!testConstructor()) {
            return 41;
        }
        if (!testGetAllMessages()) {
            return 42;
        }
        if (!testUpdatedDelegates()) {
            return 43;
        }

        return 0;
    }
};

int main() {
    int result;
    result = TraceListenerContextTest::runTest();
    if (result != 0) {
        return result;
    }
    result = TraceListenerContextsTest::runTest();
    if (result != 0) {
        return result;
    }
    result = TraceUpdatedEventArgsTest::runTest();
    if (result != 0) {
        return result;
    }
    result = MemoryTraceListenerContextTest::runTest();
    if (result != 0) {
        return result;
    }
    result = MemoryTraceListenerTest::runTest();
    if (result != 0) {
        return result;
    }

    return 0;
}