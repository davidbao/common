//
//  MemoryTraceListener.cpp
//  common
//
//  Created by baowei on 16/6/2.
//  Copyright © 2016 com. All rights reserved.
//

#include "diag/MemoryTraceListener.h"
#include "thread/Locker.h"

namespace Common
{
    MemoryTraceListenerContext MemoryTraceListenerContext::Default = MemoryTraceListenerContext();
    MemoryTraceListenerContext::MemoryTraceListenerContext(int maxMessageCount)
    {
        if(maxMessageCount >= 1000 && maxMessageCount < 64 * 1024)
            this->maxMessageCount = maxMessageCount;
        else
            this->maxMessageCount = 10000;
    }
    MemoryTraceListenerContext::MemoryTraceListenerContext(const MemoryTraceListenerContext& context) : maxMessageCount(context.maxMessageCount)
    {
    }
    MemoryTraceListenerContext::~MemoryTraceListenerContext()
    {
    }
    
    MemoryTraceListener::MemoryTraceListener(const MemoryTraceListenerContext& context) : _context(context)
    {
        _messages.setMaxLength(context.maxMessageCount);
    }
    MemoryTraceListener::~MemoryTraceListener()
    {
    }

    void MemoryTraceListener::write(const char* message, const char* category)
    {
        if(message == nullptr || strlen(message) == 0)
            return;
        
        Locker locker(&_messagesMutex);
        
        _messages.enqueue(new String(message));
        
        TraceUpdatedEventArgs args(message, category != nullptr ? (String)category : String::Empty);
        _updateDelegates.invoke(this, &args);
    }
    
    void MemoryTraceListener::getAllMessages(StringArray& messages)
    {
        Locker locker(&_messagesMutex);
        
        size_t count = _messages.count();
        if(count > 0)
        {
            String** values = new String*[count];
            _messages.copyTo(values);
            for (uint32_t i=0; i<count; i++)
            {
                messages.add(*values[i]);
            }
            delete[] values;
        }
    }
    
    Delegates* MemoryTraceListener::updatedDelegates()
    {
        return &_updateDelegates;
    }
}
