#ifndef TICKTIMEOUT_H
#define TICKTIMEOUT_H

#include "data/TimeSpan.h"
#include "thread/Thread.h"
#include "data/PrimitiveInterface.h"

using namespace Data;

namespace Threading {
    typedef bool (*delay_callback)(void *);

    class DelayExecution : public IEquatable<DelayExecution> {
    public:
        DelayExecution() {}

        virtual bool execute() = 0;
    };

    template<class T>
    class DelayCallback : public DelayExecution, public ICloneable<DelayExecution> {
    public:
        typedef bool (T::*Method)();

        DelayCallback() : DelayCallback(nullptr, nullptr) {
        }

        DelayCallback(T *instance, Method method) {
            _instance = instance;
            _method = method;
        };

        DelayCallback(const DelayCallback &callback) {
            this->operator=(callback);
        }

        bool operator()() {
            if (_instance != nullptr && _method != nullptr)
                return (_instance->*_method)();
            return false;
        };

        bool execute() override {
            return operator()();
        };

        DelayExecution *clone() const override {
            return new DelayCallback<T>(_instance, _method);
        }

        bool equals(const DelayExecution &other) const override {
            const DelayCallback *callback = dynamic_cast<const DelayCallback *>(&other);
            assert(callback);
            return callback->operator==(*this);
        }

        void operator=(const DelayCallback &value) {
            _instance = value._instance;
            _method = value._method;
        }

        bool operator==(const DelayCallback &value) const {
            return _instance == value._instance && _method == value._method;
        }

        bool operator!=(const DelayCallback &value) const {
            return !operator==(value);
        }

        T *instance() const {
            return _instance;
        }

    private:
        T *_instance;
        Method _method;
    };

    class TickTimeout {
    public:
        static bool isTimeout(uint32_t start, uint32_t end, uint32_t now);

        static bool isTimeout(uint32_t start, uint32_t timeout);

        static bool isTimeout(uint32_t start, const TimeSpan &timeout);

        static uint32_t getDeadTickCount(uint32_t timeout);

        static uint32_t getDeadTickCount(uint32_t start, uint32_t timeout);

        static uint32_t elapsed(uint32_t start, uint32_t end);

        static uint32_t elapsed(uint32_t start);

        static uint32_t getCurrentTickCount();

        static uint32_t getPrevTickCount(uint32_t start, uint32_t elapsed);

        static uint32_t getNextTickCount(uint32_t start, uint32_t elapsed);

        static bool
        sdelay(uint32_t sec = 10, delay_callback condition = nullptr, void *parameter = nullptr, uint32_t sleepms = 1);

        static bool msdelay(uint32_t msec = 3000, delay_callback condition = nullptr, void *parameter = nullptr,
                            uint32_t sleepms = 1);

        static bool delay(const TimeSpan &timeout, delay_callback condition = nullptr, void *parameter = nullptr,
                          uint32_t sleepms = 1);

        template<class T>
        static bool sdelay(uint32_t sec, const DelayCallback<T> &callback = DelayCallback<T>(), uint32_t sleepms = 1) {
            return delay(TimeSpan::fromSeconds(sec), callback, sleepms);
        }

        template<class T>
        static bool
        msdelay(uint32_t msec, const DelayCallback<T> &callback = DelayCallback<T>(), uint32_t sleepms = 1) {
            return delay(TimeSpan::fromMilliseconds(msec), callback, sleepms);
        }

        template<class T>
        static bool
        delay(const TimeSpan &timeout, const DelayCallback<T> &callback = DelayCallback<T>(), uint32_t sleepms = 1) {
            return delay(timeout, delayCallback, &callback, sleepms);
        }

    private:
        static bool delayCallback(void *parameter);

    private:
        static const uint32_t MaxTickCount = UINT32_MAX;

#if !WIN32

        /* Returns the number of 100ns ticks from unspecified time: this should be monotonic */
        static int64_t mono_100ns_ticks();

        static int64_t get_boot_time();

        /* Returns the number of milliseconds from boot time: this should be monotonic */
        static uint32_t mono_msec_ticks();

#endif
    };
}
#endif // TICKTIMEOUT_H
