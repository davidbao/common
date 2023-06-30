#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "driver/DriverManager.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/channels/TcpInteractive.h"
#include "system/Resources.h"
#include "communication/ClientService.h"
#include "communication/BaseCommConfig.h"
#include "configuration/ConfigService.h"
#ifdef PHONE_OS
#include "system/Application.h"
#endif

namespace Communication {
#ifdef PHONE_OS
    void ClientService::inactived(void* owner, void* sender, EventArgs* args)
    {
        ClientService* cs = static_cast<ClientService*>(owner);
        assert(cs);
        cs->inactive();
    }
    void ClientService::actived(void* owner, void* sender, EventArgs* args)
    {
        ClientService* cs = static_cast<ClientService*>(owner);
        assert(cs);
        cs->active();
    }
    void ClientService::networkChanged(void* owner, void* sender, EventArgs* args)
    {
        ClientService* cs = static_cast<ClientService*>(owner);
        assert(cs);
        
        PhoneApplication::NetworkChangedEventArgs* nce = dynamic_cast<PhoneApplication::NetworkChangedEventArgs*>(args);
        assert(nce);
        cs->networkChanged(nce->oldStatus, nce->newStatus);
    }
#endif

    ClientService::ClientService() : ClientService(Client::Empty) {
    }

    ClientService::ClientService(const Client &client) : BaseCommService(), _client(client), _ti(nullptr) {
    }

    ClientService::~ClientService() {
        _ti = nullptr;
    }

    bool ClientService::initialize(const InstructionCallback &callback) {
        if (!BaseCommService::initialize(callback))
            return false;

        DriverManager *dm = manager();
        assert(dm);
        Channel *channel = dm->getChannel("TcpClient");
        if (channel == nullptr)
            return true;
        auto ti = dynamic_cast<TcpInteractive *>(channel->interactive());
        assert(ti);
        _ti = ti;

#ifdef PHONE_OS
        PhoneApplication* app = PhoneApplication::instance();
        app->addEventActived(Delegate(this, actived));
        app->addEventInactived(Delegate(this, inactived));
        app->addEventNetworkChanged(Delegate(this, networkChanged));
#endif

        return true;
    }

    bool ClientService::unInitialize() {
        _ti = nullptr;

#ifdef PHONE_OS
        PhoneApplication* app = PhoneApplication::instance();
        app->removeEventActived(Delegate(this, actived));
        app->removeEventInactived(Delegate(this, inactived));
        app->removeEventNetworkChanged(Delegate(this, networkChanged));
#endif
        return BaseCommService::unInitialize();
    }

    void ClientService::createTcpDevice(const InstructionCallback &callback, bool enabled) {
        if (callback.tcpInstructions == nullptr)
            return;
        if (_instructionPool != nullptr)
            return;

        DriverManager *dm = manager();
        assert(dm);

        auto set = new TcpInstructionSet(callback.owner, callback.tcpInstructions);
        auto cd = new ChannelDescription("TcpClient", interactiveName());
        cd->setEnabled(enabled);
        auto tc = (TcpClientChannelContext *) cd->context();
        tc->setAddress(_client.address);
        tc->setPort(_client.port);
        tc->setReuseAddress(_client.reuseAddress);
        tc->setSendBufferSize(_client.sendBufferSize);
        tc->setReceiveBufferSize(_client.receiveBufferSize);
        tc->setNoDelay(_client.noDelay);
        tc->setBlocking(_client.blocking);
        tc->setSenderType(_client.senderType);
        tc->setReceiverType(_client.receiverType);
        tc->setOpenTimeout(_client.timeout.open);
        tc->setSendTimeout(_client.timeout.send);
        tc->setReceiveTimeout(_client.timeout.receive);

        String ddName = String::convert("TcpClientDevice_%s:%d", _client.address.c_str(), _client.port);
        auto dd = new DeviceDescription(ddName, cd, set);
        dd->setReceiveTimeout(_client.timeout.receive);
        dd->setSendTimeout(_client.timeout.send);

//        BasePacketContext::setPacketLength(_client.packetLength);

        dm->description()->addDevice(dd);

        if (!_client.multiplexingReceiver()) {
            _instructionPool = new TcpMultiSampler(dm, cd, dd, _client.connection, callback.tcpSampler, callback.owner);
        } else {
            TcpSingleSampler::startSinglePool();
            _instructionPool = new TcpSingleSampler(dm, cd, dd, _client.connection, callback.tcpSampler,
                                                    callback.owner);
        }
        dm->addPool(_instructionPool);
        clientSampler()->setConnectedError(_client.connection.showError);

#ifdef WIN32
        String fmt = String::UTF8toGBK(Resources::getString("CreateTcpChannelInfo"));
#else
        String fmt = Resources::getString("CreateTcpChannelInfo");
#endif
        Trace::writeFormatLine(fmt.c_str(), cd->name().c_str(), tc->address().c_str(), tc->port());
    }

