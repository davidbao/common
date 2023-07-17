//
//  TcpClient.cpp
//  common
//
//  Created by baowei on 2016/12/31.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifdef WIN32

#include <winsock2.h>
#include <WS2tcpip.h>
#include <WSPiApi.h>
#include <iphlpapi.h>

#define poll WSAPoll
#else

#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <csignal>
#include <netinet/tcp.h>
#include <poll.h>

#ifndef __ANDROID__

#include <ifaddrs.h>

#endif
#ifdef AF_LINK

#include <net/if_dl.h>

#endif
#ifdef AF_PACKET

#include <netpacket/packet.h>

#endif
#endif

#include <cerrno>
#include <fcntl.h>

#include "net/TcpClient.h"
#include "diag/Trace.h"
#ifdef DEBUG
#include "diag/Stopwatch.h"
#endif
#include "system/Math.h"
#include "data/DateTime.h"
#include "data/ByteArray.h"
#include "data/ValueType.h"
#include "thread/TickTimeout.h"
#include "exception/Exception.h"
#include "net/Dns.h"
//#include "IO/File.h"

//#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bio.h>

#ifdef WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#ifndef socklen_t
#define socklen_t int
#endif
#define SHUT_RDWR SD_BOTH
#else
#define closesocket(a) ::close(a)
#define SUPPORT_IP6
#endif

#ifdef __EMSCRIPTEN__

#include <emscripten/emscripten.h>

#endif

using namespace Diag;
using namespace System;

namespace Net {
    static struct InitializeTcpClient {
        InitializeTcpClient() {
            TcpClient::initializeSocket();
        }
    } InitializeTcpClient;

    TcpClient::TcpClient(int socketId, IPVersion ipVersion) {
        _socket = -1;
        _connected = false;

        initializeSocket();

        if (socketId != -1) {
            _socket = socketId;
            _connected = true;

            updateEndpoints(ipVersion == IPVersion::IPV4 ? AF_INET : AF_INET6);
        }
    }

    TcpClient::~TcpClient() {
        TcpClient::close();

        _socket = -1;
    }

    bool TcpClient::useReceiveTimeout() const {
        return false;
    }

    ssize_t TcpClient::send(const uint8_t *buffer, off_t offset, size_t count) {
        return write(buffer + offset, count);
    }

    ssize_t TcpClient::receive(uint8_t *buffer, off_t offset, size_t count) {
        return read(buffer + offset, count);
    }

    ssize_t TcpClient::receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) {
        if (timeout == 0) {
            return receive(buffer, offset, count);
        }
        // use it if useReceiveTimeout() return true;
        throw NotImplementedException("Can not implement this method.");
    }

    ssize_t TcpClient::receive(ByteArray *buffer, size_t count, uint32_t timeout) {
        if (timeout == 0) {
            auto temp = new uint8_t[count];
            ssize_t readCount = receive(temp, 0, count);
            if (readCount > 0) {
                buffer->addRange(temp, readCount);
            }
            delete[] temp;
            return readCount;
        }
        // use it if useReceiveTimeout() return true;
        throw NotImplementedException("Can not implement this method.");
    }

    bool TcpClient::connectToHost(const String &host, uint16_t port, const TimeSpan &timeout, bool reuseAddress) {
        return connectToHost(host, port, (uint32_t) timeout.totalMilliseconds(), reuseAddress);
    }

    bool TcpClient::connectToHost(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress) {
#ifdef __APPLE__
        return connectToHost_IPV6(host, port, timeout, reuseAddress);
#else
        return connectToHost_IPV6(host, port, timeout, reuseAddress);
#endif
    }

    bool TcpClient::connectToHost(const String &host, uint16_t port, uint32_t timeout) {
        return connectToHost(host, port, timeout, false);
    }

    bool TcpClient::connectToHost(const Endpoint &host, const TimeSpan &timeout, bool reuseAddress) {
        return connectToHost(host.address, host.port, timeout, reuseAddress);
    }

    bool TcpClient::connectToHost_IPV6(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress) {
        // Fix bug: support IPV6.
        struct addrinfo hints{}, *res, *res0;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
#ifdef __APPLE__
        hints.ai_flags = AI_DEFAULT;
#else
        hints.ai_flags = AI_ALL;
#endif
        int error = getaddrinfo(host, nullptr, &hints, &res0);
        if (error) {
            Debug::writeFormatLine("getaddrinfo failed with error: %s", gai_strerror(error));
            /*NOTREACHED*/
            return false;
        }
        for (res = res0; res; res = res->ai_next) {
//            Debug::writeFormatLine("res->ai_family: %d, res->ai_socktype: %d, res->ai_protocol: %d",
//                                   res->ai_family, res->ai_socktype, res->ai_protocol);

            /* Allocate a socket */
            _socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (_socket < 0)
                continue;

#ifdef __EMSCRIPTEN__
            fcntl(_socket, F_SETFL, O_NONBLOCK);
#else
            /* Connect the socket with timeout */
            unsigned long non_blocking = 1;
            if (ioctl(_socket, FIONBIO, &non_blocking) != 0)
                Debug::writeFormatLine("ioctl failed with error: %s", strerror(errno));
#endif

            if (reuseAddress) {
                if (!setReuseAddress(true))
                    return false;
            }

            if (res->ai_family == AF_INET) {
                // Found IPv4 address.
                // Wrap the native address structure, and add to results.

                if (((struct sockaddr_in *) res->ai_addr)->sin_port == 0)
                    ((struct sockaddr_in *) res->ai_addr)->sin_port = htons(port);
            } else if (res->ai_family == AF_INET6) {
                // Found IPv6 address.
                // Wrap the native address structure, and add to results.

                if (((struct sockaddr_in6 *) res->ai_addr)->sin6_port == 0)
                    ((struct sockaddr_in6 *) res->ai_addr)->sin6_port = htons(port);
            }

#ifdef __EMSCRIPTEN__
            int ret = connect(_socket, res->ai_addr, res->ai_addrlen);
            if (ret == -1 && errno != EINPROGRESS) {
                _connected = false;
            } else {
                _connected = true;

                updateEndpoints(res->ai_family);
            }
#else
            /* Connect the socket with timeout */
            int result = connect(_socket, res->ai_addr, (int) res->ai_addrlen);
            if (result == -1) {
                if (isWriteSet(timeout) && hasNoError()) {
                    if (updateEndpoints(res->ai_family)) {
                        Debug::writeFormatLine("TcpClient'%d'.ipv6.connected, endpoint: %s, peerEndpoint: %s",
                                               _socket, _endpoint.toString().c_str(), _peerEndpoint.toString().c_str());
                        _connected = true;
                    }
                }
            }
#endif
        }
        freeaddrinfo(res0);

        return connected();
    }

    bool TcpClient::connectToHost_IPV4(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress) {
        struct sockaddr_in sin{};     /* an Internet endpoint address  */

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        if ((sin.sin_port = htons(port)) == 0)
            Debug::writeFormatLine("invalid port \"%d\"", port);

        //		/* Map host name to IP address, allowing for dotted decimal */
        //		if ( (phe = gethostbyname(host)) != nullptr )
        //			memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        //		else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
        //			Debug::writeFormatLine("can't get \"%s\" host entry", host);
        if (!Dns::getHostByName(host, sin))
            return false;

        /* Allocate a socket */
        _socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket < 0)
            Debug::writeFormatLine("can't create tcp client socket: %s", strerror(errno));

#ifdef __EMSCRIPTEN__
        fcntl(_socket, F_SETFL, O_NONBLOCK);
#else
        unsigned long non_blocking = 1;
        if (ioctl(_socket, FIONBIO, &non_blocking) != 0)
            Debug::writeFormatLine("ioctl failed with error: %s", strerror(errno));
#endif

        if (reuseAddress) {
            if (!setReuseAddress(true))
                return false;
        }

#ifdef __EMSCRIPTEN__
        int ret = connect(_socket, (struct sockaddr *) &sin, sizeof(sin));
        if (ret == -1 && errno != EINPROGRESS) {
            _connected = false;
        } else {
            _connected = true;

            updateEndpoints(AF_INET);
        }
