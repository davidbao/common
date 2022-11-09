//
//  Interactive.h
//  common
//
//  Created by baowei on 2016/12/6.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Interactive_h
#define Interactive_h

#include "data/NetType.h"
#include "data/Convert.h"
#include "exception/Exception.h"

using namespace Common;

namespace Drivers
{
    class Device;
	class Channel;
    class DriverManager;
	class Interactive
	{
	public:
        virtual ~Interactive();
        
	protected:
        Interactive(DriverManager* dm, Channel* channel = nullptr);

	public:
		virtual bool open() = 0;
		virtual void close() = 0;

		virtual bool connected() = 0;
		virtual size_t available() = 0;

		virtual ssize_t send(const uint8_t* buffer, off_t offset, size_t count) = 0;
		virtual ssize_t receive(uint8_t* buffer, off_t offset, size_t count) = 0;
        virtual ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint timeout);
        virtual ssize_t receive(ByteArray* buffer, size_t count, uint timeout);
        
        virtual bool isClosing() const;

        Channel* channel() const;
        void setChannel(Channel* channel);
        
    protected:
        DriverManager* manager() const;

	protected:
		Channel* _channel;
		bool _useReceiveTimeout;
        
        DriverManager* _manager;
	};
    
    class EthernetEndpoint
    {
    public:
        virtual ~EthernetEndpoint();
        virtual const Endpoint& peerEndpoint() const = 0;
        virtual const Endpoint& endpoint() const = 0;
        const P2PEndpoint p2pEndpoint() const;
    };
    
    class BackgroudReceiver
    {
    public:
        virtual ~BackgroudReceiver();
        
    protected:
        bool receiveFromBuffer(Device* device);
    };
}

#endif // Interactive_h
