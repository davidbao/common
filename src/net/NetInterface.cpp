//
//  NetInterface.cpp
//  common
//
//  Created by baowei on 31/03/2017.
//  Copyright © 2017 com. All rights reserved.
//

#if WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#include <WSPiApi.h>
#include "exception/Exception.h"
#include <iphlpapi.h>
#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <net/if.h>
#include <netinet/tcp.h>

#ifndef __ANDROID__

#include <ifaddrs.h>

#ifdef __linux__
#include <linux/rtnetlink.h>
#include <fcntl.h>
#endif
#endif
#ifdef AF_LINK

#include <net/if_dl.h>

#endif
#ifdef AF_PACKET
#include <netpacket/packet.h>
#endif
#endif

#include <errno.h>
#include "net/NetInterface.h"
#include "diag/Trace.h"
#include "system/Math.h"
#include "system/Convert.h"
#include "data/DateTime.h"
#include "thread/Process.h"
#include "thread/TickTimeout.h"
#include "net/Dns.h"
#include "net/TcpClient.h"

#ifdef DEBUG

#include "system/Application.h"

#endif

#if WIN32
#undef errno
#define errno WSAGetLastError()
#define ioctl(s, cmd, argp) ioctlsocket(s, cmd, argp)
#ifndef socklen_t
#define socklen_t int
#endif
#define SHUT_RDWR SD_BOTH
#else
#define closesocket(a) ::close(a)
#define SUPPORT_IP6
#endif

namespace Common {
    String NetInterface::Client::stateStr() const {
        return NetInterface::toClientStateStr(state);
    }

    bool NetInterface::Client::equals(const Client &other) const {
        return address == other.address && macAddress == other.macAddress && state == other.state;
    }

    bool NetInterface::getMacAddresses(MacAddresses &addresses) {
#if WIN32
#define MAX_TRIES 3
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

        int i = 0;
        // default to unspecified address family (both)
        ULONG family = AF_INET;

        PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
        ULONG outBufLen = 0;
        ULONG Iterations = 0;

        // Set the flags to pass to GetAdaptersAddresses
        ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
        PIP_ADAPTER_ADDRESSES pCurrAddresses = nullptr;
        DWORD dwRetVal = 0;
        do
        {
            pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
            if (pAddresses == nullptr)
            {
                printf
                ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
                exit(1);
            }

            dwRetVal = GetAdaptersAddresses(family, flags, nullptr, pAddresses, &outBufLen);

            if (dwRetVal == ERROR_BUFFER_OVERFLOW)
            {
                FREE(pAddresses);
                pAddresses = nullptr;
            }
            else {
                break;
            }

            Iterations++;

        } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

        if (dwRetVal == NO_ERROR)
        {
            // If successful, output some information from the data we received
            pCurrAddresses = pAddresses;
            while (pCurrAddresses)
            {
                if (pCurrAddresses->PhysicalAddressLength == (ULONG)MacAddress::Count)
                {
                    MacAddress addr(pCurrAddresses->PhysicalAddress);
                    addresses.add(addr);
                }

                pCurrAddresses = pCurrAddresses->Next;
            }
        }

        if (pAddresses)
        {
            FREE(pAddresses);
        }
#else
#ifndef __ANDROID__
        // Head of the interface address linked list
        ifaddrs *ifap = nullptr;

        int r = getifaddrs(&ifap);

        if (r != 0) {
            // Non-zero return code means an error
            printf("return code = %d\n", r);
        }

        ifaddrs *current = ifap;

        if (current == nullptr) {
            printf("No interfaces found\n");
        }

        while (current != nullptr) {
            //            const sockaddr_in* interfaceAddress = castToIP4(current->ifa_addr);
            //            const sockaddr_in* broadcastAddress = castToIP4(current->ifa_dstaddr);
            //            const sockaddr_in* subnetMask       = castToIP4(current->ifa_netmask);

            //            printf("Interface %s", current->ifa_name);
            //            if (current->ifa_addr != nullptr) {
            //                printf(" %s", levelToString(current->ifa_addr->sa_family));
            //            }
            //            printf("\nStatus    = %s\n", (current->ifa_flags & IFF_UP) ? "Online" : "Down");

            // The MAC address and the interfaceAddress come in as
            // different interfaces with the same name.

            uint8_t sa_family = 0;
#ifdef AF_LINK
            sa_family = AF_LINK;
#endif
#ifdef AF_PACKET
            sa_family = AF_PACKET;
#endif
            if ((current->ifa_addr != nullptr) && (current->ifa_addr->sa_family == sa_family)) {
#if __linux__ || __EMSCRIPTEN__ || MSYS
                // Linux
                struct ifreq ifr;

                int fd = socket(AF_INET, SOCK_DGRAM, 0);

                ifr.ifr_addr.sa_family = AF_INET;
                strcpy(ifr.ifr_name, current->ifa_name);
                ioctl(fd, SIOCGIFHWADDR, &ifr);
                ::close(fd);

                uint8_t* MAC = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);

#else
                // Posix/FreeBSD/MacOS
                sockaddr_dl *sdl = (struct sockaddr_dl *) current->ifa_addr;
                uint8_t *MAC = reinterpret_cast<uint8_t *>(LLADDR(sdl));
#endif
                MacAddress addr(MAC);
                addresses.add(addr);
            }

            current = current->ifa_next;
        }

