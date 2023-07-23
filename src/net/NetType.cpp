//
//  NetType.cpp
//  common
//
//  Created by baowei on 2022/9/23.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "net/NetType.h"
#include "data/DateTime.h"
#include "data/StringArray.h"
#include "IO/Stream.h"
#include "system/Random.h"
#include "data/Convert.h"
#include "system/Regex.h"

using namespace System;

namespace Net {
    const MacAddress MacAddress::Empty = MacAddress();

    MacAddress::MacAddress() : address{} {
        memset(address, 0, sizeof(address));
    }

    MacAddress::MacAddress(uint64_t addr, bool bigEndian) : address{} {
        if (bigEndian) {
            address[0] = (addr >> 40) & 0xFF;
            address[1] = (addr >> 32) & 0xFF;
            address[2] = (addr >> 24) & 0xFF;
            address[3] = (addr >> 16) & 0xFF;
            address[4] = (addr >> 8) & 0xFF;
            address[5] = addr & 0xFF;
        } else {
            address[5] = (addr >> 40) & 0xFF;
            address[4] = (addr >> 32) & 0xFF;
            address[3] = (addr >> 24) & 0xFF;
            address[2] = (addr >> 16) & 0xFF;
            address[1] = (addr >> 8) & 0xFF;
            address[0] = addr & 0xFF;
        }
    }

    MacAddress::MacAddress(const uint8_t addr[Count]) : address{} {
        memcpy(address, addr, sizeof(address));
    }

    MacAddress::MacAddress(const String &addr) : address{} {
        parse(addr, *this);
    }

    MacAddress::MacAddress(const MacAddress &addr) : address{} {
        this->operator=(addr);
    }

    MacAddress::~MacAddress() = default;

    bool MacAddress::equals(const MacAddress &other) const {
        for (size_t i = 0; i < Count; i++) {
            if (this->address[i] != other.address[i])
                return false;
        }
        return true;
    }

    void MacAddress::evaluates(const MacAddress &other) {
        memcpy(this->address, other.address, sizeof(address));
    }

    int MacAddress::compareTo(const MacAddress &other) const {
        return memcmp(this->address, other.address, sizeof(address));
    }

    bool MacAddress::isEmpty() const {
        for (uint8_t addr : address) {
            if (addr != 0)
                return false;
        }
        return true;
    }

    void MacAddress::empty() {
        memset(address, 0, sizeof(address));
    }

    String MacAddress::toString() const {
        if (isEmpty())
            return String::Empty;

        char temp[64];
        sprintf(temp, "%02x:%02x:%02x:%02x:%02x:%02x",
                address[0], address[1], address[2], address[3], address[4], address[5]);
        return temp;
    }

    void MacAddress::write(Stream *stream) const {
        stream->write(address, 0, sizeof(address));
    }

    void MacAddress::read(Stream *stream) {
        stream->read(address, 0, sizeof(address));
    }

    MacAddress &MacAddress::operator=(const String &value) {
        MacAddress ip;
        if (MacAddress::parse(value, ip))
            this->operator=(ip);
        return *this;
    }

    MacAddress &MacAddress::operator=(const MacAddress &other) {
        if (this != &other) {
            MacAddress::evaluates(other);
        }
        return *this;
    }

    MacAddress::operator String() const {
        return toString();
    }

    uint64_t MacAddress::toUInt64(bool bigEndian) const {
        if (bigEndian) {
            return ((uint64_t) address[0] << 40) | ((uint64_t) address[1] << 32) | ((uint64_t) address[2] << 24) |
                   (address[3] << 16) | (address[4] << 8) | address[5];
        } else {
            return ((uint64_t) address[5] << 40) | ((uint64_t) address[4] << 32) | ((uint64_t) address[3] << 24) |
                   (address[2] << 16) | (address[1] << 8) | address[0];
        }
    }

