#include <assert.h>
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "thread/Thread.h"
#include "thread/Process.h"
#include "diag/Trace.h"
#include "system/Math.h"
#include "driver/DriverManager.h"
#include "driver/devices/Device.h"
#include "driver/channels/TcpServerInteractive.h"
#include "system/Resources.h"

#if __APPLE__
#define HAS_KEVENT 1

#include <sys/event.h>

#elif __linux__ || __arm_linux__ || __ANDROID__
#define HAS_EPOLL 1
#include <sys/epoll.h>
#include <sys/socket.h>
#else
#define HAS_ONLY_ASYNC 1
#endif

using namespace Data;
using namespace Diag;

namespace Drivers {
    TcpClientEventArgs::TcpClientEventArgs(const Endpoint &endpoint) {
        this->endpoint = endpoint;
    }

    TcpClientEventArgs::~TcpClientEventArgs() {
    }

    TcpServerInteractive::Client::Client(DriverManager *dm, Channel *channel, TcpClient *client) {
        TcpServerChannelContext *tcc = (TcpServerChannelContext *) (channel->description()->context());
        assert(tcc);

#ifdef HAS_ONLY_ASYNC
        Trace::info("Start a tcp server receiver(async).");
        _receiver = new TcpServerAsyncReceiver(dm, channel, client);
        
        Trace::info("Start a tcp server sender(async).");
        _sender = new TcpServerAsyncSender(dm, channel, client, _receiver);
#else
        if (tcc->asyncReceiver()) {
            Trace::info("Start a tcp server receiver(async).");
            _receiver = new TcpServerAsyncReceiver(dm, channel, client);
        } else if (tcc->syncReceiver()) {
            Trace::info("Start a tcp server receiver(sync).");
            _receiver = new TcpServerSyncReceiver(dm, channel, client);
        } else if (tcc->multiPlexingReceiver()) {
            Trace::info("Start a tcp server receiver(multiplexing).");
            _receiver = new TcpServerSyncReceiver(dm, channel, client);
        }

        if (tcc->asyncSender()) {
            Trace::info("Start a tcp server sender(async).");
            _sender = new TcpServerAsyncSender(dm, channel, client, _receiver);
        } else if (tcc->syncSender()) {
            Trace::info("Start a tcp server sender(sync).");
            _sender = new TcpServerSyncSender(dm, channel, client, _receiver);
        } else if (tcc->multiPlexingSender()) {
            Trace::info("Start a tcp server sender(multiplexing).");
            _sender = new TcpServerSyncSender(dm, channel, client, _receiver);
        }
#endif

        _tcpClient = client;
        _closeFlag = false;
        _deleteFlag = false;
        _deleteStart = 0;
    }

    TcpServerInteractive::Client::~Client() {
        delete _receiver;
        _receiver = nullptr;
        delete _sender;
        _sender = nullptr;

        delete _tcpClient;
        _tcpClient = nullptr;
    }

    bool TcpServerInteractive::Client::connected() const {
        bool result = _receiver->connected();
        return result;
    }

    size_t TcpServerInteractive::Client::available() {
        return _receiver->available();
    }

    const Endpoint &TcpServerInteractive::Client::peerEndpoint() const {
        return _tcpClient->peerEndpoint();
    }

    const Endpoint &TcpServerInteractive::Client::endpoint() const {
        return _tcpClient->endpoint();
    }

    Device *TcpServerInteractive::Client::receiverDevice() const {
        return _receiver->device();
    }

    Device *TcpServerInteractive::Client::senderDevice() const {
        return _sender->device();
    }

    InstructionPool *TcpServerInteractive::Client::senderInstructionPool() const {
        return _sender->instructionPool();
    }

    TcpClient *TcpServerInteractive::Client::tcpClient() const {
        return _tcpClient;
    }

    int TcpServerInteractive::Client::socketId() const {
        return _tcpClient->socketId();
    }

    void TcpServerInteractive::Client::start() {
        _receiver->start();
    }

