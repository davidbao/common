//
//  SqliteClientTest.cpp
//  common
//
//  Created by baowei on 2022/11/24.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "database/SnowFlake.h"

using namespace Database;

bool testGenerateId() {
    {
        uint64_t id = SnowFlake::generateId();
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 1000; ++i) {
            id = SnowFlake::generateId();
//            printf("id = %lld\n", id);
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 5000; ++i) {
            id = SnowFlake::generateId();
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 1000 * 1000; ++i) {
            id = SnowFlake::generateId();
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }

    {
        uint64_t id = SnowFlake::generateId(1, 2);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(1, 2);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(33, 2);
        if (id != 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(33);
        if (id != 0) {
            return false;
        }
    }

    {
        uint64_t id = SnowFlake::generateId(2);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(3);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(4);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = SnowFlake::generateId(33);
        if (id != 0) {
            return false;
        }
    }

    return true;
}

int main() {
    if(!testGenerateId()) {
        return 1;
    }

    return 0;
}