//
//  StopMemoryTest.cpp
//  common
//
//  Created by baowei on 2023/8/7.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifdef DEBUG
#include "diag/StopMemory.h"

using namespace Diag;

bool testConstructor() {
    {
        StopMemory sm;
        if (!sm.info().isNullOrEmpty()) {
            return false;
        }
    }
    {
        StopMemory sm("testAbc123");
        if (sm.info() != "testAbc123") {
            return false;
        }
    }
    {
        StopMemory sm("testAbc123", 1024);
        if (sm.info() != "testAbc123") {
            return false;
        }
        if (sm.deadMemory() != 1024) {
            return false;
        }
    }

    return true;
}

bool testStart() {
    {
        StopMemory sm("testAbc123");
        auto value = malloc(1024 * 512);
        sm.stop();
        free(value);
        int64_t used = sm.used();
        if (used < 1024 * 512) {
            printf("used1: %lld\n", used);
            return false;
        }
    }

    {
        StopMemory sm("testAbc123");
        auto value = malloc(1024 * 640);
        sm.stop();
        free(value);
        int64_t used = sm.used();
        if (used < 1024 * 640) {
            printf("used2: %lld\n", used);
            return false;
        }
        sm.reStart();
        value = malloc(1024 * 1024 * 1);
        sm.stop();
        free(value);
        used = sm.used();
        if (used < 1024 * 1024 * 1) {
            printf("used3: %lld\n", used);
            return false;
        }
    }

    return true;
}

bool testProperties() {
    {
        StopMemory sm("testAbc123");
        if (sm.info() != "testAbc123") {
            return false;
        }
        sm.setInfo("testAbc456");
        if (sm.info() != "testAbc456") {
            return false;
        }
    }
    {
        StopMemory sm("testAbc123", 1000);
        if (sm.deadMemory() != 1000) {
            return false;
        }
    }

    {
        StopMemory sm;
        if (!sm.isRunning()) {
            return false;
        }
        sm.stop();
        if (sm.isRunning()) {
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
#endif  // DEBUG
