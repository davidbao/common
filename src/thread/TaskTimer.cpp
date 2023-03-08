//
//  TaskTimer.cpp
//  common
//
//  Created by baowei on 2017/4/3.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "thread/TaskTimer.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "system/Math.h"
#include "diag/Trace.h"

namespace Threading {
    TaskTimer::Group::Group(const String &name, Action *action, const TimeSpan &interval) {
        this->name = name;
        this->action = action;
        this->interval = interval;
        this->start = 0;
    }

    TaskTimer::Group::~Group() {
        if (action != nullptr) {
            delete action;
            action = nullptr;
        }
    }

    bool TaskTimer::Group::isTimeUp() {
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

    void TaskTimer::Group::execute() {
        if (action != nullptr) {
            action->execute();
        }
    }

    TaskTimer::TaskTimer(const String &name) {
        _name = name;
        _timer = nullptr;
    }

    TaskTimer::~TaskTimer() {
        stop();
    }

    bool TaskTimer::add(const String &name, Action *action, const TimeSpan &interval) {
        if (contains(name))
            return false;

        auto *group = new Group(name, action, interval);
        Locker locker(&_groups);
        _groups.add(group);
        return true;
    }

    bool TaskTimer::remove(const String &name) {
        Locker locker(&_groups);
        for (size_t i = 0; i < _groups.count(); i++) {
            if (_groups[i]->name == name) {
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
            if (group->name == name) {
                group->interval = interval;
                group->start = TickTimeout::getCurrentTickCount();

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
            if (_groups[i]->name == name) {
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
        if (isStarted())
            return;

        String name = !_name.isNullOrEmpty() ? _name : "TaskTimer";
        _timer = new Timer(name, (int) dueTime.totalMilliseconds(), 1, &TaskTimer::taskTimeUp, this);

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
