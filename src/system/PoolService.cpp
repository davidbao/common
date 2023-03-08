//
//  PoolService.cpp
//  common
//
//  Created by baowei on 18/4/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#include <cassert>
#include "system/PoolService.h"

namespace System {
    PoolBaseService::PoolBaseService(const TimeSpan &interval) {
        _timer = nullptr;
        _interval = interval;
    }

    PoolBaseService::~PoolBaseService() {
        stop();
    }

    void PoolBaseService::start() {
        if (_timer == nullptr) {
            _timer = new Timer("PoolService.timer", _interval, &PoolBaseService::processProc, this);
        }
    }

    void PoolBaseService::stop() {
        invoke();

        if (_timer != nullptr) {
            _timer->stop(TimeSpan::fromSeconds(30));
            delete _timer;
            _timer = nullptr;
        }
    }

    bool PoolBaseService::isStarted() const {
        return _timer != nullptr && _timer->running();
    }

    void PoolBaseService::invoke() {
    }

    void PoolBaseService::processProc() {
        invoke();
    }

    IPoolEntry::IPoolEntry() = default;

    IPoolEntry::~IPoolEntry() = default;

    PoolService::PoolService(bool batch, const TimeSpan &interval, int maxLength) : PoolBaseService(interval),
                                                                                    _values(maxLength), _batch(batch) {
    }

    PoolService::~PoolService() {
        invoke();
    }

    void PoolService::process(const IPoolEntry *value) {

    }

    void PoolService::process(const IPoolEntry **value, size_t count) {
    }

    void PoolService::add(const IPoolEntry *value) {
        _valuesMutex.lock();
        _values.enqueue(value);
        _valuesMutex.unlock();
    }

    void PoolService::invoke() {
        _valuesMutex.lock();
        size_t count = _values.count();
        if (count > 0) {
            auto values = new IPoolEntry *[count];
            _values.copyTo(values);
            _values.makeNull(false);
            _valuesMutex.unlock();

            if (!_batch) {
                for (uint32_t i = 0; i < count; i++) {
                    IPoolEntry *value = values[i];
                    process(value);
                    delete value;
                }
            } else {
                process((const IPoolEntry **) values, count);
                for (uint32_t i = 0; i < count; i++) {
                    IPoolEntry *value = values[i];
                    delete value;
                }
            }
            delete[] values;
        } else {
            _valuesMutex.unlock();
        }
    }
}
