#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "thread/Thread.h"
#include "driver/channels/Channel.h"
#include "driver/channels/TcpChannelContext.h"
#include "driver/channels/TcpInteractive.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/DriverManager.h"
#include <assert.h>

#if __APPLE__
#define HAS_KEVENT 1

#include <sys/event.h>

#elif __linux__ || __arm_linux__ || __ANDROID__
#define HAS_EPOLL 1
#include <sys/epoll.h>
#include <sys/socket.h>
#elif WIN32
#include <io.h>
#else
#define HAS_ONLY_ASYNC 1
#endif

using namespace Data;
using namespace Diag;

namespace Drivers {
    TcpMultiPlexingReceiver::Client::Client(TcpClient *client, TcpBackgroundReceiver *receiver) : _client(client),
                                                                                                  _receiver(receiver) {
    }

    TcpMultiPlexingReceiver::Client::~Client() {
    }

    TcpClient *TcpMultiPlexingReceiver::Client::tcpClient() const {
        return _client;
    }

    int TcpMultiPlexingReceiver::Client::socketId() const {
        return _client != nullptr ? _client->socketId() : -1;
    }

    bool TcpMultiPlexingReceiver::Client::processReceivedBuffer(const ByteArray &buffer) {
        bool result = _receiver->processBuffer(buffer);
        return result;
    }

    TcpMultiPlexingReceiver::Clients::Clients() {
    }

    TcpMultiPlexingReceiver::Clients::~Clients() {
    }

    void TcpMultiPlexingReceiver::Clients::add(TcpClient *client, TcpBackgroundReceiver *receiver) {
        Locker locker(&_clientsMutex);
        _clients.add(new Client(client, receiver));
    }

    void TcpMultiPlexingReceiver::Clients::remove(TcpClient *client) {
#ifdef DEBUG
        Stopwatch sw("TcpMultiPlexingReceiver::Clients::remove", 3000);
#endif
        for (size_t i = 0; i < _clients.count(); i++) {
            Client *temp = _clients[i];
            if (temp->tcpClient() == client) {
//                TickTimeout::delay(TimeSpan::fromSeconds(20), isProcessed, temp);
                Locker locker(&_clientsMutex);
                _clients.removeAt(i);
                break;
            }
        }
    }

    void TcpMultiPlexingReceiver::Clients::clear() {
        Locker locker(&_clientsMutex);
        _clients.clear();
    }

    bool TcpMultiPlexingReceiver::Clients::process(int socketId, int bufferLength) {
#ifdef DEBUG
        Stopwatch sw("TcpMultiPlexingReceiver::Clients::process", 1000);
#endif
        bool result = false;
        if (bufferLength > 0) {
            Locker locker(&_clientsMutex);
            Client *client = at(socketId);
            if (client != nullptr) {
                ByteArray buffer;
                client->tcpClient()->Receiver::receive(bufferLength, buffer);
                if (buffer.count() > 0) {
//                    Debug::writeFormatLine("TcpMultiPlexingReceiver.received expect: %d, actual: %d", bufferLength, buffer.count());
                    result = client->processReceivedBuffer(buffer);
                }
            }
        }
        return result;
    }

    TcpMultiPlexingReceiver::Client *TcpMultiPlexingReceiver::Clients::at(int socketId) const {
        for (size_t i = 0; i < _clients.count(); i++) {
            Client *client = _clients[i];
            if (client->socketId() == socketId)
                return client;
        }
        return nullptr;
    }

    int TcpMultiPlexingReceiver::_fd = -1;
    Thread *TcpMultiPlexingReceiver::_multiplexingThread = nullptr;
    bool TcpMultiPlexingReceiver::_multiplexingLoop = false;
    TcpMultiPlexingReceiver::Clients TcpMultiPlexingReceiver::_clients;
    int TcpMultiPlexingReceiver::_exitSockets[2] = {0, 0};

    TcpMultiPlexingReceiver::TcpMultiPlexingReceiver() {
    }

    TcpMultiPlexingReceiver::~TcpMultiPlexingReceiver() {
    }

    void TcpMultiPlexingReceiver::addClient(TcpClient *client, TcpBackgroundReceiver *receiver) {
        open(receiver);

        if (client == nullptr)
            return;
        if (_fd == -1)
            return;

#ifdef HAS_KEVENT
        struct kevent ev;
        EV_SET(&ev, client->socketId(), EVFILT_READ, EV_ADD, 0, 0, NULL);
        int ret = kevent(_fd, &ev, 1, NULL, 0, NULL);
        if (ret == -1) {
            Debug::writeFormatLine("kevent failed:%d", errno);
        }
#elif HAS_EPOLL
        struct epoll_event ev;
        memset(&ev.data, 0, sizeof(ev.data));
        ev.events = EPOLLIN | EPOLLET;
//        ev.events = EPOLLIN;
        ev.data.fd = client->socketId();
        epoll_ctl(_fd, EPOLL_CTL_ADD, client->socketId(), &ev);
#endif

        _clients.add(client, receiver);
    }