    bool TcpServerInteractive::Client::closeFlag() const {
        return _closeFlag;
    }

    void TcpServerInteractive::Client::setCloseManually(bool closeFlag) {
#ifdef DEBUG
        Debug::writeFormatLine("setCloseManually'%d', closeFlag: %s", socketId(), closeFlag ? "true" : "false");
#endif
        _closeFlag = closeFlag;
    }

    bool TcpServerInteractive::Client::deleteFlag() const {
        return _deleteFlag;
    }

    void TcpServerInteractive::Client::setDeleteManually(bool deleteFlag) {
        _deleteFlag = deleteFlag;
        if (deleteFlag)
            _deleteStart = TickTimeout::getCurrentTickCount();
    }

    uint32_t TcpServerInteractive::Client::deleteStart() const {
        return _deleteStart;
    }

    Device::Status TcpServerInteractive::Client::senderStatus() const {
        return _sender != nullptr ? _sender->senderStatus() : Device::Status::Unknown;
    }

    bool TcpServerInteractive::Client::isOnline() const {
        return !_deleteFlag && senderStatus() == Device::Status::Online;
    }

    bool TcpServerInteractive::Client::processReceivedBuffer(const ByteArray &buffer) {
        return _receiver->processBuffer(buffer);
    }

    TcpServerInteractive::Clients::Clients() {
    }

    TcpServerInteractive::Clients::~Clients() {
    }

    TcpServerInteractive::Client *
    TcpServerInteractive::Clients::add(DriverManager *dm, Channel *channel, TcpClient *client) {
        Locker locker(&_clientsMutex);
        Client *c = new Client(dm, channel, client);
        c->start();
        _clients.add(c);

        return c;
    }

    void TcpServerInteractive::Clients::remove(Client *client) {
#ifdef DEBUG
        Stopwatch sw("TcpServerInteractive::Clients::remove", 3000);
#endif
        Locker locker(&_clientsMutex);
        _clients.remove(client);
    }

    size_t TcpServerInteractive::Clients::count() {
        Locker locker(&_clientsMutex);
        return _clients.count();
    }

    void TcpServerInteractive::Clients::clear() {
        _clientsMutex.lock();
        _clients.clear();
        _clientsMutex.unlock();
    }

    bool TcpServerInteractive::Clients::process(int socketId, int bufferLength) {
#ifdef DEBUG
        Stopwatch sw("TcpServerInteractive::Clients::process", 1000);
#endif
        bool result = false;
        if (bufferLength > 0) {
            Locker locker(&_clientsMutex);
            Client *client = at(socketId);
            if (client != nullptr) {
                ByteArray buffer;
                client->tcpClient()->Receiver::receive(bufferLength, buffer);
                if (buffer.count() > 0) {
//                    Debug::writeFormatLine("TcpServerInteractive.received expect: %d, actual: %d", bufferLength, buffer.count());
                    result = client->processReceivedBuffer(buffer);
                }
            }
        }
        return result;
    }

    bool TcpServerInteractive::Clients::containsPeerEndpoint(const Endpoint &peerEndpoint) {
        for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
        {
            Client *client = _clients.at(i);
            if (client != nullptr && peerEndpoint == client->peerEndpoint()) {
                return true;
            }
        }
        return false;
    }

    Device *TcpServerInteractive::Clients::getReceiverDevice(const Endpoint &peerEndpoint) {
        for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
        {
            Client *client = _clients.at(i);
            if (client != nullptr && peerEndpoint == client->peerEndpoint()) {
                return client->receiverDevice();
            }
        }
        return nullptr;
    }

    Device *TcpServerInteractive::Clients::getSenderDevice(const Endpoint &peerEndpoint) {
        for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
        {
            Client *client = _clients.at(i);
            if (client != nullptr && peerEndpoint == client->peerEndpoint()) {
                return client->senderDevice();
            }
        }
        return nullptr;
    }

