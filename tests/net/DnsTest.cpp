//
//  DnsTest.cpp
//  common
//
//  Created by baowei on 2023/7/14.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "net/Dns.h"
#include "data/String.h"

using namespace Net;
using namespace Data;

bool tetGetHostName() {
    {
        String name = Dns::getHostName();
        if (name == String::Empty) {
            return false;
        }
    }
    {
        StringArray names;
        if (!Dns::getHostNames(names)) {
            return false;
        }
        if (names.count() == 0) {
            return false;
        }
    }
    {
        String host = "127.0.0.1";
        int port = 10032;
        struct sockaddr_in sin{};     /* an Internet endpoint address  */

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        if ((sin.sin_port = htons(port)) == 0) {
            return false;
        }
        if (!Dns::getHostByName(host, sin)) {
            return false;
        }
    }

    return true;
}

bool testGetHostAddress() {
    IPAddresses addresses;
    Dns::getHostAddresses(addresses);
    if (addresses.count() == 0) {
        return false;
    }
    return true;
}

int main() {
    if (!tetGetHostName()) {
        return 1;
    }
#ifndef WIN32
    if (!testGetHostAddress()) {
        return 2;
    }
#endif

    return 0;
}