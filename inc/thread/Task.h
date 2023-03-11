//
//  Task.h
//  common
//
//  Created by baowei on 2023/3/8.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef Task_h
#define Task_h

#include "system/Action.h"
#include "thread/Thread.h"
#include "thread/Locker.h"

using namespace System;

namespace Threading {
    enum TaskStatus {
        TaskCreated,
        TaskRunning,
        TaskFaulted,
        TaskCompletion,
        TaskTimeout,
        TaskCanceled
    };

    template<class TResult>
    class TaskResult {
    public:
        TaskResult() = default;

        TaskResult(const TaskResult &other) {
            _result = other._result;
        }

        TaskResult &operator=(const TaskResult &other) {
            if (this != &other) {
                _result = other._result;
            }
            return *this;
        }

        ~TaskResult() = default;

        void setValue(const TResult &result) {
            Locker locker(&_mutex);
            _result = result;
        }

        TResult getValue() {
            Locker locker(&_mutex);
            return _result;
        }

    private:
        TResult _result;
        Mutex _mutex;
    };

    class TaskBackground {
    public:
        TaskBackground();

        virtual ~TaskBackground() = default;

        virtual void *resultPtr() = 0;

        virtual void execute();

        TaskStatus status() const;

        bool isCompleted() const;

        bool wait(const TimeSpan &timeout = TimeSpan::Zero);

        bool wait(int delaySeconds);

        void cancel(const TimeSpan &delay = TimeSpan::Zero);

        void cancel(int delaySeconds);

        template<class TResult>
        TResult result() {
            auto r = static_cast<TaskResult<TResult> *>(resultPtr());
            return r != nullptr ? r->getValue() : TResult();
        }

    protected:
        void start();

    private:
        Thread _thread;

        TaskStatus _status;
    };

    class Task;

    template<class TResult>
    class TaskState : public TaskBackground {
    private:
        template<class Function, class... Args>
        explicit TaskState(Function &&f, Args &&... args) : TaskBackground(), _action(f, args...) {
            start();
        }

        void execute() override {
            _result.setValue(_action.execute());

            TaskBackground::execute();
        }

        void *resultPtr() override {
            return &_result;
        }

    private:
        Func<TResult> _action;

        TaskResult<TResult> _result;

        friend Task;
    };

    template<class TResult>
    class TaskState<TResult &> : public TaskBackground {
    private:
        template<class Function, class... Args>
        explicit TaskState(Function &&f, Args &&... args) : TaskBackground(), _action(f, args...) {
            start();
        }

        void execute() override {
            _result.setValue(_action.execute());

            TaskBackground::execute();
        }

        void *resultPtr() override {
            return &_result;
        }

    private:
        Func<TResult &> _action;

        TaskResult<TResult &> _result;

        friend Task;
    };

    template<>
    class TaskState<void> : public TaskBackground {
    private:
        template<class Function, class... Args>
        explicit TaskState(Function &&f, Args &&... args) : TaskBackground(), _action(f, args...) {
            start();
        }

        void execute() override {
            _action.execute();

            TaskBackground::execute();
        }

        void *resultPtr() override {
            return nullptr;
        }

    private:
        Func<void> _action;

        friend Task;
    };

    class Task {
    public:
        Task();

        template<class Function, class... Args>
        explicit Task(Function &&f, Args &&... args) {
            typedef typename std::result_of<typename std::decay<Function>::type(
                    typename std::decay<Args>::type...)>::type Result;
            _state = new TaskState<Result>(f, args...);
        }

        Task(Task &&other) noexcept;

        Task(const Task &) = delete;

        Task &operator=(const Task &) = delete;

        Task &operator=(Task &&other) noexcept;

        ~Task();

        TaskStatus status() const;

        template<class TResult>
        TResult result() {
            if (_state != nullptr) {
                if (!isCompleted()) {
                    wait();
                }
                return _state->result<TResult>();
            } else {
                return TResult();
            }
        }

        bool isCompleted() const;

        bool wait(const TimeSpan &timeout = TimeSpan::Zero);

        void cancel(const TimeSpan &delay = TimeSpan::Zero);

        void cancel(int delaySeconds);

    public:
        template<class Function, class... Args>
        static Task run(Function &&f, Args &&... args) {
            return Task(f, args...);
        }

    private:
        TaskBackground *_state;
    };
}

#endif // Task_h
