//
//  Thread.cpp
//  common
//
//  Created by baowei on 2015/7/25.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "thread/Thread.h"
#include "diag/Trace.h"
#include "exception/Exception.h"

#ifdef WIN_OS

#include <Windows.h>

#else

#include <pthread.h>
#include <unistd.h>

#endif

using namespace Diag;
using namespace System;

namespace Threading {
    ThreadId::ThreadId() = default;

    ThreadId::ThreadId(const threadId_t &id) : _id(id) {
    }

    ThreadId::ThreadId(const ThreadId &id) {
        ThreadId::evaluates(id);
    }

    ThreadId::~ThreadId() = default;

    bool ThreadId::equals(const ThreadId &other) const {
        return _id == other._id;
    }

    void ThreadId::evaluates(const ThreadId &other) {
        _id = other._id;
    }

    int ThreadId::compareTo(const ThreadId &other) const {
        if (_id > other._id) return 1;
        if (_id < other._id) return -1;
        return 0;
    }

    ThreadId &ThreadId::operator=(const ThreadId &value) {
        if (this != &value) {
            _id = value._id;
        }
        return *this;
    }

    PList<Thread> Thread::_threads(false, 255);

    Thread::~Thread() {
#ifdef DEBUG
        printDebugInfo("Destroy a thread.");
#endif
        _threads.lock();
        _threads.remove(this);
        _threads.unlock();

        if (_thread.joinable()) {
            _thread.join();
        }
    }

    void Thread::init(const String &name) {
#ifdef DEBUG
        printDebugInfo("Create a thread.");
#endif
        _name = name;
        _running = false;

        _threads.lock();
        _threads.add(this);
        _threads.unlock();
    }

    void Thread::start() {
        if (!isAlive()) {
#ifdef DEBUG
            printDebugInfo("Start a thread.");
#endif

            _thread = thread(&Thread::run, this);

            _running = true;
        }
    }

    bool Thread::isAlive() const {
        return _running;
    }

    const String &Thread::name() const {
        return _name;
    }

    void Thread::msleep(uint32_t msecs) {
        if (msecs == 0)
            return;

#if WIN32
        Sleep((DWORD) msecs);
#elif __EMSCRIPTEN__
        // Can not invoke emscripten_sleep.
#else
        if (msecs < 10)
            msecs = 10;
        usleep(msecs * 1000);
#endif
    }

    void Thread::sleep(const TimeSpan &interval) {
        if (interval == TimeSpan::Zero)
            return;

        Thread::msleep((uint32_t) interval.totalMilliseconds());
    }

    ThreadId Thread::currentThreadId() {
        return ThreadId(this_thread::get_id());
    }

    Thread *Thread::currentThread() {
        Locker locker(&_threads);
        for (size_t i = 0; i < _threads.count(); ++i) {
            Thread *thread = _threads[i];
            if (thread->_thread.get_id() == this_thread::get_id()) {
                return thread;
            }
        }
        return nullptr;
    }

    ThreadPriority Thread::priority() {
        if (!isAlive()) {
            return Lowest;
        }

#ifdef WIN32
        ThreadPriority priority = Normal;
        int nPriority = GetThreadPriority(_thread.native_handle());
        switch (nPriority) {
            case THREAD_PRIORITY_HIGHEST:
                priority = Highest;
                break;
            case THREAD_PRIORITY_ABOVE_NORMAL:
                priority = AboveNormal;
                break;
            case THREAD_PRIORITY_NORMAL:
                priority = Normal;
                break;
            case THREAD_PRIORITY_BELOW_NORMAL:
                priority = BelowNormal;
                break;
            case THREAD_PRIORITY_LOWEST:
                priority = Lowest;
                break;
            default:
                break;
        }
        return priority;
#elif __linux__
        struct sched_param param = {0};
        int policy = SCHED_OTHER;
        if (pthread_getschedparam(_thread.native_handle(), &policy, &param) != 0) {
            Trace::error("Failed to get thread priority!");
            return Lowest;
        } else {
            ThreadPriority priority = Normal;
            switch (policy) {
                case SCHED_FIFO:
                    priority = Highest;
                    break;
                case SCHED_RR:
                    priority = AboveNormal;
                    break;
                case SCHED_OTHER:
                    priority = Normal;
                    break;
                case SCHED_BATCH:
                    priority = BelowNormal;
                    break;
                case SCHED_IDLE:
                    priority = Lowest;
                    break;
                default:
                    break;
            }
            return priority;
        }
#elif MAC_OS
        struct sched_param param = {0};
        int policy = SCHED_OTHER;
        if (pthread_getschedparam(_thread.native_handle(), &policy, &param) != 0) {
            Trace::error("Failed to get thread priority!");
            return Lowest;
        } else {
            if (policy == SCHED_FIFO) {
                return Highest;
            } else if (policy == SCHED_RR) {
                return AboveNormal;
            } else if (policy == SCHED_OTHER) {
                int min = sched_get_priority_min(policy);
                int max = sched_get_priority_max(policy);
                int step = (max - min) / 3;
                if (param.sched_priority > min + step * 2 && param.sched_priority <= max) {
                    return Normal;
                } else if (param.sched_priority > min + step && param.sched_priority <= min + step * 2) {
                    return BelowNormal;
                } else {
                    return Lowest;
                }
            }
            return Lowest;
        }
#else
        return Normal;
#endif
    }

