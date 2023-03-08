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
    class Timer : public Action {
    public:
        template<class Function, class... Args>
        explicit Timer(const String &name, int dueTime, int period,
                       Function &&f, Args &&... args) : Action(f, args...) {
            init(name, dueTime, period);
        }

        template<class Function, class... Args>
        explicit Timer(const String &name, int period, Function &&f, Args &&... args) : Action(f, args...) {
            init(name, Zero, period);
        }

        template<class Function, class... Args>
        explicit Timer(const String &name, TimeSpan dueTime, TimeSpan period,
                       Function &&f, Args &&... args) : Action(f, args...) {
            init(name, (int) dueTime.totalMilliseconds(), (int) period.totalMilliseconds());
        }

        template<class Function, class... Args>
        explicit Timer(const String &name, TimeSpan period,
                       Function &&f, Args &&... args) : Action(f, args...) {
            init(name, Zero, (int) period.totalMilliseconds());
        }

        Timer(const Timer &) = delete;

        ~Timer() override;

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
        void init(const String &name, int dueTime, int period);

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

    public:
        static const int Zero = 0;
        static const int Infinite = -1;

    private:
        String _name;

        int _dueTime;
        int _period;

#ifdef __EMSCRIPTEN__
        std::atomic<bool> _running;
#else
        bool _firstInvoke;
        uint32_t _start;
        std::atomic<bool> _loop;

        Thread *_thread;
#endif

    private:
#ifdef __EMSCRIPTEN__
        static Dictionary<int, DateTime> _timers;
#endif
    };
}

#endif // Timer_h
