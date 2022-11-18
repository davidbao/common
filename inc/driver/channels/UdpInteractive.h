#ifndef UDPINTERACTIVE_H
#define UDPINTERACTIVE_H

#include "Interactive.h"
#include "UdpChannelContext.h"
#include "ChannelDescription.h"
#include "Channel.h"
#include "net/UdpClient.h"

namespace Drivers
{
	class UdpInteractive : public Interactive
	{
	public:
        class Client
        {
        public:
            String iface;
            IPAddress address;
            int port;
            bool opened;
            UdpClient* client;
            
            Client(const IPAddress& address, int port, const String& iface);
            Client(const IPAddress& address, int port, bool opened);
            Client(const Client& client);
            ~Client();
            
            bool open(UdpChannelContext* ucc);
            
            ssize_t send(const uint16_t port, const uint8_t *data, size_t len);
            
        private:
            bool isOpened() const;
        };
        class Clients : public IIndexGetter<Client*>
        {
        public:
            Clients();
            
            bool contains(const Client* client) const;
            void add(const Client* client);
            void clear();
            size_t count() const;
            Client* at(size_t pos) const override;

        private:
            PList<Client> _items;
        };
        
		UdpInteractive(DriverManager* dm, Channel* channel);
		~UdpInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;

	private:
		inline UdpChannelContext* getChannelContext()
		{
			return (UdpChannelContext*)(_channel->description()->context());
		}
        
        ssize_t sendInner(Client* client, const uint8_t* buffer, off_t offset, size_t count);

	private:
		Clients _clients;
	};
}
#endif // UDPINTERACTIVE_H
