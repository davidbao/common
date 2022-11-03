/*
 * UdpClient.h
 *
 *  Created on: May 15, 2014
 *      Author: baowei
 */

#ifndef UDPCLIENT_H_
#define UDPCLIENT_H_

#include "data/ValueType.h"
#include "data/PList.h"

namespace Common
{
	class UdpClient
	{
	public:
		UdpClient();
		virtual ~UdpClient();

		bool open(const char* host, const ushort port);
		bool open(const ushort port);
		void close();

		ssize_t write(const char* host, const ushort port, const uint8_t *data, size_t count);
		ssize_t write(const ushort port, const uint8_t *data, size_t count);
        ssize_t write(const ushort port, const ByteArray& buffer);

        int sendBufferSize() const;
		void setSendBufferSize(int bufferSize);
        
        bool setBlocking(bool blocking = false);
        
    private:
        bool isBroadcast(const char* host) const;
        
    public:
        static const int MaxSendBufferSize = 65535;

	private:
		int _socket;
	};
    typedef PList<UdpClient> UdpClients;

} /* namespace Drivers */

#endif /* UDPCLIENT_H_ */
