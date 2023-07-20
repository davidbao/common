//
//  Delegate.h
//  common
//
//  Created by baowei on 15/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Delegate_h
#define Delegate_h

#include "data/List.h"
#include <functional>

using namespace Data;

namespace System {
    class EventArgs {
    public:
        EventArgs();

        virtual ~EventArgs();
    };

    class HandledEventArgs : public EventArgs {
    public:
        explicit HandledEventArgs(bool handled = false);

        HandledEventArgs(const HandledEventArgs &other);

    public:
        bool handled;
    };

    class IDelegate {
    public:
        IDelegate() = default;

        virtual ~IDelegate() = default;

        virtual void invoke(void *sender, EventArgs *args) const = 0;

        void invoke(void *sender) const {
            invoke(sender, nullptr);
        }

        void invoke() const {
            invoke(nullptr, nullptr);
        }
    };

    typedef void (*EventHandler)(void *, void *, EventArgs *);

    class Delegates;

    class Delegate : public IEquatable<Delegate>, public IEvaluation<Delegate> {
    public:
        Delegate();

        explicit Delegate(EventHandler handler);

        Delegate(void *owner, EventHandler handler);

        Delegate(const Delegate &other);

        Delegate(Delegate &&other) noexcept;

        Delegate &operator=(const Delegate &other);

        void evaluates(const Delegate &other) override;

        bool equals(const Delegate &other) const override;

        void invoke(void *sender = nullptr, EventArgs *args = nullptr) const;

    private:
        friend Delegates;

        EventHandler handler;
        void *owner;
    };

    class Delegates : public List<Delegate> {
    public:
        using List<Delegate>::add;
        using List<Delegate>::remove;

        explicit Delegates(size_t capacity = 5);

        Delegates(const Delegates &array);

        Delegates(Delegates &&array) noexcept;

        Delegates(const List &array, off_t offset, size_t count);

        Delegates(const Delegate *array, size_t count, size_t capacity = DefaultCapacity);

        Delegates(const Delegate &value, size_t count);

        Delegates(std::initializer_list<Delegate> list);

        void add(void *owner, EventHandler handler);

        void remove(void *owner, EventHandler handler);

        void invoke(void *sender = nullptr, EventArgs *args = nullptr);

        bool contains(EventHandler handler);
    };
}

#endif // Delegate_h
