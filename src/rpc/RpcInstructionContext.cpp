//
//  RpcInstructionContext.cpp
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#include "rpc/RpcInstructionContext.h"

namespace rpc
{
    RpcStreamContext::RpcStreamContext()
    {
    }
    RpcStreamContext::~RpcStreamContext()
    {
    }

    RpcHeartbeatRequest::RpcHeartbeatRequest()
    {
    }
    void RpcHeartbeatRequest::write(Stream* stream) const
    {
    }
    void RpcHeartbeatRequest::read(Stream* stream)
    {
    }
    void RpcHeartbeatRequest::copyFrom(const RpcHeartbeatRequest* value)
    {
    }
    RpcHeartbeatResponse::RpcHeartbeatResponse()
    {
    }
    void RpcHeartbeatResponse::write(Stream* stream) const
    {
    }
    void RpcHeartbeatResponse::read(Stream* stream)
    {
    }
    void RpcHeartbeatResponse::copyFrom(const RpcHeartbeatResponse* value)
    {
    }

    RpcCloseRequest::RpcCloseRequest()
    {
    }
    void RpcCloseRequest::write(Stream* stream) const
    {
    }
    void RpcCloseRequest::read(Stream* stream)
    {
    }
    void RpcCloseRequest::copyFrom(const RpcCloseRequest* value)
    {
    }
    RpcCloseResponse::RpcCloseResponse()
    {
    }
    void RpcCloseResponse::write(Stream* stream) const
    {
    }
    void RpcCloseResponse::read(Stream* stream)
    {
    }
    void RpcCloseResponse::copyFrom(const RpcCloseResponse* value)
    {
    }

    RpcSyncRequest::RpcSyncRequest(const RpcMethodContext& context, const IRpcSyncRequestData* data)
    {
        _context.copyFrom(&context);
        _data = data != nullptr ? data->clone() : nullptr;
    }
    RpcSyncRequest::~RpcSyncRequest()
    {
        if(_data != nullptr)
            delete _data;
    }
    void RpcSyncRequest::read(Stream* stream)
    {
        _context.read(stream);
    }
    void RpcSyncRequest::write(Stream* stream) const
    {
        _context.write(stream);
        if(_data != nullptr)
            _data->write(stream);
    }
    void RpcSyncRequest::copyFrom(const RpcSyncRequest* value)
    {
        _context.copyFrom(&value->methodContext());
        IRpcSyncRequestData* data = value->_data;
        if(data != nullptr)
        {
            if(_data == nullptr)
                _data = data->clone();
            else
                _data->copyFrom(data);
        }
    }
    const RpcMethodContext& RpcSyncRequest::methodContext() const
    {
        return _context;
    }
    IRpcSyncRequestData* RpcSyncRequest::data() const
    {
        return _data;
    }

    RpcSyncResponse::RpcSyncResponse(const RpcMethodContext& context, IRpcSyncResponseData* data)
    {
        _context.copyFrom(&context);
        _data = data != nullptr ? data->clone() : nullptr;
    }
    RpcSyncResponse::~RpcSyncResponse()
    {
        if(_data != nullptr)
            delete _data;
    }
    void RpcSyncResponse::read(Stream* stream)
    {
        _context.read(stream);
        if(_data != nullptr)
            _data->read(stream);
    }
    void RpcSyncResponse::write(Stream* stream) const
    {
        _context.write(stream);
        if(_data != nullptr)
            _data->write(stream);
    }
    void RpcSyncResponse::copyFrom(const RpcSyncResponse* value)
    {
        _context.copyFrom(&value->methodContext());
        IRpcSyncResponseData* data = value->_data;
        if(data != nullptr)
        {
            if(_data == nullptr)
                _data = data->clone();
            else
                _data->copyFrom(data);
        }
    }
    const RpcMethodContext& RpcSyncResponse::methodContext() const
    {
        return _context;
    }
    IRpcSyncResponseData* RpcSyncResponse::data() const
    {
        return _data;
    }
    void RpcSyncResponse::setData(IRpcSyncResponseData* data)
    {
        if(data != nullptr)
        {
            _data = data->clone();
        }
    }

