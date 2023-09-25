//
//  ServiceFactoryTest.cpp
//  common
//
//  Created by baowei on 2023/9/22.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "system/ServiceFactory.h"

using namespace System;

class TestService : public IService {
public:
    TestService(const String &text) : _text(text) {
    }

    ~TestService() override {
    }

private:
    String _text;
};

bool testAddService() {
    {
        TestService ts("test1");
        auto sf = ServiceFactory::instance();
        sf->addService(&ts);

        auto actual = sf->getService<TestService>();
        if (actual != &ts) {
            return false;
        }
    }
    {
        TestService ts("test1");
        auto sf = ServiceFactory::instance();
        sf->addService("test1", &ts);

        auto actual = sf->getService<TestService>("test1");
        if (actual != &ts) {
            return false;
        }
    }
    {
        TestService ts("test1"), ts2("test2");
        auto sf = ServiceFactory::instance();
        sf->addService("test1", &ts);
        sf->addService("test2", &ts2);

        auto actual = sf->getService<TestService>("test1");
        if (actual != &ts) {
            return false;
        }
        auto actual2 = sf->getService<TestService>("test2");
        if (actual2 != &ts2) {
            return false;
        }
    }

    return true;
}

bool testRemoveService() {
    {
        TestService ts("test1");
        auto sf = ServiceFactory::instance();
        sf->addService(&ts);

        sf->removeService<TestService>();

        auto actual = sf->getService<TestService>();
        if (actual != nullptr) {
            return false;
        }
    }
    {
        TestService ts("test1");
        auto sf = ServiceFactory::instance();
        sf->addService("test1", &ts);

        sf->removeService<TestService>("test1");

        auto actual = sf->getService<TestService>("test1");
        if (actual != nullptr) {
            return false;
        }
    }
    {
        TestService ts("test1"), ts2("test2");
        auto sf = ServiceFactory::instance();
        sf->addService("test1", &ts);
        sf->addService("test2", &ts2);

        sf->removeService<TestService>("test2");
        auto actual = sf->getService<TestService>("test1");
        if (actual != &ts) {
            return false;
        }

        sf->removeService<TestService>("test1");
        auto actual2 = sf->getService<TestService>("test2");
        if (actual2 != nullptr) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testAddService()) {
        return 1;
    }
    if (!testRemoveService()) {
        return 2;
    }

    return 0;
}