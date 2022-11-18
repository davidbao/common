#include "thread/TickTimeout.h"
#include "data/Convert.h"
#include "thread/Thread.h"
#include "data/Vector.h"
#include "diag/Trace.h"
#include "thread/ThreadPool.h"
#include "driver/channels/Channel.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/channels/TcpInteractive.h"
#include "driver/channels/TcpServerInteractive.h"
#include "driver/channels/UdpServerInteractive.h"
#include "driver/channels/UdpInteractive.h"
#include "driver/channels/MockInteractive.h"
#ifndef __EMSCRIPTEN__
#include "driver/channels/SerialInteractive.h"
#include "driver/channels/SerialServerInteractive.h"
#include "driver/channels/ParallelInteractive.h"
#include "driver/channels/BluetoothInteractive.h"
#include "driver/channels/BluetoothServerInteractive.h"
#endif

using namespace Common;

namespace Drivers
{
    Channel::Channel(DriverManager* dm, ChannelDescription* description) : _interactive(nullptr), _mock(nullptr)
    {
        if(dm == nullptr)
            throw ArgumentNullException("dm");
        
        _description = description;
        _manager = dm;
        _opened = false;
        _opening = false;
        
        const String& iName = _description->interactiveName();
        if(iName == "TcpInteractive")
        {
            _interactive = new TcpInteractive(dm, this);
        }
        else if(iName == "TcpServerInteractive")
        {
            _interactive = new TcpServerInteractive(dm, this);
        }
        else if(iName == "UdpInteractive")
        {
            _interactive = new UdpInteractive(dm, this);
        }
        else if(iName == "UdpServerInteractive")
        {
            _interactive = new UdpServerInteractive(dm, this);
        }
#ifndef __EMSCRIPTEN__
        else if(iName == "SerialInteractive")
        {
            _interactive = new SerialInteractive(dm, this);
        }
        else if (iName == "SerialServerInteractive")
        {
            _interactive = new SerialServerInteractive(dm, this);
        }
        else if(iName == "ParallelInteractive")
        {
            _interactive = new ParallelInteractive(dm, this);
        }
        else if(iName == "BluetoothInteractive")
        {
            _interactive = new BluetoothInteractive(dm, this);
        }
        else if(iName == "BluetoothServerInteractive")
        {
            _interactive = new BluetoothServerInteractive(dm, this);
        }
        else if(iName == "WebSocketServerInteractive")
        {
            _interactive = new WebSocketServerInteractive(dm, this);
        }
        else if(iName == "WebSocketSSLServerInteractive")
        {
            _interactive = new WebSocketSSLServerInteractive(dm, this);
        }
        else if(iName == "TcpSSLInteractive")
        {
            _interactive = new TcpSSLInteractive(dm, this);
        }
        else if(iName == "TcpSSLServerInteractive")
        {
            _interactive = new TcpSSLServerInteractive(dm, this);
        }
#endif
        else
        {
            Interactive* interactive = _description->interactive();
            if(interactive != nullptr)
            {
                interactive->setChannel(this);
                _interactive = interactive;
            }
            else
            {
                throw NotImplementedException(String::convert("Can not create a interactive. name: %s", iName.c_str()));
            }
        }
    }
    
    Channel::~Channel()
    {
        ThreadPool::stop(openAction);
        ThreadPool::stop(reopenAction);
        
        _description = nullptr;
        if(_interactive != nullptr)
        {
            delete _interactive;
            _interactive = nullptr;
        }
        if(_mock != nullptr)
        {
            delete _mock;
            _mock = nullptr;
        }
    }
    
    bool Channel::connected()
    {
        Interactive* i = interactive();
        return i != nullptr ? i->connected() : false;
    }
    bool Channel::useReceiveTimeout() const
    {
        return context()->useReceiveTimeout();
    }
    size_t Channel::available()
    {
        Interactive* i = interactive();
        return i != nullptr ? i->available() : 0;
    }
    
