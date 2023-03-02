#include <assert.h>
#include "thread/Locker.h"
#include "thread/TickTimeout.h"
#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "system/Resources.h"
#include "driver/DriverManager.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/channels/TcpInteractive.h"
#include "driver/channels/TcpBackgroundReceiver.h"

using namespace Diag;

namespace Drivers {
    TcpBackgroundReceiver::TcpBackgroundReceiver(DriverManager *dm, Channel *channel, TcpClient *client) {
        if (dm == nullptr)
            throw ArgumentNullException("dm");
        if (client == nullptr)
            throw ArgumentNullException("client");
        if (channel == nullptr)
            throw ArgumentNullException("channel");

        _client = client;
        _manager = dm;
        _originalDevice = nullptr;

        _context = dynamic_cast<TcpChannelContext *>(channel->description()->context());
        assert(_context);

        createDevice(channel);
    }

    TcpBackgroundReceiver::~TcpBackgroundReceiver() {
        if (_originalDevice != nullptr) {
            _originalDevice->removeReceiveDevice(_device);
            _originalDevice = nullptr;
        }

        if (_channel != nullptr) {
            delete _channel;
            _channel = nullptr;
        }
        if (_device != nullptr) {
            delete _device;
            _device = nullptr;
        }
        if (_dd != nullptr) {
            delete _dd;
            _dd = nullptr;
        }
        if (_client != nullptr) {
            _client = nullptr;
        }
    }

    size_t TcpBackgroundReceiver::available() {
        return _client != nullptr ? _client->available() : 0;
    }

    bool TcpBackgroundReceiver::connected() const {
        return _client != nullptr ? _client->connected() : false;
    }

    TcpClient *TcpBackgroundReceiver::tcpClient() const {
        return _client;
    }

    const Endpoint &TcpBackgroundReceiver::peerEndpoint() const {
        return _client->peerEndpoint();
    }

    const Endpoint &TcpBackgroundReceiver::endpoint() const {
        return _client->endpoint();
    }

    int TcpBackgroundReceiver::socketId() const {
        return _client->socketId();
    }

    Device *TcpBackgroundReceiver::device() const {
        return _device;
    }

    Channel *TcpBackgroundReceiver::channel() const {
        return _channel;
    }

    const TcpChannelContext *TcpBackgroundReceiver::context() const {
        return _context;
    }

    DriverManager *TcpBackgroundReceiver::manager() {
        return _manager;
    }

    void TcpBackgroundReceiver::createDevice(const Channel *channel) {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = dm->getDevice(channel);
        assert(device);
        _originalDevice = device;

        String channelName = String::convert("%s_%s:%d",
                                             channel->name().c_str(),
                                             _client->peerEndpoint().address.c_str(),
                                             _client->peerEndpoint().port);
        ChannelDescription *cd = new ChannelDescription(channelName, channel->description()->clientInteractiveName());
//        cd->context()->setReopened(false);
        TcpChannelContext *tc = dynamic_cast<TcpChannelContext *>(cd->context());
        tc->copyFrom(_context);
        _channel = new Channel(dm, cd);
        TcpInteractive *ti = dynamic_cast<TcpInteractive *>(_channel->interactive());
        assert(ti);
        ti->updateTcpClient(_client);

        String deviceName = String::convert("%s_%s:%d",
                                            device->name().c_str(),
                                            _client->peerEndpoint().address.c_str(),
                                            _client->peerEndpoint().port);
        DeviceDescription *dd = new DeviceDescription(deviceName.c_str(), cd,
                                                      device->instructionSet()->clone(),
                                                      device->description()->context()->clone());
        _dd = dd;
        _device = new Device(dd, _channel);
        device->addReceiveDevice(_device);
    }

    void TcpBackgroundReceiver::start() {
    }

    void TcpBackgroundReceiver::stop() {
    }

    bool TcpBackgroundReceiver::processBuffer(const ByteArray &buffer) {
        return false;
    }

