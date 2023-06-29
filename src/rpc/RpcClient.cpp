//
//  RpcClient.cpp
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "rpc/RpcClient.h"
#include "rpc/RpcContext.h"
#include "RpcInstruction.h"
#include "rpc/RpcInstructionContext.h"
#include "communication/ClientService.h"

namespace Rpc {
    RpcClientContext::RpcClientContext(const Endpoint &endpoint) : endpoint(endpoint) {
    }

    RpcClientContext::RpcClientContext(const RpcClientContext &other) : RpcClientContext(other.endpoint) {
    }

    bool RpcClientContext::equals(const RpcClientContext &other) const {
        return this->endpoint == other.endpoint;
    }

    void RpcClientContext::evaluates(const RpcClientContext &other) {
        this->endpoint = other.endpoint;
    }

    RpcClient::RpcClient(const RpcClientContext &context) : _context(context), _service(nullptr) {
    }

    RpcClient::~RpcClient() {
        unInitService();
    }

    bool RpcClient::connect(bool asyncOpen) {
        return initService(asyncOpen);
    }

    bool RpcClient::disconnect() {
        return unInitService();
    }

    bool RpcClient::connected() const {
        if (_service != nullptr)
            return _service->connected();
        return false;
    }

    const RpcClientContext &RpcClient::context() const {
        return _context;
    }

    const Endpoint &RpcClient::endpoint() const {
        return _context.endpoint;
    }

    RpcStatus RpcClient::invoke(const RpcMethodContext &context, const IRpcSyncRequestData &request,
                                IRpcSyncResponseData &response) {
        if (_service == nullptr)
            initService();

        return RpcSender::invoke(context, request, response);
    }

    RpcStatus RpcClient::invokeAsync(const RpcMethodContext &context, const IRpcAsyncRequestData &request,
                                     const IRpcAsyncResponseData &response, async_callback action, void *owner) {
        if (_service == nullptr)
            initService();

        return RpcSender::invokeAsync(context, request, response, action, owner);
    }

    RpcStatus RpcClient::notify(const RpcMethodContext &context, const IRpcNotifyInfo &info) {
        if (_service == nullptr)
            initService();

        return RpcSender::notify(context, info);
    }

    bool RpcClient::sendSync(const RpcMethodContext &context, const RpcSyncRequest &request, RpcSyncResponse &response,
                             const String &name) {
        return _service->sendSync<RpcSyncRequest, RpcSyncResponse, RpcSyncContext>(request, response, name,
                                                                                   context.tryCount);
    }

    bool RpcClient::sendAsync(const RpcMethodContext &context, const RpcAsyncRequest &request, const String &name) {
        return _service->sendAsync<RpcAsyncRequest, RpcAsyncRequestContext>(request, name);
    }

    bool RpcClient::sendAsync(const RpcMethodContext &context, const RpcNotifyInfo &info, const String &name) {
        return _service->sendAsync<RpcNotifyInfo, RpcNotifyContext>(info, name);
    }

    bool RpcClient::sendAsync(const Endpoint &peerEndpoint, const RpcAsyncResponse &response, const String &name) {
        return _service->sendAsync<RpcAsyncResponse, RpcAsyncResponseContext>(response, name);
    }

    bool RpcClient::initService(bool asyncOpen) {
        if (_service == nullptr) {
            Client client(true);
            client.address = _context.endpoint.address;
            client.port = _context.endpoint.port;
            client.secure = Secure::None;
            client.sendBufferSize = 2 * 1024 * 1024;
            client.receiveBufferSize = 2 * 1024 * 1024;
            client.connection.detectionInterval = TimeSpan::fromSeconds(1);
            client.connection.resumeInterval = TimeSpan::fromSeconds(1);
//            client.senderType = TcpChannelContext::Async;
//            client.receiverType = TcpChannelContext::Async;

            _service = ClientServiceFactory::create(client);
            BaseCommService::InstructionCallback callback;
            callback.tcpInstructions = generateInstructions;
            callback.tcpSampler = clientHeartbeatInstruction;
            callback.owner = this;
            callback.asyncOpen = asyncOpen;
            callback.behaviour.senderType = ServiceBehaviour::SenderTypes::Async;
            if (!_service->initialize(callback))
                return false;

            Sampler *sampler = _service->clientSampler();
            if (sampler != nullptr) {
                sampler->statusChangedDelegates()->add(this, samplerStatusEventHandler);
            }
            return true;
        }
        return false;
    }

    bool RpcClient::unInitService() {
        if (_service != nullptr) {
            const Endpoint &ep = _service->client().endpoint();
            Trace::writeFormatLine("Uninitialized the rpc client service! endpoint: %s", ep.toString().c_str());

            // send close instruction.
            RpcCloseRequest request;
            RpcCloseResponse response;
            _service->sendSync<RpcCloseRequest, RpcCloseResponse, RpcCloseContext>(request, response, "Close");

            Sampler *sampler = _service->clientSampler();
            if (sampler != nullptr) {
                sampler->statusChangedDelegates()->remove(this, samplerStatusEventHandler);
            }

            _service->unInitialize();
            delete _service;
            _service = nullptr;
        }
        return true;
    }

    void RpcClient::onServerStatusChanged(bool online) {
    }

    void RpcClient::generateInstructions(void *owner, Instructions *instructions) {
        auto cs = static_cast<RpcClient *>(owner);
        assert(cs);

        // sender's instructions.
        instructions->add(new HeartbeatInstruction(new InstructionDescription("Heartbeat", new RpcHeartbeatContext())));
        instructions->add(new CloseInstruction(new InstructionDescription("Close", new RpcCloseContext())));

        instructions->add(new RpcSyncInstruction(new InstructionDescription("RpcSync", new RpcSyncContext())));

        instructions->add(new RpcAsyncRequestInstruction(
                new InstructionDescription("RpcAsyncRequest", new RpcAsyncRequestContext())));
        instructions->add(new ServerRpcAsyncResponseInstruction(
                new InstructionDescription("ServerRpcAsyncResponse", new RpcAsyncResponseContext()), cs));

        instructions->add(new RpcNotifyInstruction(new InstructionDescription("RpcNotify", new RpcNotifyContext())));

        // receiver's instructions.
        instructions->add(
                new ServerRpcSyncInstruction2(new InstructionDescription("ServerRpcSync", new RpcSyncContext()), cs));

        instructions->add(new ServerRpcAsyncRequestInstruction2(
                new InstructionDescription("ServerRpcAsyncRequest", new RpcAsyncRequestContext()), cs));
        instructions->add(new RpcAsyncResponseInstruction2(
                new InstructionDescription("RpcAsyncResponse", new RpcAsyncResponseContext())));

        instructions->add(
                new ServerRpcNotifyInstruction2(new InstructionDescription("ServerRpcNotify", new RpcNotifyContext()),
                                                cs));
    }

    InstructionDescription *RpcClient::clientHeartbeatInstruction(void *) {
        return new InstructionDescription("Heartbeat", new RpcHeartbeatContext());
    }

    void RpcClient::samplerStatusEventHandler(void *owner, void *sender, EventArgs *args) {
        auto cs = static_cast<RpcClient *>(owner);
        assert(cs);
        auto e = dynamic_cast<DeviceStatusEventArgs *>(args);
        assert(e);
        cs->onServerStatusChanged(e->newStatus == Device::Status::Online);
    }
}
