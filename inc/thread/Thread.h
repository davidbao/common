#ifndef THREAD_H
#define THREAD_H

#include <memory>
#include <stdlib.h>
#include "data/ValueType.h"
#include "data/PrimitiveInterface.h"
#include "data/Dictionary.h"
#include "data/DateTime.h"
#include "Mutex.h"
#include "Locker.h"

#ifdef WIN32
#include <thread>
#elif __EMSCRIPTEN__
#else
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#ifndef MSYS
#include <sys/syscall.h>
#endif
#endif	// WIN32

using namespace std;

namespace Common
{
    class ThreadHolder
    {
    public:
        class Value
        {
        public:
            Value();
            virtual ~Value();
        };
        
        void* owner;
        void* owner2;
        Value* value;
        bool autoDeleteValue;
        
        ThreadHolder(void* owner = nullptr, Value* value = nullptr, bool autoDeleteValue = true);
        ThreadHolder(void* owner, void* owner2, Value* value, bool autoDeleteValue);
        ~ThreadHolder();
    };
    
    typedef void (*action_callback)();
    typedef void (*action_callback2)(void*);
    typedef void (*action_callback3)(ThreadHolder*);

    class ThreadExecution : public IEquatable<ThreadExecution>
    {
    public:
        ThreadExecution() {}
        
        virtual void execute() = 0;
    };
    template <class T>
    class ThreadCallback : public ThreadExecution, public ICloneable<ThreadExecution>
    {
    public:
        typedef void (T::*Method)();
        typedef void (T::*HolderMethod)(ThreadHolder* holder);
        
        ThreadCallback() : ThreadCallback(nullptr, nullptr)
        {
        }
        ThreadCallback(T* instance, Method method)
        {
            _instance = instance;
            _method = method;
            _holderMethod = nullptr;
            _holder = nullptr;
        };
        ThreadCallback(T* instance, HolderMethod method, ThreadHolder* holder)
        {
            _instance = instance;
            _method = nullptr;
            _holderMethod = method;
            _holder = holder;
        };
        ThreadCallback(const ThreadCallback& callback)
        {
            this->operator=(callback);
        }
        
        void operator()()
        {
            if(_instance != nullptr)
            {
                if(_method != nullptr)
                    return (_instance->*_method)();
                else if(_holderMethod != nullptr)
                    return (_instance->*_holderMethod)(_holder);
            }
        }
        
        void execute() override
        {
            return operator()();
        }
        
        ThreadExecution* clone() const override
        {
            if(_method != nullptr)
                return new ThreadCallback<T>(_instance, _method);
            else if(_holderMethod != nullptr)
                return new ThreadCallback<T>(_instance, _holderMethod, _holder);
            else
                return new ThreadCallback<T>();
        }
        
        bool equals(const ThreadExecution& other) const override
        {
            const ThreadCallback* callback = dynamic_cast<const ThreadCallback*>(&other);
            assert(callback);
            return callback->operator==(*this);
        }
        
        void operator=(const ThreadCallback& value)
        {
            _instance = value._instance;
            _method = value._method;
            _holder = value._holder;
        }
        bool operator==(const ThreadCallback& value) const
        {
            return _instance == value._instance && _method == value._method &&
                    _holderMethod == value._holderMethod && _holder == value._holder;
        }
        bool operator!=(const ThreadCallback& value) const
        {
            return !operator==(value);
        }
        
        T* instance() const
        {
            return _instance;
        }
        
        bool canExecution() const
        {
            if(_instance != nullptr)
                return _method != nullptr || _holderMethod != nullptr;
            return false;
        }
        
    private:
        T* _instance;
        Method _method;
        
        HolderMethod _holderMethod;
        ThreadHolder* _holder;
    };
    
    class Timer;
    class Thread
    {
    public:
        /* Thread priorities */
        enum Priority
        {
            MinValue    = 0u,   /* lowest possible priority */
            Default     = 50u,  /* normal priority */
            MaxValue    = 100u  /* highest possible priority */
        };
        class Id
        {
        public:
            Id();
            Id(const Id& id);
            ~Id();
            
            void operator=(const Id& value);
            bool operator==(const Id& value) const;
            bool operator!=(const Id& value) const;
            
        private:
#ifdef WIN32
            Id(thread::id id);
#elif __EMSCRIPTEN__
            Id(uint32_t id);
#else
            Id(pthread_t id);
#endif
            
        private:
            friend Thread;
#ifdef WIN32
            thread::id _id;
#elif __EMSCRIPTEN__
            uint32_t _id;
#else
            pthread_t _id;
#endif
        };
        
        Thread(const String& name, bool autoDelete = false);
        ~Thread();
        
        bool isAlive() const;
        void setSleepPeriod(uint32_t msec);
        