    void TcpMultiPlexingReceiver::removeClient(TcpClient *client) {
        if (client == nullptr)
            return;
        if (_fd == -1)
            return;

#ifdef HAS_KEVENT
        struct kevent ev;
        EV_SET(&ev, client->socketId(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(_fd, &ev, 1, NULL, 0, NULL);
#elif HAS_EPOLL
        struct epoll_event ev;
        memset(&ev.data, 0, sizeof(ev.data));
        ev.events = EPOLLIN | EPOLLET;
//        ev.events = EPOLLIN;
        ev.data.fd = client->socketId();
        epoll_ctl(_fd, EPOLL_CTL_DEL, client->socketId(), &ev);
#endif

        _clients.remove(client);
    }

    void TcpMultiPlexingReceiver::open(TcpBackgroundReceiver *receiver) {
        if (_multiplexingThread == nullptr) {
#ifdef HAS_KEVENT
            int kq = kqueue();
            if (kq == -1) {
                Debug::writeFormatLine("kqueue failed: %d", errno);
                return;
            }
            _fd = kq;
#elif HAS_EPOLL
            int maxCount = MaxEventCount;
            int epollfd = epoll_create(maxCount);
            if (epollfd == -1)
            {
                Debug::writeFormatLine("epoll_create failed: %d", errno);
                return;
            }
            _fd = epollfd;
#endif

            _multiplexingThread = new Thread("client.multiplexingProc", multiplexingProcInner, receiver);
            _multiplexingThread->start();
        }
    }

    void TcpMultiPlexingReceiver::close() {
        _multiplexingLoop = false;
        if (_fd != -1) {
            // for exit proc.
#ifdef HAS_KEVENT
            struct kevent ev;
            EV_SET(&ev, 0, EVFILT_USER, EV_ENABLE, NOTE_FFCOPY | NOTE_TRIGGER | 0x1, 0, NULL);
            kevent(_fd, &ev, 1, NULL, 0, NULL);
#elif HAS_EPOLL
            uint8_t dummy[1] = {0};
            ::write(0, dummy, sizeof(dummy));
#endif
        }
        if (_multiplexingThread != nullptr) {
            delete _multiplexingThread;
            _multiplexingThread = nullptr;
        }

        if (_fd != -1) {
            ::close(_fd);
            _fd = -1;
        }

        _clients.clear();
    }

    void TcpMultiPlexingReceiver::multiplexingProcInner(void *parameter) {
        TcpBackgroundReceiver *receiver = (TcpBackgroundReceiver *) parameter;
        assert(receiver);
        multiplexingProc(receiver->context());
    }

#ifdef HAS_KEVENT

    void TcpMultiPlexingReceiver::multiplexingProc(const TcpChannelContext *context) {
        int kq = _fd;
        if (kq == -1)
            return;

        int ret;
        int maxCount = MaxEventCount;
        int receiveBufferSize = context->receiveBufferSize();

        // for exit.
        struct kevent ev;
        EV_SET(&ev, 0, EVFILT_USER, EV_ADD, NOTE_FFCOPY, 0, NULL);
        kevent(kq, &ev, 1, NULL, 0, NULL);

        _multiplexingLoop = true;
//        static const TimeSpan timeout = TimeSpan::fromMilliseconds(100);
//        struct timespec ts;
//        uint32_t waitms = (uint32_t)timeout.totalMilliseconds();
//        ts.tv_sec = waitms / 1000;
//        ts.tv_nsec = (waitms % 1000) * 1000 * 1000;
        struct kevent *eventlist = new struct kevent[maxCount];
        while (_multiplexingLoop) {
            ret = kevent(kq, NULL, 0, eventlist, maxCount, nullptr);
            if (ret <= 0)
                continue;

            for (int i = 0; i < ret; i++) {
                struct kevent event = eventlist[i];
                uintptr_t sockfd = event.ident;
                int16_t filter = event.filter;
                uint32_t flags = event.flags;
                intptr_t data = event.data;

                if (flags & EV_ERROR) {
                    Debug::writeFormatLine("socket broken, error:%ld", data);
                    EV_SET(&ev, sockfd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &ev, 1, NULL, 0, NULL);
                } else if (sockfd > 0 && filter == EVFILT_READ) {
                    // read from socket, length = data, can not use data if used ssl.
                    if (receiveBufferSize > 0) {
                        _clients.process(sockfd, receiveBufferSize);
                    }
                }
            }
        }
        delete[] eventlist;
    }

#elif HAS_EPOLL
    void TcpMultiPlexingReceiver::multiplexingProc(const TcpChannelContext* context)
    {
        int epollfd = _fd;
        if(epollfd == -1)
            return;
        
        int ret;
        int maxCount = MaxEventCount;
        int receiveBufferSize = context->receiveBufferSize();

        // for exit
        socketpair(AF_UNIX, SOCK_STREAM, 0, _exitSockets);
        struct epoll_event ev;
        memset(&ev.data, 0, sizeof(ev.data));
        ev.events = EPOLLIN;
        ev.data.fd = _exitSockets[1];
        epoll_ctl(epollfd, EPOLL_CTL_ADD, _exitSockets[1], &ev);

        _multiplexingLoop = true;
        struct epoll_event* eventlist = new struct epoll_event[maxCount];
        while (_multiplexingLoop)
        {
            ret = ::epoll_wait(epollfd, eventlist, maxCount, -1);
            if (ret <= 0)
            {
                continue;
            }

            for (int i=0; i<ret; i++)
            {
                const struct epoll_event& event = eventlist[i];
                int sockfd = event.data.fd;
                uint32_t events = event.events;

                if(sockfd == _exitSockets[1])
                {
                    break;
                }
                else if (sockfd > 0 && (events & EPOLLIN))
                {
                    // read from socket
                    if(receiveBufferSize > 0)
                    {
                        _clients.process(sockfd, receiveBufferSize);
                    }
                }
            }
        }
        delete[] eventlist;
    }
#else
    void TcpMultiPlexingReceiver::multiplexingProc(const TcpChannelContext*)
    {
    }
#endif

    TcpInteractive::TcpInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel) {
        if (channel == nullptr)
            throw ArgumentException("channel");
        if ((TcpClientChannelContext *) channel->description()->context() == nullptr)
            throw ArgumentException("channel");

        _tcpClient = nullptr;
        _autoDelete = true;
        _receiver = nullptr;
        _multiplexingReceiver = false;
    }

    TcpInteractive::~TcpInteractive() {
        close();
    }

    bool TcpInteractive::open() {
        close();

        TcpClientChannelContext *tcc = getChannelContext();

#ifdef DEBUG
        String message = String::convert("connect a host, address = %s, port = %d", tcc->address().c_str(),
                                         tcc->port());
        Stopwatch sw(message, 1000);
#endif
        _tcpClient = createClient();
        bool result = _tcpClient->connectToHost(tcc->address().c_str(), tcc->port(), tcc->openTimeout(),
                                                tcc->reuseAddress());
        if (result) {
            if (tcc->sendBufferSize() > 0) {
                _tcpClient->setSendBufferSize(tcc->sendBufferSize());
            }
            if (tcc->receiveBufferSize() > 0) {
                _tcpClient->setReceiveBufferSize(tcc->receiveBufferSize());
            }
            if (tcc->noDelay()) {
                _tcpClient->setNoDelay(true);
            }
            _tcpClient->setBlocking(tcc->isBlocking());

#ifdef DEBUG
            int stimeout = _tcpClient->sendTimeout();
            int rtimeout = _tcpClient->receiveTimeout();
            if (stimeout > 0 || rtimeout > 0)
                Debug::writeFormatLine("before update, send timeout: %d ms, recv timeout: %d ms", stimeout, rtimeout);
#endif

            if (tcc->sendTimeout() != TimeSpan::Zero) {
                _tcpClient->setSendTimeout((int) tcc->sendTimeout().totalMilliseconds());
            }
            if (tcc->receiveTimeout() != TimeSpan::Zero) {
                _tcpClient->setReceiveTimeout((int) tcc->receiveTimeout().totalMilliseconds());
            }

#ifdef DEBUG
            int stimeout2 = _tcpClient->sendTimeout();
            int rtimeout2 = _tcpClient->receiveTimeout();
            if (stimeout2 > 0 || rtimeout2 > 0)
                Debug::writeFormatLine("after update, send timeout: %d ms, recv timeout: %d ms", stimeout2, rtimeout2);
#endif

#ifdef HAS_ONLY_ASYNC
            Trace::info("Start a tcp client receiver(async).");
            _multiplexingReceiver = false;
            _receiver = new TcpClientAsyncReceiver(manager(), _channel, _tcpClient);
            _receiver->start();
#else
            if (tcc->asyncReceiver()) {
                Trace::info("Start a tcp client receiver(async).");
                _multiplexingReceiver = false;
                _receiver = new TcpClientAsyncReceiver(manager(), _channel, _tcpClient);
                _receiver->start();
            } else if (tcc->syncReceiver()) {
//                Trace::debug("Can not start a tcp client receiver(sync).");
            } else if (tcc->multiPlexingReceiver()) {
                Trace::info("Start a tcp client receiver(mulitiplexing).");
                _multiplexingReceiver = true;
                _receiver = new TcpClientSyncReceiver(manager(), _channel, _tcpClient);
                _receiver->start();

                TcpMultiPlexingReceiver::addClient(_tcpClient, _receiver);
            }
#endif
        }
        return result;
    }

    void TcpInteractive::close() {
        if (_autoDelete) {
            if (_multiplexingReceiver) {
                TcpMultiPlexingReceiver::removeClient(_tcpClient);
            }

            if (_receiver != nullptr) {
                delete _receiver;
                _receiver = nullptr;
            }
            if (_tcpClient != nullptr) {
                _tcpClient->close();
                delete _tcpClient;
                _tcpClient = nullptr;
            }
        }
    }

    TcpClient *TcpInteractive::createClient() {
        return new TcpClient();
    }

    void TcpInteractive::updateTcpClient(TcpClient *tcpClient) {
        _tcpClient = tcpClient;
        _autoDelete = false;
    }

    bool TcpInteractive::connectdInner() const {
//#ifdef DEBUG
//        if(_tcpClient != nullptr)
//            Debug::writeFormatLine("TcpInteractive::connected, socketId: %d, connected: %s", _tcpClient->socketId(), _tcpClient->connected() ? "true" : "false");
//#endif
        return _tcpClient != nullptr ? _tcpClient->connected() : false;
    }

    bool TcpInteractive::connected() {
        return connectdInner();
    }

    size_t TcpInteractive::available() {
        return _tcpClient != nullptr ? _tcpClient->available() : 0;
    }

    TcpClientChannelContext *TcpInteractive::getChannelContext() const {
        if (_channel == nullptr || _channel->description() == nullptr)
            return nullptr;

        return dynamic_cast<TcpClientChannelContext *>(_channel->description()->context());
    }

    ssize_t TcpInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("socket send", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _tcpClient->write(buffer + offset, count);
        }
        return len;
    }

