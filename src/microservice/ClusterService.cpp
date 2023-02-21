//
//  ClusterService.cpp
//  common
//
//  Created by baowei on 2020/3/17.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/ClusterService.h"
#include "system/ServiceFactory.h"
#include "thread/ThreadPool.h"
#include "thread/Timer.h"
#include "configuration/ConfigService.h"

namespace Microservice {
    ClusterContext::ClusterContext() {
    }

    ClusterContext::ClusterContext(const String &key, const String &value, const Version &version) {
        this->version = version;
        this->key = key;
        this->value = value;
    }

    ClusterContext::ClusterContext(const ClusterContext &context) {
        this->operator=(context);
    }

    void ClusterContext::write(Stream *stream) const {
        key.write(stream);
        value.write(stream);
        version.write(stream);
    }

    void ClusterContext::read(Stream *stream) {
        key.read(stream);
        value.read(stream);
        version.read(stream);
    }

    void ClusterContext::copyFrom(const ClusterContext *value) {
        this->key = value->key;
        this->value = value->value;
        this->version = value->version;
    }

    void ClusterContext::operator=(const ClusterContext &value) {
        copyFrom(&value);
    }

    bool ClusterContext::operator==(const ClusterContext &value) const {
        return this->version == value.version &&
               this->key == value.key &&
               this->value == value.value;
    }

    bool ClusterContext::operator!=(const ClusterContext &value) const {
        return !operator==(value);
    }

    ClusterContexts::ClusterContexts() : _items(true), _map(false) {
    }

    ClusterContexts::ClusterContexts(const ClusterContexts &value) : ClusterContexts() {
        this->operator=(value);
    }

    ClusterContexts::ClusterContexts(const ClusterContext **values, size_t count) {
        for (uint32_t i = 0; i < count; i++) {
            const ClusterContext *value = values[i];
            add(*value);
        }
    }

    ClusterContexts::~ClusterContexts() {
    }

    void ClusterContexts::add(const ClusterContext &value) {
        ClusterContext *context = new ClusterContext(value);
        _items.add(context);
        _map.add(value.key, context);
    }

    void ClusterContexts::add(const ClusterContexts &value, off_t offset, size_t count) {
        for (size_t i = offset; i < count; i++) {
            if (i < value.count()) {
                const ClusterContext context = value[i];
                update(context);
            }
        }
    }

    void ClusterContexts::update(const ClusterContexts &value) {
        for (size_t i = 0; i < value.count(); i++) {
            const ClusterContext context = value[i];
            update(context);
        }
    }

    void ClusterContexts::update(const ClusterContext &value) {
        if (_map.contains(value.key)) {
            ClusterContext *temp = nullptr;
            if (_map.at(value.key, temp) && temp != nullptr &&
                value.version > temp->version) {
                ClusterContext old(*temp);

                temp->operator=(value);

                ValueChangedEventArgs e(old, value);
                _valueChangedDelegates.invoke(this, &e);
            }
        } else {
            add(value);

            ValueChangedEventArgs e(ClusterContext(), value);
            _valueChangedDelegates.invoke(this, &e);
        }
    }

    const ClusterContext &ClusterContexts::at(size_t pos) const {
        const ClusterContext *context = _items.at(pos);
        if (context != nullptr)
            return *context;
        static ClusterContext empty;
        return empty;
    }

    size_t ClusterContexts::count() const {
        return _items.count();
    }

    bool ClusterContexts::contains(const ClusterContext &value) const {
        for (size_t i = 0; i < count(); i++) {
            const ClusterContext *context = _items[i];
            if (context->operator==(value)) {
                return true;
            }
        }
        return false;
    }

    bool ClusterContexts::at(const String &key, String &value) const {
        ClusterContext *context = nullptr;
        if (_map.at(key, context)) {
            value = context->value;
            return true;
        }
        return false;
    }

    void ClusterContexts::clear() {
        _items.clear();
        _map.clear();
    }

