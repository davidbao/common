#include "thread/TickTimeout.h"
#include "data/Convert.h"
#include "thread/Thread.h"
#include "data/Vector.h"
#include "diag/Trace.h"
#include "diag/Trace.h"
#include "system/Math.h"
#include "diag/Stopwatch.h"
#include "thread/ThreadPool.h"
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

using namespace Data;

namespace Drivers
{
    ChannelDescription::ChannelDescription(const String& name, const String& interactiveName) : _name(name), _interactiveName(interactiveName), _interactive(nullptr)
    {
        _name = name;
        _interactiveName = interactiveName;
        _context = NULL;
        _interactive = NULL;
        _iType = InteractiveType::ITSender;
        _enabled = true;
        
        if(_interactiveName == "TcpInteractive")
        {
            _context = new TcpClientChannelContext();
        }
        else if(_interactiveName == "TcpServerInteractive")
        {
            _context = new TcpServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
        else if(_interactiveName == "TcpSSLInteractive")
        {
            _context = new TcpClientChannelContext();
        }
        else if(_interactiveName == "TcpSSLServerInteractive")
        {
            _context = new TcpSSLServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
        else if(_interactiveName == "WebSocketServerInteractive")
        {
            _context = new TcpServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
        else if(_interactiveName == "WebSocketSSLServerInteractive")
        {
            _context = new TcpSSLServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
        else if(_interactiveName == "UdpInteractive")
        {
            _context = new UdpChannelContext();
        }
        else if(_interactiveName == "UdpServerInteractive")
        {
            _context = new UdpServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
#ifndef __EMSCRIPTEN__
        else if(_interactiveName == "SerialInteractive")
        {
            _context = new SerialChannelContext(name);
        }
        else if (_interactiveName == "SerialServerInteractive")
        {
            _context = new SerialChannelContext(name);
            _iType = InteractiveType::ITReceiver;
        }
        else if(_interactiveName == "ParallelInteractive")
        {
            _context = new ParallelChannelContext(name);
        }
        else if (_interactiveName == "BluetoothInteractive")
        {
            _context = new BluetoothChannelContext();
        }
        else if (_interactiveName == "BluetoothServerInteractive")
        {
            _context = new BluetoothServerChannelContext();
            _iType = InteractiveType::ITReceiver;
        }
#endif
        else
        {
            throw NotImplementedException("Can not create a channel context.");
        }
    }
    ChannelDescription::ChannelDescription(const String& name, const String& interactiveName, InteractiveType iType) : ChannelDescription(name, interactiveName)
    {
        _iType = iType;
    }
    ChannelDescription::ChannelDescription(const String& name, ChannelContext* context, Interactive* interactive, InteractiveType iType)
    {
        _name = name;
        _interactiveName = String::Empty;
        _context = context;
        _interactive = interactive;
        _iType = iType;
        _enabled = true;
    }
    
    ChannelDescription::~ChannelDescription()
    {
        delete _context;
        _context = NULL;
        _interactive = NULL;    // deleted by Channel
    }
    
    void ChannelDescription::setName(const String& name)
    {
        _name = name;
    }
    
    const String& ChannelDescription::name() const
    {
        return _name;
    }
    
    ChannelContext* ChannelDescription::context() const
    {
        return _context;
    }
    const String& ChannelDescription::interactiveName() const
    {
        return _interactiveName;
    }
    String ChannelDescription::clientInteractiveName() const
    {
        if(isReceiver() || isSenderAndReceiver())
        {
            if(_interactiveName == "TcpServerInteractive")
            {
                return "TcpInteractive";
            }
            else if(_interactiveName == "TcpSSLServerInteractive")
            {
                return "TcpSSLInteractive";
            }
            else if(_interactiveName == "WebSocketServerInteractive")
            {
                return "TcpInteractive";
            }
            else if(_interactiveName == "WebSocketSSLServerInteractive")
            {
                return "TcpSSLInteractive";
            }
            else if(_interactiveName == "UdpServerInteractive")
            {
                return "UdpInteractive";
            }
            else if (_interactiveName == "BluetoothServerInteractive")
            {
                return "BluetoothInteractive";
            }
            else
            {
                throw NotImplementedException("Can not get a interactive name.");
            }
        }
        else
        {
            return interactiveName();
        }
    }
    
    InteractiveType ChannelDescription::interactiveType() const
    {
        return _iType;
    }
    bool ChannelDescription::isSender() const
    {
        return _iType == InteractiveType::ITSender;
    }
    bool ChannelDescription::isReceiver() const
    {
        return _iType == InteractiveType::ITReceiver;
    }
    bool ChannelDescription::isSenderAndReceiver() const
    {
        return _iType == InteractiveType::ITSenderAndReceiver;
    }
    bool ChannelDescription::isSampler() const
    {
        return isSender() || isSenderAndReceiver();
    }
    
    bool ChannelDescription::enabled() const
    {
        return _enabled;
    }
    void ChannelDescription::setEnabled(bool enabled)
    {
        _enabled = enabled;
    }
    
    Interactive* ChannelDescription::interactive() const
    {
        return _interactive;
    }
}
