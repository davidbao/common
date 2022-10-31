#include "driver/channels/TcpServerChannelContext.h"

using namespace Common;

namespace Drivers
{
    TcpServerChannelContext::TcpServerChannelContext() : TcpServerChannelContext(Endpoint::Empty)
    {
    }
    TcpServerChannelContext::TcpServerChannelContext(const Endpoint& endpoint, int maxConnections) : TcpChannelContext(endpoint), _maxConnections(maxConnections), _closeTimeout(0, 1, 0)
    {
    }
    TcpServerChannelContext::~TcpServerChannelContext()
    {
    }

    int TcpServerChannelContext::maxConnections() const
    {
        return _maxConnections;
    }
    void TcpServerChannelContext::setMaxConnections(int maxConnections)
    {
        _maxConnections = maxConnections;
    }
    
    const TimeSpan& TcpServerChannelContext::closeTimeout() const
    {
        return _closeTimeout;
    }
    void TcpServerChannelContext::setCloseTimeout(const TimeSpan& timeout)
    {
        _closeTimeout = timeout;
    }
    void TcpServerChannelContext::setCloseTimeout(uint milliSeconds)
    {
        _closeTimeout = TimeSpan::fromMilliseconds((double)milliSeconds);
    }

    void TcpServerChannelContext::copyFrom(const TcpChannelContext* context)
    {
        TcpChannelContext::copyFrom(context);
        
        const TcpServerChannelContext* tc = (const TcpServerChannelContext*)context;
        _maxConnections = tc->_maxConnections;
        _closeTimeout = tc->_closeTimeout;
    }

    TcpSSLServerChannelContext::TcpSSLServerChannelContext() : TcpSSLServerChannelContext(Endpoint::Empty)
    {
    }
    TcpSSLServerChannelContext::TcpSSLServerChannelContext(const Endpoint& endpoint, int maxConnections) : TcpServerChannelContext(endpoint, maxConnections)
    {
    }
    TcpSSLServerChannelContext::~TcpSSLServerChannelContext()
    {
    }

    const String& TcpSSLServerChannelContext::certFile() const
    {
        return _certFile;
    }
    void TcpSSLServerChannelContext::setCertFile(const String& certFile)
    {
        _certFile = certFile;
    }
    const String& TcpSSLServerChannelContext::keyFile() const
    {
        return _keyFile;
    }
    void TcpSSLServerChannelContext::setKeyFile(const String& keyFile)
    {
        _keyFile = keyFile;
    }
    const String& TcpSSLServerChannelContext::cacertFile() const
    {
        return _cacertFile;
    }
    void TcpSSLServerChannelContext::setCacertFile(const String& cacertFile)
    {
        _cacertFile = cacertFile;
    }
    
    void TcpSSLServerChannelContext::copyFrom(const TcpChannelContext* context)
    {
        TcpServerChannelContext::copyFrom(context);
        
        const TcpSSLServerChannelContext* tc = (const TcpSSLServerChannelContext*)context;
        _certFile = tc->_certFile;
        _keyFile = tc->_keyFile;
        _cacertFile = tc->_cacertFile;
    }
}