    bool ClusterContexts::remove(const ClusterContext &value) {
        for (size_t i = 0; i < count(); i++) {
            const ClusterContext *context = _items[i];
            if (context->operator==(value)) {
                return removeAt(i);
            }
        }
        return false;
    }

    bool ClusterContexts::removeAt(size_t i) {
        const ClusterContext *context = _items.at(i);
        if (context != nullptr) {
            _map.remove(context->key);
            return _items.removeAt(i);
        }
        return false;
    }

    void ClusterContexts::operator=(const ClusterContexts &value) {
        clear();
        for (uint32_t i = 0; i < value.count(); i++) {
            const ClusterContext context = value[i];
            add(context);
        }
    }

    bool ClusterContexts::operator==(const ClusterContexts &value) const {
        if (count() != value.count())
            return false;

        for (uint32_t i = 0; i < value.count(); i++) {
            const ClusterContext context1 = value[i];
            const ClusterContext context2 = at(i);
            if (context1 != context2)
                return false;
        }
        return true;
    }

    bool ClusterContexts::operator!=(const ClusterContexts &value) const {
        return !operator==(value);
    }

    void ClusterContexts::write(Stream *stream) const {
        stream->writeUInt32((uint32_t) count());
        for (uint32_t i = 0; i < (uint32_t) count(); i++) {
            const ClusterContext context = at(i);
            context.write(stream);
        }
    }

    void ClusterContexts::read(Stream *stream) {
        clear();
        size_t count = stream->readUInt32();
        for (uint32_t i = 0; i < count; i++) {
            ClusterContext context;
            context.read(stream);
            add(context);
        }
    }

    Delegates *ClusterContexts::valueChangedDelegates() {
        return &_valueChangedDelegates;
    }

    bool ClusterContexts::setValue(const String &key, const String &value, Version &version) {
        ClusterContext *context = nullptr;
        if (_map.at(key, context)) {
            if (context->value != value) {
                context->value = value;
                context->version = Version::increase(context->version);
                version = context->version;
                return true;
            }
        } else {
            ClusterContext temp(key, value, Version::Version1_0);
            add(temp);
            version = temp.version;
            return true;
        }
        return false;
    }

    PullRequest::PullRequest() {
    }

    PullRequest::PullRequest(const String &key, const Version &version) : key(key), version(version) {
    }

    void PullRequest::write(Stream *stream) const {
    }

    void PullRequest::read(Stream *stream) {
    }

    void PullRequest::copyFrom(const IRpcSyncRequestData *value) {
    }

    IRpcSyncRequestData *PullRequest::clone() const {
        PullRequest *data = new PullRequest();
        data->copyFrom(this);
        return data;
    }

    PullResponse::PullResponse() {
    }

    void PullResponse::write(Stream *stream) const {
        context.write(stream);
    }

    void PullResponse::read(Stream *stream) {
        context.read(stream);
    }

    void PullResponse::copyFrom(const IRpcSyncResponseData *value) {
        PullResponse *response = (PullResponse *) value;
        this->context.copyFrom(&response->context);
    }

    IRpcSyncResponseData *PullResponse::clone() const {
        PullResponse *data = new PullResponse();
        data->copyFrom(this);
        return data;
    }

    PushRequest::PushRequest() {
    }

    PushRequest::PushRequest(const ClusterContext &context) {
        contexts.add(context);
    }

    PushRequest::PushRequest(const ClusterContexts &contexts) : contexts(contexts) {
    }

    PushRequest::PushRequest(const ClusterContexts &contexts, off_t offset, size_t count) {
        this->contexts.add(contexts, offset, count);
    }

    PushRequest::~PushRequest() {
    }

    void PushRequest::write(Stream *stream) const {
        contexts.write(stream);
    }

    void PushRequest::read(Stream *stream) {
        contexts.read(stream);
    }

