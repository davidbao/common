#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/Timer.h"
#include "net/Dns.h"
#include "net/NetInterface.h"
#include "system/Application.h"
#include "driver/DriverManager.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/channels/TcpServerInteractive.h"
#include "driver/channels/UdpServerInteractive.h"
#include "system/Resources.h"
#include "communication/ServerService.h"

namespace Communication {
    ServerService::ServerService() : ServerService(Server::Empty) {
    }

    ServerService::ServerService(const Server &server)
            : BaseCommService(), _server(server), _tsi(nullptr), _usi(nullptr) {
    }

    ServerService::~ServerService() {
    }

    bool ServerService::initialize(const InstructionCallback &callback) {
        if (!BaseCommService::initialize(callback))
            return false;

        DriverManager *dm = manager();
        assert(dm);
        Channel *channel = dm->getChannel("TcpServer");
        if (channel) {
            TcpServerInteractive *tsi = dynamic_cast<TcpServerInteractive *>(channel->interactive());
            _tsi = tsi;
        }
        channel = dm->getChannel("UdpServer");
        if (channel) {
            UdpServerInteractive *usi = dynamic_cast<UdpServerInteractive *>(channel->interactive());
            _usi = usi;
        }
        return true;
    }

    bool ServerService::unInitialize() {
        return BaseCommService::unInitialize();
    }

    void ServerService::addDynamicInstructions(const Endpoint &peerEndpoint, Instructions *sendInstructions,
                                               Instructions *recvInstructions) {
        if (_tsi != nullptr) {
            Trace::writeFormatLine("add dynamic instructions, peer endpoint: %s, server endpoint: %s",
                                   peerEndpoint.toString().c_str(), _server.endpoint().toString().c_str());
            _tsi->addDynamicInstructions(peerEndpoint, sendInstructions, recvInstructions);
        }
    }

    void ServerService::clearDynamicInstructions(const Endpoint &peerEndpoint) {
        if (_tsi != nullptr) {
            Trace::writeFormatLine("clear dynamic instructions, peer endpoint: %s, server endpoint: %s",
                                   peerEndpoint.toString().c_str(), _server.endpoint().toString().c_str());
            _tsi->clearDynamicInstructions(peerEndpoint);
            _tsi->closeClient(peerEndpoint);
        }
    }

    void ServerService::stopLoopSender(const String &name) {
        Debug::writeFormatLine("ServerService::stopLoopSender, name: %s", name.c_str());
        Locker locker(&_loopSendersMutex);
        for (size_t i = 0; i < _loopSenders.count(); i++) {
            BaseLoopSender *sender = _loopSenders[i];
            if (sender->name() == name) {
                _loopSenders.removeAt(i);
                break;
            }
        }
    }

    // stop async sender.
    void ServerService::stopPacketSender(const String &name) {
        Debug::writeFormatLine("ServerService::stopPacketSender, name: %s", name.c_str());
        Locker locker(&_packetSendersMutex);
        for (size_t i = 0; i < _packetSenders.count(); i++) {
            BasePacketSender *sender = _packetSenders[i];
            if (sender->name() == name) {
                _packetSenders.removeAt(i);
                break;
            }
        }
    }

    // stop sync sender.
    bool ServerService::stopPacketSyncSender(const String &name) {
        Debug::writeFormatLine("ServerService::stopPacketSyncSender, name: %s", name.c_str());
        Locker locker(&_packetSendersMutex);
        for (size_t i = 0; i < _packetSenders.count(); i++) {
            BasePacketSender *sender = _packetSenders[i];
            if (sender->name() == name) {
                _packetSenders.removeAt(i);
                return true;
            }
        }
        return false;
    }

    bool ServerService::hasPacketSyncSender(const String &name) {
        Locker locker(&_packetSendersMutex);
        for (size_t i = 0; i < _packetSenders.count(); i++) {
            BasePacketSender *sender = _packetSenders[i];
            if (sender->name() == name) {
                return true;
            }
        }
        return false;
    }

    Delegates *ServerService::receivedDelegates() {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = dm->getDevice("TcpDevice");
        assert(device);
        return device->receivedDelegates();
    }

