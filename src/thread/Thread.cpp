//#include <thread>
#if !__ANDROID__ && !WIN32 && !__EMSCRIPTEN__ && !MSYS

#include <execinfo.h>

#endif

#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "diag/Stopwatch.h"
#include "thread/TickTimeout.h"
#include "diag/Trace.h"
#include "exception/Exception.h"

#if WIN32
#include <windows.h>
#endif    // WIN32

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

using namespace Diag;
using namespace System;

namespace Threading {
    unique_ptr<Thread> Thread::_tcs;

#if WIN32
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    
#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)
    
    //
    // Usage: SetThreadName (-1, "MainThread");
    //
    void SetThreadName(DWORD dwThreadID, const char* threadName)
    {
//        THREADNAME_INFO info;
//        info.dwType = 0x1000;
//        info.szName = threadName;
//        info.dwThreadID = dwThreadID;
//        info.dwFlags = 0;
        
//        __try
//        {
//            RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
//        }
//        __except(EXCEPTION_EXECUTE_HANDLER)
//        {
//        }
    }
#endif    // WIN32

    void SetThreadName(const char *threadName) {
#if WIN32
        SetThreadName(GetCurrentThreadId(), threadName);
#else
//        pthread_setname_np(threadName);
#endif
    }

    bool isThreadDead(void *parameter) {
        return parameter != nullptr ? !((Thread *) parameter)->isAlive() : false;
    }

    void runAThread(void *parameter) {
        if (parameter != nullptr) {
            ((Thread *) parameter)->run();
        }
    }

    ThreadHolder::Value::Value() {
    }

    ThreadHolder::Value::~Value() {
    }

    ThreadHolder::ThreadHolder(void *owner, Value *value, bool autoDeleteValue) : ThreadHolder(owner, nullptr, value,
                                                                                               autoDeleteValue) {
    }

    ThreadHolder::ThreadHolder(void *owner, void *owner2, Value *value, bool autoDeleteValue) {
        this->owner = owner;
        this->owner2 = owner2;
        this->value = value;
        this->autoDeleteValue = autoDeleteValue;
    }

    ThreadHolder::~ThreadHolder() {
        if (autoDeleteValue && value != nullptr) {
            delete (value);
        }
    }

    Thread::Id::Id() {
#ifdef WIN32
#elif __EMSCRIPTEN__
        _id = 0;
#else
        _id = 0;
#endif
    }

#ifdef WIN32
    Thread::Id::Id(thread::id id) : _id(id)
    {
    }
#elif __EMSCRIPTEN__
    Thread::Id::Id(uint32_t id) : _id(id)
    {
    }
#else

    Thread::Id::Id(pthread_t id) : _id(id) {
    }

#endif

    Thread::Id::Id(const Id &id) {
        this->operator=(id);
    }

    Thread::Id::~Id() {
    }

    void Thread::Id::operator=(const Id &value) {
        _id = value._id;
    }

    bool Thread::Id::operator==(const Id &value) const {
#ifdef WIN32
        return _id == value._id;
#elif __EMSCRIPTEN__
        return true;
#else
        return pthread_equal(_id, value._id) != 0;
#endif
    }

    bool Thread::Id::operator!=(const Id &value) const {
        return !operator==(value);
    }

#ifdef __EMSCRIPTEN__
    Dictionary<int, DateTime> Thread::_timers;
#else
#endif

    Thread::Thread(const String &name, bool autoDelete) : _name(name), _autoDelete(autoDelete) {
        _startAction = nullptr;
        _procAction = nullptr;
        _stopAction = nullptr;

        _startAction2 = nullptr;
        _procAction2 = nullptr;
        _stopAction2 = nullptr;

        _startAction3 = nullptr;
        _procAction3 = nullptr;
        _stopAction3 = nullptr;

        _startExecution = nullptr;
        _procExecution = nullptr;
        _stopExecution = nullptr;

        _procParam = nullptr;
        _startParam = nullptr;
        _stopParam = nullptr;

        _break = false;
        _msec = 1;

#ifdef WIN32
        _thread = nullptr;
#elif __EMSCRIPTEN__
        _timers.add((int)this, DateTime::now());
#else
        _thread = 0;
#endif
        _running = false;

        _alreadySetName = false;

        _start = 0;
        _firstInvoke = true;

#ifdef DEBUG
#ifdef WIN32
        String temp = String::UTF8toGBK(_name);
#else
        String temp = _name;
#endif
        Debug::writeFormatLine("Create a thread. name: '%s'", temp.c_str());
#endif
    }

