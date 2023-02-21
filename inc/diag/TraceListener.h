//
//  TraceListener.h
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef TraceListener_h
#define TraceListener_h

#include "data/String.h"
#include "data/PList.h"
#include "system/Delegate.h"

using namespace Data;
using namespace System;

namespace Diag {
    class TraceListenerContext : public IEquatable<TraceListenerContext>, public IEvaluation<TraceListenerContext> {
    public:
        bool enable;

        TraceListenerContext();

        TraceListenerContext(const TraceListenerContext &context);

        ~TraceListenerContext() override;

        bool equals(const TraceListenerContext &other) const override;

        void evaluates(const TraceListenerContext &other) override;

        TraceListenerContext &operator=(const TraceListenerContext &other);
    };

    class TraceListenerContexts : public PList<TraceListenerContext> {
    public:
        explicit TraceListenerContexts(bool autoDelete = true, size_t capacity = DefaultCapacity);

        TraceListenerContexts(const TraceListenerContexts &contexts);

        ~TraceListenerContexts() override;

    private:
        explicit TraceListenerContexts(const TraceListenerContext *context);

    public:
        static const TraceListenerContexts Default;
        static const TraceListenerContexts Empty;
    };

    class TraceListener {
    public:
        TraceListener();

        virtual ~TraceListener();

    public:
        static TraceListener *create(const TraceListenerContext *context);

    protected:
        virtual void write(const String &message, const String &category) = 0;

    private:
        friend class Trace;
    };

    typedef PList<TraceListener> TraceListeners;

    class TraceUpdatedEventArgs : public EventArgs {
    public:
        String message;
        String category;

        TraceUpdatedEventArgs(const String &message, const String &category);

        ~TraceUpdatedEventArgs() override {
        }
    };
}

#endif /* TraceListener_h */
