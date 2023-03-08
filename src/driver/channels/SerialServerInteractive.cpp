#include <cassert>
#include "driver/channels/SerialServerInteractive.h"
#include "driver/DriverManager.h"
#include "system/Resources.h"
#include "diag/Stopwatch.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    SerialServerInteractive::SerialServerInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel),
                                                                                            _port(nullptr),
                                                                                            _receiveThread(nullptr),
                                                                                            _device(nullptr) {
        if (channel == nullptr)
            throw ArgumentException("channel");
        if ((SerialChannelContext *) channel->description()->context() == nullptr)
            throw ArgumentException("channel");
    }

    SerialServerInteractive::~SerialServerInteractive() {
        close();
    }

    bool SerialServerInteractive::open() {
        _receiveThread = new Timer(
                "Serial_receiveProc", 1,
                &SerialServerInteractive::receiveProc, this);

        SerialChannelContext *scc = getChannelContext();

#ifdef DEBUG
        String message = String::convert("open a serial port, name = %s", scc->portName().c_str());
        Stopwatch sw(message);
#endif

        _port = new SerialPort(scc->portName().c_str());
        _port->setBaudRate(scc->baudRate());
        _port->setStopBits(scc->stopBits());
        _port->setDataBits(scc->dataBits());
        _port->setParity(scc->parity());
        _port->setHandshake(scc->handshake());
        _port->setUseSignal(scc->useSignal());
        _port->setRtsEnable(scc->rtsEnable());
        _port->setDtrEnable(scc->dtrEnable());

        return _port->open();
    }

    void SerialServerInteractive::close() {
        if (_receiveThread != nullptr) {
            _receiveThread->stop();
            delete _receiveThread;
            _receiveThread = nullptr;
        }

        if (_port != nullptr) {
            _port->close();
            delete _port;
            _port = nullptr;
        }

        _device = nullptr;
    }

    bool SerialServerInteractive::connectdInner() const {
        return _port != nullptr && _port->isOpen();
    }

    bool SerialServerInteractive::connected() {
        return connectdInner();
    }

    size_t SerialServerInteractive::available() {
        return connected() ? _port->available() : 0;
    }

    ssize_t SerialServerInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("serial send", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->send(buffer, offset, count);
        }
        return len;
    }

    ssize_t SerialServerInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("serial receive", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->receive(buffer, offset, count);
        }
        return len;
    }

    ssize_t SerialServerInteractive::receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) {
#ifdef DEBUG
        Stopwatch sw("serial receive2", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->receive(buffer, offset, count, timeout);
        }
        return len;
    }

    void SerialServerInteractive::receiveProc() {
#ifdef DEBUG
        Stopwatch sw("SerialServerInteractive_receiveProc", 1000);
#endif
        if (_device == nullptr) {
            DriverManager *dm = manager();
            assert(dm);
            _device = dm->getDevice(_channel);
        }

        if (connected() && available() > 0) {
            receiveFromBuffer(_device);
        }
    }
}
