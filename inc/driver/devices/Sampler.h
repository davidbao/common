#ifndef SAMPLER_H
#define SAMPLER_H

#include "thread/Mutex.h"
#include "system/Delegate.h"
#include "thread/Thread.h"
#include "driver/channels/Channel.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/instructions/InstructionDescription.h"
#include "Device.h"
#include "DeviceDescription.h"
#include "InstructionPool.h"

using namespace Data;

namespace Drivers
{
    class Sampler : public InstructionPool
    {
    public:
        Sampler(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd, bool skipSampler = true);
        ~Sampler() override;
        
        virtual Device::Status getConnectStatus() const;
        bool isOnline() const;
        bool isOffline() const;
        void setConnectedError(bool showError = false);
        
        Delegates* statusChangedDelegates();
        
        void reset() override;
        
    protected:
        // return the sample interval, unit: ms
        virtual uint32_t detectionInterval() const;
        // return the resume interval, unit: ms
        virtual uint32_t resumeInterval() const;
        // return the sample interval, unit: ms
        virtual int detectionCount() const;
        
        virtual void setConnectStatus(Device::Status status);
        
        // return the sample instruction context, do not warry about deleting.
        virtual InstructionDescription* sampleInstruction();
        
		void instructionProcInner() override;
        
        virtual bool checkOnlineFailed();
        
        virtual bool reopen();
        
    protected:
        virtual void addSampleInstruction();
        void errorHandle(const DeviceDescription* dd, const InstructionDescription* id, bool error) override;
        
    private:
        uint32_t _sampleStart;
        Device::Status _connected;
        
        int _checkOnlineFailedCount;
        bool _isInvalidStatus;
        int _connetedFailedCount;
        uint32_t _sampleInterval;
        
        bool _skipSampler;
        
        bool _showConnectedError;
        
        Delegates _statusChangedAction;
    };
}
#endif // SAMPLER_H