    InstructionPool *TcpServerInteractive::Clients::getClientPool(const Endpoint &endpoint) {
//        Locker locker(&_clientsMutex);
        for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
        {
            Client *client = _clients.at(i);
            if (client != nullptr && client->isOnline() &&
                endpoint == client->peerEndpoint()) {
                return client->senderInstructionPool();
            }
        }
        return nullptr;
    }

    void TcpServerInteractive::Clients::getClientPools(InstructionPools &ips) {
//        Locker locker(&_clientsMutex);
        ips.setAutoDelete(false);
        if (_clients.count() > 0) {
            for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
            {
                Client *client = _clients.at(i);
                if (client != nullptr && client->isOnline()) {
                    ips.add(client->senderInstructionPool());
                }
            }
        }
    }

    void TcpServerInteractive::Clients::getClientPools(const Endpoints &endpoints, InstructionPools &ips) {
//        Locker locker(&_clientsMutex);
        ips.setAutoDelete(false);
        if (_clients.count() > 0) {
            for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
            {
                Client *client = _clients.at(i);
                if (client != nullptr && client->isOnline()) {
                    for (size_t j = 0; j < endpoints.count(); j++) {
                        const Endpoint endpoint = endpoints[j];
                        if (endpoint.isEmpty() || endpoint == client->peerEndpoint()) {
                            ips.add(client->senderInstructionPool());
                            break;
                        }
                    }
                }
            }
        }
    }

    void TcpServerInteractive::Clients::getClientPoolsWithoutClient(const Endpoints &endpoints, InstructionPools &ips) {
//        Locker locker(&_clientsMutex);
        ips.setAutoDelete(false);
        if (_clients.count() > 0) {
            for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
            {
                Client *client = _clients.at(i);
                if (client != nullptr && client->isOnline()) {
                    for (size_t j = 0; j < endpoints.count(); j++) {
                        const Endpoint endpoint = endpoints[j];
                        if (endpoint.isEmpty() || endpoint != client->peerEndpoint()) {
                            ips.add(client->senderInstructionPool());
                            break;
                        }
                    }
                }
            }
        }
    }

    TcpServerInteractive::Client *TcpServerInteractive::Clients::getClient(const Endpoint &peerEndpoint) {
        for (ssize_t i = _clients.count() - 1; i >= 0; i--)   // be careful of zombie client
        {
            Client *client = _clients.at(i);
            if (client != nullptr && client->peerEndpoint() == peerEndpoint) {
                return client;
            }
        }
        return nullptr;
    }

    bool TcpServerInteractive::Clients::closeClient(const Endpoint &peerEndpoint) {
        for (size_t i = 0; i < _clients.count(); i++) {
            Client *temp = _clients.at(i);
            if (temp != nullptr) {
                if (temp->tcpClient()->peerEndpoint() == peerEndpoint) {
                    temp->setCloseManually(true);
                    return true;
                }
            }
        }
        return false;
    }

    bool TcpServerInteractive::Clients::getUnusedClients(PList<Client> &clients) {
        Locker locker(&_clientsMutex);
        clients.setAutoDelete(false);
        for (size_t i = 0; i < _clients.count(); i++) {
            Client *client = _clients.at(i);
            if (client != nullptr) {
                if (!client->connected() ||
                    client->closeFlag()) {
#ifdef DEBUG
                    Debug::writeFormatLine("getUnusedClients, client'%d' connected: %s, closeFlag: %s",
                                           client->socketId(),
                                           client->connected() ? "true" : "false",
                                           client->closeFlag() ? "true" : "false");
#endif
                    clients.add(client);
                }
            }
        }
        return clients.count() > 0;
    }

    TcpServerInteractive::Client *TcpServerInteractive::Clients::at(int socketId) const {
        for (size_t i = 0; i < _clients.count(); i++) {
            Client *client = _clients[i];
            if (client->socketId() == socketId)
                return client;
        }
        return nullptr;
    }

