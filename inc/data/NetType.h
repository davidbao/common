#ifndef NetType_h
#define NetType_h

#include "data/Vector.h"
#include "data/NetType.h"
#include "data/ValueType.h"

namespace Common {
    struct MacAddress {
    public:
        static const int Count = 6;
        uint8_t address[Count];

        MacAddress();

        MacAddress(uint64_t addr);

        MacAddress(const uint8_t addr[Count]);

        MacAddress(const String &addr);

        MacAddress(const MacAddress &addr);

        String toString() const;

        uint64_t toUInt64(bool bigEndian = true) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        MacAddress &operator=(const String &value);

        MacAddress &operator=(const MacAddress &value);

        bool operator==(const MacAddress &value) const;

        bool operator!=(const MacAddress &value) const;

        operator String() const;

        bool isEmpty() const;

        void empty();

    public:
        static bool parse(const String &str, MacAddress &address);

        static bool increase(const MacAddress &address, MacAddress &newAddress);

        static MacAddress randomLocalAddress(uint8_t oui1 = 0x08);

    public:
        static const MacAddress Empty;
    };

    class MacAddresses : public IIndexGetter<MacAddress> {
    public:
        MacAddresses();

        String toString(const char splitSymbol = ' ') const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        MacAddresses &operator=(const String &value);

        MacAddresses &operator=(const MacAddresses &value);

        bool operator==(const MacAddresses &value) const;

        bool operator!=(const MacAddresses &value) const;

        operator String() const;

        bool isEmpty() const;

        void empty();

        size_t count() const;

        MacAddress at(size_t pos) const override;

        void add(const MacAddress &address);

        void add(const String &address);

        void remove(const MacAddress &address);

        void remove(const String &address);

        void clear();

    public:
        static bool parse(const String &str, MacAddresses &value, const char splitSymbol = ' ');

    private:
        Vector<MacAddress> _addresses;
    };

    struct IPAddress {
    public:
        static const int Count = 4;
        uint8_t address[Count];

        IPAddress();

        IPAddress(uint8_t addr[Count]);

        IPAddress(uint32_t ip);

        IPAddress(const String &ipAddr);

        IPAddress(const IPAddress &ipAddr);

        String toString() const;

        uint toUInt32(bool bigEndian = true) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        IPAddress &operator=(const String &value);

        IPAddress &operator=(const IPAddress &value);

        bool operator==(const IPAddress &value) const;

        bool operator!=(const IPAddress &value) const;

        operator String() const;

        bool isEmpty() const;

        void empty();

    public:
        static bool parse(const String &str, IPAddress &value);

        static bool isIPAddress(const String &str);

        static bool getNetworkAddress(const String &ipStr, const String &netmaskStr, IPAddress &network);

        static bool getNetworkAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &network);

        static bool getBroadcastAddress(const String &ipStr, const String &netmaskStr, IPAddress &broadcast);

