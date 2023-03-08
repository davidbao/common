//
//  CommSampler.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef CommSampler_h
#define CommSampler_h

#include "driver/channels/ChannelDescription.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/devices/Sampler.h"
#include "BaseCommConfig.h"

using namespace Data;
using namespace Drivers;

namespace Communication
{
    typedef InstructionDescription* (*sampler_callback)(void*);
    
    class TcpMultiSampler : public Sampler
    {
    public:
        TcpMultiSampler(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd, const Client::Connection& connection, sampler_callback action = nullptr, void* owner = nullptr);
        ~TcpMultiSampler() override;
        
    protected:
        // return the sample interval, unit: ms
        uint32_t detectionInterval() const override;
        // return the resume interval, unit: ms
        uint32_t resumeInterval() const override;
        // return the sample interval, unit: ms
        int detectionCount() const override;
        
        InstructionDescription* sampleInstruction() override;
        void setConnectStatus(Device::Status status) override;
        
        bool checkOnlineFailed() override;
        
    private:
        uint32_t _detectionInterval;
        uint32_t _resumeInterval;
        int _detectionCount;

        sampler_callback _sampleCallback;
        void* _owner;
    };

    class TcpSingleSampler : public Sampler
    {
    public:
        TcpSingleSampler(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd, const Client::Connection& connection, sampler_callback action = nullptr, void* owner = nullptr);
        ~TcpSingleSampler() override;
        
        void start(Channel* channel, Device* device) override;
        void stop() override;
        
    public:
        static void startSinglePool();
        static void stopSinglePool();
        
    protected:
        // return the sample interval, unit: ms
        uint32_t detectionInterval() const override;
        // return the resume interval, unit: ms
        uint32_t resumeInterval() const override;
        // return the sample interval, unit: ms
        int detectionCount() const override;
        
        InstructionDescription* sampleInstruction() override;
        void setConnectStatus(Device::Status status) override;
        
        bool checkOnlineFailed() override;
        
        bool reopen() override;

    private:
        uint32_t _detectionInterval;
        uint32_t _resumeInterval;
        int _detectionCount;

        sampler_callback _sampleCallback;
        void* _owner;
        
    private:
        static Timer* _singleTimer;
        
        static Mutex _poolsMutex;
        static PList<TcpSingleSampler> _pools;
    };
    
    class UdpSampler : public Sampler
    {
    public:
        UdpSampler(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd, const Broadcast& broadcast, sampler_callback action = nullptr, void* owner = nullptr);
        ~UdpSampler() override;
        
    protected:
        // return the sample interval, unit: ms
        uint32_t detectionInterval() const override;
        // return the resume interval, unit: ms
        uint32_t resumeInterval() const override;
        // return the sample interval, unit: ms
        int detectionCount() const override;
        
        InstructionDescription* sampleInstruction() override;
        
    private:
        uint32_t _detectionInterval;
        uint32_t _resumeInterval;
        int _detectionCount;
        
        Broadcast _broadcast;
        sampler_callback _sampleCallback;
        void* _owner;
    };
}

#endif // CommSampler_h
