//
//  TaskTimer.cpp
//  common
//
//  Created by baowei on 2017/4/3.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "thread/TaskTimer.h"
#include "thread/Locker.h"
#include "system/Math.h"
#include "diag/Trace.h"
#include "system/Environment.h"

namespace Threading {
    const String &TaskTimer::Group::name() const {
        return _name;
    }

    bool TaskTimer::Group::isTimeUp() {
        if (_start == 0) {
            _start = Environment::getTickCount();
            return true;
        }

        assert(_interval != TimeSpan::Zero);
        bool result = Environment::getTickCount() - _start > (uint64_t) _interval.totalMilliseconds();
        if (result) {
            _start = Environment::getTickCount();
        }
        return result;
    }

    void TaskTimer::Group::execute() const {
        _action.execute();
    }

    void TaskTimer::Group::change(const String &name, const TimeSpan &interval) {
        _interval = interval;
        _start = 0;
    }

    TaskTimer::TaskTimer(const String &name) :
            _name(!name.isNullOrEmpty() ? name : "TaskTimer"), _timer(nullptr) {
    }

    TaskTimer::~TaskTimer() {
        stop();
    }

    bool TaskTimer::remove(const String &name) {
        Locker locker(&_groups);
        for (size_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->name() == name) {
                _groups.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool TaskTimer::change(const String &name, const TimeSpan &interval) {
        ThreadId currentThreadId = Thread::currentThreadId();
        if (_currentThreadId != currentThreadId) {
            _groups.lock();
        }

        for (size_t i = 0; i < _groups.count(); i++) {
            Group *group = _groups[i];
            if (group->name() == name) {
                group->change(name, interval);

                if (_currentThreadId != currentThreadId) {
                    _groups.unlock();
                }
                return true;
            }
        }

        if (_currentThreadId != currentThreadId) {
            _groups.unlock();
        }
        return false;
    }

    bool TaskTimer::contains(const String &name) {
        Locker locker(&_groups);
        for (size_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->name() == name) {
                return true;
            }
        }
        return false;
    }

    void TaskTimer::clear() {
        Locker locker(&_groups);
        _groups.clear();
    }

    void TaskTimer::start(const TimeSpan &dueTime) {
        if (running())
            return;

        _timer = new Timer(name(), (int) dueTime.totalMilliseconds(), 1, &TaskTimer::taskTimeUp, this);
    }

    void TaskTimer::stop() {
        if (_timer != nullptr) {
            delete _timer;
            _timer = nullptr;
        }
    }

    bool TaskTimer::running() const {
        return _timer != nullptr && _timer->running();
    }

    const String &TaskTimer::name() const {
        return _name;
    }

    bool TaskTimer::hasTimer() {
        Locker locker(&_groups);
        return _groups.count() > 0;
    }

    void TaskTimer::taskTimeUp() {
        _currentThreadId = Thread::currentThreadId();

        Locker locker(&_groups);

        for (size_t i = 0; i < _groups.count(); i++) {
            Group *group = _groups[i];
            if (group->isTimeUp()) {
                group->execute();
            }
        }
    }
}
