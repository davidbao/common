#ifndef BLUETOOTHSERVERINTERACTIVE_H
#define BLUETOOTHSERVERINTERACTIVE_H

#include "Interactive.h"
#include "Channel.h"
#include "BluetoothServerChannelContext.h"
#include "net/BluetoothClient.h"
#include "net/BluetoothServer.h"
#include "BluetoothServerClient.h"
#include "TcpServerInteractive.h"
#include "thread/Mutex.h"
#include "data/LoopArray.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"

using namespace Common;

namespace Drivers
{
	class BluetoothServerInteractive : public Interactive
	{
	public:
		BluetoothServerInteractive(DriverManager* dm, Channel* channel);
		~BluetoothServerInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;

		inline void setAcceptAction(client_accpet_callback acceptAction)
		{
			_acceptAction = acceptAction;
		}
		inline void setCloseAction(client_close_callback closeAction)
		{
			_closeAction = closeAction;
		}

	private:
		friend void bluetooth_acceptProc(void* parameter);
		void acceptProcInner();

		friend void bluetooth_closeProc(void* parameter);
		void closeProcInner();

		inline BluetoothServerChannelContext* getChannelContext()  
		{
			return (BluetoothServerChannelContext*)(_channel->description()->context());
		}

		bool rebind();

	private:
		BluetoothServer* _bluetoothServer;
		Mutex _bluetoothServerMutex;

		Mutex _mutexClients;
		BluetoothServerClients _clients;

		Thread* _acceptThread;
		Thread* _closeThread;

		client_accpet_callback _acceptAction;
		client_close_callback _closeAction;
	};
}

#endif // BLUETOOTHSERVERINTERACTIVE_H