        freeifaddrs(ifap);
#endif
#endif
        return addresses.count() > 0;
    }

    bool NetInterface::ping(const String &ipAddress, const String &iface, int detectionCount) {
        if (ipAddress.isNullOrEmpty())
            return false;

        if (detectionCount < 1)
            detectionCount = 1;
        if (detectionCount > 10)
            detectionCount = 10;

        Process process;
        process.setRedirectStdout(true);
        process.setWaitingTimeout((detectionCount + 1) * 1000);
#ifdef DEBUG
        process.showChildLog = false;
#endif
        String para;
        if (iface.isNullOrEmpty())
            para = String::convert("-c %d %s", detectionCount, ipAddress.c_str());
        else
            para = String::convert("-c %d -I %s %s", detectionCount, iface.c_str(), ipAddress.c_str());
        Process::start((String) "ping", para, &process);
        const String &str = process.stdoutStr();
        const String &transmittedStr = "packets transmitted";
        ssize_t start = str.find(transmittedStr);
        if (start > 0) {
            start += transmittedStr.length();
            int end = str.find("packets received");
            if (end > start) {
                String countStr = str.substr(start, end - start);
                countStr = countStr.trim(' ', ',');
                int count;
                return Int32::parse(countStr, count) && count > 0;
            }
        }
        return false;
    }

    const IPAddress NetInterface::getIpAddress(const String &iface) {
#ifdef WIN32
        return IPAddress::Empty;
#elif !__ANDROID__
        struct ifaddrs *ifap, *ifa;
        struct sockaddr_in *sa;
        String addr;

        getifaddrs(&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr != nullptr && ifa->ifa_addr->sa_family == AF_INET && iface == ifa->ifa_name) {
                sa = (struct sockaddr_in *) ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                break;
//                printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
            }
        }
        freeifaddrs(ifap);

        IPAddress address;
        IPAddress::parse(addr, address);
        return address;
#else
        int fd;
        struct ifreq ifr;

        // only support IPV4.
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        /* I want to get an IPv4 IP address */
        ifr.ifr_addr.sa_family = AF_INET;

        /* I want IP address attached to "eth0" */
        strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);

        ioctl(fd, SIOCGIFADDR, &ifr);

        closesocket(fd);

        String str = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        IPAddress address;
        IPAddress::parse(str, address);
        return address;