    TcpServerInteractive::TcpServerInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel),
                                                                                      _tcpServer(nullptr),
                                                                                      _acceptTimer(nullptr),
                                                                                      _closeTimer(nullptr),
                                                                                      _multiplexingThread(nullptr),
                                                                                      _multiplexingLoop(false), _fd(-1),
                                                                                      _exitSockets{0, 0} {
    }

    TcpServerInteractive::~TcpServerInteractive() {
        close();
    }

    bool TcpServerInteractive::open() {
        return rebind();
    }

    bool TcpServerInteractive::rebind() {
        static int bindingCount = 0;
        static const int MaxBindingCount = 5;
        static const uint32_t MaxDelayTime = 2000;    // 2s

        _tcpServerMutex.lock();
        if (_tcpServer != nullptr) {
            _tcpServer->close();
            delete _tcpServer;
            _tcpServer = nullptr;
        }

        _tcpServer = createServer();
        TcpServerChannelContext *tcc = getChannelContext();
        updateContext(tcc);

#ifdef DEBUG
        String message = String::convert("listen a socket, address = %s, port = %d, max connections = %d",
                                         !tcc->address().isNullOrEmpty() ? tcc->address().c_str() : "any",
                                         tcc->port(),
                                         tcc->maxConnections());
        Stopwatch sw(message, 1000);
#endif

        bindingCount++;

        bool result = _tcpServer->bind(tcc->address(), tcc->port(), tcc->reuseAddress());
        if (result) {
            _tcpServer->setBlocking(false);
            result = _tcpServer->listen();
            _tcpServerMutex.unlock();
            if (result) {
                if (result) {
                    if (tcc->sendBufferSize() > 0) {
                        _tcpServer->setSendBufferSize(tcc->sendBufferSize());
                    }
                    if (tcc->receiveBufferSize() > 0) {
                        _tcpServer->setReceiveBufferSize(tcc->receiveBufferSize());
                    }
                    if (tcc->noDelay()) {
                        _tcpServer->setNoDelay(true);
                    }
                }

#ifdef HAS_ONLY_ASYNC
                Trace::info("Start a tcp server receiver(async).");
                _acceptTimer = new Timer("server.acceptProc", 1, &TcpServerInteractive::acceptProc, this);
#else
                if (tcc->asyncReceiver()) {
                    Trace::info("Start a tcp server receiver(async).");
                    _acceptTimer = new Timer("server.acceptProc", 1, &TcpServerInteractive::acceptProc, this);
                } else if (tcc->syncReceiver()) {
                    Trace::info("Start a tcp server receiver(sync).");
                    _acceptTimer = new Timer("acceptProc", 1, &TcpServerInteractive::acceptProc, this);
                } else if (tcc->multiPlexingReceiver()) {
                    Trace::info("Start a tcp server receiver(multiplexing).");
                    _multiplexingThread = new Thread("server.multiplexingProc",
                                                     &TcpServerInteractive::multiplexingProc, this);
                    _multiplexingThread->start();
                }
#endif

                _closeTimer = new Timer("server.closeProc", 1000, &TcpServerInteractive::closeProc, this);

                String message = String::convert("listen a socket, address = %s, port = %d, max connections: %d",
                                                 !tcc->address().isNullOrEmpty() ? tcc->address().c_str() : "any",
                                                 tcc->port(), tcc->maxConnections());
                Trace::info(message);

                return true;
            } else {
                Debug::writeFormatLine("Failed to listen the TcpServerInteractive socket. address = %s, port = %d",
                                       tcc->address().c_str(), tcc->port());

                if (bindingCount < MaxBindingCount) {
                    Thread::msleep(MaxDelayTime);
                    return rebind();
                } else {
                    String message = String::convert(Resources::getString("SocketListenFailed").c_str(),
                                                     tcc->address().c_str(), tcc->port());
                    Trace::writeLine(message.c_str(), Trace::Error);
                    throw BindingException(message.c_str());
                }
            }
        } else {
            _tcpServerMutex.unlock();
            Debug::writeFormatLine("Failed to bind the TcpServerInteractive socket. address = %s, port = %d",
                                   tcc->address().c_str(), tcc->port());

            if (bindingCount < MaxBindingCount) {
                Thread::msleep(MaxDelayTime);
                return rebind();
            } else {
                String message = String::convert(Resources::getString("SocketBindingFailed").c_str(),
                                                 tcc->address().c_str(), tcc->port());
                Trace::writeLine(message.c_str(), Trace::Error);
                throw BindingException(message.c_str());
            }
        }
        return false;
    }

    TcpServerChannelContext *TcpServerInteractive::getChannelContext() const {
        return (TcpServerChannelContext *) (_channel->description()->context());
    }

    void TcpServerInteractive::close() {
        _multiplexingLoop = false;
        if (_fd != -1) {
            // for exit proc.
#ifdef HAS_KEVENT
            struct kevent ev;
            EV_SET(&ev, 0, EVFILT_USER, EV_ENABLE, NOTE_FFCOPY | NOTE_TRIGGER | 0x1, 0, NULL);
            kevent(_fd, &ev, 1, NULL, 0, NULL);
#elif HAS_EPOLL
            uint8_t dummy[1] = {0};
            ::write(_exitSockets[0], dummy, sizeof(dummy));
#endif
        }
        if (_multiplexingThread != nullptr) {
            delete _multiplexingThread;
            _multiplexingThread = nullptr;
        }

        if (_closeTimer != nullptr) {
            delete _closeTimer;
            _closeTimer = nullptr;
        }

        _clients.clear();

        _tcpServerMutex.lock();
        if (_tcpServer != nullptr) {
            _tcpServer->close();
            delete _tcpServer;
            _tcpServer = nullptr;
        }
        _tcpServerMutex.unlock();

        if (_acceptTimer != nullptr) {
            delete _acceptTimer;
            _acceptTimer = nullptr;
        }
    }

    void TcpServerInteractive::updateContext(const TcpServerChannelContext *tcc) {
        _tcpServer->setReceiveTimeout(tcc->receiveTimeout());
    }

    bool TcpServerInteractive::connected() {
        throw NotSupportedException("Cannot support this method, use 'TcpBackgroundReceiver' instead.");
    }

    size_t TcpServerInteractive::available() {
        throw NotSupportedException("Cannot support this method, use 'TcpBackgroundReceiver' instead.");
    }

    ssize_t TcpServerInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
        throw NotSupportedException("Cannot support this method, use 'TcpBackgroundReceiver' instead.");
    }

    ssize_t TcpServerInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
        throw NotSupportedException("Cannot support this method, use 'TcpBackgroundReceiver' instead.");
    }

    const Endpoint &TcpServerInteractive::endpoint() const {
        TcpServerChannelContext *tsc = getChannelContext();
        return tsc != nullptr ? tsc->endpoint() : Endpoint::Empty;
    }

    size_t TcpServerInteractive::clientCount() {
        return _clients.count();
    }

    bool TcpServerInteractive::containsPeerEndpoint(const Endpoint &peerEndpoint) {
        return _clients.containsPeerEndpoint(peerEndpoint);
    }

    bool TcpServerInteractive::closeClient(const Endpoint &peerEndpoint) {
        return _clients.closeClient(peerEndpoint);
    }

    Device *TcpServerInteractive::getReceiverDevice(const Endpoint &peerEndpoint) {
        return _clients.getReceiverDevice(peerEndpoint);
    }

    Device *TcpServerInteractive::getSenderDevice(const Endpoint &peerEndpoint) {
        return _clients.getSenderDevice(peerEndpoint);
    }

    InstructionPool *TcpServerInteractive::getClientPool(const Endpoint &endpoint) {
        return _clients.getClientPool(endpoint);
    }

    void TcpServerInteractive::getClientPools(InstructionPools &ips) {
        _clients.getClientPools(ips);
    }

    void TcpServerInteractive::getClientPools(const Endpoints &endpoints, InstructionPools &ips) {
        _clients.getClientPools(endpoints, ips);
    }

    void TcpServerInteractive::getClientPoolsWithoutClient(const Endpoints &endpoints, InstructionPools &ips) {
        _clients.getClientPoolsWithoutClient(endpoints, ips);
    }

    Delegates *TcpServerInteractive::acceptDelegates() {
        return &_acceptAction;
    }

    Delegates *TcpServerInteractive::closeDelegates() {
        return &_closeAction;
    }

    void TcpServerInteractive::addDynamicInstructions(const Endpoint &peerEndpoint, Instructions *sendInstructions,
                                                      Instructions *recvInstructions) {
        if (recvInstructions != nullptr) {
            Device *rdevice = getReceiverDevice(peerEndpoint);
            if (rdevice != nullptr) {
                rdevice->addDynamicInstructions(recvInstructions);
            }
        }

        if (sendInstructions != nullptr) {
            Device *sdevice = getSenderDevice(peerEndpoint);
            if (sdevice != nullptr) {
                sdevice->addDynamicInstructions(sendInstructions);
            }
        }
    }

    void TcpServerInteractive::clearDynamicInstructions(const Endpoint &peerEndpoint) {
        Device *rdevice = getReceiverDevice(peerEndpoint);
        if (rdevice != nullptr) {
            rdevice->clearDynamicInstructions();
        }

        Device *sdevice = getSenderDevice(peerEndpoint);
        if (sdevice != nullptr) {
            sdevice->clearDynamicInstructions();
        }
    }

    TcpServer *TcpServerInteractive::createServer() {
        return new TcpServer();
    }

    void TcpServerInteractive::acceptProc() {
        if (_tcpServer != nullptr) {
            bool valid = _tcpServer != nullptr ? _tcpServer->isValid() : false;
            if (valid) {
                TcpClient *client = _tcpServer->accept();
                if (client != nullptr) {
                    Locker locker(&_tcpServerMutex);
                    openClient(client);
                }
            }
        }
    }

    void TcpServerInteractive::closeProc() {
        PList<Client> clients;
        if (_clients.getUnusedClients(clients)) {
            for (size_t i = 0; i < clients.count(); i++) {
                Client *client = clients[i];
                closeClient(client);
            }
        }
    }

