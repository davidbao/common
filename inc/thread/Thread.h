//
//  Thread.h
//  common
//
//  Created by baowei on 2015/7/25.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Thread_h
#define Thread_h

#include <memory>
#include <cstdlib>
#include "data/String.h"
#include "data/ValueType.h"
#include "data/PrimitiveInterface.h"
#include "data/PList.h"
#include "data/DateTime.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

#include <thread>
#include <atomic>

using namespace std;

namespace Threading {
    class ThreadHolder {
    public:
        class Value {
        public:
            Value();

            virtual ~Value();
        };

        void *owner;
        Value *value;
        bool autoDeleteValue;

        explicit ThreadHolder(void *owner = nullptr, Value *value = nullptr, bool autoDeleteValue = true);

        ~ThreadHolder();
    };

    typedef void (*ThreadStart)();

    typedef void (*ParameterizedThreadStart)(void *);

    typedef void (*ThreadHolderAction)(ThreadHolder *);

    class ThreadExecution : public IEquatable<ThreadExecution> {
    public:
        ThreadExecution() = default;

        virtual void execute() = 0;
    };

    template<class T>
    class ObjectThreadStart : public ThreadExecution, public ICloneable<ThreadExecution> {
    public:
        typedef void (T::*Method)();

        typedef void (T::*HolderMethod)(ThreadHolder *holder);

        ObjectThreadStart() : _instance(nullptr), _method(nullptr),
                              _hMethod(nullptr), _holder(nullptr) {
        }

        ObjectThreadStart(T *instance, Method method) : _instance(instance), _method(method),
                                                        _hMethod(nullptr), _holder(nullptr) {
        }

        ObjectThreadStart(T *instance, HolderMethod method, ThreadHolder *holder) : _instance(instance),
                                                                                    _method(nullptr),
                                                                                    _hMethod(method),
                                                                                    _holder(holder) {
        }

        ObjectThreadStart(const ObjectThreadStart &callback) : _instance(nullptr), _method(nullptr),
                                                               _hMethod(nullptr), _holder(nullptr) {
            this->operator=(callback);
        }

        void operator()() {
            this->execute();
        }

        void execute() override {
            if (_instance != nullptr) {
                if (_method != nullptr)
                    return (_instance->*_method)();
                else if (_hMethod != nullptr)
                    return (_instance->*_hMethod)(_holder);
            }
        }

        ThreadExecution *clone() const override {
            if (_method != nullptr)
                return new ObjectThreadStart<T>(_instance, _method);
            else if (_hMethod != nullptr)
                return new ObjectThreadStart<T>(_instance, _hMethod, _holder);
            else
                return new ObjectThreadStart<T>();
        }

        bool equals(const ThreadExecution &other) const override {
            auto callback = dynamic_cast<const ObjectThreadStart *>(&other);
            return _instance == callback->_instance && _method == callback->_method &&
                   _hMethod == callback->_hMethod && _holder == callback->_holder;
        }

        ObjectThreadStart &operator=(const ObjectThreadStart &other) {
            if (this != &other) {
                _instance = other._instance;
                _method = other._method;
                _hMethod = other._hMethod;
                _holder = other._holder;
            }
            return *this;
        }

        T *instance() const {
            return _instance;
        }

        bool canExecution() const {
            return _instance != nullptr && (_method != nullptr || _hMethod != nullptr);
        }

    private:
        T *_instance;
        Method _method;

        HolderMethod _hMethod;
        ThreadHolder *_holder;
    };

    class Thread;

    class ThreadId : public IEquatable<ThreadId>, public IEvaluation<ThreadId>, public IComparable<ThreadId> {
    public:
#define threadId_t thread::id

        ThreadId();

        ThreadId(const ThreadId &id);

        ~ThreadId() override;

        bool equals(const ThreadId &other) const override;

        void evaluates(const ThreadId &other) override;

        int compareTo(const ThreadId &other) const override;

        ThreadId &operator=(const ThreadId &value);

    private:
        explicit ThreadId(const threadId_t &id);

    private:
        friend Thread;

        threadId_t _id;
    };

    enum ThreadPriority {
        /*=========================================================================
        ** Constants for thread priorities.
        =========================================================================*/
        Lowest = 0,
        BelowNormal = 1,
        Normal = 2,
        AboveNormal = 3,
        Highest = 4,
        PriorityLast = Highest,
        PriorityCount = PriorityLast + 1
    };

    class Thread {
    public:
        Thread(const String &name, ThreadStart start);

        Thread(const String &name, ParameterizedThreadStart start);

        template<class T>
        Thread(const String &name, const ObjectThreadStart<T> &start) : Thread(name) {
            _action.cStart = start.canExecution() ? start.clone() : nullptr;
        }

        ~Thread();

        bool isAlive() const;

        void start(void *parameter = nullptr);

        const String &name() const;

        ThreadPriority priority();

        void setPriority(ThreadPriority priority);

        void join();

        void detach();

        ThreadId id() const;

        bool joinable() const;

    public:
        static void msleep(uint32_t msecs);

        static void sleep(const TimeSpan &interval);

        static ThreadId currentThreadId();

        static Thread *currentThread();

    private:
        explicit Thread(const String &name);

        void run();

        void run2(void *parameter);

#ifdef DEBUG

        void printDebugInfo(const String &info);

#endif

    private:
        struct Action {
            ThreadStart start;
            ParameterizedThreadStart pStart;
            ThreadExecution *cStart;

            Action();

            ~Action();
        };

        Action _action;

        thread _thread;

        std::atomic<bool> _running;

        String _name;

    private:
        static PList<Thread> _threads;
    };
}

#endif // Thread_h
