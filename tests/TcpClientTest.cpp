//
//  TimeSpanTest.cpp
//  common
//
//  Created by baowei on 2023/1/6.
//  Copyright © 2023 com. All rights reserved.
//

#include "net/TcpClient.h"
#include "net/TcpServer.h"

using namespace Net;

void setUp() {

}

void cleanUp() {

}

bool testConstructor() {
    {
        TcpClient test;
    }

    return true;
}

int main() {
    setUp();

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }

    cleanUp();

    return result;
}