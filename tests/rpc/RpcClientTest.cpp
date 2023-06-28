//
//  RpcClientTest.cpp
//  common
//
//  Created by baowei on 2023/6/26.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "rpc/RpcClient.h"
#include "rpc/RpcServer.h"
#include "data/Vector.h"

using namespace Rpc;
using namespace Data;

typedef Vector<int> IntArray;

static const int g_port = 11459;
static const Endpoint _endpoint("127.0.0.1", g_port);

class HelloRequest : public IRpcSyncRequestData {
public:
    String user;

    explicit HelloRequest(const String &user = String::Empty) : user(user) {
    }

    void write(Stream *stream) const override {
        user.write(stream);
    }

    void read(Stream *stream) override {
        user.read(stream);
    }

    void copyFrom(const IRpcSyncRequestData *value) override {
        auto request = (HelloRequest *) value;
        this->user = request->user;
    }

    IRpcSyncRequestData *clone() const override {
        auto request = new HelloRequest();
        request->copyFrom(this);
        return request;
    }
};

class HelloResponse : public IRpcSyncResponseData {
public:
    String message;

    void write(Stream *stream) const override {
        message.write(stream);
    }

    void read(Stream *stream) override {
        message.read(stream);
    }

    void copyFrom(const IRpcSyncResponseData *value) override {
        auto response = (HelloResponse *) value;
        this->message = response->message;
    }

    IRpcSyncResponseData *clone() const override {
        auto data = new HelloResponse();
        data->copyFrom(this);
        return data;
    }
};

class MultiPacketRequest : public IRpcSyncRequestData {
public:
    IntArray array;

    MultiPacketRequest() : array(1024 * 1024) {
    }

    explicit MultiPacketRequest(const IntArray &value) : array(value) {
    }

    void write(Stream *stream) const override {
        stream->writeUInt32((uint32_t) array.count());
        stream->write((uint8_t *) array.data(), 0, array.count() * sizeof(int32_t));
//        for (uint i=0; i<value.count(); i++)
//        {
//            stream->writeInt32(value[i]);
//        }
    }

    void read(Stream *stream) override {
        uint32_t count = stream->readUInt32();
        auto buffer = new int32_t[count];
        stream->read((uint8_t *) buffer, 0, count * sizeof(int32_t));
        array.addRange(buffer, count);
//        for (uint i=0; i<count; i++)
//        {
//            value.add(stream->readInt32());
//        }
        delete[] buffer;
    }

    void copyFrom(const IRpcSyncRequestData *value) override {
        auto request = (MultiPacketRequest *) value;
        this->array = request->array;
    }

    IRpcSyncRequestData *clone() const override {
        auto data = new MultiPacketRequest();
        data->copyFrom(this);
        return data;
    }
};

class MultiPacketResponse : public IRpcSyncResponseData {
public:
    String message;

    void write(Stream *stream) const override {
        message.write(stream);
    }

    void read(Stream *stream) override {
        message.read(stream);
    }

    void copyFrom(const IRpcSyncResponseData *value) override {
        auto response = (MultiPacketResponse *) value;
        this->message = response->message;
    }

    IRpcSyncResponseData *clone() const override {
        auto data = new MultiPacketResponse();
        data->copyFrom(this);
        return data;
    }
};

class TestRpcServer : public RpcServer {
public:
    explicit TestRpcServer(const RpcServerContext &context) : RpcServer(context) {
        registerMethod(new RpcSyncMethod("sayHello", HelloRequest(), HelloResponse(), TestRpcServer::onSayHello));
        registerMethod(new RpcNotifyMethod("sayHello2", HelloRequest(), TestRpcServer::onSayHello2));
        registerMethod(new RpcAsyncMethod("sayHello3", HelloRequest(), HelloResponse(), TestRpcServer::onSayHello3));

        registerMethod(new RpcSyncMethod("sayHelloMultiPacket", MultiPacketRequest(), MultiPacketResponse(),
                                         TestRpcServer::onSayHelloMultiPacket));
        registerMethod(new RpcNotifyMethod("sayHello2MultiPacket", MultiPacketRequest(),
                                           TestRpcServer::onSayHello2MultiPacket));
    }

//    Endpoint _endpoint;
    void onClientOpened(const Endpoint &endpoint) override {
//        _endpoint = endpoint;

//        sayHello(endpoint, "test server");
        Task::run(sayHelloAsync, this);
    }

