#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "thread/Thread.h"
#include "driver/channels/Channel.h"
#include "driver/channels/BluetoothChannelContext.h"
#include "driver/channels/BluetoothInteractive.h"
#include "driver/channels/ChannelDescription.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    BluetoothInteractive::BluetoothInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel) {
        if (channel == nullptr)
            throw ArgumentException("channel");
        if ((BluetoothChannelContext *) channel->description()->context() == nullptr)
            throw ArgumentException("channel");

        _bluetoothClient = nullptr;
        _autoDelete = true;
        _receiveClient = nullptr;
    }

    BluetoothInteractive::~BluetoothInteractive() {
        close();
    }

    bool BluetoothInteractive::open() {
        close();

        BluetoothChannelContext *tcc = getChannelContext();

//#ifdef DEBUG
//		String message = Convert::convertStr("connect a host, address = %s, port = %d", tcc->address().c_str(), tcc->port());
//		Stopwatch sw(message, 100);
//#endif

        _bluetoothClient = new BluetoothClient();
        //bool result = _bluetoothClient->connectToHost(tcc->address().c_str(), tcc->port(), tcc->getOpenTimeout());
        //if (result)
        {
            if (tcc->sendBufferSize() > 0) {
                _bluetoothClient->setSendBufferSize(tcc->sendBufferSize());
            }
            if (tcc->receiveBufferSize() > 0) {
                _bluetoothClient->setReceiveBufferSize(tcc->receiveBufferSize());
            }

            if (tcc->asyncReceiver()) {
                _receiveClient = new BluetoothServerClient(manager(), _bluetoothClient, _channel, false);
            }
        }
        return true;
    }

    void BluetoothInteractive::close() {
        if (_autoDelete) {
            if (_receiveClient != nullptr) {
                delete _receiveClient;
                _receiveClient = nullptr;
            }
            if (_bluetoothClient != nullptr) {
                _bluetoothClient->close();
                delete _bluetoothClient;
                _bluetoothClient = nullptr;
            }
        }
    }

    bool BluetoothInteractive::connected() {
        return _bluetoothClient != nullptr ? _bluetoothClient->connected() : false;
    }

    size_t BluetoothInteractive::available() {
        return _bluetoothClient != nullptr ? _bluetoothClient->available() : 0;
    }

    ssize_t BluetoothInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("socket send", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _bluetoothClient->write(buffer + offset, count);
        }
        return len;
    }

    ssize_t BluetoothInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("bluetooth socket recv", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _bluetoothClient->read(buffer + offset, count);
        }
        return len;
    }
}
