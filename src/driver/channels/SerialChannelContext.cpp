#include "driver/channels/SerialChannelContext.h"

namespace Drivers
{
    SerialChannelContext::SerialChannelContext(const String& portName) : _serial(portName)
	{
	}
	SerialChannelContext::~SerialChannelContext()
	{
	}

    void SerialChannelContext::setPortName(const String& portName)
    {
        _serial.portName = portName;
    }
    const String& SerialChannelContext::portName() const
    {
        return _serial.portName;
    }
	void SerialChannelContext::setBaudRate(int baudRate)
	{
		_serial.baudRate = baudRate;
	}
	void SerialChannelContext::setBaudRate(const String& baudRate)
	{
		int value;
		if (Int32::parse(baudRate, value))
		{
			_serial.baudRate = value;
		}
	}
	int SerialChannelContext::baudRate() const
	{
		return _serial.baudRate;
	}
	void SerialChannelContext::setDataBits(SerialInfo::DataBitsType dataBits)
	{
		_serial.dataBits = dataBits;
	}
	void SerialChannelContext::setDataBits(const String& dataBits)
	{
        _serial.dataBits = SerialInfo::parseDataBits(dataBits);
	}
	SerialInfo::DataBitsType SerialChannelContext::dataBits() const
	{
		return _serial.dataBits;
	}
	void SerialChannelContext::setParity(SerialInfo::ParityType parity)
	{	
		_serial.parity = parity;
	}
	void SerialChannelContext::setParity(const String& parity)
	{
		_serial.parity = SerialInfo::parseParity(parity);
	}
	SerialInfo::ParityType SerialChannelContext::parity() const
	{	
		return _serial.parity;
	}
	void SerialChannelContext::setStopBits(SerialInfo::StopBitsType stopBits)
	{
		_serial.stopBits = stopBits;
	}
	void SerialChannelContext::setStopBits(const String& stopBits)
	{
		_serial.stopBits = SerialInfo::parseStopBits(stopBits);
	}
	SerialInfo::StopBitsType SerialChannelContext::stopBits() const
	{
		return _serial.stopBits;
	}
	void SerialChannelContext::setHandshake(SerialInfo::HandshakeType handshake)
	{
		_serial.handshake = handshake;
	}
	void SerialChannelContext::setHandshake(const String& handshake)
	{
		_serial.handshake = SerialInfo::parseHandshake(handshake);
	}
	SerialInfo::HandshakeType SerialChannelContext::handshake() const
	{
		return _serial.handshake;
	}
    bool SerialChannelContext::rtsEnable() const
    {
        return _serial.rtsEnable;
    }
    void SerialChannelContext::setRtsEnable(bool enabled)
    {
        _serial.rtsEnable = enabled;
    }
    bool SerialChannelContext::dtrEnable() const
    {
        return _serial.dtrEnable;
    }
    void SerialChannelContext::setDtrEnable(bool enabled)
    {
        _serial.dtrEnable = enabled;
    }
    bool SerialChannelContext::useSignal() const
    {
        return _serial.useSignal;
    }
    void SerialChannelContext::setUseSignal(bool useSignal)
    {
        _serial.useSignal = useSignal;
    }
    
    const SerialInfo& SerialChannelContext::serial() const
    {
        return _serial;
    }
    void SerialChannelContext::setSerial(const SerialInfo& serial)
    {
        _serial = serial;
    }
}
