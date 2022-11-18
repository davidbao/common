//
//  TaskTimer.h
//  common
//
//  Created by baowei on 03/04/2017.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef TaskTimer_h
#define TaskTimer_h

#include "Timer.h"
#include "Mutex.h"

namespace Common
{
    class TaskTimer
    {
    public:
        class Group
        {
        public:
            String name;
            timer_callback callback;
            TimerExecution* execution;
            uint32_t start;
            TimeSpan interval;
            void* owner;
            
            Group(const String& name, timer_callback callback, const TimeSpan& interval, void* owner = nullptr);
            Group(const String& name, TimerExecution* execution, const TimeSpan& interval);
            ~Group();
            
            bool isTimeup();
        };
        typedef PList<Group> Groups;
        
        TaskTimer(const String& name, void* owner = nullptr);
        ~TaskTimer();
        
        bool add(const String& name, timer_callback callback, const TimeSpan& interval, void* owner = nullptr);
        bool add(const String& name, timer_callback callback, void* owner, const TimeSpan& interval);
        template<class T>
        bool add(const String& name, const TimerCallback<T>& callback, const TimeSpan& interval)
        {
            if(contains(name))
                return false;
            
            Group* group = new Group(name, callback.clone(), interval);
            Locker locker(&_groupsMutex);
            _groups.add(group);
            return true;
        }
        
        bool remove(timer_callback callback);
        bool remove(const String& name);
        template<class T>
        bool remove(const TimerCallback<T>& callback)
        {
            Locker locker(&_groupsMutex);
            for (uint32_t i=0; i<_groups.count(); i++)
            {
                if(_groups[i]->execution != nullptr &&
                   _groups[i]->execution->equals(callback))
                {
                    _groups.removeAt(i);
                    return true;
                }
            }
            return false;
        }
        
        bool change(const String& name, const TimeSpan& interval);
        
        bool contains(timer_callback callback);
        bool contains(const String& name);
        
        void clear();
        
        void start(const TimeSpan& dueTime = TimeSpan::Zero);
        void stop();
        
        bool isStarted() const;
        
    private:
        void taskTimeUpInner();
        
    private:
        static void taskTimeUp(void* state);
        
    private:
        Timer* _timer;
        
        String _name;
        void* _owner;
        Groups _groups;
        Mutex _groupsMutex;
        
        Thread::Id _currentThreadId;
    };
}

#endif /* TaskTimer_h */
