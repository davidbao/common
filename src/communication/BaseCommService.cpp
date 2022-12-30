#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/Timer.h"
#include "driver/DriverManager.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "system/Resources.h"
#include "communication/BaseCommConfig.h"
#include "communication/BaseCommService.h"

using namespace Data;
using namespace Drivers;

namespace Communication
{
    ServiceBehaviour::ServiceBehaviour() : syncInitValue(true), senderType(SenderTypes::None)
    {
    }
    ServiceBehaviour::ServiceBehaviour(const ServiceBehaviour& value) : ServiceBehaviour()
    {
        this->operator=(value);
    }
    void ServiceBehaviour::operator=(const ServiceBehaviour& value)
    {
        this->syncInitValue = value.syncInitValue;
        this->senderType = value.senderType;
        this->includedServices = value.includedServices;
        this->excludedServices = value.excludedServices;
    }
    bool ServiceBehaviour::operator==(const ServiceBehaviour& value) const
    {
        return this->syncInitValue == value.syncInitValue &&
        this->senderType == value.senderType &&
        this->includedServices == value.includedServices &&
        this->excludedServices == value.excludedServices;
    }
    bool ServiceBehaviour::operator!=(const ServiceBehaviour& value) const
    {
        return !operator==(value);
    }
    bool ServiceBehaviour::isSender() const
    {
        return isAsyncSender() || isSyncSender();
    }
    bool ServiceBehaviour::isAsyncSender() const
    {
        return senderType == SenderTypes::Async;
    }
    bool ServiceBehaviour::isSyncSender() const
    {
        return senderType == SenderTypes::Sync;
    }
    bool ServiceBehaviour::isSyncInitValue() const
    {
        return this->syncInitValue;
    }
    bool ServiceBehaviour::containService(const String& name) const
    {
        if(excludedServices.count() > 0)
        {
            return !excludedServices.contains(name);
        }
        
        if(includedServices.count() > 0)
        {
            return includedServices.contains(name);
        }
        return true;
    }
    
    BaseCommService::InstructionCallback::InstructionCallback()
    {
        tcpInstructions = nullptr;
        tcpSampler = nullptr;
        udpSendInstructions = nullptr;
        udpRecvInstructions = nullptr;
        udpSampler = nullptr;
        owner = nullptr;
        asyncOpen = true;
    }
    BaseCommService::InstructionCallback::InstructionCallback(const BaseCommService::InstructionCallback& callback)
    {
        tcpInstructions = callback.tcpInstructions;
        tcpSampler = callback.tcpSampler;
        udpSendInstructions = callback.udpSendInstructions;
        udpRecvInstructions = callback.udpRecvInstructions;
        udpSampler = callback.udpSampler;
        owner = callback.owner;
        asyncOpen = callback.asyncOpen;
        behaviour = callback.behaviour;
    }
    bool BaseCommService::InstructionCallback::hasUdpInstructions() const
    {
        return udpSendInstructions != nullptr && udpRecvInstructions != nullptr;
    }
    
    BaseCommService::BaseLoopSender::BaseLoopSender(BaseCommService* service, const String& name, const TimeSpan& interval, const Endpoint& endpoint, int sendCount)
    {
        assert(service);
        _service = service;
        _interval = interval;
        _name = name;
        _sendCount = sendCount;
        
        _timer = nullptr;
        _endpoint = endpoint;
        _invokeCount = 0;
    }
    BaseCommService::BaseLoopSender::~BaseLoopSender()
    {
        stop();
    }
    const String& BaseCommService::BaseLoopSender::name() const
    {
        return _name;
    }
    void BaseCommService::BaseLoopSender::start()
    {
        assert(_timer == nullptr);
        _timer = new Timer("BaseLoopSenderTimer", timerCallback, this, _interval);
    }
    void BaseCommService::BaseLoopSender::stop()
    {
        if(_timer != nullptr)
        {
            _timer->stop(30);
            delete _timer;
            _timer = nullptr;
        }
    }
    bool BaseCommService::BaseLoopSender::canInvoke()
    {
        if(_sendCount == -1)
            return true;
        else
        {
            _invokeCount++;
            if(_invokeCount > _sendCount)
            {
                _timer->stop();
                return false;
            }
            return true;
        }
    }
    void BaseCommService::BaseLoopSender::timerCallback(void* state)
    {
        BaseCommService::BaseLoopSender* sender = static_cast<BaseCommService::BaseLoopSender*>(state);
        assert(sender);
        sender->invoke();
    }
    
    BaseCommService::BasePacketSender::BasePacketSender(BaseCommService* service, const String& name, const TimeSpan& interval)
    {
        assert(service);
        _service = service;
        _interval = interval;
        _name = name;
        
        _timer = nullptr;
    }
    BaseCommService::BasePacketSender::~BasePacketSender()
    {
        stop();
    }
    const String& BaseCommService::BasePacketSender::name() const
    {
        return _name;
    }
    void BaseCommService::BasePacketSender::start()
    {
        assert(_timer == nullptr);
        _timer = new Timer("BasePacketSenderTimer", timerCallback, this, _interval);
    }
    void BaseCommService::BasePacketSender::stop()
    {
        if(_timer != nullptr)
        {
            _timer->stop(30);
            delete _timer;
            _timer = nullptr;
        }
    }
    void BaseCommService::BasePacketSender::invoke()
    {
        // FIX Bug: Pure Virtual Function Called.
    }
    void BaseCommService::BasePacketSender::timerCallback(void* state)
    {
        BaseCommService::BasePacketSender* sender = static_cast<BaseCommService::BasePacketSender*>(state);
        assert(sender);
        sender->invoke();
    }
    
    BaseCommService::BaseCommService() : _instructionPool(nullptr)
    {
        _manager = new DriverManager();
    }
    
    BaseCommService::~BaseCommService()
    {
        unInitialize();
        
        delete _manager;
        _manager = nullptr;
    }
    
    bool BaseCommService::initialize(const InstructionCallback& callback)
    {
#if DEBUG
        Stopwatch sw("Construct the BaseCommService instance", 1000);
#endif
        
        try
        {
            createDevice(callback);
            
            _behaviour = callback.behaviour;

            if(callback.asyncOpen)
                _manager->openAsync();
            else
                _manager->open();
            
            return true;
        }
        catch (const Exception& e)
        {
            _error = e.message();
            return false;
        }
    }
    bool BaseCommService::unInitialize()
    {
        _instructionPool = nullptr;
        
        _loopSendersMutex.lock();
        for (uint32_t i=0; i<_loopSenders.count(); i++)
        {
            BaseLoopSender* sender = _loopSenders[i];
            sender->stop();
        }
        _loopSenders.clear();
        _loopSendersMutex.unlock();
        
        _packetSendersMutex.lock();
        for (uint32_t i=0; i<_packetSenders.count(); i++)
        {
            BasePacketSender* sender = _packetSenders[i];
            sender->stop();
        }
        _packetSenders.clear();
        _packetSendersMutex.unlock();
        
        if(_manager != nullptr)
        {
            _manager->close();
            return true;
        }
        return false;
    }
    
    DriverManager* BaseCommService::manager() const
    {
        return _manager;
    }
    
    const ServiceBehaviour& BaseCommService::behaviour() const
    {
        return _behaviour;
    }
    void BaseCommService::setServiceBehaviour(const ServiceBehaviour& behaviour)
    {
        _behaviour = behaviour;
    }
}
