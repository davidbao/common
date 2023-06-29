//
//  TcpServer.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef TcpServer_h
#define TcpServer_h

#include "data/StringArray.h"
#include "data/String.h"
#include "data/TimeSpan.h"
#include "TcpClient.h"

namespace Net {
    class TcpServer {
    public:
        TcpServer();

        virtual ~TcpServer();

        bool bind(const Endpoint &endpoint, bool reuseAddress = false);

        bool bind(const String &address, int port, bool reuseAddress = false);

        bool listen();

        TcpClient *accept(const TimeSpan &timeout = TimeSpan::fromSeconds(3));

        void close();

        bool setBlocking(bool blocking = false);

        bool isValid() const;

        bool isListening() const;

        int socketId() const;

        int sendBufferSize() const;

        void setSendBufferSize(int bufferSize);

        int receiveBufferSize() const;

        void setReceiveBufferSize(int bufferSize);

        int sendTimeout() const;

        void setSendTimeout(uint32_t timeout);

        void setSendTimeout(const TimeSpan &timeout);

        int receiveTimeout() const;

        void setReceiveTimeout(uint32_t timeout);

        void setReceiveTimeout(const TimeSpan &timeout);

        bool noDelay() const;

        void setNoDelay(bool noDelay = true);

        bool setReuseAddress(bool reuseAddress = true);

    protected:
        virtual bool handshaking(TcpClient *client, const TimeSpan &timeout);

        bool handshaking(TcpClient *client);

        virtual TcpClient *createClient(int sockId) const;

        virtual int backlog() const;

    protected:
        int _socket;
        bool _isListening;

        TimeSpan _sendTimeout;
        TimeSpan _recvTimeout;
    };

#ifndef __EMSCRIPTEN__

    class TcpSSLServer : public TcpServer {
    public:
        explicit TcpSSLServer(SSLVersion version = SSLVersion::SSLv23);

        ~TcpSSLServer() override;

        void setCertFile(const String &file);

        void setKeyFile(const String &file);

        void setCACertFile(const String &file);

    protected:
        using TcpServer::handshaking;
        
        bool handshaking(TcpClient *client, const TimeSpan &timeout) override;

        TcpClient *createClient(int sockId) const override;

        int backlog() const override;

    protected:
        SSLVersion _version;

        String _certf;
        String _keyf;
        String _cacert;

    public:
        static const char *ServerKey;
        static const char *ServerCert;
    };

    class WebSocketServer : public TcpServer {
    public:
        WebSocketServer();

        ~WebSocketServer() override;

    protected:
        using TcpServer::handshaking;

        bool handshaking(TcpClient *client, const TimeSpan &timeout) override;

        TcpClient *createClient(int sockId) const override;
    };

    class WebSocketSSLServer : public TcpSSLServer {
    public:
        explicit WebSocketSSLServer(SSLVersion version = SSLVersion::SSLv23);

        ~WebSocketSSLServer() override;

    protected:
        using TcpServer::handshaking;

        bool handshaking(TcpClient *client, const TimeSpan &timeout) override;

        TcpClient *createClient(int sockId) const override;
    };

#endif
}

#endif // TcpServer_h
