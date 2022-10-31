#include "diag/Trace.h"
#include "data/ByteArray.h"
#include "driver/channels/Interactive.h"
#include "driver/devices/Device.h"

namespace Drivers
{
    Interactive::Interactive(DriverManager* dm, Channel* channel)
    {
        if(dm == NULL)
            throw ArgumentNullException("dm");
        
        _manager = dm;
        _channel = channel;
        _useReceiveTimeout = false;
    }
    Interactive::~Interactive()
    {
        _channel = NULL;
    }
    bool Interactive::isClosing() const
    {
        return false;
    }
    ssize_t Interactive::receive(uint8_t* buffer, off_t offset, size_t count, uint timeout)
    {
        if(timeout == 0)
        {
            return receive(buffer, offset, count);
        }
        // use it if useReceiveTimeout() return true;
        throw NotImplementedException("Can not implement this method.");
    }
    ssize_t Interactive::receive(ByteArray* buffer, size_t count, uint timeout)
    {
        if(timeout == 0)
        {
            uint8_t* temp = new uint8_t[count];
            ssize_t readCount = receive(temp, 0, count);
            if(readCount > 0)
            {
                buffer->addRange(temp, readCount);
            }
            delete[] temp;
            return readCount;
        }
        // use it if useReceiveTimeout() return true;
        throw NotImplementedException("Can not implement this method.");
    }
    
    Channel* Interactive::channel() const
    {
        return _channel;
    }
    void Interactive::setChannel(Channel* channel)
    {
        _channel = channel;
    }
    DriverManager* Interactive::manager() const
    {
        return _manager;
    }
    
    EthernetEndpoint::~EthernetEndpoint()
    {
    }
    const P2PEndpoint EthernetEndpoint::p2pEndpoint() const
    {
        return P2PEndpoint(endpoint(), peerEndpoint());
    }
    
    BackgroudReceiver::~BackgroudReceiver()
    {
    }
    bool BackgroudReceiver::receiveFromBuffer(Device* device)
    {
        if(!device)
            return false;
        
        // receive & match & execute
        return device->executeInstruction();
    }
}
