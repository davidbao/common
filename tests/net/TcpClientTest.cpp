//
//  TcpClientTest.cpp
//  common
//
//  Created by baowei on 2023/1/6.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "net/TcpClient.h"
#include "net/TcpServer.h"
#include "thread/Timer.h"

using namespace Net;
using namespace Threading;

int _serverPort = 8058;
String _serverHost = "127.0.0.1";
String _sendingStr = "testAbc123中文";
String _receivingStr = "中文testAbc123";

void setUp() {

}

void cleanUp() {

}

bool testConstructor() {
    {
        TcpClient test;
        if (test.socketId() != -1) {
            return false;
        }
    }

    return true;
}

bool testConnect() {
    {
        TcpClient test;
        if (!test.connectToHost(_serverHost, _serverPort)) {
            return false;
        }
    }
    {
        TcpClient test;
        if (!test.connectToHost(_serverHost, _serverPort, 2000)) {
            return false;
        }
    }
    {
        TcpClient test;
        if (!test.connectToHost(Endpoint(_serverHost, _serverPort))) {
            return false;
        }
    }
    {
        TcpClient test;
        if (!test.connectToHost(Endpoint(_serverHost, _serverPort), TimeSpan::fromSeconds(2))) {
            return false;
        }
    }

    return true;
}

bool testSendAndReceive() {
    {
        TcpClient test;
        if (!test.connectToHost(_serverHost, _serverPort)) {
            return false;
        }

        ssize_t length = test.send(_sendingStr);
        if (length != _sendingStr.length()) {
            return false;
        }
        String str;
        if (test.receiveBySize(&str, _receivingStr.length(), 1000) <= 0) {
            return false;
        }
        if (str != _receivingStr) {
            return false;
        }
    }

    return true;
}

int main() {
    // start a tcp server.
    TcpServer server;
    if (!server.bind("any", _serverPort)) {
        return -1;
    }
    server.listen();
    auto acceptProc = [](TcpServer *ts) {
        TcpClient *tc = ts->accept();
        if (tc != nullptr) {
            tc->send(_receivingStr);
        }
    };
    Timer test("accept.timer", 100, acceptProc, &server);

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testConnect()) {
        result = 2;
    }
    if (!testSendAndReceive()) {
        result = 3;
    }

    cleanUp();

    return result;
}