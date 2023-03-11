//
//  TcpTemplate.cpp
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/TcpTemplate.h"
#include "diag/Trace.h"
#include "IO/MemoryStream.h"
#include "data/StringArray.h"
#include "microservice/LoadBalancerClient.h"

namespace Microservice {
    TcpStringRequest::TcpStringRequest(const String &body) : body(body) {
    }

    void TcpStringRequest::write(Stream *stream) const {
        body.write(stream);
    }

    void TcpStringRequest::read(Stream *stream) {
        body.read(stream);
    }

    void TcpStringRequest::copyFrom(const IRpcSyncRequestData *value) {
        TcpStringRequest *request = (TcpStringRequest *) value;
        this->body = request->body;
    }

    IRpcSyncRequestData *TcpStringRequest::clone() const {
        TcpStringRequest *data = new TcpStringRequest();
        data->copyFrom(this);
        return data;
    }

    TcpStringResponse::TcpStringResponse() {
    }

    void TcpStringResponse::write(Stream *stream) const {
        content.write(stream);
    }

    void TcpStringResponse::read(Stream *stream) {
        content.read(stream);
    }

    void TcpStringResponse::copyFrom(const IRpcSyncResponseData *value) {
        TcpStringResponse *response = (TcpStringResponse *) value;
        this->content = response->content;
    }

    IRpcSyncResponseData *TcpStringResponse::clone() const {
        TcpStringResponse *data = new TcpStringResponse();
        data->copyFrom(this);
        return data;
    }

    Mutex TcpTemplate::_clientsMutex;
    RpcClients TcpTemplate::_clients;
    const char *TcpTemplate::DefaultMethodName = "ease.tcp.defaultmethod";

    TcpTemplate::TcpTemplate() {
    }

    bool TcpTemplate::get(const String &url, String &content) {
        content = getContent(url);
        return !content.isNullOrEmpty();
    }

    String TcpTemplate::getContent(const String &url) {
        Url rUrl(url);
        if (!rUrl.isEmpty()) {
            String body = rUrl.relativeUrl();
            return postContent(url, body);
        }
        return String::Empty;
    }

    String TcpTemplate::postContent(const String &url, const String &body) {
        RpcMethodContext context(DefaultMethodName);
        TcpStringRequest request(body);
        TcpStringResponse response;
        if (invoke(url, request, response))
            return response.content;
        return String::Empty;
    }

    bool TcpTemplate::invoke(const String &url, const IRpcSyncRequestData &request, IRpcSyncResponseData &response) {
        // suck like ease.tcp://localhost:5002/hello
        Url rUrl = parseUrl(url);
        if (rUrl.isEmpty())
            return false;

        Locker locker(&_clientsMutex);
        RpcClient *client = getOrNewClient(rUrl);
        RpcMethodContext context(DefaultMethodName);
        RpcStatus status = client->invoke(context, request, response);
        return status.isOk();
    }

    bool TcpTemplate::invokeAsync(const String &url, const IRpcAsyncRequestData &request,
                                  const IRpcAsyncResponseData &response, async_callback action, void *owner) {
        // suck like ease.tcp://localhost:5002/hello
        Url rUrl = parseUrl(url);
        if (rUrl.isEmpty())
            return false;

        Locker locker(&_clientsMutex);
        RpcClient *client = getOrNewClient(rUrl);
        RpcMethodContext context(DefaultMethodName);
        RpcStatus status = client->invokeAsync(context, request, response, action, owner);
        return status.isOk();
    }

    bool TcpTemplate::notify(const String &url, const IRpcNotifyInfo &info) {
        // suck like ease.tcp://localhost:5002/hello
        Url rUrl = parseUrl(url);
        if (rUrl.isEmpty())
            return false;

        Locker locker(&_clientsMutex);
        RpcClient *client = getOrNewClient(rUrl);
        RpcMethodContext context(DefaultMethodName);
        RpcStatus status = client->notify(context, info);
        return status.isOk();
    }

    Url TcpTemplate::parseUrl(const String &original) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        LoadBalancerClient *service = factory->getService<LoadBalancerClient>();
        if (service != nullptr) {
            Url url = service->reconstructUrl(original);
            if (url.scheme() == Url::SchemeEaseTcp || url.scheme() == Url::SchemeEaseTcps)
                return url;
        }
        return Url::Empty;
    }

    RpcClient *TcpTemplate::getOrNewClient(const Url &url) {
        RpcClient *client = nullptr;
        for (size_t i = 0; i < _clients.count(); i++) {
            RpcClient *temp = _clients[i];
            const Endpoint &endpoint = temp->endpoint();
            if (endpoint == url.endpoint()) {
                client = temp;
                break;
            }
        }

        if (client == nullptr) {
            RpcClientContext context(url.endpoint(), url.isSecure() ? Secure(true) : Secure::None);
            client = new RpcClient(context);
            _clients.add(client);
        }
        return client;
    }
}
