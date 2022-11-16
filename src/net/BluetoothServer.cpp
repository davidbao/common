#if WIN32
#include <winsock2.h>
#include <ws2bth.h>
#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#else
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/ioctl.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>
#include <signal.h>
#include <netinet/tcp.h>
#endif
#include "net/BluetoothServer.h"
#include "net/TcpClient.h"
#include <errno.h>
#include "diag/Trace.h"
#include "data/Convert.h"
#include "thread/TickTimeout.h"
#include "system/Math.h"

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#define SHUT_RDWR SD_BOTH
#define socklen_t int

////#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "irprops.lib")
#else
#define closesocket(a) ::close(a)
#endif

using namespace Common;

namespace Net
{
	BluetoothServer::BluetoothServer()
	{
		_socket = -1;
		_isListening = false;
		TcpClient::initializeSocket();
	}

	BluetoothServer::~BluetoothServer()
	{
		close();

		_socket = -1;
	}

	bool BluetoothServer::bind()
	{
#if WIN32
		_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
		if (_socket < 0)
		{
			Debug::writeFormatLine("can't create bluetooth socket: %s", strerror(errno));
			return false;
		}

		WSAPROTOCOL_INFO protocolInfo;
		int protocolInfoSize = sizeof(protocolInfo);
		if (0 != getsockopt(_socket, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize))
		{
			Debug::writeFormatLine("failed to invoke getsockopt: %s", strerror(errno));
			return false;
		}
		SOCKADDR_BTH address;
		address.addressFamily = AF_BTH;
		address.btAddr = 0;
		address.serviceClassId = GUID_NULL;
		address.port = BT_PORT_ANY;
		sockaddr *pAddr = (sockaddr*)&address;

		return  ::bind(_socket, pAddr, sizeof(SOCKADDR_BTH)) != -1;
#else
		return false;
#endif
	}

	bool BluetoothServer::listen(int maxConnections)
	{
#if WIN32
		if (_socket != -1)
		{
			SOCKADDR_BTH address;
			address.addressFamily = AF_BTH;
			address.btAddr = 0;
			address.serviceClassId = GUID_NULL;
			address.port = BT_PORT_ANY;
			sockaddr *pAddr = (sockaddr*)&address;

			int size = sizeof(SOCKADDR_BTH);
			if (0 != getsockname(_socket, pAddr, &size))
			{
				Debug::writeFormatLine("failed to invoke getsockname: %s", strerror(errno));
				return false;
			}

			bool result = ::listen(_socket, maxConnections) != -1;
			if (!result)
			{
				Debug::writeFormatLine("failed to invoke listen: %s", strerror(errno));
				return false;
			}

			WSAQUERYSET service;
			memset(&service, 0, sizeof(service));
			service.dwSize = sizeof(service);
			service.lpszServiceInstanceName = "BluetoothServer";
			service.lpszComment = "Pushing data";

			GUID serviceID = SerialPortServiceClass_UUID;

			service.lpServiceClassId = &serviceID;
			service.dwNumberOfCsAddrs = 1;
			service.dwNameSpace = NS_BTH;

			CSADDR_INFO csAddr;
			memset(&csAddr, 0, sizeof(csAddr));
			csAddr.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
			csAddr.LocalAddr.lpSockaddr = pAddr;
			csAddr.iSocketType = SOCK_STREAM;
			csAddr.iProtocol = BTHPROTO_RFCOMM;
			service.lpcsaBuffer = &csAddr;

			if (0 != WSASetService(&service, RNRSERVICE_REGISTER, 0))
			{
				Debug::writeFormatLine("failed to invoke WSASetService: %s", strerror(errno));
			}

			_isListening = true;
			return true;
		}
#endif
		return false;
	}

	int BluetoothServer::accept()
	{
#if WIN32
		if (_socket != -1)
		{
			SOCKADDR_BTH sab2;
			int ilen = sizeof(sab2);
			int result = ::accept(_socket, (struct sockaddr *)&sab2, &ilen);
			//if(result != -1)
			//{
			//	String addr = inet_ntoa(sin.sin_addr);
			//	_clientAddrs.add(addr);
			//}
			return result;
		}
#endif
		return false;
	}

	void BluetoothServer::close()
	{
#if WIN32
		if (_socket != -1)
		{
			SOCKADDR_BTH address;
			address.addressFamily = AF_BTH;
			address.btAddr = 0;
			address.serviceClassId = GUID_NULL;
			address.port = BT_PORT_ANY;
			sockaddr *pAddr = (sockaddr*)&address;

			int size = sizeof(SOCKADDR_BTH);
			if (0 != getsockname(_socket, pAddr, &size))
			{
				Debug::writeFormatLine("failed to invoke getsockname: %s", strerror(errno));
			}

			WSAQUERYSET service;
			memset(&service, 0, sizeof(service));
			service.dwSize = sizeof(service);
			service.lpszServiceInstanceName = "BluetoothServer";
			service.lpszComment = "Pushing data";

			GUID serviceID = SerialPortServiceClass_UUID;

			service.lpServiceClassId = &serviceID;
			service.dwNumberOfCsAddrs = 1;
			service.dwNameSpace = NS_BTH;

			CSADDR_INFO csAddr;
			memset(&csAddr, 0, sizeof(csAddr));
			csAddr.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
			csAddr.LocalAddr.lpSockaddr = pAddr;
			csAddr.iSocketType = SOCK_STREAM;
			csAddr.iProtocol = BTHPROTO_RFCOMM;
			service.lpcsaBuffer = &csAddr;

			if (0 != WSASetService(&service, RNRSERVICE_DELETE, 0))
			{
				Debug::writeFormatLine("WSASetService failed with error = %s", strerror(errno));
			}

			int result;
			if (_isListening)
			{
				result = shutdown(_socket, SHUT_RDWR);
				if (result == -1)
				{
					Debug::writeFormatLine("shutdown failed with error = %s", strerror(errno));
				}
			}
			result = closesocket(_socket);
			if (result == -1)
			{
				Debug::writeFormatLine("closesocket failed with error = %s", strerror(errno));
			}
			else
			{
				_socket = -1;
				_isListening = false;
			}
		}
#endif
	}

	int BluetoothServer::sendBufferSize() const
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
	void BluetoothServer::setSendBufferSize(int bufferSize)
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
	int BluetoothServer::receiveBufferSize() const
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
	void BluetoothServer::setReceiveBufferSize(int bufferSize)
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
