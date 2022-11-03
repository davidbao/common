//
//  BaseCommService.h
//  common
//
//  Created by baowei on 2017/2/17.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef BaseCommService_h
#define BaseCommService_h

#include "thread/Thread.h"
#include "driver/instructions/InstructionContext.h"
#include "configuration/Configuration.h"
#include "CommSampler.h"
#include "CommInstructionSet.h"

using namespace Common;

namespace Drivers
{
    class DriverManager;
}
using namespace Drivers;

namespace Communication
{
    class ServiceBehaviour
    {
    public:
        enum SenderTypes
        {
            None = 0,
            Async,
            Sync
        };
        
        enum Qos
        {
            Qos0 = 0,
            Qos1 = 1
        };
        
        bool syncInitValue;
        SenderTypes senderType;
        StringArray includedServices;
        StringArray excludedServices;
        
        ServiceBehaviour();
        ServiceBehaviour(const ServiceBehaviour& value);
        
        void operator=(const ServiceBehaviour& value);
        bool operator==(const ServiceBehaviour& value) const;
        bool operator!=(const ServiceBehaviour& value) const;
        
        bool isSender() const;
        bool isAsyncSender() const;
        bool isSyncSender() const;
        
        bool isSyncInitValue() const;
        
        bool containService(const String& name) const;
    };
    
    class BaseCommService
    {
    public:
        struct InstructionCallback
        {
            instructions_callback tcpInstructions;
            sampler_callback tcpSampler;
            instructions_callback udpSendInstructions;
            instructions_callback udpRecvInstructions;
            sampler_callback udpSampler;
            void* owner;
            bool asyncOpen;
            
            ServiceBehaviour behaviour;
            
            InstructionCallback();
            InstructionCallback(const InstructionCallback& callback);
            
            bool hasUdpInstructions() const;
        };
        
        class BaseLoopSender
        {
        public:
            BaseLoopSender(BaseCommService* service, const String& name, const TimeSpan& interval, const Endpoint& endpoint, int sendCount);
            virtual ~BaseLoopSender();
            
            const String& name() const;
            
            void start();
            void stop();
            
        protected:
            virtual bool invoke() = 0;
            bool canInvoke();
            
        private:
            static void timerCallback(void* state);
            
        protected:
            BaseCommService* _service;
            String _name;
            TimeSpan _interval;
            int _sendCount;
            
            Timer* _timer;
            Endpoint _endpoint;
            int _invokeCount;
        };
        typedef PList<BaseLoopSender> BaseLoopSenders;
        
        class BasePacketSender
        {
        public:
            BasePacketSender(BaseCommService* service, const String& name, const TimeSpan& interval);
            virtual ~BasePacketSender();
            
            const String& name() const;
            
            void start();
            void stop();
            
        protected:
            virtual void invoke();
            
        private:
            static void timerCallback(void* state);
            
        protected:
            BaseCommService* _service;
            String _name;
            TimeSpan _interval;
            
            Timer* _timer;
        };
        typedef PList<BasePacketSender> BasePacketSenders;
        
        BaseCommService();
        virtual ~BaseCommService();
        
        inline const String& error() const
        {
            return _error;
        }
        
        virtual bool initialize(const InstructionCallback& callback);
        virtual bool unInitialize();
        
        const ServiceBehaviour& behaviour() const;
        void setServiceBehaviour(const ServiceBehaviour& behaviour);
        
    protected:
        virtual void createDevice(const InstructionCallback& callback) = 0;
        
        DriverManager* manager() const;
        
    protected:
        String _error;
        
        InstructionPool* _instructionPool;
        
        DriverManager* _manager;
        
        BaseLoopSenders _loopSenders;
        Mutex _loopSendersMutex;
        
        BasePacketSenders _packetSenders;
        Mutex _packetSendersMutex;
        
        ServiceBehaviour _behaviour;
    };
}

#endif  // BaseCommService_h
