//
//  StopwatchTest.cpp
//  common
//
//  Created by baowei on 2023/8/7.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "diag/Stopwatch.h"
#include "thread/Thread.h"

using namespace Diag;
using namespace Threading;

bool testConstructor() {
    {
        Stopwatch sw;
        if (!sw.info().isNullOrEmpty()) {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123");
        if (sw.info() != "testAbc123") {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123", TimeSpan::fromSeconds(3));
        if (sw.info() != "testAbc123") {
            return false;
        }
        if (sw.deadTime() != TimeSpan::fromSeconds(3)) {
            return false;
        }
    }

    {
        Stopwatch sw(1000);
        if (sw.deadTimeMilliseconds() != 1000) {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123", 1000);
        if (sw.info() != "testAbc123") {
            return false;
        }
        if (sw.deadTimeMilliseconds() != 1000) {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        Stopwatch sw("testAbc123");
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsedMilliseconds() < 200) {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123");
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsed() < TimeSpan::fromMilliseconds(200)) {
            return false;
        }
    }

    {
        Stopwatch sw("testAbc123");
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsedMilliseconds() < 200) {
            return false;
        }
        sw.start(100);
        Thread::sleep(100);
        sw.stop();
        if (sw.elapsedMilliseconds() < 100) {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123");
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsedMilliseconds() < 200) {
            return false;
        }
        sw.start(TimeSpan::fromMilliseconds(100));
        Thread::sleep(100);
        sw.stop();
        if (sw.elapsedMilliseconds() < 100) {
            return false;
        }
    }

    {
        Stopwatch sw("testAbc123");
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsedMilliseconds() < 200) {
            return false;
        }
        sw.reStart();
        Thread::sleep(200);
        sw.stop();
        if (sw.elapsedMilliseconds() < 200) {
            return false;
        }
    }

    return true;
}

bool testProperties() {
    {
        Stopwatch sw("testAbc123");
        if (sw.info() != "testAbc123") {
            return false;
        }
        sw.setInfo("testAbc456");
        if (sw.info() != "testAbc456") {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123", 1000);
        if (sw.deadTimeMilliseconds() != 1000) {
            return false;
        }
    }
    {
        Stopwatch sw("testAbc123", 1000);
        if (sw.deadTime() != TimeSpan::fromMilliseconds(1000)) {
            return false;
        }
    }

    {
        Stopwatch sw;
        if (!sw.isRunning()) {
            return false;
        }
        sw.stop();
        if (sw.isRunning()) {
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
    if (!testProperties()) {
        return 3;
    }

    return 0;
}