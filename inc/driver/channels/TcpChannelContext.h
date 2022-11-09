#ifndef TCPCHANNELCONTEXT_H
#define TCPCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "data/Convert.h"
#include "data/TimeSpan.h"
#include "EthernetContext.h"

using namespace Common;

namespace Drivers
{
	class TcpChannelContext : public ChannelContext, public EthernetContext
	{
	public:
        enum Types
        {
            Sync = 0,
            Async = 1,
            MultiPlexing = 2
        };
        
        TcpChannelContext();
        TcpChannelContext(const Endpoint& endpoint);
        ~TcpChannelContext() override;
        
        virtual const TimeSpan& closeTimeout() const = 0;

        int sendBufferSize() const;
        void setSendBufferSize(int bufferSize);
        int receiveBufferSize() const;
        void setReceiveBufferSize(int bufferSize);
        
        const TimeSpan& sendTimeout() const;
        void setSendTimeout(uint timeout);
        void setSendTimeout(TimeSpan timeout);
        
        const TimeSpan& receiveTimeout() const;
        void setReceiveTimeout(uint timeout);
        void setReceiveTimeout(TimeSpan timeout);
        
        bool noDelay() const;
        void setNoDelay(bool noDelay = true);
        
        bool reuseAddress() const;
        void setReuseAddress(bool reuseAddress);
        
        bool isBlocking() const;
        void setBlocking(bool blocking = false);
        
        virtual void copyFrom(const TcpChannelContext* context);
        
        bool asyncReceiver() const;
        bool syncReceiver() const;
        bool multiPlexingReceiver() const;
        void setReceiverType(Types type);
        
        bool asyncSender() const;
        bool syncSender() const;
        bool multiPlexingSender() const;
        void setSenderType(Types type);
        
        bool backgroundReceiver() const;
        void setBackgroundReceiver(bool backgroundReceiver);
        
    public:
        static Types parseTypeStr(const String& str);
        static String toTypeStr(Types type);

	protected:
        int _recvBufferSize;
		int _sendBufferSize;
        
        TimeSpan _sendTimeout;
        TimeSpan _recvTimeout;

		bool _noDelay;
        
        bool _reuseAddress;
        bool _blocking;
        
        Types _receiverType;
        Types _senderType;
        bool _backgroundReceiver;
	};
    
    class TcpClientChannelContext : public TcpChannelContext
    {
    public:
        TcpClientChannelContext();
        TcpClientChannelContext(const Endpoint& endpoint);
        ~TcpClientChannelContext() override;
        
        const TimeSpan& closeTimeout() const override;
        
        const TimeSpan& openTimeout() const;
        void setOpenTimeout(uint timeout);
        void setOpenTimeout(TimeSpan timeout);
        
        void copyFrom(const TcpChannelContext* context) override;
        
    private:
        TimeSpan _openTimeout;
    };
    
    class SSLChannelContext : public TcpClientChannelContext
    {
    public:
        SSLChannelContext();
        SSLChannelContext(const Endpoint& endpoint);
        ~SSLChannelContext() override;
        
        const bool peek() const;
        void setPeek(bool peek);
        
    private:
        bool _peek;
    };
}

#endif // TCPCHANNELCONTEXT_H
