//
//  Locker.cpp
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#include "thread/Locker.h"
#include "thread/Mutex.h"

namespace Common {
    Locker::Locker(IMutex *mutexp) : _mutex(nullptr) {
        if (mutexp != nullptr) {
            _mutex = mutexp;
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
