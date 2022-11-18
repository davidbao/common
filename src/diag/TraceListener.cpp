//
//  TraceListener.cpp
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright © 2016 com. All rights reserved.
//

#include "diag/TraceListener.h"
#include "diag/FileTraceListener.h"
#include "diag/MemoryTraceListener.h"

namespace Common
{
    TraceListenerContext::TraceListenerContext()
    {
        enable = true;
    }
    TraceListenerContext::TraceListenerContext(const TraceListenerContext& context)
    {
        this->enable = context.enable;
    }
    TraceListenerContext::~TraceListenerContext()
    {
    }
    TraceListenerContexts TraceListenerContexts::Default = TraceListenerContexts(new FileTraceListenerContext());
    TraceListenerContexts TraceListenerContexts::Empty = TraceListenerContexts();
    TraceListenerContexts::TraceListenerContexts(const TraceListenerContext* context)
    {
        if(context != nullptr)
            add(context);
    }
    TraceListenerContexts::TraceListenerContexts(bool autoDelete, uint32_t capacity) : PList<TraceListenerContext>(autoDelete, capacity)
    {
    }
    TraceListenerContexts::TraceListenerContexts(const TraceListenerContexts& contexts) : PList<TraceListenerContext>(contexts)
    {
    }
    TraceListenerContexts::~TraceListenerContexts()
    {
    }
    
    TraceListener::TraceListener()
    {
    }
    TraceListener::~TraceListener()
    {
    }
    
    TraceListener* TraceListener::create(const TraceListenerContext* context)
    {
        if(context != nullptr)
        {
            const FileTraceListenerContext* fcontext = dynamic_cast<const FileTraceListenerContext*>(context);
            if(fcontext != nullptr)
            {
                return new FileTraceListener(*fcontext);
            }
            
            const MemoryTraceListenerContext* mcontext = dynamic_cast<const MemoryTraceListenerContext*>(context);
            if(mcontext != nullptr)
            {
                return new MemoryTraceListener(*mcontext);
            }
        }
        return nullptr;
    }
}
