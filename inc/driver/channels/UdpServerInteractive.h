#ifndef UDPSERVERINTERACTIVE_H
#define UDPSERVERINTERACTIVE_H

#include "Interactive.h"
#include "Channel.h"
#include "UdpServerChannelContext.h"
#include "net/UdpClient.h"
#include "net/UdpServer.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "driver/devices/Device.h"

using namespace Data;

namespace Drivers
{
	class UdpServerInteractive : public Interactive, public EthernetEndpoint, public BackgroudReceiver
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
		friend void udp_receiveProc(void* parameter);
		void receiveProcInner();
        
        bool connectdInner() const;

		inline UdpServerChannelContext* getChannelContext()
		{
			return (UdpServerChannelContext*)(_channel->description()->context());
		}

		bool rebind();

	private:
		UdpServer* _udpServer;

		Thread* _receiveThread;

		Devices _devices;
        bool _retrieved;
	};
}

#endif // UDPSERVERINTERACTIVE_H
