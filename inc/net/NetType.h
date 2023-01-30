//
//  NetType.h
//  common
//
//  Created by baowei on 2022/9/23.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef NetType_h
#define NetType_h

#include "data/Vector.h"
#include "net/NetType.h"
#include "data/String.h"

namespace Data {
    class StringArray;
}
using namespace Data;

namespace Net {
    struct MacAddress : public IEquatable<MacAddress>, public IEvaluation<MacAddress>, public IComparable<MacAddress> {
    public:
        static const int Count = 6;
        uint8_t address[Count];

        MacAddress();

        MacAddress(uint64_t addr, bool bigEndian = true);

        MacAddress(const uint8_t addr[Count]);

        MacAddress(const String &addr);

        MacAddress(const MacAddress &addr);

        ~MacAddress() override;

        bool equals(const MacAddress &other) const override;

        void evaluates(const MacAddress &other) override;

        int compareTo(const MacAddress &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        uint64_t toUInt64(bool bigEndian = true) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        MacAddress &operator=(const String &other);

        MacAddress &operator=(const MacAddress &other);

        operator String() const;

    public:
        static bool parse(const String &str, MacAddress &address);

        static bool increase(const MacAddress &address, MacAddress &newAddress);

        static MacAddress randomLocalAddress(uint8_t oui1 = 0x08);

    public:
        static const MacAddress Empty;
    };

    class MacAddresses : public Vector<MacAddress> {
    public:
        MacAddresses();

        MacAddresses(const String &addresses);

        MacAddresses(const MacAddresses &array);

        MacAddresses(std::initializer_list<MacAddress> list);

        String toString(const char splitSymbol = ' ') const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        MacAddresses &operator=(const String &value);

        MacAddresses &operator=(const MacAddresses &value);

        operator String() const;

    public:
        static bool parse(const String &str, MacAddresses &value, const char splitSymbol = ' ');
    };

    struct IPAddress : public IEquatable<IPAddress>, public IEvaluation<IPAddress>, public IComparable<IPAddress> {
    public:
        static const int Count = 4;
        uint8_t address[Count];

        IPAddress();

        IPAddress(uint8_t addr[Count]);

        IPAddress(uint32_t ip, bool bigEndian = true);

        IPAddress(const String &ipAddr);

        IPAddress(const IPAddress &ipAddr);

        ~IPAddress() override;

        bool equals(const IPAddress &other) const override;

        void evaluates(const IPAddress &other) override;

        int compareTo(const IPAddress &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        uint32_t toUInt32(bool bigEndian = true) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        IPAddress &operator=(const String &value);

        IPAddress &operator=(const IPAddress &value);

        operator String() const;

    public:
        static bool parse(const String &str, IPAddress &value);

        static bool isIPAddress(const String &str);

        static bool getNetworkAddress(const String &ipStr, const String &netmaskStr, IPAddress &network);

        static bool getNetworkAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &network);

        static bool getBroadcastAddress(const String &ipStr, const String &netmaskStr, IPAddress &broadcast);

        static bool getBroadcastAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &broadcast);

        static bool isSubnet(const String &ipStr1, const String &ipStr2, const String &netmaskStr);

        static bool isSubnet(const IPAddress &ip1, const IPAddress &ip2, const IPAddress &netmask);

        static bool isStartToEnd(const String &startStr, const String &endStr, const String &netmaskStr);

        static bool isStartToEnd(const IPAddress &start, const IPAddress &end, const IPAddress &netmask);

    public:
        static const IPAddress Empty;
    };

    class IPAddresses : public Vector<IPAddress> {
    public:
        IPAddresses();

        IPAddresses(const String &addresses);

        IPAddresses(const IPAddresses &array);

        IPAddresses(std::initializer_list<IPAddress> list);

        String toString(const char splitSymbol = ' ') const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        IPAddresses &operator=(const String &value);

        IPAddresses &operator=(const IPAddresses &value);

        operator String() const;

    public:
        static bool parse(const String &str, IPAddresses &value, const char splitSymbol = ' ');
    };

