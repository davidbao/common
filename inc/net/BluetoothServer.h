#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

namespace Net
{
	class BluetoothServer
	{
	public:
		BluetoothServer();
		~BluetoothServer();

		bool bind();
		bool listen(int maxConnections = 30);
		int accept();
		void close();

		inline bool isValid() const
		{
			return _socket != -1;
		}
		
		int sendBufferSize() const;
		void setSendBufferSize(int bufferSize);
		int receiveBufferSize() const;
		void setReceiveBufferSize(int bufferSize);

	private:
		int _socket;
		bool _isListening;
	};
}

#endif // BLUETOOTHSERVER_H