#ifdef HAS_KEVENT

    void TcpServerInteractive::multiplexingProc() {
        if (_tcpServer == nullptr || !_tcpServer->isListening()) {
            return;
        }

        int kq = kqueue();
        if (kq == -1) {
            Debug::writeFormatLine("kqueue failed: %d", errno);
            return;
        }
        _fd = kq;

        TcpServerChannelContext *context = this->getChannelContext();
        assert(context);
        int maxCount = context->maxConnections();

        struct kevent ev;
        uintptr_t listenfd = static_cast<uintptr_t>(_tcpServer->socketId());
        EV_SET(&ev, listenfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        int ret = kevent(kq, &ev, 1, NULL, 0, NULL);
        if (ret == -1) {
            Debug::writeFormatLine("kevent failed:%d", errno);
            return;
        }

        // for exit.
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
            for (int i = 0; i < ret; i++) {
                const struct kevent &event = eventlist[i];
                uintptr_t sockfd = event.ident;
                int16_t filter = event.filter;
                uint32_t flags = event.flags;
                intptr_t data = event.data;

                if (sockfd == listenfd) {
                    TcpClient *client = nullptr;
                    do {
                        client = _tcpServer->accept();
                        if (client != nullptr) {
                            openClient(client);
                        }
                    } while (client != nullptr);
                } else if (sockfd > 0 && filter == EVFILT_READ) {
                    // read from socket, length = data
                    if (data > 0) {
                        _clients.process(sockfd, data);
                    }
                }
            }
        }
        delete[] eventlist;
    }

