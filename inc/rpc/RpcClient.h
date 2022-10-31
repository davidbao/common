//
//  RcpClient.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RcpClient_h
#define RcpClient_h

#include "data/ValueType.h"
#include "communication/ClientService.h"
#include "rpc/RpcContext.h"
#include "rpc/RpcInvoker.h"

using namespace Common;
using namespace Communication;

namespace rpc
{
    class RpcClientContext
    {
    public:
        Endpoint endpoint;
        Secure secure;
        
        RpcClientContext(const Endpoint& endpoint, const Secure& secure = Secure::None);
        RpcClientContext(const RpcClientContext& value);
        
        void operator=(const RpcClientContext& value);
        bool operator==(const RpcClientContext& value) const;
        bool operator!=(const RpcClientContext& value) const;
    };

    class RpcClient : public RpcSender, public RpcReceiver
    {
    public:
        RpcClient(const RpcClientContext& context);
        virtual ~RpcClient();
        
        bool connect(bool asyncOpen = false);
        bool disconnect();
        
        bool connected() const override final;
        
        const RpcClientContext& context() const;
        const Endpoint& endpoint() const;
        
        RpcStatus invoke(const RpcMethodContext& context, const IRpcSyncRequestData& request, IRpcSyncResponseData& response) override final;
        RpcStatus invokeAsync(const RpcMethodContext& context, const IRpcAsyncRequestData& request, const IRpcAsyncResponseData& response, async_callback action, void* owner = nullptr) override final;
        RpcStatus notify(const RpcMethodContext& context, const IRpcNotifyInfo& info) override final;
        
    protected:
        bool sendSync(const RpcMethodContext& context, const RpcSyncRequest& request, RpcSyncResponse& response, const String& name) override final;
        bool sendAsync(const RpcMethodContext& context, const RpcAsyncRequest& request, const String& name) override final;
        bool sendAsync(const RpcMethodContext& context, const RpcNotifyInfo& info, const String& name) override final;
        
        bool sendAsync(const Endpoint& peerEndpoint, const RpcAsyncResponse& response, const String& name) override final;
        
        virtual void onServerStatusChanged(bool online);
        
    private:
        bool initService(bool asyncOpen = false);
        bool unInitService();
        
    private:
        static InstructionDescription* clientHeartbeatInstruction(void*);
        static void generateInstructions(void* owner, Instructions* instructions);
        static void samplerStatusEventHandler(void* owner, void* sender, EventArgs* args);
        
    private:
        RpcClientContext _context;
        ClientService* _service;
    };

    typedef Vector<RpcClient> RpcClients;
}

#endif /* RcpClient_h */
