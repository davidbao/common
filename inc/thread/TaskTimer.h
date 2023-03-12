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

namespace Threading {
    class TaskTimer {
    public:
        explicit TaskTimer(const String &name = String::Empty);

        ~TaskTimer();

        template<class Function, class... Args>
        bool add(const String &name, const TimeSpan &interval, Function &&f, Args &&... args) {
            if (contains(name))
                return false;

            auto group = new Group(name, interval, f, args...);
            Locker locker(&_groups);
            _groups.add(group);
            return true;
        }

        bool remove(const String &name);

        bool change(const String &name, const TimeSpan &interval);

        bool contains(const String &name);

        void clear();

        void start(const TimeSpan &dueTime = TimeSpan::Zero);

        void stop();

        bool running() const;

        const String &name() const;

        bool hasTimer();

    private:
        void taskTimeUp();

    private:
        class Group {
        public:
            template<class Function, class... Args>
            Group(const String &name, const TimeSpan &interval, Function &&f, Args &&... args) :
                    _name(name), _interval(interval), _start(0), _action(f, args...) {
            }

            ~Group() = default;

            const String &name() const;

            bool isTimeUp();

            void execute() const;

            void change(const String &name, const TimeSpan &interval);

        private:
            String _name;
            uint64_t _start;
            TimeSpan _interval;
            Action _action;
        };

        typedef PList<Group> Groups;

        Timer *_timer;

        String _name;
        Groups _groups;

        ThreadId _currentThreadId;
    };
}

#endif /* TaskTimer_h */
