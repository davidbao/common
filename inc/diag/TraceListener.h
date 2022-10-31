//
//  TraceListener.h
//  common
//
//  Created by baowei on 2016/6/2.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef TraceListener_h
#define TraceListener_h

#include "data/ValueType.h"

namespace Common
{
    enum LogLevel
    {
        /// <summary>
        /// Output error-handling messages.
        /// </summary>
        LogError,
        /// <summary>
        /// Output informational messages, warnings, and error-handling messages.
        /// </summary>
        LogInfo,
        /// <summary>
        /// Output no tracing and debugging messages.
        /// </summary>
        LogOff,
        /// <summary>
        /// Output all debugging and tracing messages.
        /// </summary>
        LogVerbose,
        /// <summary>
        /// Output warnings and error-handling messages.
        /// </summary>
        LogWarning,
        /// <summary>
        /// Output system message.
        /// </summary>
        LogSystem,
    };
    
    class TraceListenerContext
    {
    public:
        bool enable;
        
        TraceListenerContext();
        TraceListenerContext(const TraceListenerContext& context);
        virtual ~TraceListenerContext();
    };
//    typedef Vector<TraceListenerContext> TraceListenerContexts;
    class TraceListenerContexts : public Vector<TraceListenerContext>
    {
    public:
        TraceListenerContexts(bool autoDelete = true, uint capacity = DefaultCapacity);
        TraceListenerContexts(const TraceListenerContexts& contexts);
        ~TraceListenerContexts() override;
        
    private:
        TraceListenerContexts(const TraceListenerContext* context);
        
    public:
        static TraceListenerContexts Default;
        static TraceListenerContexts Empty;
    };
    
    class TraceListener 
    {
    public:
        TraceListener();
        virtual ~TraceListener();
        
        static TraceListener* create(const TraceListenerContext* context);
        
    protected:
        virtual void write(const char* message, const char* category) = 0;
        
    private:
        friend class Trace;
    };
    
    typedef Vector<TraceListener> TraceListeners;
}

#endif /* TraceListener_h */
