//
//  MemoryTraceListener.h
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef MemoryTraceListener_h
#define MemoryTraceListener_h

#include "TraceListener.h"
#include "data/LoopList.h"
#include "data/StringArray.h"
#include "thread/Mutex.h"
#include "system/Delegate.h"

using namespace System;

namespace Diag {
    class MemoryTraceListenerContext : public TraceListenerContext {
    public:
        int maxMessageCount;

        explicit MemoryTraceListenerContext(int maxMessageCount = 10000);

        MemoryTraceListenerContext(const MemoryTraceListenerContext &context);

        ~MemoryTraceListenerContext() override;

        bool equals(const TraceListenerContext &other) const override;

        void evaluates(const TraceListenerContext &other) override;

        MemoryTraceListenerContext &operator=(const MemoryTraceListenerContext &other);

    public:
        static const MemoryTraceListenerContext Default;
    };

    class MemoryTraceListener : public TraceListener {
    public:
        explicit MemoryTraceListener(const MemoryTraceListenerContext &context = MemoryTraceListenerContext::Default);

        ~MemoryTraceListener() override;

        void getAllMessages(StringArray &messages);

        Delegates *updatedDelegates();

        const MemoryTraceListenerContext &context() const;

    protected:
        void write(const String &message, const String &category) override;

    private:
        LoopList<String> _messages;

        Delegates _updateDelegates;

        MemoryTraceListenerContext _context;
    };
}

#endif /* MemoryTraceListener_h */
