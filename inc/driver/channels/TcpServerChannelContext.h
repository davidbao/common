#ifndef TCPSERVERCHANNELCONTEXT_H
#define TCPSERVERCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "data/TimeSpan.h"
#include "TcpChannelContext.h"

using namespace Common;

namespace Drivers
{
	class TcpServerChannelContext : public TcpChannelContext
	{
	public:
        TcpServerChannelContext();
        TcpServerChannelContext(const Endpoint& endpoint, int maxConnections = 65535);
        ~TcpServerChannelContext() override;

        int maxConnections() const;
        void setMaxConnections(int maxConnections);
        
        const TimeSpan& closeTimeout() const override;
        void setCloseTimeout(const TimeSpan& timeout);
        void setCloseTimeout(uint32_t milliSeconds);
        
        void copyFrom(const TcpChannelContext* context) override;

	private:
		int _maxConnections;
        TimeSpan _closeTimeout;
        
	};
    
    class TcpSSLServerChannelContext : public TcpServerChannelContext
    {
    public:
        TcpSSLServerChannelContext();
        TcpSSLServerChannelContext(const Endpoint& endpoint, int maxConnections = 5000);
        ~TcpSSLServerChannelContext() override;
        
        const String& certFile() const;
        void setCertFile(const String& certFile);
        const String& keyFile() const;
        void setKeyFile(const String& keyFile);
        const String& cacertFile() const;
        void setCacertFile(const String& cacertFile);
        
        void copyFrom(const TcpChannelContext* context) override;
        
    private:
        String _certFile;
        String _keyFile;
        String _cacertFile;
    };
}

#endif // TCPSERVERCHANNELCONTEXT_H
