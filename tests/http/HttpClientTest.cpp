//
//  HttpClientTest.cpp
//  common
//
//  Created by baowei on 2022/10/18.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "http/HttpClient.h"
#include "http/HttpServer.h"
#include "thread/TickTimeout.h"
#include "IO/Directory.h"

using namespace Http;

int _serverPort = 8059;
String _downloadFileName = Path::combine(Path::getTempPath(), "download_source_test.txt");

void cleanUp() {
    if(File::exists(_downloadFileName)) {
        File::deleteFile(_downloadFileName);
    }
}

static HttpStatus onAction(void* parameter, const HttpRequest& request, HttpResponse& response) {
    if(request.method == HttpMethod::Get) {
        if (request.match("get/test")) {
            response.headers.add("Content-Type", "text/plain");
            response.setContent("test abc.");
        } else if (request.match("get/test2")) {
            response.headers.add("Content-Type", "application/octet-stream");
            auto ms = new MemoryStream();
            ms->writeByte(1);
            ms->writeByte(2);
            ms->writeByte(3);
            ms->writeByte(4);
            ms->writeByte(5);
            ms->seek(0, SeekOrigin::SeekBegin);
            response.setContent(ms);
        } else if (request.match("get/test3")) {
            response.headers.add("Content-Type", "application/json");
            JsonNode node;
            node.add(JsonNode("test1", 1));
            node.add(JsonNode("test2", "abc"));
            node.add(JsonNode("test3", true));
            response.setContent(node);
        } else if (request.match("download/test")) {
            response.headers.add("Content-Type", "application/octet-stream");
            FileStream fs(_downloadFileName, FileMode::FileCreate, FileAccess::FileWrite);
            fs.writeText("abc123");
            fs.close();
            response.setContent(new FileStream(_downloadFileName, FileMode::FileOpenWithoutException, FileAccess::FileRead));
        }
    } else if(request.method == HttpMethod::Post) {
        if (request.match("post/test")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "abc") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("bcd");
            }
        } else if (request.match("post/test2")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "bcd") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("cde");
            }
        } else if (request.match("post/test3")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "cde") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("def");
            }
        }
    } else if(request.method == HttpMethod::Put) {
        if (request.match("put/test")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "abc") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("bcd");
            }
        } else if (request.match("put/test2")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "bcd") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("cde");
            }
        } else if (request.match("put/test3")) {
            auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
            const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
            if(body == "cde") {
                response.headers.add("Content-Type", "text/plain");
                response.setContent("def");
            }
        } else if (request.match("upload/test")) {
            auto streamContent = dynamic_cast<HttpStreamContent*>(request.content);
            if(streamContent != nullptr) {
                streamContent->stream()->seek(0, SeekOrigin::SeekBegin);
                String str;
                streamContent->stream()->readToEnd(str);
                response.headers.add("Content-Type", "text/plain");
                response.setContent(str + "456");
            }
        }
    }
    return HttpStatus::HttpOk;
}

bool _finished = false;
bool testGet() {
    Url baseUrl("http", Endpoint("127.0.0.1", _serverPort));
    HttpHeaders textHeaders({HttpHeader("Content-Type", "text/plain")});
    HttpHeaders jsonHeaders({HttpHeader("Content-Type", "application/json")});
    HttpHeaders streamHeaders({HttpHeader("Content-Type", "application/octet-stream")});
    HttpClient client;

//    String response;
//    if(!client.get(Url(baseUrl, "get/test"), textHeaders, response)) {
//        return false;
//    }
//    if(response != "test abc.") {
//        return false;
//    }
//
//    ByteArray response2;
//    if(!client.get(Url(baseUrl, "get/test2"), streamHeaders, response2)) {
//        return false;
//    }
//    if(response2.count() != 5) {
//        return false;
//    }
//    if(!(response2[0] == 1 && response2[1] == 2 && response2[2] == 3 && response2[3] == 4 && response2[4] == 5)) {
//        return false;
//    }

    JsonNode response3;
    if(!client.get(Url(baseUrl, "get/test3"), jsonHeaders, response3)) {
        return false;
    }
    JsonNode node;
    if(!JsonNode::parse("{\"test1\":1,\"test2\":\"abc\",\"test3\":true}", node)) {
        return false;
    }
    if(response3 != node) {
        return false;
    }

    _finished = false;
    client.getAsync(Url(baseUrl, "get/test"), textHeaders, [](HttpResponse& request) {
        auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
        const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
        _finished = body == "test abc.";
    });
    TickTimeout::delay(TimeSpan::fromSeconds(3), [](void*) {
        return _finished;
    });
    if(!_finished) {
        return false;
    }

    return true;
}

