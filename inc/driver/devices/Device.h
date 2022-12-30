#ifndef DEVICE_H
#define DEVICE_H

#include "data/StreamVector.h"
#include "data/ValueType.h"
#include "thread/Locker.h"
#include "system/Delegate.h"
#include "driver/instructions/Instruction.h"
#include "DeviceDescription.h"

using namespace Data;

namespace Drivers
{
	class Channel;
	class Instruction;
	class InstructionSet;
	class DeviceDescription;
	class Device
	{
	public:
		enum Status
		{
			Unknown = 0,
			Offline = 1,
			Online = 2,
            Disabled = 3
		};

		struct StatusSnap : public StreamAccessor
		{
		public:
			Status OldStatus;
			Status NewStatus;

            StatusSnap();
            StatusSnap(Status status);
            StatusSnap(Status oldStatus, Status newStatus);

            void write(Stream* stream) const override;
            void read(Stream* stream) override;
            void copyFrom(const StatusSnap* status);
		};
        
        class ReceivedEventArgs : public HandledEventArgs
        {
        public:
            ReceivedEventArgs(const ByteArray& buffer)
            {
                this->buffer = buffer;
            }
            
            ByteArray buffer;
        };
        
        Device(DeviceDescription* dd, Channel* channel);
		~Device();

		DeviceDescription* description() const;
		Channel* getChannel() const;

		bool receive(ByteArray* buffer);

		const String& name() const;

        bool executeInstruction();
        bool executeInstruction(const ByteArray& buffer);
		InstructionContext* executeInstruction(InstructionDescription* id);
        InstructionContext* executeInstructionAsync(InstructionDescription* id);

		InstructionSet* instructionSet() const;
        
        void addReceiveDevice(Device* device);
        void removeReceiveDevice(Device* device);
        
        void setAllowLog(bool allowInformation, bool allowMessage);
        
        void addDynamicInstructions(Instructions* instructions);
        void clearDynamicInstructions();
        
        Instruction* matchInstruction(const ByteArray& buffer);
        
        Delegates* receivedDelegates();
        
        Instructions* instructions();
        
    public:
        static String getStatusStr(const Status status);
        static bool isStatusChanged(Status oldStatus, Status newStatus);
        
    private:
        bool executeSingleInstruction(const ByteArray& buffer);
        
		InstructionContext* executeInstruction(Instruction* instruction, InstructionContext* ic, const ByteArray* buffer = nullptr);
        InstructionContext* executeInstructionAsync(Instruction* instruction, InstructionContext* ic);
        
        void setReceiveInstruction(Instruction* instruction);
        
        Instruction* getInstruction(const String& instructionName);
        Instruction* getInstruction(const InstructionDescription* id);
        
        Instruction* matchInstruction(Instructions* instructions, const ByteArray& buffer);
        
        void clearDynamicInstructionsInner();
        
    private:
        static Instruction* getInstruction(Instructions* instructions, const String& instructionName);
        static void setAllowLog(Instructions* instructions, bool allowInformation, bool allowMessage);
        
        static void deviceReceived(void* owner, void* sender, EventArgs* args);

	private:
		DeviceDescription* _description;
		Instructions* _instructions;
		Channel* _channel;
        
        Mutex _receiveDevicesMutex;
        PList<Device> _receiveDevices;
        
        Instructions* _dynamicInstructions;
        Mutex _dynamicInstructionsMutex;
        
        Instructions _deletedInstructions;
        Mutex _deletedInstructionsMutex;
        uint32_t _deletedTick;
        
        Delegates _receivedDelegates;
	};
    typedef PList<Device> Devices;
    
    class DeviceStatusEventArgs : public EventArgs
    {
    public:
        String deviceName;
        Device::Status oldStatus;
        Device::Status newStatus;
        
        DeviceStatusEventArgs(const String& deviceName, Device::Status oldStatus, Device::Status newStatus);
        DeviceStatusEventArgs(const DeviceStatusEventArgs& args);
        ~DeviceStatusEventArgs() override;
    };
}
#endif // DEVICE_H
