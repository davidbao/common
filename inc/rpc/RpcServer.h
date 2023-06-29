//
//  RpcServer.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef RpcServer_h
#define RpcServer_h

#include "data/String.h"
#include "communication/ServerService.h"
#include "rpc/RpcContext.h"
#include "rpc/RpcInvoker.h"

using namespace Data;
using namespace Communication;

namespace Rpc {
    class RpcServerContext : IEvaluation<RpcServerContext>, IEquatable<RpcServerContext> {
    public:
        Endpoint endpoint;

        explicit RpcServerContext(const Endpoint &endpoint);

        RpcServerContext(const RpcServerContext &other);

        bool equals(const RpcServerContext &other) const override;

        void evaluates(const RpcServerContext &other) override;
    };

    class RpcServerEventContainer;

    class IRpcServerEvent {
    protected:
        IRpcServerEvent();

        virtual ~IRpcServerEvent();

        virtual bool closeClient(const Endpoint &endpoint) = 0;

    private:
        friend RpcServerEventContainer;
    };

    class RpcServer : public RpcReceiver, public RpcSender, public IRpcServerEvent {
    public:
        explicit RpcServer(const RpcServerContext &context);

        ~RpcServer() override;

        bool connected() const final;

        bool start();

        bool stop();

    protected:
        bool sendAsync(const Endpoint &peerEndpoint, const RpcAsyncResponse &response, const String &name) final;

        bool sendSync(const RpcMethodContext &context, const RpcSyncRequest &request, RpcSyncResponse &response,
                      const String &name) final;

        bool sendAsync(const RpcMethodContext &context, const RpcAsyncRequest &request, const String &name) final;

        bool sendAsync(const RpcMethodContext &context, const RpcNotifyInfo &info, const String &name) final;

        bool closeClient(const Endpoint &endpoint) final;

        virtual void onClientOpened(const Endpoint &endpoint);

        virtual void onClientClosed(const Endpoint &endpoint);

    private:
        bool initService();

        bool unInitService();

    private:
        static void generateInstructions(void *owner, Instructions *instructions);

        static void clientCloseEventHandler(void *owner, void *sender, EventArgs *args);

        static void clientAcceptEventHandler(void *owner, void *sender, EventArgs *args);

    private:
        RpcServerContext _context;
        ServerService *_service;
    };
}

#endif /* RpcServer_h */