#else
        /* Connect the socket with timeout */
        int result = connect(_socket, (struct sockaddr *) &sin, sizeof(sin));
        if (result == -1) {
            if (isWriteSet(timeout) && hasNoError()) {
                if (updateEndpoints(AF_INET)) {
                    Debug::writeFormatLine("TcpClient'%d'.ipv4.connected, endpoint: %s, peerEndpoint: %s",
                                           _socket, _endpoint.toString().c_str(), _peerEndpoint.toString().c_str());
                    _connected = true;
                }
            }
        }
#endif
        return connected();
    }

    void TcpClient::close() {
        if (_socket != -1) {
            int result;
#ifndef __EMSCRIPTEN__
            if (_connected) {
                result = shutdown(_socket, SHUT_RDWR);
                if (result == -1) {
                    Debug::writeFormatLine("shutdown failed with error = %s", strerror(errno));
                }
            }
#endif
            result = closesocket(_socket);
            if (result == -1) {
                Debug::writeFormatLine("closesocket failed with error = %s", strerror(errno));
            } else {
                _socket = -1;
                _connected = false;
            }
        }
    }

    const Endpoint &TcpClient::peerEndpoint() const {
        return _peerEndpoint;
    }

    const Endpoint &TcpClient::endpoint() const {
        return _endpoint;
    }

    int TcpClient::socketId() const {
        return _socket;
    }

    bool TcpClient::isClosing() const {
        return false;
    }

    size_t TcpClient::available() {
        if (_socket != -1) {
            u_long argp = 0;
            if (ioctl(_socket, FIONREAD, &argp) == 0) {
                return (int) argp;
            }
        }
        return 0;
    }

    ssize_t TcpClient::write(const uint8_t *data, size_t count) {
        if (_socket != -1) {
            ssize_t length = ::send(_socket, (const char *) data, (int) count, 0);
#ifdef __EMSCRIPTEN__
            if (length == -1) {
                if (errno == EAGAIN || errno == EDEADLK) {
                    //try again
//                    Debug::writeLine("TcpClient::write in progress.");
                    static int count = 0;
                    count++;
                    if (count >= 10) {
                        count = 0;
                    } else {
                        Thread::msleep(100);
                        length = write(data, count);
                    }
                }
            }
#endif
            return length;
        }
        return 0;
    }

    ssize_t TcpClient::write(const ByteArray &data) {
        return write(data.data(), data.count());
    }

    ssize_t TcpClient::write(const String &str) {
        return write((const uint8_t *) str.c_str(), str.length());
    }

    ssize_t TcpClient::read(uint8_t *data, size_t count) {
        if (_socket != -1) {
            ssize_t length = ::recv(_socket, (char *) data, (int) count, 0);
            return length >= 0 ? length : 0;
        }
        return 0;
    }

    bool TcpClient::connected() {
        if (isClosing())
            return false;

#ifdef __EMSCRIPTEN__
        return _connected;
//#elif __arm_linux__
//        return _connected;
#else
        if (_connected) {
            static const int timeout = 1000;
            if (isWriteSet(timeout)) {
                if (!hasNoError())
                    _connected = false;
            } else {
                Debug::writeFormatLine("connect function'%s' failed with error code: %d, error: %s",
                                       !_peerEndpoint.isEmpty() ? _peerEndpoint.toString().c_str()
                                                                : ((Int32) _socket).toString().c_str(),
                                       errno, strerror(errno)); //timeout or error happen
                _connected = false;
            }
        }
        return _connected;
#endif
    }

    bool TcpClient::hasNoError() const {
        if (_socket != -1) {
            socklen_t error = 0;
#if WIN32
            int len = sizeof(error);
#else
            socklen_t len = sizeof(error);
#endif
            if (getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char *) &error, &len) < 0) {
                Debug::writeFormatLine("getsockopt1 function'%s' failed with error code: %d, error: %s",
                                       !_peerEndpoint.isEmpty() ? _peerEndpoint.toString().c_str()
                                                                : ((Int32) _socket).toString().c_str(),
                                       errno, strerror(errno));
            } else if (error != 0) {
                Debug::writeFormatLine("getsockopt2 function'%s' socketId'%d' failed with error code: %d, error: %s",
                                       !_peerEndpoint.isEmpty() ? _peerEndpoint.toString().c_str()
                                                                : ((Int32) _socket).toString().c_str(),
                                       socketId(), error, strerror((int) error));
            } else {
                return true;
            }
        }
        return false;
    }

    bool TcpClient::isWriteSet(const TimeSpan &timeout) const {
        return isWriteSet((uint32_t) timeout.totalMilliseconds());
    }

    bool TcpClient::isWriteSet(uint32_t timeout) const {
        if (_socket != -1) {
            struct pollfd pfd_write{};
            pfd_write.fd = _socket;
            pfd_write.events = POLLOUT;
            return poll(&pfd_write, 1, (int) timeout) == 1;
        }
        return false;
    }

    void TcpClient::initializeSocket() {
        static bool initSocket = false;
        if (!initSocket) {
            initSocket = true;
#if WIN32
            // Declare and initialize variables
            WSADATA wsaData = {0};
            // Initialize Winsock
            int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0) {
                Debug::writeFormatLine("WSAStartup failed: %d", result);
                return;
            }
#elif !__EMSCRIPTEN__
            signal(SIGPIPE, SIG_IGN);
#endif
        }
    }

    int TcpClient::sendBufferSize() const {
        socklen_t bufferSize = 0;
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
#if WIN32
            int len = sizeof(bufferSize);
#else
            socklen_t len = sizeof(bufferSize);
#endif
            int result = getsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *) &bufferSize, &len);
            if (result == -1) {
                Debug::writeFormatLine("getsockopt'SO_SNDBUF' failed with error = %s", strerror(errno));
            }
        }
#endif  // __EMSCRIPTEN__
        return (int) bufferSize;
    }

    void TcpClient::setSendBufferSize(int bufferSize) {
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
            int result = setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (const char *) &bufferSize, sizeof(int));
            if (result == -1) {
                Debug::writeFormatLine("setsockopt'SO_SNDBUF' failed with error = %s", strerror(errno));
            }
        }
#endif  // __EMSCRIPTEN__
    }

    int TcpClient::receiveBufferSize() const {
        socklen_t bufferSize = 0;
#ifndef __EMSCRIPTEN__
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
#endif  // __EMSCRIPTEN__
        return (int) bufferSize;
    }

    void TcpClient::setReceiveBufferSize(int bufferSize) {
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
            int result = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (const char *) &bufferSize, sizeof(int));
            if (result == -1) {
                Debug::writeFormatLine("setsockopt'SO_RCVBUF' failed with error = %s", strerror(errno));
            }
        }
#endif  // __EMSCRIPTEN__
    }

    bool TcpClient::noDelay() const {
        socklen_t yes = 0;
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
#if WIN32
            int len = sizeof(yes);
#else
            socklen_t len = sizeof(yes);
#endif
            int result = getsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &yes, &len);
            if (result == -1) {
                Debug::writeFormatLine("getsockopt'TCP_NODELAY' failed with error = %s", strerror(errno));
            }
        }
#endif
        return yes != 0;
    }

    void TcpClient::setNoDelay(bool noDelay) {
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
            int yes = noDelay ? 1 : 0;
            int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &yes, sizeof(yes));
            if (result == -1) {
                Debug::writeFormatLine("setsockopt'TCP_NODELAY' failed with error = %s", strerror(errno));
            }
        }