        void start(action_callback startAction, action_callback stopAction = nullptr);
        void startProc(action_callback procAction, uint32_t msec = 1, action_callback startAction = nullptr, action_callback stopAction = nullptr);
        void startProc(action_callback procAction, TimeSpan interval = TimeSpan::fromMilliseconds(1), action_callback startAction = nullptr, action_callback stopAction = nullptr);
        
        void start(action_callback2 startAction, void* startParam, action_callback2 stopAction = nullptr, void* stopParam = nullptr);
        void startProc(action_callback2 procAction, void* procParam, uint32_t msec = 1, action_callback2 startAction = nullptr,
                       void* startParam = nullptr, action_callback2 stopAction = nullptr, void* stopParam = nullptr);
        void startProc(action_callback2 procAction, void* procParam, TimeSpan interval = TimeSpan::fromMilliseconds(1), action_callback2 startAction = nullptr,
                       void* startParam = nullptr, action_callback2 stopAction = nullptr, void* stopParam = nullptr);
        
        void start(action_callback3 startAction, ThreadHolder* startParam, action_callback3 stopAction = nullptr, ThreadHolder* stopParam = nullptr);
        void startProc(action_callback3 procAction, ThreadHolder* procParam, uint32_t msec = 1, action_callback3 startAction = nullptr,
                       ThreadHolder* startParam = nullptr, action_callback3 stopAction = nullptr, ThreadHolder* stopParam = nullptr);
        void startProc(action_callback3 procAction, ThreadHolder* procParam, TimeSpan interval = TimeSpan::fromMilliseconds(1), action_callback3 startAction = nullptr,
                       ThreadHolder* startParam = nullptr, action_callback3 stopAction = nullptr, ThreadHolder* stopParam = nullptr);
        
        template<class T>
        void start(const ThreadCallback<T>& startAction, const ThreadCallback<T>& stopAction = ThreadCallback<T>())
        {
            startProc(ThreadCallback<T>(), TimeSpan::fromMilliseconds(1), startAction, stopAction);
        }
        template<class T>
        void startProc(const ThreadCallback<T>& procAction, const TimeSpan& interval,
                       const ThreadCallback<T>& startAction = ThreadCallback<T>(), const ThreadCallback<T>& stopAction = ThreadCallback<T>())
        {
            if(_autoDelete)
            {
                if(_tcs.get() == nullptr)
                {
                    _tcs.reset(this);
                }
            }
            
            if(isAlive())
                return;
            
            _procExecution = procAction.canExecution() ? procAction.clone() : nullptr;
            _msec = (uint32_t)interval.totalMilliseconds();
            _startExecution = startAction.canExecution() ? startAction.clone() : nullptr;
            _stopExecution = stopAction.canExecution() ? stopAction.clone() : nullptr;
            
            start();
        }
        
        void stop(uint32_t delaySeconds = 10);
        void stop(const TimeSpan& delay);
        void stopmsec(uint32_t delaymsec = 10*1000);
        
        const String& name() const;
        
        void setPriority(int prio);
        
    public:
        static void msleep(uint32_t msecs);
        static void sleep(const TimeSpan& interval);
        
        static void beginInvoke(action_callback3 startAction, ThreadHolder* startParam, action_callback3 stopAction = nullptr, ThreadHolder* stopParam = nullptr);
        
        static Thread::Id currentThreadId();
        COMMON_ATTR_DEPRECATED("use Thread::operator==")
        static bool equals(Thread::Id id1, Thread::Id id2);
        
    private:
        friend void runAThread(void* parameter);
        
        void run();
        void runOnce();
        void start();
        
        bool msleepWithBreak(uint32_t start, uint32_t msec);
        
//        void loop();
//        void loop2();
//        void loop3();
        void loop(action_callback2 callback);
        
        void print_stacktrace();
        
        bool hasProc() const;

    private:
        static void* call_from_thread(void * parameter);
#ifdef __EMSCRIPTEN__
//        static Uint32 call_from_timer(Uint32 interval, void *param);
        static void call_from_timer(int value);
#endif
        
    private:
        friend Timer;
        
        action_callback _startAction;
        action_callback _procAction;
        action_callback _stopAction;
        
        action_callback2 _startAction2;
        action_callback2 _procAction2;
        action_callback2 _stopAction2;
        
        action_callback3 _startAction3;
        action_callback3 _procAction3;
        action_callback3 _stopAction3;
        
        ThreadExecution* _startExecution;
        ThreadExecution* _procExecution;
        ThreadExecution* _stopExecution;
        
        void* _procParam;
        void* _startParam;
        void* _stopParam;
        
        bool _break;
        uint32_t _msec;
        bool _autoDelete;
        
        static unique_ptr<Thread> _tcs;
        
#ifdef WIN32
        thread* _thread;
#elif __EMSCRIPTEN__
        static Dictionary<int, DateTime> _timers;
#else
        pthread_t _thread;
#endif
        bool _running;
        
        String _name;
        bool _alreadySetName;
        
        uint32_t _start;
        bool _firstInvoke;
    };
}

#endif // THREAD_H
