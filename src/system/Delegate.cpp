//
//  Delegate.cpp
//  common
//
//  Created by baowei on 15/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Delegate.h"

namespace System {
    EventArgs::EventArgs() = default;

    EventArgs::~EventArgs() = default;

    HandledEventArgs::HandledEventArgs(bool handled) {
        this->handled = handled;
    }

    Delegate::Delegate() : owner(nullptr), handler(nullptr) {
    }

    Delegate::Delegate(EventHandler handler) : Delegate(nullptr, handler) {
    }

    Delegate::Delegate(void *owner, EventHandler handler) : owner(owner), handler(handler) {
    }

    Delegate::Delegate(const Delegate &other) : Delegate(other.owner, other.handler) {
    }

    Delegate::Delegate(Delegate &&other) noexcept {
        owner = other.owner;
        other.owner = nullptr;
        handler = other.handler;
        other.handler = nullptr;
    }

    Delegate &Delegate::operator=(const Delegate &other) {
        if (this != &other) {
            Delegate::evaluates(other);
        }
        return *this;
    }

    void Delegate::evaluates(const Delegate &other) {
        owner = other.owner;
        handler = other.handler;
    }

    bool Delegate::equals(const Delegate &other) const {
        return owner == other.owner &&
               handler == other.handler;
    }

    void Delegate::invoke(void *sender, EventArgs *args) const {
        if (handler != nullptr) {
            handler(owner, sender, args);
        }
    }

    Delegates::Delegates(size_t capacity) : List<Delegate>(capacity) {
    }

    Delegates::Delegates(const Delegates &array) = default;

    Delegates::Delegates(Delegates &&array) noexcept: List(array.capacity()) {
    }

    Delegates::Delegates(const List &array, off_t offset, size_t count) : List(array, offset, count) {
    }

    Delegates::Delegates(const Delegate *array, size_t count, size_t capacity) : List(array, count, capacity) {
    }

    Delegates::Delegates(const Delegate &value, size_t count) : List(DefaultCapacity) {
    }

    Delegates::Delegates(std::initializer_list<Delegate> list) : Delegates(DefaultCapacity) {
    }

    void Delegates::add(void *owner, EventHandler handler) {
        for (size_t i = 0; i < count(); i++) {
            const Delegate &delegate = at(i);
            if (delegate.owner == owner &&
                delegate.handler == handler) {
                return;
            }
        }
        List<Delegate>::add(Delegate(owner, handler));
    }

    void Delegates::remove(void *owner, EventHandler handler) {
        for (size_t i = 0; i < count(); i++) {
            const Delegate &delegate = at(i);
            if (delegate.owner == owner && delegate.handler == handler) {
                List<Delegate>::removeAt(i);
                break;
            }
        }
    }

    void Delegates::invoke(void *sender, EventArgs *args) {
        for (size_t i = 0; i < count(); i++) {
            const Delegate &delegate = at(i);
            delegate.invoke(sender, args);
            auto e = dynamic_cast<HandledEventArgs *>(args);
            if (e != nullptr && e->handled) {
                break;
            }
        }
    }

    bool Delegates::contains(EventHandler handler) {
        for (size_t i = 0; i < count(); i++) {
            const Delegate &delegate = at(i);
            if (delegate.handler == handler) {
                return true;
            }
        }
        return false;
    }
}
