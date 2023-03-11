#include "communication/ServerServices.h"
#include "driver/channels/TcpServerInteractive.h"

namespace Communication {
    const ServerServices::TypeString ServerServices::TypeString::Empty;
    const ServerServices::TypeString ServerServices::TypeString::Tcp = ServerServices::TypeString(Type::Tcp, "tcp");
    const ServerServices::TypeString ServerServices::TypeString::WebSocket = ServerServices::TypeString(Type::WebSocket,
                                                                                                        "ws");
    const ServerServices::TypeString ServerServices::TypeString::SSL = ServerServices::TypeString(Type::SSL, "ssl");
    const ServerServices::TypeString ServerServices::TypeString::WebSocketSSL = ServerServices::TypeString(
            Type::WebSocketSSL, "wss");

    ServerServices::TypeString::TypeString(Type type, const String &str) {
        this->type = type;
        this->str = str;
    }

    bool ServerServices::TypeString::isEmpty() const {
        return type == Type::None || str.isNullOrEmpty();
    }

    const ServerServices::TypeStrings ServerServices::TypeStrings::All = TypeStrings(
            ServerServices::TypeString::Tcp,
            ServerServices::TypeString::WebSocket,
            ServerServices::TypeString::SSL,
            ServerServices::TypeString::WebSocketSSL);
    const ServerServices::TypeStrings ServerServices::TypeStrings::TcpAndSSL = TypeStrings(
            ServerServices::TypeString::Tcp,
            ServerServices::TypeString::SSL);

    ServerServices::TypeStrings::TypeStrings() {
    }

    ServerServices::TypeStrings::TypeStrings(const TypeString &tcp, const TypeString &ssl) : TypeStrings(tcp,
                                                                                                         TypeString::Empty,
                                                                                                         ssl,
                                                                                                         TypeString::Empty) {
    }

    ServerServices::TypeStrings::TypeStrings(const TypeString &tcp, const TypeString &ws, const TypeString &ssl,
                                             const TypeString &wss) {
        _strings[TcpIndex] = tcp;
        _strings[WebSocketIndex] = ws;
        _strings[SSLIndex] = ssl;
        _strings[WebSocketSSLIndex] = wss;
    }

    const ServerServices::TypeString &ServerServices::TypeStrings::at(size_t pos) const {
        if (pos < Type::Count) {
            return _strings[pos];
        }
        return TypeString::Empty;
    }

    ServerServices::ServerServices(Type type) : _type(type) {
        memset(_services, 0, sizeof(_services));
    }

