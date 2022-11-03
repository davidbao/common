#ifndef TCPINTERACTIVE_H
#define TCPINTERACTIVE_H

#include "Interactive.h"
#include "TcpChannelContext.h"
#include "ChannelDescription.h"
#include "Channel.h"
#include "thread/Timer.h"
#include "net/TcpClient.h"
#include "TcpBackgroundReceiver.h"
#include "TcpBackgroundSender.h"
#include "driver/devices/InstructionPool.h"

namespace Drivers
{
    class TcpMultiPlexingReceiver
    {
    public:
        class Client
        {
        public:
            Client(TcpClient* client, TcpBackgroundReceiver* receiver);
            ~Client();
            
            TcpClient* tcpClient() const;
            int socketId() const;
            
            bool processReceivedBuffer(const ByteArray& buffer);
            
        private:
            TcpClient* _client;
            TcpBackgroundReceiver* _receiver;
        };
        class Clients
        {
        public:
            Clients();
            ~Clients();
            
            void add(TcpClient* client, TcpBackgroundReceiver* receiver);
            void remove(TcpClient* client);
            
            void clear();
            
            bool process(int socketId, int bufferLength);
            
        private:
            Client* at(int socketId) const;
            
        private:
            PList<Client> _clients;
            Mutex _clientsMutex;
        };
        
        TcpMultiPlexingReceiver();
        virtual ~TcpMultiPlexingReceiver();
        
    public:
        static void addClient(TcpClient* client, TcpBackgroundReceiver* receiver);
        static void removeClient(TcpClient* client);
        
        static void open(TcpBackgroundReceiver* receiver);
        static void close();

        static void multiplexingProcInner(void* parameter);
        static void multiplexingProc(const TcpChannelContext* context);
        
    private:
        static int _fd;
        
        static Thread* _multiplexingThread;
        static bool _multiplexingLoop;
        
        static Clients _clients;
        
        static int _exitSockets[2];
        
        static const int MaxEventCount = 100;
    };

	class TcpInteractive : public Interactive, public EthernetEndpoint, public TcpMultiPlexingReceiver
	{
	public:
		TcpInteractive(DriverManager* dm, Channel* channel);
		~TcpInteractive() override;

		bool open() override;
		void close() override;

		bool connected() override;
        size_t available() override;

		ssize_t send(const uint8_t* buffer, off_t offset, size_t count) override;
		ssize_t receive(uint8_t* buffer, off_t offset, size_t count) override;
        
        const Endpoint& peerEndpoint() const override;
        const Endpoint& endpoint() const override;
        
        void addDynamicInstructions(Instructions* sendInstructions, Instructions* recvInstructions);
        void clearDynamicInstructions();
        
        bool isClosing() const override;
        
        TcpClientChannelContext* getChannelContext() const;
        
    protected:
        virtual TcpClient* createClient();

	private:
        bool connectdInner() const;
        
        void updateTcpClient(TcpClient* tcpClient);
        
        Device* getReceiverDevice();
        Device* getSenderDevice();
        
    private:
        friend class TcpBackgroundReceiver;
        friend class TcpBackgroundSender;

	protected:
		TcpClient* _tcpClient;
		bool _autoDelete;
        
        TcpBackgroundReceiver* _receiver;
        bool _multiplexingReceiver;
	};
    
#ifndef __EMSCRIPTEN__
    class TcpSSLInteractive : public TcpInteractive
    {
    public:
        TcpSSLInteractive(DriverManager* dm, Channel* channel);
        ~TcpSSLInteractive() override;
        
        SSLVersion sslVersion() const;
        
    protected:
        TcpClient* createClient() override;
        
    private:
        SSLChannelContext* getChannelContext();
    };
#endif
}
#endif // TCPINTERACTIVE_H
