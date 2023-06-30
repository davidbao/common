#ifndef UDPSERVERINTERACTIVE_H
#define UDPSERVERINTERACTIVE_H

#include "Interactive.h"
#include "Channel.h"
#include "UdpServerChannelContext.h"
#include "net/UdpClient.h"
#include "net/UdpServer.h"
#include "thread/Timer.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "driver/devices/Device.h"

using namespace Data;

namespace Drivers
{
	class UdpServerInteractive : public Interactive, public EthernetEndpoint, public BackgroundReceiver
	{
	public:
		UdpServerInteractive(DriverManager* dm, Channel* channel);
		~UdpServerInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

        ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;
        ssize_t receive(uint8_t* buffer, off_t offset, size_t count, uint32_t timeout) override;
        
        const Endpoint& peerEndpoint() const override;
        const Endpoint& endpoint() const override;

	private:
		void receiveProc();
        
        bool connectdInner() const;

		UdpServerChannelContext* getChannelContext() const;

		bool rebind();

	private:
		UdpServer* _udpServer;

		Timer* _receiveTimer;

		Devices _devices;
        bool _retrieved;
	};
}

#endif // UDPSERVERINTERACTIVE_H
