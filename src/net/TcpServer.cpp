#if WIN32
#include <winsock2.h>
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
#include <fcntl.h>
#endif
#include "net/TcpServer.h"
#include "net/TcpClient.h"
#include <errno.h>
#include "diag/Trace.h"
#include "data/Convert.h"
#include "thread/TickTimeout.h"
#include "system/Math.h"
#include "net/Dns.h"
#include "IO/File.h"
#include "IO/Path.h"
#include "IO/MemoryStream.h"
#include "diag/Stopwatch.h"

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#define SHUT_RDWR SD_BOTH
#define socklen_t int
#else
#define closesocket(a) ::close(a)
#endif

namespace Net
{
    TcpServer::TcpServer()
    {
        _socket = -1;
        _isListening = false;
        TcpClient::initializeSocket();
    }
    
    TcpServer::~TcpServer()
    {
        close();
        
        _socket = -1;
    }
    
    bool TcpServer::bind(const Endpoint& endpoint, bool reuseAddress)
    {
        return bind(endpoint.address, endpoint.port, reuseAddress);
    }
    bool TcpServer::bind(const String& address, int port, bool reuseAddress)
    {
//        struct hostent  *phe;		/* pointer to host information entry    */
        struct sockaddr_in sin;     /* an Internet endpoint address  */
        
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        if ((sin.sin_port = htons(port)) == 0)
            Debug::writeFormatLine("invalid port \"%d\"", port);
        
        /* Map host name to IP address, allowing for dotted decimal */
        bool anyAddr = false;
        if(!address.isNullOrEmpty())
        {
            const char* anyStr = "any";
#if WIN32
            anyAddr = stricmp(address.c_str(), anyStr) == 0;
#else
            anyAddr = strcasecmp(address.c_str(), anyStr) == 0;
#endif
        }
        else
        {
            anyAddr = true;
        }
        if(!anyAddr)
        {
//            if ( (phe = gethostbyname(address.c_str())) != NULL )
//                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
//            else if ( (sin.sin_addr.s_addr = inet_addr(address.c_str())) == INADDR_NONE )
//                Debug::writeFormatLine("can't get \"%s\" host entry", address.c_str());
            Dns::getHostByName(address, sin);
        }
        else
        {
            sin.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        
        /* Allocate a socket */
        _socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket < 0)
        {
            Debug::writeFormatLine("can't create tcp server socket: %s", strerror(errno));
            return false;
        }
        
        if(reuseAddress)
        {
            if(!setReuseAddress(true))
                return false;
        }
        
        return ::bind(_socket, (struct sockaddr *) & sin, sizeof (sin)) != -1;
    }
    
    bool TcpServer::listen()
    {
        if(_socket != -1)
        {
            bool result = ::listen(_socket, backlog()) != -1;
            _isListening = true;
            return result;
        }
        return false;
    }
    int TcpServer::backlog() const
    {
        return 65535;
    }
    
    TcpClient* TcpServer::accept(const TimeSpan& timeout)
    {
#ifdef DEBUG
        Stopwatch sw("TcpServer::accept", 1000);
#endif
        if(_socket != -1)
        {
            struct sockaddr_in sin;
            socklen_t clen = sizeof(sin);
            memset(&sin, 0, clen);
            sin.sin_family = AF_INET;
            int sockfd = ::accept(_socket, (struct sockaddr *)&sin, &clen);
            if(sockfd == -1)
            {
                return nullptr;
            }
            else
            {
                TcpClient* client = createClient(sockfd);
//#ifdef DEBUG
//                if(client != nullptr)
//                {
//                    Debug::writeLine(String::format("client'%d' is %s.", sockfd, client->connected() ? "connected" : "not connected"));
//                }
//#endif
                if(!handshaking(client, timeout))
                {
                    Debug::writeFormatLine("Failed to hanshake, sockId: %d", client->socketId());
                    delete client;
                    return nullptr;
                }
                return client;
            }
        }
        return nullptr;
    }
    
