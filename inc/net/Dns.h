#ifndef DNS_H
#define DNS_H

#include "data/Vector.h"
#include "data/StringMap.h"
#include "data/ValueType.h"
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
#include <signal.h>
#include <net/if.h>
#include <netinet/tcp.h>
#endif

namespace Common
{
	class Dns
	{
	public:
		static String getHostName();
		static bool getHostNames(StringArray& hostNames);

        static bool getHostByName(const String& host, struct sockaddr_in& sin, const String& dnsServer = "114.114.114.114");

	private:
        Dns();
        ~Dns();
        
    private:
        static bool getHostByNameByApp(const char* host, String& address);
        static bool getHostByNameByNslookup(const char* host, String& address, const char* dnsServer = "114.114.114.114");
        
    private:
        static StringMap _hostNames;
	};
}

#endif	// DNS_H
