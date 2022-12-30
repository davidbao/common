#include "system/Math.h"
#include "driver/devices/Device.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/TickTimeout.h"
#include "exception/Exception.h"
#include "driver/instructions/Instruction.h"
#include "driver/instructions/InstructionSet.h"
#include "driver/channels/Channel.h"
#include "driver/devices/DeviceDescription.h"

using namespace Diag;
using namespace System;

namespace Drivers
{
    Device::StatusSnap::StatusSnap()
    {
        OldStatus = Unknown;
        NewStatus = Unknown;
    }
    Device::StatusSnap::StatusSnap(Status status)
    {
        OldStatus = Unknown;
        NewStatus = status;
    }
    Device::StatusSnap::StatusSnap(Status oldStatus, Status newStatus)
    {
        OldStatus = oldStatus;
        NewStatus = newStatus;
    }
    
    void Device::StatusSnap::write(Stream* stream) const
    {
        stream->writeByte(OldStatus);
        stream->writeByte(NewStatus);
    }
    void Device::StatusSnap::read(Stream* stream)
    {
        OldStatus = (Status)stream->readByte();
        NewStatus = (Status)stream->readByte();
    }
    void Device::StatusSnap::copyFrom(const StatusSnap* status)
    {
        OldStatus = status->OldStatus;
        NewStatus = status->NewStatus;
    }

	Device::Device(DeviceDescription* dd, Channel* channel)
	{
		_description = dd;
		InstructionSet* is = instructionSet();
		if(is == nullptr)
		{
			throw ArgumentException("Is must not be nullptr.", "dd.InstructionSet");
		}
		_instructions = new Instructions();
		is->generateInstructions(_instructions);

		_channel = channel;
        
        Locker locker(&_receiveDevicesMutex);
        _receiveDevices.setAutoDelete(false);
        
        _dynamicInstructions = nullptr;
        _deletedTick = TickTimeout::getCurrentTickCount();
	}
	Device::~Device()
	{
		_description = nullptr;
		delete _instructions;

		_channel = nullptr;

        Locker locker(&_dynamicInstructionsMutex);
        if(_dynamicInstructions != nullptr)
        {
            delete _dynamicInstructions;
            _dynamicInstructions = nullptr;
        }
	}

	const String& Device::name() const
	{
		return _description != nullptr ? _description->name() : String::Empty;
	}

	DeviceDescription* Device::description() const
	{
		return _description;
	}

	Channel* Device::getChannel() const
	{
		return _channel;
	}

	Instruction* Device::getInstruction(const String& instructionName)
	{
		if (instructionName.isNullOrEmpty())
			return nullptr;

        Instruction* instruction = getInstruction(_instructions, instructionName);
        if(instruction != nullptr)
            return instruction;

        Locker locker(&_dynamicInstructionsMutex);
        if(_dynamicInstructions != nullptr)
        {
            return getInstruction(_dynamicInstructions, instructionName);
        }
        return nullptr;
	}
    Instruction* Device::getInstruction(const InstructionDescription* id)
    {
        return id != nullptr ? getInstruction(id->name()) : nullptr;
    }

	InstructionSet* Device::instructionSet() const
	{
		return _description != nullptr ? _description->instructionSet() : nullptr;
	}