#endif
    }

    bool TcpClient::setBlocking(bool blocking) {
        if (_socket != -1) {
#ifdef __EMSCRIPTEN__
            int flag = fcntl(_socket, F_GETFL, 0);
            if (flag < 0) {
                Debug::writeFormatLine("fcntl(F_GETFL) failed with error: %d\n", flag);
                return false;
            }

            if (blocking)
                flag &= ~O_NONBLOCK;
            else
                flag |= O_NONBLOCK;
            int result = fcntl(_socket, F_SETFL, flag);
            if (result != 0) {
                Debug::writeFormatLine("fcntl(F_SETFL) failed with error: %d\n", result);
            } else {
                return true;
            }
#else
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
#endif
        }
        return false;
    }

    bool TcpClient::updateEndpoints(int ai_family) {
        if (_socket != -1) {
            if (ai_family == AF_INET) {
                // Found IPv4 address.
                struct sockaddr_in sin{};
                socklen_t clen = sizeof(sin);
                memset(&sin, 0, clen);
                int result = ::getpeername(_socket, (struct sockaddr *) &sin, &clen);
                if (result != -1) {
                    _peerEndpoint.address = inet_ntoa(sin.sin_addr);
                    _peerEndpoint.port = ntohs(sin.sin_port);
//                    Debug::writeFormatLine("peer endpoint: %s", _peerEndpoint.toString().c_str());
                } else {
                    Debug::writeFormatLine("getpeername'ipv4' failed with error = %s", strerror(errno));
                    return false;
                }

#ifndef __EMSCRIPTEN__
                memset(&sin, 0, clen);
                result = ::getsockname(_socket, (struct sockaddr *) &sin, &clen);
                if (result != -1) {
                    _endpoint.address = inet_ntoa(sin.sin_addr);
                    _endpoint.port = ntohs(sin.sin_port);
//                    Trace::writeFormatLine("local endpoint: %s", _endpoint.toString().c_str());
                } else {
                    Debug::writeFormatLine("getsockname'ipv4' failed with error = %s", strerror(errno));
                }
#endif
            } else if (ai_family == AF_INET6) {
#ifdef SUPPORT_IP6
                // Found IPv6 address.
                struct sockaddr_in6 sin{};
                socklen_t clen = sizeof(sin);
                memset(&sin, 0, clen);
                int result = ::getpeername(_socket, (struct sockaddr *) &sin, &clen);
                if (result != -1) {
                    char ip[255];
                    _peerEndpoint.address = inet_ntop(PF_INET6, &sin.sin6_addr, ip, sizeof(ip));
                    _peerEndpoint.port = ntohs(sin.sin6_port);
//                    Debug::writeFormatLine("peer endpoint: %s", _peerEndpoint.toString().c_str());
                } else {
                    Debug::writeFormatLine("getpeername'ipv6' failed with error = %s", strerror(errno));
                    return false;
                }

#ifndef __EMSCRIPTEN__
                memset(&sin, 0, clen);
                result = ::getsockname(_socket, (struct sockaddr *) &sin, &clen);
                if (result != -1) {
                    char ip[255];
                    _endpoint.address = inet_ntop(PF_INET6, &sin.sin6_addr, ip, sizeof(ip));
                    _endpoint.port = ntohs(sin.sin6_port);
//                    Trace::writeFormatLine("'ipv6'local endpoint: %s", _endpoint.toString().c_str());
                } else {
                    Debug::writeFormatLine("getsockname'ipv6' failed with error = %s", strerror(errno));
                }
#endif
#endif
            }
            return true;
        }
        return false;
    }

    bool TcpClient::setReuseAddress(bool reuseAddress) {
        Debug::writeFormatLine("TcpClient.setReuseAddress, %s", reuseAddress ? "true" : "false");

        if (_socket != -1) {
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
        return false;
    }

    int TcpClient::sendTimeout() const {
#ifndef __EMSCRIPTEN__
        struct timeval timeout{0, 0};
        if (_socket != -1) {
#if WIN32
            int len = sizeof(timeout);
#else
            socklen_t len = sizeof(timeout);
#endif
            int result = getsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (char *) &timeout, &len);
            if (result == -1) {
                Debug::writeFormatLine("getsockopt'SO_SNDTIMEO' failed with error = %s", strerror(errno));
            }
        }
        return (int) (timeout.tv_sec * 1000 + timeout.tv_usec / 1000);
#else
        return 0;
#endif
    }

    void TcpClient::setSendTimeout(int timeout) {
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
            struct timeval tv{};
            tv.tv_sec = (long) timeout / 1000;
            tv.tv_usec = 1000 * (timeout % 1000);

            int result = setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *) &tv, sizeof(tv));
            if (result == -1) {
                Debug::writeFormatLine("SO_SNDTIMEO failed with error = %s", strerror(errno));
            }
        }
#endif
    }

    void TcpClient::setSendTimeout(const TimeSpan &timeout) {
        setSendTimeout((int) timeout.totalMilliseconds());
    }

    int TcpClient::receiveTimeout() const {
#ifndef __EMSCRIPTEN__
        struct timeval timeout{0, 0};
        if (_socket != -1) {
#if WIN32
            int len = sizeof(timeout);
#else
            socklen_t len = sizeof(timeout);
#endif
            int result = getsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, &len);
            if (result == -1) {
                Debug::writeFormatLine("getsockopt'SO_RCVTIMEO' failed with error = %s", strerror(errno));
            }
        }
        return (int) (timeout.tv_sec * 1000 + timeout.tv_usec / 1000);
#else
        return 0;
#endif
    }

    void TcpClient::setReceiveTimeout(int timeout) {
#ifndef __EMSCRIPTEN__
        if (_socket != -1) {
            struct timeval tv{};
            tv.tv_sec = (long) timeout / 1000;
            tv.tv_usec = 1000 * (timeout % 1000);

            int result = setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(tv));
            if (result == -1) {
                Debug::writeFormatLine("SO_RCVTIMEO failed with error = %s", strerror(errno));
            }
        }
#endif
    }

    void TcpClient::setReceiveTimeout(const TimeSpan &timeout) {
        setReceiveTimeout((int) timeout.totalMilliseconds());
    }

    bool TcpClient::decode(const ByteArray &buffer, ByteArray &plainBuffer) {
        return false;
    }

