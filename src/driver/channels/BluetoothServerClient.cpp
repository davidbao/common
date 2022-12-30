#include <assert.h>
#include "thread/Locker.h"
#include "thread/TickTimeout.h"
#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "system/Resources.h"
#include "driver/DriverManager.h"
#include "driver/devices/DeviceDescription.h"
#include "driver/channels/ChannelDescription.h"
#include "driver/channels/BluetoothInteractive.h"
#include "driver/channels/BluetoothServerClient.h"

using namespace Diag;

namespace Drivers {
    void bluetooth_receiveProc(void *parameter) {
        BluetoothServerClient *ti = (BluetoothServerClient *) parameter;
        assert(ti);
        ti->receiveProcInner();
    }

    BluetoothServerClient::BluetoothServerClient(DriverManager *dm, BluetoothClient *client, Channel *channel,
                                                 bool autoDelete) {
        if (dm == nullptr)
            throw ArgumentNullException("dm");
        if (client == nullptr)
            throw ArgumentNullException("client");
        if (channel == nullptr)
            throw ArgumentNullException("channel");

        _client = client;
        _autoDelete = autoDelete;
        _manager = dm;

        _context = dynamic_cast<BluetoothChannelBaseContext *>(channel->description()->context());
        assert(_context);

        if (autoDelete) {
            createDevice(channel);
        } else {
            updateDevice(channel);
        }

        _receiveThread = new Thread("Bluetooth_receiveProc");
        _receiveThread->startProc(bluetooth_receiveProc, this, 1);

        _startTime = TickTimeout::getCurrentTickCount();
    }

    BluetoothServerClient::~BluetoothServerClient() {
        if (_receiveThread != nullptr) {
            _receiveThread->stop();
            delete _receiveThread;
            _receiveThread = nullptr;
        }

        if (_channel != nullptr) {
            if (_autoDelete)
                delete _channel;
            _channel = nullptr;
        }
        if (_device != nullptr) {
            if (_autoDelete)
                delete _device;
            _device = nullptr;
        }
        if (_dd != nullptr) {
            if (_autoDelete)
                delete _dd;
            _dd = nullptr;
        }

        if (_client != nullptr) {
            if (_autoDelete)
                delete _client;
            _client = nullptr;
        }
    }

    size_t BluetoothServerClient::available() {
        size_t result = _client != nullptr ? _client->available() : 0;
        if (result > 0) {
            _startTime = TickTimeout::getCurrentTickCount();
        }
        return result;
    }

    bool BluetoothServerClient::connected() const {
        TimeSpan clientTimeout = _context->closeTimout();
        if (TickTimeout::isTimeout(_startTime, clientTimeout))
            return false;

        return _client != nullptr ? _client->connected() : false;
    }

    BluetoothClient *BluetoothServerClient::getBluetoothClient() const {
        return _client;
    }

    const Endpoint &BluetoothServerClient::peerEndpoint() const {
        return _client->peerEndpoint();
    }

    int BluetoothServerClient::socketId() const {
        return _client->socketId();
    }

    void BluetoothServerClient::receiveProcInner() {
#ifdef DEBUG
        Stopwatch sw("BluetoothServerClient_receiveProc", 1000);
#endif

        if (connected() && available() > 0) {
            receiveFromBuffer(_device);
        }
    }

    void BluetoothServerClient::createDevice(const Channel *channel) {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = dm->getDevice(channel);
        assert(device);

        String channelName = "BluetoothClient";
        ChannelDescription *cd = new ChannelDescription(channelName, "BluetoothInteractive");
        BluetoothChannelBaseContext *tc = dynamic_cast<BluetoothChannelBaseContext *>(cd->context());
        tc->copyFrom(_context);
        _channel = new Channel(dm, cd);
        BluetoothInteractive *ti = dynamic_cast<BluetoothInteractive *>(_channel->interactive());
        assert(ti);
        ti->updateBluetoothClient(_client);

        String deviceName = "BluetoothClientDevice";
        DeviceDescription *dd = new DeviceDescription(deviceName, cd,
                                                      device->instructionSet()->clone(),
                                                      device->description()->context()->clone());
        _dd = dd;
        _device = new Device(dd, _channel);
    }

    void BluetoothServerClient::updateDevice(Channel *channel) {
        DriverManager *dm = manager();
        assert(dm);
        Device *device = dm->getDevice(channel);
        assert(device);

        _channel = channel;
        _device = device;
    }

    ssize_t BluetoothServerClient::send(const uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("socket send", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _client->write(buffer + offset, count);
        }
        return len;
    }
}