    void PushRequest::copyFrom(const IRpcSyncRequestData *value) {
        PushRequest *response = (PushRequest *) value;
        this->contexts = response->contexts;
    }

    IRpcSyncRequestData *PushRequest::clone() const {
        PushRequest *data = new PushRequest();
        data->copyFrom(this);
        return data;
    }

    PushResponse::PushResponse() {
    }

    PushResponse::~PushResponse() {
    }

    void PushResponse::write(Stream *stream) const {
    }

    void PushResponse::read(Stream *stream) {
    }

    void PushResponse::copyFrom(const IRpcSyncResponseData *value) {
    }

    IRpcSyncResponseData *PushResponse::clone() const {
        PushResponse *data = new PushResponse();
        data->copyFrom(this);
        return data;
    }

    PushNotifyRequest::PushNotifyRequest(const Endpoint &endpoint, int packetCount) : endpoint(endpoint),
                                                                                      packetCount(packetCount) {
    }

    PushNotifyRequest::~PushNotifyRequest() {
    }

    void PushNotifyRequest::write(Stream *stream) const {
        endpoint.write(stream);
        stream->writeInt32(packetCount);
    }

    void PushNotifyRequest::read(Stream *stream) {
        endpoint.read(stream);
        packetCount = stream->readInt32();
    }

    void PushNotifyRequest::copyFrom(const IRpcSyncRequestData *value) {
        PushNotifyRequest *request = (PushNotifyRequest *) value;
        this->endpoint = request->endpoint;
        this->packetCount = request->packetCount;
    }

    IRpcSyncRequestData *PushNotifyRequest::clone() const {
        PushNotifyRequest *data = new PushNotifyRequest();
        data->copyFrom(this);
        return data;
    }

    PushNotifyResponse::PushNotifyResponse() {
    }

    PushNotifyResponse::~PushNotifyResponse() {
    }

    void PushNotifyResponse::write(Stream *stream) const {
    }

    void PushNotifyResponse::read(Stream *stream) {
    }

    void PushNotifyResponse::copyFrom(const IRpcSyncResponseData *value) {
//        PushNotifyResponse* response = (PushNotifyResponse*)value;
    }

    IRpcSyncResponseData *PushNotifyResponse::clone() const {
        PushNotifyResponse *data = new PushNotifyResponse();
        data->copyFrom(this);
        return data;
    }

    ValueChangedEventArgs::ValueChangedEventArgs(const ClusterContext &oldValue, const ClusterContext &newValue)
            : oldValue(oldValue), newValue(newValue) {
    }

    PushAllHolder::PushAllHolder(const Endpoint &endpoint) : endpoint(endpoint) {
    }

    ClusterRpcServer::ClusterRpcServer(const RpcServerContext &context, ClusterService *service) : RpcServer(context),
                                                                                                   _service(service) {
        registerMethod(new RpcSyncMethod("push", PushRequest(), PushResponse(), onPush, this));
    }

    bool ClusterRpcServer::onPush(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        const PushRequest *hrequest = dynamic_cast<const PushRequest *>(request);
//        PushResponse* hresponse = dynamic_cast<PushResponse*>(response);
//        assert(hrequest && hresponse);
//        for(uint32_t i=0; i<hrequest->contexts.count(); i++)
//        {
//            ClusterContext context = hrequest->contexts.at(i);
//            Trace::info(String::format("onPush, key: %s, value: %s, version: %s",
//                                       context.key.c_str(), context.value.c_str(), context.version.toString().c_str()));
//        }
        ClusterRpcServer *cs = static_cast<ClusterRpcServer *>(owner);
        assert(cs);
        cs->_service->update(hrequest->contexts);
        return true;
    }

    void ClusterRpcServer::onClientOpened(const Endpoint &endpoint) {
        _service->pushAll(this, endpoint);
    }

    void ClusterRpcServer::onClientClosed(const Endpoint &endpoint) {
    }

