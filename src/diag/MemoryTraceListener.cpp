//
//  MemoryTraceListener.cpp
//  common
//
//  Created by baowei on 16/6/2.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "diag/MemoryTraceListener.h"
#include "diag/Trace.h"
#include "thread/Locker.h"

namespace Diag {
    const MemoryTraceListenerContext MemoryTraceListenerContext::Default = MemoryTraceListenerContext();

    MemoryTraceListenerContext::MemoryTraceListenerContext(int maxMessageCount) {
        if (maxMessageCount >= 1000 && maxMessageCount < 64 * 1024)
            this->maxMessageCount = maxMessageCount;
        else
            this->maxMessageCount = 10000;
    }

    MemoryTraceListenerContext::MemoryTraceListenerContext(const MemoryTraceListenerContext &context)
            : MemoryTraceListenerContext(context.maxMessageCount) {
    }

    MemoryTraceListenerContext::~MemoryTraceListenerContext() = default;

    bool MemoryTraceListenerContext::equals(const TraceListenerContext &other) const {
        if (!TraceListenerContext::equals(other)) {
            return false;
        }

        auto context = dynamic_cast<const MemoryTraceListenerContext *>(&other);
        if (context != nullptr) {
            return this->maxMessageCount == context->maxMessageCount;
        }
        return false;
    }

    void MemoryTraceListenerContext::evaluates(const TraceListenerContext &other) {
        TraceListenerContext::evaluates(other);

        auto context = dynamic_cast<const MemoryTraceListenerContext *>(&other);
        if (context != nullptr) {
            this->maxMessageCount = context->maxMessageCount;
        }
    }

    MemoryTraceListenerContext &MemoryTraceListenerContext::operator=(const MemoryTraceListenerContext &other) {
        MemoryTraceListenerContext::evaluates(other);
        return *this;
    }

    MemoryTraceListener::MemoryTraceListener(const MemoryTraceListenerContext &context) :
            _messages(context.maxMessageCount), _context(context) {
    }

    MemoryTraceListener::~MemoryTraceListener() = default;

    void MemoryTraceListener::write(const String &message, const String &category) {
        if (message.isNullOrEmpty()) {
            return;
        }

        {
            Locker locker(&_messages);
            _messages.enqueue(message);
        }

        TraceUpdatedEventArgs args(message, !category.isNullOrEmpty() ? category.c_str() : Trace::Info);
        _updateDelegates.invoke(this, &args);
    }

    void MemoryTraceListener::getAllMessages(StringArray &messages) {
        Locker locker(&_messages);

        size_t count = _messages.count();
        if (count > 0) {
            auto values = new String*[count];
            _messages.copyTo(values);
            for (uint32_t i = 0; i < count; i++) {
                messages.add(*values[i]);
            }
            delete[] values;
        }
    }

    Delegates *MemoryTraceListener::updatedDelegates() {
        return &_updateDelegates;
    }

    const MemoryTraceListenerContext &MemoryTraceListener::context() const {
        return _context;
    }
}
