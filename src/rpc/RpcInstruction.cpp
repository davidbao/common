//
//  RpcInstruction.cpp
//  common
//
//  Created by baowei on 2020/2/21.
//  Copyright © 2020 com. All rights reserved.
//

#include "RpcInstruction.h"
#include "data/Dictionary.h"

namespace rpc
{
    RpcServerEventContainer::RpcServerEventContainer(IRpcServerEvent* receiver)
    {
        assert(receiver);
        _receiver = receiver;
    }
    RpcServerEventContainer::~RpcServerEventContainer()
    {
        _receiver = nullptr;
    }

    bool RpcServerEventContainer::onCloseClient(const Endpoint& endpoint)
    {
        return _receiver->closeClient(endpoint);
    }

    RpcReceiverEventContainer::RpcReceiverEventContainer(IRpcReceiverEvent* receiver)
    {
        assert(receiver);
        _receiver = receiver;
    }
    RpcReceiverEventContainer::~RpcReceiverEventContainer()
    {
        _receiver = nullptr;
    }

    RpcSyncContext* RpcReceiverEventContainer::onSyncSetValue(RpcSyncContext* context)
    {
        if (context != nullptr)
        {
            RpcSyncRequest* request = context->inputData();
            RpcSyncResponse* reponse = context->outputData();
            Stream* stream = context->stream();
            IRpcSyncResponseData* data = nullptr;
            _receiver->onSyncSetValue(request->methodContext(), stream, request->data(), data);
            if(data != nullptr)
            {
                reponse->setData(data);
                delete data;
            }
        }
        return context;
    }
    RpcAsyncRequestContext* RpcReceiverEventContainer::onAsyncRequestSetValue(RpcAsyncRequestContext* context)
    {
        if (context != nullptr)
        {
            RpcAsyncRequest* request = context->inputData();
            Stream* stream = context->stream();
            _receiver->onAsyncRequestSetValue(request->methodContext(), stream, request->token(), context->peerEndpoint());
        }
        return context;
    }
    RpcNotifyContext* RpcReceiverEventContainer::onNotifySetValue(RpcNotifyContext* context)
    {
        if (context != nullptr)
        {
            RpcNotifyInfo* request = context->inputData();
            Stream* stream = context->stream();
            _receiver->onNotifySetValue(request->methodContext(), stream, request->info());
        }
        return context;
    }

    RpcSenderEventContainer::RpcSenderEventContainer(IRpcSenderEvent* receiver)
    {
        assert(receiver);
        _receiver = receiver;
    }
    RpcSenderEventContainer::~RpcSenderEventContainer()
    {
        _receiver = nullptr;
    }

    RpcAsyncResponseContext* RpcSenderEventContainer::onAsyncResponseSetValue(RpcAsyncResponseContext* context)
    {
        if (context != nullptr)
        {
            RpcAsyncResponse* response = context->inputData();
            Stream* stream = context->stream();
            _receiver->onAsyncResponseSetValue(response->methodContext(), stream, response->token());
        }
        return context;
    }

    // heartbeat
    HeartbeatInstruction::HeartbeatInstruction(InstructionDescription* id) : ElementInstruction<RpcHeartbeatRequest, RpcHeartbeatResponse>(id)
    {
    }
    HeartbeatInstruction::~HeartbeatInstruction()
    {
    }
    uint8_t HeartbeatInstruction::command() const
    {
        return 0x01;
    }
    
    bool HeartbeatInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerHeartbeatInstruction::ServerHeartbeatInstruction(InstructionDescription* id) : ServerElementInstruction<RpcHeartbeatRequest, RpcHeartbeatResponse>(id)
    {
    }
    ServerHeartbeatInstruction::~ServerHeartbeatInstruction()
    {
    }
    uint8_t ServerHeartbeatInstruction::command() const
    {
        return 0x01;
    }
    
    bool ServerHeartbeatInstruction::allowLogMessage() const
    {
        return false;
    }

    // close
    CloseInstruction::CloseInstruction(InstructionDescription* id) : ElementInstruction<RpcCloseRequest, RpcCloseResponse>(id)
    {
    }
    CloseInstruction::~CloseInstruction()
    {
    }
    uint8_t CloseInstruction::command() const
    {
        return 0x02;
    }
    
    bool CloseInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerCloseInstruction::ServerCloseInstruction(InstructionDescription* id, IRpcServerEvent* receiver) : ServerElementInstruction<RpcCloseRequest, RpcCloseResponse>(id),  RpcServerEventContainer(receiver)
    {
    }
    ServerCloseInstruction::~ServerCloseInstruction()
    {
    }
    uint8_t ServerCloseInstruction::command() const
    {
        return 0x02;
    }
    