    void ServerService::createTcpDevice(const InstructionCallback &callback) {
        if (callback.tcpInstructions == nullptr)
            return;

        DriverManager *dm = manager();
        assert(dm);

        const Server &server = _server;
        String address = server.address;
        int port = server.port;
        int maxConnections = server.maxConnections;

        TcpInstructionSet *set = new TcpInstructionSet(callback.owner, callback.tcpInstructions);
        ChannelDescription *cd = new ChannelDescription("TcpServer", interactiveName());
        DeviceDescription *dd = new DeviceDescription("TcpDevice", cd, set);
        dd->setReceiveTimeout(server.timeout.receive);
        dd->setSendTimeout(server.timeout.send);

        TcpServerChannelContext *tc = (TcpServerChannelContext *) cd->context();
        tc->setAddress(address);
        tc->setPort(port);
        tc->setReuseAddress(server.reuseAddress);
        tc->setReceiverType(server.receiverType);
        tc->setSenderType(server.senderType);
        tc->setMaxConnections(maxConnections);
        tc->setCloseTimeout(server.timeout.close);
        tc->setSendBufferSize(server.sendBufferSize);
        tc->setReceiveBufferSize(server.receiveBufferSize);
        tc->setReceiveTimeout(server.timeout.receive);
        tc->setSendTimeout(server.timeout.send);
        tc->setNoDelay(server.noDelay);
        tc->setReopened(false);
        dm->description()->addDevice(dd);

        Trace::writeFormatLine(Resources::getString("CreateTcpServerChannelInfo").c_str(), dd->name().c_str(),
                               tc->address().c_str(), tc->port());
    }

    void ServerService::createUdpReceiveDevice(const InstructionCallback &callback) {
        DriverManager *dm = manager();
        assert(dm);

        const Server &server = _server;

        UdpInstructionSet *set = new UdpInstructionSet(callback.owner, callback.udpRecvInstructions);
        ChannelDescription *cd = new ChannelDescription("UdpServer", "UdpServerInteractive");
        DeviceDescription *dd = new DeviceDescription("UdpServerDevice", cd, set);
        UdpServerChannelContext *uc = (UdpServerChannelContext *) cd->context();

        uc->setPort(server.broadcast.receiveport);
        dm->description()->addDevice(dd);

        Trace::writeFormatLine("Create the UDP receive ethernet channel: name: %s, port: %d",
                               cd->name().c_str(), uc->port());
    }

    void ServerService::createUdpSendDevice(const InstructionCallback &callback) {
        createUdpSendDevice(callback, "any");
    }

    void ServerService::createUdpSendDevice(const InstructionCallback &callback, const String &ipAddress) {
        DriverManager *dm = manager();
        assert(dm);

        const Server &server = _server;

        UdpInstructionSet *set = new UdpInstructionSet(callback.owner, callback.udpSendInstructions);
        ChannelDescription *cd = new ChannelDescription(String::convert("UdpClient_%s", ipAddress.c_str()),
                                                        "UdpInteractive");
        DeviceDescription *dd = new DeviceDescription(String::convert("UdpClientDevice_%s", ipAddress.c_str()), cd,
                                                      set);
        UdpChannelContext *uc = (UdpChannelContext *) cd->context();

        uc->setAddress(ipAddress);
        uc->setPort(server.broadcast.sendport);
        dm->description()->addDevice(dd);

        UdpSampler *sampler = new UdpSampler(dm, cd, dd, server.broadcast, callback.udpSampler, callback.owner);
        sampler->pause();
        dm->addPool(sampler);

        Trace::writeFormatLine("Create the UDP send ethernet channel(ServerService): name: %s, port: %d",
                               cd->name().c_str(), uc->port());
    }

    void ServerService::createDevice(const InstructionCallback &callback) {
        if (!_server.enabled)
            return;

        const Server &server = _server;

        createTcpDevice(callback);

        if (server.broadcast.enabled && callback.hasUdpInstructions()) {
            // create udp device.
            createUdpReceiveDevice(callback);
            createUdpSendDevice(callback);
        }
    }

    String ServerService::interactiveName() const {
        return "TcpServerInteractive";
    }

    uint32_t ServerService::maxPacketLength() const {
        uint32_t sendBufferSize = 65535;
        TcpServerInteractive *tsi = this->tsi();
        TcpServerChannelContext *context = tsi != nullptr ? tsi->getChannelContext() : nullptr;
        if (context != nullptr) {
            sendBufferSize = (uint32_t) context->sendBufferSize();
        }
        return sendBufferSize - 100;
    }

