//
//  Locker.cpp
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "thread/Locker.h"
#include "thread/Mutex.h"

namespace Threading {
    Locker::Locker(IMutex *m) : _mutex(nullptr) {
        if (m != nullptr) {
            _mutex = m;
            _mutex->lock();
        }
    }

    Locker::~Locker() {
        if (_mutex != nullptr) {
            _mutex->unlock();
            _mutex = nullptr;
        }
    }
}
