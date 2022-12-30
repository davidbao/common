//
//  RpcInstruction.h
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef RpcInstruction_h
#define RpcInstruction_h

#include "communication/ClientInstruction.h"
#include "communication/ServerInstruction.h"
#include "rpc/RpcInstructionContext.h"
#include "rpc/RpcServer.h"
#include "rpc/RpcClient.h"

using namespace Data;

namespace Rpc {
    class RpcServerEventContainer {
    public:
        RpcServerEventContainer(IRpcServerEvent *receiver);

        virtual ~RpcServerEventContainer();

    protected:
        bool onCloseClient(const Endpoint &endpoint);

    private:
        IRpcServerEvent *_receiver;
    };

    class RpcReceiverEventContainer {
    public:
        RpcReceiverEventContainer(IRpcReceiverEvent *receiver);

        virtual ~RpcReceiverEventContainer();

    protected:
        RpcSyncContext *onSyncSetValue(RpcSyncContext *context);

        RpcAsyncRequestContext *onAsyncRequestSetValue(RpcAsyncRequestContext *context);

        RpcNotifyContext *onNotifySetValue(RpcNotifyContext *context);

    private:
        IRpcReceiverEvent *_receiver;
    };

    class RpcSenderEventContainer {
    public:
        RpcSenderEventContainer(IRpcSenderEvent *receiver);

        virtual ~RpcSenderEventContainer();

    protected:
        RpcAsyncResponseContext *onAsyncResponseSetValue(RpcAsyncResponseContext *context);

    private:
        IRpcSenderEvent *_receiver;
    };

    class HeartbeatInstruction : public ElementInstruction<RpcHeartbeatRequest, RpcHeartbeatResponse> {
    public:
        HeartbeatInstruction(InstructionDescription *id);

        ~HeartbeatInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerHeartbeatInstruction : public ServerElementInstruction<RpcHeartbeatRequest, RpcHeartbeatResponse> {
    public:
        ServerHeartbeatInstruction(InstructionDescription *id);

        ~ServerHeartbeatInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class CloseInstruction : public ElementInstruction<RpcCloseRequest, RpcCloseResponse> {
    public:
        CloseInstruction(InstructionDescription *id);

        ~CloseInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerCloseInstruction
            : public ServerElementInstruction<RpcCloseRequest, RpcCloseResponse>, public RpcServerEventContainer {
    public:
        ServerCloseInstruction(InstructionDescription *id, IRpcServerEvent *receiver);

        ~ServerCloseInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;

        ClientContext *setValue(ClientContext *context) override;
    };

    class RpcSyncInstruction : public ElementInstruction<RpcSyncRequest, RpcSyncResponse> {
    public:
        RpcSyncInstruction(InstructionDescription *id);

        ~RpcSyncInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerRpcSyncInstruction
            : public ServerElementInstruction<RpcSyncRequest, RpcSyncResponse>, public RpcReceiverEventContainer {
    public:
        ServerRpcSyncInstruction(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcSyncInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override;
    };

    class RpcAsyncRequestInstruction : public ElementAInstruction<RpcAsyncRequest> {
    public:
        RpcAsyncRequestInstruction(InstructionDescription *id);

        ~RpcAsyncRequestInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerRpcAsyncRequestInstruction
            : public ServerElementAInstruction<RpcAsyncRequest>, public RpcReceiverEventContainer {
    public:
        ServerRpcAsyncRequestInstruction(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcAsyncRequestInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override;
    };

    class RpcAsyncResponseInstruction : public ElementAInstruction<RpcAsyncResponse> {
    public:
        RpcAsyncResponseInstruction(InstructionDescription *id);

        ~RpcAsyncResponseInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerRpcAsyncResponseInstruction
            : public ServerElementAInstruction<RpcAsyncResponse>, public RpcSenderEventContainer {
    public:
        ServerRpcAsyncResponseInstruction(InstructionDescription *id, IRpcSenderEvent *receiver);

        ~ServerRpcAsyncResponseInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override;
    };

    class RpcNotifyInstruction : public ElementAInstruction<RpcNotifyInfo> {
    public:
        RpcNotifyInstruction(InstructionDescription *id);

        ~RpcNotifyInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;
    };

    class ServerRpcNotifyInstruction
            : public ServerElementAInstruction<RpcNotifyInfo>, public RpcReceiverEventContainer {
    public:
        ServerRpcNotifyInstruction(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcNotifyInstruction() override;

        uint8_t command() const override;

        bool allowLogMessage() const override;

        bool setCommandBuffer(MemoryStream &ms, ClientContext *context) override;
    };

    class RpcSyncInstruction2 : public RpcSyncInstruction {
    public:
        RpcSyncInstruction2(InstructionDescription *id);

        ~RpcSyncInstruction2() override;

        uint8_t command() const override;
    };

    class ServerRpcSyncInstruction2 : public ServerRpcSyncInstruction {
    public:
        ServerRpcSyncInstruction2(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcSyncInstruction2() override;

        uint8_t command() const override;
    };

    class RpcAsyncRequestInstruction2 : public RpcAsyncRequestInstruction {
    public:
        RpcAsyncRequestInstruction2(InstructionDescription *id);

        ~RpcAsyncRequestInstruction2() override;

        uint8_t command() const override;
    };

    class ServerRpcAsyncRequestInstruction2 : public ServerRpcAsyncRequestInstruction {
    public:
        ServerRpcAsyncRequestInstruction2(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcAsyncRequestInstruction2() override;

        uint8_t command() const override;
    };

    class RpcAsyncResponseInstruction2 : public RpcAsyncResponseInstruction {
    public:
        RpcAsyncResponseInstruction2(InstructionDescription *id);

        ~RpcAsyncResponseInstruction2() override;

        uint8_t command() const override;
    };

    class ServerRpcAsyncResponseInstruction2 : public ServerRpcAsyncResponseInstruction {
    public:
        ServerRpcAsyncResponseInstruction2(InstructionDescription *id, IRpcSenderEvent *receiver);

        ~ServerRpcAsyncResponseInstruction2() override;

        uint8_t command() const override;
    };

    class RpcNotifyInstruction2 : public RpcNotifyInstruction {
    public:
        RpcNotifyInstruction2(InstructionDescription *id);

        ~RpcNotifyInstruction2() override;

        uint8_t command() const override;
    };

    class ServerRpcNotifyInstruction2 : public ServerRpcNotifyInstruction {
    public:
        ServerRpcNotifyInstruction2(InstructionDescription *id, IRpcReceiverEvent *receiver);

        ~ServerRpcNotifyInstruction2() override;

        uint8_t command() const override;
    };
}

#endif /* RpcInstruction_h */