    ServerServices::~ServerServices() {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr) {
                delete _services[i];
                _services[i] = nullptr;
            }
        }
    }

    void
    ServerServices::updateConfig(const BaseCommConfig &config, const String &prefix, const TypeStrings &typeStrings) {
        static const char *fmt = "%s_%s";
        if ((_type & Tcp) == Tcp &&
            !typeStrings[TcpIndex].isEmpty()) {
            _servers[TcpIndex] = config.server(String::convert(fmt, prefix.c_str(), typeStrings[TcpIndex].str.c_str()));
        }
        if ((_type & WebSocket) == WebSocket &&
            !typeStrings[WebSocketIndex].isEmpty()) {
            _servers[WebSocketIndex] = config.server(
                    String::convert(fmt, prefix.c_str(), typeStrings[WebSocketIndex].str.c_str()));
        }
        if ((_type & SSL) == SSL &&
            !typeStrings[SSLIndex].isEmpty()) {
            _servers[SSLIndex] = config.server(String::convert(fmt, prefix.c_str(), typeStrings[SSLIndex].str.c_str()));
        }
        if ((_type & WebSocketSSL) == WebSocketSSL &&
            !typeStrings[WebSocketSSLIndex].isEmpty()) {
            _servers[WebSocketSSLIndex] = config.server(
                    String::convert(fmt, prefix.c_str(), typeStrings[WebSocketSSLIndex].str.c_str()));
        }
    }

    void ServerServices::updateConfig(const StringMap &properties) {
        String str;
        for (size_t i = 0; i < Type::Count; i++) {
            String scheme;
            const String prefix = String::format("servers[%d]", i);
            if (properties.at(String::format("%s.scheme", prefix.c_str()), scheme)) {
                int index = -1;
                if ((_type & Tcp) == Tcp && scheme == TypeString::Tcp.str) {
                    index = TcpIndex;
                } else if ((_type & WebSocket) == WebSocket && scheme == TypeString::WebSocket.str) {
                    index = WebSocketIndex;
                } else if ((_type & SSL) == SSL && scheme == TypeString::SSL.str) {
                    index = SSLIndex;
                } else if ((_type & WebSocketSSL) == WebSocketSSL && scheme == TypeString::WebSocketSSL.str) {
                    index = WebSocketSSLIndex;
                }
                if (index >= 0) {
                    Server &server = _servers[index];
                    server.updateConfig(properties, prefix);
                }
            }
        }
    }

    bool ServerServices::initialize(const BaseCommService::InstructionCallback &callback) {
        bool result[Type::Count];
        for (size_t i = 0; i < Type::Count; i++) {
            result[i] = true;
        }
        if ((_type & Tcp) == Tcp) {
            if (!_servers[TcpIndex].isEmpty() && _services[TcpIndex] == nullptr) {
                _services[TcpIndex] = new ServerService(_servers[TcpIndex]);
                result[TcpIndex] = _services[TcpIndex]->initialize(callback);
            }
        }
        if ((_type & WebSocket) == WebSocket) {
            if (!_servers[WebSocketIndex].isEmpty() && _services[WebSocketIndex] == nullptr) {
                _services[WebSocketIndex] = new WebServerService(_servers[WebSocketIndex]);
                result[WebSocketIndex] = _services[WebSocketIndex]->initialize(callback);
            }
        }
        if ((_type & SSL) == SSL) {
            if (!_servers[SSLIndex].isEmpty() && _services[SSLIndex] == nullptr) {
                _services[SSLIndex] = new SSLServerService(_servers[SSLIndex]);
                result[SSLIndex] = _services[SSLIndex]->initialize(callback);
            }
        }
        if ((_type & WebSocketSSL) == WebSocketSSL) {
            if (!_servers[WebSocketSSLIndex].isEmpty() && _services[WebSocketSSLIndex] == nullptr) {
                _services[WebSocketSSLIndex] = new WebSSLServerService(_servers[WebSocketSSLIndex]);
                result[WebSocketSSLIndex] = _services[WebSocketSSLIndex]->initialize(callback);
            }
        }

        for (size_t i = 0; i < Type::Count; i++) {
            if (!result[i])
                return false;
        }
        return true;
    }

    bool ServerServices::unInitialize() {
        bool result[Type::Count];
        for (size_t i = 0; i < Type::Count; i++) {
            result[i] = true;
        }
        if ((_type & Tcp) == Tcp) {
            if (_services[TcpIndex] != nullptr) {
                result[TcpIndex] = _services[TcpIndex]->unInitialize();
                delete _services[TcpIndex];
                _services[TcpIndex] = nullptr;
            }
        }
        if ((_type & WebSocket) == WebSocket) {
            if (_services[WebSocketIndex] != nullptr) {
                result[WebSocketIndex] = _services[WebSocketIndex]->unInitialize();
                delete _services[WebSocketIndex];
                _services[WebSocketIndex] = nullptr;
            }
        }
        if ((_type & SSL) == SSL) {
            if (_services[SSLIndex] != nullptr) {
                result[SSLIndex] = _services[SSLIndex]->unInitialize();
                delete _services[SSLIndex];
                _services[SSLIndex] = nullptr;
            }
        }
        if ((_type & WebSocketSSL) == WebSocketSSL) {
            if (_services[WebSocketSSLIndex] != nullptr) {
                result[WebSocketSSLIndex] = _services[WebSocketSSLIndex]->unInitialize();
                delete _services[WebSocketSSLIndex];
                _services[WebSocketSSLIndex] = nullptr;
            }
        }

        for (size_t i = 0; i < Type::Count; i++) {
            if (!result[i])
                return false;
        }
        return true;
    }

    void ServerServices::getAllServices(PList<ServerService> &services) const {
        services.setAutoDelete(false);
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr)
                services.add(_services[i]);
        }
    }

    ServerService *ServerServices::getService(const Endpoint &peerEndpoint) const {
        for (size_t i = 0; i < Type::Count; i++) {
            ServerService *ss = _services[i];
            if (ss != nullptr && ss->contains(peerEndpoint))
                return ss;
        }
        return nullptr;
    }

    ServerService *ServerServices::getService(Type type) const {
        if ((_type & type) == Tcp) {
            return _services[TcpIndex];
        } else if ((_type & type) == WebSocket) {
            return _services[WebSocketIndex];
        } else if ((_type & type) == SSL) {
            return _services[SSLIndex];
        } else if ((_type & type) == WebSocketSSL) {
            return _services[WebSocketSSLIndex];
        }
        return nullptr;
    }

    bool ServerServices::hasServices() const {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr)
                return true;
        }
        return false;
    }

    TcpServerInteractive::Client *ServerServices::getClient(const Endpoint &peerEndpoint) const {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr) {
                TcpServerInteractive::Client *client = _services[i]->getClient(peerEndpoint);
                if (client != nullptr)
                    return client;
            }
        }
        return nullptr;
    }

    Server *ServerServices::getServer(Type type) {
        if ((_type & type) == Tcp) {
            return &_servers[TcpIndex];
        } else if ((_type & type) == WebSocket) {
            return &_servers[WebSocketIndex];
        } else if ((_type & type) == SSL) {
            return &_servers[SSLIndex];
        } else if ((_type & type) == WebSocketSSL) {
            return &_servers[WebSocketSSLIndex];
        }
        return nullptr;
    }

    bool ServerServices::isEmpty(Type type) {
        Server *server = getServer(type);
        return server == nullptr || server->isEmpty();
    }

    bool ServerServices::contains(const Endpoint &endpoint) const {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr && _services[i]->contains(endpoint))
                return true;
        }
        return false;
    }

    bool ServerServices::contains(Type type) const {
        return getService(type) != nullptr;
    }

    InstructionPool *ServerServices::getClientPool(const Endpoint &endpoint) {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr) {
                InstructionPool *ip = _services[i]->tsi()->getClientPool(endpoint);
                if (ip != nullptr)
                    return ip;
            }
        }
        return nullptr;
    }

    void ServerServices::getClientPools(const Endpoints &endpoints, InstructionPools &ips) const {
        for (size_t i = 0; i < Type::Count; i++) {
            if (_services[i] != nullptr)
                _services[i]->tsi()->getClientPools(endpoints, ips);
        }
    }

    size_t ServerServices::clientCount() const {
        size_t count = 0;
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            count += ss->clientCount();
        }
        return count;
    }

    void ServerServices::addCloseDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            TcpServerInteractive *tsi = ss->tsi();
            if (tsi != nullptr) {
                tsi->closeDelegates()->add(delegate);
            }
        }
    }

    void ServerServices::removeCloseDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            TcpServerInteractive *tsi = ss->tsi();
            if (tsi != nullptr) {
                tsi->closeDelegates()->remove(delegate);
            }
        }
    }

    void ServerServices::addAcceptDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            TcpServerInteractive *tsi = ss->tsi();
            if (tsi != nullptr) {
                tsi->acceptDelegates()->add(delegate);
            }
        }
    }

    void ServerServices::removeAcceptDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            TcpServerInteractive *tsi = ss->tsi();
            if (tsi != nullptr) {
                tsi->acceptDelegates()->remove(delegate);
            }
        }
    }

    void ServerServices::addReceivedDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            ss->receivedDelegates()->add(delegate);
        }
    }

    void ServerServices::removeReceivedDelegate(const Delegate &delegate) {
        PList<ServerService> services;
        getAllServices(services);
        for (size_t i = 0; i < services.count(); i++) {
            ServerService *ss = services[i];
            ss->receivedDelegates()->remove(delegate);
        }
    }

    bool ServerServices::closeClient(const Endpoint &endpoint) {
        ServerService *ss = getService(endpoint);
        if (ss != nullptr) {
            TcpServerInteractive *tsi = ss->tsi();
            if (tsi != nullptr) {
                return tsi->closeClient(endpoint);
            }
        }
        return false;
    }

    void ServerServices::stopPacketSender(const String &name, const Endpoint &peerEndpoint) {
        if (peerEndpoint.isEmpty()) {
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                ss->stopPacketSender(name);
            }
        } else {
            ServerService *ss = getService(peerEndpoint);
            if (ss != nullptr) {
                ss->stopPacketSender(name);
            }
        }
    }

    bool ServerServices::stopPacketSyncSender(const String &name, const Endpoint &peerEndpoint) {
        if (peerEndpoint.isEmpty()) {
            PList<ServerService> services;
            getAllServices(services);
            for (size_t i = 0; i < services.count(); i++) {
                ServerService *ss = services[i];
                ss->stopPacketSyncSender(name);
            }
            return true;
        } else {
            ServerService *ss = getService(peerEndpoint);
            if (ss != nullptr) {
                return ss->stopPacketSyncSender(name);
            }
        }
        return false;
    }
}