    ClusterRpcClient::ClusterRpcClient(const RpcClientContext &context, ClusterService *service) : RpcClient(context),
                                                                                                   _service(service) {
        registerMethod(new RpcSyncMethod("push", PushRequest(), PushResponse(), onPush, this));
    }

    bool ClusterRpcClient::onPush(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        const PushRequest *hrequest = dynamic_cast<const PushRequest *>(request);
//        PushResponse* hresponse = dynamic_cast<PushResponse*>(response);
//        assert(hrequest && hresponse);
//#ifdef DEBUG
//        for(uint32_t i=0; i<hrequest->contexts.count(); i++)
//        {
//            ClusterContext context = hrequest->contexts.at(i);
//            Trace::info(String::format("onPush, key: %s, value: %s, version: %s",
//                                       context.key.c_str(), context.value.c_str(), context.version.toString().c_str()));
//        }
//#endif
        ClusterRpcClient *cc = static_cast<ClusterRpcClient *>(owner);
        assert(cc);
        cc->_service->update(hrequest->contexts);
        return true;
    }

    bool ClusterRpcClient::push(const ClusterContexts &contexts) {
        RpcMethodContext context("push");
        PushRequest request(contexts);
        PushResponse response;
        RpcStatus status = invoke(context, request, response);
        return status.isOk();
    }

    void ClusterRpcClient::onServerStatusChanged(bool online) {
    }

    void IClusterService::addValue(const String &key, const String &value) {
        setValue(key, value);
    }

    void IClusterService::removeValue(const String &key) {
        setValue(key, String::Empty);
    }

    ClusterService::ClusterService() : _server(nullptr), _sendTimer(nullptr), _maxPacketCount(0) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IClusterService>(this);
    }

    ClusterService::~ClusterService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IClusterService>();
    }

    bool ClusterService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        if (!cs->contains("summer.cluster")) {
            return false;
        }

        if (_sendTimer == nullptr) {
            TimeSpan interval = TimeSpan::fromSeconds(1);
            cs->getProperty("summer.cluster.sync.interval", interval);
            _sendTimer = new Timer("cluster_send_timer", pushTimeUp, this, interval);
        }

        cs->getProperty("summer.cluster.sync.maxPacketCount", _maxPacketCount);
