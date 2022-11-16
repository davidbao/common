//
//  RpcContext.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RpcContext_h
#define RpcContext_h

#include "data/ValueType.h"
#include "data/Dictionary.h"
#include "net/NetType.h"

using namespace Net;

namespace rpc
{
    struct RpcStatus
    {
    public:
        RpcStatus(int status = Ok);
        
        bool isOk() const;
        
        int status() const;

    public:
        static const int Ok = 0;
        static const int CommError = 1;
        static const int Disconnected = 2;
        
    private:
        int _status;
    };

    class IRpcData
    {
    public:
        IRpcData();
        virtual ~IRpcData();
        
        virtual void write(Stream* stream) const = 0;
        virtual void read(Stream* stream) = 0;
        virtual void copyFrom(const IRpcData* value) = 0;
        virtual IRpcData* clone() const = 0;
    };
    typedef IRpcData IRpcSyncRequestData;
    typedef IRpcData IRpcSyncResponseData;
    typedef IRpcData IRpcAsyncRequestData;
    typedef IRpcData IRpcAsyncResponseData;
    typedef IRpcData IRpcNotifyInfo;

    class RpcMethodContext
    {
    public:
        String name;
        Endpoint endpoint;
        int tryCount;
        
        RpcMethodContext(const String& name = String::Empty, int tryCount = -1);
        RpcMethodContext(const String& name, const Endpoint& endpoint, int tryCount = -1);
        RpcMethodContext(const RpcMethodContext& context);
        
        void write(Stream* stream) const;
        void read(Stream* stream);
        void copyFrom(const RpcMethodContext* value);
        
    public:
        static const RpcMethodContext Empty;
    };

    typedef bool (*sync_callback)(void*, const IRpcSyncRequestData*, IRpcSyncResponseData*);
    typedef bool (*async_callback)(void*, const IRpcAsyncRequestData*, IRpcAsyncResponseData*);
    typedef void (*notify_callback)(void*, const IRpcNotifyInfo*);
}

#endif /* RpcContext_h */
