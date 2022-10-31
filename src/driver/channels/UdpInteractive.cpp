#include "exception/Exception.h"
#include "diag/Stopwatch.h"
#include "thread/Thread.h"
#include "net/NetInterface.h"
#include "driver/channels/Channel.h"
#include "driver/channels/UdpChannelContext.h"
#include "driver/channels/UdpInteractive.h"
#include "driver/channels/ChannelDescription.h"

using namespace Common;

namespace Drivers
{
    UdpInteractive::Client::Client(const IPAddress& address, int port, const String& iface)
    {
        this->iface = iface;
        this->address = address;
        this->port = port;
        this->opened = !address.isEmpty();
        this->client = new UdpClient();
    }
    UdpInteractive::Client::Client(const IPAddress& address, int port, bool opened)
    {
        this->address = address;
        this->port = port;
        this->opened = opened;
        this->client = new UdpClient();
    }
    UdpInteractive::Client::Client(const Client& client) : Client(client.address,client.port, client.iface)
    {        
    }
    UdpInteractive::Client::~Client()
    {
        delete this->client;
        this->client = nullptr;
    }
    bool UdpInteractive::Client::open(UdpChannelContext* ucc)
    {
        bool result = this->client->open(address.toString(), port);
        if (result && ucc->sendBufferSize() > 0)
        {
            this->client->setSendBufferSize(ucc->sendBufferSize());
        }
        return result;
    }
    ssize_t UdpInteractive::Client::send(const ushort port, const uint8_t *data, size_t len)
    {
#ifdef WIN32
		return this->client->write(port, data, len);
#else
        if(isOpened())
            return this->client->write(port, data, len);
        else
        {
            IPAddress addr;
            if(NetInterface::isInterfaceUp(iface))
            {
                addr = NetInterface::getIpAddress(iface);
            }
            if(!addr.isEmpty())
            {
                Debug::writeFormatLine("open an udp client(delay), iface: %s, addr: %s, port: %d",
                                       iface.c_str(), addr.toString().c_str(), port);
                address = addr;
                opened = true;
                client->open(address.toString(), port);
                return this->client->write(port, data, len);
            }
            return 0;
        }
#endif
    }
    bool UdpInteractive::Client::isOpened() const
    {
        return opened;
    }
    UdpInteractive::Clients::Clients()
    {
    }
    bool UdpInteractive::Clients::contains(const Client* client) const
    {
        for (uint i=0; i<_items.count(); i++)
        {
            const Client* item = _items[i];
            if(item->iface == client->iface)
                return true;
        }
        return false;
    }
    void UdpInteractive::Clients::add(const Client* client)
    {
        if(!contains(client))
            _items.add(client);
    }
    void UdpInteractive::Clients::clear()
    {
        _items.clear();
    }
    size_t UdpInteractive::Clients::count() const
    {
        return _items.count();
    }
    UdpInteractive::Client* UdpInteractive::Clients::at(size_t pos) const
    {
        return _items.at(pos);
    }

	UdpInteractive::UdpInteractive(DriverManager* dm, Channel* channel) : Interactive(dm, channel)
	{
		if(channel == nullptr)
			throw ArgumentException("channel");
		if((UdpChannelContext*)channel->description()->context() == nullptr)
			throw ArgumentException("channel");
	}

	UdpInteractive::~UdpInteractive()
	{
		close();
	}

	bool UdpInteractive::open()
	{
		close();

		UdpChannelContext* ucc = getChannelContext();
#ifdef WIN32
		Client* client = new Client(ucc->address(), ucc->port(), true);
		_clients.add(client);
        return client->open(ucc);
#else
        if(ucc->address() == "any")
        {
            StringArray ifaces;
            NetInterface::getInterfaceNames(ifaces);
            for(uint i=0;i<ifaces.count();i++)
            {
                String iface = ifaces[i];
                if(iface.find("lo") < 0)
                {
                    Debug::writeFormatLine("interface name: %s", iface.c_str());
                    IPAddress addr;
                    if(NetInterface::isInterfaceUp(iface))
                    {
                        addr = NetInterface::getIpAddress(iface);
                    }

                    if(!addr.isEmpty())
                    {
                        Debug::writeFormatLine("open an udp client, iface: %s, addr: %s, port: %d",
                                               iface.c_str(), addr.toString().c_str(), ucc->port());
                        Client* client = new Client(addr, ucc->port(), iface);
                        _clients.add(client);
                        client->open(ucc);
                    }
                    else
                    {
                        Client* client = new Client(IPAddress::Empty, ucc->port(), iface);
                        _clients.add(client);
                    }
                }
            }
            return true;
        }
        else
        {
            Client* client = new Client(ucc->address(), ucc->port(), true);
            _clients.add(client);
            return client->open(ucc);
        }
#endif
	}

	void UdpInteractive::close()
	{
        for (uint i=0; i<_clients.count(); i++)
        {
            Client* client = _clients[i];
			client->client->close();
        }
        _clients.clear();
	}

	bool UdpInteractive::connected()
	{
		return _clients.count() > 0;
	}
    size_t UdpInteractive::available()
	{
		return 0;
	}

	ssize_t UdpInteractive::send(const uint8_t* buffer, off_t offset, size_t count)
	{
		ssize_t len = 0;
		if(connected())
		{
            for (size_t i=0; i<_clients.count(); i++)
            {
                Client* client = _clients[i];
                ssize_t length = sendInner(client, buffer, offset, count);
                if(length > 0)
                    len = length;
            }
		}
		return len;
	}
    ssize_t UdpInteractive::sendInner(Client* client, const uint8_t* buffer, off_t offset, size_t count)
    {
#ifdef DEBUG
        Stopwatch sw("socket send", 1000);
#endif
        ssize_t len = 0;
        if(connected())
        {
            UdpChannelContext* context = getChannelContext();
            len = client->send(context->port(), buffer + offset, count);
//#ifdef DEBUG
//            Debug::writeFormatLine("udp send, len: %d, iface: %s, addr: %s, port: %d", len, client->iface.c_str(), client->address.toString().c_str(), client->port);
//#endif
        }
        return len;
    }

	ssize_t UdpInteractive::receive(uint8_t* buffer, off_t offset, size_t count)
	{
		throw new NotSupportedException("UdpClient can not receive anything.");
	}
}
