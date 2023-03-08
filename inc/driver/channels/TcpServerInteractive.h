#ifndef TCPSERVERINTERACTIVE_H
#define TCPSERVERINTERACTIVE_H

#include "system/Delegate.h"
#include "thread/Thread.h"
#include "thread/Timer.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "net/TcpClient.h"
#include "net/TcpServer.h"
#include "Interactive.h"
#include "Channel.h"
#include "TcpServerChannelContext.h"
#include "TcpBackgroundReceiver.h"
#include "TcpBackgroundSender.h"
#include "driver/devices/InstructionPool.h"

using namespace Data;

namespace Drivers {
    typedef void (*client_accpet_callback)(const Endpoint &);

    typedef void(*client_close_callback)(const Endpoint &);

    class TcpClientEventArgs : public EventArgs {
    public:
        Endpoint endpoint;

        TcpClientEventArgs(const Endpoint &endpoint);

        ~TcpClientEventArgs() override;
    };

    class TcpServerInteractive : public Interactive {
    public:
        class Client {
        public:
            Client(DriverManager *dm, Channel *channel, TcpClient *client);

            ~Client();

            bool connected() const;

            size_t available();

            const Endpoint &peerEndpoint() const;

            const Endpoint &endpoint() const;

            Device *receiverDevice() const;

            Device *senderDevice() const;

            InstructionPool *senderInstructionPool() const;

            TcpClient *tcpClient() const;

            int socketId() const;

            void start();

            bool closeFlag() const;

            void setCloseManually(bool closeFlag = true);

            bool deleteFlag() const;

            void setDeleteManually(bool deleteFlag = true);

            uint32_t deleteStart() const;

            Device::Status senderStatus() const;

            bool isOnline() const;

            TcpServerChannelContext *getChannelContext() const;

            bool processReceivedBuffer(const ByteArray &buffer);

        private:
            TcpBackgroundReceiver *_receiver;
            TcpBackgroundSender *_sender;

            TcpClient *_tcpClient;

            bool _closeFlag;

            bool _deleteFlag;
            uint32_t _deleteStart;
        };

//        typedef PList<Client> Clients;
        class Clients {
        public:
            Clients();

            ~Clients();

            TcpServerInteractive::Client *add(DriverManager *dm, Channel *channel, TcpClient *client);

            void remove(Client *client);

            size_t count();

            void clear();

            bool process(int socketId, int bufferLength);

            bool getUnusedClients(PList<Client> &clients);

            bool containsPeerEndpoint(const Endpoint &peerEndpoint);

            Device *getReceiverDevice(const Endpoint &peerEndpoint);

            Device *getSenderDevice(const Endpoint &peerEndpoint);

            InstructionPool *getClientPool(const Endpoint &endpoint);

            void getClientPools(InstructionPools &ips);

            void getClientPools(const Endpoints &endpoints, InstructionPools &ips);

            void getClientPoolsWithoutClient(const Endpoints &endpoints, InstructionPools &ips);

            Client *getClient(const Endpoint &peerEndpoint);

            bool closeClient(const Endpoint &peerEndpoint);

        private:
            Client *at(int socketId) const;

//            void deleteClients();

        private:
            PList<Client> _clients;
            Mutex _clientsMutex;

//            PList<Client> _deleteClients;
//            Mutex _deleteClientsMutex;
        };

        TcpServerInteractive(DriverManager *dm, Channel *channel);

        ~TcpServerInteractive() override;

        bool open() override;

        void close() override;

        bool connected() override;

        size_t available() override;

        ssize_t send(const uint8_t *buffer, off_t offset, size_t count) override;

        ssize_t receive(uint8_t *buffer, off_t offset, size_t count) override;

        Delegates *acceptDelegates();

        Delegates *closeDelegates();

        const Endpoint &endpoint() const;

        size_t clientCount();

        InstructionPool *getClientPool(const Endpoint &endpoint);

        void getClientPools(InstructionPools &ips);

        void getClientPools(const Endpoints &endpoints, InstructionPools &ips);

        void getClientPoolsWithoutClient(const Endpoints &endpoints, InstructionPools &ips);

        void addDynamicInstructions(const Endpoint &peerEndpoint, Instructions *sendInstructions,
                                    Instructions *recvInstructions);

        void clearDynamicInstructions(const Endpoint &peerEndpoint);

        bool containsPeerEndpoint(const Endpoint &peerEndpoint);

        bool closeClient(const Endpoint &peerEndpoint);

        Client *getClient(const Endpoint &peerEndpoint);

        TcpServerChannelContext *getChannelContext() const;

    protected:
        virtual TcpServer *createServer();

        virtual void updateContext(const TcpServerChannelContext *tcc);

    private:
        void acceptProc();

        void closeProc();

        void multiplexingProc();

        bool rebind();

        Device *getReceiverDevice(const Endpoint &peerEndpoint);

        Device *getSenderDevice(const Endpoint &peerEndpoint);

        void openClient(TcpClient *tc);

        void closeClient(Client *client);

    protected:
        TcpServer *_tcpServer;

    private:
        Mutex _tcpServerMutex;

        Clients _clients;

        Timer *_acceptTimer;
        Timer *_closeTimer;

        Thread *_multiplexingThread;
        bool _multiplexingLoop;

        Delegates _acceptAction;
        Delegates _closeAction;

        int _fd;

        int _exitSockets[2];

//        static const int MaxEventCount = 1000;
    };

#ifndef __EMSCRIPTEN__

    class TcpSSLServerInteractive : public TcpServerInteractive {
    public:
        TcpSSLServerInteractive(DriverManager *dm, Channel *channel);

        ~TcpSSLServerInteractive() override;

    protected:
        TcpServer *createServer() override;

        void updateContext(const TcpServerChannelContext *tcc) override;

    private:
        inline TcpSSLServerChannelContext *getChannelContext() const {
            return (TcpSSLServerChannelContext *) (_channel->description()->context());
        }
    };

    class WebSocketServerInteractive : public TcpServerInteractive {
    public:
        WebSocketServerInteractive(DriverManager *dm, Channel *channel);

        ~WebSocketServerInteractive() override;

    protected:
        TcpServer *createServer() override;
    };

    class WebSocketSSLServerInteractive : public TcpServerInteractive {
    public:
        WebSocketSSLServerInteractive(DriverManager *dm, Channel *channel);

        ~WebSocketSSLServerInteractive() override;

    protected:
        TcpServer *createServer() override;

        void updateContext(const TcpServerChannelContext *tcc) override;

    private:
        inline TcpSSLServerChannelContext *getChannelContext() const {
            return (TcpSSLServerChannelContext *) (_channel->description()->context());
        }
    };

#endif
}

#endif // TCPSERVERINTERACTIVE_H
