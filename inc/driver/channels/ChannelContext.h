#ifndef CHANNELCONTEXT_H
#define CHANNELCONTEXT_H

#include "data/TimeSpan.h"
#include "driver/Context.h"

using namespace Common;

namespace Drivers
{
    enum InteractiveType
    {
        ITUnknown = 0,
        ITSender = 1,
        ITReceiver = 2,
        ITSenderAndReceiver = ITSender | ITReceiver
    };

	class ChannelContext : public Context
	{
	public:
        ChannelContext();
        ~ChannelContext() override;
        
        virtual void setReopened(bool reopened);
        virtual void setDeviceInterval(const TimeSpan& deviceInterval);

        bool useReceiveTimeout() const;
        void setUseReceiveTimeout(bool useReceiveTimeout);
        
        bool reopened() const;
        
        const TimeSpan& deviceInterval() const;
        
    public:
        static String toInteractiveTypeStr(InteractiveType type);
        static InteractiveType parseInteractiveType(const String& str);

	protected:
		bool _useReceiveTimeout;
		bool _reopened;
        TimeSpan _deviceInterval;
	};
}

#endif // CHANNELCONTEXT_H
