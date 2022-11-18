//
//  PoolService.cpp
//  common
//
//  Created by baowei on 18/4/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#include <assert.h>
#include "system/PoolService.h"

namespace Common {
    PoolBaseService::PoolBaseService(TimeSpan interval) {
        _thread = nullptr;
        _interval = interval;
    }

    PoolBaseService::~PoolBaseService() {
        stop();
    }

    void PoolBaseService::start() {
        if (_thread == nullptr) {
            _thread = new Thread("PoolService");
            _thread->startProc(processProc, this, _interval);
        }
    }

    void PoolBaseService::stop() {
        invoke();

        if (_thread != nullptr) {
            _thread->stop(TimeSpan::fromSeconds(30));
            delete _thread;
            _thread = nullptr;
        }
    }

    bool PoolBaseService::isStarted() const {
        return _thread != nullptr && _thread->isAlive();
    }

    void PoolBaseService::invoke() {
    }

    void PoolBaseService::processProc(void *parameter) {
        PoolBaseService *service = (PoolBaseService *) parameter;
        assert(service);
        service->processProcInner();
    }

    void PoolBaseService::processProcInner() {
        invoke();
    }

    IPoolEntry::IPoolEntry() {
    }

    IPoolEntry::~IPoolEntry() {
    }

    PoolService::PoolService(bool batch, TimeSpan interval, int maxLength) : PoolBaseService(interval),
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
            IPoolEntry **values = new IPoolEntry *[count];
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
