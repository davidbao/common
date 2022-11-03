#ifndef TCPBACKGROUNDSENDER_H
#define TCPBACKGROUNDSENDER_H

#include "Interactive.h"
#include "Channel.h"
#include "TcpServerChannelContext.h"
#include "net/TcpClient.h"
#include "net/TcpServer.h"
#include "data/LoopArray.h"
#include "thread/Thread.h"
#include "thread/TickTimeout.h"
#include "thread/Locker.h"
#include "data/TimeSpan.h"
#include "../devices/InstructionPool.h"

using namespace Common;

namespace Drivers
{
    class TcpBackgroundSender;
    class SenderPool : public InstructionPool
    {
    public:
        SenderPool(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd);
        ~SenderPool() override;
        
        virtual Device::Status getConnectStatus() const;
        bool isOnline() const;
        bool isOffline() const;
        void setConnectedError(bool showError = false);
        
        Delegates* statusChangedDelegates();
        
        void reset() override;

    protected:
        // unit: ms
        int detectionCount() const;
        void setConnectStatus(Device::Status status);
        Device::Status connectStatus() const;
        
        virtual bool reopen();
        
    protected:
        void errorHandle(const DeviceDescription* dd, const InstructionDescription* id, bool error) override;
        
    private:
        friend TcpBackgroundSender;
        
        Device::Status _connected;
        
        int _checkFailedCount;
        
        bool _showConnectedError;
        
        Delegates _statusChangedAction;
    };

    class SenderMultiPool : public SenderPool
    {
    public:
        SenderMultiPool(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd);
        ~SenderMultiPool() override;
    };

    class SenderSinglePool : public SenderPool
    {
    public:
        SenderSinglePool(DriverManager* dm, ChannelDescription* cd, DeviceDescription* dd);
        ~SenderSinglePool() override;
        
        void start(Channel* channel, Device* device) override;
        void stop() override;
        
    public:
        static void startSinglePool();
        static void stopSinglePool();
        
    protected:
        bool reopen() override;
        
        InstructionContext* executeInstruction(InstructionDescription* id) override;
        
    private:
        static void instructionSingleProc(void* parameter);
        
    private:
        static Thread* _singleThread;
        
        static Mutex _poolsMutex;
        static PList<SenderSinglePool> _pools;
    };
    
    class Device;
    class Channel;
    class DriverManager;
    class InstructionPool;
    class DeviceDescription;
    class TcpBackgroundSender
    {
    public:
        TcpBackgroundSender(DriverManager* dm, Channel* channel, TcpClient* client, TcpBackgroundReceiver* receiver);
        virtual ~TcpBackgroundSender();

        InstructionPool* instructionPool() const;
        
        Device* device() const;
        
        Channel* channel() const;
        
        bool connected() const;
        
        void setSenderStatus(Device::Status status = Device::Status::Online);
        Device::Status senderStatus() const;
        
    protected:
        virtual void createDevice(const Channel* channel);
        
        DriverManager* manager();
        
    protected:
        TcpClient* _client;
        
        TcpChannelContext* _context;
        
        Channel* _channel;
        Device* _device;
        DeviceDescription* _dd;
        
        SenderPool* _pool;
        
        DriverManager* _manager;
        
        TcpBackgroundReceiver* _receiver;
    };

    class TcpServerAsyncSender : public TcpBackgroundSender
    {
    public:
        TcpServerAsyncSender(DriverManager* dm, Channel* channel, TcpClient* client, TcpBackgroundReceiver* receiver);
        
    protected:
        void createDevice(const Channel* channel) override;
    };

    class TcpServerSyncSender : public TcpBackgroundSender
    {
    public:
        TcpServerSyncSender(DriverManager* dm, Channel* channel, TcpClient* client, TcpBackgroundReceiver* receiver);
        
    protected:
        void createDevice(const Channel* channel) override;
    };
    
    typedef PList<TcpBackgroundSender> TcpBackgroundSenders;
}

#endif // TCPBACKGROUNDSENDER_H