    struct Port
            : public IEquatable<Port>,
              public IEvaluation<Port>,
              public IComparable<Port>,
              public IEquatable<Port, int>,
              public IEvaluation<int>,
              public IComparable<int> {
    public:
        using IComparable<Port>::operator>;
        using IComparable<Port>::operator>=;
        using IComparable<Port>::operator<;
        using IComparable<Port>::operator<=;
        using IComparable<int>::operator>;
        using IComparable<int>::operator>=;
        using IComparable<int>::operator<;
        using IComparable<int>::operator<=;

        Port(uint16_t value = DefaultValue);

        Port(const Port &port);

        ~Port() override;

        bool equals(const Port &other) const override;

        void evaluates(const Port &other) override;

        int compareTo(const Port &other) const override;

        bool equals(const int &other) const override;

        void evaluates(const int &other) override;

        int compareTo(const int &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        operator uint16_t() const;

        operator int() const;

        operator String() const;

        Port &operator=(const uint16_t &value);

        Port &operator=(const String &value);

        Port &operator=(const Port &value);

    public:
        static bool parse(const String &str, Port &port);

        static bool parse(const String &str, int &port);

        static bool parse(const String &str, uint16_t &port);

    public:
        static const Port Empty;
        static const uint16_t DefaultValue = 0;
        static const uint16_t MinValue = 1;
        static const uint16_t MaxValue = 65533;

    private:
        uint16_t _value;
    };

    struct PortRange : public IEquatable<PortRange>, public IEvaluation<PortRange>, public IComparable<PortRange> {
    public:
        Port from;
        Port to;

        PortRange(uint16_t from = Port::DefaultValue, uint16_t to = Port::DefaultValue);

        PortRange(const Port &from, const Port &to);

        PortRange(const PortRange &range);

        ~PortRange() override;

        bool equals(const PortRange &other) const override;

        void evaluates(const PortRange &other) override;

        int compareTo(const PortRange &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        void read(Stream *stream);

        void write(Stream *stream) const;

        PortRange &operator=(const PortRange &value);

        operator String() const;

    public:
        static bool parse(const String &str, PortRange &range);
    };

    class Endpoint
            : public IEquatable<Endpoint>,
              public IEquatable<Endpoint, String>,
              public IEvaluation<Endpoint>,
              public IComparable<Endpoint> {
    public:
        String address;
        uint16_t port;

        Endpoint(const String &address = String::Empty, uint16_t port = Port::DefaultValue);

        Endpoint(const String &address, const Port &port);

        Endpoint(const Endpoint &endpoint);

        ~Endpoint() override;

        bool equals(const Endpoint &other) const override;

        void evaluates(const Endpoint &other) override;

        int compareTo(const Endpoint &other) const override;

        bool equals(const String &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        bool isAnyAddress() const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        Endpoint &operator=(const Endpoint &value);

        operator String() const;

    public:
        static bool parse(const String &str, Endpoint &value);

    public:
        static const Endpoint Empty;
    };

    class Endpoints : public Vector<Endpoint> {
    public:
        Endpoints();

        Endpoints(const Endpoints &array);

        Endpoints(std::initializer_list<Endpoint> list);

        String toString(const char splitSymbol = ' ') const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        Endpoints &operator=(const Endpoints &value);

        operator String() const;
    };

    class P2PEndpoint
            : public IEquatable<P2PEndpoint>, public IEvaluation<P2PEndpoint>, public IComparable<P2PEndpoint> {
    public:
        Endpoint local;
        Endpoint peer;

        P2PEndpoint(const Endpoint &local = Endpoint::Empty, const Endpoint &peer = Endpoint::Empty);

        P2PEndpoint(const P2PEndpoint &endpoint);

        ~P2PEndpoint() override;

        bool equals(const P2PEndpoint &other) const override;

        void evaluates(const P2PEndpoint &other) override;

        int compareTo(const P2PEndpoint &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        P2PEndpoint &operator=(const P2PEndpoint &value);

        operator String() const;

    public:
        static const P2PEndpoint Empty;
    };

    class Url : public IEquatable<Url>, public IEvaluation<Url>, public IComparable<Url> {
    public:
        Url(const String &urlString = String::Empty);

        Url(const String &baseUrl, const String &relativeUrl);

        Url(const Url &baseUrl, const String &relativeUrl);

        Url(const String &scheme, const Endpoint &endpoint, const String &relativeUrl = String::Empty);

        Url(const Url &url);

        ~Url() override;

        bool equals(const Url &other) const override;

        void evaluates(const Url &other) override;

        int compareTo(const Url &other) const override;

        bool isEmpty() const;

        void empty();

        String toString() const;

        const Endpoint &endpoint() const;

        const String &address() const;

        uint16_t port() const;

        const String &scheme() const;

        const String &relativeUrl() const;

        bool isDefaultPort() const;

        bool isSecure() const;

        bool isHttpScheme() const;

        bool isWebSocketScheme() const;

        bool isEaseTcpScheme() const;

        bool isMysqlScheme() const;

        bool isMqttScheme() const;

        bool isCoapScheme() const;

        Url &operator=(const Url &value);

        Url &operator=(const String &value);

        operator String() const;

    public:
        static bool parse(const String &urlString, Url &value);

        static bool parse(const String &baseUrl, const String &relativeUrl, Url &value);

        static bool parse(const Url &baseUrl, const String &relativeUrl, Url &value);

    public:
        static const char *SchemeHttp;
        static const char *SchemeHttps;
        static const char *SchemeWebSocket;
        static const char *SchemeWebSockets;
        static const char *SchemeEaseTcp;
        static const char *SchemeEaseTcps;
        static const char *SchemeMysql;
        static const char *SchemeMysqls;
        static const char *SchemeMqtt;
        static const char *SchemeMqtts;
        static const char *SchemeCoap;

        static const Url Empty;

        static const StringArray Schemes;

    private:
        static const int DefaultHttpPort = 80;
        static const int DefaultHttpsPort = 443;
        static const int DefaultMysqlPort = 3306;
        static const int DefaultMysqlsPort = 13306;
        static const int DefaultMqttPort = 1883;
        static const int DefaultMqttsPort = 8883;
        static const int DefaultCoapPort = 5683;
        static const char SplitSymbol = '/';
        static const char ParaSymbol = '?';

    private:
        String _scheme;
        Endpoint _endpoint;
        String _relativeUrl;
    };
}

#endif // NetType_h
