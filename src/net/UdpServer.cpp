/*
 * UdpServer.cpp
 *
 *  Created on: May 15, 2014
 *      Author: baowei
 */

#if WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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

#include "net/UdpServer.h"
#include <errno.h>
#include "diag/Trace.h"
#include "data/Convert.h"
#include "thread/TickTimeout.h"
#include "system/Math.h"
#include "net/TcpClient.h"
#include "net/Dns.h"
#include "data/ByteArray.h"

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#define SHUT_RDWR SD_BOTH
#else
#define closesocket(a) ::close(a)
#endif

using namespace Diag;

namespace Net {
    UdpServer::UdpServer() {
        _socket = -1;
        TcpClient::initializeSocket();
    }

    UdpServer::~UdpServer() {
        close();

        _socket = -1;
    }

    bool UdpServer::bind(const Endpoint &endpoint, bool reuseAddress) {
        return bind(endpoint.address, endpoint.port, reuseAddress);
    }

    bool UdpServer::bind(const String &addr, int port, bool reuseAddress) {
//        struct hostent  *phe;		/* pointer to host information entry    */
        struct sockaddr_in sin;     /* an Internet endpoint address  */

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        if ((sin.sin_port = htons(port)) == 0)
            Debug::writeFormatLine("invalid port \"%d\"", port);
        bool anyAddr = false;
        if (!addr.isNullOrEmpty()) {
            const char *anyStr = "any";
#if WIN32
            anyAddr = stricmp(addr.c_str(), anyStr) == 0;
#else
            anyAddr = strcasecmp(addr.c_str(), anyStr) == 0;
#endif
        } else {
            anyAddr = true;
        }
        if (!anyAddr) {
//            if ( (phe = gethostbyname(addr.c_str())) != NULL )
//                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
//            else if ( (sin.sin_addr.s_addr = inet_addr(addr.c_str())) == INADDR_NONE )
//                Debug::writeFormatLine("can't get \"%s\" host entry", addr.c_str());
            Dns::getHostByName(addr, sin);
        } else {
            sin.sin_addr.s_addr = htonl(INADDR_ANY);
        }

        /* Allocate a socket */
        _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (_socket < 0)
            Debug::writeFormatLine("can't create udp server socket: %s", strerror(errno));

//        setBlocking(true);

        if (reuseAddress) {
            if (!setReuseAddress(true))
                return false;
        }

        return ::bind(_socket, (struct sockaddr *) &sin, sizeof(sin)) != -1;
    }

    void UdpServer::close() {
        if (_socket != -1) {
            int result;
            result = closesocket(_socket);
            if (result == -1) {
                Debug::writeFormatLine("closesocket failed with error = %s", strerror(errno));
            } else {
                _socket = -1;
            }
        }
    }

    bool UdpServer::setBlocking(bool blocking) {
        if (_socket != -1) {
            //-------------------------
            // Set the socket I/O mode: In this case FIONBIO
            // enables or disables the blocking mode for the
            // socket based on the numerical value of mode.
            // If mode = 0, blocking is enabled;
            // If mode != 0, non-blocking mode is enabled.
            u_long mode = blocking ? 0 : 1;
            int result = ::ioctl(_socket, FIONBIO, &mode);
            if (result != 0) {
                Debug::writeFormatLine("ioctlsocket failed with error: %d\n", result);
            } else {
                return true;
            }
        }
        return false;
    }

    bool UdpServer::setReuseAddress(bool reuseAddress) {
        int opt = 1;
        int result;
        result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));
        if (result == -1) {
            Debug::writeFormatLine("SO_REUSEADDR failed with error = %s", strerror(errno));
            return false;
        }