    void ClientService::createUdpSendDevice(const InstructionCallback &callback) {
        DriverManager *dm = manager();
        assert(dm);

        const Client &client = _client;

        auto set = new UdpInstructionSet(callback.owner, callback.udpSendInstructions);
        auto cd = new ChannelDescription("UdpClient", "UdpInteractive");
        auto dd = new DeviceDescription("UdpClientDevice", cd, set);
        auto uc = (UdpChannelContext *) cd->context();

        uc->setPort(client.broadcast.sendport);
        uc->setSendBufferSize(_client.sendBufferSize);
        dm->description()->addDevice(dd);
        if (callback.udpSampler != nullptr) {
            dm->addPool(_instructionPool = new UdpSampler(dm, cd, dd, client.broadcast, callback.udpSampler,
                                                          callback.owner));
        }

        Trace::writeFormatLine("Create the UDP send ethernet channel(ClientService): name: %s, port: %d",
                               cd->name().c_str(), uc->port());
    }

    void ClientService::createUdpReceiveDevice(const InstructionCallback &callback) {
        DriverManager *dm = manager();
        assert(dm);

        const Client &client = _client;

        auto set = new UdpInstructionSet(callback.owner, callback.udpRecvInstructions);
        auto cd = new ChannelDescription("UdpServer", "UdpServerInteractive");
        auto dd = new DeviceDescription("UdpServerDevice", cd, set);
        auto uc = (UdpServerChannelContext *) cd->context();

        uc->setPort(client.broadcast.receiveport);
        uc->setReceiveBufferSize(_client.receiveBufferSize);
        dm->description()->addDevice(dd);

        Trace::writeFormatLine("Create the UDP receive ethernet channel: name: %s, port: %d",
                               cd->name().c_str(), uc->port());
    }

    void ClientService::createDevice(const InstructionCallback &callback) {
        if (!_client.enabled)
            return;

        const Client &client = _client;

        bool createUdpDevice = false;
        if (client.broadcast.enabled && callback.hasUdpInstructions()) {
            // create udp device.
            createUdpDevice = true;
            createUdpReceiveDevice(callback);
            createUdpSendDevice(callback);
        }
        createTcpDevice(callback, !createUdpDevice);
    }

    Delegates *ClientService::receivedDelegates() {
        DriverManager *dm = manager();
        if (dm != nullptr) {
            String ddName = String::convert("TcpClientDevice_%s:%d", _client.address.c_str(), _client.port);
            Device *device = dm->getDevice(ddName);
            assert(device);
            return device->receivedDelegates();
        }
        return nullptr;
    }

    TcpInteractive *ClientService::ti() const {
        return _ti;
    }

    Interactive *ClientService::interactive() const {
        return _ti;
    }

    bool ClientService::isSendSuccessfully(const String &instructionName, const ClientContext *ic) const {
        return isSendSuccessfully(instructionName, ic, ic != nullptr && !ic->hasException());
    }

    bool
    ClientService::isSendSuccessfully(const String &instructionName, const ClientContext *ic, bool condition) const {
        if (condition) {
//#if DEBUG
//            Debug::writeFormatLine("Send an instraction '%s' successfully.", instructionName.c_str());
//#endif
            return ic->state() == 0;
        } else {
//            String mess = String::convert(Resources::getString("FailedToSendInstruction").c_str(),
//                                          instructionName.c_str(),
//                                          (ic != nullptr && ic->hasException() ?
//                                           ic->exception()->message().c_str() :
//                                           Resources::getString("CommunicationException").c_str()));
//            
//            Trace::writeLine(mess, Trace::Error);
            return false;
        }
    }

