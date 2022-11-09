#if WIN32
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#include "exception/Exception.h"
//#pragma comment (lib, "Ws2_32.lib")
#else
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/ioctl.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/tcp.h>
#endif
#include <errno.h>
#include "net/BluetoothClient.h"
#include "diag/Trace.h"
#include "system/Math.h"
#include "data/Convert.h"
#include "thread/TickTimeout.h"
#include "net/TcpClient.h"

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#define socklen_t int
#define SHUT_RDWR SD_BOTH
#else
#define closesocket(a) ::close(a)
#endif

using namespace Common;

namespace Drivers
{
	BluetoothClient::BluetoothClient(int socketId)
	{
		_socket = -1;
		_connected = false;
		TcpClient::initializeSocket();

		if(socketId != -1)
		{
			_socket = socketId;
			_connected = true;

#if WIN32
			SOCKADDR_BTH sab;
			int clen = sizeof(sab);
			int result = ::getpeername(_socket, (struct sockaddr *)&sab, &clen);
			if(result != -1)
			{
				_peerEndpoint.address = Convert::convertStr("%04x%08x", GET_NAP(sab.btAddr), GET_SAP(sab.btAddr));
				_peerEndpoint.port = sab.port;
			}
#endif
		}
	}

	BluetoothClient::~BluetoothClient()
	{
		close();

		_socket = -1;
		//#if WIN32
		//		WSACleanup();
		//#endif
	}

	void BluetoothClient::close()
	{
		if (_socket != -1) 
		{
			int result;
			if(_connected)
			{
				result = shutdown(_socket, SHUT_RDWR);
				if (result == -1)
				{
					Debug::writeFormatLine("shutdown failed with error = %s", strerror(errno) );
				}
			}
			result = closesocket(_socket);
			if (result == -1)
			{
				Debug::writeFormatLine("closesocket failed with error = %s", strerror(errno) );
			}
			else
			{
				_socket = -1;
				_connected = false;
			}
		}
		//_connected = false;
	}

    size_t BluetoothClient::available()
	{
		if(_socket != -1)
		{
			u_long argp = 0;
			if(ioctl(_socket, FIONREAD, &argp) == 0)
			{
				return argp;
			}
		}
		return 0;
	}

	ssize_t BluetoothClient::write(const uint8_t *data, size_t count)
	{
		return _socket != -1 ? ::send(_socket, (char*)data, count, 0) : 0;
	}

	ssize_t BluetoothClient::read(uint8_t *data, size_t count)
	{
		return _socket != -1 ? ::recv(_socket, (char*)data, count, 0) : 0;
	}

	bool BluetoothClient::connected()
	{
//		if(_connected)
//		{
//			fd_set myset; 
//			FD_ZERO(&myset); 
//			FD_SET(_socket, &myset); 
//			_connected = select(_socket+1, NULL, &myset, NULL, NULL) != 0 && FD_ISSET(_socket, &myset);
//#ifdef DEBUG
//			if(!_connected)
//				Debug::writeLine("BluetoothClient::connected = false.");
//#endif
//		}
		return _connected;
	}

	ssize_t BluetoothClient::receiveBySize(BluetoothClient* client, uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint timeout)
	{
		uint startTime = TickTimeout::getCurrentTickCount();
		uint deadTime = TickTimeout::getDeadTickCount(startTime, timeout);

        size_t available = client->available();
		bool dataReady = false;
		do
		{
			if (client->available() >= count)
			{
				dataReady = true;
				break;
			}
			uint now = TickTimeout::getCurrentTickCount();
			if (client->available() == available)
			{
				Thread::msleep(1);
				if (TickTimeout::isTimeout(startTime, deadTime, now))
					break;
			}
			else
			{
				available = client->available();
				deadTime = TickTimeout::getDeadTickCount(now, timeout);
			}
		} while (true);

		if (client->available() > 0)
		{
			return dataReady
				? client->read(buffer + offset, count)
				: client->read(buffer + offset, Math::min(client->available(), bufferLength));
		}
		return 0;
	}

	int BluetoothClient::sendBufferSize() const
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
	void BluetoothClient::setSendBufferSize(int bufferSize)
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
	int BluetoothClient::receiveBufferSize() const
	{
		socklen_t bufferSize = 0;
		if (_socket != -1)
		{
#if WIN32
			int len = sizeof(bufferSize);
#else
			socklen_t len = sizeof(bufferSize);
#endif
			int result = getsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &len);
			if (result == -1)
			{
				Debug::writeFormatLine("getsockopt'SO_RCVBUF' failed with error = %s", strerror(errno));
			}
		}
		return bufferSize;
	}
	void BluetoothClient::setReceiveBufferSize(int bufferSize)
	{
		if (_socket != -1)
		{
			int result = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(int));
			if (result == -1)
			{
				Debug::writeFormatLine("setsockopt'SO_RCVBUF' failed with error = %s", strerror(errno));
			}
		}
	}
}