//        result = setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&opt, sizeof(opt));
//        if (result == -1)
//        {
//            Debug::writeFormatLine("SO_REUSEPORT' failed with error = %s", strerror(errno));
//            return false;
//        }
        return true;
    }

    ssize_t UdpServer::read(uint8_t *data, size_t count, bool peek) {
        if (_socket != -1) {
            struct sockaddr_in sin;     /* an Internet endpoint address  */
            memset(&sin, 0, sizeof(sin));
            socklen_t len = sizeof(sin);

            //            return (int)::recv(_socket, data, maxlen, 0);
            ssize_t result = ::recvfrom(_socket, (char *) data, count, peek ? MSG_PEEK : 0, (struct sockaddr *) &sin,
                                        &len);
#if WIN32
            // If the datagram or message is larger than the buffer specified, the buffer is filled with the first part of the datagram,
            // and recvfrom generates the error WSAEMSGSIZE. For unreliable protocols (for example, udp) the excess data is lost.
            if(result > 0 || (result == -1 && WSAGetLastError() == WSAEMSGSIZE))
#else
            if (result > 0)
#endif
            {
#if WIN32
                if (result == -1)
                    result = count;
#endif

//#ifdef DEBUG
//                ByteArray array(data, result);
//                Debug::writeFormatLine("udp received: %s", array.toString().c_str());
//#endif

                _peerEndpoint.address = inet_ntoa(sin.sin_addr);
                _peerEndpoint.port = ntohs(sin.sin_port);

                memset(&sin, 0, len);
                int result2 = ::getsockname(_socket, (struct sockaddr *) &sin, &len);
                if (result2 != -1) {
                    _endpoint.address = inet_ntoa(sin.sin_addr);
                    _endpoint.port = ntohs(sin.sin_port);
                }
            }
            return result;
        }
        return 0;
    }

    ssize_t UdpServer::receive(uint8_t *data, size_t count, uint32_t timeout) {
        ssize_t len = 0;
        if (_socket != -1) {
            uint32_t startTime = (uint32_t) TickTimeout::getCurrentTickCount();
            uint32_t deadTime = (uint32_t) TickTimeout::getDeadTickCount(startTime, timeout);
            do {
                size_t available = this->available();
                if (available >= count) {
                    len = count;
                    break;
                }

                if (TickTimeout::isTimeout(startTime, deadTime)) {
                    len = available;
                    break;
                }

                Thread::msleep(1);
            } while (true);

            return read(data, len);
        }
        return len;
    }

    int UdpServer::available() {
        if (_socket != -1) {
            u_long argp = 0;
#if __APPLE__
            // ioctl (fd, FIONREAD, XXX) returns the size of
            // the UDP header as well on
            // Darwin.
            //
            // Use getsockopt SO_NREAD instead to get the
            // right values for TCP and UDP.
            //
            // ai_canonname can be null in some cases on darwin, where the runtime assumes it will
            // be the value of the ip buffer.

            socklen_t optlen = sizeof(int);
            if (getsockopt(_socket, SOL_SOCKET, SO_NREAD, &argp, &optlen) == 0) {
                return (int) argp;
            }
#else
            if(ioctl(_socket, FIONREAD, &argp) == 0)
            {
                return (int)argp;
            }
#endif
        }
        return 0;
    }

    int UdpServer::receiveBufferSize() const {
        socklen_t bufferSize = 0;
        if (_socket != -1) {
#if WIN32
            int len = sizeof(bufferSize);
#else
            socklen_t len = sizeof(bufferSize);
#endif
            int result = getsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *) &bufferSize, &len);
            if (result == -1) {
                Debug::writeFormatLine("getsockopt'SO_RCVBUF' failed with error = %s", strerror(errno));
            }
        }
        return bufferSize;
    }

    void UdpServer::setReceiveBufferSize(int bufferSize) {
        if (_socket != -1) {
            int result = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char *) &bufferSize, sizeof(int));
            if (result == -1) {
                Debug::writeFormatLine("setsockopt'SO_RCVBUF' failed with error = %s", strerror(errno));
            }
        }
    }

} /* namespace Drivers */
