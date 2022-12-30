#ifndef SERIALCHANNELCONTEXT_H
#define SERIALCHANNELCONTEXT_H

#include "ChannelContext.h"
#include "IO/SerialInfo.h"

using namespace Data;

namespace Drivers
{
	class SerialChannelContext : public ChannelContext
	{
	public:
        SerialChannelContext(const String& portName = String::Empty);
		~SerialChannelContext() override;

        void setPortName(const String& portName);
        const String& portName() const;
		void setBaudRate(int);
		void setBaudRate(const String&);
		int baudRate() const;
		void setDataBits(SerialInfo::DataBitsType);
		void setDataBits(const String&);
		SerialInfo::DataBitsType dataBits() const;
		void setParity(SerialInfo::ParityType);
		void setParity(const String&);
		SerialInfo::ParityType parity() const;
		void setStopBits(SerialInfo::StopBitsType);
		void setStopBits(const String&);
		SerialInfo::StopBitsType stopBits() const;
		void setHandshake(SerialInfo::HandshakeType);
		void setHandshake(const String&);
		SerialInfo::HandshakeType handshake() const;
        bool rtsEnable() const;
        void setRtsEnable(bool enabled);
        bool dtrEnable() const;
        void setDtrEnable(bool enabled);
        bool useSignal() const;
        void setUseSignal(bool useSignal);
        
        const SerialInfo& serial() const;
        void setSerial(const SerialInfo& serial);

	private:
        SerialInfo _serial;
	};
}

#endif //SERIALCHANNELCONTEXT_H