#ifndef __EMSCRIPTEN__
    const char *TcpSSLClient::ClientKey = "-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEAx+//hOd6CLTmg409wUFUcJCyP64kGRsHnnmi7lM3rQpFyi6W\nFQYlGFn0rsbEY0KQHjuETEXyXy0zxnl30Y51YaGwpZjKvcvRyPD5R3rfwLlteab/\nwI/mTSehtFxxIPYuBf3xAg9OJ1F8ZQLwDObX3Fw4A4+I8hdPobfOtw5upnkH9xiL\nnLza9zOIWpLQm+8/kCBsgiEQtwOQf3G2jaOFY2KlIU5A1cx86QaRefCGBQnAVV4B\n9+CCh591esibV29hslBhTMnlhBXpeK+LGnO0H+ItmRctxrzZUBgbPSOTG3Bds9fU\nPQI0/4LJSEWRSa4kiYhixXiJ9DHjQUVyX23KSQIDAQABAoIBAGHocAtIQ2VSGPPg\nzZX+hPgek3tFrp2ws9lyHUxx6fjN6AErMQmYmb/EBWM1KOSmquZzJMUpF5huDUWr\nI/a1uVuXD2b4XIf28INjJd9QF5RL+YUyN1m33gJ4XuJwxmFqzHzYrxXgIth0OeNJ\nPf5DaP+bFT8BIgHTUoaR+SOBaMQR8EMuGoF8c7ynkOx+O0WNdAgCoElU9lLA8jc2\nEB8LXX+gKESXQUJkPq+p3O/EvCEQBSjzErLvlMZNmoW598lAS7uuJrZ5krJ+2PV6\nsjK/4PXHql8ojIVjiJv1GEuJIdVs1THLLQeqWdbWd0bpGpjmiev+jmaGutcGTDSD\nYlww8EUCgYEA+CQNu7LVRTJcTlWVW+0nyV6xQuJMfzPgt1PYrSPWpj83t8YA6V/j\nzoFkDkyl8N+EieqO/bW2EZNbGJX0qXMQ2xo60Y10rnrqbjGKiQqa1eZSohX2hhvB\nfps3EnK7qfPgaHydI7QFquAml8sVk0AMHAZ6VUcn5NBb0c+rhNVjyUcCgYEAzkUb\nmQ91ttT7v3SJYQeSQGz3y43XNBkXFS/GgoxiReonsH7bzQKBF/yY9ZltfW7fvVx9\nCKEzCNmCgPA/Ll7cT/P8bnBukF4eXTHZZfvjdq41bf3NvQ3iz0aeK68nV+yVm8ZQ\nf+bWs5OWgMBryFJbQ6v21Dwzvf1e8tB09t4Ul+8CgYEA2JInBk4kA5vbombhz/Sh\niAhEb9qw50hw9bCRaTPhUQHV8Gg5PXZ6ji6yFCTjo+KBfBmjKV4iBR72H/QpMIeE\nC6bN9T0fvHT9DKXigo8l6lB2iI7gtdWdwJFZtJk8ny2CC8/PGm0xkRzHsi4Gq5j3\nuLISrUFIWVVc/jbhPTM9P1sCgYA6qs0+q3b/mXRA8BRi1JR0XoncC2u6FajLDduY\nJfWNjRvcxzgdBylQeTfIIutRUcexRC0L1018puDHa42M0s42r16Mys2noh1gxFzg\n9NOtXb80mYt6yghIi/1Qz3CRQWkL9UvQFQfb645S8Ge3gjUS+kYp9zWARCBNT5z0\n2hVhYQKBgQDGA1dreFOmaQBEH/8t9O2zmodYPPtsU1OHFB/r9rQo6ayrXcw0KBpE\nytlaFjzJgQXcYKeGIJx/FkcZ+gYsktPLG3FsfZqu7de2mGlb/5+dcXNmOwcaX0Fz\nnZ6DWetOGgfY3sPELMVHGpdy7FDKzBebd8KcsCK8mPWysBYYutuZsg==\n-----END RSA PRIVATE KEY-----\n";
    const char *TcpSSLClient::ClientCert = "-----BEGIN CERTIFICATE-----\nMIIDkjCCAnoCCQCQAHQSCrvUkTANBgkqhkiG9w0BAQUFADCBijELMAkGA1UEBhMC\nQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0JlaWppbmcxETAPBgNVBAoM\nCFBlcnNvbmFsMREwDwYDVQQLDAhQZXJzb25hbDEMMAoGA1UEAwwDTi9BMSMwIQYJ\nKoZIhvcNAQkBFhRkYXZpZGJhb0BuZXRlYXNlLmNvbTAeFw0yMTA1MjEwMzM2MDJa\nFw0yMjA1MjEwMzM2MDJaMIGKMQswCQYDVQQGEwJDTjEQMA4GA1UECAwHQmVpamlu\nZzEQMA4GA1UEBwwHQmVpamluZzERMA8GA1UECgwIUGVyc29uYWwxETAPBgNVBAsM\nCFBlcnNvbmFsMQwwCgYDVQQDDANOL0ExIzAhBgkqhkiG9w0BCQEWFGRhdmlkYmFv\nQG5ldGVhc2UuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAx+//\nhOd6CLTmg409wUFUcJCyP64kGRsHnnmi7lM3rQpFyi6WFQYlGFn0rsbEY0KQHjuE\nTEXyXy0zxnl30Y51YaGwpZjKvcvRyPD5R3rfwLlteab/wI/mTSehtFxxIPYuBf3x\nAg9OJ1F8ZQLwDObX3Fw4A4+I8hdPobfOtw5upnkH9xiLnLza9zOIWpLQm+8/kCBs\ngiEQtwOQf3G2jaOFY2KlIU5A1cx86QaRefCGBQnAVV4B9+CCh591esibV29hslBh\nTMnlhBXpeK+LGnO0H+ItmRctxrzZUBgbPSOTG3Bds9fUPQI0/4LJSEWRSa4kiYhi\nxXiJ9DHjQUVyX23KSQIDAQABMA0GCSqGSIb3DQEBBQUAA4IBAQAfAKD7J9ltOP7C\n9+MNojkAZp4BIigPRztPtvnmi3ycL9B39QZUG7F6s5v43VQox9KXmaX0BDqrgKCc\nQoi0dWPVIVw1c9eIEq0s8OBPyN+MsVGAk21vPEcZ9pT9Z5/cvm9a9wX2zqcRK3lm\nGkReaK5c+P1lVmqYCnSF89+Su8rYGzr7AmNHTQPALzB0mEA5YnqWCFm9aG4Kj9Al\nr/sv9WZRZHoYGFiUPMmDjE3jYoVR9G1zzkpYKH26uRFEGCXQFg5AC+eU2GgvQSfX\nHzlN63zKj+6Ah66TIUqChSI4ZJwAljgg4OdTO7ft/FKQs5BGRcZ+I3/Lcy6dVRSe\noM5tsmrf\n-----END CERTIFICATE-----\n";

    static struct InitializeTcpSSLClient {
        InitializeTcpSSLClient() {
            TcpSSLClient::initializeSSL();
        }
    } InitializeTcpSSLClient;

    TcpSSLClient::TcpSSLClient(SSLVersion version, int socketId, IPVersion ipVersion) : TcpClient(socketId, ipVersion) {
        _version = version;
        _sslContext = nullptr;
        _ssl = nullptr;
        _peek = false;

        initializeSSL();
    }

    TcpSSLClient::~TcpSSLClient() {
        TcpSSLClient::close();
    }

    void TcpSSLClient::initializeSSL() {
        static bool initSSL = false;
        if (!initSSL) {
            initSSL = true;

            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();
        }
    }

    bool TcpSSLClient::connectToHost(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress) {
        const SSL_METHOD *meth;
        switch (_version) {
            case SSLv3:
                return false;
            case SSLv23:
                meth = SSLv23_client_method();
                break;
            case TLSv1:
                meth = TLSv1_client_method();
                break;
            case TLSv1_1:
                meth = TLSv1_1_client_method();
                break;
            case TLSv1_2:
                meth = TLSv1_2_client_method();
                break;
            case DTLSv1:
                meth = DTLSv1_client_method();
                break;
            case DTLSv1_2:
                meth = DTLSv1_2_client_method();
                break;
            default:
                meth = SSLv23_client_method();
                break;
        }
        SSL_CTX *ctx = SSL_CTX_new(meth);
        if (ctx == nullptr) {
            Trace::writeLine("Can not create SSL context!", Trace::Error);
            return false;
        }

//        String rootPath = Application::instance()->rootPath();
//        rootPath = "/Volumes/Software/Temp/easesuite/test";
//        const String certf = Path::combine(rootPath, "client.crt");
//        const String keyf = Path::combine(rootPath, "client.key");
//        const String cacert = Path::combine(rootPath, "ca.crt");

//        if(File::exists(_cacert))
//        {
//            SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, nullptr);
//            SSL_CTX_load_verify_locations(ctx, _cacert, nullptr);
//        }
//
//        if(File::exists(_certf))
//        {
//            if (SSL_CTX_use_certificate_file(ctx, _certf, SSL_FILETYPE_PEM) <= 0)
//            {
//                Trace::writeLine("Failed to use the cert!", Trace::Error);
//                SSL_CTX_free(ctx);
//                return false;
//            }
//        }
//        else
//        {
//            if (!TcpSSLClient::useCertificate(ctx, ClientCert))
//            {
//                Trace::writeLine("Failed to check the cert!", Trace::Error);
//                SSL_CTX_free(ctx);
//                return false;
//            }
//        }
//
//        if(File::exists(_keyf))
//        {
//            if (SSL_CTX_use_PrivateKey_file(ctx, _keyf, SSL_FILETYPE_PEM) <= 0)
//            {
//                Trace::writeLine("Failed to use the key!", Trace::Error);
//                SSL_CTX_free(ctx);
//                return false;
//            }
//        }
//        else
//        {
//            if (!TcpSSLClient::usePrivateKey(ctx, ClientKey))
//            {
//                Trace::writeLine("Failed to check the key!", Trace::Error);
//                SSL_CTX_free(ctx);
//                return false;
//            }
//        }
//
//        if (!SSL_CTX_check_private_key(ctx))
//        {
//            Trace::writeLine("The cert and the key is not match!", Trace::Error);
//            SSL_CTX_free(ctx);
//            return false;
//        }

        if (!TcpClient::connectToHost(host, port, timeout, reuseAddress)) {
            SSL_CTX_free(ctx);
//            Debug::writeFormatLine("Can not connect the host! address: %s, port: %d", host, port);
            return false;
        }

        SSL *ssl = SSL_new(ctx);
        if (ssl == nullptr) {
            Trace::writeLine("Failed to create SSL instance!", Trace::Error);
            _connected = false;
            SSL_CTX_free(ctx);
            return false;
        }
        // socket and SSL binding
        if (SSL_set_fd(ssl, _socket) < 1) {
            Trace::writeLine("Failed to set socket id!", Trace::Error);
            _connected = false;
            SSL_CTX_free(ctx);
            return false;
        }

        uint32_t start = TickTimeout::getCurrentTickCount();
        uint32_t end = TickTimeout::getDeadTickCount(start, timeout);
        bool loop = true;
        while (loop) {
            // SSL connect
            int ret = SSL_connect(ssl);
            if (ret <= 0) {
                int error = SSL_get_error(ssl, ret);
                switch (error) {
                    case SSL_ERROR_NONE:
                    case SSL_ERROR_WANT_READ:
                    case SSL_ERROR_WANT_WRITE:
                        break;
                    default:
                        Trace::error(
                                String::format("Could not build SSL session: SSL_get_error: %d, errno: %s(%d)", error,
                                               strerror(errno), errno));
                        _connected = false;
                        SSL_CTX_free(ctx);
                        SSL_free(ssl);
                        return false;
                }

                if (TickTimeout::isTimeout(start, end)) {
                    Trace::error(String::format("Failed to connect SSL server: error: %d, errno: %s(%d)", error,
                                                strerror(errno), errno));
                    _connected = false;
                    SSL_free(ssl);
                    SSL_CTX_free(ctx);
                    return false;
                }
                Thread::msleep(100);
            } else {
                loop = false;
            }
        }

//        X509* server_cert = SSL_get_peer_certificate(ssl);
//        if(server_cert != nullptr)
//        {
////#ifdef DEBUG
////            Debug::writeFormatLine("server cert, name: %s, issuer: %s",
////                                   X509_get_subject_name(server_cert), 0, 0),
////                                X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0));
////#endif
//            X509_free(server_cert);
//        }
//        else
//        {
//            Trace::writeLine("Can not retrieve cert from server!");
//            _connected = false;
//            SSL_CTX_free(ctx);
//            SSL_free(ssl);
//            return false;
//        }

        _connected = true;
        _sslContext = ctx;
        _ssl = ssl;

        return true;
    }

    void TcpSSLClient::close() {
        if (_ssl != nullptr)
            SSL_shutdown((SSL *) _ssl);

        TcpClient::close();

        if (_ssl != nullptr)
            SSL_free((SSL *) _ssl);
        if (_sslContext != nullptr)
            SSL_CTX_free((SSL_CTX *) _sslContext);

        _sslContext = nullptr;
        _ssl = nullptr;
    }

    ssize_t TcpSSLClient::write(const uint8_t *data, size_t count) {
#ifdef DEBUG
        Stopwatch sw("TcpSSLClient::write", 1000);
#endif
        if (_ssl != nullptr) {
            int ires = 0, icount = 0;
            bool isCoutinue = true;
            while (isCoutinue) {
                ires = SSL_write((SSL *) _ssl, data + icount, (int) (count - icount));
                int nRes = SSL_get_error((SSL *) _ssl, ires);
                if (nRes == SSL_ERROR_NONE) {
                    if (ires > 0) {
                        if (icount >= (ssize_t) count) {
                            break;
                        }
                        icount += ires;
                        if (icount >= (ssize_t) count) {
                            break;
                        }
                        continue;
                    }
                } else if (nRes == SSL_ERROR_WANT_READ) {
                    continue;
                } else {
                    break;
                }
            }

            return (ssize_t) count;
        }
        return 0;
    }

    ssize_t TcpSSLClient::read(uint8_t *data, size_t count) {
#ifdef DEBUG
        Stopwatch sw("TcpSSLClient::read", 1000);
#endif
        if (_ssl != nullptr) {
            int length = SSL_read((SSL *) _ssl, data, (int) count);
            return length >= 0 ? length : 0;
        }
        return 0;
    }

    ssize_t TcpSSLClient::peek(uint8_t *data, size_t count) {
#ifdef DEBUG
        Stopwatch sw("TcpSSLClient::peek", 1000);
#endif
        if (_ssl != nullptr) {
            int ires = 0, icount = 0;
            bool isCoutinue = true;
            while (isCoutinue) {
                ires = SSL_peek((SSL *) _ssl, data + icount, (int) (count - icount));
                int nRes = SSL_get_error((SSL *) _ssl, ires);
                if (nRes == SSL_ERROR_NONE) {
                    if (ires > 0) {
                        icount += ires;
                        if (icount >= (ssize_t) count) {
                            break;
                        }
                        continue;
                    }
                } else {
                    break;
                }
            }

            return (ssize_t) count;
        }
        return 0;
    }

    size_t TcpSSLClient::available() {
        size_t available = 0;
        if (_ssl != nullptr) {
            available = SSL_pending((SSL *) _ssl);
            if (available == 0)
                return TcpClient::available();
        }
        return available;
    }

    void TcpSSLClient::setSSLContext(void *context) {
        _sslContext = context;
    }

    void TcpSSLClient::setSSL(void *ssl) {
        _ssl = ssl;
    }

    bool TcpSSLClient::usePrivateKey(void *context, const char *keyStr) {
        auto ctx = (SSL_CTX *) context;
        assert(ctx);

        BIO *in = nullptr;
        if ((in = BIO_new_mem_buf(keyStr, -1)) == nullptr)
            return false;
//        BIO*in = BIO_new(BIO_s_file_internal());
//        if (in == nullptr) {
//            SSLerr(SSL_F_SSL_CTX_USE_PRIVATEKEY_FILE, ERR_R_BUF_LIB);
//            return false;
//        }
//
//        if (BIO_read_filename(in, keyf.c_str()) <= 0) {
//            SSLerr(SSL_F_SSL_CTX_USE_PRIVATEKEY_FILE, ERR_R_SYS_LIB);
//            return false;
//        }
        EVP_PKEY *pkey = PEM_read_bio_PrivateKey(in, nullptr, nullptr, nullptr);
        if (pkey == nullptr) {
            if (in != nullptr)
                BIO_free(in);
            return false;
        }
        if (SSL_CTX_use_PrivateKey(ctx, pkey) <= 0)
            // if (SSL_CTX_use_PrivateKey_file(ctx, keyf, SSL_FILETYPE_PEM) <= 0)
        {
            Trace::writeLine("Failed to use the key!", Trace::Error);
            EVP_PKEY_free(pkey);
            if (in != nullptr)
                BIO_free(in);
            return false;
        }

        EVP_PKEY_free(pkey);
        if (in != nullptr)
            BIO_free(in);

        return true;
    }

    bool TcpSSLClient::useCertificate(void *context, const char *certStr) {
        auto ctx = (SSL_CTX *) context;
        assert(ctx);

        BIO *in = nullptr;
        if ((in = BIO_new_mem_buf(certStr, -1)) == nullptr)
            return false;
//        BIO*in = BIO_new(BIO_s_file_internal());
//        if (in == nullptr) {
//            SSLerr(SSL_F_SSL_CTX_USE_PRIVATEKEY_FILE, ERR_R_BUF_LIB);
//            return false;
//        }
//
//        if (BIO_read_filename(in, keyf.c_str()) <= 0) {
//            SSLerr(SSL_F_SSL_CTX_USE_PRIVATEKEY_FILE, ERR_R_SYS_LIB);
//            return false;
//        }
        X509 *x = PEM_read_bio_X509(in, nullptr, nullptr, nullptr);
        if (x == nullptr) {
            if (in != nullptr)
                BIO_free(in);
            return false;
        }
        if (SSL_CTX_use_certificate(ctx, x) <= 0)
            // if (SSL_CTX_use_certificate_file(ctx, certf, SSL_FILETYPE_PEM) <= 0)
        {
            Trace::writeLine("Failed to use the cert!", Trace::Error);
            X509_free(x);
            if (in != nullptr)
                BIO_free(in);
            return false;
        }

        X509_free(x);
        if (in != nullptr)
            BIO_free(in);

        return true;
    }

    void TcpSSLClient::setCertFile(const String &file) {
        _certf = file;
    }

    void TcpSSLClient::setKeyFile(const String &file) {
        _keyf = file;
    }

    void TcpSSLClient::setCACertFile(const String &file) {
        _cacert = file;
    }

    SSLVersion TcpSSLClient::sslVersion() const {
        return _version;
    }

    bool TcpSSLClient::peek() const {
        return _peek;
    }

    void TcpSSLClient::setPeek(bool peek) {
        _peek = peek;
    }

