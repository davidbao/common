//
//  Delegate.h
//  common
//
//  Created by baowei on 15/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef __common__Delegate__
#define __common__Delegate__

#include "thread/Mutex.h"
#include "data/PList.h"

namespace Common
{
    class EventArgs
    {
    public:
        EventArgs();
        virtual ~EventArgs();
    };
    class HandledEventArgs : public EventArgs
    {
    public:
        HandledEventArgs(bool handled = false);
        
    public:
        bool handled;
    };
    
    typedef void (*EventHandler)(void*, void*, EventArgs*);
//    typedef void (*HandledEventHandler)(void*, HandledEventArgs*);
    
    class Delegate
    {
    public:
        Delegate(const Delegate& delegate);
        Delegate(EventHandler handler);
        Delegate(void* owner, EventHandler handler);
        
        void invoke(void* sender = nullptr, EventArgs* args = nullptr);
        
    public:
        EventHandler handler;
        void* owner;
    };
//    typedef PList<Delegate> Delegates;
    class Delegates : public PList<Delegate>
    {
    public:
        Delegates(bool autoDelete = true, uint32_t capacity = 5);

        void add(const Delegate& delegate);
        void remove(const Delegate& delegate);
        void add(void* owner, EventHandler handler);
        void remove(void* owner, EventHandler handler);
        void invoke(void* sender = nullptr, EventArgs* args = nullptr);
        bool contains(EventHandler handler);
        
    private:
        Mutex _mutex;
    };
}

#endif /* defined(__common__Delegate__) */