//        if(_maxPacketCount < 100)
//            _maxPacketCount = 100;

        Endpoint current;
        if (!cs->getProperty("summer.cluster.current", current)) {
            return false;
        }
        _current = current;

        int index = 0;
        static const char *prefix = "summer.cluster.nodes[%d]";
        while (true) {
            String str = String::format(prefix, index);
            if (!cs->contains(str))
                break;

            Endpoint node;
            if (cs->getProperty(str, node)) {
                if (node != current)
                    _nodes.add(node);
            }

            index++;
        }

        if (!_current.isEmpty()) {
            // listen cluster server.
            RpcServerContext context(_current);
            _server = new ClusterRpcServer(context, this);
            _server->start();
        }

        // connect the another cluster servers.
        for (uint32_t i = 0; i < _nodes.count(); i++) {
            Endpoint endpoint = _nodes[i];
            if (!endpoint.isEmpty()) {
                RpcClientContext context(endpoint);
                RpcClient *client = new ClusterRpcClient(context, this);
                client->connect(true);
                _clients.add(client);
            }
        }

        return true;
    }

    bool ClusterService::unInitialize() {
        if (_sendTimer != nullptr) {
            delete _sendTimer;
            _sendTimer = nullptr;
        }

        for (uint32_t i = 0; i < _clients.count(); i++) {
            RpcClient *client = _clients[i];
            client->disconnect();
        }
        _clients.clear();

        if (_server != nullptr) {
            _server->stop();
            delete _server;
            _server = nullptr;
        }
        return true;
    }

    const Endpoint ClusterService::serverEndpoint() const {
        return _current;
    }

    void ClusterService::update(const ClusterContexts &value) {
        Locker locker(&_contextsMutex);
        _contexts.update(value);
    }

    void ClusterService::update(const ClusterContext &value) {
        Locker locker(&_contextsMutex);
        _contexts.update(value);
    }

    void ClusterService::pushAll(ClusterRpcServer *server, const Endpoint &endpoint) {
        Locker locker(&_contextsMutex);
        if (_contexts.count() > 0) {
            ThreadHolder *holder = new ThreadHolder(this, new PushAllHolder(endpoint));
            ThreadPool::startAsync(pushAllAction, holder);
        }
    }

    void ClusterService::pushAllAction(ThreadHolder *holder) {
        ClusterService *cc = static_cast<ClusterService *>(holder->owner);
        assert(cc);
        PushAllHolder *value = (PushAllHolder *) holder->value;
        assert(value);

        cc->pushAllSync(cc->_server, value->endpoint);

        delete holder;
    }

    bool ClusterService::pushAllSync(ClusterRpcServer *server, const Endpoint &endpoint) {
        Locker locker(&_contextsMutex);

        const size_t count = _contexts.count();
        for (size_t i = 0; i < count; i += _maxPacketCount) {
            off_t offset = (off_t) i;
            size_t minCount = Math::min(_maxPacketCount, (int) (count - i));
            if (!pushAllSync(server, endpoint, _contexts, offset, minCount))
                return false;
        }
        return true;
    }

    bool
    ClusterService::pushAllSync(ClusterRpcServer *server, const Endpoint &endpoint, const ClusterContexts &contexts,
                                off_t offset, size_t count) {
        RpcMethodContext context("push", endpoint);
        PushRequest request(contexts, offset, count);
        if (request.contexts.count() > 0) {
            PushResponse response;
            RpcStatus status = server->invoke(context, request, response);
            if (!status.isOk()) {
                Trace::error(String::format("Failed to push all values to the other service'%s'.",
                                            endpoint.toString().c_str()));
                return false;
            }
            return true;
        }
        return true;
    }

    bool ClusterService::hasNodes() const {
        return _nodes.count() > 0;
    }

    void ClusterService::setValue(const String &key, const String &value) {
        Locker locker(&_contextsMutex);

        Version version;
        if (_contexts.setValue(key, value, version)) {
            Locker locker2(&_senderContextsMutex);
            _senderContexts.enqueue(new ClusterContext(key, value, version));
        }
    }

    void ClusterService::pushProc() {
        _senderContextsMutex.lock();
        size_t count = _senderContexts.count();
        if (count > 0) {
            ClusterContext **contexts = new ClusterContext *[count];
            _senderContexts.copyTo(contexts);
            _senderContexts.makeNull(false);
            _senderContextsMutex.unlock();

            pushProc((const ClusterContext **) contexts, count);

            for (uint32_t i = 0; i < count; i++) {
                ClusterContext *context = contexts[i];
                delete context;
            }
            delete[] contexts;
        } else {
            _senderContextsMutex.unlock();
        }
    }

    void ClusterService::pushProc(const ClusterContext **values, size_t count) {
        ClusterContexts contexts(values, count);

        // send to the other services.
        for (uint32_t i = 0; i < _clients.count(); i++) {
            RpcClient *client = _clients[i];
            RpcMethodContext context("push", 3);
            PushRequest request(contexts);
            PushResponse response;
            RpcStatus status = client->invoke(context, request, response);
            if (!status.isOk()) {
                Trace::error(String::format("Failed to push to the other service'%s'.",
                                            client->endpoint().toString().c_str()));
            }
        }
    }

    void ClusterService::pushTimeUp(void *state) {
        ClusterService *cs = (ClusterService *) state;
        assert(cs);
        cs->pushProc();
    }

    const ClusterContexts &ClusterService::contexts() {
        Locker locker(&_contextsMutex);
        return _contexts;
    }

    Delegates *ClusterService::valueChangedDelegates() {
        Locker locker(&_contextsMutex);
        return _contexts.valueChangedDelegates();
    }
}
