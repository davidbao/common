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
#include "system/Action.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

#include <thread>
#include <atomic>

using namespace std;
using namespace System;

namespace Threading {
    typedef Action ThreadStart;

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

    class Thread : public Action {
    public:
        template<class Function, class... Args>
        explicit Thread(const String &name, Function &&f, Args &&... args) : Action(f, args...) {
            init(name);
        }

        Thread(const Thread &) = delete;

        ~Thread() override;

        bool isAlive() const;

        void start();

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

        static unsigned int concurrency();

    private:
        void init(const String &name);

        void run();

#ifdef DEBUG

        void printDebugInfo(const String &info);

#endif

    private:
        thread _thread;

        std::atomic<bool> _running;

        String _name;

    private:
        static PList<Thread> _threads;
    };
}

#endif // Thread_h
