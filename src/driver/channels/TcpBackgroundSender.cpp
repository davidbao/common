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
#include "driver/channels/TcpBackgroundSender.h"

using namespace Diag;

namespace Drivers {
    SenderPool::SenderPool(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd) : InstructionPool(dm, cd,
                                                                                                               dd) {
        _connected = Device::Online;

        _showConnectedError = true;
        _checkFailedCount = 0;
    }

    SenderPool::~SenderPool() {
        stop();

        _connected = Device::Unknown;
    }

    void SenderPool::setConnectStatus(Device::Status status) {
        if (_connected != status) {
            Device::Status oldStatus = _connected;
            _connected = status;

            DeviceStatusEventArgs e(_dd->name(), oldStatus, status);
            _statusChangedAction.invoke(this, &e);
        }
    }

    Device::Status SenderPool::connectStatus() const {
        return _connected;
    }

    Device::Status SenderPool::getConnectStatus() const {
        return _connected;
    }

    bool SenderPool::isOnline() const {
        return getConnectStatus() == Device::Online;
    }

    bool SenderPool::isOffline() const {
        return getConnectStatus() == Device::Offline;
    }

    bool SenderPool::reopen() {
        return _channel != nullptr ? _channel->open() : false;
    }

    void SenderPool::errorHandle(const DeviceDescription *dd, const InstructionDescription *id, bool error) {
        if (error) {
            if (_showConnectedError)
                Trace::writeFormatLine(Resources::getString("UnableToConnectDevice").c_str(), _dd->name().c_str());

            _checkFailedCount++;

            if (_checkFailedCount >= detectionCount()) {
                setConnectStatus(Device::Offline);

                if (_channel != NULL && _channel->reopened()) {
                    reopen();
                }

                if (_showConnectedError)
                    Trace::writeFormatLine(Resources::getString("DeviceFailure").c_str(), _dd->name().c_str());

                _checkFailedCount = 0;
            }
        } else {
            setConnectStatus(Device::Online);

            _checkFailedCount = 0;
        }
    }

    void SenderPool::setConnectedError(bool showError) {
        _showConnectedError = showError;
    }

    Delegates *SenderPool::statusChangedDelegates() {
        return &_statusChangedAction;
    }

    void SenderPool::reset() {
        InstructionPool::reset();

        _connected = Device::Unknown;
        _checkFailedCount = 0;
    }

    int SenderPool::detectionCount() const {
        return 3;
    }

    SenderMultiPool::SenderMultiPool(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd) : SenderPool(dm,
                                                                                                                    cd,
                                                                                                                    dd) {
    }

    SenderMultiPool::~SenderMultiPool() {
    }

    Timer *SenderSinglePool::_singleTimer = nullptr;
    Mutex SenderSinglePool::_poolsMutex;
    PList<SenderSinglePool> SenderSinglePool::_pools(false);

    SenderSinglePool::SenderSinglePool(DriverManager *dm, ChannelDescription *cd, DeviceDescription *dd) : SenderPool(
            dm, cd, dd) {
    }

    SenderSinglePool::~SenderSinglePool() {
    }

    void SenderSinglePool::start(Channel *channel, Device *device) {
        if (!channel)
            return;
        if (!device)
            return;

        _channel = channel;
        _device = device;

        _poolsMutex.lock();
        _pools.add(this);
        _poolsMutex.unlock();

        resume();
    }

    void SenderSinglePool::stop() {
        pause();

        reset();

        _poolsMutex.lock();
        _pools.remove(this);
        _poolsMutex.unlock();

        _channel = nullptr;
        _device = nullptr;
    }

    void SenderSinglePool::startSinglePool() {
        if (_singleTimer == nullptr) {
            auto instructionSingleProc = []() {
#ifdef DEBUG
                Stopwatch sw("multiplexing.TcpSyncSender::instructionSingleProc", 1000);
#endif
                _poolsMutex.lock();
                for (size_t i = 0; i < _pools.count(); i++) {
                    SenderSinglePool *pool = _pools[i];
                    pool->processInstructions();
                }
                _poolsMutex.unlock();
            };
            _singleTimer = new Timer("server.InstructionSingleProc.timer", 1, instructionSingleProc);
        }
    }