    String ClientService::interactiveName() const {
        return "TcpInteractive";
    }

    uint32_t ClientService::maxPacketLength() const {
        uint32_t sendBufferSize = 65535;
        TcpInteractive *ti = this->ti();
        TcpClientChannelContext *context = ti != nullptr ? ti->getChannelContext() : nullptr;
        if (context != nullptr) {
            sendBufferSize = (uint32_t) context->sendBufferSize();
        }
        return sendBufferSize - 100;
    }

    Sampler *ClientService::clientSampler() const {
        return dynamic_cast<Sampler *>(_instructionPool);
    }

    const Endpoint &ClientService::peerEndpoint() const {
        return _ti != nullptr ? _ti->peerEndpoint() : Endpoint::Empty;
    }

    const Endpoint &ClientService::endpoint() const {
        return _ti != nullptr ? _ti->endpoint() : Endpoint::Empty;
    }

    P2PEndpoint ClientService::p2pEndpoint() const {
        return _ti != nullptr ? _ti->p2pEndpoint() : P2PEndpoint::Empty;
    }

    bool ClientService::connected() const {
        return _ti != nullptr && _ti->connected();
    }

    const Client &ClientService::client() const {
        return _client;
    }

    void ClientService::setClient(const Client &client) {
        _client = client;
    }

    // stop async sender.
    void ClientService::stopLoopSender(const String &name) {
        Debug::writeFormatLine("ClientService::stopLoopSender, name: %s", name.c_str());
        Locker locker(&_loopSendersMutex);
        for (size_t i = 0; i < _loopSenders.count(); i++) {
            BaseLoopSender *sender = _loopSenders[i];
            if (sender->name() == name) {
                _loopSenders.removeAt(i);
                break;
            }
        }
    }

    bool ClientService::hasLoopSender(const String &name) {
        Locker locker(&_loopSendersMutex);
        for (size_t i = 0; i < _loopSenders.count(); i++) {
            BaseLoopSender *sender = _loopSenders[i];
            if (sender->name() == name) {
                return true;
            }
        }
        return false;
    }