        static bool getBroadcastAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &broadcast);

        static bool isSubnet(const IPAddress &ip1, const IPAddress &ip2, const IPAddress &netmask);

        static bool isStartToEnd(const IPAddress &start, const IPAddress &end, const IPAddress &netmask);

    public:
        static const IPAddress Empty;
    };

    class IPAddresses : public IIndexGetter<IPAddress> {
    public:
        IPAddresses();

        String toString(const char splitSymbol = ' ') const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        IPAddresses &operator=(const String &value);

        IPAddresses &operator=(const IPAddresses &value);

        bool operator==(const IPAddresses &value) const;

        bool operator!=(const IPAddresses &value) const;

        operator String() const;

        bool isEmpty() const;

        void empty();

        size_t count() const;

        IPAddress at(size_t pos) const override;

        void add(const IPAddress &address);

        void add(const String &address);

        void remove(const IPAddress &address);

        void remove(const String &address);

        void clear();

    public:
        static bool parse(const String &str, IPAddresses &value, const char splitSymbol = ' ');

    private:
        Vector<IPAddress> _addresses;
    };

    class Endpoint {
    public:
        String address;
        int port;

        Endpoint(const String &address = String::Empty, int port = 0);

        Endpoint(const Endpoint &endpoint);

        bool isEmpty() const;

        void empty();

        String toString() const;

        bool isAnyAddress() const;

        Endpoint &operator=(const Endpoint &value);

        bool operator==(const Endpoint &value) const;

        bool operator!=(const Endpoint &value) const;

        bool operator>(const Endpoint &value) const;

        bool operator>=(const Endpoint &value) const;

        bool operator<(const Endpoint &value) const;

        bool operator<=(const Endpoint &value) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        int compareTo(const Endpoint &value) const;

        Endpoint *clone() const;

    public:
        static bool parse(const String &str, Endpoint &value);

    public:
        static const Endpoint Empty;
    };

    class Endpoints : public IIndexGetter<Endpoint> {
    public:
        Endpoints();

        void add(const Endpoint &value);

        void addRange(const Endpoints &value);

        Endpoint at(size_t pos) const override;

        size_t count() const;

        bool contains(const Endpoint &value);

        void clear();

        bool remove(const Endpoint &value);

        bool removeAt(size_t i);

        Endpoints &operator=(const Endpoints &value);

        bool operator==(const Endpoints &value) const;

        bool operator!=(const Endpoints &value) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

    private:
        Vector<Endpoint> _items;
    };

    class P2PEndpoint {
    public:
        Endpoint local;
        Endpoint peer;

        P2PEndpoint(const Endpoint &local = Endpoint::Empty, const Endpoint &peer = Endpoint::Empty);

        P2PEndpoint(const P2PEndpoint &endpoint);

        bool isEmpty() const;

        String toString() const;

        P2PEndpoint &operator=(const P2PEndpoint &value);

        bool operator==(const P2PEndpoint &value) const;

        bool operator!=(const P2PEndpoint &value) const;

        bool operator>(const P2PEndpoint &value) const;

        bool operator>=(const P2PEndpoint &value) const;

        bool operator<(const P2PEndpoint &value) const;

        bool operator<=(const P2PEndpoint &value) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        int compareTo(const P2PEndpoint &value) const;

    public:
        static const P2PEndpoint Empty;
    };

    struct Port {
        Port(ushort value = DefaultPort);

        Port(const Port &port);

        operator ushort() const;

        Port &operator=(const ushort &value);

        Port &operator=(const String &value);

        Port &operator=(const Port &value);

        bool operator==(const Port &value) const;

        bool operator!=(const Port &value) const;

        void read(Stream *stream);

        void write(Stream *stream) const;

        bool isEmpty() const;

        String toString() const;

        static bool parse(const String &str, Port &port);

        static bool parse(const String &str, int &port);

        static bool parse(const String &str, ushort &port);

    public:
        static Port Empty;

    private:
        ushort _value;

    private:
        static const ushort DefaultPort = 0;
        static const ushort MinValue = 1;
        static const ushort MaxValue = 65533;
    };

    struct PortRange {
        int from;
        int to;

        PortRange(int from = DefaultPort, int to = DefaultPort);

        PortRange(const PortRange &pr);

        PortRange &operator=(const PortRange &value);

        bool operator==(const PortRange &value) const;

        bool operator!=(const PortRange &value) const;

        void read(Stream *stream);

        void write(Stream *stream) const;

        String toString() const;

        static bool parse(const String &str, PortRange &pr);

        static const int DefaultPort = 0;
    };

    class Url {
    public:
        Url(const String &uriString = String::Empty);

        Url(const String &baseUrl, const String &relativeUrl);

        Url(const Url &baseUrl, const String &relativeUrl);

        Url(const String &scheme, const Endpoint &endpoint, const String &relativeUrl = String::Empty);

        Url(const Url &url);

        bool isEmpty() const;

        void empty();

        String toString() const;

        bool isDefaultPort() const;

        const Endpoint &endpoint() const;

        const String &address() const;

        int port() const;

        const String &scheme() const;

        bool isSecure() const;

        const String &relativeUrl() const;

        bool isHttpScheme() const;

        bool isWebSocketScheme() const;

        bool isEaseTcpScheme() const;

        bool isMysqlScheme() const;

        bool isMqttScheme() const;

        bool isCoapScheme() const;

        Url &operator=(const Url &value);

        bool operator==(const Url &value) const;

        bool operator!=(const Url &value) const;

        bool operator>(const Url &value) const;

        bool operator>=(const Url &value) const;

        bool operator<(const Url &value) const;

        bool operator<=(const Url &value) const;

    public:
        static bool parse(const String &uriString, Url &value);

        static bool parse(const String &baseUrl, const String &relativeUrl, Url &value);

        static bool parse(const Url &baseUrl, const String &relativeUrl, Url &value);

    public:
        static const String SchemeHttp;
        static const String SchemeHttps;
        static const String SchemeWebSocket;
        static const String SchemeWebSockets;
        static const String SchemeEaseTcp;
        static const String SchemeEaseTcps;
        static const String SchemeMysql;
        static const String SchemeMysqls;
        static const String SchemeMqtt;
        static const String SchemeMqtts;
        static const String SchemeCoap;

        static const Url Empty;

    private:
        static const int DefaultHttpPort = 80;
        static const int DefaultHttpsPort = 443;
        static const char SplitSymbol = '/';
        static const char ParaSymbol = '?';

    private:
        String _scheme;
        Endpoint _endpoint;
        String _relativeUrl;
    };
}

#endif /* NetType_h */