#endif
    }

    const IPAddress NetInterface::getMaskAddress(const String &iface) {
#ifdef WIN32
        return IPAddress::Empty;
#else
        int fd;
        struct ifreq ifr;

        // only support IPV4.
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        /* I want to get an IPv4 IP address */
        ifr.ifr_addr.sa_family = AF_INET;

        /* I want IP address attached to "eth0" */
        strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

        ioctl(fd, SIOCGIFNETMASK, &ifr);

        closesocket(fd);

        String str = inet_ntoa(((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr);
        IPAddress address;
        IPAddress::parse(str, address);
        return address;
#endif
    }

    bool NetInterface::isInterfaceUp(const String &iface) {
#ifdef WIN32
        return false;
#else
        struct ifreq ifr;
        int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, iface.c_str());
        if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
            //            Debug::writeLine("TcpClient::isInterfaceUp: SIOCGIFFLAGS");
        }
        closesocket(fd);
        return !!(ifr.ifr_flags & IFF_UP);
#endif
    }

    const MacAddress NetInterface::getMacAddress(const String &iface) {
        MacAddress result;
#ifdef WIN32
        return result;
#elif !__ANDROID__
        // Head of the interface address linked list
        ifaddrs *ifap = nullptr;

        int r = getifaddrs(&ifap);

        if (r != 0) {
            // Non-zero return code means an error
            printf("return code = %d\n", r);
        }

        ifaddrs *current = ifap;

        if (current == nullptr) {
            printf("No interfaces found\n");
        }

        while (current != nullptr) {
            //            const sockaddr_in* interfaceAddress = castToIP4(current->ifa_addr);
            //            const sockaddr_in* broadcastAddress = castToIP4(current->ifa_dstaddr);
            //            const sockaddr_in* subnetMask       = castToIP4(current->ifa_netmask);

            //            printf("Interface %s", current->ifa_name);
            //            if (current->ifa_addr != nullptr) {
            //                printf(" %s", levelToString(current->ifa_addr->sa_family));
            //            }
            //            printf("\nStatus    = %s\n", (current->ifa_flags & IFF_UP) ? "Online" : "Down");

            // The MAC address and the interfaceAddress come in as
            // different interfaces with the same name.

            uint8_t sa_family = 0;
#ifdef AF_LINK
            sa_family = AF_LINK;
#endif  // AF_LINK
#ifdef AF_PACKET
            sa_family = AF_PACKET;
#endif  // AF_PACKET
            if ((current->ifa_addr != nullptr) && (current->ifa_addr->sa_family == sa_family)) {
                if (iface == current->ifa_name) {
#if __linux__ || __EMSCRIPTEN__ || MSYS
                    // Linux
                    struct ifreq ifr;
                    
                    int fd = socket(AF_INET, SOCK_DGRAM, 0);
                    
                    ifr.ifr_addr.sa_family = AF_INET;
                    strcpy(ifr.ifr_name, current->ifa_name);
                    ioctl(fd, SIOCGIFHWADDR, &ifr);
                    ::close(fd);
                    
                    uint8_t* MAC = reinterpret_cast<uint8_t*>(ifr.ifr_hwaddr.sa_data);

#else
                    // Posix/FreeBSD/MacOS
                    sockaddr_dl *sdl = (struct sockaddr_dl *) current->ifa_addr;
                    uint8_t *MAC = reinterpret_cast<uint8_t *>(LLADDR(sdl));
#endif
                    result = MacAddress(MAC);
                    break;
                }
            }

            current = current->ifa_next;
        }

        freeifaddrs(ifap);
#endif  // __ANDROID__
        return result;
    }

