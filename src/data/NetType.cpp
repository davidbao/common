#include "data/NetType.h"
#include "data/DateTime.h"
#include "data/StringArray.h"
#include "IO/Stream.h"
#include "system/Random.h"
#include "system/Convert.h"
#include "system/Regex.h"

namespace Common {
    const MacAddress MacAddress::Empty = MacAddress();

    MacAddress::MacAddress() {
        memset(address, 0, sizeof(address));
    }

    MacAddress::MacAddress(uint64_t addr) {
        address[0] = (addr >> 40) & 0xFF;
        address[1] = (addr >> 32) & 0xFF;
        address[2] = (addr >> 24) & 0xFF;
        address[3] = (addr >> 16) & 0xFF;
        address[4] = (addr >> 8) & 0xFF;
        address[5] = addr & 0xFF;
    }

    MacAddress::MacAddress(const uint8_t addr[Count]) {
        memcpy(address, addr, sizeof(address));
    }

    MacAddress::MacAddress(const String &addr) {
        parse(addr, *this);
    }

    MacAddress::MacAddress(const MacAddress &addr) {
        this->operator=(addr);
    }

    String MacAddress::toString() const {
        if (isEmpty())
            return String::Empty;

        char temp[64];
        sprintf(temp, "%02X:%02X:%02X:%02X:%02X:%02X",
                address[0], address[1], address[2], address[3], address[4], address[5]);
        return temp;
    }

    void MacAddress::write(Stream *stream) const {
        stream->write(address, 0, sizeof(address));
    }

    void MacAddress::read(Stream *stream) {
        stream->read(address, 0, sizeof(address));
    }

    MacAddress& MacAddress::operator=(const String &value) {
        MacAddress ip;
        if (MacAddress::parse(value, ip))
            this->operator=(ip);
        return *this;
    }

    MacAddress& MacAddress::operator=(const MacAddress &value) {
        memcpy(this->address, value.address, Count);
        return *this;
    }

    bool MacAddress::operator==(const MacAddress &value) const {
        for (uint i = 0; i < Count; i++) {
            if (this->address[i] != value.address[i])
                return false;
        }
        return true;
    }

    bool MacAddress::operator!=(const MacAddress &value) const {
        return !operator==(value);
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

    bool MacAddress::isEmpty() const {
        for (uint i = 0; i < Count; i++) {
            if (this->address[i] != 0)
                return false;
        }
        return true;
    }

    void MacAddress::empty() {
        memset(this->address, 0, Count);
    }

    bool MacAddress::parse(const String &str, MacAddress &address) {
        int addr[Count];
        int result = sscanf(str.c_str(), "%02X:%02X:%02X:%02X:%02X:%02X",
                            &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
        if (result == Count &&
            addr[0] <= 255 && addr[1] <= 255 && addr[2] <= 255 &&
            addr[3] <= 255 && addr[4] <= 255 && addr[5] <= 255) {
            for (int i = 0; i < Count; i++) {
                address.address[i] = (uint8_t) addr[i];
            }
            return true;
        }
        return false;
    }

    bool MacAddress::increase(const MacAddress &address, MacAddress &newAddress) {
        Int64 value = address.toUInt64();
        value++;
        newAddress = MacAddress(value);
        return true;
    }

    MacAddress MacAddress::randomLocalAddress(uint8_t oui1) {
        MacAddress mac;
        uint64_t value = DateTime::now().total1970Milliseconds() / 10 + Random::getRandValue(0, 65535);
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

    MacAddresses::MacAddresses() {
    }

    String MacAddresses::toString(const char splitSymbol) const {
        String str;
        for (uint i = 0; i < _addresses.count(); i++) {
            if (str.length() > 0)
                str.append(splitSymbol);
            str.append(_addresses[i]);
        }
        return str;
    }

    void MacAddresses::write(Stream *stream) const {
        stream->writeByte((uint8_t) _addresses.count());
        for (uint i = 0; i < _addresses.count(); i++) {
            _addresses[i].write(stream);
        }
    }

    void MacAddresses::read(Stream *stream) {
        uint8_t count = stream->readByte();
        for (uint i = 0; i < count; i++) {
            MacAddress address;
            address.read(stream);
            add(address);
        }
    }

    MacAddresses& MacAddresses::operator=(const String &value) {
        clear();
        parse(value, *this);
        return *this;
    }

    MacAddresses& MacAddresses::operator=(const MacAddresses &value) {
        clear();
        for (uint i = 0; i < value._addresses.count(); i++) {
            add(value._addresses[i]);
        }
        return *this;
    }

    bool MacAddresses::operator==(const MacAddresses &value) const {
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] != value._addresses[i])
                return false;
        }
        return true;
    }