    void SenderSinglePool::stopSinglePool() {
        if (_singleTimer != nullptr) {
            _singleTimer->stop();
            delete _singleTimer;
            _singleTimer = nullptr;
        }
    }

    bool SenderSinglePool::reopen() {
        if (_channel != nullptr) {
            _channel->openAsync();
            return true;
        }
        return false;
    }

    InstructionContext *SenderSinglePool::executeInstruction(InstructionDescription *id) {
        return _device != nullptr ? _device->executeInstructionAsync(id) : nullptr;
    }

    TcpBackgroundSender::TcpBackgroundSender(DriverManager *dm, Channel *channel, TcpClient *client,
                                             TcpBackgroundReceiver *receiver) {
        _client = client;
        _manager = dm;
        _receiver = receiver;

        _context = dynamic_cast<TcpChannelContext *>(channel->description()->context());
        assert(_context);
    }

    TcpBackgroundSender::~TcpBackgroundSender() {
        if (_pool != nullptr) {
            _pool->stop();
            delete _pool;
            _pool = nullptr;
        }

        if (_channel != NULL) {
            delete _channel;
            _channel = NULL;
        }
        if (_device != NULL) {
            delete _device;
            _device = NULL;
        }
        if (_dd != NULL) {
            delete _dd;
            _dd = NULL;
        }
        if (_client != NULL) {
            _client = NULL;
        }
    }

    InstructionPool *TcpBackgroundSender::instructionPool() const {
        return _pool;
    }

    Device *TcpBackgroundSender::device() const {
        return _device;
    }

    Channel *TcpBackgroundSender::channel() const {
        return _channel;
    }

    DriverManager *TcpBackgroundSender::manager() {
        return _manager;
    }

    bool TcpBackgroundSender::connected() const {
//        Debug::writeFormatLine("TcpBackgroundSender::connected, socketId: %d, connected: %s, offline: %s",
//                               _client->socketId(), _client->connected() ? "true" : "false", _pool->isOffline() ? "true" : "false");
        return _client != nullptr ? _client->connected() : false;
//        return !_pool->isOffline();
    }

    void TcpBackgroundSender::setSenderStatus(Device::Status status) {
        if (_pool != nullptr)
            _pool->setConnectStatus(status);
    }

    Device::Status TcpBackgroundSender::senderStatus() const {
        return _pool != nullptr ? _pool->connectStatus() : Device::Status::Unknown;
    }

    void TcpBackgroundSender::createDevice(const Channel *channel) {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = _receiver->device();
        assert(device);

        String channelName = String::convert("%s_%s:%d",
                                             channel->name().c_str(),
                                             _client->peerEndpoint().address.c_str(),
                                             _client->peerEndpoint().port);
        ChannelDescription *cd = new ChannelDescription(channelName.c_str(),
                                                        channel->description()->clientInteractiveName());
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
        _device->addReceiveDevice(device);
    }

    TcpServerAsyncSender::TcpServerAsyncSender(DriverManager *dm, Channel *channel, TcpClient *client,
                                               TcpBackgroundReceiver *receiver) : TcpBackgroundSender(dm, channel,
                                                                                                      client,
                                                                                                      receiver) {
        createDevice(channel);
    }

    void TcpServerAsyncSender::createDevice(const Channel *channel) {
        TcpBackgroundSender::createDevice(channel);

        DriverManager *dm = manager();
        assert(dm);
        _pool = new SenderMultiPool(dm, _channel->description(), _dd);
        _pool->start(_channel, _device);
    }

    TcpServerSyncSender::TcpServerSyncSender(DriverManager *dm, Channel *channel, TcpClient *client,
                                             TcpBackgroundReceiver *receiver) : TcpBackgroundSender(dm, channel, client,
                                                                                                    receiver) {
        SenderSinglePool::startSinglePool();

        createDevice(channel);
    }

    void TcpServerSyncSender::createDevice(const Channel *channel) {
        TcpBackgroundSender::createDevice(channel);

        DriverManager *dm = manager();
        assert(dm);
        _pool = new SenderSinglePool(dm, _channel->description(), _dd);
        _pool->start(_channel, _device);
    }
}