    ssize_t Channel::send(const uint8_t* buffer, off_t offset, size_t count)
    {
        Interactive* i = interactive();
        return i != nullptr ? i->send(buffer, offset, count) : 0;
    }
    ssize_t Channel::receive(uint8_t* buffer, off_t offset, size_t count)
    {
        Interactive* i = interactive();
        return i != nullptr ? i->receive(buffer, offset, count) : 0;
    }
    ssize_t Channel::receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout)
    {
        Interactive* i = interactive();
        return i != nullptr ? i->receive(buffer, offset, count, timeout) : 0;
    }
    ssize_t Channel::receive(ByteArray* buffer, size_t count, uint32_t timeout)
    {
        Interactive* i = interactive();
        return i != nullptr ? i->receive(buffer, count, timeout) : 0;
    }
    
    bool Channel::open()
    {
        _opening = true;
        bool result = false;
        if(_description->enabled())
        {
            if(_interactive != nullptr)
            {
                result = _interactive->open();
            }
            
            OpenedEventArgs e(result);
            _openedDelegates.invoke(this, &e);
            
//            Trace::writeLine(String::convert("Open a channel'%s'!", name().c_str()), Trace::Info);
        }
        _opened = true;
        _opening = false;
        return result;
    }
    void Channel::openAction(ThreadHolder* holder)
    {
        Channel* channel = static_cast<Channel*>(holder->owner);
        assert(channel);
        
        try
        {
            channel->open();
        }
        catch (const Exception&)
        {
        }
        
        delete holder;
    }
    void Channel::openAsync()
    {
        if(!_opening)
        {
            Debug::writeFormatLine("Channel::openAsync");
            ThreadPool::startAsync(openAction, this);
        }
    }
    bool Channel::isOpened(void* parameter)
    {
        Channel* channel = (Channel*)parameter;
        return !channel->_opening;
    }
    void Channel::close()
    {
        ThreadPool::stop(openAction);
        ThreadPool::stop(reopenAction);
        
        if(_opening)
        {
            // Waiting for opened.
            TickTimeout::msdelay((uint32_t)-1, isOpened, (void*)this);
        }
        
        _opened = false;
        if(_interactive != nullptr)
        {
            _interactive->close();
        }
        
        EventArgs e;
        _closedDelegates.invoke(this, &e);
        
//        Trace::writeLine(String::convert("Close a channel'%s'!", name().c_str()), Trace::Info);
    }
    bool Channel::reopen()
    {
//        Trace::writeLine(String::convert("Reopen a channel'%s'!", name().c_str()), Trace::Info);
        
        close();
        return open();
    }
    void Channel::reopenAction(ThreadHolder* holder)
    {
        Channel* channel = static_cast<Channel*>(holder->owner);
        assert(channel);
        
        try
        {
            channel->reopen();
        }
        catch (const Exception&)
        {
        }
        
        delete holder;
    }
    void Channel::reopenAsync()
    {
        if(!_opening)
        {
            Debug::writeFormatLine("Channel::reopenAsync");
            ThreadPool::startAsync(reopenAction, this);
        }
    }
    
    bool Channel::reopened() const
    {
        if(ThreadPool::isAlive(openAction))
        {
            // openning...
            return false;
        }
        return context()->reopened();
    }
    
    bool Channel::opened() const
    {
        return _opened;
    }
    
    const String& Channel::name() const
    {
        return _description != nullptr ? _description->name() : String::Empty;
    }
    ChannelDescription* Channel::description() const
    {
        return _description;
    }
    ChannelContext* Channel::context() const
    {
        return _description != nullptr ? _description->context() : nullptr;
    }

    bool Channel::enabled() const
    {
        return _description->enabled();
    }
    
    DriverManager* Channel::manager()
    {
        return _manager;
    }
    
    Interactive* Channel::interactive() const
    {
        return _mock == nullptr ? _interactive : _mock;
    }
    void Channel::updateMockRecvBuffer(const ByteArray& buffer)
    {
        if(_mock == nullptr)
        {
            _mock = new MockInteractive(_manager, this);
        }
        _mock->updateRecvBuffer(buffer);
    }
    
    Delegates* Channel::openedDelegates()
    {
        return &_openedDelegates;
    }

    Delegates* Channel::closedDelegates()
    {
        return &_closedDelegates;
    }
}
