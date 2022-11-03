//
//  RpcInvoker.h
//  common
//
//  Created by baowei on 2020/3/19.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RpcInvoker_h
#define RpcInvoker_h

#include "data/ValueType.h"
#include "rpc/RpcContext.h"
#include "rpc/RpcInstructionContext.h"

using namespace Common;
using namespace Communication;

namespace rpc
{
    class RpcAsyncElement
    {
    public:
        Uuid token;
        IRpcAsyncRequestData* request;
        IRpcAsyncResponseData* response;
        async_callback action;
        void* owner;

        RpcAsyncElement(const Uuid& token, const IRpcAsyncRequestData* request, const IRpcAsyncResponseData* response, async_callback action, void* owner = nullptr);
        ~RpcAsyncElement();
    };
    typedef Map<Uuid, RpcAsyncElement> RpcAsyncElements;

    class RpcMethod
    {
    public:
        String name;
        IRpcData* request;

        RpcMethod(const String& name, const IRpcData& request);
        virtual ~RpcMethod();
    };
    class RpcSyncMethod : public RpcMethod
    {
    public:
        IRpcSyncResponseData* response;
        sync_callback action;
        void* owner;

        RpcSyncMethod(const String& name, const IRpcSyncRequestData& request, const IRpcSyncResponseData& response, sync_callback action, void* owner = nullptr);
        ~RpcSyncMethod() override;
    };
    class RpcAsyncMethod : public RpcMethod
    {
    public:
        IRpcAsyncResponseData* response;
        async_callback action;
        void* owner;

        RpcAsyncMethod(const String& name, const IRpcAsyncRequestData& request, const IRpcAsyncResponseData& response, async_callback action, void* owner = nullptr);
        ~RpcAsyncMethod() override;
    };
    class RpcNotifyMethod : public RpcMethod
    {
    public:
        notify_callback action;
        void* owner;

        RpcNotifyMethod(const String& name, const IRpcNotifyInfo& request, notify_callback action, void* owner = nullptr);
        ~RpcNotifyMethod() override;
    };
    class RpcMethods
    {
    public:
        RpcMethods();
        ~RpcMethods();

        void add(RpcMethod* method);

        bool at(const String& methodName, RpcMethod*& method) const;

        bool contains(const String& methodName) const;

    private:
        PList<RpcMethod> _methods;
    };

    class RpcSenderEventContainer;
    class IRpcSenderEvent
    {
    public:
        IRpcSenderEvent();
        virtual ~IRpcSenderEvent();

    protected:
        virtual bool onAsyncResponseSetValue(const RpcMethodContext& context, Stream* stream, const Uuid& token) = 0;

    private:
        friend RpcSenderEventContainer;
    };

    class RpcSender : public IRpcSenderEvent
    {
    public:
        RpcSender();
        ~RpcSender() override;

        virtual bool connected() const = 0;

        virtual RpcStatus invoke(const RpcMethodContext& context, const IRpcSyncRequestData& request, IRpcSyncResponseData& response);
        virtual RpcStatus invokeAsync(const RpcMethodContext& context, const IRpcAsyncRequestData& request, const IRpcAsyncResponseData& response, async_callback action, void* owner = nullptr);
        virtual RpcStatus notify(const RpcMethodContext& context, const IRpcNotifyInfo& info);

    protected:
        virtual bool sendSync(const RpcMethodContext& context, const RpcSyncRequest& request, RpcSyncResponse& response, const String& name) = 0;
        virtual bool sendAsync(const RpcMethodContext& context, const RpcAsyncRequest& request, const String& name) = 0;
        virtual bool sendAsync(const RpcMethodContext& context, const RpcNotifyInfo& info, const String& name) = 0;

        bool onAsyncResponseSetValue(const RpcMethodContext& context, Stream* stream, const Uuid& token) override final;

    private:
        Mutex _asyncElementsMutex;
        RpcAsyncElements _asyncElements;
    };

    class RpcReceiverEventContainer;
    class IRpcReceiverEvent
    {
    public:
        IRpcReceiverEvent();
        virtual ~IRpcReceiverEvent();

    protected:
        virtual bool onSyncSetValue(const RpcMethodContext& context, Stream* stream, IRpcSyncRequestData* request, IRpcSyncResponseData*& response) = 0;
        virtual bool onAsyncRequestSetValue(const RpcMethodContext& context, Stream* stream, const Uuid& token, const Endpoint& peerEndpoint) = 0;
        virtual bool onNotifySetValue(const RpcMethodContext& context, Stream* stream, IRpcNotifyInfo* request) = 0;

    private:
        friend RpcReceiverEventContainer;
    };

    class RpcReceiver : public IRpcReceiverEvent
    {
    public:
        RpcReceiver();
        ~RpcReceiver() override;

        virtual bool connected() const = 0;

        void registerMethod(RpcMethod* method);

    protected:
        virtual bool sendAsync(const Endpoint& peerEndpoint, const RpcAsyncResponse& response, const String& name) = 0;

        bool onSyncSetValue(const RpcMethodContext& context, Stream* stream, IRpcSyncRequestData* request, IRpcSyncResponseData*& response) override final;
        bool onAsyncRequestSetValue(const RpcMethodContext& context, Stream* stream, const Uuid& token, const Endpoint& peerEndpoint) override final;
        bool onNotifySetValue(const RpcMethodContext& context, Stream* stream, IRpcNotifyInfo* info) override final;

    private:
        Mutex _methodsMutex;
        RpcMethods _methods;
    };
}

#endif /* RpcInvoker_h */