    Thread::~Thread() {
#ifdef __EMSCRIPTEN__
        _timers.remove((int)this);
#endif

        if (_startExecution != nullptr) {
            delete _startExecution;
            _startExecution = nullptr;
        }
        if (_procExecution != nullptr) {
            delete _procExecution;
            _procExecution = nullptr;
        }
        if (_stopExecution != nullptr) {
            delete _stopExecution;
            _stopExecution = nullptr;
        }

        Debug::writeFormatLine("Destroy a thread. name: '%s'", _name.c_str());
    }

    void Thread::start(action_callback startAction, action_callback stopAction) {
        startProc(nullptr, 1, startAction, stopAction);
    }

    void Thread::startProc(action_callback procAction, uint32_t msec, action_callback startAction,
                           action_callback stopAction) {
        if (_autoDelete) {
            if (_tcs.get() == nullptr) {
                _tcs.reset(this);
            }
        }

        if (isAlive())
            return;

        _procAction = procAction;
        _msec = msec;
        _startAction = startAction;
        _stopAction = stopAction;

        start();
    }

    void Thread::startProc(action_callback procAction, TimeSpan interval, action_callback startAction,
                           action_callback stopAction) {
        startProc(procAction, (uint32_t) interval.totalMilliseconds(), startAction, stopAction);
    }

    void Thread::start(action_callback2 startAction, void *startParam, action_callback2 stopAction, void *stopParam) {
        startProc(nullptr, nullptr, 1, startAction, startParam, stopAction, stopParam);
    }

    void Thread::startProc(action_callback2 procAction, void *procParam, uint32_t msec, action_callback2 startAction,
                           void *startParam, action_callback2 stopAction, void *stopParam) {
        if (_autoDelete) {
            if (_tcs.get() == nullptr) {
                _tcs.reset(this);
            }
        }

        if (isAlive())
            return;

        _procAction2 = procAction;
        _procParam = procParam;
        _msec = msec;
        _startAction2 = startAction;
        _startParam = startParam;
        _stopAction2 = stopAction;
        _stopParam = stopParam;

        start();
    }

    void
    Thread::startProc(action_callback2 procAction, void *procParam, TimeSpan interval, action_callback2 startAction,
                      void *startParam, action_callback2 stopAction, void *stopParam) {
        startProc(procAction, procParam, (uint32_t) interval.totalMilliseconds(), startAction, startParam, stopAction,
                  stopParam);
    }

    void Thread::start(action_callback3 startAction, ThreadHolder *startParam, action_callback3 stopAction,
                       ThreadHolder *stopParam) {
        startProc(nullptr, nullptr, 1, startAction, startParam, stopAction, stopParam);
    }

    void
    Thread::startProc(action_callback3 procAction, ThreadHolder *procParam, uint32_t msec, action_callback3 startAction,
                      ThreadHolder *startParam, action_callback3 stopAction, ThreadHolder *stopParam) {
        if (_autoDelete) {
            if (_tcs.get() == nullptr) {
                _tcs.reset(this);
            }
        }

        if (isAlive())
            return;

        _procAction3 = procAction;
        _procParam = procParam;
        _msec = msec;
        _startAction3 = startAction;
        _startParam = startParam;
        _stopAction3 = stopAction;
        _stopParam = stopParam;

        start();
    }

    void Thread::startProc(action_callback3 procAction, ThreadHolder *procParam, TimeSpan interval,
                           action_callback3 startAction,
                           ThreadHolder *startParam, action_callback3 stopAction, ThreadHolder *stopParam) {
        startProc(procAction, procParam, (uint32_t) interval.totalMilliseconds(), startAction, startParam, stopAction,
                  stopParam);
    }