//    bool TcpSSLClient::decode(const ByteArray &buffer, ByteArray &plainBuffer) {
//        if (_ssl != nullptr) {
//            BIO *rbio = BIO_new(BIO_s_mem());
////        BIO *wbio = BIO_new(BIO_s_mem());
//            SSL_set_bio((SSL *) _ssl, rbio, rbio);
//
//            int wLen = BIO_write(rbio, buffer.data(), buffer.count());
//            uint8_t bytes[1024];
//            int len = SSL_read((SSL *) _ssl, bytes, 1024);
//            if (len > 0) {
//                plainBuffer = ByteArray(bytes, len);
//                return true;
//            }
//        }
//        return false;
//    }

    const TimeSpan WebSocketClient::MinReceiveTimeout = TimeSpan::fromSeconds(1);

    WebSocketClient::WebSocketClient(int sockId, IPVersion ipVersion, const TimeSpan &receiveTimeout) : TcpClient(
            sockId, ipVersion) {
        _position = -1;
        _decoding = true;
        _encoding = true;
        _isClosing = false;
        _receiveTimeout = receiveTimeout < MinReceiveTimeout ? MinReceiveTimeout : receiveTimeout;
    }

    WebSocketClient::~WebSocketClient() = default;

    void WebSocketClient::disableDecoding() {
        _decoding = false;
    }

    void WebSocketClient::enableDecoding() {
        _decoding = true;
    }

    size_t WebSocketClient::available() {
        if (_buffer.count() > 0 &&
            _position >= 0 && _position < (int) _buffer.count()) {
            size_t bufferCount = _buffer.count() - _position;
            return bufferCount + 6;     // buffer count + header count
        }
        return TcpClient::available();
    }

    void WebSocketClient::clearReceiveBuffer() {
        Locker locker(&_bufferMutex);

        _buffer.clear();
        _position = 0;

        TcpClient::clearReceiveBuffer();
    }

    bool WebSocketClient::isClosing() const {
        return _isClosing;
    }

    ssize_t WebSocketClient::write(const uint8_t *data, size_t count) {
        if (!_encoding)
            return TcpClient::write(data, count);

        ByteArray original(data, count);
        ByteArray encoded;
        int frameType = encodeFrame(original, encoded, WS_BINARY_FRAME);
        if (frameType == WS_ERROR_FRAME) {
            Debug::writeLine("Failed to encode data by web socket");
        }

//        Debug::writeFormatLine("original: '%s'", original.toString().c_str());
//        Debug::writeFormatLine("encoded: '%s'", encoded.toString().c_str());

        return TcpClient::write(encoded.data(), encoded.count());
    }

    ssize_t WebSocketClient::read(uint8_t *data, size_t count) {
        if (!_decoding)
            return TcpClient::read(data, count);

        Locker locker(&_bufferMutex);

        if (_buffer.count() == 0) {
            size_t available = this->available();
//            Trace::writeFormatLine("web socket available: %d", count);
            if (available > 0) {
                disableDecoding();
                bool result = readFrame();
                enableDecoding();
                if (!result)
                    return -1;
            }
        }

        if (_position >= 0 && _position < (int) _buffer.count()) {
            ssize_t bufferCount = (ssize_t) _buffer.count() - _position;
            if ((ssize_t) count >= bufferCount) {
                memcpy(data, _buffer.data() + _position, bufferCount);
                _buffer.clear();
                _position = 0;
                return bufferCount + 6;     // buffer count + header count
            } else {
                memcpy(data, _buffer.data() + _position, count);
                _position += (off_t) count;
                return (ssize_t) count;
            }
        }
        return -1;
    }

    int WebSocketClient::sendWithoutEncoding(const uint8_t *buffer, off_t offset, int count) {
        _encoding = false;
        bool result = this->send(buffer, offset, count);
        _encoding = true;
        return result;
    }

    bool WebSocketClient::readFrame() {
        if (isClosing())
            return false;

        ByteArray original;
        static const int wsHeaderLength = 2;
        ssize_t headerLength = this->receiveBySize(&original, wsHeaderLength, _receiveTimeout);
        if (wsHeaderLength != headerLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine(
                    "web socket'%d' receive error! header length is incorrect! expected length: %d, recv: '%s'",
                    _socket, wsHeaderLength, messages.toString().c_str());
#endif
            return false;
        }
        const int header2Length = ((original[1] & 0x7f) == 0x7e) ? 6 : 4;
        headerLength = this->receiveBySize(&original, header2Length, _receiveTimeout);
        if (header2Length != headerLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine(
                    "web socket'%d' receive error! header2 length is incorrect! expected length: %d, recv: '%s'",
                    _socket, header2Length, messages.toString().c_str());
#endif
            return false;
        }

        uint64_t payloadLength = 0;
        FrameType frameType = getFrameLength(original, payloadLength);
        if (frameType == WS_CLOSING_FRAME) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("web socket'%d' receive close frame! recv: '%s'",
                                   _socket, messages.toString().c_str());
#endif
            _isClosing = true;
            return false;
        } else if (frameType == WS_ERROR_FRAME) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("web socket'%d' receive error: frame is incorrect! recv: %s",
                                   _socket, messages.toString().c_str());
