#ifndef UDPCHANNELCONTEXT_H
#define UDPCHANNELCONTEXT_H

#include "data/Convert.h"
#include "ChannelContext.h"
#include "EthernetContext.h"

using namespace Data;

namespace Drivers
{
	class UdpChannelContext : public ChannelContext, public EthernetContext
	{
	public:
        UdpChannelContext() : UdpChannelContext(Endpoint::Empty)
		{
		}
        UdpChannelContext(int port) : UdpChannelContext(Endpoint(String::Empty, port))
		{
		}
		UdpChannelContext(const Endpoint& endpoint) : EthernetContext(endpoint), _sendBufferSize(0)
		{
		}
		~UdpChannelContext() override
        {
        }
        
        inline int sendBufferSize() const
        {
            return _sendBufferSize;
        }
        inline void setSendBufferSize(int bufferSize)
        {
            static const int minValue = 128;
            static const int maxValue = 65507;
            if(bufferSize >= minValue && bufferSize <= maxValue)
                _sendBufferSize = bufferSize;
            else if(bufferSize > maxValue)
                _sendBufferSize = maxValue;
        }
        
    private:
        int _sendBufferSize;
	};
}

#endif // UDPCHANNELCONTEXT_H