    bool ServerCloseInstruction::allowLogMessage() const
    {
        return false;
    }
    ClientContext* ServerCloseInstruction::setValue(ClientContext* context)
    {
        RpcCloseContext* rcontext = dynamic_cast<RpcCloseContext*>(context);
        if (rcontext != nullptr)
        {
            // close the client.
            onCloseClient(rcontext->peerEndpoint());
        }
        return context;
    }

    // sync
    RpcSyncInstruction::RpcSyncInstruction(InstructionDescription* id) : ElementInstruction<RpcSyncRequest, RpcSyncResponse>(id)
    {
    }
    RpcSyncInstruction::~RpcSyncInstruction()
    {
    }
    uint8_t RpcSyncInstruction::command() const
    {
        return 0x10;
    }
    
    bool RpcSyncInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerRpcSyncInstruction::ServerRpcSyncInstruction(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerElementInstruction<RpcSyncRequest, RpcSyncResponse>(id), RpcReceiverEventContainer(receiver)
    {
    }
    ServerRpcSyncInstruction::~ServerRpcSyncInstruction()
    {
    }
    
    uint8_t ServerRpcSyncInstruction::command() const
    {
        return 0x10;
    }
    
    bool ServerRpcSyncInstruction::allowLogMessage() const
    {
        return false;
    }
    
    bool ServerRpcSyncInstruction::setCommandBuffer(MemoryStream& ms, ClientContext* context)
    {
        RpcSyncContext* rcontext = dynamic_cast<RpcSyncContext*>(context);
        if (rcontext != nullptr)
        {
            ms.readByte();    // skip command
            readVersion(&ms);
            
            rcontext->inputData()->read(&ms);
            rcontext->setStream(&ms);
            
            return onSyncSetValue(rcontext);
        }
        return false;
    }

    // async request
    RpcAsyncRequestInstruction::RpcAsyncRequestInstruction(InstructionDescription* id) : ElementAInstruction<RpcAsyncRequest>(id)
    {
    }
    RpcAsyncRequestInstruction::~RpcAsyncRequestInstruction()
    {
    }
    uint8_t RpcAsyncRequestInstruction::command() const
    {
        return 0x11;
    }
    
    bool RpcAsyncRequestInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerRpcAsyncRequestInstruction::ServerRpcAsyncRequestInstruction(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerElementAInstruction<RpcAsyncRequest>(id), RpcReceiverEventContainer(receiver)
    {
    }
    ServerRpcAsyncRequestInstruction::~ServerRpcAsyncRequestInstruction()
    {
    }
    
    uint8_t ServerRpcAsyncRequestInstruction::command() const
    {
        return 0x11;
    }
    
    bool ServerRpcAsyncRequestInstruction::allowLogMessage() const
    {
        return false;
    }
    
    bool ServerRpcAsyncRequestInstruction::setCommandBuffer(MemoryStream& ms, ClientContext* context)
    {
        RpcAsyncRequestContext* rcontext = dynamic_cast<RpcAsyncRequestContext*>(context);
        if (rcontext != nullptr)
        {
            ms.readByte();    // skip command
            readVersion(&ms);
            
            rcontext->inputData()->read(&ms);
            rcontext->setStream(&ms);
            
            return onAsyncRequestSetValue(rcontext);
        }
        return false;
    }

    // async response
    RpcAsyncResponseInstruction::RpcAsyncResponseInstruction(InstructionDescription* id) : ElementAInstruction<RpcAsyncResponse>(id)
    {
    }
    RpcAsyncResponseInstruction::~RpcAsyncResponseInstruction()
    {
    }
    uint8_t RpcAsyncResponseInstruction::command() const
    {
        return 0x12;
    }
    
    bool RpcAsyncResponseInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerRpcAsyncResponseInstruction::ServerRpcAsyncResponseInstruction(InstructionDescription* id, IRpcSenderEvent* receiver) : ServerElementAInstruction<RpcAsyncResponse>(id), RpcSenderEventContainer(receiver)
    {
    }
    ServerRpcAsyncResponseInstruction::~ServerRpcAsyncResponseInstruction()
    {
    }
    
    uint8_t ServerRpcAsyncResponseInstruction::command() const
    {
        return 0x12;
    }
    
    bool ServerRpcAsyncResponseInstruction::allowLogMessage() const
    {
        return false;
    }
    
    bool ServerRpcAsyncResponseInstruction::setCommandBuffer(MemoryStream& ms, ClientContext* context)
    {
        RpcAsyncResponseContext* rcontext = dynamic_cast<RpcAsyncResponseContext*>(context);
        if (rcontext != nullptr)
        {
            ms.readByte();    // skip command
            readVersion(&ms);
            
            rcontext->inputData()->read(&ms);
            rcontext->setStream(&ms);
            
            return onAsyncResponseSetValue(rcontext);
        }
        return false;
    }

    // notify
    RpcNotifyInstruction::RpcNotifyInstruction(InstructionDescription* id) : ElementAInstruction<RpcNotifyInfo>(id)
    {
    }
    RpcNotifyInstruction::~RpcNotifyInstruction()
    {
    }
    uint8_t RpcNotifyInstruction::command() const
    {
        return 0x13;
    }
    
    bool RpcNotifyInstruction::allowLogMessage() const
    {
        return false;
    }

    ServerRpcNotifyInstruction::ServerRpcNotifyInstruction(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerElementAInstruction<RpcNotifyInfo>(id), RpcReceiverEventContainer(receiver)
    {
    }
    ServerRpcNotifyInstruction::~ServerRpcNotifyInstruction()
    {
    }
    
    uint8_t ServerRpcNotifyInstruction::command() const
    {
        return 0x13;
    }
    
    bool ServerRpcNotifyInstruction::allowLogMessage() const
    {
        return false;
    }
    
    bool ServerRpcNotifyInstruction::setCommandBuffer(MemoryStream& ms, ClientContext* context)
    {
        RpcNotifyContext* rcontext = dynamic_cast<RpcNotifyContext*>(context);
        if (rcontext != nullptr)
        {
            ms.readByte();    // skip command
            readVersion(&ms);

            rcontext->inputData()->read(&ms);
            rcontext->setStream(&ms);

            return onNotifySetValue(rcontext);
        }
        return false;
    }

    RpcSyncInstruction2::RpcSyncInstruction2(InstructionDescription* id) : RpcSyncInstruction(id)
    {
    }
    RpcSyncInstruction2::~RpcSyncInstruction2()
    {
    }
    uint8_t RpcSyncInstruction2::command() const
    {
        return 0x15;
    }

    ServerRpcSyncInstruction2::ServerRpcSyncInstruction2(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerRpcSyncInstruction(id, receiver)
    {
    }
    ServerRpcSyncInstruction2::~ServerRpcSyncInstruction2()
    {
    }
    
    uint8_t ServerRpcSyncInstruction2::command() const
    {
        return 0x15;
    }

    RpcAsyncRequestInstruction2::RpcAsyncRequestInstruction2(InstructionDescription* id) : RpcAsyncRequestInstruction(id)
    {
    }
    RpcAsyncRequestInstruction2::~RpcAsyncRequestInstruction2()
    {
    }

    uint8_t RpcAsyncRequestInstruction2::command() const
    {
        return 0x16;
    }
    
    ServerRpcAsyncRequestInstruction2::ServerRpcAsyncRequestInstruction2(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerRpcAsyncRequestInstruction(id, receiver)
    {
    }
    ServerRpcAsyncRequestInstruction2::~ServerRpcAsyncRequestInstruction2()
    {
    }
    
    uint8_t ServerRpcAsyncRequestInstruction2::command() const
    {
        return 0x16;
    }

    RpcAsyncResponseInstruction2::RpcAsyncResponseInstruction2(InstructionDescription* id) : RpcAsyncResponseInstruction(id)
    {
    }
    RpcAsyncResponseInstruction2::~RpcAsyncResponseInstruction2()
    {
    }

    uint8_t RpcAsyncResponseInstruction2::command() const
    {
        return 0x17;
    }

    ServerRpcAsyncResponseInstruction2::ServerRpcAsyncResponseInstruction2(InstructionDescription* id, IRpcSenderEvent* receiver) : ServerRpcAsyncResponseInstruction(id, receiver)
    {
    }
    ServerRpcAsyncResponseInstruction2::~ServerRpcAsyncResponseInstruction2()
    {
    }
    
    uint8_t ServerRpcAsyncResponseInstruction2::command() const
    {
        return 0x17;
    }

    RpcNotifyInstruction2::RpcNotifyInstruction2(InstructionDescription* id) : RpcNotifyInstruction(id)
    {
    }
    RpcNotifyInstruction2::~RpcNotifyInstruction2()
    {
    }

    uint8_t RpcNotifyInstruction2::command() const
    {
        return 0x18;
    }

    ServerRpcNotifyInstruction2::ServerRpcNotifyInstruction2(InstructionDescription* id, IRpcReceiverEvent* receiver) : ServerRpcNotifyInstruction(id, receiver)
    {
    }
    ServerRpcNotifyInstruction2::~ServerRpcNotifyInstruction2()
    {
    }
    
    uint8_t ServerRpcNotifyInstruction2::command() const
    {
        return 0x18;
    }
}