    RpcAsyncRequest::RpcAsyncRequest(const RpcMethodContext& context, const IRpcAsyncRequestData* data)
    {
        _context.copyFrom(&context);
        _token = Uuid::generate();
        _data = data != nullptr ? data->clone() : nullptr;
    }
    RpcAsyncRequest::~RpcAsyncRequest()
    {
        if(_data != nullptr)
            delete _data;
    }
    void RpcAsyncRequest::read(Stream* stream)
    {
        _context.read(stream);
        _token.read(stream);
    }
    void RpcAsyncRequest::write(Stream* stream) const
    {
        _context.write(stream);
        _token.write(stream);
        if(_data != nullptr)
            _data->write(stream);
    }
    void RpcAsyncRequest::copyFrom(const RpcAsyncRequest* value)
    {
        _context.copyFrom(&value->methodContext());
        _token = value->_token;
        IRpcAsyncRequestData* data = value->_data;
        if(data != nullptr)
        {
            if(_data == nullptr)
                _data = data->clone();
            else
                _data->copyFrom(data);
        }
    }
    const RpcMethodContext& RpcAsyncRequest::methodContext() const
    {
        return _context;
    }
    IRpcAsyncRequestData* RpcAsyncRequest::data() const
    {
        return _data;
    }
    Uuid RpcAsyncRequest::token() const
    {
        return _token;
    }

    RpcAsyncResponse::RpcAsyncResponse(const RpcMethodContext& context, const Uuid& token, IRpcAsyncResponseData* data)
    {
        _context.copyFrom(&context);
        _token = token;
        _data = data != nullptr ? data->clone() : nullptr;
    }
    RpcAsyncResponse::~RpcAsyncResponse()
    {
        if(_data != nullptr)
            delete _data;
    }
    void RpcAsyncResponse::read(Stream* stream)
    {
        _context.read(stream);
        _token.read(stream);
        if(_data != nullptr)
            _data->read(stream);
    }
    void RpcAsyncResponse::write(Stream* stream) const
    {
        _context.write(stream);
        _token.write(stream);
        if(_data != nullptr)
            _data->write(stream);
    }
    void RpcAsyncResponse::copyFrom(const RpcAsyncResponse* value)
    {
        _context.copyFrom(&value->methodContext());
        _token = value->_token;
        IRpcAsyncResponseData* data = value->_data;
        if(data != nullptr)
        {
            if(_data == nullptr)
                _data = data->clone();
            else
                _data->copyFrom(data);
        }
    }
    const RpcMethodContext& RpcAsyncResponse::methodContext() const
    {
        return _context;
    }
    IRpcAsyncResponseData* RpcAsyncResponse::data() const
    {
        return _data;
    }
    void RpcAsyncResponse::setData(IRpcAsyncResponseData* data)
    {
        if(data != nullptr)
        {
            _data = data->clone();
        }
    }
    Uuid RpcAsyncResponse::token() const
    {
        return _token;
    }

    RpcNotifyInfo::RpcNotifyInfo(const RpcMethodContext& context, const IRpcNotifyInfo* info)
    {
        _context.copyFrom(&context);
        _info = info != nullptr ? info->clone() : nullptr;
    }
    RpcNotifyInfo::~RpcNotifyInfo()
    {
        if(_info != nullptr)
            delete _info;
    }
    void RpcNotifyInfo::read(Stream* stream)
    {
        _context.read(stream);
    }
    void RpcNotifyInfo::write(Stream* stream) const
    {
        _context.write(stream);
        if(_info != nullptr)
            _info->write(stream);
    }
    void RpcNotifyInfo::copyFrom(const RpcNotifyInfo* value)
    {
        _context.copyFrom(&value->methodContext());
        IRpcNotifyInfo* info = value->_info;
        if(info != nullptr)
        {
            if(_info == nullptr)
                _info = info->clone();
            else
                _info->copyFrom(info);
        }
    }
    const RpcMethodContext& RpcNotifyInfo::methodContext() const
    {
        return _context;
    }
    IRpcNotifyInfo* RpcNotifyInfo::info() const
    {
        return _info;
    }
}
