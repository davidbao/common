//
//  UdpServer.h
//  common
//
//  Created by baowei on 2014/5/15.
//  Copyright (c) 2014 com. All rights reserved.
//

#ifndef UdpServer_h
#define UdpServer_h

#include "data/StringArray.h"
#include "data/String.h"
#include "net/NetType.h"

namespace Net
{
	class UdpServer
	{
	public:
		UdpServer();
		virtual ~UdpServer();

        bool bind(const Endpoint& endpoint, bool reuseAddress = false);
		bool bind(const String& addr, int port, bool reuseAddress = false);

		void close();

		ssize_t read(uint8_t *data, size_t count, bool peek = false);
        ssize_t receive(uint8_t *data, size_t count, uint32_t timeout);

		inline bool isValid() const
		{
			return _socket != -1;
		}
        
        inline const Endpoint& peerEndpoint() const
        {
            return _peerEndpoint;
        }
        inline const Endpoint& endpoint() const
        {
            return _endpoint;
        }

		int available();
        
        bool setBlocking(bool blocking = false);

		int receiveBufferSize() const;
		void setReceiveBufferSize(int bufferSize);
        
        bool setReuseAddress(bool reuseAddress = true);

	private:
		int _socket;
        
        Endpoint _peerEndpoint;
        Endpoint _endpoint;
	};

}

#endif /* UdpServer_h */
