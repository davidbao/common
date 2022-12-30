#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdint.h>
#include "thread/Thread.h"
#include "data/TimeSpan.h"
#include "data/PList.h"
#include "data/Dictionary.h"

namespace Threading {
    typedef void (*timer_callback)(void *);

    class TimerExecution : public IEquatable<TimerExecution> {
    public:
        TimerExecution() {}

        virtual void execute() = 0;
    };

    template<class T>
    class TimerCallback : public TimerExecution, public ICloneable<TimerExecution> {
    public:
        typedef void (T::*Method)();

        TimerCallback() : TimerCallback(nullptr, nullptr) {
        }

        TimerCallback(T *instance, Method method) {
            _instance = instance;
            _method = method;
        };

        TimerCallback(const TimerCallback &callback) {
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
            return new TimerCallback<T>(_instance, _method);
        }

        bool equals(const TimerExecution &other) const override {
            const TimerCallback *callback = dynamic_cast<const TimerCallback *>(&other);
            assert(callback);
            return callback->operator==(*this);
        }

        void operator=(const TimerCallback &value) {
            _instance = value._instance;
            _method = value._method;
        }

        bool operator==(const TimerCallback &value) const {
            return _instance == value._instance && _method == value._method;
        }

        bool operator!=(const TimerCallback &value) const {
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
        Timer(const String &name, timer_callback callback, void *state, int dueTime, int period);

        Timer(const String &name, timer_callback callback, void *state, int period);

        Timer(const String &name, timer_callback callback, int period);

        Timer(const String &name, timer_callback callback, void *state, const TimeSpan &dueTime,
              const TimeSpan &period);

        Timer(const String &name, timer_callback callback, void *state, const TimeSpan &period);

        Timer(const String &name, timer_callback callback, const TimeSpan &period);

        template<class T>
        Timer(const String &name, const TimerCallback<T> &callback, const TimeSpan &dueTime, const TimeSpan &period)
                : Timer(name, timerCallback, this, dueTime, period) {
            _execution = callback.clone();
        }

        template<class T>
        Timer(const String &name, const TimerCallback<T> &callback, const TimeSpan &period) : Timer(name, callback,
                                                                                                    TimeSpan::Zero,
                                                                                                    period) {
        }

        template<class T>
        Timer(const String &name, const TimerCallback<T> &callback, int period) : Timer(name, callback,
                                                                                        TimeSpan::fromMilliseconds(
                                                                                                period)) {
        }

        template<class T>
        Timer(const String &name, const TimerCallback<T> &callback, int dueTime, int period) : Timer(name, callback,
                                                                                                     TimeSpan::fromMilliseconds(
                                                                                                             dueTime),
                                                                                                     TimeSpan::fromMilliseconds(
                                                                                                             period)) {
        }

        ~Timer();

        void start();

        void stop(uint32_t delaySeconds = 10);

        bool change(int dueTime, int period);

        bool change(TimeSpan dueTime, TimeSpan period);

        bool change(int period);

        bool change(TimeSpan period);

        bool running() const;

        void setPriority(int priority);

    private:
        void fire();

        const String &name() const;

#ifndef __EMSCRIPTEN__

        void timerProcInner();

#endif

    private:
#ifdef __EMSCRIPTEN__
        static void call_from_timer(int value);
#else

        static void timerProc(void *parameter);

#endif

        static void timerCallback(void *parameter);

    public:
        static const int Zero = 0;
        static const int Infinite = 0xFFFFFFFF;

    private:
#ifdef __EMSCRIPTEN__
        bool _running;
        String _name;
#else
        Thread *_thread;
#endif

        timer_callback _callback;
        void *_state;
        int _dueTime;
        int _period;

        bool _firstInvoke;
        uint32_t _start;

        TimerExecution *_execution;

    private:
#ifdef __EMSCRIPTEN__
        static Dictionary<int, DateTime> _timers;
#endif
    };
}
#endif // TIMER_H
