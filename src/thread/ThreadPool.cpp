//
//  ThreadPool.cpp
//  common
//
//  Created by baowei on 16/8/1.
//  Copyright © 2016 com. All rights reserved.
//

#include "thread/ThreadPool.h"
#include <assert.h>
#include "thread/Locker.h"
#include "thread/TickTimeout.h"

namespace Threading {
    ThreadPool::Item::Item(action_callback3 action, ThreadHolder *holder) {
        static int no = 0;
        no++;
        thread = new Thread(String::convert("ThreadPool%d", no));
        this->action = action;
        this->holder = holder;
    }

    ThreadPool::Item::~Item() {
        delete thread;
    }

    ThreadPool *ThreadPool::_instance = nullptr;

    ThreadPool::ThreadPool() {
    }

    ThreadPool::~ThreadPool() {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];

            Locker locker(&item->threadMutex);
            item->thread->stop();
        }
        _items.clear();
    }

    void ThreadPool::startAsync(action_callback3 action, void *owner) {
        instance()->startAsyncInner(action, owner);
    }

    void ThreadPool::startAsync(action_callback3 action, ThreadHolder *holder) {
        instance()->startAsyncInner2(action, holder);
    }

    bool ThreadPool::isAlive(action_callback3 action) {
        return instance()->isAliveInner(action);
    }

    bool ThreadPool::isAlive(const ThreadHolder *holder) {
        return instance()->isAliveInner(holder);
    }

    void ThreadPool::stop(action_callback3 action) {
        instance()->stopInner(action);
    }

    void ThreadPool::stop(const ThreadHolder *holder) {
        instance()->stopInner(holder);
    }

    bool ThreadPool::invoke(action_callback3 action, void *owner, const TimeSpan &timeout) {
        startAsync(action, owner);
        return TickTimeout::delay(timeout, isStopped, (void *) action);
    }

    bool ThreadPool::invoke(action_callback3 action, ThreadHolder *holder, const TimeSpan &timeout) {
        startAsync(action, holder);
        return TickTimeout::delay(timeout, isStopped, (void *) action);
    }

    bool ThreadPool::isStopped(void *parameter) {
        action_callback3 action = (action_callback3) parameter;
        return !isAlive(action);
    }

    int ThreadPool::threadCount() {
        return instance()->threadCountInner();
    }

    int ThreadPool::threadCountInner() {
        int count = 0;
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];
            if (item->thread->isAlive())
                count++;
        }
        return count;
    }

    ThreadPool *ThreadPool::instance() {
        if (_instance == nullptr) {
            _instance = new ThreadPool();
        }
        return _instance;
    }

    void ThreadPool::startAsyncInner(action_callback3 action, void *owner) {
        startAsyncInner2(action, new ThreadHolder(owner));
    }

    void ThreadPool::startAsyncInner2(action_callback3 action, ThreadHolder *holder) {
        Item *item = getInvalidItem(action, holder);
        assert(item);
        assert(!item->thread->isAlive());

        Locker locker(&item->threadMutex);

        item->thread->start(action, holder);
    }

    bool ThreadPool::isAliveInner(action_callback3 action) {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];
            if (item->action == action)
                return item->thread->isAlive();
        }

        return false;
    }

    bool ThreadPool::isAliveInner(const ThreadHolder *holder) {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];
            if (item->holder == holder)
                return item->thread->isAlive();
        }

        return false;
    }

    void ThreadPool::stopInner(action_callback3 action) {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];
            if (item->action == action &&
                item->thread->isAlive()) {
                item->thread->stop();
                break;
            }
        }
    }

    void ThreadPool::stopInner(const ThreadHolder *holder) {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];
            if (item->holder == holder &&
                item->thread->isAlive()) {
                item->thread->stop();
                break;
            }
        }
    }

    ThreadPool::Item *ThreadPool::getInvalidItem(action_callback3 action, ThreadHolder *holder) {
        Locker locker(&_itemsMutex);
        for (uint32_t i = 0; i < _items.count(); i++) {
            Item *item = _items[i];

            Locker locker2(&item->threadMutex);
            if (!item->thread->isAlive()) {
                item->action = action;
                item->holder = holder;
                return item;
            }
        }

        ThreadPool::Item *item = new ThreadPool::Item(action, holder);
        _items.add(item);
        return item;
    }
}
