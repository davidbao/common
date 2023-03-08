#ifndef SERIALSERVERINTERACTIVE_H
#define SERIALSERVERINTERACTIVE_H

#include "Interactive.h"
#include "Channel.h"
#include "UdpServerChannelContext.h"
#include "thread/Timer.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "IO/SerialPort.h"

using namespace Data;

namespace Drivers {
    class Device;

    class SerialServerInteractive : public Interactive, public BackgroudReceiver {
    public:
        SerialServerInteractive(DriverManager *dm, Channel *channel);

        ~SerialServerInteractive() override;

        bool open() override;

        void close() override;

        bool connected() override;

        size_t available() override;

        ssize_t send(const uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count, uint32_t timeout) override;

    private:
        void receiveProc();

        bool connectdInner() const;

        inline SerialChannelContext *getChannelContext() {
            return (SerialChannelContext *) (_channel->description()->context());
        }

    private:
        SerialPort *_port;

        Timer *_receiveThread;

        Device *_device;
    };
}

#endif // SERIALSERVERINTERACTIVE_H
