//
//  SerialPort.h
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#ifndef SerialPort_h
#define SerialPort_h

#include "IOPort.h"
#include "SerialInfo.h"

namespace IO {
    class SerialPort final : public IOPort {
    public:
        explicit SerialPort(const String &name);

        ~SerialPort() override;

        bool open() override;

        const String &name() const override;

        const SerialInfo &serial() const;

        void setSerialInfo(const SerialInfo &serial);

        void setBaudRate(int baudRate);

        int baudRate() const;

        void setDataBits(SerialInfo::DataBitsType dataBits);

        SerialInfo::DataBitsType dataBits() const;

        void setParity(SerialInfo::ParityType parity);

        SerialInfo::ParityType parity() const;

        void setStopBits(SerialInfo::StopBitsType stopBits);

        SerialInfo::StopBitsType stopBits() const;

        void setHandshake(SerialInfo::HandshakeType handshake);

        SerialInfo::HandshakeType handshake() const;

        bool rtsEnable() const;

        void setRtsEnable(bool enabled);

        bool dtrEnable() const;

        void setDtrEnable(bool enabled);

        bool useSignal() const;

        void setUseSignal(bool useSignal);

    private:
        bool setCommConfig(int baudRate, SerialInfo::ParityType parity, SerialInfo::DataBitsType dataBits,
                           SerialInfo::StopBitsType stopBits, SerialInfo::HandshakeType handshake);

    private:
        SerialInfo _serial;
    };
}
#endif  // SerialPort_h
