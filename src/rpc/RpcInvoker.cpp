//
//  RpcInvoker.cpp
//  common
//
//  Created by baowei on 2020/3/19.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "rpc/RpcInvoker.h"
#include "rpc/RpcInstructionContext.h"

namespace Rpc {
    RpcAsyncElement::RpcAsyncElement(const Uuid &token, const IRpcAsyncRequestData *request,
                                     const IRpcAsyncResponseData *response, async_callback action, void *owner) {
        this->token = token;
        this->request = request->clone();
        this->response = response->clone();
        this->action = action;
        this->owner = owner;
    }

    RpcAsyncElement::~RpcAsyncElement() {
        delete request;
        delete response;
    }

    RpcMethod::RpcMethod(const String &name, const IRpcData &request) {
        this->name = name;
        this->request = request.clone();
    }

    RpcMethod::~RpcMethod() {
        delete request;
    }

    RpcSyncMethod::RpcSyncMethod(const String &name, const IRpcSyncRequestData &request,
                                 const IRpcSyncResponseData &response, sync_callback action, void *owner) : RpcMethod(
            name, request) {
        this->response = response.clone();
        this->action = action;
        this->owner = owner;
    }

    RpcSyncMethod::~RpcSyncMethod() {
        delete response;
        action = nullptr;
    }

    RpcAsyncMethod::RpcAsyncMethod(const String &name, const IRpcAsyncRequestData &request,
                                   const IRpcAsyncResponseData &response, async_callback action, void *owner)
            : RpcMethod(name, request) {
        this->response = response.clone();
        this->action = action;
        this->owner = owner;
    }

    RpcAsyncMethod::~RpcAsyncMethod() {
        delete response;
        action = nullptr;
    }

    RpcNotifyMethod::RpcNotifyMethod(const String &name, const IRpcNotifyInfo &request,
                                     notify_callback action, void *owner) : RpcMethod(name, request) {
        this->action = action;
        this->owner = owner;
    }

    RpcNotifyMethod::~RpcNotifyMethod() {
        action = nullptr;
    }

    RpcMethods::RpcMethods() {
    }

    RpcMethods::~RpcMethods() {
    }

    void RpcMethods::add(RpcMethod *method) {
        if (method == nullptr)
            return;

        if (!contains(method->name))
            _methods.add(method);
        else {
            for (size_t i = 0; i < _methods.count(); i++) {
                RpcMethod *item = _methods[i];
                if (item->name == method->name) {
                    _methods.set(i, method);
                }
            }
        }
    }

    bool RpcMethods::at(const String &methodName, RpcMethod *&method) const {
        for (size_t i = 0; i < _methods.count(); i++) {
            RpcMethod *item = _methods[i];
            if (item->name == methodName) {
                method = item;
                return true;
            }
        }
        return false;
    }

    bool RpcMethods::contains(const String &methodName) const {
        for (size_t i = 0; i < _methods.count(); i++) {
            const RpcMethod *item = _methods[i];
            if (item->name == methodName)
                return true;
        }
        return false;
    }

    IRpcSenderEvent::IRpcSenderEvent() {
    }

    IRpcSenderEvent::~IRpcSenderEvent() {
    }

    RpcSender::RpcSender() : _asyncElements(true) {
    }

    RpcSender::~RpcSender() {
    }

    RpcStatus RpcSender::invoke(const RpcMethodContext &context, const IRpcSyncRequestData &request,
                                IRpcSyncResponseData &response) {
        if (connected()) {
            RpcSyncRequest rrequest(context, &request);
            RpcSyncResponse rresponse(context, &response);
            bool result = sendSync(context, rrequest, rresponse, "RpcSync");
//            bool result = _service->sendSync<RpcSyncRequest, RpcSyncResponse, RpcSyncContext>(rrequest, rresponse, "RpcSync", context.tryCount);
            if (result) {
                response.copyFrom(rresponse.data());
                return RpcStatus(RpcStatus::Ok);
            } else {
                return RpcStatus(RpcStatus::CommError);
            }
        }
        return RpcStatus(RpcStatus::Disconnected);
    }

