//
//  TaskTimer.cpp
//  common
//
//  Created by baowei on 03/04/2017.
//  Copyright © 2017 com. All rights reserved.
//

#include "thread/TaskTimer.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "system/Math.h"
#include "diag/Trace.h"

namespace Threading {
    TaskTimer::Group::Group(const String &name, timer_callback callback, const TimeSpan &interval, void *owner) {
        this->name = name;
        this->callback = callback;
        this->execution = nullptr;
        this->interval = interval;
        this->start = 0;
        this->owner = owner;
    }

    TaskTimer::Group::Group(const String &name, TimerExecution *execution, const TimeSpan &interval) {
        this->name = name;
        this->callback = nullptr;
        this->execution = execution;
        this->interval = interval;
        this->start = 0;
        this->owner = nullptr;
    }

    TaskTimer::Group::~Group() {
        if (execution != nullptr) {
            delete execution;
            execution = nullptr;
        }
    }

    bool TaskTimer::Group::isTimeup() {
        if (start == 0) {
            start = TickTimeout::getCurrentTickCount();
            return true;
        }

        assert(interval != TimeSpan::Zero);
        bool result = TickTimeout::isTimeout(start, interval);
        if (result) {
            start = TickTimeout::getCurrentTickCount();
        }
        return result;
    }

    TaskTimer::TaskTimer(const String &name, void *owner) {
        _name = name;
        _owner = owner;
        _timer = nullptr;
    }

    TaskTimer::~TaskTimer() {
        stop();

        _owner = nullptr;
    }

    bool TaskTimer::add(const String &name, timer_callback callback, const TimeSpan &interval, void *owner) {
        if (contains(name))
            return false;

        Group *group = new Group(name, callback, interval, owner);
        Locker locker(&_groupsMutex);
        _groups.add(group);
        return true;
    }

    bool TaskTimer::add(const String &name, timer_callback callback, void *owner, const TimeSpan &interval) {
        return add(name, callback, interval, owner);
    }

    bool TaskTimer::remove(timer_callback callback) {
        Locker locker(&_groupsMutex);
        for (uint32_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->callback == callback) {
                _groups.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool TaskTimer::remove(const String &name) {
        Locker locker(&_groupsMutex);
        for (uint32_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->name == name) {
                _groups.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool TaskTimer::change(const String &name, const TimeSpan &interval) {
        Thread::Id currentThreadId = Thread::currentThreadId();
        if (_currentThreadId != currentThreadId) {
            _groupsMutex.lock();
        }

        for (uint32_t i = 0; i < _groups.count(); i++) {
            Group *group = _groups[i];
            if (group->name == name) {
                group->interval = interval;
                group->start = TickTimeout::getCurrentTickCount();

                if (_currentThreadId != currentThreadId) {
                    _groupsMutex.unlock();
                }
                return true;
            }
        }

        if (_currentThreadId != currentThreadId) {
            _groupsMutex.unlock();
        }
        return false;
    }

    bool TaskTimer::contains(timer_callback callback) {
        Locker locker(&_groupsMutex);
        for (uint32_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->callback == callback) {
                return true;
            }
        }
        return false;
    }

    bool TaskTimer::contains(const String &name) {
        Locker locker(&_groupsMutex);
        for (uint32_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->name == name) {
                return true;
            }
        }
        return false;
    }

    void TaskTimer::clear() {
        Locker locker(&_groupsMutex);
        _groups.clear();
    }

    void TaskTimer::start(const TimeSpan &dueTime) {
        if (isStarted())
            return;

        String name = !_name.isNullOrEmpty() ? _name : "TaskTimer";
        if (dueTime == TimeSpan::Zero)
            _timer = new Timer(name, taskTimeUp, this, 1);
        else
            _timer = new Timer(name, taskTimeUp, this, (int) dueTime.totalMilliseconds(), 1);

//        uint32_t interval = 0;      // ms
//        if(_groups.count() == 1)
//        {
//            interval = (uint32_t)_groups[0]->interval.totalMilliseconds();
//        }
//        else if(_groups.count() > 1)
//        {
//            Group* group1 = _groups[0];
//            Group* group2 = _groups[1];
//            interval = Math::getGreatestCommonDivisor((uint32_t)group1->interval.totalMilliseconds(), (uint32_t)group2->interval.totalMilliseconds());
//            
//            for (uint32_t i=2; i<_groups.count(); i++)
//            {
//                Group* group = _groups[i];
//                interval = Math::getGreatestCommonDivisor(interval, (uint32_t)group->interval.totalMilliseconds());
//            }
//        }
//        
//        if(interval > 0)
//        {
//            interval = interval >= 1000 ? interval / 100 : interval;
//            _timer = new Timer(taskTimeUp, this, interval);
//        }
    }

    void TaskTimer::stop() {
        if (_timer != nullptr) {
            delete _timer;
            _timer = nullptr;
        }
    }

    bool TaskTimer::isStarted() const {
        return _timer != nullptr;
    }

    void TaskTimer::taskTimeUp(void *state) {
        TaskTimer *tt = static_cast<TaskTimer *>(state);
        assert(tt);
        tt->taskTimeUpInner();
    }

    void TaskTimer::taskTimeUpInner() {
        _currentThreadId = Thread::currentThreadId();

        Locker locker(&_groupsMutex);

        for (uint32_t i = 0; i < _groups.count(); i++) {
            Group *group = _groups[i];
            if (group->isTimeup()) {
                if (group->callback != nullptr)
                    group->callback(group->owner != nullptr ? group->owner : _owner);
                else if (group->execution != nullptr)
                    group->execution->execute();
            }
        }
    }
}
