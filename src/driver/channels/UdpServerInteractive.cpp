#include <assert.h>
#include "driver/channels/UdpServerInteractive.h"
#include "driver/devices/Device.h"
#include "driver/DriverManager.h"
#include "system/Resources.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "thread/Thread.h"
#include "diag/Trace.h"

using namespace Data;
using namespace Diag;

namespace Drivers {
    void udp_receiveProc(void *parameter) {
        UdpServerInteractive *ui = (UdpServerInteractive *) parameter;
        assert(ui);
        ui->receiveProcInner();
    }

    UdpServerInteractive::UdpServerInteractive(DriverManager *dm, Channel *channel) : Interactive(dm, channel),
                                                                                      _udpServer(nullptr),
                                                                                      _receiveThread(nullptr),
                                                                                      _retrieved(false) {
        if (channel == nullptr)
            throw ArgumentException("channel");
        if ((TcpServerChannelContext *) channel->description()->context() == nullptr)
            throw ArgumentException("channel");

        _devices.setAutoDelete(false);
    }

    UdpServerInteractive::~UdpServerInteractive() {
        close();
    }

    bool UdpServerInteractive::open() {
        _receiveThread = new Thread("Udp_receiveProc");
        _receiveThread->startProc(udp_receiveProc, this, 1);

        return rebind();
    }

    bool UdpServerInteractive::rebind() {
        static int bindingCount = 0;
        static const int MaxBindingCount = 5;
        static const uint32_t MaxDelayTime = 2000;    // 2s

        if (_udpServer != nullptr) {
            _udpServer->close();
            delete _udpServer;
            _udpServer = nullptr;
        }
        _udpServer = new UdpServer();

        UdpServerChannelContext *ucc = getChannelContext();
#ifdef DEBUG
        String message = String::convert("bind a udp socket, port = %d", ucc->port());
        Stopwatch sw(message);
#endif

        bindingCount++;

        bool result = _udpServer->bind(ucc->address(), ucc->port());
        if (!result) {
            Debug::writeFormatLine("Failed to bind the UdpServerInteractive socket. port = %d", ucc->port());

            if (bindingCount < MaxBindingCount) {
                Thread::msleep(MaxDelayTime);
                return rebind();
            } else {
                String message = String::convert(Resources::getString("SocketBindingFailed2").c_str(), ucc->port());
                Trace::writeLine(message.c_str(), Trace::Error);
                throw BindingException(message.c_str());
            }
            return false;
        } else {
            if (ucc->receiveBufferSize() > 0) {
                _udpServer->setReceiveBufferSize(ucc->receiveBufferSize());
            }
            return true;
        }
    }

    void UdpServerInteractive::close() {
        if (_receiveThread != nullptr) {
            _receiveThread->stop();
            delete _receiveThread;
            _receiveThread = nullptr;
        }

        if (_udpServer != nullptr) {
            _udpServer->close();
            delete _udpServer;
            _udpServer = nullptr;
        }
    }

    bool UdpServerInteractive::connectdInner() const {
        return _udpServer != nullptr;
    }

    bool UdpServerInteractive::connected() {
        return connectdInner();
    }

    size_t UdpServerInteractive::available() {
        return _udpServer->available();
    }

    const Endpoint &UdpServerInteractive::peerEndpoint() const {
        return connectdInner() ? _udpServer->peerEndpoint() : Endpoint::Empty;
    }

    const Endpoint &UdpServerInteractive::endpoint() const {
        return connectdInner() ? _udpServer->endpoint() : Endpoint::Empty;
    }

    ssize_t UdpServerInteractive::send(const uint8_t *buffer, off_t offset, size_t count) {
        throw new NotSupportedException("UdpServer can not send anything.");
    }

    ssize_t UdpServerInteractive::receive(uint8_t *buffer, off_t offset, size_t count) {
#ifdef DEBUG
        Stopwatch sw("udp socket recv", 1000);
#endif

        ssize_t len = 0;
        if (connected()) {
            UdpServerChannelContext *context = getChannelContext();
            len = _udpServer->read(buffer + offset, count, context->peek());
//#ifdef DEBUG
//            Debug::writeFormatLine("udp receive, len: %d", len);
//#endif
        }
        return len;
    }

    ssize_t UdpServerInteractive::receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) {
#ifdef DEBUG
        Stopwatch sw("socket recv2", 1000);
#endif
        ssize_t len = 0;
        if (connected()) {
            len = _udpServer->receive(buffer + offset, count, timeout);
        }
        return len;
    }

    void UdpServerInteractive::receiveProcInner() {
#ifdef DEBUG
        Stopwatch sw("udp server receiveProc", 1000);
#endif
        if (!_retrieved) {
            _retrieved = true;
            DriverManager *dm = manager();
            assert(dm);
            dm->getDevices(_channel, _devices);
        }

        if (connected() && available() > 0) {
            if (_devices.count() > 0) {
                ByteArray buffer;
                Device *first = _devices[0];
                if (first->receive(&buffer)) {
                    for (uint32_t i = 0; i < _devices.count(); i++) {
                        Device *device = _devices[i];
                        if (device->executeInstruction(buffer))
                            break;
                    }
                }
            }
        }
    }
}