#ifndef __ANDROID__
#ifdef __linux__
#define BUFSIZE 8192
    struct route_info
    {
        u_int dstAddr;
        u_int srcAddr;
        u_int gateWay;
        char ifName[IF_NAMESIZE];
    };
    int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
    {
        struct nlmsghdr *nlHdr;
        int readLen = 0, msgLen = 0;
        do{
            if((readLen = (int)::recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
            {
                perror("SOCK READ: ");
                return -1;
            }
            
            nlHdr = (struct nlmsghdr *)bufPtr;
            if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
            {
                perror("Error in recieved packet");
                return -1;
            }
            
            
            if(nlHdr->nlmsg_type == NLMSG_DONE)
            {
                break;
            }
            else
            {
                
                bufPtr += readLen;
                msgLen += readLen;
            }
            
            
            if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
            {
                
                break;
            }
        } while(((int)nlHdr->nlmsg_seq != seqNum) || ((int)nlHdr->nlmsg_pid != pId));
        return msgLen;
    }
    const String parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
    {
        struct rtmsg *rtMsg;
        struct rtattr *rtAttr;
        int rtLen;
        char *tempBuf = nullptr;
        struct in_addr dst;
        struct in_addr gate;
        
        tempBuf = (char *)malloc(100);
        rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
        // If the route is not for AF_INET or does not belong to main routing table
        //then return.
        if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
            return String::Empty;
        
        rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
        rtLen = RTM_PAYLOAD(nlHdr);
        for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
            switch(rtAttr->rta_type) {
                case RTA_OIF:
                    if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
                    break;
                case RTA_GATEWAY:
                    rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
                    break;
                case RTA_PREFSRC:
                    rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
                    break;
                case RTA_DST:
                    rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
                    break;
            }
        }
        
        char gateway[BUFSIZE];
        memset(gateway, 0, sizeof(gateway));
        dst.s_addr = rtInfo->dstAddr;
        if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
        {
//            printf("oif:%s",rtInfo->ifName);
            gate.s_addr = rtInfo->gateWay;
            sprintf(gateway, "%s", (char *)inet_ntoa(gate));
//            printf("%s\n",gateway);
            gate.s_addr = rtInfo->srcAddr;
//            printf("src:%s\n",(char *)inet_ntoa(gate));
            gate.s_addr = rtInfo->dstAddr;
//            printf("dst:%s\n",(char *)inet_ntoa(gate));
        }
        free(tempBuf);
        return gateway;
    }
    const IPAddress NetInterface::getGatewayAddress(const String& iface)
    {
        struct nlmsghdr *nlMsg;
//        struct rtmsg *rtMsg;
        struct route_info *rtInfo;
        char msgBuf[BUFSIZE];
        
        int sock, len, msgSeq = 0;
        
        if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
        {
            perror("Socket Creation: ");
            return IPAddress::Empty;
        }
        
        memset(msgBuf, 0, BUFSIZE);
        
        nlMsg = (struct nlmsghdr *)msgBuf;
//        rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
        
        nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
        nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
        
        nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
        nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
        nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
        
        
        if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
            printf("Write To Socket Failed…\n");
            return IPAddress::Empty;
        }
        
        
        if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
            printf("Read From Socket Failed…\n");
            return IPAddress::Empty;
        }
        
        String gateway;
        rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
        for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
            memset(rtInfo, 0, sizeof(struct route_info));
            gateway = parseRoutes(nlMsg, rtInfo);
            if(iface == rtInfo->ifName)
                break;
        }
        free(rtInfo);
        close(sock);
        IPAddress address;
        IPAddress::parse(gateway, address);
        return address;
    }
    
    //root@ok335x:~# cat /proc/net/dev
    //Inter-|   Receive                                                |  Transmit
    // face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    //    lo:    1344      12    0    0    0     0          0         0     1344      12    0    0    0     0       0          0
    //  usb0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
    //  eth1:    2533      30    0    2    0     0          0         0      452       5    0    0    0     0       0          0
    //  eth0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
    bool NetInterface::getData(const String& iface, Data& data)
    {
        uint nDevLen = iface.length();
        if (nDevLen < 1 || nDevLen > 100)
        {
            printf("dev length too long\n");
            return false;
        }
        int fd = ::open("/proc/net/dev", O_RDONLY | O_EXCL);
        if (-1 == fd)
        {
            printf("/proc/net/dev not exists!\n");
            return false;
        }
        
        char buf[1024*2];
        lseek(fd, 0, SEEK_SET);
        int nBytes = ::read(fd, buf, sizeof(buf)-1);
        if (-1 == nBytes)
        {
            printf("read /proc/net/dev error\n");
            close(fd);
            return false;
        }
        buf[nBytes] = '\0';
        char* pDev = strstr(buf, iface);
        if (nullptr == pDev)
        {
            printf("don't find dev %s\n", iface.c_str());
            close(fd);
            return false;
        }
        char *p;
        char *ifconfig_value;
        int i = 0;
        for (p = strtok(pDev, " \t\r\n"); p; p = strtok(nullptr, " \t\r\n"))
        {
            i++;
            ifconfig_value = (char*)malloc(32);
            strcpy(ifconfig_value, p);
            if(i == 2)
            {
                data.recevieBytes = atol(ifconfig_value);
            }
            if(i == 10)
            {
                data.transmitBytes = atol(ifconfig_value);
                break;
            }
            free(ifconfig_value);
        }
        close(fd);
        return true;
    }