#endif
            return false;
        }

        uint64_t validLength = this->receiveBySize(&original, payloadLength, _receiveTimeout);
        if (payloadLength != validLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("web socket'%d' receive error: length is incorrect! expected length: %d, recv: %s",
                                   _socket, payloadLength, messages.toString().c_str());
#endif
            return false;
        }

        if (!decodeFrame(original, _buffer)) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("failed to decode socket'%d' original buffer: '%s'", _socket,
                                   messages.toString().c_str());
#endif
            return false;
        }

//        Debug::writeFormatLine("original buffer: '%s', decoded buffer: '%s'", original.toString().c_str(), _buffer.toString().c_str());
        _position = 0;
        return true;
    }

    WebSocketClient::FrameType WebSocketClient::getFrameLength(const ByteArray &inFrame, uint64_t &payloadLength) {
        int frameType = WS_OPENING_FRAME;
        const uint8_t *frameData = inFrame.data();
        const size_t frameLength = inFrame.count();
        if (frameLength < 2) {
            frameType = WS_ERROR_FRAME;
        }

        if ((frameData[0] & 0x70) != 0x0) {
            frameType = WS_ERROR_FRAME;
        }

        frameType = (frameData[0] & 0x80);
        if ((frameData[0] & 0x80) != 0x80) {
            frameType = WS_ERROR_FRAME;
        }

        if ((frameData[1] & 0x80) != 0x80) {
            frameType = WS_ERROR_FRAME;
        }

        uint8_t payloadFieldExtraBytes = 0;
        auto opcode = (uint8_t) (frameData[0] & 0x0f);
        if (opcode == WS_BINARY_FRAME || opcode == WS_TEXT_FRAME) {
            payloadLength = static_cast<uint16_t>(frameData[1] & 0x7f);
            if (payloadLength == 0x7e) {
                uint16_t payloadLength16b = 0;
                payloadFieldExtraBytes = 2;
                memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength16b);
            } else if (payloadLength == 0x7f) {
//                ret = WS_ERROR_FRAME;
                uint64_t payloadLength64b = 0;
                payloadFieldExtraBytes = 8;
                memcpy(&payloadLength64b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength64b);
            }
        } else if (opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME) {
            // skip ping/pong
        } else if (opcode == WS_CLOSING_FRAME) {
            frameType = WS_CLOSING_FRAME;
        } else {
            frameType = WS_ERROR_FRAME;
        }
        return (FrameType) frameType;
    }

    bool WebSocketClient::decodeFrame(const ByteArray &inFrame, ByteArray &outMessage) {
        int ret = WS_OPENING_FRAME;
        const uint8_t *frameData = inFrame.data();
        const size_t frameLength = inFrame.count();
//#ifdef DEBUG
//        Debug::writeFormatLine("decodeFrame, inFrame: %s", inFrame.toString().c_str());
//#endif
        if (frameLength < 2) {
            ret = WS_ERROR_FRAME;
        }

        if ((frameData[0] & 0x70) != 0x0) {
            ret = WS_ERROR_FRAME;
        }

        ret = (frameData[0] & 0x80);
        if ((frameData[0] & 0x80) != 0x80) {
            ret = WS_ERROR_FRAME;
        }

        if ((frameData[1] & 0x80) != 0x80) {
            ret = WS_ERROR_FRAME;
        }

        uint64_t payloadLength = 0;
        uint8_t payloadFieldExtraBytes = 0;
        auto opcode = (uint8_t) (frameData[0] & 0x0f);
        if (opcode == WS_BINARY_FRAME || opcode == WS_TEXT_FRAME) {
            payloadLength = static_cast<uint64_t>(frameData[1] & 0x7f);
            if (payloadLength == 0x7e) {
                uint16_t payloadLength16b = 0;
                payloadFieldExtraBytes = 2;
                memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength16b);
            } else if (payloadLength == 0x7f) {
//                ret = WS_ERROR_FRAME;
                uint64_t payloadLength64b = 0;
                payloadFieldExtraBytes = 8;
                memcpy(&payloadLength64b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength64b);
            }
        } else if (opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME) {
            // skip ping/pong
        } else if (opcode == WS_CLOSING_FRAME) {
            ret = WS_CLOSING_FRAME;
        } else {
            ret = WS_ERROR_FRAME;
        }

        // decoding
        if ((ret != WS_ERROR_FRAME) && (payloadLength > 0)) {
            // header: 2 bytes, masking key: 4 bytes
            const uint8_t *maskingKey = &frameData[2 + payloadFieldExtraBytes];
            auto payloadData = new uint8_t[payloadLength];
            memset(payloadData, 0, payloadLength);
            memcpy(payloadData, &frameData[2 + payloadFieldExtraBytes + 4], payloadLength);
            for (uint64_t i = 0; i < payloadLength; i++) {
                payloadData[i] = payloadData[i] ^ maskingKey[i % 4];
            }

            outMessage.addRange(payloadData, payloadLength);
//#ifdef DEBUG
//            Debug::writeFormatLine("decodeFrame, recv: %s", outMessage.toString().c_str());
//#endif
            delete[] payloadData;
        }
        return !(ret == WS_ERROR_FRAME || ret == WS_CLOSING_FRAME);
    }

    int WebSocketClient::encodeFrame(const ByteArray &inMessage, ByteArray &outFrame, FrameType frameType) {
        int ret = WS_EMPTY_FRAME;
        const size_t messageLength = inMessage.count();
//        if (messageLength > (uint32_t)UInt16::MaxValue)
//        {
//            return WS_ERROR_FRAME;
//        }

        uint8_t payloadFieldExtraBytes;
        if (messageLength <= 0x7d) {
            payloadFieldExtraBytes = 0;
        } else if (messageLength <= 0xffff) {
            payloadFieldExtraBytes = 2;
        } else {
            payloadFieldExtraBytes = 8;
        }
        uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
        auto frameHeader = new uint8_t[frameHeaderSize];
        memset(frameHeader, 0, frameHeaderSize);
        frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);

        if (messageLength <= 0x7d) {
            frameHeader[1] = static_cast<uint8_t>(messageLength);
        } else if (messageLength <= 0xffff) {
            frameHeader[1] = 0x7e;
            uint16_t len = htons((uint16_t) messageLength);
            memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
        } else {
            frameHeader[1] = 0x7f;
            memcpy(&frameHeader[2], &messageLength, payloadFieldExtraBytes);
        }

        size_t frameSize = frameHeaderSize + messageLength;
        auto frame = new uint8_t[frameSize];
        memcpy(frame, frameHeader, frameHeaderSize);
        memcpy(frame + frameHeaderSize, inMessage.data(), messageLength);
        outFrame.addRange(frame, frameSize);

        delete[] frame;
        delete[] frameHeader;
        return ret;
    }

    const TimeSpan WebSocketSSLClient::MinReceiveTimeout = TimeSpan::fromSeconds(1);

    WebSocketSSLClient::WebSocketSSLClient(SSLVersion version, int sockId, IPVersion ipVersion,
                                           const TimeSpan &receiveTimeout)
            : TcpSSLClient(version, sockId, ipVersion) {
        _position = -1;
        _decoding = true;
        _encoding = true;
        _isClosing = false;
        _receiveTimeout = receiveTimeout < MinReceiveTimeout ? MinReceiveTimeout : receiveTimeout;
    }

    WebSocketSSLClient::~WebSocketSSLClient() = default;

    void WebSocketSSLClient::disableDecoding() {
        _decoding = false;
    }

    void WebSocketSSLClient::enableDecoding() {
        _decoding = true;
    }

    bool WebSocketSSLClient::isClosing() const {
        return _isClosing;
    }

    size_t WebSocketSSLClient::available() {
        if (_buffer.count() > 0 &&
            _position >= 0 && _position < (off_t) _buffer.count()) {
            size_t bufferCount = _buffer.count() - _position;
            return bufferCount + 6;     // buffer count + header count
        }
        return TcpSSLClient::available();
    }

    void WebSocketSSLClient::clearReceiveBuffer() {
        Locker locker(&_bufferMutex);

        _buffer.clear();
        _position = 0;

        TcpSSLClient::clearReceiveBuffer();
    }

    ssize_t WebSocketSSLClient::write(const uint8_t *data, size_t count) {
        if (!_encoding)
            return TcpSSLClient::write(data, count);

        ByteArray original(data, count);
        ByteArray encoded;
        int frameType = encodeFrame(original, encoded, WS_BINARY_FRAME);
        if (frameType == WS_ERROR_FRAME) {
            Debug::writeLine("Failed to encode data by web socket");
        }

        //        Debug::writeFormatLine("original: '%s'", original.toString().c_str());
        //        Debug::writeFormatLine("encoded: '%s'", encoded.toString().c_str());

        return TcpSSLClient::write(encoded.data(), encoded.count());
    }

    ssize_t WebSocketSSLClient::read(uint8_t *data, size_t count) {
        if (!_decoding)
            return TcpSSLClient::read(data, count);

        Locker locker(&_bufferMutex);

        if (_buffer.count() == 0) {
            size_t available = this->available();
//            Trace::writeFormatLine("web ssl socket available: %d", available);
            if (available > 0) {
                disableDecoding();
                bool result = readFrame();
                enableDecoding();
                if (!result)
                    return -1;
            }
        }

        if (_position >= 0 && _position < (int) _buffer.count()) {
            ssize_t bufferCount = (ssize_t) _buffer.count() - _position;
            if ((ssize_t) count >= bufferCount) {
                memcpy(data, _buffer.data() + _position, bufferCount);
                _buffer.clear();
                _position = 0;
                return bufferCount + 6;     // buffer count + header count
            } else {
                memcpy(data, _buffer.data() + _position, count);
                _position += (off_t) count;
                //#ifdef DEBUG
                //                ByteArray messages(data, maxlen);
                //                Debug::writeFormatLine("tcp receive once, recv: %s", messages.toString().c_str());
                //#endif
                return (ssize_t) count;
            }
        }
        return -1;
    }

    ssize_t WebSocketSSLClient::sendWithoutEncoding(const uint8_t *buffer, off_t offset, size_t count) {
        _encoding = false;
        bool result = this->send(buffer, offset, count);
        _encoding = true;
        return result;
    }

    bool WebSocketSSLClient::readFrame() {
        if (isClosing())
            return false;

        ByteArray original;
        static const int wsHeaderLength = 2;
        ssize_t headerLength = this->receiveBySize(&original, wsHeaderLength, _receiveTimeout);
        if (wsHeaderLength != headerLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine(
                    "web ssl socket'%d' receive error! header length is incorrect! expected length: %d, recv: '%s'",
                    _socket, wsHeaderLength, messages.toString().c_str());
#endif
            return false;
        }
        const int header2Length = ((original[1] & 0x7f) == 0x7e) ? 6 : 4;
        headerLength = this->receiveBySize(&original, header2Length, _receiveTimeout);
        if (header2Length != headerLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine(
                    "web ssl socket'%d' receive error! header2 length is incorrect! expected length: %d, recv: '%s'",
                    _socket, header2Length, messages.toString().c_str());
#endif
            return false;
        }

        uint64_t payloadLength = 0;
        FrameType frameType = getFrameLength(original, payloadLength);
        if (frameType == WS_CLOSING_FRAME) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("web ssl socket'%d' receive close frame! recv: '%s'",
                                   _socket, messages.toString().c_str());
#endif
            _isClosing = true;
            return false;
        } else if (frameType == WS_ERROR_FRAME) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("web ssl socket'%d receive error: frame is incorrect! recv: %s",
                                   _socket, messages.toString().c_str());
