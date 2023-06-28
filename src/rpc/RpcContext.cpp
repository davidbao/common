//
//  RcpContext.cpp
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "rpc/RpcContext.h"
#include "IO/Stream.h"

namespace Rpc {
    RpcStatus::RpcStatus(int status) : _status(status) {
    }

    bool RpcStatus::isOk() const {
        return _status == Ok;
    }

    int RpcStatus::status() const {
        return _status;
    }

    IRpcData::IRpcData() = default;

    IRpcData::~IRpcData() = default;

    const RpcMethodContext RpcMethodContext::Empty = RpcMethodContext();

    RpcMethodContext::RpcMethodContext(const String &name, int tryCount) : RpcMethodContext(name, Endpoint::Empty,
                                                                                            tryCount) {
    }

    RpcMethodContext::RpcMethodContext(const String &name, const Endpoint &endpoint, int tryCount) :
            name(name), endpoint(endpoint), tryCount(tryCount) {
    }

    RpcMethodContext::RpcMethodContext(const RpcMethodContext &context) :
        name(context.name), endpoint(context.endpoint), tryCount(context.tryCount) {
    }

    void RpcMethodContext::write(Stream *stream) const {
        name.write(stream);
        endpoint.write(stream);
        stream->writeInt32(tryCount);
    }

    void RpcMethodContext::read(Stream *stream) {
        name.read(stream);
        endpoint.read(stream);
        tryCount = stream->readInt32();
    }

    void RpcMethodContext::copyFrom(const RpcMethodContext *value) {
        this->name = value->name;
        this->endpoint = value->endpoint;
        this->tryCount = value->tryCount;
    }
}
