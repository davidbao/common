//
//  TcpService.cpp
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/TcpService.h"
#include "configuration/ConfigService.h"
#include "microservice/TcpTemplate.h"

namespace Microservice {
    ITcpAction::ITcpAction() {
    }

    ITcpAction::~ITcpAction() {
    }

    TcpStatus ITcpAction::onGetAction(const StringArray &texts, String &result) {
        return TcpStatus::TcpNotFound;
    }

    TcpStatus ITcpAction::onPostAction(const StringArray &texts, const String &body, String &result) {
        return TcpStatus::TcpNotFound;
    }

    TcpService::TcpService() : _server(nullptr), _actions(false) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<ITcpService>(this);
    }

    TcpService::~TcpService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<ITcpService>();
    }

    bool TcpService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        bool enable = true;
        if (cs->getProperty("server.enabled", enable) && !enable) {
            return false;
        }
        String scheme;
        if (!(cs->getProperty("server.scheme", scheme) && (scheme == "rpc" || scheme == "rpcs"))) {
            return false;
        }
        Port serverPort;
        if (!cs->getProperty("server.port", serverPort)) {
            Trace::error("server.port is incorrect.");
            return false;
        }

        RpcServerContext context(Endpoint("any", serverPort), Secure::None);
        _server = new RpcServer(context);
        registerMethod(new RpcSyncMethod(TcpTemplate::DefaultMethodName, TcpStringRequest(), TcpStringResponse(),
                                         onStringResponse, this));
        return _server->start();
    }

    bool TcpService::unInitialize() {
        if (_server != nullptr) {
            bool result = _server->stop();
            delete _server;
            _server = nullptr;
            return result;
        }
        return false;
    }

    void TcpService::registerMethod(RpcMethod *method) {
        if (_server != nullptr) {
            _server->registerMethod(method);
        }
    }

    void TcpService::registerAction(ITcpAction *action) {
        Locker locker(&_actionsMutex);
        _actions.add(action);
    }

    void TcpService::deregisterAction(ITcpAction *action) {
        Locker locker(&_actionsMutex);
        _actions.remove(action);
    }

    bool TcpService::onStringResponse(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        TcpService *ts = (TcpService *) owner;
        return ts->onStringResponseInner(request, response);
    }

    bool TcpService::onStringResponseInner(const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        Locker locker(&_actionsMutex);
        for (size_t i = 0; i < _actions.count(); i++) {
            ITcpAction *action = _actions[i];
            const TcpStringRequest *trequest = dynamic_cast<const TcpStringRequest *>(request);
            TcpStringResponse *tresponse = dynamic_cast<TcpStringResponse *>(response);
            StringArray texts;
            StringArray::parse(trequest->body, texts, '/');
            String result;
            TcpStatus status = action->onGetAction(texts, result);
            if (status != TcpStatus::TcpNotFound) {
                tresponse->content = result;
                return status == TcpStatus::TcpOk;
            }
        }
        return false;
    }
}
