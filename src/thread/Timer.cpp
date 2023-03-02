#include "thread/Timer.h"
#include "thread/TickTimeout.h"
#include "diag/Trace.h"
#include "exception/Exception.h"

#ifdef __EMSCRIPTEN__

#include <GL/gl.h>
#include <GL/glut.h>

#endif

using namespace Diag;

namespace Threading {
#ifdef __EMSCRIPTEN__
    Dictionary<int, DateTime> Timer::_timers;
#endif

    Timer::Timer(const String &name, TimerCallback callback, void *state, int dueTime, int period) {
        _callback = callback;
        _state = state;
        _dueTime = dueTime;
        _period = period;
        _execution = nullptr;

        String tName = name;
        if (tName.isNullOrEmpty()) {
            static int no = 0;
            no++;
            tName = String::convert("TimerProc%d", no);
        }
        _name = tName;

#ifdef __EMSCRIPTEN__
        _running = false;
        _timers.add((int) this, DateTime::now());
#else
        _firstInvoke = true;
        _start = 0;
        _loop = false;

        _thread = new Thread(tName, ObjectThreadStart<Timer>(this, &Timer::threadProc));
#endif

#ifdef DEBUG
        printDebugInfo("Create a timer.");
#endif // DEBUG

        start();
    }

    Timer::Timer(const String &name, TimerCallback callback, void *state, int period) :
            Timer(name, callback, state, Zero, period) {
    }

    Timer::Timer(const String &name, TimerCallback callback, int dueTime, int period) :
            Timer(name, callback, this, dueTime, period) {
    }

    Timer::Timer(const String &name, TimerCallback callback, int period) : Timer(name, callback, this, period) {
    }

    Timer::Timer(const String &name, TimerCallback callback, void *state, const TimeSpan &dueTime,
                 const TimeSpan &period)
            : Timer(name, callback, state, (int) dueTime.totalMilliseconds(), (int) period.totalMilliseconds()) {
    }

    Timer::Timer(const String &name, TimerCallback callback, void *state, const TimeSpan &period) :
            Timer(name, callback, state, TimeSpan::Zero, period) {
    }

    Timer::Timer(const String &name, TimerCallback callback, const TimeSpan &dueTime, const TimeSpan &period) :
            Timer(name, callback, this, dueTime, period) {
    }

    Timer::Timer(const String &name, TimerCallback callback, const TimeSpan &period) :
            Timer(name, callback, this, period) {
    }

    Timer::~Timer() {
#ifdef __EMSCRIPTEN__
        _timers.remove((int) this);
#endif

        stop();

#ifdef DEBUG
        printDebugInfo("Destroy a timer.");
#endif // DEBUG

#ifndef __EMSCRIPTEN__
        delete _thread;
        _thread = nullptr;
#endif

        if (_execution != nullptr) {
            delete _execution;
            _execution = nullptr;
        }
    }

    void Timer::start() {
        if (!running()) {
#ifdef DEBUG
            printDebugInfo("Start a timer.");
#endif // DEBUG

#ifdef __EMSCRIPTEN__
            _running = true;
            glutTimerFunc(_dueTime == Zero ? 0 : _dueTime, threadProc, (int) this);
#else
            _firstInvoke = true;
            _loop = true;

            _thread->start();
#endif
        }
    }

    void Timer::stop(uint32_t delaySeconds) {
#ifdef DEBUG
        printDebugInfo("Stop a timer.");
#endif // DEBUG

#ifdef __EMSCRIPTEN__
        _running = false;
#else
        _loop = false;

        if (_thread->id() != Thread::currentThreadId()) {
            auto isThreadDead = [](void *parameter) {
                return parameter != nullptr && !((Thread *) parameter)->isAlive();
            };
            TickTimeout::sdelay(delaySeconds, isThreadDead, _thread);
            if (_thread->joinable()) {
                _thread->join();
            } else if (_thread->isAlive()) {
                _thread->detach();
            }
        } else {
        }
#endif
    }

    void Timer::stop(const TimeSpan &delay) {
        stop((uint32_t) delay.totalSeconds());
    }

    const String &Timer::name() const {
        return _name;
    }

    bool Timer::running() const {
#ifdef __EMSCRIPTEN__
        return _running;
#else
        return _thread->isAlive();
#endif
    }

    bool Timer::change(int dueTime, int period) {
        if (period != _period) {
            _dueTime = dueTime;
            _period = period;

#ifdef __EMSCRIPTEN__
            if (running()) {
                glutTimerFunc(_dueTime == Zero ? 0 : _dueTime, threadProc, (int) this);
            }
#else
            _firstInvoke = true;
#endif
            return true;
        }
        return false;
    }

    bool Timer::change(const TimeSpan &dueTime, const TimeSpan &period) {
        return change((int) dueTime.totalMilliseconds(), (int) period.totalMilliseconds());
    }

    bool Timer::change(int period) {
        return change(_dueTime, period);
    }

    bool Timer::change(const TimeSpan &period) {
        return change((int) period.totalMilliseconds());
    }

    void Timer::timerCallback(void *parameter) {
        auto timer = static_cast<Timer *>(parameter);
        assert(timer);
        if (timer->_execution != nullptr)
            timer->_execution->execute();
    }

    void Timer::fire() {
        if (_callback != nullptr)
            _callback(_state);
    }

#ifdef __EMSCRIPTEN__

    void Timer::threadProc(int value) {
        if (!_timers.contains(value)) {
            Debug::writeFormatLine("removed timer's invoke, value: 0x%X", value);
            return;
        }

        auto timer = (Timer *) value;
        if (timer != nullptr && timer->running()) {
//            Stopwatch sw(String::convert("timer->fire, period: %d", timer->_period), 100);
            timer->fire();
//            sw.stop();

            if (timer->_period > 0) {
                uint32_t msec = timer->_period < 10 ? 10 : timer->_period;
                glutTimerFunc(msec, threadProc, value);
            }
        }
    }

#else

    void Timer::threadProc() {
        while (_loop) {
            if (_firstInvoke) {
                _firstInvoke = false;
                _start = TickTimeout::getCurrentTickCount();

                if (_dueTime == Zero) {
                    fire();
                } else if (_dueTime == Infinite) {
                } else {
                    if (msleepWithBreak(_start, _dueTime)) {
                        fire();
                        _start = TickTimeout::getCurrentTickCount();
                    }
                }
            } else {
                _start = TickTimeout::getNextTickCount(_start, _period);
                if (_dueTime != Infinite) {
                    fire();
                }
            }

            msleepWithBreak(_start, _period);
        }
    }

    bool Timer::msleepWithBreak(uint32_t start, uint32_t msec) const {
        const uint32_t unitmsec = 10;        // 10 ms
        if (msec <= unitmsec) {
            if (msec != 0) {
                Thread::msleep(msec);
                return true;
            }
            return false;
        } else {
            uint32_t end = TickTimeout::getDeadTickCount(start, msec);
            while (_loop) {
                if (TickTimeout::isTimeout(start, end)) {
                    break;
                }
                Thread::msleep(unitmsec);
            }
            return _loop;
        }
    }

#endif

#ifdef DEBUG

    void Timer::printDebugInfo(const String &info) {
#ifdef WIN32
        String name = String::UTF8toGBK(_name);
#else
        String name = _name;
#endif  // WIN32

        String str = String::format("%s name: '%s', period: %d ms", info.c_str(), name.c_str(), _period);
#ifdef __EMSCRIPTEN__
        str.append(String::format(", value: 0x%X", (int) this));
#else
#endif
        Debug::writeFormatLine(str);
    }

#endif
}