	bool Device::receive(ByteArray* buffer)
	{
#ifdef DEBUG
        Stopwatch sw(String::convert("Device::receive, name: %s", name().c_str()), 1000);
#endif
		bool result = instructionSet()->receive(this, _channel, buffer);
#ifdef DEBUG
        if(sw.elapsed() >= 3000)
        {
            Debug::writeFormatLine("channel connected: %s, available: %d", _channel->connected() ? "true" : "false", _channel->available());
        }
#endif
        return result;
	}
	Instruction* Device::matchInstruction(const ByteArray& buffer)
	{
#ifdef DEBUG
        Stopwatch sw("Device::matchInstruction", 1000);
#endif
        Instruction* instruction = matchInstruction(_instructions, buffer);
        if(instruction != nullptr)
            return instruction;

        Locker locker(&_dynamicInstructionsMutex);
        if(_dynamicInstructions != nullptr)
        {
            return matchInstruction(_dynamicInstructions, buffer);
        }
        return nullptr;
	}
    bool Device::executeInstruction()
    {
        ByteArray buffer;
        if(receive(&buffer))
        {
            return executeSingleInstruction(buffer);
        }
        return false;
    }
    bool Device::executeInstruction(const ByteArray& buffer)
    {
#ifdef DEBUG
        Stopwatch sw("Device::executeInstruction", 1000);
#endif
        PList<ByteArray> buffers;
        if(instructionSet()->recombine(buffer, buffers))
        {
            for (uint32_t i=0; i<buffers.count(); i++)
            {
                const ByteArray* singleBuffer = buffers[i];
//#ifdef DEBUG
//                String str;
//                uint32_t logLength = 128;
//                size_t count = singleBuffer->count();
//                if(count > logLength)
//                {
//                    ByteArray array(singleBuffer->data(), Math::min(logLength, count));
//                    str = array.toString();
//                }
//                else
//                {
//                    str = singleBuffer->toString();
//                }
//                Debug::writeFormatLine("Device::executeInstruction, recv buffer: %s", str.c_str());
//#endif
                executeSingleInstruction(*singleBuffer);
            }
            return true;
        }
        return false;
    }
    bool Device::executeSingleInstruction(const ByteArray& buffer)
    {
#ifdef DEBUG
        Stopwatch sw("Device::executeSingleInstruction", 1000);
#endif
        ReceivedEventArgs e(buffer);
        _receivedDelegates.invoke(this, &e);
        if(!e.handled)
        {
            Instruction* instruction = matchInstruction(buffer);
            if (instruction != nullptr)
            {
                executeInstruction(instruction, instruction->context(), &buffer);
                return true;
            }
//            else
//            {
//#ifdef DEBUG
//                String str;
//                uint32_t logLength = 128;
//                size_t count = buffer.count();
//                if(count > logLength)
//                {
//                    ByteArray array(buffer.data(), Math::min(logLength, count));
//                    str = array.toString();
//                }
//                else
//                {
//                    str = buffer.toString();
//                }
//                Debug::writeFormatLine("Can not match the instructions! device name: %s, recv buffer: %s", name().c_str(), str.c_str());
//#endif
//            }
        }
        return false;
    }
	InstructionContext* Device::executeInstruction(InstructionDescription* id)
	{
        Instruction* instruction = getInstruction(id);
//#ifdef DEBUG
//        if(instruction == nullptr)
//        {
//            Debug::writeFormatLine("Can not find the instruction, name: %s", id->name().c_str());
//        }
//#endif
        return executeInstruction(instruction, id->context());
	}
	InstructionContext* Device::executeInstruction(Instruction* instruction, InstructionContext* ic, const ByteArray* buffer)
	{
		if(instruction != nullptr)
		{
#ifdef DEBUG
            Stopwatch sw(String::convert("Device::executeInstruction, device name: %s, instruction name: %s",
                                         name().c_str(), instruction->description()->name().c_str()), 1000);
#endif
            clearDynamicInstructionsInner();
            
			Channel* channel = getChannel();
			if(channel != nullptr && channel->connected())
			{
//#ifdef DEBUG
//				Debug::writeFormatLine("Execute a instruction, device name: %s, instruction name: %s.",
//					name().c_str(), instruction->description()->name().c_str());
//#endif
                instruction->_isExecuting = true;
				InstructionContext* oldic = instruction->context();
                instruction->setContext(ic);
				InstructionContext* result = instruction->execute(channel->interactive(), this, ic, buffer);
				instruction->setContext(oldic);
                instruction->_isExecuting = false;
				return result;
			}
		}
		return nullptr;
	}

    InstructionContext* Device::executeInstructionAsync(InstructionDescription* id)
    {
        Instruction* instruction = getInstruction(id);
    //#ifdef DEBUG
    //        if(instruction == nullptr)
    //        {
    //            Debug::writeFormatLine("Can not find the instruction, name: %s", id->name().c_str());
    //        }
    //#endif
        return executeInstructionAsync(instruction, id->context());
    }
    InstructionContext* Device::executeInstructionAsync(Instruction* instruction, InstructionContext* ic)
    {
        if(instruction != nullptr)
        {
    #ifdef DEBUG
            Stopwatch sw(String::convert("Device::executeInstructionAsync, device name: %s, instruction name: %s",
                                         name().c_str(), instruction->description()->name().c_str()), 1000);
    #endif
            clearDynamicInstructionsInner();
            
            Channel* channel = getChannel();
            if(channel != nullptr && channel->connected())
            {
    //#ifdef DEBUG
    //                Debug::writeFormatLine("Execute a instruction, device name: %s, instruction name: %s.",
    //                    name().c_str(), instruction->description()->name().c_str());
    //#endif
                instruction->_isExecuting = true;
                InstructionContext* oldic = instruction->context();
                instruction->setContext(ic);
                InstructionContext* result = instruction->executeAsync(channel->interactive(), this, ic);
                instruction->setContext(oldic);
                instruction->_isExecuting = false;
                return result;
            }
        }
        return nullptr;
    }

