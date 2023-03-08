//
//  ThreadPool.h
//  common
//
//  Created by baowei on 16/8/1.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef ThreadPool_h
#define ThreadPool_h

#include "data/PList.h"
#include "thread/Thread.h"
#include "thread/Mutex.h"

namespace Threading {
//    class ThreadPool {
//    public:
//        class Item {
//        public:
//            Mutex threadMutex;
//            Thread *thread;
//            ThreadHolder *holder;
//            ThreadHolderAction action;
//
//            Item(ThreadHolderAction action, ThreadHolder *holder);
//
//            ~Item();
//        };
//
//        typedef PList<Item> Items;
//
//    public:
//        static void startAsync(ThreadHolderAction action, void *owner = nullptr);
//
//        static void startAsync(ThreadHolderAction action, ThreadHolder *holder);
//
//        static bool isAlive(ThreadHolderAction action);
//
//        static bool isAlive(const ThreadHolder *holder);
//
//        static void stop(ThreadHolderAction action);
//
//        static void stop(const ThreadHolder *holder);
//
//        static bool
//        invoke(ThreadHolderAction action, void *owner = nullptr, const TimeSpan &timeout = TimeSpan::MaxValue);
//
//        static bool invoke(ThreadHolderAction action, ThreadHolder *holder, const TimeSpan &timeout = TimeSpan::MaxValue);
//
//        static int threadCount();
//
//    private:
//        ThreadPool();
//
//        ~ThreadPool();
//
//        void startAsyncInner(ThreadHolderAction action, void *owner = nullptr);
//
//        void startAsyncInner2(ThreadHolderAction action, ThreadHolder *holder);
//
//        bool isAliveInner(ThreadHolderAction action);
//
//        bool isAliveInner(const ThreadHolder *holder);
//
//        void stopInner(ThreadHolderAction action);
//
//        void stopInner(const ThreadHolder *holder);
//
//        Item *getInvalidItem(ThreadHolderAction action, ThreadHolder *holder);
//
//        int threadCountInner();
//
//    private:
//        static ThreadPool *instance();
//
//        static bool isStopped(void *parameter);
//
//    private:
//        Mutex _itemsMutex;
//        Items _items;
//
//        static ThreadPool *_instance;
//    };
}

#endif // ThreadPool_h