bool testPost() {
    Url baseUrl("http", Endpoint("127.0.0.1", _serverPort));
    HttpHeaders textHeaders({HttpHeader("Content-Type", "text/plain")});
    HttpHeaders streamHeaders({HttpHeader("Content-Type", "application/octet-stream")});
    HttpClient client;

    String request = "abc";
    String response;
    if(!client.post(Url(baseUrl, "post/test"), textHeaders, request, response)) {
        return false;
    }
    if(response != "bcd") {
        return false;
    }

    String request2 = "bcd";
    if(!client.post(Url(baseUrl, "post/test2"), textHeaders, request)) {
        return false;
    }

    _finished = false;
    String request3 = "cde";
    client.postAsync(Url(baseUrl, "post/test3"), textHeaders, request3, [](HttpResponse& request) {
        auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
        const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
        _finished = body == "def";
    });
    TickTimeout::delay(TimeSpan::fromSeconds(3), [](void*) {
        return _finished;
    });
    if(!_finished) {
        return false;
    }

    return true;
}

bool testPut() {
    Url baseUrl("http", Endpoint("127.0.0.1", _serverPort));
    HttpHeaders textHeaders({HttpHeader("Content-Type", "text/plain")});
    HttpHeaders streamHeaders({HttpHeader("Content-Type", "application/octet-stream")});
    HttpClient client;

    String request = "abc";
    String response;
    if(!client.put(Url(baseUrl, "put/test"), textHeaders, request, response)) {
        return false;
    }
    if(response != "bcd") {
        return false;
    }

    String request2 = "bcd";
    if(!client.put(Url(baseUrl, "put/test2"), textHeaders, request)) {
        return false;
    }

    _finished = false;
    String request3 = "cde";
    client.putAsync(Url(baseUrl, "put/test3"), textHeaders, request3, [](HttpResponse& request) {
        auto stringContent = dynamic_cast<HttpStringContent*>(request.content);
        const String& body = stringContent != nullptr ? stringContent->value() : String::Empty;
        _finished = body == "def";
    });
    TickTimeout::delay(TimeSpan::fromSeconds(10), [](void*) {
        return _finished;
    });
    if(!_finished) {
        return false;
    }

    return true;
}

bool testDownload() {
    Url baseUrl("http", Endpoint("127.0.0.1", _serverPort));
    HttpHeaders textHeaders({HttpHeader("Content-Type", "text/plain")});
    HttpHeaders streamHeaders({HttpHeader("Content-Type", "application/octet-stream")});
    HttpClient client;

    String fileName = Path::combine(Path::getTempPath(), "download_target_test.txt");
    if(!client.download(Url(baseUrl, "download/test"), textHeaders, fileName)) {
        return false;
    }
    if(!File::exists(fileName)) {
        return false;
    }
    FileStream fs(fileName, FileMode::FileOpenWithoutException, FileAccess::FileRead);
    String text;
    fs.readToEnd(text);
    fs.close();
    File::deleteFile(fileName);
    if(text != "abc123") {
        return false;
    }

    return true;
}

bool testUpload() {
    Url baseUrl("http", Endpoint("127.0.0.1", _serverPort));
    HttpHeaders streamHeaders({HttpHeader("Content-Type", "application/octet-stream")});
    HttpClient client;

    String fileName = Path::combine(Path::getTempPath(), "upload_source_test.txt");
    String response;
    FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileWrite);
    fs.writeText("123abc");
    fs.close();
    bool result = client.upload(Url(baseUrl, "upload/test"), streamHeaders, fileName, response);
    if(File::exists(fileName)) {
        File::deleteFile(fileName);
    }
    if(response != "123abc456") {
        return false;
    }
    return result;
}

int main() {
    // start a web server.
    HttpServer server;
    HttpServer::Context context(Endpoint("any", _serverPort));
    HttpServer::Actions actions(nullptr, onAction);
    server.startHttpServer(context, actions);

    auto func = [](HttpServer *server) {
        return server->isHttpServerAlive();
    };
    Thread::delay(1500, Func<bool>(func, &server));

    int result = 0;
    if(!testGet()) {
        result = 1;
    }
    if(!testPost()) {
        result = 2;
    }
    if(!testPut()) {
        result = 3;
    }
    if(!testDownload()) {
        result = 4;
    }
    if(!testUpload()) {
        result = 5;
    }

    cleanUp();

    return result;
}