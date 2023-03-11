//
//  Task.cpp
//  common
//
//  Created by baowei on 2023/3/10.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "thread/Task.h"

namespace Threading {
    TaskBackground::TaskBackground() {
        _thread = Thread("task", &TaskBackground::execute, this);
    }

    void TaskBackground::execute() {
        _status = TaskCompletion;
    }

    TaskStatus TaskBackground::status() const {
        return _status;
    }

    bool TaskBackground::isCompleted() const {
        return !_thread.isAlive();
    }

    bool TaskBackground::wait(const TimeSpan &timeout) {
        if (timeout == TimeSpan::Zero) {
            _thread.join();
            return true;
        } else {
            auto isThreadDead = [](Thread *thread) {
                return !thread->isAlive();
            };
            Thread::delay(timeout, Func<bool>(isThreadDead, &_thread));
            if (_thread.isAlive()) {
                _status = TaskTimeout;
                return false;
            } else {
                return true;
            }
        }
    }

    bool TaskBackground::wait(int delaySeconds) {
        return wait(TimeSpan::fromSeconds(delaySeconds));
    }

    void TaskBackground::cancel(const TimeSpan &delay) {
        if (_status == TaskCanceled) {
            return;
        }
        if (_thread.id() != Thread::currentThreadId()) {
            if (delay > TimeSpan::Zero) {
                auto isThreadDead = [](Thread *thread) {
                    return !thread->isAlive();
                };
                Thread::delay(delay, Func<bool>(isThreadDead, &_thread));
            }
            if (_thread.isAlive()) {
                _status = TaskCanceled;
                _thread.detach();
            }
        } else {
            _status = TaskCanceled;
        }
    }

    void TaskBackground::cancel(int delaySeconds) {
        cancel(TimeSpan::fromSeconds(delaySeconds));
    }

    void TaskBackground::start() {
        _thread.start();
        _status = _thread.isAlive() ? TaskRunning : TaskFaulted;
    }

    Task::Task() : _state(nullptr) {
    }

    Task::Task(Task &&other) noexcept {
        _state = other._state;
        other._state = nullptr;
    }

    Task &Task::operator=(Task &&other) noexcept {
        if (this != &other) {
            _state = other._state;
            other._state = nullptr;
        }
        return *this;
    }

    Task::~Task() {
        delete _state;
        _state = nullptr;
    }

    TaskStatus Task::status() const {
        return _state != nullptr ? _state->status() : TaskCreated;
    }

    bool Task::isCompleted() const {
        return _state != nullptr && _state->isCompleted();
    }

    bool Task::wait(const TimeSpan &timeout) {
        return _state != nullptr && _state->wait(timeout);
    }

    void Task::cancel(const TimeSpan &delay) {
        if (_state != nullptr) {
            _state->cancel(delay);
        }
    }

    void Task::cancel(int delaySeconds) {
        if (_state != nullptr) {
            _state->cancel(delaySeconds);
        }
    }
}
