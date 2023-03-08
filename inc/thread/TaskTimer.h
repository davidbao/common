//
//  TaskTimer.h
//  common
//
//  Created by baowei on 2017/4/3.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef TaskTimer_h
#define TaskTimer_h

#include "thread/Timer.h"
#include "thread/Mutex.h"

namespace Threading {
    class TaskTimer {
    public:
        class Group {
        public:
            String name;
            Action *action;
            uint32_t start;
            TimeSpan interval;

            Group(const String &name, Action *action, const TimeSpan &interval);

            ~Group();

            bool isTimeUp();

            void execute();
        };

        typedef PList<Group> Groups;

        explicit TaskTimer(const String &name);

        ~TaskTimer();

        bool add(const String &name, Action *action, const TimeSpan &interval);

        bool remove(const String &name);

        bool change(const String &name, const TimeSpan &interval);

        bool contains(const String &name);

        void clear();

        void start(const TimeSpan &dueTime = TimeSpan::Zero);

        void stop();

        bool isStarted() const;

    private:
        void taskTimeUp();

    private:
        Timer *_timer;

        String _name;
        Groups _groups;

        ThreadId _currentThreadId;
    };
}

#endif /* TaskTimer_h */