    bool MacAddress::parse(const String &str, MacAddress &address) {
        int addr[Count];
        int result = sscanf(str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
                            &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
        if (result == Count &&
            addr[0] <= UInt8::MaxValue && addr[1] <= UInt8::MaxValue && addr[2] <= UInt8::MaxValue &&
            addr[3] <= UInt8::MaxValue && addr[4] <= UInt8::MaxValue && addr[5] <= UInt8::MaxValue) {
            for (int i = 0; i < Count; i++) {
                address.address[i] = (uint8_t) addr[i];
            }
            return true;
        }
        return false;
    }

    bool MacAddress::increase(const MacAddress &address, MacAddress &newAddress) {
        UInt64 value = address.toUInt64();
        value++;
        newAddress = MacAddress(value);
        return true;
    }

    MacAddress MacAddress::randomLocalAddress(uint8_t oui1) {
        MacAddress mac;
        uint64_t value = (uint64_t) (DateTime::now().total1970Milliseconds() / 10) + Random::getRandValue(0, 65535);
        // bit0: 0: unicast; 1: multicast
        // bit1: 0: globally; 1: locally
        mac.address[0] = oui1 | 0x02;
        mac.address[1] = (value & 0xFF00000000) >> 32;
        mac.address[2] = (value & 0xFF000000) >> 24;
        mac.address[3] = (value & 0xFF0000) >> 16;
        mac.address[4] = (value & 0xFF00) >> 8;
        mac.address[5] = (value & 0xFF);
        return mac;
    }

    MacAddresses::MacAddresses() = default;

    MacAddresses::MacAddresses(const String &addresses) {
        parse(addresses, *this);
    }

    MacAddresses::MacAddresses(const MacAddresses &array) = default;

    MacAddresses::MacAddresses(std::initializer_list<MacAddress> list) : Vector<MacAddress>(list) {
    }

    String MacAddresses::toString(const char splitSymbol) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0)
                str.append(splitSymbol);
            str.append(at(i));
        }
        return str;
    }

    void MacAddresses::write(Stream *stream) const {
        stream->writeByte((uint8_t) count());
        for (size_t i = 0; i < count(); i++) {
            at(i).write(stream);
        }
    }

    void MacAddresses::read(Stream *stream) {
        uint8_t count = stream->readByte();
        for (uint8_t i = 0; i < count; i++) {
            MacAddress address;
            address.read(stream);
            add(address);
        }
    }

    MacAddresses &MacAddresses::operator=(const String &value) {
        clear();
        parse(value, *this);
        return *this;
    }

    MacAddresses &MacAddresses::operator=(const MacAddresses &value) {
        Vector<MacAddress>::evaluates(value);
        return *this;
    }

    MacAddresses::operator String() const {
        return toString();
    }

    bool MacAddresses::parse(const String &str, MacAddresses &value, const char splitSymbol) {
        value.clear();

        StringArray texts;
        Convert::splitStr(str, texts, splitSymbol);
        for (size_t i = 0; i < texts.count(); i++) {
            const String ipStr = texts[i].trim();
            MacAddress address;
            if (!MacAddress::parse(ipStr, address))
                return false;

            value.add(address);
        }
        return true;
    }

    const IPAddress IPAddress::Empty = IPAddress();

    IPAddress::IPAddress() : address{} {
        memset(this->address, 0, Count);
    }

    IPAddress::IPAddress(uint8_t addr[Count]) : address{} {
        memcpy(this->address, addr, Count);
    }

    IPAddress::IPAddress(uint32_t ip, bool bigEndian) : address{} {
        if (bigEndian) {
            address[0] = (ip >> 24) & 0xFF;
            address[1] = (ip >> 16) & 0xFF;
            address[2] = (ip >> 8) & 0xFF;
            address[3] = ip & 0xFF;
        } else {
            address[3] = (ip >> 24) & 0xFF;
            address[2] = (ip >> 16) & 0xFF;
            address[1] = (ip >> 8) & 0xFF;
            address[0] = ip & 0xFF;
        }
    }

    IPAddress::IPAddress(const String &ipAddr) : IPAddress() {
        this->operator=(ipAddr);
    }

    IPAddress::IPAddress(const IPAddress &ipAddr) : IPAddress() {
        this->operator=(ipAddr);
    }

    IPAddress::~IPAddress() = default;

    bool IPAddress::equals(const IPAddress &other) const {
        for (size_t i = 0; i < Count; i++) {
            if (this->address[i] != other.address[i])
                return false;
        }
        return true;
    }

    void IPAddress::evaluates(const IPAddress &other) {
        memcpy(this->address, other.address, sizeof(address));
    }

    int IPAddress::compareTo(const IPAddress &other) const {
        return memcmp(this->address, other.address, sizeof(address));
    }

    String IPAddress::toString() const {
        if (isEmpty())
            return String::Empty;

        char temp[64];
        sprintf(temp, "%d.%d.%d.%d",
                address[0], address[1], address[2], address[3]);
        return temp;
    }

    uint32_t IPAddress::toUInt32(bool bigEndian) const {
        if (bigEndian) {
            return (address[0] << 24) | (address[1] << 16) | (address[2] << 8) | address[3];
        } else {
            return (address[3] << 24) | (address[2] << 16) | (address[1] << 8) | address[0];
        }
    }

    void IPAddress::write(Stream *stream) const {
        stream->write(address, 0, sizeof(address));
    }

    void IPAddress::read(Stream *stream) {
        stream->read(address, 0, sizeof(address));
    }

    IPAddress &IPAddress::operator=(const String &value) {
        if (value.isNullOrEmpty())
            this->operator=(Empty);
        else {
            IPAddress ip;
            if (IPAddress::parse(value, ip))
                this->operator=(ip);
        }
        return *this;
    }

    IPAddress &IPAddress::operator=(const IPAddress &value) {
        if (this != &value) {
            IPAddress::evaluates(value);
        }
        return *this;
    }

    IPAddress::operator String() const {
        return toString();
    }

    bool IPAddress::isEmpty() const {
        for (uint8_t addr : address) {
            if (addr != 0)
                return false;
        }
        return true;
    }

    void IPAddress::empty() {
        memset(this->address, 0, Count);
    }

    bool IPAddress::parse(const String &str, IPAddress &address) {
        int addr[Count];
        uint32_t len = 0;
        String temp = str.trim();
        int result = sscanf(temp.c_str(), "%d.%d.%d.%d%n",
                            &addr[0], &addr[1], &addr[2], &addr[3], &len);
        if (result == Count && temp.length() == len &&
            addr[0] <= 255 && addr[1] <= 255 && addr[2] <= 255 && addr[3] <= 255) {
            for (int i = 0; i < Count; i++) {
                address.address[i] = (uint8_t) addr[i];
            }
            return true;
        }
        return false;
    }

    bool IPAddress::isIPAddress(const String &str) {
        IPAddress address;
        return IPAddress::parse(str, address);
    }

    bool IPAddress::getNetworkAddress(const String &ipStr, const String &netmaskStr, IPAddress &network) {
        IPAddress ip, netmask;
        if (IPAddress::parse(ipStr, ip) && IPAddress::parse(netmaskStr, netmask))
            return getNetworkAddress(ip, netmask, network);
        return false;
    }

    bool IPAddress::getNetworkAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &network) {
        for (size_t i = 0; i < Count; i++) {
            network.address[i] = ip.address[i] & netmask.address[i];
        }
        return true;
    }

    bool IPAddress::getBroadcastAddress(const String &ipStr, const String &netmaskStr, IPAddress &broadcast) {
        IPAddress ip, netmask;
        if (IPAddress::parse(ipStr, ip) && IPAddress::parse(netmaskStr, netmask))
            return getBroadcastAddress(ip, netmask, broadcast);
        return false;
    }

    bool IPAddress::getBroadcastAddress(const IPAddress &ip, const IPAddress &netmask, IPAddress &broadcast) {
        IPAddress network;
        if (getNetworkAddress(ip, netmask, network)) {
            for (size_t i = 0; i < Count; i++) {
                uint8_t temp = ~netmask.address[i];
                broadcast.address[i] = network.address[i] | temp;
            }
            return true;
        }
        return false;
    }

    bool IPAddress::isSubnet(const String &ipStr1, const String &ipStr2, const String &netmaskStr) {
        IPAddress ip1, ip2, netmask;
        if (parse(ipStr1, ip1) && parse(ipStr2, ip2) && parse(netmaskStr, netmask)) {
            return isSubnet(ip1, ip2, netmask);
        }
        return false;
    }

    bool IPAddress::isSubnet(const IPAddress &ip1, const IPAddress &ip2, const IPAddress &netmask) {
        for (size_t i = 0; i < Count; i++) {
            uint8_t b1 = ip1.address[i] & netmask.address[i];
            uint8_t b2 = ip2.address[i] & netmask.address[i];
            if (b1 != b2)
                return false;
        }
        return true;
    }

    bool IPAddress::isStartToEnd(const String &startStr, const String &endStr, const String &netmaskStr) {
        IPAddress start, end, netmask;
        if (parse(startStr, start) && parse(endStr, end) && parse(netmaskStr, netmask)) {
            return isStartToEnd(start, end, netmask);
        }
        return false;
    }

    bool IPAddress::isStartToEnd(const IPAddress &start, const IPAddress &end, const IPAddress &netmask) {
        uint32_t n = ~netmask.toUInt32();
        uint32_t s = start.toUInt32() & n;
        uint32_t e = end.toUInt32() & n;
        return s <= e;
    }

    IPAddresses::IPAddresses() = default;

    IPAddresses::IPAddresses(const String &addresses) {
        parse(addresses, *this);
    }

    IPAddresses::IPAddresses(const IPAddresses &array) = default;

    IPAddresses::IPAddresses(std::initializer_list<IPAddress> list) : Vector<IPAddress>(list) {
    }

    String IPAddresses::toString(const char splitSymbol) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0)
                str.append(splitSymbol);
            str.append(at(i));
        }
        return str;
    }

    void IPAddresses::write(Stream *stream) const {
        stream->writeByte((uint8_t) count());
        for (size_t i = 0; i < count(); i++) {
            at(i).write(stream);
        }
    }

    void IPAddresses::read(Stream *stream) {
        clear();
        uint8_t count = stream->readByte();
        for (size_t i = 0; i < count; i++) {
            IPAddress address;
            address.read(stream);
            add(address);
        }
    }

    IPAddresses &IPAddresses::operator=(const String &value) {
        clear();
        String str = value;
        str = String::replace(str, ";", " ");
        str = String::replace(str, ",", " ");
        parse(value, *this);
        return *this;
    }

    IPAddresses &IPAddresses::operator=(const IPAddresses &value) {
        evaluates(value);
        return *this;
    }

    IPAddresses::operator String() const {
        return toString();
    }

    bool IPAddresses::parse(const String &str, IPAddresses &value, const char splitSymbol) {
        value.clear();

        StringArray texts;
        Convert::splitStr(str, texts, splitSymbol);
        for (size_t i = 0; i < texts.count(); i++) {
            const String ipStr = texts[i].trim();
            IPAddress address;
            if (!IPAddress::parse(ipStr, address))
                return false;

            value.add(address);
        }
        return true;
    }

    const Port Port::Empty(0);

    Port::Port(uint16_t value) {
        if (value >= MinValue && value <= MaxValue)
            _value = value;
        else
            _value = DefaultValue;
    }

    Port::Port(const Port &port) {
        _value = port._value;
    }

    Port::~Port() = default;

    bool Port::equals(const Port &other) const {
        return _value == other._value;
    }

    void Port::evaluates(const Port &other) {
        _value = other._value;
    }

    int Port::compareTo(const Port &other) const {
        if (_value > other._value) return 1;
        if (_value < other._value) return -1;
        return 0;
    }

    bool Port::equals(const int &other) const {
        return _value == other;
    }

    void Port::evaluates(const int &other) {
        _value = other;
    }

    int Port::compareTo(const int &other) const {
        if (_value > other) return 1;
        if (_value < other) return -1;
        return 0;
    }

    bool Port::isEmpty() const {
        return _value == DefaultValue;
    }

    void Port::empty() {
        _value = DefaultValue;
    }

    Port::operator uint16_t() const {
        return _value;
    }

    Port::operator int() const {
        return _value;
    }

    Port::operator unsigned int() const {
        return _value;
    }

    Port::operator String() const {
        return UInt16(_value).toString();
    }

    Port &Port::operator=(const uint16_t &value) {
        _value = value;
        return *this;
    }

    Port &Port::operator=(const String &value) {
        parse(value, *this);
        return *this;
    }

    Port &Port::operator=(const Port &value) {
        evaluates(value);
        return *this;
    }

    void Port::read(Stream *stream) {
        _value = stream->readUInt16();
    }

    void Port::write(Stream *stream) const {
        stream->writeUInt16(_value);
    }

    String Port::toString() const {
        return ((UInt16) _value).toString();
    }

    bool Port::parse(const String &str, Port &port) {
        uint16_t value;
        if (UInt16::parse(str, value) &&
            (value >= MinValue && value <= MaxValue)) {
            port._value = value;
            return true;
        }
        return false;
    }

    bool Port::parse(const String &str, int &port) {
        Port p;
        if (Port::parse(str, p)) {
            port = p;
            return true;
        }
        return false;
    }

    bool Port::parse(const String &str, uint16_t &port) {
        Port p;
        if (Port::parse(str, p)) {
            port = p;
            return true;
        }
        return false;
    }

    PortRange::PortRange(uint16_t from, uint16_t to) : from(from), to(to) {
    }

    PortRange::PortRange(const Port &from, const Port &to) : from(from), to(to) {
    }

    PortRange::PortRange(const PortRange &range) {
        this->from = range.from;
        this->to = range.to;
    }

    PortRange::~PortRange() = default;

    bool PortRange::equals(const PortRange &other) const {
        return this->from == other.from && this->to == other.to;
    }

    void PortRange::evaluates(const PortRange &other) {
        this->from = other.from;
        this->to = other.to;
    }

    int PortRange::compareTo(const PortRange &other) const {
        if (from != other.from) {
            if (from > other.from) {
                return 1;
            }
            return -1;
        }
        if (to != other.to) {
            if (to > other.to) {
                return 1;
            }
            return -1;
        }
        return 0;
    }

    bool PortRange::isEmpty() const {
        return from.isEmpty() || to.isEmpty();
    }

    void PortRange::empty() {
        from.empty();
        to.empty();
    }

    PortRange &PortRange::operator=(const PortRange &value) {
        evaluates(value);
        return *this;
    }

    PortRange::operator String() const {
        return toString();
    }

    void PortRange::read(Stream *stream) {
        this->from.read(stream);
        this->to.read(stream);
    }

    void PortRange::write(Stream *stream) const {
        ((UInt16) this->from).write(stream);
        ((UInt16) this->to).write(stream);
    }

    String PortRange::toString() const {
        if (from == to) {
            return isEmpty() ? String::Empty : from.toString();
        } else {
            return String::convert("%d-%d", (int) from, (int) to);
        }
    }

    bool PortRange::parse(const String &str, PortRange &range) {
        String temp = str.trim();
        int port;
        if (Int32::parse(temp, port)) {
            if (!(port >= Port::MinValue && port <= Port::MaxValue))
                return false;

            range = PortRange(port, port);
            return true;
        }
        StringArray items;
        StringArray::parse(temp, items, '-');
        if (items.count() == 2) {
            int from = 0, to = 0;
            if (Int32::parse(items[0], from) &&
                Int32::parse(items[1], to) &&
                to >= from) {
                if (!(from >= Port::MinValue && from <= Port::MaxValue))
                    return false;
                if (!(to >= Port::MinValue && to <= Port::MaxValue))
                    return false;

                range = PortRange(from, to);
                return true;
            }
        }
        return false;
    }

    const Endpoint Endpoint::Empty("", 0);

    Endpoint::Endpoint(const String &address, uint16_t port) : Endpoint(address, Port(port)) {
    }

    Endpoint::Endpoint(const String &address, const Port &port) : address(address), port(port) {
    }

    Endpoint::Endpoint(const Endpoint &endpoint) : address(endpoint.address), port(endpoint.port) {
    }

    Endpoint::~Endpoint() = default;

    bool Endpoint::equals(const Endpoint &other) const {
        return address == other.address && port == other.port;
    }

    void Endpoint::evaluates(const Endpoint &other) {
        address = other.address;
        port = other.port;
    }

    int Endpoint::compareTo(const Endpoint &other) const {
        if (port > other.port) {
            return 1;
        } else if (port < other.port) {
            return -1;
        }
        if (address > other.address) {
            return 1;
        } else if (address < other.address) {
            return -1;
        }
        return 0;
    }

    bool Endpoint::equals(const String &other) const {
        Endpoint endpoint;
        if (!Endpoint::parse(other, endpoint)) {
            return false;
        }
        return address == endpoint.address && port == endpoint.port;
    }

    bool Endpoint::isEmpty() const {
        return address.isNullOrEmpty() || port == Port::DefaultValue;
    }

    void Endpoint::empty() {
        address.empty();
        port = Port::DefaultValue;
    }

    String Endpoint::toString() const {
        if (isEmpty())
            return String::Empty;

        return String::convert("%s:%d", address.c_str(), port);
    }

    bool Endpoint::isAnyAddress() const {
        return (address.isNullOrEmpty() ||
                String::equals(address, "any", true) ||
                String::equals(address, "0.0.0.0", true));
    }

    void Endpoint::write(Stream *stream, bool bigEndian) const {
        address.write(stream, String::StreamLength1);
        stream->writeInt32(port, bigEndian);
    }

    void Endpoint::read(Stream *stream, bool bigEndian) {
        address.read(stream, String::StreamLength1);
        port = stream->readInt32(bigEndian);
    }

    bool Endpoint::parse(const String &str, Endpoint &value) {
        StringArray texts;
        StringArray::parse(str, texts, ':');
        if (texts.count() == 2) {
            String address = texts[0].trim();
            Port port;
            if (Port::parse(texts[1].trim(), port)) {
                value.address = address;
                value.port = port;
                return true;
            }
        }
        return false;
    }

    Endpoint &Endpoint::operator=(const Endpoint &value) {
        address = value.address;
        port = value.port;
        return *this;
    }

    Endpoint::operator String() const {
        return toString();
    }

    Endpoints::Endpoints() = default;

    Endpoints::Endpoints(const Endpoints &array) = default;

    Endpoints::Endpoints(std::initializer_list<Endpoint> list) : Vector<Endpoint>(list) {
    }

    String Endpoints::toString(const char splitSymbol) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0)
                str.append(splitSymbol);
            str.append(at(i));
        }
        return str;
    }

    void Endpoints::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt32((uint32_t) count(), bigEndian);
        for (size_t i = 0; i < count(); i++) {
            const Endpoint &endpoint = at(i);
            endpoint.write(stream, bigEndian);
        }
    }

    void Endpoints::read(Stream *stream, bool bigEndian) {
        size_t count = stream->readUInt32(bigEndian);
        for (size_t i = 0; i < count; i++) {
            Endpoint endpoint;
            endpoint.read(stream, bigEndian);
            add(endpoint);
        }
    }

    Endpoints &Endpoints::operator=(const Endpoints &value) {
        evaluates(value);
        return *this;
    }

    Endpoints::operator String() const {
        return toString();
    }

    const P2PEndpoint P2PEndpoint::Empty;

    P2PEndpoint::P2PEndpoint(const Endpoint &local, const Endpoint &peer) : local(local), peer(peer) {
    }

    P2PEndpoint::P2PEndpoint(const P2PEndpoint &endpoint) : P2PEndpoint(endpoint.local, endpoint.peer) {
    }

    P2PEndpoint::~P2PEndpoint() = default;

    bool P2PEndpoint::equals(const P2PEndpoint &other) const {
        return local == other.local && peer == other.peer;
    }

    void P2PEndpoint::evaluates(const P2PEndpoint &other) {
        local = other.local;
        peer = other.peer;
    }

    int P2PEndpoint::compareTo(const P2PEndpoint &other) const {
        if (local > other.local) {
            return 1;
        } else if (local < other.local) {
            return -1;
        }

        if (peer > other.peer) {
            return 1;
        } else if (peer < other.peer) {
            return -1;
        }
        return 0;
    }

    bool P2PEndpoint::isEmpty() const {
        return local.isEmpty() && peer.isEmpty();
    }

    void P2PEndpoint::empty() {
        local.empty();
        peer.empty();
    }

    String P2PEndpoint::toString() const {
        return String::convert("%s, %s", local.toString().c_str(), peer.toString().c_str());
    }

    void P2PEndpoint::write(Stream *stream, bool bigEndian) const {
        local.write(stream, bigEndian);
        peer.write(stream, bigEndian);
    }

    void P2PEndpoint::read(Stream *stream, bool bigEndian) {
        local.read(stream, bigEndian);
        peer.read(stream, bigEndian);
    }

    P2PEndpoint &P2PEndpoint::operator=(const P2PEndpoint &value) {
        evaluates(value);
        return *this;
    }

    P2PEndpoint::operator String() const {
        return toString();
    }

    const char *Url::SchemeHttp = "http";
    const char *Url::SchemeHttps = "https";
    const char *Url::SchemeWebSocket = "ws";
    const char *Url::SchemeWebSockets = "wss";
    const char *Url::SchemeEaseTcp = "ease.tcp";
    const char *Url::SchemeEaseTcps = "ease.tcps";
    const char *Url::SchemeMysql = "mysql";
    const char *Url::SchemeMysqls = "mysqls";
    const char *Url::SchemeMqtt = "mqtt";
    const char *Url::SchemeMqtts = "mqtts";
    const char *Url::SchemeCoap = "coap";
    const Url Url::Empty("");
    const StringArray Url::Schemes = {SchemeHttp, SchemeHttps,
                                      SchemeWebSocket, SchemeWebSockets,
                                      SchemeEaseTcp, SchemeEaseTcps,
                                      SchemeMysql, SchemeMysqls,
                                      SchemeMqtt, SchemeMqtts,
                                      SchemeCoap};

    Url::Url(const String &urlString) {
        Url::parse(urlString, *this);
    }

    Url::Url(const String &baseUrl, const String &relativeUrl) {
        Url url(baseUrl);
        Url::parse(url, relativeUrl, *this);
    }

    Url::Url(const Url &baseUrl, const String &relativeUrl) {
        Url::parse(baseUrl, relativeUrl, *this);
    }

    Url::Url(const String &scheme, const Endpoint &endpoint, const String &relativeUrl) {
        this->_scheme = scheme;
        this->_endpoint = endpoint;
        this->_relativeUrl = relativeUrl;
    }

    Url::Url(const Url &url) {
        this->operator=(url);
    }

    Url::~Url() = default;

    bool Url::equals(const Url &other) const {
        return this->_scheme == other._scheme &&
               this->_endpoint == other._endpoint &&
               this->_relativeUrl == other._relativeUrl;
    }

    void Url::evaluates(const Url &other) {
        this->_scheme = other._scheme;
        this->_endpoint = other._endpoint;
        this->_relativeUrl = other._relativeUrl;
    }

    int Url::compareTo(const Url &other) const {
        return toString().toLower().compareTo(other.toString().toLower());
    }

    bool Url::isEmpty() const {
        return _scheme.isNullOrEmpty() && _endpoint.isEmpty();
    }

    void Url::empty() {
        _scheme.empty();
        _endpoint.empty();
    }

    String Url::toString() const {
        if (isEmpty())
            return String::Empty;

        String item = (_relativeUrl.length() > 0 && _relativeUrl[0] == SplitSymbol) ? _relativeUrl.substr(1)
                                                                                    : _relativeUrl;
        return String::format("%s://%s/%s",
                              _scheme.c_str(),
                              !isDefaultPort() ? _endpoint.toString().c_str() : _endpoint.address.c_str(),
                              item.c_str());
    }

    const Endpoint &Url::endpoint() const {
        return _endpoint;
    }

    const String &Url::address() const {
        return _endpoint.address;
    }

    uint16_t Url::port() const {
        return _endpoint.port;
    }

    const String &Url::scheme() const {
        return _scheme;
    }

    const String &Url::relativeUrl() const {
        return _relativeUrl;
    }

    bool Url::isDefaultPort() const {
        if (_scheme == SchemeHttp)
            return _endpoint.port == DefaultHttpPort;
        else if (_scheme == SchemeHttps)
            return _endpoint.port == DefaultHttpsPort;
        else if (_scheme == SchemeMysql)
            return _endpoint.port == DefaultMysqlPort;
        else if (_scheme == SchemeMysqls)
            return _endpoint.port == DefaultMysqlsPort;
        else if (_scheme == SchemeMqtt)
            return _endpoint.port == DefaultMqttPort;
        else if (_scheme == SchemeMqtts)
            return _endpoint.port == DefaultMqttsPort;
        else if (_scheme == SchemeCoap)
            return _endpoint.port == DefaultCoapPort;
        return false;
    }

    bool Url::isSecure() const {
        return _scheme == SchemeHttps || _scheme == SchemeMysqls || _scheme == SchemeMqtts ||
               _scheme == SchemeEaseTcps || _scheme == SchemeWebSockets;
    }

    bool Url::isHttpScheme() const {
        return _scheme == SchemeHttp || _scheme == SchemeHttps;
    }

    bool Url::isWebSocketScheme() const {
        return _scheme == SchemeWebSocket || _scheme == SchemeWebSockets;
    }

    bool Url::isEaseTcpScheme() const {
        return _scheme == SchemeEaseTcp || _scheme == SchemeEaseTcps;
    }

    bool Url::isMysqlScheme() const {
        return _scheme == SchemeMysql || _scheme == SchemeMysqls;
    }

    bool Url::isMqttScheme() const {
        return _scheme == SchemeMqtt || _scheme == SchemeMqtts;
    }

    bool Url::isCoapScheme() const {
        return _scheme == SchemeCoap;
    }

    Url &Url::operator=(const Url &value) {
        evaluates(value);
        return *this;
    }

    Url &Url::operator=(const String &value) {
        parse(value, *this);
        return *this;
    }

    Url::operator String() const {
        return toString();
    }

    bool Url::parse(const String &urlString, Url &value) {
        if (urlString.isNullOrEmpty())
            return false;

#if defined(CC_CLANG) || !defined(CC_GNU) || CC_GNU >= 501
        static const Regex urlRegex(
                R"(^([a-z|A-Z|0-9|.|_-|]+)://([a-z|A-Z|0-9|.|_-]+)(:\d*|\S*)([a-zA-Z0-9=\-\.\?\,\'\/\\+&%\$#_]*)$)");
//        static const char *fmt = R"(^(%s)://([a-z|A-Z|0-9|.|_-]+)(:\d*|\S*)([a-zA-Z0-9=\-\.\?\,\'\/\\+&%\$#_]*)$)";
//        static const String pattern = String::format(fmt, Schemes.toString('|').c_str());
//        static const Regex urlRegex(pattern);
        StringArray groups;
        bool result = urlRegex.match(urlString, groups);
        if (result) {
            if (groups.count() > 3) {
                String scheme = groups[0].trim();
                String address = groups[1].trim();
                String str = groups[2].trim(' ', ':');
                Port port;
                String relativeUrl;
                if (Port::parse(str, port)) {
                    relativeUrl = groups[3].trim();
                } else {
                    if (scheme == SchemeHttp)
                        port = DefaultHttpPort;
                    else if (scheme == SchemeHttps)
                        port = DefaultHttpsPort;
                    else if (scheme == SchemeMysql)
                        port = DefaultMysqlPort;
                    else if (scheme == SchemeMysqls)
                        port = DefaultMysqlsPort;
                    else if (scheme == SchemeMqtt)
                        port = DefaultMqttPort;
                    else if (scheme == SchemeMqtts)
                        port = DefaultMqttsPort;
                    else if (scheme == SchemeCoap)
                        port = DefaultCoapPort;
                    relativeUrl = str;
                }

                value._scheme = scheme;
                value._endpoint = Endpoint(address, port);
                if (!relativeUrl.isNullOrEmpty() && relativeUrl[0] == '/')
                    relativeUrl = relativeUrl.substr(1, relativeUrl.length() - 1);
                value._relativeUrl = relativeUrl;
                return true;
            }
        }
#else
        // https://en.cppreference.com/w/c/io/fscanf
        char schemeStr[255] = {}, endpointStr[255] = {}, relativeStr[255] = {};
        int result = sscanf(urlString.c_str(), "%[a-z|A-Z|0-9|.|_-]://%[a-z|A-Z|0-9|.|_-|:]%s", schemeStr, endpointStr, relativeStr);
        if (result >= 2) {
            value._scheme = schemeStr;
            Endpoint endpoint;
            if(Endpoint::parse(endpointStr, endpoint)) {
                value._endpoint = endpoint;
            } else {
                Port port;
                String scheme = schemeStr;
                if (scheme == SchemeHttp)
                    port = DefaultHttpPort;
                else if (scheme == SchemeHttps)
                    port = DefaultHttpsPort;
                else if (scheme == SchemeMysql)
                    port = DefaultMysqlPort;
                else if (scheme == SchemeMysqls)
                    port = DefaultMysqlsPort;
                else if (scheme == SchemeMqtt)
                    port = DefaultMqttPort;
                else if (scheme == SchemeMqtts)
                    port = DefaultMqttsPort;
                else if (scheme == SchemeCoap)
                    port = DefaultCoapPort;
                value._endpoint = Endpoint(endpointStr, port);
            }
            value._relativeUrl = String(relativeStr).trim('/', '\\');
            return true;
        }
#endif
        return false;
    }

    bool Url::parse(const String &baseUrl, const String &relativeUrl, Url &value) {
        Url url(baseUrl);
        return parse(url, relativeUrl, value);
    }

    bool Url::parse(const Url &baseUrl, const String &relativeUrl, Url &value) {
        Url url(baseUrl);
        if (!url.isEmpty()) {
            if (url._relativeUrl.isNullOrEmpty()) {
                url._relativeUrl = relativeUrl;
            } else {
                bool hasSplitSymbol = url._relativeUrl[url._relativeUrl.length() - 1] == SplitSymbol;
                String item = (relativeUrl.length() > 0 && relativeUrl[0] == SplitSymbol) ? relativeUrl.substr(1)
                                                                                          : relativeUrl;
                String temp = url._relativeUrl;
                if (hasSplitSymbol)
                    temp.append(SplitSymbol);
                if ((!temp.isNullOrEmpty() &&
                     (temp[temp.length() - 1] != SplitSymbol && temp[temp.length() - 1] != ParaSymbol)) &&
                    (!item.isNullOrEmpty() && (item[0] != SplitSymbol && item[0] != ParaSymbol))) {
                    temp.append(SplitSymbol);
                }
                temp.append(item);
                url._relativeUrl = temp;
            }
            value = url;
            return true;
        }
        return false;
    }
}