    void Thread::beginInvoke(action_callback3 startAction, ThreadHolder *startParam, action_callback3 stopAction,
                             ThreadHolder *stopParam) {
        Thread *thread = new Thread("AsyncInvoke", true);
        thread->start(startAction, startParam, stopAction, stopParam);
    }

    void Thread::stopmsec(uint32_t delaymsec) {
        if (isAlive()) {
            _break = true;

#ifdef DEBUG
#ifdef WIN32
            String name = String::UTF8toGBK(_name);
#else
            String name = _name;
#endif
            String msg = String::convert("Thread::stopmsec, name: %s", name.c_str());
            Stopwatch sw(msg, 1000);
#endif

#ifdef WIN32
            thread::id this_id = this_thread::get_id();
            thread::id thread_id = _thread->get_id();
            if (this_id != thread_id)
            {
                TickTimeout::msdelay(delaymsec, isThreadDead, this);

                if (isAlive())
                {
                    _thread->detach();
                }
            }
            _thread = nullptr;
#elif __EMSCRIPTEN__
#else
            pthread_t this_id = pthread_self();
            if (!pthread_equal(this_id, _thread)) {
                TickTimeout::msdelay(delaymsec, isThreadDead, this);

                if (isAlive()) {
#ifndef __ANDROID__
                    pthread_cancel(_thread);
//                    pthread_detach(_thread);
#endif
                }
            }
            _thread = 0;
#endif
            _running = false;
        }
    }

    void Thread::stop(uint32_t delaySeconds) {
        Debug::writeFormatLine("Stop a thread. name: '%s'", _name.c_str());
        stopmsec(delaySeconds * 1000);
    }

    void Thread::stop(const TimeSpan &delay) {
        stop((uint32_t) delay.totalSeconds());
    }

    void Thread::start() {
        if (!_running) {
#ifdef DEBUG
#ifdef WIN32
            String name = String::UTF8toGBK(_name);
#else
            String name = _name;
#endif
            Debug::writeFormatLine("Start a thread, name: '%s'", name.c_str());
#endif

            _alreadySetName = false;
            _start = 0;
            _firstInvoke = true;
            _break = false;
            _running = true;

#ifdef WIN32
            _thread = new thread(runAThread, this);
#elif __EMSCRIPTEN__
            glutTimerFunc(0, call_from_timer, (int)this);
#else
            pthread_create(&_thread, nullptr, call_from_thread, this);
#endif

#if TARGET_OS_MAC
            setPriority(80);
#endif
        }
    }

    void *Thread::call_from_thread(void *parameter) {
        if (parameter != nullptr) {
            ((Thread *) parameter)->run();
        }
        return nullptr;
    }

#ifdef __EMSCRIPTEN__
    void Thread::call_from_timer(int value)
    {
        if(!_timers.contains(value))
        {
            Debug::writeFormatLine("removed thread's invoke, value: 0x%X", value);
            return;
        }
        
        Thread* thread = (Thread*)value;
//        Debug::writeFormatLine("call_from_timer. interval: %d, thread: %s", interval, thread != nullptr ? "is not null" : "is null");
        if(thread != nullptr && thread->isAlive())
        {
//            Debug::writeFormatLine("The thread is running. name: %s, hasProc: %s", thread->name().c_str(), thread->hasProc() ? "yes" : "no");
//            Stopwatch sw(String::convert("thread->runOnce, thread name: %s", thread->name().c_str()), 100);
            thread->runOnce();
//            sw.stop();
            
            if(thread->hasProc())
            {
                uint32_t msec = thread->_msec < 10 ? 10 : thread->_msec;
                glutTimerFunc(msec, call_from_timer, value);
            }
        }
    }
#endif