    RpcStatus RpcSender::invokeAsync(const RpcMethodContext &context, const IRpcAsyncRequestData &request,
                                     const IRpcAsyncResponseData &response, async_callback action, void *owner) {
        if (connected()) {
            RpcAsyncRequest rrequest(context, &request);
            _asyncElementsMutex.lock();
            _asyncElements.add(rrequest.token(),
                               new RpcAsyncElement(rrequest.token(), &request, &response, action, owner));
            _asyncElementsMutex.unlock();
            bool result = sendAsync(context, rrequest, "RpcAsyncRequest");
//            bool result = _service->sendAsync<RpcAsyncRequest, RpcAsyncRequestContext>(rrequest, "RpcAsyncRequest");
            if (result) {
                return RpcStatus(RpcStatus::Ok);
            } else {
                return RpcStatus(RpcStatus::CommError);
            }
        }
        return RpcStatus(RpcStatus::Disconnected);
    }

    RpcStatus RpcSender::notify(const RpcMethodContext &context, const IRpcNotifyInfo &info) {
        if (connected()) {
            RpcNotifyInfo rinfo(context, &info);
            bool result = sendAsync(context, rinfo, "RpcNotify");
//            bool result = _service->sendAsync<RpcNotifyInfo, RpcNotifyContext>(rinfo, "RpcNotify");
            if (result) {
                return RpcStatus(RpcStatus::Ok);
            } else {
                return RpcStatus(RpcStatus::CommError);
            }
        }
        return RpcStatus(RpcStatus::Disconnected);
    }

    bool RpcSender::onAsyncResponseSetValue(const RpcMethodContext &context, Stream *stream, const Uuid &token) {
        _asyncElementsMutex.lock();
        RpcAsyncElement *element = nullptr;
        if (_asyncElements.at(token, element)) {
            bool result = false;
            async_callback action = element->action;
            if (action != nullptr) {
                element->response->read(stream);
                action(element->owner, element->request, element->response);
                result = true;
            }

            if (result)
                _asyncElements.remove(token);
        }
        _asyncElementsMutex.unlock();
        return false;
    }

    IRpcReceiverEvent::IRpcReceiverEvent() {
    }

    IRpcReceiverEvent::~IRpcReceiverEvent() {
    }

    RpcReceiver::RpcReceiver() : _methods() {
    }

    RpcReceiver::~RpcReceiver() {
    }

    void RpcReceiver::registerMethod(RpcMethod *method) {
        _methods.add(method);
    }

    bool RpcReceiver::onSyncSetValue(const RpcMethodContext &context, Stream *stream, IRpcSyncRequestData *request,
                                     IRpcSyncResponseData *&response) {
        RpcMethod *m = nullptr;
        RpcSyncMethod *method = nullptr;
        if (_methods.at(context.name, m) && (method = dynamic_cast<RpcSyncMethod *>(m)) != nullptr) {
            sync_callback action = method->action;
            if (action != nullptr) {
                method->request->read(stream);
                action(method->owner, method->request, method->response);
                response = method->response->clone();
                return true;
            }
        }
        return false;
    }

    bool RpcReceiver::onAsyncRequestSetValue(const RpcMethodContext &context, Stream *stream, const Uuid &token,
                                             const Endpoint &peerEndpoint) {
        RpcMethod *m = nullptr;
        RpcAsyncMethod *method = nullptr;
        if (_methods.at(context.name, m) && (method = dynamic_cast<RpcAsyncMethod *>(m)) != nullptr) {
            async_callback action = method->action;
            if (action != nullptr) {
                method->request->read(stream);
                action(method->owner, method->request, method->response);

                // send async reponse.
                if (connected()) {
                    RpcAsyncResponse response(context, token, method->response);
                    return sendAsync(peerEndpoint, response, "RpcAsyncResponse");
//                    return _service->sendAsync<RpcAsyncResponse, RpcAsyncResponseContext>(peerEndpoint, response, "RpcAsyncResponse");
                }
            }
        }
        return false;
    }

    bool RpcReceiver::onNotifySetValue(const RpcMethodContext &context, Stream *stream, IRpcNotifyInfo *info) {
        RpcMethod *m = nullptr;
        RpcNotifyMethod *method = nullptr;
        if (_methods.at(context.name, m) && (method = dynamic_cast<RpcNotifyMethod *>(m)) != nullptr) {
            notify_callback action = method->action;
            if (action != nullptr) {
                method->request->read(stream);
                action(method->owner, method->request);
                return true;
            }
        }
        return false;
    }
}