    String sayHello(const Endpoint &endpoint, const String &user) {
        RpcMethodContext context("sayHello", endpoint);
        HelloRequest request(user);
        HelloResponse response;
        RpcStatus status = invoke(context, request, response);
        if (status.isOk())
            return response.message;
        return String::Empty;
    }

    static void sayHelloAsync(TestRpcServer *server) {
//        Thread::msleep(100);

        server->sayHello(Endpoint::Empty, "test server");
    }

    bool sayHello2(const Endpoint &endpoint, const String &user) {
        RpcMethodContext context("sayHello2", endpoint);
        HelloRequest request(user);
        RpcStatus status = notify(context, request);
        return status.isOk();
    }

    bool sayHello3(const Endpoint &endpoint, const String &user) {
        RpcMethodContext context("sayHello3", endpoint);
        HelloRequest request(user);
        HelloResponse response;
        RpcStatus status = invokeAsync(context, request, response, onEndSayHello3);
        return status.isOk();
    }

    static bool onEndSayHello3(void *owner, const IRpcAsyncRequestData *request, IRpcAsyncResponseData *response) {
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        Debug::writeFormatLine("response: %s", hresponse->message.c_str());
        return true;
    }

    static bool onSayHello(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        assert(hrequest && hresponse);
        hresponse->message = String::format("Hello %s!", hrequest->user.c_str());
        Debug::writeFormatLine("onSayHello, message: %s", hresponse->message.c_str());
        return true;
    }

    static void onSayHello2(void *owner, const IRpcNotifyInfo *request) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        assert(hrequest);
        Debug::writeFormatLine(String::format("Hello %s!", hrequest->user.c_str()));
    }

    static bool onSayHello3(void *owner, const IRpcAsyncRequestData *request, IRpcAsyncResponseData *response) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        assert(hrequest && hresponse);
        hresponse->message = String::format("Hello %s!", hrequest->user.c_str());
        return true;
    }

    static bool onSayHelloMultiPacket(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        auto hrequest = dynamic_cast<const MultiPacketRequest *>(request);
        auto hresponse = dynamic_cast<MultiPacketResponse *>(response);
        assert(hrequest && hresponse);
        const IntArray &value = hrequest->array;
        String valueStr;
        if (value.count() > 0) {
            valueStr = String::format("%d-%d", value[0], value[value.count() - 1]);
        } else {
            valueStr = "count=0";
        }
        hresponse->message = String::format("Hello %s!", valueStr.c_str());
        return true;
    }

    static void onSayHello2MultiPacket(void *owner, const IRpcNotifyInfo *request) {
        auto hrequest = dynamic_cast<const MultiPacketRequest *>(request);
        assert(hrequest);
        const IntArray &value = hrequest->array;
        String valueStr;
        if (value.count() > 0) {
            valueStr = String::format("%d-%d", value[0], value[value.count() - 1]);
        } else {
            valueStr = "count=0";
        }
        Debug::writeFormatLine(String::format("Hello %s!", valueStr.c_str()));
    }
};

class TestRpcClient : public RpcClient {
public:
    explicit TestRpcClient(const RpcClientContext &context) : RpcClient(context) {
        registerMethod(new RpcSyncMethod("sayHello", HelloRequest(), HelloResponse(), TestRpcClient::onSayHello));
        registerMethod(new RpcNotifyMethod("sayHello2", HelloRequest(), TestRpcClient::onSayHello2));
        registerMethod(new RpcAsyncMethod("sayHello3", HelloRequest(), HelloResponse(), TestRpcClient::onSayHello3));
    }

