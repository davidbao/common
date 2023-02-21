//
//  TraceListener.cpp
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "diag/TraceListener.h"
#include "diag/FileTraceListener.h"
#include "diag/MemoryTraceListener.h"

namespace Diag {
    TraceListenerContext::TraceListenerContext() : enable(true) {
    }

    TraceListenerContext::TraceListenerContext(const TraceListenerContext &context) : enable(true) {
        TraceListenerContext::evaluates(context);
    }

    TraceListenerContext::~TraceListenerContext() = default;

    bool TraceListenerContext::equals(const TraceListenerContext &other) const {
        return this->enable == other.enable;
    }

    void TraceListenerContext::evaluates(const TraceListenerContext &other) {
        this->enable = other.enable;
    }

    TraceListenerContext &TraceListenerContext::operator=(const TraceListenerContext &other) {
        this->evaluates(other);
        return *this;
    }

    const TraceListenerContexts TraceListenerContexts::Default = TraceListenerContexts(new FileTraceListenerContext());
    const TraceListenerContexts TraceListenerContexts::Empty = TraceListenerContexts(true, 256);

    TraceListenerContexts::TraceListenerContexts(bool autoDelete, size_t capacity) :
            PList<TraceListenerContext>(autoDelete, capacity) {
    }

    TraceListenerContexts::TraceListenerContexts(const TraceListenerContexts &contexts) = default;

    TraceListenerContexts::TraceListenerContexts(const TraceListenerContext *context) {
        add(context);
    }

    TraceListenerContexts::~TraceListenerContexts() = default;

    TraceListener::TraceListener() = default;

    TraceListener::~TraceListener() = default;

    TraceListener *TraceListener::create(const TraceListenerContext *context) {
        auto fileContext = dynamic_cast<const FileTraceListenerContext *>(context);
        if (fileContext != nullptr) {
            return new FileTraceListener(*fileContext);
        }

        auto mcontext = dynamic_cast<const MemoryTraceListenerContext *>(context);
        if (mcontext != nullptr) {
            return new MemoryTraceListener(*mcontext);
        }
        return nullptr;
    }

    TraceUpdatedEventArgs::TraceUpdatedEventArgs(const String &message, const String &category) {
        this->message = message;
        this->category = category;
    }
}
