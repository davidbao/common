#ifndef SERIALPORT_H
#define SERIALPORT_H

#include "IOPort.h"
#include "SerialInfo.h"

namespace Common
{
	class SerialPort : public IOPort
	{
	public:
		SerialPort(const String& name);
		~SerialPort() override;

		bool open() override;
        
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
		bool setCommConfig(int baudRate, SerialInfo::ParityType parity, SerialInfo::DataBitsType dataBits, SerialInfo::StopBitsType stopBits, SerialInfo::HandshakeType handshake);

	private:
		int _baudRate;
		SerialInfo::DataBitsType _dataBits;
		SerialInfo::StopBitsType _stopBits;
		SerialInfo::HandshakeType _handshake;
		SerialInfo::ParityType _parity;
		bool _rtsEnable;
		bool _dtrEnable;
		bool _useSignal;
	};
}
#endif  //SERIALPORT_H
