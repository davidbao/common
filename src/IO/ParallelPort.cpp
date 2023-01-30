//
//  ParallelPort.cpp
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "IO/ParallelPort.h"

namespace IO {
    ParallelPort::ParallelPort(const String &name) : IOPort(), _name(name) {
    }

    ParallelPort::~ParallelPort() = default;

    const String &ParallelPort::name() const {
        return _name;
    }
}
