#ifndef UDPSERVERCHANNELCONTEXT_H
#define UDPSERVERCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "EthernetContext.h"

namespace Drivers
{
	class UdpServerChannelContext : public ChannelContext, public EthernetContext
	{
	public:
        UdpServerChannelContext() : UdpServerChannelContext(Endpoint::Empty)
		{
		}
		UdpServerChannelContext(const Endpoint& endpoint) : EthernetContext(endpoint), _peek(false), _recvBufferSize(0)
		{
		}
		~UdpServerChannelContext() override
		{
		}
        
		inline const bool peek() const
		{
			return _peek;
		}
		inline void setPeek(bool peek)
		{
			_peek = peek;
		}
        
        inline int receiveBufferSize() const
        {
            return _recvBufferSize;
        }
        inline void setReceiveBufferSize(int bufferSize)
        {
            static const int minValue = 128;
            static const int maxValue = 65507;
            if(bufferSize >= minValue && bufferSize <= maxValue)
                _recvBufferSize = bufferSize;
            else if(bufferSize > maxValue)
                _recvBufferSize = maxValue;
        }

	private:
        bool _peek;
        int _recvBufferSize;
	};
}

#endif // UDPSERVERCHANNELCONTEXT_H
