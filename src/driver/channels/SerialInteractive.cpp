#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "diag/Trace.h"
#include "driver/channels/SerialInteractive.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    SerialInteractive::SerialInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel) {
        if (channel == nullptr)
            throw ArgumentException("channel");
        if ((SerialChannelContext *) channel->description()->context() == nullptr)
            throw ArgumentException("channel");

        _port = nullptr;
    }

    SerialInteractive::~SerialInteractive() {
        close();
    }

    bool SerialInteractive::open() {
        close();

        SerialChannelContext *scc = getChannelContext();

        String message = String::convert(
                "open a serial port, name = %s, baudrate = %d, databits = %s, parity = %s, stopbit = %s",
                scc->portName().c_str(),
                scc->baudRate(),
                SerialInfo::convertDataBitsStr(scc->dataBits()).c_str(),
                SerialInfo::convertParityStr(scc->parity()).c_str(),
                SerialInfo::convertStopBitsStr(scc->stopBits()).c_str());
        Trace::writeLine(message, Trace::Info);

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

    void SerialInteractive::close() {
        if (_port != nullptr) {
            _port->close();
            delete _port;
            _port = nullptr;
        }
    }

    bool SerialInteractive::connected() {
        return _port != nullptr ? _port->isOpen() : false;
    }

    size_t SerialInteractive::available() {
        return _port->available();
    }

    ssize_t SerialInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("serial send", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->send(buffer, offset, count);
        }
        return len;
    }

    ssize_t SerialInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("serial receive", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->receive(buffer, offset, count);
        }
        return len;
    }

    ssize_t SerialInteractive::receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) {
#ifdef DEBUG
        Stopwatch sw("serial receive2", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _port->receive(buffer, offset, count, timeout);
        }
        return len;
    }
}