    void TcpServer::close()
    {
        if (_socket != -1)
        {
            int result;
            if(_isListening)
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
                _isListening = false;
            }
        }
    }
    
    bool TcpServer::setBlocking(bool blocking)
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
    
    bool TcpServer::isValid() const
    {
        return _socket != -1;
    }
    bool TcpServer::isListening() const
    {
        return _isListening;
    }

    int TcpServer::socketId() const
    {
        return _socket;
    }
    
    int TcpServer::sendBufferSize() const
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
    void TcpServer::setSendBufferSize(int bufferSize)
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
    int TcpServer::receiveBufferSize() const
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
    void TcpServer::setReceiveBufferSize(int bufferSize)
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
    
    bool TcpServer::noDelay() const
    {
        socklen_t yes = 0;
        if (_socket != -1)
        {
#if WIN32
            int len = sizeof(yes);
#else
            socklen_t len = sizeof(yes);
#endif
            int result = getsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, &len);
            if (result == -1)
            {
                Debug::writeFormatLine("getsockopt'TCP_NODELAY' failed with error = %s", strerror(errno));
            }
        }
        return yes == 0 ? false : false;
    }
    void TcpServer::setNoDelay(bool noDelay)
    {
        if (_socket != -1)
        {
            int yes = noDelay ? 1 : 0;
            int result = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));
            if (result == -1)
            {
                Debug::writeFormatLine("setsockopt'TCP_NODELAY' failed with error = %s", strerror(errno));
            }
        }
    }
    
    bool TcpServer::setReuseAddress(bool reuseAddress)
    {
        Debug::writeFormatLine("TcpServer.setReuseAddress, %s", reuseAddress ? "true" : "false");
        
        int opt = 1;
        int result;
        result = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
        if (result == -1)
        {
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
    
    int TcpServer::sendTimeout() const
    {
        return (int)_sendTimeout.totalMilliseconds();
    }
    void TcpServer::setSendTimeout(uint timeout)
    {
        if(timeout > 0)
        {
            _sendTimeout = TimeSpan::fromMilliseconds(timeout);
        }
    }
    void TcpServer::setSendTimeout(const TimeSpan& timeout)
    {
        if(timeout != TimeSpan::Zero)
        {
            _sendTimeout = timeout;
        }
    }

    int TcpServer::receiveTimeout() const
    {
        return (int)_recvTimeout.totalMilliseconds();
    }
    void TcpServer::setReceiveTimeout(uint timeout)
    {
        if(timeout > 0)
        {
            _recvTimeout = TimeSpan::fromMilliseconds(timeout);
        }
    }
    void TcpServer::setReceiveTimeout(const TimeSpan& timeout)
    {
        if(timeout != TimeSpan::Zero)
        {
            _recvTimeout = timeout;
        }
    }
    
    bool TcpServer::handshaking(TcpClient* client, const TimeSpan& timeout)
    {
        return true;
    }
    TcpClient* TcpServer::createClient(int sockfd) const
    {
        return new TcpClient(sockfd);
    }
    
#ifndef __EMSCRIPTEN__
    const char* TcpSSLServer::ServerKey = "-----BEGIN RSA PRIVATE KEY-----\nMIIEogIBAAKCAQEApOcbB6rejm2jcDRFajXqBPsb8zg+HZUZk7nAnCGj5r05QDfI\nVB7s1FIARFpSFY7gPIoXUNjXWiF+docqCjwMccTOxr2Zmzf9Y+3/by4lf0bFZH6a\nUyv9KOZTAT08dlAfsmEV0pPEMQYHW7w7hu5REQTSOlkIHHbVpaT8eQviF6TW6IBo\nbEA1G2pDggoN1P/9Oc/JUnvfibLvusjUQ/suHLBbVNOjUgMBoKJrGinVEjMv9vFh\ntxKsTZcXIUVLHZrH7rlizH5LI0b6P3S9WrStWcHYEQ6bdBULHdNexD3VtVylrtLW\nAdRg9IVE7Jk31JKNQ8mbVit+zgYboUplMAhPuwIDAQABAoIBACtGlzpg5j08mqrE\nnP7tnTenasNenDaJPYgeNGUFEw1n1VF8Ei+5We+ygW1uwqHzt/dLSikmbwWxSo3y\nlN7QuXdV7eoMMfahL/d8qw9TbfXD7eOcsgVnwu1r3Jazql1mU5stjVuQ+ejfqyiw\n8nJLsqyjtkQnv1AxA4qQDM37h18qsH+jkSwvh+a9opTrht3k7jkLCyvKgdw4TBaQ\nJJ5YnzmsXtvlxa52o4LsChigJYMAsyWXY9puTu0Xn5PtDN1i6GPwboon46oQOzHL\nGMdGv1kiEZILVyYMYU8RqSpMp7iKdht6ON1GTwjirP9ONsshWg2/v1H8I6AjnfKV\nVZNNB8ECgYEAzqsIsxRkLWjqkcdMXebY0WEjnemDQ0WVFIP1348KhZb5q/N6miu7\nyLPIV0v5djAgpkVIFsrdWx82d7VNQHq8Epo/pvdO4Yq1DMko1JHBf/Uvl9EL9FU2\nccSWYfxjFv6BLoVWiDxO//Lb6F3i+Jnd0L8J/IEZmGkiUghboJCvdqECgYEAzEPl\nkwXk/UJV4O9i2YqUAXDJ/r5tpzIHjfro/0wehioPcuZzhY0n3GTuuNX4UDCV4JIs\nIu4zTWjOXyglOClipgu46WK/AbtmyciLXmHtBUupxeWlubh0qPxQcCbV79w60kWC\nIirO3AoinPaeOMd3pba/sZ7iGLMqCpw7nWRhVNsCgYBJtXp+1FD+WSMAJ0K3YvQF\niU24rmYgghqvnHj+6PdpufUGPOqHPf3VkTls2S351yRt6NvTNTtJmpJEk6cfziZx\nxA3bsOyzSFNWYhBr8ChBSONWlDpdRF8MqH6yJ9/3fYH5Y5j/MfNDGg0QjRu8arRQ\nFTKsmibU0vpoo456b+AEwQKBgBz3rADznwvDcoZZrH0JSAAETMERpF9HmwaphI1f\n8Bw6rsIuqEdSQ0rgDXKs9W/50+drE1NI0IGz0UBxYwXZ6KF/nBOM62WFfAg1pn4W\nCRTtLABiluQFopWsYoPlHgXMB5N2RAQwafGOhmYYG0wWDYO0rO61h37VOasTiOjD\nKYJtAoGAHotp5/rseNKLb1T6sU0/nENLYzJrE4XggOQZX5pkdiaQHlKSoz0y/ehq\nU9iTNDg0uEiiQTIlGCPFI+FE4Yvh1qNUGSujhcWsaz5Du6iR584+bTgzBWjEhqRP\n0cDXETKVILGwcJyl2yNON0sTm+M/Jf6SSqrWZQIA9/qWgbuJMec=\n-----END RSA PRIVATE KEY-----\n";
    const char* TcpSSLServer::ServerCert = "-----BEGIN CERTIFICATE-----\nMIIDkjCCAnoCCQDND6EOm+CizjANBgkqhkiG9w0BAQsFADCBijELMAkGA1UEBhMC\nQ04xEDAOBgNVBAgMB0JlaWppbmcxEDAOBgNVBAcMB0JlaWppbmcxETAPBgNVBAoM\nCFBlcnNvbmFsMREwDwYDVQQLDAhQZXJzb25hbDEMMAoGA1UEAwwDTi9BMSMwIQYJ\nKoZIhvcNAQkBFhRkYXZpZGJhb0BuZXRlYXNlLmNvbTAeFw0yMTA1MjEwNTM2MjZa\nFw0zMTA1MTkwNTM2MjZaMIGKMQswCQYDVQQGEwJDTjEQMA4GA1UECAwHQmVpamlu\nZzEQMA4GA1UEBwwHQmVpamluZzERMA8GA1UECgwIUGVyc29uYWwxETAPBgNVBAsM\nCFBlcnNvbmFsMQwwCgYDVQQDDANOL0ExIzAhBgkqhkiG9w0BCQEWFGRhdmlkYmFv\nQG5ldGVhc2UuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEApOcb\nB6rejm2jcDRFajXqBPsb8zg+HZUZk7nAnCGj5r05QDfIVB7s1FIARFpSFY7gPIoX\nUNjXWiF+docqCjwMccTOxr2Zmzf9Y+3/by4lf0bFZH6aUyv9KOZTAT08dlAfsmEV\n0pPEMQYHW7w7hu5REQTSOlkIHHbVpaT8eQviF6TW6IBobEA1G2pDggoN1P/9Oc/J\nUnvfibLvusjUQ/suHLBbVNOjUgMBoKJrGinVEjMv9vFhtxKsTZcXIUVLHZrH7rli\nzH5LI0b6P3S9WrStWcHYEQ6bdBULHdNexD3VtVylrtLWAdRg9IVE7Jk31JKNQ8mb\nVit+zgYboUplMAhPuwIDAQABMA0GCSqGSIb3DQEBCwUAA4IBAQAYTkkS2wKEshjc\nepGpsF7BCgUy5847HRX3146c3D7lJCZkD33NbQRHag5G0ASFYkvSvYzZNwrL3PMK\n8mCF9xCyABKeU448xvsxARXyMDvzd58zkNVy0TN6v6lh4y26XQJCuyVQyfmpdGtB\nj3HP5pJR96bDrHhHjaNt45rzyhvTL/A//9r4ZOQc6thhPl4zlqt4A05wc6Kmze7q\ntHN6zg3549lmgWcrYHbbQvY+zj+bvgXYRYkkXY3IBa/or9SD3HRRGrHCeptz3Dvz\nuYHozpFrT1mtyRTxKyb/7EaIVMz5gMCm+Qvqj0cf36d1mkPJwl/ukUUXEeNyygUY\nn6oOiLNd\n-----END CERTIFICATE-----\n";
    
    TcpSSLServer::TcpSSLServer(SSLVersion version) : TcpServer()
    {
        _version = SSLv23;
        
        OpenSSL_add_ssl_algorithms();
        SSL_load_error_strings();
    }
    
    TcpSSLServer::~TcpSSLServer()
    {
    }
    
    int TcpSSLServer::backlog() const
    {
        return 0;
    }

    bool TcpSSLServer::handshaking(TcpClient* client, const TimeSpan& timeout)
    {
#ifdef DEBUG
        Stopwatch sw("TcpSSLServer::handshaking", 1000);
#endif
        const SSL_METHOD *meth;
        switch (_version)
        {
            case SSLv3:
                return false;
//                meth = SSLv23_server_method();
                break;
            case SSLv23:
                meth = SSLv23_server_method();
                break;
            case TLSv1:
                meth = TLSv1_server_method();
                break;
            case TLSv1_1:
                meth = TLSv1_1_server_method();
                break;
            case TLSv1_2:
                meth = TLSv1_2_server_method();
                break;
            default:
                meth = TLSv1_server_method();
                break;
        }
        SSL_CTX* ctx = SSL_CTX_new(meth);
        if(ctx == nullptr)
        {
            Trace::writeLine("Can not create SSL context!", Trace::Error);
            return false;
        }
        
        if(File::exists(_cacert))
        {
            SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
            SSL_CTX_load_verify_locations(ctx, _cacert, NULL);
        }
        else
        {
            SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
        }
        
        if(File::exists(_certf))
        {
            if (SSL_CTX_use_certificate_file(ctx, _certf, SSL_FILETYPE_PEM) <= 0)
            {
                Trace::writeLine("Failed to use the cert!", Trace::Error);
                SSL_CTX_free(ctx);
                return false;
            }
        }
        else
        {
            if (!TcpSSLClient::useCertificate(ctx, ServerCert))
            {
                Trace::writeLine("Failed to check the cert!", Trace::Error);
                SSL_CTX_free(ctx);
                return false;
            }
        }
        
        if(File::exists(_keyf))
        {
            if (SSL_CTX_use_PrivateKey_file(ctx, _keyf, SSL_FILETYPE_PEM) <= 0)
            {
                Trace::writeLine("Failed to use the key!", Trace::Error);
                SSL_CTX_free(ctx);
                return false;
            }
        }
        else
        {
            if (!TcpSSLClient::usePrivateKey(ctx, ServerKey))
            {
                Trace::writeLine("Failed to check the key!", Trace::Error);
                SSL_CTX_free(ctx);
                return false;
            }
        }
        
        if (!SSL_CTX_check_private_key(ctx))
        {
            Trace::writeLine("The cert and the key is not match!", Trace::Error);
            SSL_CTX_free(ctx);
            return false;
        }
        
        SSL* ssl = SSL_new(ctx);
        if(ssl == NULL)
        {
            Trace::writeLine("Failed to create SSL instance!", Trace::Error);
            SSL_CTX_free(ctx);
            return false;
        }
        if (SSL_set_fd(ssl, client->socketId()) < 1)
        {
            Trace::writeLine("Failed to set socket id!", Trace::Error);
            SSL_CTX_free(ctx);
            SSL_free(ssl);
            return false;
        }
        
        // non-blocking
        bool blocking = false;
        u_long mode = blocking ? 0 : 1;
        int result = ::ioctl(client->socketId(), FIONBIO, &mode);
        if (result != 0)
        {
            Debug::writeFormatLine("TcpSSLServer::handshaking.ioctlsocket failed with error: %d\n", result);
        }
        
        uint startTime = TickTimeout::getCurrentTickCount();
        while(true)
        {
            // SSL accept
            int ret = SSL_accept(ssl);
            if(ret < 0)
            {
                int error = SSL_get_error(ssl, ret);
                switch (error)
                {
                    case SSL_ERROR_NONE:
                        break;
                    case SSL_ERROR_WANT_READ:
                    case SSL_ERROR_WANT_WRITE:
                        break;
                    default:
                        Trace::error(String::format("Could not accept SSL session'%d': SSL_get_error: %d, errno: %s(%d)", client->socketId(), error, strerror(errno), errno));
                        SSL_CTX_free(ctx);
                        SSL_free(ssl);
                        return false;
                }
                
                // timeout and break.
                if (TickTimeout::isTimeout(startTime, timeout))
                {
                    Trace::error(String::format("Could not accept SSL session'%d': timeout! SSL_get_error: %d", client->socketId(), error));
                    SSL_CTX_free(ctx);
                    SSL_free(ssl);
                    return false;
                }
                Thread::msleep(100);
            }
            else
            {
                break;
            }
        }
        
//        X509* server_cert = SSL_get_peer_certificate(ssl);
//        if(server_cert != NULL)
//        {
//            //#ifdef DEBUG
//            //            Debug::writeFormatLine("server cert, name: %s, issuer: %s",
//            //                                   X509_get_subject_name(server_cert), 0, 0),
//            //                                X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0));
//            //#endif
//            X509_free(server_cert);
//        }
//        else
//        {
//            Trace::writeLine("Can not retrieve cert from server!");
//            SSL_CTX_free(ctx);
//            SSL_free(ssl);
//            return false;
//        }
        
        TcpSSLClient* sslClient = dynamic_cast<TcpSSLClient*>(client);
        assert(sslClient);
        sslClient->setSSLContext(ctx);
        sslClient->setSSL(ssl);
        
        return true;
    }
    
    void TcpSSLServer::setCertFile(const String& file)
    {
        _certf = file;
    }
    void TcpSSLServer::setKeyFile(const String& file)
    {
        _keyf = file;
    }
    void TcpSSLServer::setCACertFile(const String& file)
    {
        _cacert = file;
    }
    TcpClient* TcpSSLServer::createClient(int sockfd) const
    {
        return new TcpSSLClient(_version, sockfd);
    }
    
    WebSocketServer::WebSocketServer() : TcpServer()
    {
    }
    
    WebSocketServer::~WebSocketServer()
    {
    }
    
    bool WebSocketServer::handshaking(TcpClient* client, const TimeSpan& timeout)
    {
#ifdef DEBUG
        Stopwatch sw("WebSocketServer::handshaking", 1000);
#endif
        WebSocketClient* wsc = dynamic_cast<WebSocketClient*>(client);
        assert(wsc);
        wsc->disableDecoding();
        
        const String header = "GET";
        ByteArray buffer;
        int length = client->receiveBySize(&buffer, (int)header.length(), timeout);
        if(length == (int)header.length())
        {
            uint8_t key[2048];
            memset(key, 0, sizeof(key));
            static const uint8_t start[] = "Sec-WebSocket-Key:";
            static const uint8_t end[] = "\r\n";
            length = client->Receiver::receiveByEndBytes(key, sizeof(key), start, sizeof(start)-1, end, sizeof(end)-1, 0, 3000);
            if(length > 0)
            {
                String keyStr = String((const char*)key, length);
                String temp = String::replace(keyStr, String((const char*)start, sizeof(start)), String::Empty);
                String websocketKey = temp.trim(' ', '\r', '\n');
                Debug::writeFormatLine("websocketKey='%s'", websocketKey.c_str());
                String serverKey = websocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
                uint8_t shaHash[SHA_DIGEST_LENGTH+1];
                memset(shaHash, 0, sizeof(shaHash));
                SHA1((const uint8_t*)serverKey.c_str(), (size_t)serverKey.length(), shaHash);
                
                String response;
                response.append("HTTP/1.1 101 Switching Protocols\r\n");
                response.append("Connection: Upgrade\r\n");
                response.append("Upgrade: websocket\r\n");
                response.append("Sec-WebSocket-Protocol: binary\r\n");
                response.append("Sec-WebSocket-Accept: ");
                response.append(String::toBase64(shaHash, 0, SHA_DIGEST_LENGTH));
                response.append("\r\n\r\n");
                
                //                    Thread::msleep(100);
                client->Receiver::clearReceiveBuffer();
                Debug::writeFormatLine("send back='%s'", response.c_str());
                // send back
                wsc->sendWithoutEncoding((const uint8_t*)response.c_str(), 0, response.length());
                wsc->enableDecoding();
                
                return true;
            }
        }
        wsc->enableDecoding();
        
        Debug::writeFormatLine("Failed to ws hanshake, received buffer length: %d", length);
        return false;
    }
    TcpClient* WebSocketServer::createClient(int sockfd) const
    {
        return new WebSocketClient(sockfd, true, receiveTimeout());
    }
    
    WebSocketSSLServer::WebSocketSSLServer(SSLVersion version) : TcpSSLServer(version)
    {
    }
    
    WebSocketSSLServer::~WebSocketSSLServer()
    {
    }
    
    bool WebSocketSSLServer::handshaking(TcpClient* client, const TimeSpan& timeout)
    {
#ifdef DEBUG
        Stopwatch sw("WebSocketSSLServer::handshaking", 1000);
#endif
        if(!TcpSSLServer::handshaking(client))
            return false;
        
        WebSocketSSLClient* wsc = dynamic_cast<WebSocketSSLClient*>(client);
        assert(wsc);
        wsc->disableDecoding();

        const String header = "GET";
        ByteArray buffer;
        int length = client->receiveBySize(&buffer, (int)header.length(), timeout);
        if(length == (int)header.length())
        {
            uint8_t key[2048];
            memset(key, 0, sizeof(key));
            static const uint8_t start[] = "Sec-WebSocket-Key:";
            static const uint8_t end[] = "\r\n";
            length = client->Receiver::receiveByEndBytes(key, sizeof(key), start, sizeof(start)-1, end, sizeof(end)-1, 0, 3000);
            if(length > 0)
            {
                String keyStr = String((const char*)key, length);
                String temp = String::replace(keyStr, String((const char*)start, sizeof(start)), String::Empty);
                String websocketKey = temp.trim(' ', '\r', '\n');
                Debug::writeFormatLine("websocketKey='%s'", websocketKey.c_str());
                String serverKey = websocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
                uint8_t shaHash[SHA_DIGEST_LENGTH+1];
                memset(shaHash, 0, sizeof(shaHash));
                SHA1((const uint8_t*)serverKey.c_str(), (size_t)serverKey.length(), shaHash);

                String response;
                response.append("HTTP/1.1 101 Switching Protocols\r\n");
                response.append("Connection: Upgrade\r\n");
                response.append("Upgrade: websocket\r\n");
                response.append("Sec-WebSocket-Protocol: binary\r\n");
                response.append("Sec-WebSocket-Accept: ");
                response.append(String::toBase64(shaHash, 0, SHA_DIGEST_LENGTH));
                response.append("\r\n\r\n");

                //                    Thread::msleep(100);
                client->Receiver::clearReceiveBuffer();
                Debug::writeFormatLine("send back='%s'", response.c_str());
                // send back
                wsc->sendWithoutEncoding((const uint8_t*)response.c_str(), 0, response.length());
                wsc->enableDecoding();

                return true;
            }
        }
        wsc->enableDecoding();
        
        Debug::writeFormatLine("Failed to wss hanshake, received buffer length: %d", length);
        return false;
    }
    
    TcpClient* WebSocketSSLServer::createClient(int sockfd) const
    {
        return new WebSocketSSLClient(_version, sockfd, true, receiveTimeout());
    }
#endif
}
