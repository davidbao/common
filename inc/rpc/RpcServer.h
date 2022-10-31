//
//  RpcServer.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RpcServer_h
#define RpcServer_h

#include "data/ValueType.h"
#include "communication/ServerService.h"
#include "rpc/RpcContext.h"
#include "rpc/RpcInvoker.h"

using namespace Common;
using namespace Communication;

namespace rpc
{
    class RpcServerContext
    {
    public:
        Endpoint endpoint;
        Secure secure;
        
        RpcServerContext(const Endpoint& endpoint, const Secure& secure = Secure::None);
        RpcServerContext(const RpcServerContext& value);
        
        void operator=(const RpcServerContext& value);
        bool operator==(const RpcServerContext& value) const;
        bool operator!=(const RpcServerContext& value) const;
    };

    class RpcServerEventContainer;
    class IRpcServerEvent
    {
    protected:
        IRpcServerEvent();
        virtual ~IRpcServerEvent();
        
        virtual bool closeClient(const Endpoint& endpoint) = 0;
        
    private:
        friend RpcServerEventContainer;
    };

    class RpcServer : public RpcReceiver, public RpcSender, public IRpcServerEvent
    {
    public:
        RpcServer(const RpcServerContext& context);
        ~RpcServer() override;
        
        bool connected() const override final;
        
        bool start();
        bool stop();
        
    protected:
        bool sendAsync(const Endpoint& peerEndpoint, const RpcAsyncResponse& response, const String& name) override final;
        
        bool sendSync(const RpcMethodContext& context, const RpcSyncRequest& request, RpcSyncResponse& response, const String& name) override final;
        bool sendAsync(const RpcMethodContext& context, const RpcAsyncRequest& request, const String& name) override final;
        bool sendAsync(const RpcMethodContext& context, const RpcNotifyInfo& info, const String& name) override final;
        
        bool closeClient(const Endpoint& endpoint) override final;
        
        virtual void onClientOpened(const Endpoint& endpoint);
        virtual void onClientClosed(const Endpoint& endpoint);
        
    private:
        bool initService();
        bool unInitService();

    private:
        static void generateInstructions(void* owner, Instructions* instructions);
        static void clientCloseEventHandler(void* owner, void* sender, EventArgs* args);
        static void clientAcceptEventHandler(void* owner, void* sender, EventArgs* args);
        
    private:
        RpcServerContext _context;
        ServerService* _service;
    };
}

#endif /* RpcServer_h */
