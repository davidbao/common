//
//  NetInterface.h
//  common
//
//  Created by baowei on 31/03/2017.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef NetInterface_h
#define NetInterface_h

#include "data/String.h"
#include "net/NetType.h"
#include "data/TimeSpan.h"
#include "data/StringArray.h"
#include "data/Vector.h"

namespace Net {
    class NetInterface {
    public:
        struct Data {
            long recevieBytes;
            long transmitBytes;
        };

        struct Client : IEquatable<Client> {
        public:
            enum State {
                // the neighbour entry is valid forever and can be
                // only be removed administratively.
                PERMANENT,
                // the neighbour entry is valid. No attempts to
                // validate this entry will be made but it can be
                // removed when its lifetime expires.
                NOARP,
                // the neighbour entry is valid until the
                // reachability timeout expires.
                REACHABLE,
                // the neighbour entry is valid but suspicious.
                // This option to ip neigh does not change the
                // neighbour state if it was valid and the address
                //     is not changed by this command.
                STALE,
                // this is a pseudo state used when initially
                // creating a neighbour entry or after trying to
                // remove it before it becomes free to do so.
                NONE,
                // the neighbour entry has not (yet) been
                // validated/resolved.
                INCOMPLETE,
                // neighbor entry validation is currently delayed.
                DELAY,
                // neighbor is being probed.
                PROBE,
                // max number of probes exceeded without success,
                // neighbor validation has ultimately failed.
                FAILED
            };

            IPAddress address;
            MacAddress macAddress;
            State state;

        public:
            String stateStr() const;

            bool equals(const Client &other) const override;
        };

        typedef Vector<Client> Clients;

        static bool getMacAddresses(MacAddresses &addresses);

        static bool ping(const String &ipAddress, const String &iface = String::Empty, int detectionCount = 2);

        static bool getClientList(const String &iface, Clients &clients);

        static bool isClientEnable(const IPAddress &address);

        static bool enableClient(const IPAddress &address, bool enable);

        static String toClientStateStr(Client::State state);

        static const IPAddress getIpAddress(const String &iface);

        static const IPAddress getMaskAddress(const String &iface);

#ifndef __ANDROID__
#ifdef __linux__
        static const IPAddress getGatewayAddress(const String& iface);
        static bool getData(const String& iface, Data& data);
#endif    // __linux__
#endif    // __ANDROID__

        static const MacAddress getMacAddress(const String &iface);

        static bool isInterfaceUp(const String &iface);

        static void getInterfaceNames(StringArray &names);
    };
}

#endif /* NetInterface_h */
