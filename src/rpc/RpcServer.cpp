//
//  RpcServer.cpp
//  common
//
//  Created by baowei on 2020/2/20.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "rpc/RpcServer.h"
#include "rpc/RpcContext.h"
#include "RpcInstruction.h"
#include "rpc/RpcInstructionContext.h"
#include "communication/ServerService.h"

namespace Rpc {
    RpcServerContext::RpcServerContext(const Endpoint &endpoint) : endpoint(endpoint) {
    }

    RpcServerContext::RpcServerContext(const RpcServerContext &other) : RpcServerContext(other.endpoint) {
    }

    bool RpcServerContext::equals(const RpcServerContext &other) const {
        return this->endpoint == other.endpoint;
    }

    void RpcServerContext::evaluates(const RpcServerContext &other) {
        this->endpoint = other.endpoint;
    }

    IRpcServerEvent::IRpcServerEvent() = default;

    IRpcServerEvent::~IRpcServerEvent() = default;

    RpcServer::RpcServer(const RpcServerContext &context) : _context(context), _service(nullptr) {
    }

    RpcServer::~RpcServer() {
        unInitService();
    }

    bool RpcServer::start() {
        return initService();
    }

    bool RpcServer::stop() {
        return unInitService();
    }

    bool RpcServer::connected() const {
        return _service != nullptr;
    }

    bool RpcServer::sendAsync(const Endpoint &peerEndpoint, const RpcAsyncResponse &response, const String &name) {
        if (peerEndpoint.isEmpty())
            return _service->sendAsyncAll<RpcAsyncResponse, RpcAsyncResponseContext>(response, name);
        else
            return _service->sendAsync<RpcAsyncResponse, RpcAsyncResponseContext>(peerEndpoint, response, name);
    }

    bool RpcServer::sendSync(const RpcMethodContext &context, const RpcSyncRequest &request, RpcSyncResponse &response,
                             const String &name) {
        if (context.endpoint.isEmpty())
            return _service->sendSyncAll<RpcSyncRequest, RpcSyncResponse, RpcSyncContext>(request, response, name,
                                                                                          context.tryCount);
        else
            return _service->sendSync<RpcSyncRequest, RpcSyncResponse, RpcSyncContext>(context.endpoint, request,
                                                                                       response, name,
                                                                                       context.tryCount);
    }

    bool RpcServer::sendAsync(const RpcMethodContext &context, const RpcAsyncRequest &request, const String &name) {
        if (context.endpoint.isEmpty())
            return _service->sendAsyncAll<RpcAsyncRequest, RpcAsyncRequestContext>(request, name);
        else
            return _service->sendAsync<RpcAsyncRequest, RpcAsyncRequestContext>(context.endpoint, request, name);
    }

    bool RpcServer::sendAsync(const RpcMethodContext &context, const RpcNotifyInfo &info, const String &name) {
        if (context.endpoint.isEmpty())
            return _service->sendAsyncAll<RpcNotifyInfo, RpcNotifyContext>(info, name);
        else
            return _service->sendAsync<RpcNotifyInfo, RpcNotifyContext>(context.endpoint, info, name);
    }

    bool RpcServer::closeClient(const Endpoint &endpoint) {
        if (_service != nullptr) {
            return _service->closeClient(endpoint);
        }
        return false;
    }

    void RpcServer::onClientOpened(const Endpoint &endpoint) {
    }

    void RpcServer::onClientClosed(const Endpoint &endpoint) {
    }

    bool RpcServer::initService() {
        if (_service == nullptr) {
            Server server(true);
            server.address = _context.endpoint.address;
            server.port = _context.endpoint.port;
            server.secure = Secure::None;
            server.sendBufferSize = 2 * 1024 * 1024;
            server.receiveBufferSize = 2 * 1024 * 1024;

            _service = ServerServiceFactory::create(server);
            BaseCommService::InstructionCallback server_callback;
            server_callback.tcpInstructions = generateInstructions;
            server_callback.owner = this;
            if (!_service->initialize(server_callback))
                return false;

            _service->addCloseDelegate(Delegate(this, clientCloseEventHandler));
            _service->addAcceptDelegate(Delegate(this, clientAcceptEventHandler));

            return true;
        }
        return false;
    }

    bool RpcServer::unInitService() {
        if (_service != nullptr) {
            _service->removeCloseDelegate(Delegate(this, clientCloseEventHandler));
            _service->removeAcceptDelegate(Delegate(this, clientAcceptEventHandler));

            return _service->unInitialize();
        }
        return false;
    }

    void RpcServer::generateInstructions(void *owner, Instructions *instructions) {
        auto cs = static_cast<RpcServer *>(owner);
        assert(cs);

        // receiver's instructions.
        instructions->add(new ServerHeartbeatInstruction(
                new InstructionDescription("ServerHeartbeat", new RpcHeartbeatContext())));
        instructions->add(
                new ServerCloseInstruction(new InstructionDescription("ServerClose", new RpcCloseContext()), cs));

        instructions->add(
                new ServerRpcSyncInstruction(new InstructionDescription("ServerRpcSync", new RpcSyncContext()), cs));

        instructions->add(new ServerRpcAsyncRequestInstruction(
                new InstructionDescription("ServerRpcAsyncRequest", new RpcAsyncRequestContext()), cs));
        instructions->add(new RpcAsyncResponseInstruction(
                new InstructionDescription("RpcAsyncResponse", new RpcAsyncResponseContext())));

        instructions->add(
                new ServerRpcNotifyInstruction(new InstructionDescription("ServerRpcNotify", new RpcNotifyContext()),
                                               cs));

        // sender's instructions.
        instructions->add(new RpcSyncInstruction2(new InstructionDescription("RpcSync", new RpcSyncContext())));

        instructions->add(new RpcAsyncRequestInstruction2(
                new InstructionDescription("RpcAsyncRequest", new RpcAsyncRequestContext())));
        instructions->add(new ServerRpcAsyncResponseInstruction2(
                new InstructionDescription("ServerRpcAsyncResponse", new RpcAsyncResponseContext()), cs));

        instructions->add(new RpcNotifyInstruction2(new InstructionDescription("RpcNotify", new RpcNotifyContext())));
    }

    void RpcServer::clientCloseEventHandler(void *owner, void *sender, EventArgs *args) {
        auto cs = static_cast<RpcServer *>(owner);
        assert(cs);
        auto e = dynamic_cast<TcpClientEventArgs *>(args);
        assert(e);
        cs->onClientClosed(e->endpoint);
    }

    void RpcServer::clientAcceptEventHandler(void *owner, void *sender, EventArgs *args) {
        auto cs = static_cast<RpcServer *>(owner);
        assert(cs);
        auto e = dynamic_cast<TcpClientEventArgs *>(args);
        assert(e);
        cs->onClientOpened(e->endpoint);
    }
}
