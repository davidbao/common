/*
 * UdpClient.cpp
 *
 *  Created on: May 15, 2014
 *      Author: baowei
 */

#if WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#endif
#include <errno.h>
#include "net/UdpClient.h"
#include "diag/Trace.h"
#include "data/Convert.h"
#include "system/Math.h"
#include "thread/TickTimeout.h"
#include "net/TcpClient.h"
#include "net/Dns.h"

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#define SHUT_RDWR SD_BOTH
#else
#define closesocket(a) ::close(a)
#endif

using namespace Common;

namespace Common
{
	UdpClient::UdpClient()
	{
		_socket = -1;
		TcpClient::initializeSocket();
	}

	UdpClient::~UdpClient()
	{
		close();

		_socket = -1;
	}

	bool UdpClient::open(const char *host, const ushort port)
	{
        /* Allocate a socket */
        _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (_socket < 0)
        {
            Debug::writeFormatLine("can't create udp client socket: %s", strerror(errno));
            return false;
        }

		bool isBroadcast = this->isBroadcast(host);
        
        struct sockaddr_in sin;

        /* Construct the server sockaddr_in structure */
        memset(&sin, 0, sizeof(sin));

        /* Clear struct */
        sin.sin_family = AF_INET;

        /* Internet/IP */
        sin.sin_addr.s_addr = isBroadcast ? htonl(INADDR_BROADCAST) : inet_addr(host);

        /* IP address */
        sin.sin_port = htons(port);
        
        if(!isBroadcast)
            ::bind(_socket, (struct sockaddr *) & sin, sizeof (sin));

        int broadcast = 1;
        // if that doesn't work, try this
        //char broadcast = '1';

        // this call is what allows broadcast packets to be sent:
        if (setsockopt(_socket,
                       SOL_SOCKET,
                       SO_BROADCAST,
                       (const char*)&broadcast,
                       sizeof(broadcast)) == -1)
        {
            Debug::writeFormatLine("setsockopt error: %s", strerror(errno));
            return false;
        }
        
//        setBlocking(true);

		return _socket != -1;
	}
	bool UdpClient::open(const ushort port)
	{
		return open(nullptr, port);
	}
	void UdpClient::close()
	{
		if (_socket != -1)
        {
            int result;
            result = closesocket(_socket);
            if (result == -1)
            {
                Debug::writeFormatLine("closesocket failed with error = %s", strerror(errno) );
            }
            else
            {
                _socket = -1;
            }
        }
	}
    
    bool UdpClient::setBlocking(bool blocking)
    {
        if (_socket != -1)
        {
            //-------------------------
            // Set the socket I/O mode: In this case FIONBIO
            // enables or disables the blocking mode for the
            // socket based on the numerical value of mode.
            // If mode = 0, blocking is enabled;
            // If mode != 0, non-blocking mode is enabled.
            u_long mode = blocking ? 0 : 1;
            int result = ::ioctl(_socket, FIONBIO, &mode);
            if (result != 0)
            {
                Debug::writeFormatLine("ioctlsocket failed with error: %d\n", result);
            }
            else
            {
                return true;
            }
        }
        return false;
    }
    
    bool UdpClient::isBroadcast(const char* host) const
    {
        bool broadcast = false;
        if(host == nullptr || strlen(host) == 0)
        {
            broadcast = true;
        }
        else
        {
            const char* anyStr = "any";
#if WIN32
            broadcast = stricmp(host, anyStr) == 0;
#else
            broadcast = strcasecmp(host, anyStr) == 0;
#endif
        }
        return broadcast;
    }
    
	ssize_t UdpClient::write(const char* host, const ushort port, const uint8_t *data, size_t count)
	{
		if(_socket != -1)
		{
			struct sockaddr_in sin;     /* an Internet endpoint address  */

			memset(&sin, 0, sizeof(sin));
			sin.sin_family = AF_INET;
			if ((sin.sin_port = htons(port)) == 0)
			{
				Debug::writeFormatLine("invalid port \"%d\"", port);
				return -1;
			}
			bool isBroadcast = this->isBroadcast(host);
			sin.sin_addr.s_addr = isBroadcast ? htonl(INADDR_BROADCAST) : inet_addr(host);

            ssize_t result = ::sendto(_socket, (char*)data, count, 0, (struct sockaddr *)&sin, sizeof(sin));
			if (result == -1)
			{
//				Debug::writeFormatLine("udp sendto failed with errorno: %d, error = %s", errno, strerror(errno));
			}
            return result;
		}
		return -1;
	}
	ssize_t UdpClient::write(const ushort port, const uint8_t *data, size_t count)
	{
		return write(nullptr, port, data, count);
	}
    ssize_t UdpClient::write(const ushort port, const ByteArray& buffer)
    {
        return write(port, buffer.data(), buffer.count());
    }

	int UdpClient::sendBufferSize() const
	{
		socklen_t bufferSize = 0;
		if (_socket != -1)
		{
#if WIN32
			int len = sizeof(bufferSize);
#else
			socklen_t len = sizeof(bufferSize);
#endif
			int result = getsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, &len);
			if (result == -1)
			{
				Debug::writeFormatLine("getsockopt'SO_SNDBUF' failed with error = %s", strerror(errno));
			}
		}
		return bufferSize;
	}
	void UdpClient::setSendBufferSize(int bufferSize)
	{
		if (_socket != -1)
		{
			int result = setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(int));
			if (result == -1)
			{
				Debug::writeFormatLine("setsockopt'SO_SNDBUF' failed with error = %s", strerror(errno));
			}
		}
	}

} /* namespace Drivers */
