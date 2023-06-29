//
//  RcpClient.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef RcpClient_h
#define RcpClient_h

#include "data/String.h"
#include "communication/ClientService.h"
#include "rpc/RpcContext.h"
#include "rpc/RpcInvoker.h"

using namespace Data;
using namespace Communication;

namespace Rpc {
    class RpcClientContext : IEvaluation<RpcClientContext>, IEquatable<RpcClientContext> {
    public:
        Endpoint endpoint;

        explicit RpcClientContext(const Endpoint &endpoint);

        RpcClientContext(const RpcClientContext &other);

        bool equals(const RpcClientContext &other) const override;

        void evaluates(const RpcClientContext &other) override;
    };

    class RpcClient : public RpcSender, public RpcReceiver {
    public:
        using RpcSender::invokeAsync;

        explicit RpcClient(const RpcClientContext &context);

        ~RpcClient() override;

        bool connect(bool asyncOpen = false);

        bool disconnect();

        bool connected() const final;

        const RpcClientContext &context() const;

        const Endpoint &endpoint() const;

        RpcStatus invoke(const RpcMethodContext &context, const IRpcSyncRequestData &request,
                         IRpcSyncResponseData &response) final;

        RpcStatus invokeAsync(const RpcMethodContext &context, const IRpcAsyncRequestData &request,
                              const IRpcAsyncResponseData &response, async_callback action,
                              void *owner) final;

        RpcStatus notify(const RpcMethodContext &context, const IRpcNotifyInfo &info) final;

    protected:
        bool sendSync(const RpcMethodContext &context, const RpcSyncRequest &request, RpcSyncResponse &response,
                      const String &name) final;

        bool sendAsync(const RpcMethodContext &context, const RpcAsyncRequest &request, const String &name) final;

        bool sendAsync(const RpcMethodContext &context, const RpcNotifyInfo &info, const String &name) final;

        bool sendAsync(const Endpoint &peerEndpoint, const RpcAsyncResponse &response, const String &name) final;

        virtual void onServerStatusChanged(bool online);

    private:
        bool initService(bool asyncOpen = false);

        bool unInitService();

    private:
        static InstructionDescription *clientHeartbeatInstruction(void *);

        static void generateInstructions(void *owner, Instructions *instructions);

        static void samplerStatusEventHandler(void *owner, void *sender, EventArgs *args);

    private:
        RpcClientContext _context;
        ClientService *_service;
    };

    typedef PList<RpcClient> RpcClients;
}

#endif /* RcpClient_h */
