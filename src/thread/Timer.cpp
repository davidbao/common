#include "thread/Timer.h"
#include "data/Convert.h"
#include "thread/TickTimeout.h"
#include "diag/Trace.h"
#include "exception/Exception.h"
#include "diag/Stopwatch.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glut.h>
#endif

namespace Common
{
#ifdef __EMSCRIPTEN__
    Dictionary<int, DateTime> Timer::_timers;
#endif
    Timer::Timer(const String& name, timer_callback callback, void* state, int dueTime, int period)
    {
        _callback = callback;
        _state = state;
        _dueTime = dueTime;
        _period = period;
        _execution = nullptr;
        
        _firstInvoke = true;
        _start = 0;

        String threadName = name;
        if(threadName.isNullOrEmpty())
        {
            static int no = 0;
            no++;
            threadName = String::convert("TimerProc%d", no);
        }
#ifdef __EMSCRIPTEN__
        _name = threadName;
        _running = false;
        _timers.add((int)this, DateTime::now());
        Debug::writeFormatLine("Create a timer. name: '%s', period: %d, value: 0x%X", name.c_str(), _period, (int)this);
#else
        _thread = new Thread(threadName);
        Debug::writeFormatLine("Create a timer. name: '%s', period: %d", name.c_str(), _period);
#endif
        
        start();
    }
    Timer::Timer(const String& name, timer_callback callback, void* state, int period) : Timer(name, callback, state, Zero, period)
    {
    }
    Timer::Timer(const String& name, timer_callback callback, int period) : Timer(name, callback, nullptr, period)
    {
    }
    
    Timer::Timer(const String& name, timer_callback callback, void* state, const TimeSpan& dueTime, const TimeSpan& period)
            : Timer(name, callback, state, (int)dueTime.totalMilliseconds(), (int)period.totalMilliseconds())
    {
    }
    Timer::Timer(const String& name, timer_callback callback, void* state, const TimeSpan& period) : Timer(name, callback, state, TimeSpan::Zero, period)
    {
    }
    Timer::Timer(const String& name, timer_callback callback, const TimeSpan& period) : Timer(name, callback, nullptr, period)
    {
    }
    
    Timer::~Timer()
    {
#ifdef __EMSCRIPTEN__
        _timers.remove((int)this);
#endif
        
        stop();
        
#ifdef __EMSCRIPTEN__
        Debug::writeFormatLine("Destroy a timer. name: '%s', period: %d, value: 0x%X", name().c_str(), _period, (int)this);
#else
        Debug::writeFormatLine("Destroy a timer. name: '%s', period: %d", name().c_str(), _period);
        delete  _thread;
        _thread = nullptr;
#endif
        
        if(_execution != nullptr)
        {
            delete _execution;
            _execution = nullptr;
        }
    }
    
    void Timer::start()
    {
        if(!running())
        {
            Debug::writeFormatLine("Start a timer. name: '%s', period: %d", name().c_str(), _period);
            
            _firstInvoke = true;
            
#ifdef __EMSCRIPTEN__
            _running = true;
            glutTimerFunc(_dueTime == Zero ? 0 : _dueTime, call_from_timer, (int)this);
#else
            _thread->startProc(timerProc, this, 1);
#endif
        }
    }
    void Timer::stop(uint32_t delaySeconds)
    {
        Debug::writeFormatLine("Stop a timer. name: '%s', period: %d", name().c_str(), _period);
        
#ifdef __EMSCRIPTEN__
        _running = false;
#else
        _thread->stop(delaySeconds);
#endif
    }
    
    const String& Timer::name() const
    {
#ifdef __EMSCRIPTEN__
        return _name;
#else
        return _thread->name();
#endif
    }
    
    bool Timer::running() const
    {
#ifdef __EMSCRIPTEN__
        return _running;
#else
        return _thread->isAlive();
#endif
    }

    void Timer::setPriority(int priority)
    {
#ifndef __EMSCRIPTEN__
        if(_thread != nullptr)
            _thread->setPriority(priority);
#endif
    }
    
    bool Timer::change(int dueTime, int period)
    {
        _dueTime = dueTime;
        _period = period;
        _firstInvoke = true;
        return true;
    }
    bool Timer::change(TimeSpan dueTime, TimeSpan period)
    {
        return change((int)dueTime.totalMilliseconds(), (int)period.totalMilliseconds());
    }
    bool Timer::change(int period)
    {
        if(period != _period)
        {
            _period = period;
            _firstInvoke = true;
            return true;
        }
        return false;
    }
    bool Timer::change(TimeSpan period)
    {
        return change((int)period.totalMilliseconds());
    }
    
    void Timer::fire()
    {
        if(_callback != nullptr)
            _callback(_state);
    }

    void Timer::timerCallback(void* parameter)
    {
        Timer* timer = static_cast<Timer*>(parameter);
        assert(timer);
        if(timer->_execution != nullptr)
            timer->_execution->execute();
    }
    
#ifdef __EMSCRIPTEN__
    void Timer::call_from_timer(int value)
    {
        if(!_timers.contains(value))
        {
            Debug::writeFormatLine("removed timer's invoke, value: 0x%X", value);
            return;
        }
        
        Timer* timer = (Timer*)value;
        if(timer != nullptr && timer->running())
        {
//            Stopwatch sw(String::convert("timer->fire, period: %d", timer->_period), 100);
            timer->fire();
//            sw.stop();
            
            if(timer->_period > 0)
            {
                uint32_t msec = timer->_period < 10 ? 10 : timer->_period;
                glutTimerFunc(msec, call_from_timer, value);
            }
        }
    }
#else
    void Timer::timerProc(void* parameter)
    {
        Timer* timer = (Timer*)parameter;
        timer->timerProcInner();
    }
    void Timer::timerProcInner()
    {
        if(_firstInvoke)
        {
            _firstInvoke = false;
            _start = TickTimeout::getCurrentTickCount();
            
            if(_dueTime == Zero)
            {
                fire();
            }
            else if(_dueTime == Infinite)
            {
            }
            else
            {
                if(_thread->msleepWithBreak(_start, _dueTime))
                {
                    fire();
                    _start = TickTimeout::getCurrentTickCount();
                }
            }
        }
        else
        {
            _start = TickTimeout::getNextTickCount(_start, _period);
            if(_dueTime != Infinite)
            {
                fire();
            }
        }
        
        _thread->msleepWithBreak(_start, _period);
    }
#endif
};
