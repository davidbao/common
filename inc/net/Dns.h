//
//  Dns.h
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#ifndef Dns_h
#define Dns_h

#include "data/Vector.h"
#include "data/StringMap.h"
#include "data/String.h"
#include "data/Dictionary.h"

#ifdef WIN32
#include <winsock2.h>
#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <csignal>
#include <net/if.h>
#include <netinet/tcp.h>

#endif

using namespace Data;

namespace Net {
    class Dns {
    public:
        static String getHostName();

        static bool getHostNames(StringArray &hostNames);

        static bool getHostByName(const String &host, struct sockaddr_in &sin,
                                  const String &dnsServer = "114.114.114.114");

    private:
        Dns();

        ~Dns();

    private:
        static bool getHostByNameByApp(const char *host, String &address);

        static bool getHostByNameByNslookup(const char *host, String &address,
                                            const char *dnsServer = "114.114.114.114");

    private:
        static StringMap _hostNames;
    };
}

#endif // Dns_h