    bool MacAddresses::operator!=(const MacAddresses &value) const {
        return !operator==(value);
    }

    MacAddresses::operator String() const {
        return toString();
    }

    bool MacAddresses::isEmpty() const {
        return _addresses.count() == 0;
    }

    void MacAddresses::empty() {
        clear();
    }

    bool MacAddresses::parse(const String &str, MacAddresses &value, const char splitSymbol) {
        value.clear();

        StringArray texts;
        Convert::splitStr(str, texts, splitSymbol);
        for (uint i = 0; i < texts.count(); i++) {
            const String ipStr = texts[i].trim();
            MacAddress address;
            if (!MacAddress::parse(ipStr, address))
                return false;

            value.add(address);
        }
        return true;
    }

    size_t MacAddresses::count() const {
        return _addresses.count();
    }

    MacAddress MacAddresses::at(size_t pos) const {
        if (pos < count()) {
            return _addresses[pos];
        }
        return MacAddress::Empty;
    }

    void MacAddresses::add(const MacAddress &address) {
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] == address)
                return;
        }
        _addresses.add(address);
    }

    void MacAddresses::add(const String &address) {
        MacAddress ip;
        if (MacAddress::parse(address, ip))
            _addresses.add(ip);
    }

    void MacAddresses::remove(const MacAddress &address) {
        Vector<MacAddress> temp;
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] == address)
                temp.add(address);
        }

        for (uint i = 0; i < temp.count(); i++) {
            _addresses.remove(temp[i]);
        }
    }

    void MacAddresses::remove(const String &address) {
        MacAddress ip;
        if (MacAddress::parse(address, ip))
            _addresses.remove(ip);
    }

    void MacAddresses::clear() {
        _addresses.clear();
    }

    const IPAddress IPAddress::Empty = IPAddress();

    IPAddress::IPAddress() {
        memset(this->address, 0, Count);
    }

    IPAddress::IPAddress(uint8_t addr[Count]) {
        memcpy(this->address, addr, Count);
    }

    IPAddress::IPAddress(uint32_t ip) {
        address[0] = (ip >> 24) & 0xFF;
        address[1] = (ip >> 16) & 0xFF;
        address[2] = (ip >> 8) & 0xFF;
        address[3] = ip & 0xFF;
    }

    IPAddress::IPAddress(const String &ipAddr) : IPAddress() {
        this->operator=(ipAddr);
    }

    IPAddress::IPAddress(const IPAddress &ipAddr) {
        this->operator=(ipAddr);
    }

    String IPAddress::toString() const {
        if (isEmpty())
            return String::Empty;

        char temp[64];
        sprintf(temp, "%d.%d.%d.%d",
                address[0], address[1], address[2], address[3]);
        return temp;
    }

    uint IPAddress::toUInt32(bool bigEndian) const {
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

    IPAddress& IPAddress::operator=(const String &value) {
        if (value.isNullOrEmpty())
            this->operator=(Empty);
        else {
            IPAddress ip;
            if (IPAddress::parse(value, ip))
                this->operator=(ip);
        }
        return *this;
    }

    IPAddress& IPAddress::operator=(const IPAddress &value) {
        memcpy(this->address, value.address, Count);
        return *this;
    }

    bool IPAddress::operator==(const IPAddress &value) const {
        for (uint i = 0; i < Count; i++) {
            if (this->address[i] != value.address[i])
                return false;
        }
        return true;
    }

    bool IPAddress::operator!=(const IPAddress &value) const {
        return !operator==(value);
    }

    IPAddress::operator String() const {
        return toString();
    }

    bool IPAddress::isEmpty() const {
        for (uint i = 0; i < Count; i++) {
            if (this->address[i] != 0)
                return false;
        }
        return true;
    }

    void IPAddress::empty() {
        memset(this->address, 0, Count);
    }

    bool IPAddress::parse(const String &str, IPAddress &address) {
        int addr[Count];
        uint len = 0;
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
        for (uint i = 0; i < Count; i++) {
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
            for (uint i = 0; i < Count; i++) {
                uint8_t temp = ~netmask.address[i];
                broadcast.address[i] = network.address[i] | temp;
            }
            return true;
        }
        return false;
    }

    bool IPAddress::isSubnet(const IPAddress &ip1, const IPAddress &ip2, const IPAddress &netmask) {
        for (uint i = 0; i < Count; i++) {
            uint8_t b1 = ip1.address[i] & netmask.address[i];
            uint8_t b2 = ip2.address[i] & netmask.address[i];
            if (b1 != b2)
                return false;
        }
        return true;
    }

    bool IPAddress::isStartToEnd(const IPAddress &start, const IPAddress &end, const IPAddress &netmask) {
        uint n = ~netmask.toUInt32();
        uint s = start.toUInt32() & n;
        uint e = end.toUInt32() & n;
        return s <= e;
    }

    IPAddresses::IPAddresses() {
    }

    String IPAddresses::toString(const char splitSymbol) const {
        String str;
        for (uint i = 0; i < _addresses.count(); i++) {
            if (str.length() > 0)
                str.append(splitSymbol);
            str.append(_addresses[i]);
        }
        return str;
    }

    void IPAddresses::write(Stream *stream) const {
        stream->writeByte((uint8_t) _addresses.count());
        for (uint i = 0; i < _addresses.count(); i++) {
            _addresses[i].write(stream);
        }
    }

    void IPAddresses::read(Stream *stream) {
        clear();
        uint8_t count = stream->readByte();
        for (uint i = 0; i < count; i++) {
            IPAddress address;
            address.read(stream);
            add(address);
        }
    }

    IPAddresses& IPAddresses::operator=(const String &value) {
        clear();
        String str = value;
        str = String::replace(str, ";", " ");
        str = String::replace(str, ",", " ");
        parse(value, *this);
        return *this;
    }

    IPAddresses& IPAddresses::operator=(const IPAddresses &value) {
        clear();
        for (uint i = 0; i < value._addresses.count(); i++) {
            add(value._addresses[i]);
        }
        return *this;
    }

    bool IPAddresses::operator==(const IPAddresses &value) const {
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] != value._addresses[i])
                return false;
        }
        return true;
    }

    bool IPAddresses::operator!=(const IPAddresses &value) const {
        return !operator==(value);
    }

    IPAddresses::operator String() const {
        return toString();
    }

    bool IPAddresses::isEmpty() const {
        return _addresses.count() == 0;
    }

    void IPAddresses::empty() {
        clear();
    }

    bool IPAddresses::parse(const String &str, IPAddresses &value, const char splitSymbol) {
        value.clear();

        StringArray texts;
        Convert::splitStr(str, texts, splitSymbol);
        for (uint i = 0; i < texts.count(); i++) {
            const String ipStr = texts[i].trim();
            IPAddress address;
            if (!IPAddress::parse(ipStr, address))
                return false;

            value.add(address);
        }
        return true;
    }

    size_t IPAddresses::count() const {
        return _addresses.count();
    }

    IPAddress IPAddresses::at(size_t pos) const {
        if (pos < count()) {
            return _addresses[pos];
        }
        return IPAddress::Empty;
    }

    void IPAddresses::add(const IPAddress &address) {
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] == address)
                return;
        }
        _addresses.add(address);
    }

    void IPAddresses::add(const String &address) {
        IPAddress ip;
        if (IPAddress::parse(address, ip))
            _addresses.add(ip);
    }

    void IPAddresses::remove(const IPAddress &address) {
        Vector<IPAddress> temp;
        for (uint i = 0; i < _addresses.count(); i++) {
            if (_addresses[i] == address)
                temp.add(address);
        }

        for (uint i = 0; i < temp.count(); i++) {
            _addresses.remove(temp[i]);
        }
    }

    void IPAddresses::remove(const String &address) {
        IPAddress ip;
        if (IPAddress::parse(address, ip))
            _addresses.remove(ip);
    }

    void IPAddresses::clear() {
        _addresses.clear();
    }

    const Endpoint Endpoint::Empty = Endpoint();

    Endpoint::Endpoint(const String &address, int port) {
        this->address = address;
        this->port = port;
    }

    Endpoint::Endpoint(const Endpoint &endpoint) {
        this->address = endpoint.address;
        this->port = endpoint.port;
    }

    bool Endpoint::isEmpty() const {
        return address.isNullOrEmpty() || port == 0;
    }

    void Endpoint::empty() {
        address.empty();
        port = 0;
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

    Endpoint& Endpoint::operator=(const Endpoint &value) {
        address = value.address;
        port = value.port;
        return *this;
    }

    bool Endpoint::operator==(const Endpoint &value) const {
        return port == value.port && address == value.address;
    }

    bool Endpoint::operator!=(const Endpoint &value) const {
        return !operator==(value);
    }

    bool Endpoint::operator>(const Endpoint &value) const {
        return compareTo(value) > 0;
    }

    bool Endpoint::operator>=(const Endpoint &value) const {
        return compareTo(value) >= 0;
    }

    bool Endpoint::operator<(const Endpoint &value) const {
        return compareTo(value) < 0;
    }

    bool Endpoint::operator<=(const Endpoint &value) const {
        return compareTo(value) <= 0;
    }

    int Endpoint::compareTo(const Endpoint &value) const {
        if (port > value.port) {
            return 1;
        } else if (port < value.port) {
            return -1;
        }

        if (address > value.address) {
            return 1;
        } else if (address < value.address) {
            return -1;
        }
        return 0;
    }

    void Endpoint::write(Stream *stream, bool bigEndian) const {
        address.write(stream, String::StreamLength1);
        stream->writeInt32(port, bigEndian);
    }

    void Endpoint::read(Stream *stream, bool bigEndian) {
        address.read(stream, String::StreamLength1);
        port = stream->readInt32(bigEndian);
    }

    Endpoint *Endpoint::clone() const {
        return new Endpoint(address, port);
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

    Endpoints::Endpoints() {
    }

    void Endpoints::add(const Endpoint &value) {
        _items.add(Endpoint(value));
    }

    void Endpoints::addRange(const Endpoints &value) {
        for (uint i = 0; i < value.count(); i++) {
            const Endpoint &endpoint = value._items[i];
            add(endpoint);
        }
    }

    Endpoint Endpoints::at(size_t pos) const {
        return _items.at(pos);
    }

    size_t Endpoints::count() const {
        return _items.count();
    }

    bool Endpoints::contains(const Endpoint &value) {
        for (uint i = 0; i < count(); i++) {
            const Endpoint &endpoint = _items[i];
            if (endpoint == value) {
                return true;
            }
        }
        return false;
    }

    void Endpoints::clear() {
        _items.clear();
    }

    bool Endpoints::remove(const Endpoint &value) {
        for (uint i = 0; i < count(); i++) {
            const Endpoint &endpoint = _items[i];
            if (endpoint == value) {
                return removeAt(i);
            }
        }
        return false;
    }

    bool Endpoints::removeAt(size_t i) {
        return _items.removeAt(i);
    }

    Endpoints& Endpoints::operator=(const Endpoints &value) {
        clear();
        for (uint i = 0; i < value.count(); i++) {
            const Endpoint &endpoint = value[i];
            add(endpoint);
        }
        return *this;
    }

    bool Endpoints::operator==(const Endpoints &value) const {
        if (count() != value.count())
            return false;

        for (uint i = 0; i < value.count(); i++) {
            const Endpoint &endpoint1 = value[i];
            const Endpoint &endpoint2 = at(i);
            if (endpoint1 != endpoint2)
                return false;
        }
        return true;
    }

    bool Endpoints::operator!=(const Endpoints &value) const {
        return !operator==(value);
    }

    void Endpoints::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt32((uint32_t) count(), bigEndian);
        for (uint32_t i = 0; i < (uint32_t) count(); i++) {
            const Endpoint &endpoint = at(i);
            endpoint.write(stream, bigEndian);
        }
    }

    void Endpoints::read(Stream *stream, bool bigEndian) {
        size_t count = stream->readUInt32(bigEndian);
        for (uint i = 0; i < count; i++) {
            Endpoint endpoint;
            endpoint.read(stream, bigEndian);
            add(endpoint);
        }
    }

    const P2PEndpoint P2PEndpoint::Empty = P2PEndpoint();

    P2PEndpoint::P2PEndpoint(const Endpoint &local, const Endpoint &peer) : local(local), peer(peer) {
    }

    P2PEndpoint::P2PEndpoint(const P2PEndpoint &endpoint) : P2PEndpoint(endpoint.local, endpoint.peer) {
    }

    bool P2PEndpoint::isEmpty() const {
        return local.isEmpty() && peer.isEmpty();
    }

    String P2PEndpoint::toString() const {
        return String::convert("local: %s, peer: %s", local.toString().c_str(), peer.toString().c_str());
    }

    P2PEndpoint& P2PEndpoint::operator=(const P2PEndpoint &value) {
        this->local = value.local;
        this->peer = value.peer;
        return *this;
    }

    bool P2PEndpoint::operator==(const P2PEndpoint &value) const {
        return this->local == value.local &&
               this->peer == value.peer;
    }

    bool P2PEndpoint::operator!=(const P2PEndpoint &value) const {
        return !operator==(value);
    }

    bool P2PEndpoint::operator>(const P2PEndpoint &value) const {
        return compareTo(value) > 0;
    }

    bool P2PEndpoint::operator>=(const P2PEndpoint &value) const {
        return compareTo(value) >= 0;
    }

    bool P2PEndpoint::operator<(const P2PEndpoint &value) const {
        return compareTo(value) < 0;
    }

    bool P2PEndpoint::operator<=(const P2PEndpoint &value) const {
        return compareTo(value) <= 0;
    }

    int P2PEndpoint::compareTo(const P2PEndpoint &value) const {
        if (local > value.local) {
            return 1;
        } else if (local < value.local) {
            return -1;
        }

        if (peer > value.peer) {
            return 1;
        } else if (peer < value.peer) {
            return -1;
        }
        return 0;
    }

    void P2PEndpoint::write(Stream *stream, bool bigEndian) const {
        local.write(stream, bigEndian);
        peer.write(stream, bigEndian);
    }

    void P2PEndpoint::read(Stream *stream, bool bigEndian) {
        local.read(stream, bigEndian);
        peer.read(stream, bigEndian);
    }

    Port Port::Empty;
    Port::Port(ushort value) {
        _value = value;
    }

    Port::Port(const Port &port) {
        _value = port._value;
    }

    Port::operator ushort() const {
        return _value;
    }

    Port& Port::operator=(const ushort &value) {
        _value = value;
        return *this;
    }

    Port& Port::operator=(const String &value) {
        parse(value, *this);
        return *this;
    }

    Port& Port::operator=(const Port &value) {
        _value = value._value;
        return *this;
    }

    bool Port::operator==(const Port &value) const {
        return _value == value._value;
    }

    bool Port::operator!=(const Port &value) const {
        return !operator==(value);
    }

    void Port::read(Stream *stream) {
        _value = stream->readUInt16();
    }

    void Port::write(Stream *stream) const {
        stream->writeUInt16(_value);
    }

    bool Port::isEmpty() const {
        return _value == DefaultPort;
    }

    String Port::toString() const {
        return ((UInt16) _value).toString();
    }

    bool Port::parse(const String &str, Port &port) {
        ushort value;
        if (UInt16::parse(str, value) &&
            (value >= 1 && value <= 65533)) {
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

    bool Port::parse(const String &str, ushort &port) {
        Port p;
        if (Port::parse(str, p)) {
            port = p;
            return true;
        }
        return false;
    }

    PortRange::PortRange(int from, int to) {
        this->from = from;
        this->to = to;
    }

    PortRange::PortRange(const PortRange &pr) {
        this->from = pr.from;
        this->to = pr.to;
    }

    PortRange& PortRange::operator=(const PortRange &value) {
        this->from = value.from;
        this->to = value.to;
        return *this;
    }

    bool PortRange::operator==(const PortRange &value) const {
        return this->from == value.from &&
               this->to == value.to;
    }

    bool PortRange::operator!=(const PortRange &value) const {
        return !operator==(value);
    }

    void PortRange::read(Stream *stream) {
        this->from = stream->readInt32();
        this->to = stream->readInt32();
    }

    void PortRange::write(Stream *stream) const {
        ((Int32) this->from).write(stream);
        ((Int32) this->to).write(stream);
    }

    String PortRange::toString() const {
        if (from == to) {
            return from == DefaultPort ? String::Empty : Int32(from).toString();
        } else {
            return String::convert("%d-%d", from, to);
        }
    }

    bool PortRange::parse(const String &str, PortRange &pr) {
        const int MinPort = 1;
        const int MaxPort = 65533;
        String temp = str.trim();
        int port;
        if (Int32::parse(temp, port)) {
            if (!(port >= MinPort && port <= MaxPort))
                return false;

            pr = PortRange(port, port);
            return true;
        }
        StringArray items;
        StringArray::parse(temp, items, '-');
        if (items.count() == 2) {
            int from = 0, to = 0;
            if (Int32::parse(items[0], from) &&
                Int32::parse(items[1], to) &&
                to >= from) {
                if (!(from >= MinPort && from <= MaxPort))
                    return false;
                if (!(to >= MinPort && to <= MaxPort))
                    return false;

                pr = PortRange(from, to);
                return true;
            }
        }
        return false;
    }

    const String Url::SchemeHttp = "http";
    const String Url::SchemeHttps = "https";
    const String Url::SchemeWebSocket = "ws";
    const String Url::SchemeWebSockets = "wss";
    const String Url::SchemeEaseTcp = "ease.tcp";
    const String Url::SchemeEaseTcps = "ease.tcps";
    const String Url::SchemeMysql = "mysql";
    const String Url::SchemeMysqls = "mysqls";
    const String Url::SchemeMqtt = "mqtt";
    const String Url::SchemeMqtts = "mqtts";
    const String Url::SchemeCoap = "coap";
    const Url Url::Empty;

    Url::Url(const String &uriString) {
        Url::parse(uriString, *this);
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

    bool Url::isDefaultPort() const {
        bool isDefaultPort = false;
        if (_scheme == SchemeHttp)
            isDefaultPort = _endpoint.port == DefaultHttpPort;
        else if (_scheme == SchemeHttps)
            isDefaultPort = _endpoint.port == DefaultHttpsPort;
        return isDefaultPort;
    }

    const Endpoint &Url::endpoint() const {
        return _endpoint;
    }

    const String &Url::address() const {
        return _endpoint.address;
    }

    int Url::port() const {
        return _endpoint.port;
    }

    const String &Url::scheme() const {
        return _scheme;
    }

    bool Url::isSecure() const {
        return _scheme == SchemeHttps || _scheme == SchemeEaseTcps || _scheme == SchemeWebSockets;
    }

    const String &Url::relativeUrl() const {
        return _relativeUrl;
    }

    bool Url::isHttpScheme() const {
        return _scheme == SchemeHttp || _scheme == SchemeHttps;
    }

    bool Url::isWebSocketScheme() const {
        return _scheme == SchemeHttp || _scheme == SchemeHttps;
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

    Url& Url::operator=(const Url &value) {
        this->_scheme = value._scheme;
        this->_endpoint = value._endpoint;
        this->_relativeUrl = value._relativeUrl;
        return *this;
    }

    bool Url::operator==(const Url &value) const {
        return this->_scheme == value._scheme &&
               this->_endpoint == value._endpoint &&
               this->_relativeUrl == value._relativeUrl;
    }

    bool Url::operator!=(const Url &value) const {
        return !operator==(value);
    }

    bool Url::operator>(const Url &value) const {
        return toString() > value.toString();
    }

    bool Url::operator>=(const Url &value) const {
        return toString() >= value.toString();
    }

    bool Url::operator<(const Url &value) const {
        return toString() < value.toString();
    }

    bool Url::operator<=(const Url &value) const {
        return toString() <= value.toString();
    }

    bool Url::parse(const String &uriString, Url &value) {
        if (uriString.isNullOrEmpty())
            return false;

        static const Regex urlRegex(
                "^(http|https|ws|wss|ease.tcp|ease.tcps|mysql|mysqls|mqtt|mqtts|coap)://([a-z|A-Z|0-9|.|_-]+)(:\\d*|\\S*)([a-zA-Z0-9=\\-\\.\\?\\,\\'\\/\\\\+&%\\$#_]*)$");
        StringArray groups;
        bool result = urlRegex.match(uriString, groups);
        if (result) {
//            for (uint i=0; i<groups.count(); i++)
//            {
//                printf("group item: %s\n", groups[i].c_str());
//            }
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
