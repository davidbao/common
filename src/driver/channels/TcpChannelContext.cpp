#include "driver/channels/TcpChannelContext.h"

using namespace Common;

namespace Drivers
{
    TcpChannelContext::TcpChannelContext() : TcpChannelContext(Endpoint::Empty)
    {
    }
    TcpChannelContext::TcpChannelContext(const Endpoint& endpoint) : EthernetContext(endpoint)
    {
        _recvBufferSize = 0;
        _sendBufferSize = 0;
        
        _noDelay = false;
        
        _reuseAddress = false;
        _blocking = false;
        
        _receiverType = Types::MultiPlexing;
        _senderType = Types::MultiPlexing;
        _backgroundReceiver = true;
    }
    TcpChannelContext::~TcpChannelContext()
    {
    }

    int TcpChannelContext::sendBufferSize() const
    {
        return _sendBufferSize;
    }
    void TcpChannelContext::setSendBufferSize(int bufferSize)
    {
        _sendBufferSize = bufferSize;
    }
    int TcpChannelContext::receiveBufferSize() const
    {
        return _recvBufferSize;
    }
    void TcpChannelContext::setReceiveBufferSize(int bufferSize)
    {
        _recvBufferSize = bufferSize;
    }

    const TimeSpan& TcpChannelContext::sendTimeout() const
    {
        return _sendTimeout;
    }
    void TcpChannelContext::setSendTimeout(uint32_t timeout)
    {
        if(timeout > 0)
        {
            _sendTimeout = TimeSpan::fromMilliseconds(timeout);
        }
    }
    void TcpChannelContext::setSendTimeout(TimeSpan timeout)
    {
        if(timeout != TimeSpan::Zero)
        {
            _sendTimeout = timeout;
        }
    }

    const TimeSpan& TcpChannelContext::receiveTimeout() const
    {
        return _recvTimeout;
    }
    void TcpChannelContext::setReceiveTimeout(uint32_t timeout)
    {
        if(timeout > 0)
        {
            _recvTimeout = TimeSpan::fromMilliseconds(timeout);
        }
    }
    void TcpChannelContext::setReceiveTimeout(TimeSpan timeout)
    {
        if(timeout != TimeSpan::Zero)
        {
            _recvTimeout = timeout;
        }
    }

    bool TcpChannelContext::noDelay() const
    {
        return _noDelay;
    }
    void TcpChannelContext::setNoDelay(bool noDelay)
    {
        _noDelay = noDelay;
    }
    
    bool TcpChannelContext::reuseAddress() const
    {
        return _reuseAddress;
    }
    void TcpChannelContext::setReuseAddress(bool reuseAddress)
    {
        _reuseAddress = reuseAddress;
    }
    
    bool TcpChannelContext::isBlocking() const
    {
        return _blocking;
    }
    void TcpChannelContext::setBlocking(bool blocking)
    {
        _blocking = blocking;
    }
    
    void TcpChannelContext::copyFrom(const TcpChannelContext* context)
    {
        _useReceiveTimeout = context->_useReceiveTimeout;
        _reopened = context->_reopened;
        
        _endpoint = context->_endpoint;
        
        _recvBufferSize = context->_recvBufferSize;
        _sendBufferSize = context->_sendBufferSize;
        
        _sendTimeout = context->_sendTimeout;
        _recvTimeout = context->_recvTimeout;
        
        _noDelay = context->_noDelay;
        
        _reuseAddress = context->_reuseAddress;
        _blocking = context->_blocking;
        
        _receiverType = context->_receiverType;
        _senderType = context->_senderType;
        _backgroundReceiver = context->_backgroundReceiver;
    }
    
    bool TcpChannelContext::asyncReceiver() const
    {
        return _receiverType == Types::Async;
    }
    bool TcpChannelContext::syncReceiver() const
    {
        return _receiverType == Types::Sync;
    }
    bool TcpChannelContext::multiPlexingReceiver() const
    {
        return _receiverType == Types::MultiPlexing;
    }
    void TcpChannelContext::setReceiverType(Types type)
    {
        _receiverType = type;
    }
    
    bool TcpChannelContext::asyncSender() const
    {
        return _senderType == Types::Async;
    }
    bool TcpChannelContext::syncSender() const
    {
        return _senderType == Types::Sync;
    }
    bool TcpChannelContext::multiPlexingSender() const
    {
        return _senderType == Types::MultiPlexing;
    }
    void TcpChannelContext::setSenderType(Types type)
    {
        _senderType = type;
    }

    bool TcpChannelContext::backgroundReceiver() const
    {
        return asyncReceiver() || _backgroundReceiver;
    }
    void TcpChannelContext::setBackgroundReceiver(bool backgroundReceiver)
    {
        _backgroundReceiver = backgroundReceiver;
    }

    TcpChannelContext::Types TcpChannelContext::parseTypeStr(const String& str)
    {
        if(String::equals(str, "async", true))
            return Async;
        else if(String::equals(str, "sync", true))
            return Sync;
        else if(String::equals(str, "multiplexing", true))
            return MultiPlexing;
        else
            return MultiPlexing;
    }
    String TcpChannelContext::toTypeStr(Types type)
    {
        switch (type)
        {
            case Async:
                return "async";
            case Sync:
                return "sync";
            case MultiPlexing:
                return "multiplexing";
            default:
                return "multiplexing";
        }
    }

    TcpClientChannelContext::TcpClientChannelContext() : TcpChannelContext()
    {
        _openTimeout = TimeSpan::fromMilliseconds(3);
    }
    TcpClientChannelContext::TcpClientChannelContext(const Endpoint& endpoint) : TcpChannelContext(endpoint)
    {
        _openTimeout = TimeSpan::fromMilliseconds(3);
    }
    TcpClientChannelContext::~TcpClientChannelContext()
    {
    }

    const TimeSpan& TcpClientChannelContext::closeTimeout() const
    {
        return receiveTimeout();
    }
    
    const TimeSpan& TcpClientChannelContext::openTimeout() const
    {
        return _openTimeout;
    }
    void TcpClientChannelContext::setOpenTimeout(uint32_t timeout)
    {
        if(timeout > 0)
        {
            _openTimeout = TimeSpan::fromMilliseconds(timeout);
        }
    }
    void TcpClientChannelContext::setOpenTimeout(TimeSpan timeout)
    {
        if(timeout != TimeSpan::Zero)
        {
            _openTimeout = timeout;
        }
    }
    
    void TcpClientChannelContext::copyFrom(const TcpChannelContext* context)
    {
        TcpChannelContext::copyFrom(context);
        
        const TcpClientChannelContext* tc = (const TcpClientChannelContext*)context;
        _openTimeout = tc->_openTimeout;
    }

    SSLChannelContext::SSLChannelContext() : TcpClientChannelContext(), _peek(false)
    {
    }
    SSLChannelContext::SSLChannelContext(const Endpoint& endpoint) : TcpClientChannelContext(endpoint), _peek(false)
    {
    }
    SSLChannelContext::~SSLChannelContext()
    {
    }
    
    const bool SSLChannelContext::peek() const
    {
        return _peek;
    }
    void SSLChannelContext::setPeek(bool peek)
    {
        _peek = peek;
    }
}
