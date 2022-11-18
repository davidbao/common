#include <assert.h>
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "thread/Thread.h"
#include "diag/Trace.h"
#include "driver/DriverManager.h"
#include "driver/devices/Device.h"
#include "system/Resources.h"
#include "driver/channels/BluetoothServerInteractive.h"

using namespace Common;

namespace Drivers
{
	void bluetooth_acceptProc(void* parameter)
	{
		BluetoothServerInteractive* ti = (BluetoothServerInteractive*)parameter;
		assert(ti);
		ti->acceptProcInner();
	}
	void bluetooth_closeProc(void* parameter)
	{
		BluetoothServerInteractive* ti = (BluetoothServerInteractive*)parameter;
		assert(ti);
		ti->closeProcInner();
	}

	BluetoothServerInteractive::BluetoothServerInteractive(DriverManager* dm, Channel* channel) : Interactive(dm, channel),
		_bluetoothServer(nullptr), _acceptThread(nullptr)
	{
		if(channel == nullptr)
			throw ArgumentNullException("channel");
		if((BluetoothServerChannelContext*)channel->description()->context() == nullptr)
			throw ArgumentNullException("channel");

		_acceptAction = nullptr;
		_closeAction = nullptr;
		_closeThread = nullptr;
	}

	BluetoothServerInteractive::~BluetoothServerInteractive()
	{
		close();
	}

	bool BluetoothServerInteractive::open()
	{
		return rebind();
	}

	bool BluetoothServerInteractive::rebind()
	{
		static int bindingCount = 0;
		static const int MaxBindingCount = 5;
		static const uint32_t MaxDelayTime = 2000;	// 2s

		_bluetoothServerMutex.lock();
		if (_bluetoothServer != nullptr)
		{
			_bluetoothServer->close();
			delete _bluetoothServer;
			_bluetoothServer = nullptr;
		}

		_bluetoothServer = new BluetoothServer();

		BluetoothServerChannelContext* tcc = getChannelContext();
#ifdef DEBUG
		String message = "listen a bluetooth socket";
		Stopwatch sw(message);
#endif

		bindingCount++;

		bool result = _bluetoothServer->bind();
		if(result)
		{
			result = _bluetoothServer->listen(tcc->maxConnections());
			_bluetoothServerMutex.unlock();
			if(result)
			{
				if (result)
				{
					if (tcc->sendBufferSize() > 0)
					{
						_bluetoothServer->setSendBufferSize(tcc->sendBufferSize());
					}
					if (tcc->receiveBufferSize() > 0)
					{
						_bluetoothServer->setReceiveBufferSize(tcc->receiveBufferSize());
					}
				}

				_acceptThread = new Thread("Bluetooth_acceptProc");
				_acceptThread->startProc(bluetooth_acceptProc, this, 1);

				_closeThread = new Thread("Bluetooth_closeProc");
				_closeThread->startProc(bluetooth_closeProc, this, 1000);

				return true;
			}
			else
			{
				Debug::writeLine("Failed to listen the BluetoothServerInteractive socket.");

				if(bindingCount < MaxBindingCount)
				{
					Thread::msleep(MaxDelayTime);
					return rebind();
				}
				else
				{
					String message = Resources::getString("BluetoothListenFailedStr");
					Trace::writeLine(message.c_str(), Trace::Error);
					throw BindingException(message.c_str());
				}
			}
		}
		else
		{
			_bluetoothServerMutex.unlock();
			Debug::writeLine("Failed to bind the BluetoothServerInteractive socket.");

			if(bindingCount < MaxBindingCount)
			{
				Thread::msleep(MaxDelayTime);
				return rebind();
			}
			else
			{
				String message = Resources::getString("BluetoothBindingFailedStr");
				Trace::writeLine(message.c_str(), Trace::Error);
			}
		}
		return false;
	}

	void BluetoothServerInteractive::close()
	{
		_bluetoothServerMutex.lock();
		if(_bluetoothServer != nullptr)
		{
			_bluetoothServer->close();
			delete _bluetoothServer;
			_bluetoothServer = nullptr;
		}
		_bluetoothServerMutex.unlock();

		if(_acceptThread != nullptr)
		{
			_acceptThread->stop();
			delete _acceptThread;
			_acceptThread = nullptr;
		}
		if (_closeThread != nullptr)
		{
			_closeThread->stop();
			delete _closeThread;
			_closeThread = nullptr;
		}
	}

	bool BluetoothServerInteractive::connected()
    {
        Locker locker(&_mutexClients);
        for (off_t i = 0; i < _clients.count(); i++)
        {
            BluetoothServerClient* client = _clients.at(i);
            if (client != nullptr)
            {
                if (client->connected())
                {
                    return true;
                }
            }
        }
        return false;
        //		throw NotSupportedException("Cannot support this method, use 'BluetoothServerClient' instead.");
    }
    size_t BluetoothServerInteractive::available()
	{
        return 0;
//		throw NotSupportedException("Cannot support this method, use 'BluetoothServerClient' instead.");
	}

	ssize_t BluetoothServerInteractive::send(const uint8_t* buffer, off_t offset, size_t count)
	{
        Locker locker(&_mutexClients);
        for (off_t i = 0; i < _clients.count(); i++)
        {
            BluetoothServerClient* client = _clients.at(i);
            if (client != nullptr)
            {
                // todo: add a pool that it contains buffer.
                client->send(buffer, offset, count);
            }
        }
        return count;
	}

	ssize_t BluetoothServerInteractive::receive(uint8_t* buffer, off_t offset, size_t count)
	{
		throw NotSupportedException("Cannot support this method, use 'BluetoothServerClient' instead.");
	}

	void BluetoothServerInteractive::acceptProcInner()
	{
		if(_bluetoothServer != nullptr)
		{
			int socketId = _bluetoothServer->accept();
			//Debug::writeLine(Convert::convertStr("client connected, socketId: %d", socketId), Trace::Info);

			Locker locker(&_bluetoothServerMutex);
			bool valid = _bluetoothServer != nullptr ? _bluetoothServer->isValid() : false;
			if(valid && socketId != -1)
			{
				BluetoothClient* client = new BluetoothClient(socketId);
				Locker locker2(&_mutexClients);
				_clients.add(new BluetoothServerClient(manager(), client, _channel));

				if(_acceptAction != nullptr)
				{
					_acceptAction(client->peerEndpoint());
				}
                
                Trace::writeLine(String::convert("client connected, peerAddr: %s, socketId: %d",
					client->peerEndpoint().address.c_str(), socketId), Trace::Info);
			}
		}
	}

	void BluetoothServerInteractive::closeProcInner()
	{
		_mutexClients.lock();
		for (off_t i = 0; i < _clients.count(); i++)
		{
			BluetoothServerClient* client = _clients.at(i);
			if (client != nullptr)
			{
				if (!client->connected())
				{
                    Endpoint peerEndpoint = client->getBluetoothClient()->peerEndpoint();
                    
					Trace::writeLine(String::convert("Remove an unused client, peerAddr: %s, socketId: %d",
						client->peerEndpoint().address.c_str(), client->socketId()), Trace::Info);
					_clients.remove(client);

					if (_closeAction != nullptr)
					{
						_closeAction(peerEndpoint);
					}
				}
			}
		}
		_mutexClients.unlock();
	}
}
