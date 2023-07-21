//
//  NetInterfaceTest.cpp
//  common
//
//  Created by baowei on 2023/7/14.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "net/NetInterface.h"
#include "data/String.h"
#include "net/NetType.h"

using namespace Net;
using namespace Data;

String getLoopFaceName() {
#ifndef WIN32
    String lo = "lo";
    StringArray ifaces;
    NetInterface::getInterfaceNames(ifaces);
    for (int i = 0; i < ifaces.count(); ++i) {
        const String &iface = ifaces[i];
        if (iface.find("lo") >= 0) {
            lo = iface;
            break;
        }
    }
    return lo;
#else
    // DO NOT SUPPORT LOOPBACK ADAPTER.
    return String::Empty;
#endif
}

String getFirstFaceName() {
    String iface;
    StringArray ifaces;
    NetInterface::getInterfaceNames(ifaces);
    for (int i = 0; i < ifaces.count(); ++i) {
        if (ifaces[i].find("e") >= 0) {
            iface = ifaces[i];
            break;
        }
    }
    return iface;
}

bool tetGetMacAddresses() {
    {
        MacAddresses addresses;
        if (!NetInterface::getMacAddresses(addresses)) {
            return false;
        }
        if (addresses.count() == 0) {
            return false;
        }
    }

    return true;
}

bool testGetIPAddress() {
#ifndef WIN32
    {
        String lo = getLoopFaceName();
        String iface = NetInterface::getIpAddress(lo);
        if (iface != "127.0.0.1") {
            return false;
        }
    }
#endif

    return true;
}

bool testPing() {
    {
        if (!NetInterface::ping("127.0.0.1")) {
            return false;
        }
        if (!NetInterface::ping("127.0.0.1", 5)) {
            return false;
        }

#ifndef  MAC_OS
        String lo = getLoopFaceName();
        if (!NetInterface::ping("127.0.0.1", lo)) {
            return false;
        }
        if (!NetInterface::ping("127.0.0.1", lo, 5)) {
            return false;
        }
#endif
    }

    return true;
}

bool testGetMaskAddress() {
#ifndef WIN32
    {
        String lo = getLoopFaceName();
        IPAddress address = NetInterface::getMaskAddress(lo);
        if (address != IPAddress("255.0.0.0")) {
            return false;
        }
    }
#endif

    return true;
}

bool testGetMacAddress() {
#ifndef WIN32
    {
        String iface = getFirstFaceName();
        printf("testGetMacAddress.iface: %s\n", iface.c_str());
        MacAddress address = NetInterface::getMacAddress(iface);
        if (address.isEmpty()) {
            return false;
        }
    }
#endif

    return true;
}

bool testGetInterfaceNames() {
#ifndef WIN32
    {
        String lo;
        StringArray ifaces;
        NetInterface::getInterfaceNames(ifaces);
        for (int i = 0; i < ifaces.count(); ++i) {
            const String &iface = ifaces[i];
            if (iface.find("lo") >= 0) {
                lo = iface;
                break;
            }
        }
        if (lo.isNullOrEmpty()) {
            return false;
        }
        if (ifaces.count() <= 1) {
            return false;
        }
    }
#endif

    return true;
}

int main() {
    if (!tetGetMacAddresses()) {
        return 1;
    }
    if (!testPing()) {
        return 2;
    }
    if (!testGetIPAddress()) {
        return 3;
    }
    if (!testGetMaskAddress()) {
        return 4;
    }
    if (!testGetMacAddress()) {
        return 5;
    }
    if (!testGetInterfaceNames()) {
        return 6;
    }

    return 0;
}