#endif    // __linux__
#endif    // __ANDROID__

    void NetInterface::getInterfaceNames(StringArray &names) {
#if PC_OS
#ifdef WIN32
#else
        struct ifaddrs *addrs, *tmp;

        getifaddrs(&addrs);
        tmp = addrs;

        uint8_t sa_family = 0;
#ifdef AF_LINK
        sa_family = AF_LINK;
#endif
#ifdef AF_PACKET
        sa_family = AF_PACKET;
#endif

        while (tmp) {
            if (tmp->ifa_addr && tmp->ifa_addr->sa_family == sa_family) {
                names.add(tmp->ifa_name);
            }
            tmp = tmp->ifa_next;
        }

        freeifaddrs(addrs);
#endif // WIN32
#endif // PC_OS
    }

    bool NetInterface::getClientList(const String &iface, Clients &clients) {
        Process process;
        process.setRedirectStdout(true);
#ifdef DEBUG
        process.showChildLog = false;
#endif
        String para = String::convert("neigh show dev %s", iface.c_str());
        Process::start((String) "ip", para, &process);
        const String &str = process.stdoutStr();
//        192.168.2.100 lladdr 40:81:4e:9a:7a:9a DELAY
//        192.168.2.101 lladdr d0:e1:40:9a:71:da REACHABLE
//        String str = "192.168.2.100 lladdr 40:81:4e:9a:7a:9a DELAY\n192.168.2.101 lladdr d0:e1:40:9a:71:da REACHABLE";
        StringArray texts;
        StringArray::parse(str, texts, '\n');
        for (uint i = 0; i < texts.count(); i++) {
            const String &text = texts[i];
            StringArray items;
            StringArray::parse(text, items, ' ');
            if (items.count() == 4) {
                Client client;
                if (IPAddress::parse(items[0], client.address) &&
                    MacAddress::parse(items[2], client.macAddress)) {
                    const String &state = items[3];
                    if (String::equals(state, "PERMANENT", true))
                        client.state = Client::State::PERMANENT;
                    else if (String::equals(state, "NOARP", true))
                        client.state = Client::State::NOARP;
                    else if (String::equals(state, "REACHABLE", true))
                        client.state = Client::State::REACHABLE;
                    else if (String::equals(state, "STALE", true))
                        client.state = Client::State::STALE;
                    else if (String::equals(state, "NONE", true))
                        client.state = Client::State::NONE;
                    else if (String::equals(state, "INCOMPLETE", true))
                        client.state = Client::State::INCOMPLETE;
                    else if (String::equals(state, "DELAY", true))
                        client.state = Client::State::DELAY;
                    else if (String::equals(state, "PROBE", true))
                        client.state = Client::State::PROBE;
                    else if (String::equals(state, "FAILED", true))
                        client.state = Client::State::FAILED;
                    else
                        client.state = Client::State::NONE;

                    clients.add(client);
                }
            }
        }
        return true;
    }

    bool NetInterface::isClientEnable(const IPAddress &address) {
        Process process;
        process.setRedirectStdout(true);
#ifdef DEBUG
        process.showChildLog = false;
#endif
        String para = String::convert("route get %s", address.toString().c_str());
        Process::start((String) "ip", para, &process);
        const String &str = process.stdoutStr();
        return str.find(address.toString()) >= 0;

    }

    bool NetInterface::enableClient(const IPAddress &address, bool enable) {
        Process process;
        process.setRedirectStdout(true);
#ifdef DEBUG
        process.showChildLog = false;
#endif
        String para = enable ?
                      String::convert("route del prohibit %s", address.toString().c_str()) :
                      String::convert("route add prohibit %s", address.toString().c_str());
        Process::start((String) "ip", para, &process);
        const String &str = process.stdoutStr();
        return str.isNullOrEmpty();
    }

    String NetInterface::toClientStateStr(Client::State state) {
        switch (state) {
            case Client::State::PERMANENT:
                return "PERMANENT";
            case Client::State::NOARP:
                return "NOARP";
            case Client::State::REACHABLE:
                return "REACHABLE";
            case Client::State::STALE:
                return "STALE";
            case Client::State::NONE:
                return "NONE";
            case Client::State::INCOMPLETE:
                return "INCOMPLETE";
            case Client::State::DELAY:
                return "DELAY";
            case Client::State::PROBE:
                return "PROBE";
            case Client::State::FAILED:
                return "FAILED";

            default:
                return "NONE";
        }
    }
}
