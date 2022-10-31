#ifndef BLUETOOTHINTERACTIVE_H
#define BLUETOOTHINTERACTIVE_H

#include "Interactive.h"
#include "BluetoothChannelContext.h"
#include "ChannelDescription.h"
#include "Channel.h"
#include "net/BluetoothClient.h"
#include "BluetoothServerClient.h"

namespace Drivers
{
	class BluetoothInteractive : public Interactive
	{
	public:
		BluetoothInteractive(DriverManager* dm, Channel* channel);
		~BluetoothInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;

	private:
		inline BluetoothChannelContext* getChannelContext()  
		{
			return (BluetoothChannelContext*)(_channel->description()->context());
		}

		friend class BluetoothServerClient;

		void updateBluetoothClient(BluetoothClient* tcpClient)
		{
			_bluetoothClient = tcpClient;
			_autoDelete = false;
		}

	private:
		BluetoothClient* _bluetoothClient;
		bool _autoDelete;
        
        BluetoothServerClient* _receiveClient;
	};
}
#endif // BLUETOOTHINTERACTIVE_H
