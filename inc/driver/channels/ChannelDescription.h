#ifndef CHANNELDESCRIPTION_H
#define CHANNELDESCRIPTION_H

#include "exception/Exception.h"
#include "../IContextProperty.h"
#include "TcpChannelContext.h"
#include "TcpServerChannelContext.h"
#include "SerialChannelContext.h"
#include "ParallelChannelContext.h"
#include "UdpServerChannelContext.h"
#include "UdpChannelContext.h"
#include "BluetoothServerChannelContext.h"

using namespace Common;

namespace Drivers
{
    class Interactive;
    class ChannelDescription
    {
    public:
        ChannelDescription(const String& name, const String& interactiveName);
        ChannelDescription(const String& name, const String& interactiveName, InteractiveType iType);
        ChannelDescription(const String& name, ChannelContext* context, Interactive* interactive, InteractiveType iType = InteractiveType::ITSender);
        
        virtual ~ChannelDescription();
        
        void setName(const String& name);
        
        const String& name() const;
        
        ChannelContext* context() const;
        const String& interactiveName() const;
        String clientInteractiveName() const;
        
        InteractiveType interactiveType() const;
        bool isSender() const;
        bool isReceiver() const;
        bool isSenderAndReceiver() const;
        bool isSampler() const;
        
        bool enabled() const;
        void setEnabled(bool enabled);
        
        Interactive* interactive() const;
        
    private:
        String _name;
        String _interactiveName;
        ChannelContext* _context;
        InteractiveType _iType;
        bool _enabled;
        Interactive* _interactive;
    };
}
#endif // CHANNELDESCRIPTION_H
