#ifndef TCPBACKGROUNDRECEIVER_H
#define TCPBACKGROUNDRECEIVER_H

#include "Interactive.h"
#include "Channel.h"
#include "TcpServerChannelContext.h"
#include "net/TcpClient.h"
#include "net/TcpServer.h"
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
    class TcpBackgroundReceiver : public BackgroudReceiver
	{
	public:
        TcpBackgroundReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
		~TcpBackgroundReceiver() override;

		virtual size_t available();
		virtual bool connected() const;

        TcpClient* tcpClient() const;
        const Endpoint& peerEndpoint() const;
        const Endpoint& endpoint() const;
        int socketId() const;
        
        Device* device() const;
        Channel* channel() const;
        
        const TcpChannelContext* context() const;
        
        virtual void start();
        virtual void stop();
        
        virtual bool processBuffer(const ByteArray& buffer);

	protected:
        DriverManager* manager();
        
    private:
        void createDevice(const Channel* channel);

	protected:
		TcpClient* _client;

		TcpChannelContext* _context;

		Channel* _channel;
		Device* _device;
        DeviceDescription* _dd;
        Device* _originalDevice;

        DriverManager* _manager;
	};

    class TcpAsyncReceiver : public TcpBackgroundReceiver
    {
    public:
        TcpAsyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpAsyncReceiver() override;
        
        void start() override;
        void stop() override;

    private:
        static void tcp_receiveProc(void* parameter);
        void receiveProcInner();
        
        void stopInner();

    private:
        Thread* _receiveThread;
    };

    class TcpSyncReceiver : public TcpBackgroundReceiver
    {
    public:
        TcpSyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpSyncReceiver() override;

        bool processBuffer(const ByteArray& buffer) override;
    };

    class TcpClientAsyncReceiver : public TcpAsyncReceiver
    {
    public:
        TcpClientAsyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpClientAsyncReceiver() override;
    };

    class TcpClientSyncReceiver : public TcpSyncReceiver
    {
    public:
        TcpClientSyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpClientSyncReceiver() override;
        
        bool connected() const override;
            
        bool isTimeout() const;
        
        void start() override;
        void stop() override;
        
        bool processBuffer(const ByteArray& buffer) override;
        
    private:
        uint32_t _startTime;
    };

    class TcpServerAsyncReceiver : public TcpAsyncReceiver
    {
    public:
        TcpServerAsyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpServerAsyncReceiver() override;

        size_t available() override;
        bool connected() const override;
        
        bool isTimeout() const;
        
        void start() override;
        void stop() override;
        
    private:
        uint32_t _startTime;
    };

    class TcpServerSyncReceiver : public TcpSyncReceiver
    {
    public:
        TcpServerSyncReceiver(DriverManager* dm, Channel* channel, TcpClient* client);
        ~TcpServerSyncReceiver() override;
        
        bool connected() const override;
        
        bool isTimeout() const;
        
        void start() override;
        void stop() override;
        
        bool processBuffer(const ByteArray& buffer) override;
        
    private:
        uint32_t _startTime;
    };
    
	typedef PList<TcpBackgroundReceiver> TcpBackgroundReceivers;
}
#endif // TCPBACKGROUNDRECEIVER_H