    void Device::deviceReceived(void* owner, void* sender, EventArgs* args)
    {
        Device* device = static_cast<Device*>(owner);
        if(device != nullptr)
        {
            device->_receivedDelegates.invoke(sender, args);
        }
    }
    void Device::addReceiveDevice(Device* device)
    {
        Locker locker(&_receiveDevicesMutex);
        
        assert(device);
        if(!_receiveDevices.contains(device))
        {
            _receiveDevices.add(device);
            
            for (uint32_t i=0; i<_instructions->count(); i++)
            {
                device->setReceiveInstruction(_instructions->at(i));
            }
            
            device->_receivedDelegates.add(Delegate(this, deviceReceived));
        }
    }
    void Device::setReceiveInstruction(Instruction* instruction)
    {
        for (uint32_t i=0; i<_instructions->count(); i++)
        {
            Instruction* ri = _instructions->at(i);
            if(instruction->description()->name() == ri->description()->name())
            {
                instruction->setReceiveInstruction(ri);
                break;
            }
        }
    }
    void Device::removeReceiveDevice(Device* device)
    {
        Locker locker(&_receiveDevicesMutex);
        
        assert(device);
        device->_receivedDelegates.remove(Delegate(this, deviceReceived));
        
        _receiveDevices.remove(device);
    }
    
    void Device::setAllowLog(bool allowInformation, bool allowMessage)
    {
        setAllowLog(_instructions, allowInformation, allowMessage);
        
        Locker locker(&_dynamicInstructionsMutex);
        if(_dynamicInstructions != nullptr)
        {
            return setAllowLog(_dynamicInstructions, allowInformation, allowMessage);
        }
    }
    
    void Device::addDynamicInstructions(Instructions* instructions)
    {
        Locker locker(&_dynamicInstructionsMutex);
        if(_dynamicInstructions != nullptr)
        {
            // The dynamic instructions maybe be used
            _dynamicInstructions->setAutoDelete(false);
            _deletedInstructionsMutex.lock();
            _deletedInstructions.addRange(*_dynamicInstructions);
            _deletedInstructionsMutex.unlock();
            delete _dynamicInstructions;
            _dynamicInstructions = nullptr;
        }
        _dynamicInstructions = instructions;
    }
    void Device::clearDynamicInstructions()
    {
        addDynamicInstructions(nullptr);
    }
    void Device::clearDynamicInstructionsInner()
    {
        const TimeSpan timeout = TimeSpan::fromSeconds(6);     // 6 seconds.
        if(TickTimeout::isTimeout(_deletedTick, timeout))
        {
            _deletedTick = TickTimeout::getCurrentTickCount();
            
            Locker locker(&_deletedInstructionsMutex);
            if(_deletedInstructions.count() > 0)
            {
                size_t count = 0;
                for (uint32_t i=0; i<_deletedInstructions.count(); i++)
                {
                    if(!_deletedInstructions[i]->_isExecuting)
                    {
                        count++;
                    }
                }
                if(count == _deletedInstructions.count())
                {
                    _deletedInstructions.clear();
                }
            }
        }
    }
    
    Instruction* Device::getInstruction(Instructions* instructions, const String& instructionName)
    {
        for (uint32_t i = 0; i < instructions->count(); i++)
        {
            Instruction* instruction = instructions->at(i);
            if(instructionName == instruction->description()->name())
            {
                return instruction;
            }
        }
        return nullptr;
    }
    Instruction* Device::matchInstruction(Instructions* instructions, const ByteArray& buffer)
    {
        for (uint32_t i = 0; i < instructions->count(); i++)
        {
            Instruction* instruction = instructions->at(i);
            if(instruction->match(&buffer, description()))
            {
                return instruction;
            }
        }
        return nullptr;
    }    
    void Device::setAllowLog(Instructions* instructions, bool allowInformation, bool allowMessage)
    {
        for (uint32_t i = 0; i < instructions->count(); i++)
        {
            Instruction* instruction = instructions->at(i);
            instruction->setAllowLog(allowInformation, allowMessage);
        }
    }
    
    String Device::getStatusStr(const Status status)
    {
        switch (status)
        {
            case Unknown:
                return "Unknown";
            case Offline:
                return "Offline";
            case Online:
                return "Online";
            case Disabled:
                return "Disabled";
            default:
                break;
        }
        return "Unknown";
    }
    bool Device::isStatusChanged(Status oldStatus, Status newStatus)
    {
        return ((newStatus == Device::Online && (oldStatus == Device::Offline || oldStatus == Device::Unknown)) ||
                (newStatus == Device::Offline && oldStatus == Device::Online));
    }
    
    Delegates* Device::receivedDelegates()
    {
        return &_receivedDelegates;
    }
    
    Instructions* Device::instructions()
    {
        return _instructions;
    }

    DeviceStatusEventArgs::DeviceStatusEventArgs(const String& deviceName, Device::Status oldStatus, Device::Status newStatus)
    {
        this->deviceName = deviceName;
        this->oldStatus = oldStatus;
        this->newStatus = newStatus;
    }
    DeviceStatusEventArgs::DeviceStatusEventArgs(const DeviceStatusEventArgs& args)
    {
        this->deviceName = args.deviceName;
        this->oldStatus = args.oldStatus;
        this->newStatus = args.newStatus;
    }
    DeviceStatusEventArgs::~DeviceStatusEventArgs()
    {
    }
}
