#ifndef BluetoothClient_h
#define BluetoothClient_h

#include "data/ValueType.h"
#include "data/NetType.h"

using namespace Common;

namespace Drivers
{
	class BluetoothClient
	{
	public:
		BluetoothClient(int socketId = -1);
		~BluetoothClient();

		void close();

		size_t available();
		ssize_t write(const uint8_t *data, size_t count);
		ssize_t read(uint8_t *data, size_t count);
		ssize_t receiveBySize(BluetoothClient* client, uint8_t* buffer, size_t bufferLength, off_t offset, size_t count, uint timeout = 3000);

		bool connected();

		inline int socketId() const
		{
			return _socket;
		}
        
        inline const Endpoint& peerEndpoint() const
        {
            return _peerEndpoint;
        }

		int sendBufferSize() const;
		void setSendBufferSize(int bufferSize);
		int receiveBufferSize() const;
		void setReceiveBufferSize(int bufferSize);

	private:
		int _socket;
		bool _connected;
        
        Endpoint _peerEndpoint;
	};
};
#endif // BluetoothClient_h