#elif HAS_EPOLL
    void TcpServerInteractive::multiplexingProc()
    {
        if(_tcpServer == nullptr || !_tcpServer->isListening())
        {
            return;
        }
        
        TcpServerChannelContext* context = this->getChannelContext();
        assert(context);
        int maxCount = context->maxConnections();
        int receiveBufferSize = context->receiveBufferSize();
        if(receiveBufferSize <= 0)
            receiveBufferSize = 65535;
        
        int epollfd = epoll_create(maxCount);
        if (epollfd == -1)
        {
            Debug::writeFormatLine("epoll_create failed: %d", errno);
            return;
        }
        _fd = epollfd;
        
        int listenfd = _tcpServer->socketId();
        
        // for listen socket
        struct epoll_event ev;
        memset(&ev.data, 0, sizeof(ev.data));
        ev.events = EPOLLIN | EPOLLET;
//        ev.events = EPOLLIN;
        ev.data.fd = listenfd;
        int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
        if (ret == -1)
        {
            Debug::writeFormatLine("epoll_ctl failed: %d", errno);
            return;
        }
        
        // for exit
        socketpair(AF_UNIX, SOCK_STREAM, 0, _exitSockets);
        ev.events = EPOLLIN;
        ev.data.fd = _exitSockets[1];
        epoll_ctl(_fd, EPOLL_CTL_ADD, _exitSockets[1], &ev);
        
        _multiplexingLoop = true;
//        uint32_t waitms = (uint32_t)timeout.totalMilliseconds();
        struct epoll_event* eventlist = new struct epoll_event[maxCount];
        while (_multiplexingLoop)
        {
            ret = ::epoll_wait(epollfd, eventlist, maxCount, -1);
            for (int i=0; i<ret; i++)
            {
                const struct epoll_event& event = eventlist[i];
                int sockfd = event.data.fd;
                uint32_t events = event.events;

                if (sockfd == listenfd)
                {
                    TcpClient* client = nullptr;
                    do
                    {
                        client = _tcpServer->accept();
                        if (client != nullptr)
                        {
                            openClient(client);
                        }
                    }while (client != nullptr);
                }
                else if(sockfd == _exitSockets[1])
                {
                    break;
                }
                else if (sockfd > 0 && (events & EPOLLIN))
                {
                    // read from socket, length = data
                    _clients.process(sockfd, receiveBufferSize);
                }
            }
        }
        delete[] eventlist;
    }
