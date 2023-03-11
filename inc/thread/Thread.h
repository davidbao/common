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
#include "data/DataInterface.h"
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
        Highest = 4
    };

    class Thread {
    public:
        Thread() noexcept;

        template<class Function, class... Args>
        explicit Thread(const String &name, Function &&f, Args &&... args) : _action(f, args...), _running(false) {
            init(name);
        }

        Thread(const Thread &) = delete;

        Thread(Thread &&other) noexcept;

        ~Thread();

        Thread &operator=(const Thread &) = delete;

        Thread &operator=(Thread &&other) noexcept;

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

        static void sleep(uint32_t msecs);

        static bool delay(const TimeSpan &interval, const Func<bool> &condition);

        static bool delay(uint32_t msecs, const Func<bool> &condition);

        static ThreadId currentThreadId();

        static Thread *currentThread();

        static unsigned int concurrency();

    private:
        void init(const String &name);

        void move(Thread &other);

        void run();

#ifdef DEBUG

        void printDebugInfo(const String &info);

#endif

    private:
        thread _thread;

        Action _action;

        std::atomic<bool> _running;

        String _name;

    private:
        static PList<Thread> _threads;
    };
}

#endif // Thread_h
