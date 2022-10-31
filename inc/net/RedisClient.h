//
//  RedisClient.h
//  common
//
//  Created by baowei on 2022/2/16.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef RedisClient_h
#define RedisClient_h

#include "hiredis/hiredis.h"
#include "data/ValueType.h"
#include "data/TimeSpan.h"
#include "thread/Thread.h"
#include "system/Delegate.h"

namespace Common
{
    class RedisClient
    {
    public:
        class ConnectOptions
        {
        public:
            ConnectOptions();
            ConnectOptions(const ConnectOptions& options);
            ~ConnectOptions();
            
            void operator=(const ConnectOptions& value);
            bool operator==(const ConnectOptions& value) const;
            bool operator!=(const ConnectOptions& value) const;
            
            bool isEmpty() const;
            
            String address;
            int port;
            String userName;
            String password;
            TimeSpan connectTimeout;
            TimeSpan commandTimeout;
        };
        
        class ErrorHandle
        {
        public:
            int detectionCount;
            TimeSpan checkInterval;
            
            int errorCount;
            
            ErrorHandle();
            
            void operator=(const ErrorHandle& value);
            bool operator==(const ErrorHandle& value) const;
            bool operator!=(const ErrorHandle& value) const;
        };
        
        RedisClient();
        ~RedisClient();
        
        bool connect(const ConnectOptions& options);
        void connectAsync(const ConnectOptions& options);
        bool disconnect(TimeSpan timeout = TimeSpan::fromSeconds(10));
        
        bool connected();
        
        bool set(const String& key, const String& value, const TimeSpan& expired = TimeSpan::Zero);
        bool get(const String& key, String& value);
        bool setCommand(const String& command, const String& key, const String& value);
        bool getCommand(const String& command, const String& key, String& value);
        
        Delegates* connectingDelegates();
        Delegates* connectedDelegates();
        Delegates* disconnectingDelegates();
        Delegates* disconnectedDelegates();
        
        void setErrorHandle(const ErrorHandle& errorHandle);
        
    private:
        bool destroy();
        
        void check();
        bool connecting() const;
        
    private:
        static void connectAction(ThreadHolder* holder);

        static void checkCallback(void* state);
        
    private:
        redisContext* _context;
        Mutex _contextMutex;
        
        ConnectOptions _connectOptions;
        ErrorHandle _errorHandle;

        Delegates _connectingDelegates;
        Delegates _connectedDelegates;
        Delegates _disconnectingDelegates;
        Delegates _disconnectedDelegates;
        
        Timer* _checkTimer;
        bool _connecting;
    };
}

#endif /* RedisClient_h */
