#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "driver/channels/MockInteractive.h"
#include "driver/channels/Channel.h"

namespace Drivers
{
    MockChannelContext::MockChannelContext()
    {
    }
    MockChannelContext::~MockChannelContext()
    {
    }
    
    MockInteractive::MockInteractive(DriverManager* dm, Channel* channel) : Interactive(dm, channel)
    {
        if(channel == nullptr)
            throw ArgumentException("channel");
        if((MockChannelContext*)channel->description()->context() == nullptr)
            throw ArgumentException("channel");
    }
    MockInteractive::~MockInteractive()
    {
        close();
    }
    bool MockInteractive::open()
    {
        return true;
    }
    void MockInteractive::close()
    {
        _stream.clear();
    }
    
    bool MockInteractive::connected()
    {
        return true;
    }
    size_t MockInteractive::available()
    {
        return (size_t)(_stream.length() - _stream.position());
    }

    ssize_t MockInteractive::send(const uint8_t* buffer, off_t offset, size_t count)
    {
#ifdef DEBUG
        Stopwatch sw("serial send", 1000);
#endif
        ssize_t len = 0;
        if(connected())
        {
            len = _stream.write(buffer, offset, count);
        }
        return len;
    }
    ssize_t MockInteractive::receive(uint8_t* buffer, off_t offset, size_t count)
    {
#ifdef DEBUG
        Stopwatch sw("serial receive", 1000);
#endif
        ssize_t len = 0;
        if(connected())
        {
            len = _stream.read(buffer, offset, count);
        }
        return len;
    }

    ssize_t MockInteractive::receive(uint8_t* buffer, off_t offset, size_t count, uint timeout)
    {
#ifdef DEBUG
        Stopwatch sw("serial receive2", 1000);
#endif
        ssize_t len = 0;
        if(connected())
        {
            len = receive(buffer, offset, count);
        }
        return len;
    }
    
    MockChannelContext* MockInteractive::getChannelContext()
    {
        return (MockChannelContext*)(_channel->description()->context());
    }
    
    void MockInteractive::updateRecvBuffer(const ByteArray& buffer)
    {
        _stream.clear();
        _stream.write(buffer.data(), 0, buffer.count());
        _stream.seek(0, SeekOrigin::SeekBegin);
    }
}
