//
//  Mutex.cpp
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#include "thread/Mutex.h"

namespace Threading {
    IMutex::IMutex() {
    }

    IMutex::~IMutex() {
    }

    Mutex::Mutex() {
        _mutex = new mutex();
    }

    Mutex::~Mutex() {
        delete _mutex;
    }

    void Mutex::lock() {
        _mutex->lock();
    }

    bool Mutex::tryLock() {
        return _mutex->try_lock();
    }

    void Mutex::unlock() {
        _mutex->unlock();
    }

    RecursiveMutex::RecursiveMutex() {
        _mutex = new recursive_mutex();
    }

    RecursiveMutex::~RecursiveMutex() {
        delete _mutex;
    }

    void RecursiveMutex::lock() {
        _mutex->lock();
    }

    bool RecursiveMutex::tryLock() {
        return _mutex->try_lock();
    }

    void RecursiveMutex::unlock() {
        _mutex->unlock();
    }
}