    const Server &ServerService::server() const {
        return _server;
    }

    TcpServerInteractive *ServerService::tsi() const {
        return _tsi;
    }

    size_t ServerService::clientCount() const {
        return _tsi != nullptr ? _tsi->clientCount() : 0;
    }

    void ServerService::addCloseDelegate(const Delegate &delegate) {
        TcpServerInteractive *tsi = this->tsi();
        if (tsi != nullptr) {
            tsi->closeDelegates()->add(delegate);
        }
    }

    void ServerService::removeCloseDelegate(const Delegate &delegate) {
        TcpServerInteractive *tsi = this->tsi();
        if (tsi != nullptr) {
            tsi->closeDelegates()->remove(delegate);
        }
    }

    void ServerService::addAcceptDelegate(const Delegate &delegate) {
        TcpServerInteractive *tsi = this->tsi();
        if (tsi != nullptr) {
            tsi->acceptDelegates()->add(delegate);
        }
    }

    void ServerService::removeAcceptDelegate(const Delegate &delegate) {
        TcpServerInteractive *tsi = this->tsi();
        if (tsi != nullptr) {
            tsi->acceptDelegates()->remove(delegate);
        }
    }

    void ServerService::addReceivedDelegate(const Delegate &delegate) {
        this->receivedDelegates()->add(delegate);
    }

    void ServerService::removeReceivedDelegate(const Delegate &delegate) {
        this->receivedDelegates()->remove(delegate);
    }

    TcpServerInteractive::Client *ServerService::getClient(const Endpoint &peerEndpoint) const {
        return _tsi != nullptr ? _tsi->getClient(peerEndpoint) : nullptr;
    }

    void ServerService::getClientPools(const Endpoints &endpoints, InstructionPools &ips) const {
        if (_tsi != nullptr) {
            _tsi->getClientPools(endpoints, ips);
        } else if (_usi != nullptr) {
            DriverManager *dm = manager();
            assert(dm);

            StringArray deviceNames;
            for (size_t i = 0; i < endpoints.count(); i++) {
                const Endpoint endpoint = endpoints[i];
                String deviceName = !endpoint.isEmpty() ?
                                    String::convert("UdpClientDevice_%s", endpoint.address.c_str()) :
                                    String::Empty;
                deviceNames.add(deviceName);
            }
            dm->getPools(deviceNames, ips);
        }
    }

    InstructionPool *ServerService::getClientPool(const Endpoint &endpoint) const {
        InstructionPools ips;
        Endpoints endpoints;
        endpoints.add(endpoint);
        getClientPools(endpoints, ips);
        if (ips.count() == 1) {
            return ips[0];
        }
        return nullptr;
    }

    void ServerService::getClientPoolsWithoutClient(const Endpoints &endpoints, InstructionPools &ips) const {
        if (_tsi != nullptr) {
            _tsi->getClientPoolsWithoutClient(endpoints, ips);
        } else if (_usi != nullptr) {
            DriverManager *dm = manager();
            assert(dm);

            StringArray deviceNames;
            for (size_t i = 0; i < endpoints.count(); i++) {
                const Endpoint endpoint = endpoints[i];
                String deviceName = !endpoint.isEmpty() ?
                                    String::convert("UdpClientDevice_%s", endpoint.address.c_str()) :
                                    String::Empty;
                deviceNames.add(deviceName);
            }
            dm->getPoolsWithoutDevice(deviceNames, ips);
        }
    }

    bool ServerService::isSendSuccessfully(const String &instructionName, const InstructionContext *ic) const {
        return isSendSuccessfully(instructionName, ic, ic != nullptr && !ic->hasException());
    }

    bool ServerService::isSendSuccessfully(const String &instructionName, const InstructionContext *ic,
                                           bool condition) const {
        if (condition) {
//#if DEBUG
//            Debug::writeFormatLine("Send an instraction '%s' successfully.", instructionName.c_str());
//#endif
            return true;
        } else {
//            String mess = String::convert(Resources::getString("FailedToSendInstruction").c_str(),
//                                          instructionName.c_str(),
//                                          (ic != nullptr && ic->hasException() ?
//                                           ic->exception()->message().c_str() :
//                                           Resources::getString("CommunicationException").c_str()));
//            
//            Trace::writeLine(mess.c_str(), Trace::Error);
            return false;
        }
    }