    void Thread::setPriority(ThreadPriority priority) {
        if (!isAlive()) {
            return;
        }

#ifdef WIN32
        int nPriority = THREAD_PRIORITY_NORMAL;
        switch (priority) {
            case Highest:
                nPriority = THREAD_PRIORITY_HIGHEST;
                break;
            case AboveNormal:
                nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
                break;
            case Normal:
                nPriority = THREAD_PRIORITY_NORMAL;
                break;
            case BelowNormal:
                nPriority = THREAD_PRIORITY_BELOW_NORMAL;
                break;
            case Lowest:
                nPriority = THREAD_PRIORITY_LOWEST;
                break;
        }
        if (!SetThreadPriority(_thread.native_handle(), nPriority))
            Trace::error(String::format("Failed to set thread priority %d.", priority));
        else
            Debug::writeLine(String::format("Set thread'%s' priority'%d' successfully.", name().c_str(), priority));
#elif __linux__
        struct sched_param param = {0};

        // The only scheduling policy guaranteed to be supported
        // everywhere is this one.
        int policy = SCHED_OTHER;
        switch (priority) {
            case Highest:
                policy = SCHED_FIFO;
                break;
            case AboveNormal:
                policy = SCHED_RR;
                break;
            case Normal:
                policy = SCHED_OTHER;
                break;
            case BelowNormal:
                policy = SCHED_BATCH;
                break;
            case Lowest:
                policy = SCHED_IDLE;
                break;
        }
        int min = sched_get_priority_min(policy);
        int max = sched_get_priority_max(policy);
        int step = (max - min + 1) / 2;
        if (step > 0) {
            param.sched_priority = min + step / 2;
        }

        if (getuid() != 0 && (policy == SCHED_FIFO || policy == SCHED_RR)) {
            // Need root user.
            Trace::error("Need root user if set thread priority!");
        } else {
            int result = pthread_setschedparam(_thread.native_handle(), policy, &param);
            if (result != 0) {
                Trace::error(String::format("Failed to set thread priority'%d:%d', result: %d.",
                                            priority, priority, param.sched_priority, result));
            } else {
                Debug::writeLine(String::format("Set thread'%s' priority'%d:%d' successfully.",
                                                name().c_str(), priority, param.sched_priority));
            }
        }
#elif MAC_OS
        struct sched_param param = {0};

        // The only scheduling policy guaranteed to be supported
        // everywhere is this one.
        int policy = SCHED_OTHER;
        switch (priority) {
            case Highest:
                policy = SCHED_FIFO;
                break;
            case AboveNormal:
                policy = SCHED_RR;
                break;
            case Lowest:
            case BelowNormal:
            case Normal:
            default:
                policy = SCHED_OTHER;
                break;
        }

        int min = sched_get_priority_min(policy);
        int max = sched_get_priority_max(policy);
        if (policy == SCHED_OTHER) {
            int step = (max - min) / 3;
            if (priority == Lowest) {
                param.sched_priority = min + step / 2;
            } else if (priority == BelowNormal) {
                param.sched_priority = min + step + step / 2;
            } else if (priority == Normal) {
                param.sched_priority = min + 2 * step + step / 2;
            }
        } else {
            int step = (max - min) / 2;
            param.sched_priority = min + step;
        }

        if (pthread_setschedparam(_thread.native_handle(), policy, &param) != 0) {
            Trace::error(String::format("Failed to set thread priority'%d:%d'.",
                                        priority, priority, param.sched_priority));
        } else {
            Debug::writeLine(String::format("Set thread'%s' priority'%d:%d' successfully.",
                                            name().c_str(), priority, param.sched_priority));
        }
#else
        // Unsupported it.
#endif
    }

    void Thread::join() {
        _thread.join();
    }

    void Thread::detach() {
        _thread.detach();
    }

    void Thread::run() {
        execute();

        _running = false;
    }

    ThreadId Thread::id() const {
        return ThreadId(_thread.get_id());
    }

    bool Thread::joinable() const {
        return _thread.joinable();
    }

    unsigned int Thread::concurrency() {
        return thread::hardware_concurrency();
    }

#ifdef DEBUG

    void Thread::printDebugInfo(const String &info) {
#ifdef WIN32
        String name = String::UTF8toGBK(_name);
#else
        String name = _name;
#endif  // WIN32

        String str = String::format("%s name: '%s'", info.c_str(), name.c_str());
    }

#endif
}
