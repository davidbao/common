#ifndef BLUETOOTHRECEIVECLIENT_H
#define BLUETOOTHRECEIVECLIENT_H

#include "Interactive.h"
#include "Channel.h"
#include "BluetoothServerChannelContext.h"
#include "net/BluetoothClient.h"
#include "net/BluetoothServer.h"
#include "data/LoopArray.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "data/TimeSpan.h"

using namespace Common;

namespace Drivers
{
	class Device;
	class Channel;
	class DriverManager;
    class DeviceDescription;
    class BluetoothServerClient : public BackgroudReceiver
	{
	public:
		BluetoothServerClient(DriverManager* dm, BluetoothClient* client, Channel* channel, bool autoDelete = true);
		~BluetoothServerClient() override;

		size_t available();
		bool connected() const;

		BluetoothClient* getBluetoothClient() const;
		const Endpoint& peerEndpoint() const;
		int socketId() const;
        
        ssize_t send(const uint8_t* buffer, off_t offset, size_t count);

	private:
		friend void bluetooth_receiveProc(void* parameter);
		void receiveProcInner();

		void createDevice(const Channel* channel);
        void updateDevice(Channel* channel);

		DriverManager* manager()
		{
			return _manager;
		}

	private:
		BluetoothClient* _client;
		uint _startTime;
        bool _autoDelete;

		BluetoothChannelBaseContext* _context;

		Thread* _receiveThread;

		Channel* _channel;
		Device* _device;
        DeviceDescription* _dd;

		DriverManager* _manager;
	};

	typedef PList<BluetoothServerClient> BluetoothServerClients;
}
#endif // BLUETOOTHRECEIVECLIENT_H