    void Thread::setPriority(int prio) {
        if (!_running)
            return;
        if (!(prio >= Priority::MinValue && prio <= Priority::MaxValue))
            return;

#ifdef WIN32
        throw NotImplementedException("Thread::setPriority");
#elif !__EMSCRIPTEN__
        // We map our priority values to pthreads scheduling params as
        // follows:
        //      0..20  to SCHED_IDLE
        //     21..40  to SCHED_BATCH
        //     41..60  to SCHED_OTHER
        //     61..80  to SCHED_RR
        //     81..100 to SCHED_FIFO
        //
        // For the last two, we can also use the additional priority
        // parameter which must be in 1..99 range under Linux (TODO:
        // what should be used for the other systems?).
        struct sched_param sparam = {0};

        // The only scheduling policy guaranteed to be supported
        // everywhere is this one.
        int policy = SCHED_OTHER;
#ifdef SCHED_IDLE
        if ( prio <= 20 )
            policy = SCHED_IDLE;
#endif
#ifdef SCHED_BATCH
        if ( 20 < prio && prio <= 40 )
            policy = SCHED_BATCH;
#endif
#ifdef SCHED_RR
        if (60 < prio && prio <= 80)
            policy = SCHED_RR;
#endif
#ifdef SCHED_FIFO
        if (80 > prio)
            policy = SCHED_FIFO;
#endif

        // This test is not redundant as it takes care of the systems
        // where neither SCHED_RR nor SCHED_FIFO are defined.
        if (prio > 60 && policy != SCHED_OTHER) {
            // There is no good way to map our 20 possible priorities
            // (61..80 or 81..100) to the 99 pthread priorities, so we
            // do the best that we can and ensure that the extremes of
            // our range are mapped to the pthread extremes and all the
            // rest fall in between.

            // This gets us to 1..96 range.
            sparam.sched_priority = ((prio - 61) % 20) * 5 + 1;

            // And we artificially increase our highest priority to the
            // highest pthread one.
            if (sparam.sched_priority == 96)
                sparam.sched_priority = 99;
        }

        if (pthread_setschedparam(_thread, policy, &sparam) != 0)
            Trace::error(String::format("Failed to set thread priority %d.", prio));
        else
            Debug::writeLine(String::format("Set thread'%s' priority'%d' successfully.", name().c_str(), prio));
#endif
    }

    bool Thread::isAlive() const {
        return _running;
    }

    void Thread::setSleepPeriod(uint32_t msec) {
        uint32_t temp = _msec;
        _msec = msec;
        Thread::msleep(temp);
    }

    const String &Thread::name() const {
        return _name;
    }

    bool Thread::hasProc() const {
        return _procAction != nullptr || _procAction2 != nullptr || _procAction3 != nullptr ||
               _procExecution != nullptr;
    }

