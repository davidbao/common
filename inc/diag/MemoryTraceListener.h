//
//  MemoryTraceListener.h
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef MemoryTraceListener_h
#define MemoryTraceListener_h

#include "TraceListener.h"
#include "data/LoopPList.h"
#include "data/StringArray.h"
#include "thread/Mutex.h"
#include "system/Delegate.h"

using namespace System;

namespace Diag {
    class MemoryTraceListenerContext : public TraceListenerContext {
    public:
        int maxMessageCount;

        MemoryTraceListenerContext(int maxMessageCount = 10000);

        MemoryTraceListenerContext(const MemoryTraceListenerContext &context);

        ~MemoryTraceListenerContext() override;

    public:
        static MemoryTraceListenerContext Default;
    };

    class MemoryTraceListener : public TraceListener {
    public:
        MemoryTraceListener(const MemoryTraceListenerContext &context = MemoryTraceListenerContext::Default);

        ~MemoryTraceListener() override;

        void getAllMessages(StringArray &messages);

        Delegates *updatedDelegates();

    protected:
        void write(const char *message, const char *category) override;

    private:
        Mutex _messagesMutex;
        LoopPList<String> _messages;

        Delegates _updateDelegates;

        MemoryTraceListenerContext _context;
    };

    class TraceUpdatedEventArgs : public EventArgs {
    public:
        TraceUpdatedEventArgs(const String &message, const String &category) {
            this->message = message;
            this->category = category;
        }

        ~TraceUpdatedEventArgs() override {
        }

        String message;
        String category;
    };
}

#endif /* MemoryTraceListener_h */
