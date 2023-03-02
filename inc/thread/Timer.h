//
//  Timer.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

#include <cassert>
#include "thread/Thread.h"
#include "data/TimeSpan.h"
#include "data/PList.h"
#include "data/Dictionary.h"

namespace Threading {
    typedef void (*TimerCallback)(void *);

    class TimerExecution : public IEquatable<TimerExecution> {
    public:
        TimerExecution() = default;

        virtual void execute() = 0;
    };

    template<class T>
    class ObjectTimerCallback : public TimerExecution, public ICloneable<TimerExecution> {
    public:
        typedef void (T::*Method)();

        ObjectTimerCallback() : ObjectTimerCallback(nullptr, nullptr) {
        }

        ObjectTimerCallback(T *instance, Method method) {
            _instance = instance;
            _method = method;
        };

        ObjectTimerCallback(const ObjectTimerCallback &callback) {
            this->operator=(callback);
        }

        void operator()() {
            if (_instance != nullptr && _method != nullptr)
                return (_instance->*_method)();
        };

        void execute() override {
            return operator()();
        };

        TimerExecution *clone() const override {
            return new ObjectTimerCallback<T>(_instance, _method);
        }

        bool equals(const TimerExecution &other) const override {
            auto callback = dynamic_cast<const ObjectTimerCallback *>(&other);
            assert(callback);
            return callback->operator==(*this);
        }

        ObjectTimerCallback &operator=(const ObjectTimerCallback &value) {
            if (this != &value) {
                _instance = value._instance;
                _method = value._method;
            }
            return *this;
        }

        bool operator==(const ObjectTimerCallback &value) const {
            return _instance == value._instance && _method == value._method;
        }

        bool operator!=(const ObjectTimerCallback &value) const {
            return !operator==(value);
        }

        T *instance() const {
            return _instance;
        }

    private:
        T *_instance;
        Method _method;
    };

    class Timer {
    public:
        // callback: A delegate representing a method to be executed.
        // state: An object containing information to be used by the callback method, or NULL.
        // dueTime: The amount of time to delay before the callback parameter invokes its methods.
        //          Specify Infinite to prevent the timer from starting.
        //          Specify Zero to start the timer immediately.
        // period: The time interval between invocations of callback, in milliseconds.
        //          Specify Infinite to disable periodic signaling.
        Timer(const String &name, TimerCallback callback, void *state, int dueTime, int period);

        Timer(const String &name, TimerCallback callback, void *state, int period);

        Timer(const String &name, TimerCallback callback, int dueTime, int period);

        Timer(const String &name, TimerCallback callback, int period);

        Timer(const String &name, TimerCallback callback, void *state, const TimeSpan &dueTime, const TimeSpan &period);

        Timer(const String &name, TimerCallback callback, void *state, const TimeSpan &period);

        Timer(const String &name, TimerCallback callback, const TimeSpan &dueTime, const TimeSpan &period);

        Timer(const String &name, TimerCallback callback, const TimeSpan &period);

        template<class T>
        Timer(const String &name, const ObjectTimerCallback<T> &callback,
              const TimeSpan &dueTime, const TimeSpan &period)
                : Timer(name, timerCallback, this, dueTime, period) {
            _execution = callback.clone();
        }

        template<class T>
        Timer(const String &name, const ObjectTimerCallback<T> &callback, const TimeSpan &period) :
                Timer(name, callback, TimeSpan::Zero, period) {
        }

        template<class T>
        Timer(const String &name, const ObjectTimerCallback<T> &callback, int period) :
                Timer(name, callback, TimeSpan::fromMilliseconds(period)) {
        }

        template<class T>
        Timer(const String &name, const ObjectTimerCallback<T> &callback, int dueTime, int period) :
                Timer(name, callback, TimeSpan::fromMilliseconds(dueTime), TimeSpan::fromMilliseconds(period)) {
        }

        ~Timer();

        void start();

        void stop(uint32_t delaySeconds = 10);

        void stop(const TimeSpan &delay);

        bool change(int dueTime, int period);

        bool change(const TimeSpan &dueTime, const TimeSpan &period);

        bool change(int period);

        bool change(const TimeSpan &period);

        bool running() const;

        const String &name() const;

    private:
        void fire();

#ifndef __EMSCRIPTEN__

        void threadProc();

        bool msleepWithBreak(uint32_t start, uint32_t msec) const;
#endif

#ifdef DEBUG

        void printDebugInfo(const String &info);

#endif

    private:
#ifdef __EMSCRIPTEN__
        static void threadProc(int value);
#endif

        static void timerCallback(void *parameter);

    public:
        static const int Zero = 0;
        static const int Infinite = -1;

    private:
        String _name;

        TimerCallback _callback;
        void *_state;

        int _dueTime;
        int _period;

        TimerExecution *_execution;

#ifdef __EMSCRIPTEN__
        bool _running;
#else
        bool _firstInvoke;
        uint32_t _start;
        bool _loop;

        Thread *_thread;
#endif

    private:
#ifdef __EMSCRIPTEN__
        static Dictionary<int, DateTime> _timers;
#endif
    };
}

#endif // Timer_h
