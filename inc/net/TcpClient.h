//
//  TcpClient.h
//  common
//
//  Created by baowei on 2016/12/31.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef TcpClient_h
#define TcpClient_h

#include "data/String.h"
#include "data/ByteArray.h"
#include "data/TimeSpan.h"
#include "net/NetType.h"
#include "net/Sender.h"
#include "net/Receiver.h"
#include "thread/Mutex.h"

namespace Net {
    class TcpClient : public Sender, public Receiver {
    public:
        using Sender::send;
        using Receiver::receive;

        enum IPVersion {
            IPV4 = 0,
            IPV6 = 1
        };

        explicit TcpClient(int sockId = -1, IPVersion ipVersion = IPVersion::IPV4);

        ~TcpClient() override;

        bool connected() override;

        bool useReceiveTimeout() const override;

        size_t available() override;

        ssize_t send(const uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) override;

        ssize_t receive(ByteArray *buffer, size_t count, uint32_t timeout) override;

        virtual ssize_t write(const uint8_t *data, size_t count);

        ssize_t write(const ByteArray &data);

        ssize_t write(const String &str);

        virtual ssize_t read(uint8_t *data, size_t count);

        virtual bool connectToHost(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress);

        bool connectToHost(const String &host, uint16_t port, uint32_t timeout = 3000);

        bool connectToHost(const String &host, uint16_t port, const TimeSpan &timeout, bool reuseAddress = false);

        bool connectToHost(const Endpoint &host, const TimeSpan &timeout = TimeSpan::fromSeconds(3), bool reuseAddress = false);

        virtual void close();

        const Endpoint &peerEndpoint() const;

        const Endpoint &endpoint() const;

        int socketId() const;

        virtual bool isClosing() const;

        int sendBufferSize() const;

        void setSendBufferSize(int bufferSize);

        int receiveBufferSize() const;

        void setReceiveBufferSize(int bufferSize);

        int sendTimeout() const;

        void setSendTimeout(int timeout);

        void setSendTimeout(const TimeSpan &timeout);

        int receiveTimeout() const;

        void setReceiveTimeout(int timeout);

        void setReceiveTimeout(const TimeSpan &timeout);

        bool noDelay() const;

        void setNoDelay(bool noDelay = true);

        bool setBlocking(bool blocking = false);

        bool setReuseAddress(bool reuseAddress = true);

        virtual bool decode(const ByteArray &buffer, ByteArray &plainBuffer);

    public:
        static void initializeSocket();

    private:
        bool updateEndpoints(int ai_family);

        bool connectToHost_IPV4(const String &host, uint16_t port, uint32_t timeout = 3000, bool reuseAddress = false);

        bool connectToHost_IPV6(const String &host, uint16_t port, uint32_t timeout = 3000, bool reuseAddress = false);

        bool hasNoError() const;

        bool isWriteSet(const TimeSpan &timeout) const;

        bool isWriteSet(uint32_t timeout) const;

    protected:
        int _socket;
        bool _connected;

        Endpoint _peerEndpoint;
        Endpoint _endpoint;
    };

    enum SSLVersion : uint8_t {
        SSLNone = 0,
        SSLv3,
        SSLv23,
        TLSv1,
        TLSv1_1,
        TLSv1_2,
        DTLSv1,
        DTLSv1_2,
    };

#ifndef __EMSCRIPTEN__

    class TcpSSLServer;

    class TcpSSLClient : public TcpClient {
    public:
        explicit TcpSSLClient(SSLVersion version = SSLVersion::SSLv23, int sockId = -1, IPVersion ipVersion = IPVersion::IPV4);

        ~TcpSSLClient() override;

        bool connectToHost(const String &host, uint16_t port, uint32_t timeout, bool reuseAddress) override;

        void close() override;

        ssize_t write(const uint8_t *data, size_t count) override;

        ssize_t read(uint8_t *data, size_t count) override;

        ssize_t peek(uint8_t *data, size_t count);

        size_t available() override;

        SSLVersion sslVersion() const;

        bool peek() const;

        void setPeek(bool peek);

        void setCertFile(const String &file);

        void setKeyFile(const String &file);

