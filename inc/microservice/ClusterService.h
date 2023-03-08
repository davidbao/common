//
//  ClusterService.h
//  common
//
//  Created by baowei on 2020/3/17.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef ClusterService_h
#define ClusterService_h

#include "data/String.h"
#include "rpc/RpcClient.h"
#include "rpc/RpcServer.h"
#include "system/ServiceFactory.h"

using namespace Data;
using namespace Rpc;

namespace Microservice
{
    class ClusterContext
    {
    public:
        String key;
        String value;
        Version version;

        ClusterContext();
        ClusterContext(const String& key, const String& value, const Version& version);
        ClusterContext(const ClusterContext& context);
        
        void write(Stream* stream) const;
        void read(Stream* stream);
        void copyFrom(const ClusterContext* value);
        
        void operator=(const ClusterContext& value);
        bool operator==(const ClusterContext& value) const;
        bool operator!=(const ClusterContext& value) const;
    };
    class ClusterContexts : public IIndexGetter<const ClusterContext&>
    {
    public:
        ClusterContexts();
        ClusterContexts(const ClusterContexts& value);
        ClusterContexts(const ClusterContext** values, size_t count);
        ~ClusterContexts();

        void add(const ClusterContext& value);
        void add(const ClusterContexts& value, off_t offset, size_t count);
        void update(const ClusterContexts& value);
        void update(const ClusterContext& value);
        
        const ClusterContext& at(size_t pos) const override;
        size_t count() const;
        bool contains(const ClusterContext& value) const;
        bool at(const String& key, String& value) const;

        void clear();
        bool remove(const ClusterContext& value);
        bool removeAt(size_t i);

        void operator=(const ClusterContexts& value);
        bool operator==(const ClusterContexts& value) const;
        bool operator!=(const ClusterContexts& value) const;

        void write(Stream* stream) const;
        void read(Stream* stream);
        
        Delegates* valueChangedDelegates();
        
        bool setValue(const String& key, const String& value, Version& version);

    private:
        PList<ClusterContext> _items;
        Map<String, ClusterContext> _map;
        
        Delegates _valueChangedDelegates;
    };

    class PullRequest : public IRpcSyncRequestData
    {
    public:
        String key;
        Version version;
        
        PullRequest();
        PullRequest(const String& key, const Version& version);
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncRequestData* value) override;
        IRpcSyncRequestData* clone() const override;
    };
    class PullResponse : public IRpcSyncResponseData
    {
    public:
        ClusterContext context;
        
        PullResponse();
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncResponseData* value) override;
        IRpcSyncResponseData* clone() const override;
    };

    class PushRequest : public IRpcSyncRequestData
    {
    public:
        ClusterContexts contexts;
        
        PushRequest();
        PushRequest(const ClusterContext& context);
        PushRequest(const ClusterContexts& contexts);
        PushRequest(const ClusterContexts& contexts, off_t offset, size_t count);
        ~PushRequest() override;
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncRequestData* value) override;
        IRpcSyncRequestData* clone() const override;
    };
    class PushResponse : public IRpcSyncResponseData
    {
    public:
        PushResponse();
        ~PushResponse() override;
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncResponseData* value) override;
        IRpcSyncResponseData* clone() const override;
    };

    class PushNotifyRequest : public IRpcSyncRequestData
    {
    public:
        Endpoint endpoint;
        int packetCount;
        
        PushNotifyRequest(const Endpoint& endpoint = Endpoint::Empty, int packetCount = 0);
        ~PushNotifyRequest() override;
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncRequestData* value) override;
        IRpcSyncRequestData* clone() const override;
    };
    class PushNotifyResponse : public IRpcSyncResponseData
    {
    public:
        PushNotifyResponse();
        ~PushNotifyResponse() override;
        
        void write(Stream* stream) const override;
        void read(Stream* stream) override;
        void copyFrom(const IRpcSyncResponseData* value) override;
        IRpcSyncResponseData* clone() const override;
    };

    class ValueChangedEventArgs : public EventArgs
    {
    public:
        ClusterContext oldValue;
        ClusterContext newValue;
        
        ValueChangedEventArgs(const ClusterContext& oldValue, const ClusterContext& newValue);
    };

//    class PushAllHolder : public ThreadHolder::Value
//    {
//    public:
//        Endpoint endpoint;
//
//        PushAllHolder(const Endpoint& endpoint);
//    };

    class ClusterService;
    class ClusterRpcServer : public RpcServer
    {
    public:
        ClusterRpcServer(const RpcServerContext& context, ClusterService* service);
        
    protected:
        void onClientOpened(const Endpoint& endpoint) override;
        void onClientClosed(const Endpoint& endpoint) override;
        
    public:
        static bool onPush(void* owner, const IRpcSyncRequestData* request, IRpcSyncResponseData* response);
        
    private:
        ClusterService* _service;
    };

    class ClusterRpcClient : public RpcClient
    {
    public:
        ClusterRpcClient(const RpcClientContext& context, ClusterService* service);
        
        bool push(const ClusterContexts& contexts);
        
    private:
        static bool onPush(void* owner, const IRpcSyncRequestData* request, IRpcSyncResponseData* response);
        
    protected:
        void onServerStatusChanged(bool online) override;
        
    private:
        ClusterService* _service;
    };

    class IClusterService : public IService
    {
    public:
        virtual bool hasNodes() const = 0;
        
        virtual void setValue(const String& key, const String& value) = 0;
        void addValue(const String& key, const String& value);
        void removeValue(const String& key);
        
        virtual const ClusterContexts& contexts() = 0;
        
        virtual Delegates* valueChangedDelegates() = 0;
    };

    class ClusterService : public IClusterService
    {
    public:
        ClusterService();
        ~ClusterService() override;
        
        bool initialize();
        bool unInitialize();
        
        const Endpoint serverEndpoint() const;
        
        void update(const ClusterContexts& value);
        void update(const ClusterContext& value);
        
        void pushAll(ClusterRpcServer* server, const Endpoint& endpoint);
        
        bool hasNodes() const override final;
        
        void setValue(const String& key, const String& value) override final;
        
        const ClusterContexts& contexts() final;
        
        Delegates* valueChangedDelegates() override final;
        
    private:
        bool pushSync(ClusterRpcServer* server, const Endpoint& endpoint);
        bool pushAllSync(ClusterRpcServer* server, const Endpoint& endpoint, const ClusterContexts& contexts, off_t offset, size_t count);
        
        void pushProc();
        void push(const ClusterContext** values, size_t count);
        
    private:
//        static void pushAllAction(ThreadHolder* holder);

    private:
        Endpoint _current;
        Endpoints _nodes;   // except current.
        
        Mutex _clientsMutex;
        RpcClients _clients;
        
        ClusterRpcServer* _server;
        
        Mutex _contextsMutex;
        ClusterContexts _contexts;
        
        Timer* _sendTimer;
        int _maxPacketCount;
        Mutex _senderContextsMutex;
        LoopPList<ClusterContext> _senderContexts;
    };
}

#endif /* ClusterService_h */