    ssize_t TcpInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("tcp socket recv", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _tcpClient->read(buffer + offset, count);
        }
        return len;
    }

    bool TcpInteractive::isClosing() const {
        return _tcpClient->isClosing();
    }

    const Endpoint &TcpInteractive::peerEndpoint() const {
        return _tcpClient != nullptr ? _tcpClient->peerEndpoint() : Endpoint::Empty;
    }

    const Endpoint &TcpInteractive::endpoint() const {
        return _tcpClient != nullptr ? _tcpClient->endpoint() : Endpoint::Empty;
    }

    Device *TcpInteractive::getReceiverDevice() {
        return _receiver != nullptr ? _receiver->device() : nullptr;
    }

    Device *TcpInteractive::getSenderDevice() {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = dm->getDevice(_channel);
        assert(device);
        return device;
    }

    void TcpInteractive::addDynamicInstructions(Instructions *sendInstructions, Instructions *recvInstructions) {
        if (recvInstructions != nullptr) {
            Device *rdevice = getReceiverDevice();
            if (rdevice != nullptr) {
                rdevice->addDynamicInstructions(recvInstructions);
            }
        }

        if (sendInstructions != nullptr) {
            Device *sdevice = getSenderDevice();
            if (sdevice != nullptr) {
                sdevice->addDynamicInstructions(sendInstructions);

                sendInstructions->setReceiveInstruction(recvInstructions);
            }
        }
    }

    void TcpInteractive::clearDynamicInstructions() {
        Device *rdevice = getReceiverDevice();
        if (rdevice != nullptr) {
            rdevice->clearDynamicInstructions();
        }

        Device *sdevice = getSenderDevice();
        if (sdevice != nullptr) {
            sdevice->clearDynamicInstructions();
        }
    }

#ifndef __EMSCRIPTEN__

    TcpSSLInteractive::TcpSSLInteractive(DriverManager *dm, Channel *channel) : TcpInteractive(dm, channel) {
    }

    TcpSSLInteractive::~TcpSSLInteractive() {
    }

    SSLVersion TcpSSLInteractive::sslVersion() const {
        TcpSSLClient *sc = dynamic_cast<TcpSSLClient *>(_tcpClient);
        return sc != nullptr ? sc->sslVersion() : SSLVersion::SSLNone;
    }

    TcpClient *TcpSSLInteractive::createClient() {
        return new TcpSSLClient();
    }

    SSLChannelContext *TcpSSLInteractive::getChannelContext() {
        return (SSLChannelContext *) (_channel->description()->context());
    }

#endif
}