        void setCACertFile(const String &file);

//        bool decode(const ByteArray &buffer, ByteArray &plainBuffer) override;

    public:
        static void initializeSSL();

    private:
        void setSSLContext(void *context);

        void setSSL(void *ssl);

    public:
        static bool usePrivateKey(void *context, const char *keyStr);

        static bool useCertificate(void *context, const char *certStr);

    private:
        friend TcpSSLServer;

        SSLVersion _version;
        void *_sslContext;
        void *_ssl;
        bool _peek;

        String _certf;
        String _keyf;
        String _cacert;

    private:
        static const char *ClientKey;
        static const char *ClientCert;
    };

    class WebSocketClient : public TcpClient {
    public:
        explicit WebSocketClient(int sockId = -1, IPVersion ipVersion = IPVersion::IPV4, const TimeSpan &receiveTimeout = TimeSpan::fromSeconds(1));

        ~WebSocketClient() override;

        size_t available() override;

        void clearReceiveBuffer() override;

        void disableDecoding();

        void enableDecoding();

        bool isClosing() const override;

        int sendWithoutEncoding(const uint8_t *buffer, off_t offset, int count);

    private:
        enum FrameType {
            WS_EMPTY_FRAME = 0xF0,
            WS_ERROR_FRAME = 0xF1,
            WS_TEXT_FRAME = 0x01,
            WS_BINARY_FRAME = 0x02,
            WS_PING_FRAME = 0x09,
            WS_PONG_FRAME = 0x0A,
            WS_OPENING_FRAME = 0xF3,
            WS_CLOSING_FRAME = 0x08
        };

    protected:
        ssize_t write(const uint8_t *data, size_t count) override;

        ssize_t read(uint8_t *data, size_t count) override;

    private:
        bool readFrame();

    private:
        static FrameType getFrameLength(const ByteArray &inFrame, uint64_t &payloadLength);

        static bool decodeFrame(const ByteArray &inFrame, ByteArray &outMessage);

        static int encodeFrame(const ByteArray &inMessage, ByteArray &outFrame, FrameType frameType = WS_BINARY_FRAME);

    private:
        off_t _position;
        ByteArray _buffer;
        Mutex _bufferMutex;
        bool _decoding;
        bool _encoding;

        bool _isClosing;

        TimeSpan _receiveTimeout;
        static const TimeSpan MinReceiveTimeout;
    };

    class WebSocketSSLClient : public TcpSSLClient {
    public:
        explicit WebSocketSSLClient(SSLVersion version = SSLVersion::SSLv23, int sockId = -1, IPVersion ipVersion = IPVersion::IPV4,
                           const TimeSpan &receiveTimeout = TimeSpan::fromSeconds(1));

        ~WebSocketSSLClient() override;

        size_t available() override;

        void clearReceiveBuffer() override;

        void disableDecoding();

        void enableDecoding();

        bool isClosing() const override;

        ssize_t sendWithoutEncoding(const uint8_t *buffer, off_t offset, size_t count);

    private:
        enum FrameType {
            WS_EMPTY_FRAME = 0xF0,
            WS_ERROR_FRAME = 0xF1,
            WS_TEXT_FRAME = 0x01,
            WS_BINARY_FRAME = 0x02,
            WS_PING_FRAME = 0x09,
            WS_PONG_FRAME = 0x0A,
            WS_OPENING_FRAME = 0xF3,
            WS_CLOSING_FRAME = 0x08
        };

    protected:
        ssize_t write(const uint8_t *data, size_t count) override;

        ssize_t read(uint8_t *data, size_t count) override;

    private:
        bool readFrame();

    private:
        static FrameType getFrameLength(const ByteArray &inFrame, uint64_t &payloadLength);

        static bool decodeFrame(const ByteArray &inFrame, ByteArray &outMessage);

        static int encodeFrame(const ByteArray &inMessage, ByteArray &outFrame, FrameType frameType = WS_BINARY_FRAME);

    private:
        off_t _position;
        ByteArray _buffer;
        Mutex _bufferMutex;
        bool _decoding;
        bool _encoding;

        bool _isClosing;

        TimeSpan _receiveTimeout;
        static const TimeSpan MinReceiveTimeout;
    };

#endif
}

#endif // TcpClient_h
