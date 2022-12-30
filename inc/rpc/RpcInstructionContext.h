//
//  RpcInstructionContext.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RpcInstructionContext_h
#define RpcInstructionContext_h

#include "data/DateTime.h"
#include "data/TimeZone.h"
#include "data/Map.h"
#include "data/ValueType.h"
#include "data/ByteArray.h"
#include "IO/Stream.h"
#include "communication/BaseCommContext.h"
#include "rpc/RpcContext.h"

using namespace Data;
using namespace Communication;

namespace Rpc {
    class RpcStreamContext {
    public:
        RpcStreamContext();

        virtual ~RpcStreamContext();

        void setStream(Stream *stream) {
            assert(stream);
            _stream = stream;
        }

        Stream *stream() const {
            return _stream;
        }

    protected:
        Stream *_stream;
    };

    class RpcHeartbeatRequest {
    public:
        RpcHeartbeatRequest();

        void write(Stream *stream) const;

        void read(Stream *stream);

        void copyFrom(const RpcHeartbeatRequest *value);
    };

    class RpcHeartbeatResponse {
    public:
        RpcHeartbeatResponse();

        void write(Stream *stream) const;

        void read(Stream *stream);

        void copyFrom(const RpcHeartbeatResponse *value);
    };

    class RpcHeartbeatContext : public ElementContext<RpcHeartbeatRequest, RpcHeartbeatResponse> {
    };

    class RpcCloseRequest {
    public:
        RpcCloseRequest();

        void write(Stream *stream) const;

        void read(Stream *stream);

        void copyFrom(const RpcCloseRequest *value);
    };

    class RpcCloseResponse {
    public:
        RpcCloseResponse();

        void write(Stream *stream) const;

        void read(Stream *stream);

        void copyFrom(const RpcCloseResponse *value);
    };

    class RpcCloseContext : public ElementContext<RpcCloseRequest, RpcCloseResponse> {
    };

    class RpcSyncRequest {
    public:
        RpcSyncRequest(const RpcMethodContext &context = RpcMethodContext::Empty,
                       const IRpcSyncRequestData *data = nullptr);

        virtual ~RpcSyncRequest();

        virtual void write(Stream *stream) const;

        virtual void read(Stream *stream);

        virtual void copyFrom(const RpcSyncRequest *value);

        const RpcMethodContext &methodContext() const;

        IRpcSyncRequestData *data() const;

    private:
        RpcMethodContext _context;
        IRpcSyncRequestData *_data;
    };

    class RpcSyncResponse {
    public:
        RpcSyncResponse(const RpcMethodContext &context = RpcMethodContext::Empty,
                        IRpcSyncResponseData *data = nullptr);

        virtual ~RpcSyncResponse();

        virtual void write(Stream *stream) const;

        virtual void read(Stream *stream);

        virtual void copyFrom(const RpcSyncResponse *value);

        const RpcMethodContext &methodContext() const;

        IRpcSyncResponseData *data() const;

        void setData(IRpcSyncResponseData *data);

    private:
        RpcMethodContext _context;
        IRpcSyncResponseData *_data;
    };

    class RpcSyncContext : public ElementContext<RpcSyncRequest, RpcSyncResponse>, public RpcStreamContext {
    };

    class RpcAsyncRequest {
    public:
        RpcAsyncRequest(const RpcMethodContext &context = RpcMethodContext::Empty,
                        const IRpcAsyncRequestData *data = nullptr);

        virtual ~RpcAsyncRequest();

        virtual void write(Stream *stream) const;

        virtual void read(Stream *stream);

        virtual void copyFrom(const RpcAsyncRequest *value);

        const RpcMethodContext &methodContext() const;

        IRpcAsyncRequestData *data() const;

        Uuid token() const;

    private:
        RpcMethodContext _context;
        IRpcAsyncRequestData *_data;
        Uuid _token;
    };

    class RpcAsyncResponse {
    public:
        RpcAsyncResponse(const RpcMethodContext &context = RpcMethodContext(), const Uuid &token = Uuid::Empty,
                         IRpcAsyncResponseData *data = nullptr);

        virtual ~RpcAsyncResponse();

        virtual void write(Stream *stream) const;

        virtual void read(Stream *stream);

        virtual void copyFrom(const RpcAsyncResponse *value);

        const RpcMethodContext &methodContext() const;

        IRpcAsyncResponseData *data() const;

        void setData(IRpcAsyncResponseData *data);

        Uuid token() const;

    private:
        RpcMethodContext _context;
        IRpcAsyncResponseData *_data;
        Uuid _token;
    };

    class RpcAsyncRequestContext : public ElementAContext<RpcAsyncRequest>, public RpcStreamContext {
    };

    class RpcAsyncResponseContext : public ElementAContext<RpcAsyncResponse>, public RpcStreamContext {
    };

    class RpcNotifyInfo {
    public:
        RpcNotifyInfo(const RpcMethodContext &context = RpcMethodContext(), const IRpcNotifyInfo *info = nullptr);

        virtual ~RpcNotifyInfo();

        virtual void write(Stream *stream) const;

        virtual void read(Stream *stream);

        virtual void copyFrom(const RpcNotifyInfo *value);

        const RpcMethodContext &methodContext() const;

        IRpcNotifyInfo *info() const;

    private:
        RpcMethodContext _context;
        IRpcNotifyInfo *_info;
    };

    class RpcNotifyContext : public ElementAContext<RpcNotifyInfo>, public RpcStreamContext {
    };
}

#endif /* RpcInstructionContext_h */
