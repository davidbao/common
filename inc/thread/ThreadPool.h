//
//  ThreadPool.h
//  common
//
//  Created by baowei on 16/8/1.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef common_ThreadPool_h
#define common_ThreadPool_h

#include "data/PList.h"
#include "Thread.h"
#include "Mutex.h"

namespace Threading {
    class ThreadPool {
    public:
        class Item {
        public:
            Mutex threadMutex;
            Thread *thread;
            ThreadHolder *holder;
            action_callback3 action;

            Item(action_callback3 action, ThreadHolder *holder);

            ~Item();
        };

        typedef PList<Item> Items;

    public:
        static void startAsync(action_callback3 action, void *owner = nullptr);

        static void startAsync(action_callback3 action, ThreadHolder *holder);

        static bool isAlive(action_callback3 action);

        static bool isAlive(const ThreadHolder *holder);

        static void stop(action_callback3 action);

        static void stop(const ThreadHolder *holder);

        static bool
        invoke(action_callback3 action, void *owner = nullptr, const TimeSpan &timeout = TimeSpan::MaxValue);

        static bool invoke(action_callback3 action, ThreadHolder *holder, const TimeSpan &timeout = TimeSpan::MaxValue);

        static int threadCount();

    private:
        ThreadPool();

        ~ThreadPool();

        void startAsyncInner(action_callback3 action, void *owner = nullptr);

        void startAsyncInner2(action_callback3 action, ThreadHolder *holder);

        bool isAliveInner(action_callback3 action);

        bool isAliveInner(const ThreadHolder *holder);

        void stopInner(action_callback3 action);

        void stopInner(const ThreadHolder *holder);

        Item *getInvalidItem(action_callback3 action, ThreadHolder *holder);

        int threadCountInner();

    private:
        static ThreadPool *instance();

        static bool isStopped(void *parameter);

    private:
        Mutex _itemsMutex;
        Items _items;

        static ThreadPool *_instance;
    };
}

#endif /* common_ThreadPool_hpp */