    TcpAsyncReceiver::TcpAsyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client) : TcpBackgroundReceiver(
            dm, channel, client), _receiveTimer(nullptr) {
    }

    TcpAsyncReceiver::~TcpAsyncReceiver() {
        stopInner();
    }

    void TcpAsyncReceiver::start() {
        if (_receiveTimer == nullptr) {
            _receiveTimer = new Timer("Tcp_receiveProc.timer",
                                      ObjectTimerCallback<TcpAsyncReceiver>(this, &TcpAsyncReceiver::receiveProc),
                                      1);
        }
    }

    void TcpAsyncReceiver::stop() {
        stopInner();
    }

    void TcpAsyncReceiver::stopInner() {
        if (_receiveTimer != nullptr) {
            uint32_t timeout = _dd->receiveDelay();
            _receiveTimer->stop(TimeSpan::fromSeconds(timeout));
            delete _receiveTimer;
            _receiveTimer = nullptr;
        }
    }

    void TcpAsyncReceiver::receiveProc() {
//#ifdef DEBUG
//        Stopwatch sw(1000);
//#endif
        try {
            if (connected() && available() > 0) {
                receiveFromBuffer(_device);
            }
        }
        catch (const exception &e) {
            String str = String::convert("TcpBackgroundReceiver::receiveProc error'%s'", e.what());
            Trace::writeLine(str, Trace::Error);
        }

//#ifdef DEBUG
//        if(sw.elapsed() > sw.dateTime())
//        {
//            String info = _device != nullptr ?
//            String::convert("tcp_receiveProc_%s", _device->name().c_str()) : "tcp_receiveProc";
//            sw.setInfo(info);
//        }
//#endif
    }

    TcpSyncReceiver::TcpSyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client) : TcpBackgroundReceiver(dm,
                                                                                                                     channel,
                                                                                                                     client) {
    }

    TcpSyncReceiver::~TcpSyncReceiver() {
    }

    bool TcpSyncReceiver::processBuffer(const ByteArray &buffer) {
#ifdef DEBUG
        Stopwatch sw("multiplexing.TcpSyncReceiver::processBuffer", 1000);
#endif
        if (connected() && buffer.count() > 0) {
            return _device->executeInstruction(buffer);
        }
        return false;
    }

    TcpClientAsyncReceiver::TcpClientAsyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client)
            : TcpAsyncReceiver(dm, channel, client) {
    }

    TcpClientAsyncReceiver::~TcpClientAsyncReceiver() {
    }

    TcpClientSyncReceiver::TcpClientSyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client)
            : TcpSyncReceiver(dm, channel, client) {
    }

    TcpClientSyncReceiver::~TcpClientSyncReceiver() {
    }

    bool TcpClientSyncReceiver::processBuffer(const ByteArray &buffer) {
        if (buffer.count() > 0)
            _startTime = TickTimeout::getCurrentTickCount();

        return TcpSyncReceiver::processBuffer(buffer);
    }

    bool TcpClientSyncReceiver::isTimeout() const {
        const TimeSpan &closeTimeout = _context->closeTimeout();
        if (TickTimeout::isTimeout(_startTime, closeTimeout))
            return true;
        return false;
    }

    void TcpClientSyncReceiver::start() {
        _startTime = TickTimeout::getCurrentTickCount();
    }

    void TcpClientSyncReceiver::stop() {
    }

    bool TcpClientSyncReceiver::connected() const {
        if (_channel->interactive()->isClosing()) {
            return false;
        }

        if (isTimeout()) {
            return false;
        }

        return TcpSyncReceiver::connected();
    }

    TcpServerAsyncReceiver::TcpServerAsyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client)
            : TcpAsyncReceiver(dm, channel, client), _startTime(0) {
    }

    TcpServerAsyncReceiver::~TcpServerAsyncReceiver() {
    }

    void TcpServerAsyncReceiver::start() {
        _startTime = TickTimeout::getCurrentTickCount();

        TcpAsyncReceiver::start();
    }

    void TcpServerAsyncReceiver::stop() {
        TcpAsyncReceiver::stop();
    }

    size_t TcpServerAsyncReceiver::available() {
        size_t result = TcpAsyncReceiver::available();
        if (result > 0) {
            _startTime = TickTimeout::getCurrentTickCount();
        }
        return result;
    }

    bool TcpServerAsyncReceiver::isTimeout() const {
        const TimeSpan &closeTimeout = _context->closeTimeout();
        if (TickTimeout::isTimeout(_startTime, closeTimeout))
            return true;
        return false;
    }

    bool TcpServerAsyncReceiver::connected() const {
        if (_channel->interactive()->isClosing()) {
            return false;
        }

        if (isTimeout()) {
            return false;
        }

        return TcpAsyncReceiver::connected();
    }

    TcpServerSyncReceiver::TcpServerSyncReceiver(DriverManager *dm, Channel *channel, TcpClient *client)
            : TcpSyncReceiver(dm, channel, client), _startTime(0) {
    }

    TcpServerSyncReceiver::~TcpServerSyncReceiver() {
    }

    bool TcpServerSyncReceiver::processBuffer(const ByteArray &buffer) {
        if (buffer.count() > 0)
            _startTime = TickTimeout::getCurrentTickCount();

        return TcpSyncReceiver::processBuffer(buffer);
    }

    bool TcpServerSyncReceiver::isTimeout() const {
        const TimeSpan &closeTimeout = _context->closeTimeout();
        if (TickTimeout::isTimeout(_startTime, closeTimeout))
            return true;
        return false;
    }

    void TcpServerSyncReceiver::start() {
        _startTime = TickTimeout::getCurrentTickCount();
    }

    void TcpServerSyncReceiver::stop() {
    }

    bool TcpServerSyncReceiver::connected() const {
        if (_channel->interactive()->isClosing()) {
#ifdef DEBUG
            Debug::writeLine("The server sync receiver is closing!");
#endif
            return false;
        }

        if (isTimeout()) {
#ifdef DEBUG
            const TimeSpan &closeTimeout = _context->closeTimeout();
            Debug::writeFormatLine("The server sync receiver is timeout'%s'", closeTimeout.toString().c_str());
#endif
            return false;
        }

        return TcpSyncReceiver::connected();
    }
}