#endif
            return false;
        }

        uint64_t validLength = this->receiveBySize(&original, payloadLength, _receiveTimeout);
        if (payloadLength != validLength) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine(
                    "web ssl socket'%d receive error: length is incorrect! expected length: %d, recv: %s",
                    _socket, payloadLength, messages.toString().c_str());
#endif
            return false;
        }

        if (!decodeFrame(original, _buffer)) {
#ifdef DEBUG
            ByteArray messages(original.data(), Math::min((int) original.count(), 128));
            Debug::writeFormatLine("failed to decode socket'%d original buffer: '%s'",
                                   _socket, messages.toString().c_str());
#endif
            return false;
        }

//        Debug::writeFormatLine("original buffer: '%s', decoded buffer: '%s'", original.toString().c_str(), _buffer.toString().c_str());
        _position = 0;
        return true;
    }

    WebSocketSSLClient::FrameType
    WebSocketSSLClient::getFrameLength(const ByteArray &inFrame, uint64_t &payloadLength) {
        int frameType = WS_OPENING_FRAME;
        const uint8_t *frameData = inFrame.data();
        const size_t frameLength = inFrame.count();
        if (frameLength < 2) {
            frameType = WS_ERROR_FRAME;
        }

        if ((frameData[0] & 0x70) != 0x0) {
            frameType = WS_ERROR_FRAME;
        }

        frameType = (frameData[0] & 0x80);
        if ((frameData[0] & 0x80) != 0x80) {
            frameType = WS_ERROR_FRAME;
        }

        if ((frameData[1] & 0x80) != 0x80) {
            frameType = WS_ERROR_FRAME;
        }

        uint8_t payloadFieldExtraBytes = 0;
        auto opcode = (uint8_t) (frameData[0] & 0x0f);
        if (opcode == WS_BINARY_FRAME || opcode == WS_TEXT_FRAME) {
            payloadLength = static_cast<uint16_t>(frameData[1] & 0x7f);
            if (payloadLength == 0x7e) {
                uint16_t payloadLength16b = 0;
                payloadFieldExtraBytes = 2;
                memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength16b);
            } else if (payloadLength == 0x7f) {
//                ret = WS_ERROR_FRAME;
                uint64_t payloadLength64b = 0;
                payloadFieldExtraBytes = 8;
                memcpy(&payloadLength64b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength64b);
            }
        } else if (opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME) {
            // skip ping/pong
        } else if (opcode == WS_CLOSING_FRAME) {
            frameType = WS_CLOSING_FRAME;
        } else {
            frameType = WS_ERROR_FRAME;
        }
        return (FrameType) frameType;
    }

    bool WebSocketSSLClient::decodeFrame(const ByteArray &inFrame, ByteArray &outMessage) {
        int ret = WS_OPENING_FRAME;
        const uint8_t *frameData = inFrame.data();
        const size_t frameLength = inFrame.count();
//#ifdef DEBUG
//        Debug::writeFormatLine("decodeFrame, inFrame: %s", inFrame.toString().c_str());
//#endif
        if (frameLength < 2) {
            ret = WS_ERROR_FRAME;
        }

        if ((frameData[0] & 0x70) != 0x0) {
            ret = WS_ERROR_FRAME;
        }

        ret = (frameData[0] & 0x80);
        if ((frameData[0] & 0x80) != 0x80) {
            ret = WS_ERROR_FRAME;
        }

        if ((frameData[1] & 0x80) != 0x80) {
            ret = WS_ERROR_FRAME;
        }

        uint64_t payloadLength = 0;
        uint8_t payloadFieldExtraBytes = 0;
        auto opcode = (uint8_t) (frameData[0] & 0x0f);
        if (opcode == WS_BINARY_FRAME || opcode == WS_TEXT_FRAME) {
            payloadLength = static_cast<uint64_t>(frameData[1] & 0x7f);
            if (payloadLength == 0x7e) {
                uint16_t payloadLength16b = 0;
                payloadFieldExtraBytes = 2;
                memcpy(&payloadLength16b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength16b);
            } else if (payloadLength == 0x7f) {
//                ret = WS_ERROR_FRAME;
                uint64_t payloadLength64b = 0;
                payloadFieldExtraBytes = 8;
                memcpy(&payloadLength64b, &frameData[2], payloadFieldExtraBytes);
                payloadLength = ntohs(payloadLength64b);
            }
        } else if (opcode == WS_PING_FRAME || opcode == WS_PONG_FRAME) {
            // skip ping/pong
        } else if (opcode == WS_CLOSING_FRAME) {
            ret = WS_CLOSING_FRAME;
        } else {
            ret = WS_ERROR_FRAME;
        }

        // decoding
        if ((ret != WS_ERROR_FRAME) && (payloadLength > 0)) {
            // header: 2 bytes, masking key: 4 bytes
            const uint8_t *maskingKey = &frameData[2 + payloadFieldExtraBytes];
            auto payloadData = new uint8_t[payloadLength];
            memset(payloadData, 0, payloadLength);
            memcpy(payloadData, &frameData[2 + payloadFieldExtraBytes + 4], payloadLength);
            for (uint64_t i = 0; i < payloadLength; i++) {
                payloadData[i] = payloadData[i] ^ maskingKey[i % 4];
            }

            outMessage.addRange(payloadData, payloadLength);
//#ifdef DEBUG
//            Debug::writeFormatLine("decodeFrame, recv: %s", outMessage.toString().c_str());
//#endif
            delete[] payloadData;
        }
        return !(ret == WS_ERROR_FRAME || ret == WS_CLOSING_FRAME);
    }

    int WebSocketSSLClient::encodeFrame(const ByteArray &inMessage, ByteArray &outFrame, FrameType frameType) {
        int ret = WS_EMPTY_FRAME;
        const size_t messageLength = inMessage.count();
//        if (messageLength > (uint32_t)UInt16::MaxValue)
//        {
//            return WS_ERROR_FRAME;
//        }

        uint8_t payloadFieldExtraBytes;
        if (messageLength <= 0x7d) {
            payloadFieldExtraBytes = 0;
        } else if (messageLength <= 0xffff) {
            payloadFieldExtraBytes = 2;
        } else {
            payloadFieldExtraBytes = 8;
        }
        uint8_t frameHeaderSize = 2 + payloadFieldExtraBytes;
        auto frameHeader = new uint8_t[frameHeaderSize];
        memset(frameHeader, 0, frameHeaderSize);
        frameHeader[0] = static_cast<uint8_t>(0x80 | frameType);

        if (messageLength <= 0x7d) {
            frameHeader[1] = static_cast<uint8_t>(messageLength);
        } else if (messageLength <= 0xffff) {
            frameHeader[1] = 0x7e;
            uint16_t len = htons((uint16_t) messageLength);
            memcpy(&frameHeader[2], &len, payloadFieldExtraBytes);
        } else {
            frameHeader[1] = 0x7f;
            memcpy(&frameHeader[2], &messageLength, payloadFieldExtraBytes);
        }

        size_t frameSize = frameHeaderSize + messageLength;
        auto frame = new uint8_t[frameSize];
        memcpy(frame, frameHeader, frameHeaderSize);
        memcpy(frame + frameHeaderSize, inMessage.data(), messageLength);
        outFrame.addRange(frame, frameSize);

        delete[] frame;
        delete[] frameHeader;
        return ret;
    }

#endif
}