    static bool onSayHello(void *owner, const IRpcSyncRequestData *request, IRpcSyncResponseData *response) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        assert(hrequest && hresponse);
        hresponse->message = String::format("Hello %s!", hrequest->user.c_str());
        Debug::writeFormatLine("onSayHello, message: %s", hresponse->message.c_str());
        return true;
    }

    static void onSayHello2(void *owner, const IRpcNotifyInfo *request) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        assert(hrequest);
        Debug::writeFormatLine(String::format("Hello %s!", hrequest->user.c_str()));
    }

    static bool onSayHello3(void *owner, const IRpcAsyncRequestData *request, IRpcAsyncResponseData *response) {
        auto hrequest = dynamic_cast<const HelloRequest *>(request);
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        assert(hrequest && hresponse);
        hresponse->message = String::format("Hello %s!", hrequest->user.c_str());
        return true;
    }

    String sayHello(const String &user) {
        RpcMethodContext context("sayHello");
        HelloRequest request(user);
        HelloResponse response;
        RpcStatus status = invoke(context, request, response);
        if (status.isOk())
            return response.message;
        return String::Empty;
    }

    bool sayHello2(const String &user) {
        RpcMethodContext context("sayHello2");
        HelloRequest request(user);
        RpcStatus status = notify(context, request);
        return status.isOk();
    }

    bool sayHello3(const String &user) {
        RpcMethodContext context("sayHello3");
        HelloRequest request(user);
        HelloResponse response;
        RpcStatus status = invokeAsync(context, request, response, onEndSayHello3);
        return status.isOk();
    }

    static bool onEndSayHello3(void *owner, const IRpcAsyncRequestData *request, IRpcAsyncResponseData *response) {
        auto hresponse = dynamic_cast<HelloResponse *>(response);
        Debug::writeFormatLine("response: %s", hresponse->message.c_str());
        return true;
    }

    String sayHelloMultiPacket(const IntArray &value) {
        RpcMethodContext context("sayHelloMultiPacket");
        MultiPacketRequest request(value);
        MultiPacketResponse response;
        RpcStatus status = invoke(context, request, response);
        if (status.isOk())
            return response.message;
        return String::Empty;
    }

    bool sayHello2MultiPacket(const IntArray &value) {
        RpcMethodContext context("sayHello2MultiPacket");
        MultiPacketRequest request(value);
        RpcStatus status = notify(context, request);
        return status.isOk();
    }
};

bool testConstructor() {
    {
        RpcClient test((RpcClientContext(_endpoint)));
        if (test.connected()) {
            return false;
        }
    }

    return true;
}

bool testConnect() {
    {
        RpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
    }
    {
        RpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect(true)) {
            return false;
        }
    }

    return true;
}

bool testInvoke() {
    {
        TestRpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
        String request = "中文Abc123";
        String response = test.sayHello(request);
        String expect = String::format("Hello %s!", request.c_str());
        if (expect != response) {
            return false;
        }
    }
    {
        TestRpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
        String request = "中文Abc123";
        if (!test.sayHello2(request)) {
            return false;
        }
    }
    {
        TestRpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
        String request = "中文Abc123";
        if (!test.sayHello3(request)) {
            return false;
        }
    }

    {
        TestRpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
        IntArray request = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        String response = test.sayHelloMultiPacket(request);
        if (response != "Hello 1-10!") {
            return false;
        }
    }
    {
        TestRpcClient test((RpcClientContext(_endpoint)));
        if (!test.connect()) {
            return false;
        }
        IntArray request = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        if (!test.sayHello2MultiPacket(request)) {
            return false;
        }
    }

    return true;
}

int main() {
    Trace::enableConsoleOutput();
    Trace::enableFlushConsoleOutput();

    TestRpcServer server((RpcServerContext(Endpoint("any", g_port))));
    server.start();

//    if (!testConstructor()) {
//        return 1;
//    }
//    if (!testConnect()) {
//        return 2;
//    }
    if (!testInvoke()) {
        return 3;
    }

    return 0;
}