    void Thread::run() {
        try {
            if (!_alreadySetName && !_name.isNullOrEmpty()) {
                _alreadySetName = true;
                SetThreadName(_name);
            }

            if (_startAction != nullptr) {
                _startAction();
            } else if (_startAction2 != nullptr) {
                _startAction2(_startParam);
            } else if (_startAction3 != nullptr) {
                _startAction3((ThreadHolder *) _startParam);
            } else if (_startExecution != nullptr) {
                _startExecution->execute();
            }

            if (_procAction != nullptr) {
                action_callback2 callback = [](void *parameter) {
                    Thread *thread = static_cast<Thread *>(parameter);
                    if (thread->_procAction != nullptr)
                        thread->_procAction();
                };
                loop(callback);
            } else if (_procAction2 != nullptr) {
                action_callback2 callback = [](void *parameter) {
                    Thread *thread = static_cast<Thread *>(parameter);
                    if (thread->_procAction2 != nullptr)
                        thread->_procAction2((ThreadHolder *) thread->_procParam);
                };
                loop(callback);
            } else if (_procAction3 != nullptr) {
                action_callback2 callback = [](void *parameter) {
                    Thread *thread = static_cast<Thread *>(parameter);
                    if (thread->_procAction3 != nullptr)
                        thread->_procAction3((ThreadHolder *) thread->_procParam);
                };
                loop(callback);
            } else if (_procExecution != nullptr) {
                action_callback2 callback = [](void *parameter) {
                    Thread *thread = static_cast<Thread *>(parameter);
                    if (thread->_procExecution != nullptr)
                        thread->_procExecution->execute();
                };
                loop(callback);
            }

            if (_stopAction != nullptr) {
                _stopAction();
            } else if (_stopAction2 != nullptr) {
                _stopAction2(_stopParam);
            } else if (_stopAction3 != nullptr) {
                _stopAction3((ThreadHolder *) _stopParam);
            } else if (_stopExecution != nullptr) {
                _stopExecution->execute();
            }
        }
        catch (...) {
//            String str = String::convert("Generate error'%s' in thread'%s' running", e.what(), _name.c_str());
//            Trace::writeLine(str, Trace::Error);
//            str = String::convert("The error is in '%s' on line '%d'", __FILE__, __LINE__);
//            Trace::writeLine(str, Trace::Error);
//            print_stacktrace();
        }

        _running = false;
        Debug::writeFormatLine("The thread is exiting. name: '%s'", _name.c_str());
        //Debug::writeFormatLine("_break: %s", _break ? "true" : "false");

//#ifdef WIN32
//		if (_thread != nullptr)
//		{
//			thread::id this_id = this_thread::get_id();
//			thread::id thread_id = _thread->get_id();
//			if (this_id != thread_id)
//			{
//				_thread->detach();
//			}
//			_thread = nullptr;
//		}
//#elif __EMSCRIPTEN__
//#else
//		pthread_t this_id = pthread_self();
//		if (!pthread_equal(this_id, _thread))
//		{
//			pthread_detach(pthread_equal);
//		}
//		_thread = 0;
//#endif
    }

    void Thread::runOnce() {
        try {
            if (_startAction != nullptr) {
                _startAction();
            } else if (_startAction2 != nullptr) {
                _startAction2(_startParam);
            } else if (_startAction3 != nullptr) {
                _startAction3((ThreadHolder *) _startParam);
            } else if (_startExecution != nullptr) {
                _startExecution->execute();
            }

            if (_procAction != nullptr) {
                _procAction();
            } else if (_procAction2 != nullptr) {
                _procAction2(_procParam);
            } else if (_procAction3 != nullptr) {
                _procAction3((ThreadHolder *) _procParam);
            } else if (_procExecution != nullptr) {
                _procExecution->execute();
            }

            if (_stopAction != nullptr) {
                _stopAction();
            } else if (_stopAction2 != nullptr) {
                _stopAction2(_stopParam);
            } else if (_stopAction3 != nullptr) {
                _stopAction3((ThreadHolder *) _stopParam);
            } else if (_stopExecution != nullptr) {
                _stopExecution->execute();
            }
        }
        catch (...) {
//            String str = String::convert("Generate error'%s' in thread'%s' running", e.what(), _name.c_str());
//            Trace::writeLine(str, Trace::Error);
//            str = String::convert("The error is in '%s' on line '%d'", __FILE__, __LINE__);
//            Trace::writeLine(str, Trace::Error);
//            print_stacktrace();
        }
    }

