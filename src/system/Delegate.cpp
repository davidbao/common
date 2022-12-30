//
//  Delegate.cpp
//  common
//
//  Created by baowei on 15/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Delegate.h"
#include "thread/Locker.h"
#include "exception/Exception.h"

namespace System {
    EventArgs::EventArgs() {
    }

    EventArgs::~EventArgs() {
    }

    HandledEventArgs::HandledEventArgs(bool handled) {
        this->handled = handled;
    }

    Delegate::Delegate(const Delegate &delegate) : Delegate(delegate.owner, delegate.handler) {
    }

    Delegate::Delegate(EventHandler handler) : Delegate(nullptr, handler) {
    }

    Delegate::Delegate(void *owner, EventHandler handler) {
        this->owner = owner;
        this->handler = handler;
    }

    void Delegate::invoke(void *sender, EventArgs *args) {
        if (handler != nullptr) {
            handler(owner, sender, args);
        }
    }

    Delegates::Delegates(bool autoDelete, uint32_t capacity) : PList<Delegate>(autoDelete, capacity) {
    }

    void Delegates::add(void *owner, EventHandler handler) {
        Locker locker(&_mutex);

        for (uint32_t i = 0; i < count(); i++) {
            Delegate *delegate = at(i);
            if (delegate != nullptr) {
                if (delegate->owner == owner &&
                    delegate->handler == handler) {
                    return;
                }
            }
        }
        PList<Delegate>::add(new Delegate(owner, handler));
    }

    void Delegates::remove(void *owner, EventHandler handler) {
        Locker locker(&_mutex);

        for (uint32_t i = 0; i < count(); i++) {
            Delegate *delegate = at(i);
            if (delegate != nullptr) {
                if (delegate->owner == owner &&
                    delegate->handler == handler) {
                    PList<Delegate>::removeAt(i);
                    break;
                }
            }
        }
    }

    void Delegates::add(const Delegate &delegate) {
        add(delegate.owner, delegate.handler);
    }

    void Delegates::remove(const Delegate &delegate) {
        remove(delegate.owner, delegate.handler);
    }

    void Delegates::invoke(void *sender, EventArgs *args) {
//        Locker locker(&_mutex);

        for (uint32_t i = 0; i < count(); i++) {
            Delegate *delegate = at(i);
            if (delegate != nullptr) {
                delegate->invoke(sender, args);
                HandledEventArgs *hargs = dynamic_cast<HandledEventArgs *>(args);
                if (hargs != nullptr && hargs->handled) {
                    break;
                }
            }
        }
    }

    bool Delegates::contains(EventHandler handler) {
        Locker locker(&_mutex);

        for (uint32_t i = 0; i < count(); i++) {
            Delegate *delegate = at(i);
            if (delegate != nullptr && delegate->handler == handler) {
                return true;
            }
        }
        return false;
    }
}
