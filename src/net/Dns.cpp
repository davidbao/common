//
//  Dns.cpp
//  common
//
//  Created by baowei on 2018/12/10.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "net/Dns.h"
#include "diag/Trace.h"
#include "exception/Exception.h"
#include <cerrno>

#ifdef WIN32
#include<Winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef __ANDROID__

#include <ifaddrs.h>

#endif
#endif

#include "net/TcpClient.h"
#include "thread/Process.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "data/Dictionary.h"

#ifdef WIN32
#undef errno
#define errno WSAGetLastError()
#else
#endif

using namespace Diag;

namespace Net {
    StringMap Dns::_hostNames;

    Dns::Dns() = default;

    Dns::~Dns() = default;

    String Dns::getHostName() {
#ifdef WIN32
        TcpClient::initializeSocket();
#endif
        char hostName[1024];
        if (gethostname(hostName, sizeof(hostName)) == 0) {
            return hostName;
        } else {
            Debug::writeFormatLine("can't get host name: %s", strerror(errno));
        }
        return String::Empty;
    }

    bool Dns::getHostNames(StringArray &hostNames) {
        TcpClient::initializeSocket();

#ifdef WIN32
        struct hostent *remoteHost = gethostbyname(getHostName().c_str());
        if (remoteHost != nullptr)
        {
            if(remoteHost->h_addrtype == AF_INET)
            {
                int i = 0;
                struct in_addr addr;
                while (remoteHost->h_addr_list[i] != 0)
                {
                    addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
                    hostNames.add(inet_ntoa(addr));
                }
                return true;
            }
        }
        else
        {
            Debug::writeFormatLine("can't get host names: %s", strerror(errno));
        }
        return false;
#else
#ifndef __ANDROID__
        struct ifaddrs *ifAddrStruct = nullptr;
        void *tmpAddrPtr;

        getifaddrs(&ifAddrStruct);

        while (ifAddrStruct != nullptr) {
            if (ifAddrStruct->ifa_addr != nullptr &&
                ifAddrStruct->ifa_addr->sa_family == AF_INET) {   // check it is IP4
                // is a valid IP4 Address
                tmpAddrPtr = &((struct sockaddr_in *) ifAddrStruct->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                hostNames.add(addressBuffer);
//                printf("%s IPV4 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
            }
//            else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6)
//            {   // check it is IP6
//                // is a valid IP6 Address
//                tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
//                char addressBuffer[INET6_ADDRSTRLEN];
//                inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
//                printf("%s IPV6 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);
//            }
            ifAddrStruct = ifAddrStruct->ifa_next;
        }
        return true;
#endif  // __ANDROID__
        return false;
#endif  // WIN32
    }

#ifndef WIN32

    bool Dns::getHostByNameByApp(const char *host, String &address) {
        Process process;
        process.setRedirectStdout(true);
        process.setWaitingTimeout(1000);    // 1s.
        String fileName = Path::combine(Path::getAppPath(), "gethostbyname");
//        Trace::writeFormatLine("getHostByName_app, fileName: %s, host: %s", fileName.c_str(), host);
        if (File::exists(fileName)) {
            Process::start(fileName, String(host), &process);
            const String &str = process.stdoutStr();
//            Trace::writeFormatLine("gethostbyname: %s", str.c_str());
            ssize_t start = str.find(String::NewLine);
            if (start > 0) {
                String ipAddrStr = str.substr(0, start);
                IPAddress ipAddr;
                if (IPAddress::parse(ipAddrStr, ipAddr)) {
                    address = ipAddrStr;
                    return true;
                }
            }
        }
        return false;
    }

    bool Dns::getHostByNameByNslookup(const char *host, String &address, const char *dnsServer) {
//        nslookup baidu.com
//        Server:        114.114.114.114
//        Address:    114.114.114.114#53
//
//        Non-authoritative answer:
//        Name:    baidu.com
//        Address: 220.181.38.251
//        Name:    baidu.com
//        Address: 220.181.38.148

        Process process;
        process.setRedirectStdout(true);
        process.setWaitingTimeout(1000);    // 5s.
        Process::start((String) "nslookup", String::convert("%s %s", host, dnsServer), &process);
        const String &str = process.stdoutStr();
        ssize_t start = str.find(host);
        if (start > 0) {
            String temp = str.substr(start + 1, str.length());
            ssize_t end = temp.find(':');
            if (end > 0) {
                String ipAddrStr = String::trim(temp.substr(end + 1, str.length()), '\r', '\n', ' ');
                IPAddress ipAddr;
                if (IPAddress::parse(ipAddrStr, ipAddr)) {
                    //                Debug::writeFormatLine("Dns::getHostByName4.true, host: %s, time: %s", host, DateTime::now().toString().c_str());
                    address = ipAddrStr;
                    return true;
                }
            }
        }
        return false;
    }

#endif

    bool Dns::getHostByName(const String &host, struct sockaddr_in &sin, const String &dnsServer) {
        if (host.isNullOrEmpty())
            return false;

//        Debug::writeFormatLine("Dns::getHostByName1, host: %s, time: %s", host, DateTime::now().toString().c_str());
        struct hostent *phe;        /* pointer to host information entry    */
        /* Map host name to IP address, allowing for dotted decimal */
        if ((phe = gethostbyname(host)) != nullptr) {
//            Debug::writeFormatLine("Dns::getHostByName2.true, host: %s, time: %s", host, DateTime::now().toString().c_str());
            memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
            return true;
        }
        //else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
        //    Debug::writeFormatLine("can't get \"%s\" host entry", host);

#ifndef WIN32
        String ipAddrStr;
        if (_hostNames.at(host, ipAddrStr) && !ipAddrStr.isNullOrEmpty()) {
            sin.sin_addr.s_addr = inet_addr(ipAddrStr.c_str());
            return true;
        }

#ifdef __arm_linux__
        if(getHostByNameByApp(host, ipAddrStr))
        {
            sin.sin_addr.s_addr = inet_addr(ipAddrStr.c_str());
            _hostNames.add(host, ipAddrStr);
            return true;
        }
#else
        if (getHostByNameByNslookup(host, ipAddrStr, dnsServer)) {
            sin.sin_addr.s_addr = inet_addr(ipAddrStr.c_str());
            _hostNames.add(host, ipAddrStr);
            return true;
        }
#endif
#endif
        return false;
    }
}