    void Thread::print_stacktrace() {
#if !__ANDROID__ && !WIN32 && !__EMSCRIPTEN__ && !MSYS
        int size = 16;
        void *array[16];
        int stack_num = backtrace(array, size);
        char **stacktrace = backtrace_symbols(array, stack_num);
        for (int i = 0; i < stack_num; ++i) {
            Trace::writeLine(stacktrace[i], Trace::Error);
        }
        free(stacktrace);
#endif
    }

//    void Thread::loop()
//    {
//        while (!_break)
//        {
//            if(_firstInvoke)
//            {
//                _firstInvoke = false;
//                _start = TickTimeout::getCurrentTickCount();
//            }
//            else
//            {
//                _start = TickTimeout::getNextTickCount(_start, _msec);
//            }
//
//            _procAction();
//
//            msleepWithBreak(_start, _msec);
//        }
//    }
//    void Thread::loop2()
//    {
//        while (!_break)
//        {
//            if(_firstInvoke)
//            {
//                _firstInvoke = false;
//                _start = TickTimeout::getCurrentTickCount();
//            }
//            else
//            {
//                _start = TickTimeout::getNextTickCount(_start, _msec);
//            }
//
//            _procAction2(_procParam);
//            msleepWithBreak(_start, _msec);
//        }
//    }
//    void Thread::loop3()
//    {
//        while (!_break)
//        {
//            if(_firstInvoke)
//            {
//                _firstInvoke = false;
//                _start = TickTimeout::getCurrentTickCount();
//            }
//            else
//            {
//                _start = TickTimeout::getNextTickCount(_start, _msec);
//            }
//
//            _procAction3((ThreadHolder*)_procParam);
//            msleepWithBreak(_start, _msec);
//        }
//    }
    void Thread::loop(action_callback2 callback) {
        while (!_break) {
            if (_firstInvoke) {
                _firstInvoke = false;
                _start = TickTimeout::getCurrentTickCount();
            } else {
                _start = TickTimeout::getNextTickCount(_start, _msec);
            }

            callback(this);

            msleepWithBreak(_start, _msec);
        }
    }

    bool Thread::msleepWithBreak(uint32_t start, uint32_t msec) {
        const uint32_t unitmsec = 10;        // 10 ms
        if (msec <= unitmsec) {
            if (msec != 0) {
                Thread::msleep(msec);
                return true;
            }
            return false;
        } else {
            uint32_t end = TickTimeout::getDeadTickCount(start, msec);
            while (!_break) {
                if (TickTimeout::isTimeout(start, end)) {
                    break;
                }
                Thread::msleep(unitmsec);
            }
            return !_break;
        }
    }

    void Thread::msleep(uint32_t msecs) {
        if (msecs == 0)
            return;

//#ifdef DEBUG
//        Stopwatch sw(String::convert("Thread::msleep, msecs: %d", msecs), 3000);
////        DateTime start = DateTime::now();
//#endif

#if WIN32
        Sleep((DWORD)msecs);
#elif __EMSCRIPTEN__
        // Can not invoke emscripten_sleep.
//        Debug::writeFormatLine("Thread::msleep, msecs: %d", msecs);
//        if(msecs < 10)
//            msecs = 10;
//        emscripten_sleep(msecs);
#else
//        std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
        if (msecs < 10)
            msecs = 10;
        usleep(msecs * 1000);

//#ifdef DEBUG
//        DateTime end = DateTime::now();
//        if(msecs < 1000 && (end - start).totalMilliseconds() > 1000)
//        {
//            Debug::writeLine(String::convert("Thread::msleep timeout!, start: '%s', end: '%s'", start.toString().c_str(), end.toString().c_str()), Trace::Error);
//        }
//#endif

//        struct timeval delay;
//        delay.tv_sec = msecs / 1000;
//        delay.tv_usec = 1000 * (msecs % 1000);
//        select(0, nullptr, nullptr, nullptr, &delay);
//
//        int ret;
//        struct timespec req, rem;
//
//        req.tv_sec = msecs / 1000;
//        req.tv_nsec = (msecs % 1000) * 1000000;
//
//        do {
//            memset (&rem, 0, sizeof (rem));
//            ret = nanosleep (&req, &rem);
//        } while (ret != 0);
#endif
    }

    void Thread::sleep(const TimeSpan &interval) {
        if (interval == TimeSpan::Zero)
            return;

        Thread::msleep((uint32_t) interval.totalMilliseconds());
    }

    Thread::Id Thread::currentThreadId() {
#ifdef WIN32
        //        return GetCurrentThreadId();
                thread::id this_id = this_thread::get_id();
                return Thread::Id(this_id);
#elif __EMSCRIPTEN__
        return Thread::Id(0);
#else
        return Thread::Id(pthread_self());
#endif
    }

    bool Thread::equals(Thread::Id id1, Thread::Id id2) {
        return id1 == id2;
    }
}
