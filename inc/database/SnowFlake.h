//
//  SnowFlake.h
//  common
//
//  Created by baowei on 2023/7/22.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef SnowFlake_h
#define SnowFlake_h

#include "data/ValueType.h"

using namespace Data;

namespace Database {
    class SnowFlake {
    public:
        static uint64_t generateId(int dataCenterId, int workerId);

        static uint64_t generateId(int workerId);

        static uint64_t generateId();
    };
}

#endif // SnowFlake_h