    // stop async sender.
    bool ClientService::stopPacketSender(const String &name) {
        Debug::writeFormatLine("ClientService::stopPacketSender, name: %s", name.c_str());
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

    bool ClientService::hasPacketSender(const String &name) {
        Locker locker(&_packetSendersMutex);
        for (size_t i = 0; i < _packetSenders.count(); i++) {
            BasePacketSender *sender = _packetSenders[i];
            if (sender->name() == name) {
                return true;
            }
        }
        return false;
    }

    // stop sync sender.
    bool ClientService::stopPacketSyncSender(const String &name) {
        Debug::writeFormatLine("ClientService::stopPacketSyncSender, name: %s", name.c_str());
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

    bool ClientService::hasPacketSyncSender(const String &name) {
        Locker locker(&_packetSendersMutex);
        for (size_t i = 0; i < _packetSenders.count(); i++) {
            BasePacketSender *sender = _packetSenders[i];
            if (sender->name() == name) {
                return true;
            }
        }
        return false;
    }

    void ClientService::addDynamicInstructions(Instructions *sendInstructions, Instructions *recvInstructions) {
        if (_ti != nullptr && _ti->connected()) {
            Trace::writeFormatLine(
                    "add dynamic instructions. peer endpoint: %s, send instructions count: %d, recv instructions count: %d",
                    _ti->peerEndpoint().toString().c_str(),
                    sendInstructions != nullptr ? sendInstructions->count() : 0,
                    recvInstructions != nullptr ? recvInstructions->count() : 0);
            _ti->addDynamicInstructions(sendInstructions, recvInstructions);
        }
    }

    void ClientService::clearDynamicInstructions() {
        if (_ti != nullptr) {
            Debug::writeFormatLine("clear dynamic instructions.");
//            _instructionPool->reset();
            _ti->clearDynamicInstructions();
        }
    }

#ifdef PHONE_OS
    void ClientService::active()
    {
        DriverManager* dm = manager();
        assert(dm);
        dm->resetAsync();
        
        _activeDelegates.invoke(this);
    }
    void ClientService::inactive()
    {
        _inactiveDelegates.invoke(this);
        
//#ifdef __ANDROID__
        DriverManager* dm = manager();
        assert(dm);
        dm->pause();
//#endif
    }
    
    void ClientService::networkChanged(PhoneApplication::NetStatus oldStatus, PhoneApplication::NetStatus newStatus)
    {
        PhoneApplication::NetworkChangedEventArgs e(oldStatus, newStatus);
        _networkChangedDelegates.invoke(this, &e);
        
        DriverManager* dm = manager();
        assert(dm);

        switch (newStatus) {
            case PhoneApplication::NetStatus::Unknown:
                assert(false);
                break;
            case PhoneApplication::NetStatus::NotReachable:
                dm->close();
                break;
            case PhoneApplication::NetStatus::ViaWiFi:
            case PhoneApplication::NetStatus::ViaWWAN:
                if(oldStatus == PhoneApplication::NetStatus::ViaWiFi ||
                   oldStatus == PhoneApplication::NetStatus::ViaWWAN)
                    dm->resetAsync();
                else if(oldStatus == PhoneApplication::NetStatus::NotReachable)
                    dm->openAsync();
                break;
            default:
                break;
        }
    }
    
    void ClientService::addEventActive(const Delegate& delegate)
    {
        _activeDelegates.add(delegate);
    }
    void ClientService::removeEventActive(const Delegate& delegate)
    {
        _activeDelegates.remove(delegate);
    }
    
    void ClientService::addEventInactive(const Delegate& delegate)
    {
        _inactiveDelegates.add(delegate);
    }
    void ClientService::removeEventInactive(const Delegate& delegate)
    {
        _inactiveDelegates.remove(delegate);
    }
    
    void ClientService::addEventNetworkChanged(const Delegate& delegate)
    {
        _networkChangedDelegates.add(delegate);
    }
    void ClientService::removeEventNetworkChanged(const Delegate& delegate)
    {
        _networkChangedDelegates.remove(delegate);
    }
#endif

#ifndef __EMSCRIPTEN__

    SSLClientService::SSLClientService() : SSLClientService(Client::Empty) {
    }

    SSLClientService::SSLClientService(const Client &client) : ClientService(client) {
    }

    SSLClientService::~SSLClientService() = default;

    uint32_t SSLClientService::maxPacketLength() const {
        auto si = dynamic_cast<TcpSSLInteractive *>(_ti);
        assert(si);
        SSLVersion version = si->sslVersion();
        if (version == SSLVersion::SSLv3 || version == SSLVersion::TLSv1)
            return 16384 - 100;
        return ClientService::maxPacketLength();
    }

    String SSLClientService::interactiveName() const {
        return "TcpSSLInteractive";
    }

#endif

    ClientServices::ClientServices(bool autoDelete) : _services(autoDelete) {
    }

    void ClientServices::add(ClientService *cs) {
        _services.add(cs);
    }

    void ClientServices::clear() {
        _services.clear();
    }

    ClientService *ClientServices::at(const Endpoint &endpoint) const {
        for (size_t i = 0; i < _services.count(); i++) {
            ClientService *cs = _services[i];
            if (cs->endpoint() == endpoint)
                return cs;
        }
        return nullptr;
    }

    void ClientServices::setAutoDelete(bool autoDelete) {
        _services.setAutoDelete(autoDelete);
    }

    size_t ClientServices::count() const {
        return _services.count();
    }

    ClientService *ClientServices::at(size_t pos) const {
        return _services.at(pos);
    }

    ClientService *ClientServiceFactory::create(const Client &client) {
#ifdef __EMSCRIPTEN__
        return new ClientService(client);
#else
        return !client.secure.enabled ? new ClientService(client) : new SSLClientService(client);
#endif
    }

    bool IClientService::hasService() const {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto cs = factory->getService<IConfigService>();
        assert(cs);

        bool enabled = false;
        return cs->getProperty("client.enabled", enabled) && enabled;
    }

    bool IClientService::connected() const {
        ClientService *cs = clientService();
        return cs != nullptr && cs->connected();
    }
}