#else
    void TcpServerInteractive::multiplexingProc()
    {
    }
#endif

    void TcpServerInteractive::openClient(TcpClient *tc) {
#ifdef DEBUG
        Stopwatch sw("TcpServerInteractive::openClient", 200);
#endif
        TcpServerChannelContext *tcc = getChannelContext();
        if (tcc->sendBufferSize() > 0) {
            tc->setSendBufferSize(tcc->sendBufferSize());
        }
        if (tcc->receiveBufferSize() > 0) {
            tc->setReceiveBufferSize(tcc->receiveBufferSize());
        }
        if (tcc->noDelay()) {
            tc->setNoDelay(true);
        }
        tc->setBlocking(false);

#ifdef DEBUG
        Client *client = _clients.add(manager(), _channel, tc);
#else
        _clients.add(manager(), _channel, tc);
#endif

        TcpClientEventArgs e(tc->peerEndpoint());
        _acceptAction.invoke(this, &e);

        if (_fd != -1) {
#ifdef HAS_KEVENT
            struct kevent ev;
            EV_SET(&ev, tc->socketId(), EVFILT_READ, EV_ADD, 0, 0, NULL);
            kevent(_fd, &ev, 1, NULL, 0, NULL);
#elif HAS_EPOLL
            struct epoll_event ev;
            memset(&ev.data, 0, sizeof(ev.data));
            ev.events = EPOLLIN | EPOLLET;
//            ev.events = EPOLLIN;
            ev.data.fd = tc->socketId();
            epoll_ctl(_fd, EPOLL_CTL_ADD, tc->socketId(), &ev);
#endif
        }

#ifdef DEBUG
        Debug::writeFormatLine("openClient, client'%d' connected: %s, closeFlag: %s",
                               client->socketId(),
                               client->connected() ? "true" : "false",
                               client->closeFlag() ? "true" : "false");
#endif
        Trace::writeLine(String::convert("client connected, endpoint: %s, peer endpoint: %s, socketId: %d",
                                         tc->endpoint().toString().c_str(), tc->peerEndpoint().toString().c_str(),
                                         tc->socketId()), Trace::Info);
    }

    void TcpServerInteractive::closeClient(Client *client) {
        TcpClient *tc = client->tcpClient();
        Trace::writeLine(String::convert("Remove an unused client, endpoint: %s, peer endpoint: %s, socketId: %d",
                                         tc->endpoint().toString().c_str(), tc->peerEndpoint().toString().c_str(),
                                         tc->socketId()), Trace::Info);

        TcpClientEventArgs e(tc->peerEndpoint());
        _closeAction.invoke(this, &e);

        if (_fd != -1) {
#ifdef HAS_KEVENT
            struct kevent ev;
            EV_SET(&ev, client->socketId(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(_fd, &ev, 1, NULL, 0, NULL);
#elif HAS_EPOLL
            struct epoll_event ev;
            memset(&ev.data, 0, sizeof(ev.data));
            ev.events = EPOLLIN | EPOLLET;
//            ev.events = EPOLLIN;
            ev.data.fd = client->socketId();
            epoll_ctl(_fd, EPOLL_CTL_DEL, client->socketId(), &ev);
#endif
        }
        _clients.remove(client);
    }

    TcpServerInteractive::Client *TcpServerInteractive::getClient(const Endpoint &peerEndpoint) {
        return _clients.getClient(peerEndpoint);
    }

#ifndef __EMSCRIPTEN__

    TcpSSLServerInteractive::TcpSSLServerInteractive(DriverManager *dm, Channel *channel) : TcpServerInteractive(dm,
                                                                                                                 channel) {
    }

    TcpSSLServerInteractive::~TcpSSLServerInteractive() {
    }

    TcpServer *TcpSSLServerInteractive::createServer() {
        return new TcpSSLServer();
    }

    void TcpSSLServerInteractive::updateContext(const TcpServerChannelContext *tcc) {
        TcpServerInteractive::updateContext(tcc);

        const TcpSSLServerChannelContext *tscc = dynamic_cast<const TcpSSLServerChannelContext *>(tcc);
        assert(tscc);
        TcpSSLServer *tss = dynamic_cast<TcpSSLServer *>(_tcpServer);
        assert(tss);
        tss->setKeyFile(tscc->keyFile());
        tss->setCertFile(tscc->certFile());
        tss->setCACertFile(tscc->cacertFile());
    }

    WebSocketServerInteractive::WebSocketServerInteractive(DriverManager *dm, Channel *channel) : TcpServerInteractive(
            dm, channel) {
    }

    WebSocketServerInteractive::~WebSocketServerInteractive() {
    }

    TcpServer *WebSocketServerInteractive::createServer() {
        return new WebSocketServer();
    }

    WebSocketSSLServerInteractive::WebSocketSSLServerInteractive(DriverManager *dm, Channel *channel)
            : TcpServerInteractive(dm, channel) {
    }

    WebSocketSSLServerInteractive::~WebSocketSSLServerInteractive() {
    }

    TcpServer *WebSocketSSLServerInteractive::createServer() {
        return new WebSocketSSLServer();
    }

    void WebSocketSSLServerInteractive::updateContext(const TcpServerChannelContext *tcc) {
        TcpServerInteractive::updateContext(tcc);

        const TcpSSLServerChannelContext *tscc = dynamic_cast<const TcpSSLServerChannelContext *>(tcc);
        assert(tscc);
        TcpSSLServer *tss = dynamic_cast<TcpSSLServer *>(_tcpServer);
        assert(tss);
        tss->setKeyFile(tscc->keyFile());
        tss->setCertFile(tscc->certFile());
        tss->setCACertFile(tscc->cacertFile());
    }

#endif
}
