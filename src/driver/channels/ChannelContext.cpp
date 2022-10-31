#include "driver/channels/ChannelContext.h"

using namespace Common;

namespace Drivers
{
    ChannelContext::ChannelContext() : _useReceiveTimeout(false), _reopened(true), _deviceInterval(TimeSpan::fromMilliseconds(100))
    {
    }
    ChannelContext::~ChannelContext()
    {
    }

    bool ChannelContext::useReceiveTimeout() const
    {
        return _useReceiveTimeout;
    }
    void ChannelContext::setUseReceiveTimeout(bool useReceiveTimeout)
    {
        _useReceiveTimeout = useReceiveTimeout;
    }

    bool ChannelContext::reopened() const
    {
        return _reopened;
    }
    void ChannelContext::setReopened(bool reopened)
    {
        _reopened = reopened;
    }

    const TimeSpan& ChannelContext::deviceInterval() const
    {
        return _deviceInterval;
    }
    void ChannelContext::setDeviceInterval(const TimeSpan& deviceInterval)
    {
        _deviceInterval = deviceInterval;
    }

    String ChannelContext::toInteractiveTypeStr(InteractiveType type)
    {
        switch (type)
        {
            case InteractiveType::ITSender:
                return "Sender";
            case InteractiveType::ITReceiver:
                return "Sender";
            case InteractiveType::ITSenderAndReceiver:
                return "SenderAndReceiver";
            default:
                return "Unknown";
        }
    }
    InteractiveType ChannelContext::parseInteractiveType(const String& str)
    {
        if(String::equals(str, "Sender", true))
        {
            return InteractiveType::ITSender;
        }
        else if(String::equals(str, "Receiver", true))
        {
            return InteractiveType::ITReceiver;
        }
        else if(String::equals(str, "SenderAndReceiver", true))
        {
            return InteractiveType::ITSenderAndReceiver;
        }
        return InteractiveType::ITUnknown;
    }
}
