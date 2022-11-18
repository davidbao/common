#ifndef SERIALINTERACTIVE_H
#define SERIALINTERACTIVE_H

#include "Interactive.h"
#include "SerialChannelContext.h"
#include "ChannelDescription.h"
#include "Channel.h"
#include "IO/SerialPort.h"

namespace Drivers
{
	class SerialInteractive : public Interactive
	{
	public:
		SerialInteractive(DriverManager* dm, Channel* channel);
		~SerialInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout) override;

	private:
		inline SerialChannelContext* getChannelContext()  
		{
			return (SerialChannelContext*)(_channel->description()->context());
		}

	private:
		SerialPort*	_port;	
	};
}

#endif //SERIALINTERACTIVE_H