    bool ServerService::contains(const Endpoint &peerEndpoint) const {
        if (_tsi != nullptr) {
            return _tsi->containsPeerEndpoint(peerEndpoint);
        }
        return false;
    }

    bool ServerService::closeClient(const Endpoint &peerEndpoint) {
        if (_tsi != nullptr) {
            return _tsi->closeClient(peerEndpoint);
        }
        return false;
    }

    SSLServerService::SSLServerService() : SSLServerService(Server::Empty) {
    }

    SSLServerService::SSLServerService(const Server &server) : ServerService(server) {
    }

    SSLServerService::~SSLServerService() {
    }

    void SSLServerService::createDevice(const InstructionCallback &callback) {
        ServerService::createDevice(callback);

        DriverManager *dm = manager();
        assert(dm);

        DriverDescription *dd = dm->description();
        const ChannelDescription *cd = dd->getChannel("TcpDevice");
        assert(cd);
        TcpSSLServerChannelContext *tc = dynamic_cast<TcpSSLServerChannelContext *>(cd->context());
        assert(tc);
        Application *app = Application::instance();
        String path = app->rootPath();
        const Secure &secure = _server.secure;
        if (secure.enabled) {
            tc->setKeyFile(Path::isPathRooted(secure.keyFile) ? secure.keyFile : Path::combine(path, secure.keyFile));
            tc->setCertFile(
                    Path::isPathRooted(secure.certFile) ? secure.certFile : Path::combine(path, secure.certFile));
            tc->setCacertFile(
                    Path::isPathRooted(secure.cacertFile) ? secure.cacertFile : Path::combine(path, secure.cacertFile));
        }
    }

    String SSLServerService::interactiveName() const {
        return "TcpSSLServerInteractive";
    }

    WebServerService::WebServerService() : WebServerService(Server::Empty) {
    }

    WebServerService::WebServerService(const Server &server) : ServerService(server) {
    }

    WebServerService::~WebServerService() {
    }

    String WebServerService::interactiveName() const {
        return "WebSocketServerInteractive";
    }

    uint32_t WebServerService::maxPacketLength() const {
        uint32_t sendBufferSize = 65535;
        TcpServerInteractive *tsi = this->tsi();
        TcpServerChannelContext *context = tsi != nullptr ? tsi->getChannelContext() : nullptr;
        if (context != nullptr) {
            sendBufferSize = (uint32_t) context->sendBufferSize();
        }
        return sendBufferSize - 100;
    }

    WebSSLServerService::WebSSLServerService() : WebSSLServerService(Server::Empty) {
    }

    WebSSLServerService::WebSSLServerService(const Server &server) : ServerService(server) {
    }

    WebSSLServerService::~WebSSLServerService() {
    }

    void WebSSLServerService::createDevice(const InstructionCallback &callback) {
        ServerService::createDevice(callback);

        DriverManager *dm = manager();
        assert(dm);

        DriverDescription *dd = dm->description();
        const ChannelDescription *cd = dd->getChannel("TcpDevice");
        assert(cd);
        TcpSSLServerChannelContext *tc = dynamic_cast<TcpSSLServerChannelContext *>(cd->context());
        assert(tc);
        Application *app = Application::instance();
        String path = app->rootPath();
        const Secure &secure = _server.secure;
        if (secure.enabled) {
            if (!secure.keyFile.isNullOrEmpty())
                tc->setKeyFile(
                        Path::isPathRooted(secure.keyFile) ? secure.keyFile : Path::combine(path, secure.keyFile));
            if (!secure.certFile.isNullOrEmpty())
                tc->setCertFile(
                        Path::isPathRooted(secure.certFile) ? secure.certFile : Path::combine(path, secure.certFile));
            if (!secure.cacertFile.isNullOrEmpty())
                tc->setCacertFile(Path::isPathRooted(secure.cacertFile) ? secure.cacertFile : Path::combine(path,
                                                                                                            secure.cacertFile));
        }
    }

    String WebSSLServerService::interactiveName() const {
        return "WebSocketSSLServerInteractive";
    }

    uint32_t WebSSLServerService::maxPacketLength() const {
        return ServerService::maxPacketLength();
    }

    ServerService *ServerServiceFactory::create(const Server &server) {
        return !server.secure.enabled ? new ServerService(server) : new SSLServerService(server);
    }
}
