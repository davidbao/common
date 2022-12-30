//
//  NetTypeTest.cpp
//  common
//
//  Created by baowei on 2022/9/24.
//  Copyright © 2022 com. All rights reserved.
//

#include "net/NetType.h"
#include "IO/MemoryStream.h"
#include "data/StringArray.h"

using namespace Net;

bool testMacAddressConstructor() {
    {
        MacAddress test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        uint64_t value = 0x309c23c51028;
        MacAddress test(value);
        if (test.isEmpty()) {
            return false;
        }
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        if (memcmp(test.address, address, sizeof(address)) != 0) {
            return false;
        }
    }
    {
        uint64_t value = 0x309c23c51028;
        MacAddress test(value, true);
        if (test.isEmpty()) {
            return false;
        }
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        if (memcmp(test.address, address, sizeof(address)) != 0) {
            return false;
        }
    }
    {
        uint64_t value = 0x2810c5239c30;
        MacAddress test(value, false);
        if (test.isEmpty()) {
            return false;
        }
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        if (memcmp(test.address, address, sizeof(address)) != 0) {
            return false;
        }
    }
    {
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        MacAddress test(address);
        if (test.isEmpty()) {
            return false;
        }
        if (memcmp(test.address, address, sizeof(address)) != 0) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        if (test.isEmpty()) {
            return false;
        }
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        if (memcmp(test.address, address, sizeof(address)) != 0) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2(test);
        uint8_t address[MacAddress::Count] = {0x30, 0x9c, 0x23, 0xc5, 0x10, 0x28};
        if (memcmp(test2.address, address, sizeof(address)) != 0) {
            return false;
        }
    }

    return true;
}

bool testMacAddressEquals() {
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testMacAddressComparison() {
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test > test2) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test >= test2) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test2 < test) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2("40:9c:23:c5:10:28");
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testMacAddressEmpty() {
    {
        MacAddress test("30:9c:23:c5:10:28");
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        MacAddress test("130:9c:23:c5:10:28");
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testMacAddressToString() {
    {
        MacAddress test("30:9c:23:c5:10:28");
        if (test.toString() != "30:9c:23:c5:10:28") {
            return false;
        }
    }
    {
        MacAddress test("30:9C:23:c5:10:28");
        if (test.toString() != "30:9c:23:c5:10:28") {
            return false;
        }
    }

    {
        MacAddress test("30:9c:23:c5:10:28");
        String str = test;
        if (str != "30:9c:23:c5:10:28") {
            return false;
        }
    }
    {
        MacAddress test("30:9C:23:c5:10:28");
        String str = test;
        if (str != "30:9c:23:c5:10:28") {
            return false;
        }
    }

    return true;
}

bool testMacAddressToUInt64() {
    {
        uint64_t value = 0x309c23c51028;
        MacAddress test("30:9C:23:c5:10:28");
        if (test.toUInt64() != value) {
            return false;
        }
    }
    {
        uint64_t value = 0x309c23c51028;
        MacAddress test("30:9C:23:c5:10:28");
        if (test.toUInt64(true) != value) {
            return false;
        }
    }
    {
        uint64_t value = 0x2810c5239c30;
        MacAddress test("30:9C:23:c5:10:28");
        if (test.toUInt64(false) != value) {
            return false;
        }
    }

    return true;
}

bool testMacAddressStream() {
    {
        MemoryStream stream;
        MacAddress test("30:9C:23:c5:10:28");
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        MacAddress test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testMacAddressOperator() {
    {
        String value = "30:9c:23:c5:10:28";
        MacAddress test = value;
        if (test.toString() != value) {
            return false;
        }
    }
    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testMacAddressStatic() {
    {
        if (!MacAddress::Empty.isEmpty()) {
            return false;
        }
    }

    {
        String str = "30:9c:23:c5:10:28";
        MacAddress test;
        if (!MacAddress::parse(str, test)) {
            return false;
        }
        if (test.toString() != str) {
            return false;
        }
    }
    {
        String str = "130:9c:23:c5:10:28";
        MacAddress test;
        if (MacAddress::parse(str, test)) {
            return false;
        }
    }

    {
        MacAddress test("30:9c:23:c5:10:28");
        MacAddress test2;
        if (!MacAddress::increase(test, test2)) {
            return false;
        }
        if (test2.toString() != "30:9c:23:c5:10:29") {
            return false;
        }
    }

    {
        MacAddress test = MacAddress::randomLocalAddress();
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        MacAddress test = MacAddress::randomLocalAddress(0x55);
        if (test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testMacAddressesConstructor() {
    {
        MacAddresses test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        MacAddresses test("30:9c:23:c5:10:28 30:9c:23:c5:10:29");
        if (!(test[0] == MacAddress("30:9c:23:c5:10:28") && test[1] == MacAddress("30:9c:23:c5:10:29"))) {
            return false;
        }
    }
    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        if (!(test[0] == MacAddress("30:9c:23:c5:10:28") && test[1] == MacAddress("30:9c:23:c5:10:29"))) {
            return false;
        }
    }
    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        MacAddresses test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testMacAddressesToString() {
    {
        MacAddresses test;
        if (test.toString() != "") {
            return false;
        }
    }
    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        if (test.toString() != "30:9c:23:c5:10:28 30:9c:23:c5:10:29") {
            return false;
        }
    }
    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        if (test.toString(';') != "30:9c:23:c5:10:28;30:9c:23:c5:10:29") {
            return false;
        }
    }

    return true;
}

bool testMacAddressesStream() {
    {
        MemoryStream stream;
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        MacAddresses test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testMacAddressesOperator() {
    {
        String str = "30:9c:23:c5:10:28 30:9c:23:c5:10:29";
        MacAddresses test;
        test = str;
        if (!(test[0] == MacAddress("30:9c:23:c5:10:28") && test[1] == MacAddress("30:9c:23:c5:10:29"))) {
            return false;
        }
    }
    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        MacAddresses test2;
        test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    {
        MacAddresses test{MacAddress("30:9c:23:c5:10:28"), MacAddress("30:9c:23:c5:10:29")};
        String str = test;
        if (str != "30:9c:23:c5:10:28 30:9c:23:c5:10:29") {
            return false;
        }
    }

    return true;
}

bool testMacAddressesParse() {
    {
        String str = "30:9c:23:c5:10:28 30:9c:23:c5:10:29";
        MacAddresses test;
        if (!MacAddresses::parse(str, test)) {
            return false;
        }
        if (!(test[0] == MacAddress("30:9c:23:c5:10:28") && test[1] == MacAddress("30:9c:23:c5:10:29"))) {
            return false;
        }
    }
    {
        String str = "30:9c:23:c5:10:28;30:9c:23:c5:10:29";
        MacAddresses test;
        if (!MacAddresses::parse(str, test, ';')) {
            return false;
        }
        if (!(test[0] == MacAddress("30:9c:23:c5:10:28") && test[1] == MacAddress("30:9c:23:c5:10:29"))) {
            return false;
        }
    }

    return true;
}

bool testIPAddressConstructor() {
    {
        IPAddress test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        uint8_t addr[4]{192, 167, 0, 1};
        IPAddress test(addr);
        if (test.toString() != "192.167.0.1") {
            return false;
        }
    }
    {
        uint32_t addr = 0xC0A70001;
        IPAddress test(addr);
        if (test.toString() != "192.167.0.1") {
            return false;
        }
    }
    {
        uint32_t addr = 0xC0A70001;
        IPAddress test(addr, true);
        if (test.toString() != "192.167.0.1") {
            return false;
        }
    }
    {
        uint32_t addr = 0x0100A7C0;
        IPAddress test(addr, false);
        if (test.toString() != "192.167.0.1") {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressEquals() {
    {
        IPAddress test("192.167.0.1");
        IPAddress test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        IPAddress test("192.167.0.1");
        IPAddress test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressComparison() {
    {
        IPAddress test("192.167.0.1");
        IPAddress test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test > test2) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test >= test2) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test2 < test) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2("192.167.0.2");
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testIPAddressEmpty() {
    {
        IPAddress test("192.167.0.1");
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        IPAddress test("1192.167.0.1");
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testIPAddressToString() {
    {
        IPAddress test("192.167.0.1");
        if (test.toString() != "192.167.0.1") {
            return false;
        }
    }

    {
        IPAddress test("192.167.0.1");
        String str = test;
        if (str != "192.167.0.1") {
            return false;
        }
    }

    return true;
}

bool testIPAddressToUInt32() {
    {
        uint32_t value = 0xC0A70001;
        IPAddress test("192.167.0.1");
        if (test.toUInt32() != value) {
            return false;
        }
    }
    {
        uint32_t value = 0xC0A70001;
        IPAddress test("192.167.0.1");
        if (test.toUInt32(true) != value) {
            return false;
        }
    }
    {
        uint32_t value = 0x0100A7C0;
        IPAddress test("192.167.0.1");
        if (test.toUInt32(false) != value) {
            return false;
        }
    }

    return true;
}

bool testIPAddressStream() {
    {
        MemoryStream stream;
        IPAddress test("192.167.0.1");
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        IPAddress test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressOperator() {
    {
        String value = "192.167.0.1";
        IPAddress test = value;
        if (test.toString() != value) {
            return false;
        }
    }
    {
        IPAddress test("192.167.0.1");
        IPAddress test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressStatic() {
    {
        if (!IPAddress::Empty.isEmpty()) {
            return false;
        }
    }

    {
        String str = "192.167.0.1";
        IPAddress test;
        if (!IPAddress::parse(str, test)) {
            return false;
        }
        if (test.toString() != str) {
            return false;
        }
    }
    {
        String str = "1192.167.0.1";
        IPAddress test;
        if (IPAddress::parse(str, test)) {
            return false;
        }
    }

    {
        if (!IPAddress::isIPAddress("192.167.0.1")) {
            return false;
        }
        if (IPAddress::isIPAddress("1192.167.0.1")) {
            return false;
        }
    }

    {
        IPAddress test;
        if (!IPAddress::getNetworkAddress("192.167.0.1", "255.255.255.0", test)) {
            return false;
        }
        if (test.toString() != "192.167.0.0") {
            return false;
        }
    }
    {
        IPAddress test;
        if (!IPAddress::getNetworkAddress(IPAddress("192.167.0.1"), IPAddress("255.255.255.0"), test)) {
            return false;
        }
        if (test.toString() != "192.167.0.0") {
            return false;
        }
    }

    {
        IPAddress test;
        if (!IPAddress::getBroadcastAddress("192.167.0.1", "255.255.255.0", test)) {
            return false;
        }
        if (test.toString() != "192.167.0.255") {
            return false;
        }
    }
    {
        IPAddress test;
        if (!IPAddress::getBroadcastAddress(IPAddress("192.167.0.1"), IPAddress("255.255.255.0"), test)) {
            return false;
        }
        if (test.toString() != "192.167.0.255") {
            return false;
        }
    }

    {
        if (!IPAddress::isSubnet(("192.167.0.1"), ("192.167.0.6"), ("255.255.255.0"))) {
            return false;
        }
        if (IPAddress::isSubnet(("192.167.0.1"), ("192.168.0.6"), ("255.255.255.0"))) {
            return false;
        }
        if (!IPAddress::isSubnet(("10.1.1.1"), ("10.8.8.8"), ("255.0.0.0"))) {
            return false;
        }
    }
    {
        if (!IPAddress::isSubnet(IPAddress("192.167.0.1"), IPAddress("192.167.0.6"), IPAddress("255.255.255.0"))) {
            return false;
        }
        if (IPAddress::isSubnet(IPAddress("192.167.0.1"), IPAddress("192.168.0.6"), IPAddress("255.255.255.0"))) {
            return false;
        }
        if (!IPAddress::isSubnet(IPAddress("10.1.1.1"), IPAddress("10.8.8.8"), IPAddress("255.0.0.0"))) {
            return false;
        }
    }

    {
        if (!IPAddress::isStartToEnd(("192.167.0.1"), ("192.167.0.6"), ("255.255.255.0"))) {
            return false;
        }
        if (IPAddress::isStartToEnd(("192.167.0.6"), ("192.167.0.1"), ("255.255.255.0"))) {
            return false;
        }
    }
    {
        if (!IPAddress::isStartToEnd(IPAddress("192.167.0.1"), IPAddress("192.167.0.6"), IPAddress("255.255.255.0"))) {
            return false;
        }
        if (IPAddress::isStartToEnd(IPAddress("192.167.0.6"), IPAddress("192.167.0.1"), IPAddress("255.255.255.0"))) {
            return false;
        }
    }

    return true;
}

bool testIPAddressesConstructor() {
    {
        IPAddresses test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        IPAddresses test("192.167.0.1 192.167.0.2");
        if (!(test[0] == IPAddress("192.167.0.1") && test[1] == IPAddress("192.167.0.2"))) {
            return false;
        }
    }
    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        if (!(test[0] == IPAddress("192.167.0.1") && test[1] == IPAddress("192.167.0.2"))) {
            return false;
        }
    }
    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        IPAddresses test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressesToString() {
    {
        IPAddresses test;
        if (test.toString() != "") {
            return false;
        }
    }
    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        if (test.toString() != "192.167.0.1 192.167.0.2") {
            return false;
        }
    }
    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        if (test.toString(';') != "192.167.0.1;192.167.0.2") {
            return false;
        }
    }

    return true;
}

bool testIPAddressesStream() {
    {
        MemoryStream stream;
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        IPAddresses test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testIPAddressesOperator() {
    {
        String str = "192.167.0.1 192.167.0.2";
        IPAddresses test;
        test = str;
        if (!(test[0] == IPAddress("192.167.0.1") && test[1] == IPAddress("192.167.0.2"))) {
            return false;
        }
    }
    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        IPAddresses test2;
        test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    {
        IPAddresses test{IPAddress("192.167.0.1"), IPAddress("192.167.0.2")};
        String str = test;
        if (str != "192.167.0.1 192.167.0.2") {
            return false;
        }
    }

    return true;
}

bool testIPAddressesParse() {
    {
        String str = "192.167.0.1 192.167.0.2";
        IPAddresses test;
        if (!IPAddresses::parse(str, test)) {
            return false;
        }
        if (!(test[0] == IPAddress("192.167.0.1") && test[1] == IPAddress("192.167.0.2"))) {
            return false;
        }
    }
    {
        String str = "192.167.0.1;192.167.0.2";
        IPAddresses test;
        if (!IPAddresses::parse(str, test, ';')) {
            return false;
        }
        if (!(test[0] == IPAddress("192.167.0.1") && test[1] == IPAddress("192.167.0.2"))) {
            return false;
        }
    }

    return true;
}

bool testPortConstructor() {
    {
        Port test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Port test(80);
        if (test.isEmpty()) {
            return false;
        }
        if (test != 80) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(test);
        if (test != test2) {
            return false;
        }
    }
    {
        Port test(65535);
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPortEquals() {
    {
        Port test(80);
        Port test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2;
        test2.evaluates(80);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Port test(80);
        if (!test.equals(80)) {
            return false;
        }
    }

    {
        Port test(80);
        Port test2;
        test2.evaluates(80);
        if (test != test2) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2;
        test2.evaluates(80);
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testPortComparison() {
    {
        Port test(80);
        Port test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        Port test(80);
        Port test2(81);
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        Port test(80);
        Port test2(81);
        if (test > test2) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test >= test2) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test2 < test) {
            return false;
        }
    }
    {
        Port test(80);
        Port test2(81);
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testPortEmpty() {
    {
        Port test(80);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Port test(80);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Port test(0);
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPortToString() {
    {
        Port test(80);
        if (test.toString() != "80") {
            return false;
        }
    }
    {
        Port test(80);
        if (test.toString() != "80") {
            return false;
        }
    }

    {
        Port test(80);
        String str = test;
        if (str != "80") {
            return false;
        }
    }
    {
        Port test(80);
        String str = test;
        if (str != "80") {
            return false;
        }
    }

    return true;
}

bool testPortStream() {
    {
        MemoryStream stream;
        Port test(80);
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Port test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPortOperator() {
    {
        Port test = 80;
        if (test.toString() != "80") {
            return false;
        }
    }
    {
        Port test(80);
        Port test2 = test;
        if (test != test2) {
            return false;
        }
    }
    {
        Port test;
        test = "80";
        if (test.toString() != "80") {
            return false;
        }
    }

    return true;
}

bool testPortStatic() {
    {
        if (!Port::Empty.isEmpty()) {
            return false;
        }
        if (Port::DefaultValue != 0) {
            return false;
        }
        if (Port::MinValue != 1) {
            return false;
        }
        if (Port::MaxValue != 65533) {
            return false;
        }
    }

    {
        Port test;
        if (!Port::parse("80", test)) {
            return false;
        }
        if (test != 80) {
            return false;
        }
    }
    {
        int test;
        if (!Port::parse("80", test)) {
            return false;
        }
        if (test != 80) {
            return false;
        }
    }
    {
        uint16_t test;
        if (!Port::parse("80", test)) {
            return false;
        }
        if (test != 80) {
            return false;
        }
    }

    {
        Port test;
        if (Port::parse("100000", test)) {
            return false;
        }
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPortRangeConstructor() {
    {
        PortRange test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        PortRange test(1, 65535);
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPortRangeEquals() {
    {
        PortRange test(1, 100);
        PortRange test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        PortRange test(1, 100);
        PortRange test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPortRangeComparison() {
    {
        PortRange test(1, 100);
        PortRange test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test > test2) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test >= test2) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test2 < test) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        PortRange test2(2, 100);
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testPortRangeEmpty() {
    {
        PortRange test(1, 100);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        PortRange test(0, 65535);
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testPortRangeToString() {
    {
        PortRange test(1, 100);
        if (test.toString() != "1-100") {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        if (test.toString() != "1-100") {
            return false;
        }
    }

    {
        PortRange test(1, 100);
        String str = test;
        if (str != "1-100") {
            return false;
        }
    }
    {
        PortRange test(1, 100);
        String str = test;
        if (str != "1-100") {
            return false;
        }
    }

    return true;
}

bool testPortRangeStream() {
    {
        MemoryStream stream;
        PortRange test(1, 100);
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        PortRange test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testPortRangeStatic() {
    {
        String str = "1-100";
        PortRange test;
        if (!PortRange::parse(str, test)) {
            return false;
        }
        if (test.toString() != str) {
            return false;
        }
    }
    {
        String str = "0-65535";
        PortRange test;
        if (PortRange::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testEndpointConstructor() {
    {
        Endpoint test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 65535);
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("localhost", 80);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2(test);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testEndpointEquals() {
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testEndpointComparison() {
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test > test2) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test >= test2) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test2 < test) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        Endpoint test2("192.167.0.2", 80);
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testEndpointEmpty() {
    {
        Endpoint test("192.167.0.1", 80);
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 65535);
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testEndpointToString() {
    {
        Endpoint test("192.167.0.1", 80);
        if (test.toString() != "192.167.0.1:80") {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        if (test.toString() != "192.167.0.1:80") {
            return false;
        }
    }

    {
        Endpoint test("192.167.0.1", 80);
        String str = test;
        if (str != "192.167.0.1:80") {
            return false;
        }
    }
    {
        Endpoint test("192.167.0.1", 80);
        String str = test;
        if (str != "192.167.0.1:80") {
            return false;
        }
    }

    return true;
}

bool testEndpointStream() {
    {
        MemoryStream stream;
        Endpoint test("192.167.0.1", 80);
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        Endpoint test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testEndpointStatic() {
    {
        String str = "192.167.0.1:80";
        Endpoint test;
        if (!Endpoint::parse(str, test)) {
            return false;
        }
        if (test.toString() != str) {
            return false;
        }
    }
    {
        String str = "0-65535";
        Endpoint test;
        if (Endpoint::parse(str, test)) {
            return false;
        }
    }

    return true;
}

bool testEndpointIsAnyAddress() {
    {
        Endpoint test("any", 80);
        if (!test.isAnyAddress()) {
            return false;
        }
    }
    {
        Endpoint test("", 80);
        if (!test.isAnyAddress()) {
            return false;
        }
    }
    {
        Endpoint test("0.0.0.0", 80);
        if (!test.isAnyAddress()) {
            return false;
        }
    }
    {
        Endpoint test("localhost", 80);
        if (test.isAnyAddress()) {
            return false;
        }
    }

    return true;
}

bool testUrlConstructor() {
    {
        Url test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        String str = "https://www.baidu.com/";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443/";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "https" && (url.endpoint() == Endpoint("www.baidu.com", 443)))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443/test.html";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "http://www.baidu.com:8080/test.html";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "http" && url.endpoint() == Endpoint("www.baidu.com", 8080))) {
            return false;
        }
    }

    {
        String str = "http://localhost:8080/test.html";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "http" && url.endpoint() == Endpoint("localhost", 8080))) {
            return false;
        }
    }

    {
        String str = "http://localhost:8080/Debug/test.html?accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "http" && url.endpoint() == Endpoint("localhost", 8080))) {
            return false;
        }
    }

    {
        String str = "http://192.168.1.6:8080/Debug/test.html?address=192.168.1.125&accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
        Url url(str);
        if (!(!url.isEmpty() && url.scheme() == "http" && url.endpoint() == Endpoint("192.168.1.6", 8080))) {
            return false;
        }
    }

    {
        String str = "abc://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
        if (url.scheme() != "abc") {
            return false;
        }
    }

    {
        String str = "ws://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "wss://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "ease.tcp://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "ease.tcps://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "mysql://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "mysql://127.0.0.1:3306/test_db";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
        if (url.relativeUrl() != "test_db") {
            return false;
        }
    }
    {
        String str = "mysqls://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "mqtt://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "mqtts://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }
    {
        String str = "coap://localhost:8080/";
        Url url(str);
        if (url.isEmpty()) {
            return false;
        }
    }

    {
        Url test("https://192.167.0.1:8080/", "test.html");
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.scheme() == "https" && test.endpoint() == "192.167.0.1:8080" && test.relativeUrl() == "test.html")) {
            return false;
        }
    }

    {
        Url test(Url("https://192.167.0.1:8080/"), "test.html");
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.scheme() == "https" && test.endpoint() == "192.167.0.1:8080" && test.relativeUrl() == "test.html")) {
            return false;
        }
    }

    {
        Url test("https", Endpoint("192.167.0.1", 8080), "test.html");
        if (test.isEmpty()) {
            return false;
        }
        if (!(test.scheme() == "https" && test.endpoint() == "192.167.0.1:8080" && test.relativeUrl() == "test.html")) {
            return false;
        }
    }

    {
        String str = "coap://localhost:8080/";
        Url url(str);
        Url url2(url);
        if (url != url2) {
            return false;
        }
    }

    return true;
}

bool testUrlEquals() {
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test.equals(test2)) {
            return false;
        }
    }

    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testUrlComparison() {
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test > test2) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test >= test2) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test2 < test) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        Url test2("https://192.167.0.2:8080/test.html");
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testUrlEmpty() {
    {
        Url test("https://192.167.0.1:8080/test.html");
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Url test("https://192.167.0.1:8080/test.html");
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        Url test("abc://192.167.0.1:8080/test.html");
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        Url test("ab~c://192.167.0.1:8080/test.html");
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testUrlToString() {
    {
        Url test("https://192.167.0.1:8080/test.html");
        if (test.toString() != "https://192.167.0.1:8080/test.html") {
            return false;
        }
    }
    {
        Url test("wss://192.167.0.1:8080");
        if (test.toString() != "wss://192.167.0.1:8080/") {
            return false;
        }
    }

    return true;
}

bool testUrlStatic() {
    if (String(Url::SchemeHttp) != "http") {
        return false;
    }
    if (String(Url::SchemeHttps) != "https") {
        return false;
    }
    if (String(Url::SchemeWebSocket) != "ws") {
        return false;
    }
    if (String(Url::SchemeWebSockets) != "wss") {
        return false;
    }
    if (String(Url::SchemeEaseTcp) != "ease.tcp") {
        return false;
    }
    if (String(Url::SchemeEaseTcps) != "ease.tcps") {
        return false;
    }
    if (String(Url::SchemeMysql) != "mysql") {
        return false;
    }
    if (String(Url::SchemeMysqls) != "mysqls") {
        return false;
    }
    if (String(Url::SchemeMqtt) != "mqtt") {
        return false;
    }
    if (String(Url::SchemeMqtts) != "mqtts") {
        return false;
    }
    if (String(Url::SchemeCoap) != "coap") {
        return false;
    }

    if (!Url::Empty.isEmpty()) {
        return false;
    }

    if (Url::Schemes.toString('|') != "http|https|ws|wss|ease.tcp|ease.tcps|mysql|mysqls|mqtt|mqtts|coap") {
        return false;
    }

    return true;
}

bool testUrlParse() {
    {
        String str = "https://www.baidu.com/";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443/";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "https" && (url.endpoint() == Endpoint("www.baidu.com", 443)))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "https://www.baidu.com:443/test.html";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "https" && url.endpoint() == Endpoint("www.baidu.com", 443))) {
            return false;
        }
    }

    {
        String str = "http://www.baidu.com:8080/test.html";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "http" && url.endpoint() == Endpoint("www.baidu.com", 8080))) {
            return false;
        }
    }

    {
        String str = "http://localhost:8080/test.html";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "http" && url.endpoint() == Endpoint("localhost", 8080))) {
            return false;
        }
    }

    {
        String str = "http://localhost:8080/Debug/test.html?accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "http" && url.endpoint() == Endpoint("localhost", 8080))) {
            return false;
        }
    }

    {
        String str = "http://192.168.1.6:8080/Debug/test.html?address=192.168.1.125&accessKey=RDNENUU3NTctODU4MC00NDk0LTlCNkQtOENCOENFODE0OTdFIDk1RkRDODFGN0Q2QTUwMDc1MzlGRjcyNkRBMDc1OEJE&projectid=B5B07187-5EB3-4AB4-9CC6-1F584AD8B012&name=admin";
        Url url;
        if (!(Url::parse(str, url) && url.scheme() == "http" && url.endpoint() == Endpoint("192.168.1.6", 8080))) {
            return false;
        }
    }

    {
        String str = "abc://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }

    {
        String str = "ab@c://localhost:8080/";
        Url url;
        if (Url::parse(str, url)) {
            return false;
        }
    }

    {
        String str = "ws://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "wss://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "ease.tcp://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "ease.tcps://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "mysql://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "mysqls://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "mqtt://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "mqtts://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }
    {
        String str = "coap://localhost:8080/";
        Url url;
        if (!Url::parse(str, url)) {
            return false;
        }
    }

    return true;
}

bool testUrlProperty() {
    {
        if (!Url("http://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("http://localhost:80/").isDefaultPort()) {
            return false;
        }
        if (!Url("https://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("https://localhost:443/").isDefaultPort()) {
            return false;
        }

        if (!Url("mysql://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("mysql://localhost:3306/").isDefaultPort()) {
            return false;
        }
        if (!Url("mysqls://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("mysqls://localhost:13306/").isDefaultPort()) {
            return false;
        }

        if (!Url("mqtt://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("mqtt://localhost:1883/").isDefaultPort()) {
            return false;
        }
        if (!Url("mqtts://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("mqtts://localhost:8883/").isDefaultPort()) {
            return false;
        }

        if (!Url("coap://localhost/").isDefaultPort()) {
            return false;
        }
        if (!Url("coap://localhost:5683/").isDefaultPort()) {
            return false;
        }
    }

    {
        if (!Url("https://localhost:8080/").isSecure()) {
            return false;
        }
        if (Url("http://localhost:8080/").isSecure()) {
            return false;
        }
        if (!Url("mysqls://localhost:8080/").isSecure()) {
            return false;
        }
        if (Url("mysql://localhost:8080/").isSecure()) {
            return false;
        }
        if (!Url("mqtts://localhost:8080/").isSecure()) {
            return false;
        }
        if (Url("mqtt://localhost:8080/").isSecure()) {
            return false;
        }
        if (!Url("ease.tcps://localhost:8080/").isSecure()) {
            return false;
        }
        if (Url("ease.tcp://localhost:8080/").isSecure()) {
            return false;
        }
        if (Url("coap://localhost:8080/").isSecure()) {
            return false;
        }
    }

    {
        if (!Url("http://localhost:8080/").isHttpScheme()) {
            return false;
        }
        if (!Url("https://localhost:8080/").isHttpScheme()) {
            return false;
        }
    }
    {
        if (!Url("ws://localhost:8080/").isWebSocketScheme()) {
            return false;
        }
        if (!Url("wss://localhost:8080/").isWebSocketScheme()) {
            return false;
        }
    }
    {
        if (!Url("ease.tcp://localhost:8080/").isEaseTcpScheme()) {
            return false;
        }
        if (!Url("ease.tcps://localhost:8080/").isEaseTcpScheme()) {
            return false;
        }
    }
    {
        if (!Url("mysql://localhost:8080/").isMysqlScheme()) {
            return false;
        }
        if (!Url("mysqls://localhost:8080/").isMysqlScheme()) {
            return false;
        }
    }
    {
        if (!Url("mqtt://localhost:8080/").isMqttScheme()) {
            return false;
        }
        if (!Url("mqtts://localhost:8080/").isMqttScheme()) {
            return false;
        }
    }
    {
        if (!Url("coap://localhost:8080/").isCoapScheme()) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointConstructor() {
    {
        P2PEndpoint test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        if (!(test.local == "192.167.0.1:1384" && test.peer == "192.167.0.1:80")) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(test);
        if (!test.equals(test2)) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointEquals() {
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2;
        test2.evaluates(test);
        if (!test.equals(test2)) {
            return false;
        }
    }

    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2 = test;
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointComparison() {
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2 = test;
        if (test.compareTo(test2) != 0) {
            return false;
        }
    }

    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test.compareTo(test2) > 0) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test.compareTo(test2) >= 0) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test2.compareTo(test) < 0) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test2.compareTo(test) <= 0) {
            return false;
        }
    }

    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test > test2) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test >= test2) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test2 < test) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2(Endpoint("192.167.0.2", 1384), Endpoint("192.167.0.2", 80));
        if (test2 <= test) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointEmpty() {
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        test.empty();
        if (!test.isEmpty()) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointToString() {
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        if (test.toString() != "192.167.0.1:1384, 192.167.0.1:80") {
            return false;
        }
    }

    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        String str = test;
        if (str != "192.167.0.1:1384, 192.167.0.1:80") {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointStream() {
    {
        MemoryStream stream;
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        test.write(&stream);
        stream.seek(0, SeekOrigin::SeekBegin);
        P2PEndpoint test2;
        test2.read(&stream);
        if (test != test2) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointOperator() {
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2 = test;
        if (test != test2) {
            return false;
        }
    }
    {
        P2PEndpoint test(Endpoint("192.167.0.1", 1384), Endpoint("192.167.0.1", 80));
        P2PEndpoint test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }

    return true;
}

bool testP2PEndpointStatic() {
    {
        if (!P2PEndpoint::Empty.isEmpty()) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testMacAddressConstructor()) {
        return 1;
    }
    if (!testMacAddressEquals()) {
        return 2;
    }
    if (!testMacAddressComparison()) {
        return 3;
    }
    if (!testMacAddressEmpty()) {
        return 4;
    }
    if (!testMacAddressToString()) {
        return 5;
    }
    if (!testMacAddressToUInt64()) {
        return 6;
    }
    if (!testMacAddressStream()) {
        return 7;
    }
    if (!testMacAddressOperator()) {
        return 8;
    }
    if (!testMacAddressStatic()) {
        return 9;
    }
    if (!testMacAddressesConstructor()) {
        return 11;
    }
    if (!testMacAddressesToString()) {
        return 12;
    }
    if (!testMacAddressesStream()) {
        return 13;
    }
    if (!testMacAddressesOperator()) {
        return 14;
    }
    if (!testMacAddressesParse()) {
        return 15;
    }

    if (!testIPAddressConstructor()) {
        return 21;
    }
    if (!testIPAddressEquals()) {
        return 22;
    }
    if (!testIPAddressComparison()) {
        return 23;
    }
    if (!testIPAddressEmpty()) {
        return 24;
    }
    if (!testIPAddressToString()) {
        return 25;
    }
    if (!testIPAddressStream()) {
        return 26;
    }
    if (!testIPAddressOperator()) {
        return 27;
    }
    if (!testIPAddressStatic()) {
        return 28;
    }
    if (!testIPAddressesConstructor()) {
        return 31;
    }
    if (!testIPAddressesToString()) {
        return 32;
    }
    if (!testIPAddressesStream()) {
        return 33;
    }
    if (!testIPAddressesOperator()) {
        return 34;
    }
    if (!testIPAddressesParse()) {
        return 35;
    }
    if (!testIPAddressToUInt32()) {
        return 36;
    }

    if (!testPortConstructor()) {
        return 41;
    }
    if (!testPortEquals()) {
        return 42;
    }
    if (!testPortComparison()) {
        return 43;
    }
    if (!testPortEmpty()) {
        return 44;
    }
    if (!testPortToString()) {
        return 45;
    }
    if (!testPortStream()) {
        return 46;
    }
    if (!testPortOperator()) {
        return 47;
    }
    if (!testPortStatic()) {
        return 48;
    }

    if (!testPortRangeConstructor()) {
        return 51;
    }
    if (!testPortRangeEquals()) {
        return 52;
    }
    if (!testPortRangeComparison()) {
        return 53;
    }
    if (!testPortRangeEmpty()) {
        return 54;
    }
    if (!testPortRangeToString()) {
        return 55;
    }
    if (!testPortRangeStream()) {
        return 56;
    }
    if (!testPortRangeStatic()) {
        return 57;
    }

    if (!testEndpointConstructor()) {
        return 61;
    }
    if (!testEndpointEquals()) {
        return 62;
    }
    if (!testEndpointComparison()) {
        return 63;
    }
    if (!testEndpointEmpty()) {
        return 64;
    }
    if (!testEndpointToString()) {
        return 65;
    }
    if (!testEndpointStream()) {
        return 66;
    }
    if (!testEndpointStatic()) {
        return 67;
    }
    if (!testEndpointIsAnyAddress()) {
        return 68;
    }

    if (!testUrlConstructor()) {
        return 71;
    }
    if (!testUrlEquals()) {
        return 72;
    }
    if (!testUrlComparison()) {
        return 73;
    }
    if (!testUrlEmpty()) {
        return 74;
    }
    if (!testUrlToString()) {
        return 75;
    }
    if (!testUrlStatic()) {
        return 76;
    }
    if (!testUrlParse()) {
        return 77;
    }
    if (!testUrlProperty()) {
        return 78;
    }

    if (!testP2PEndpointConstructor()) {
        return 81;
    }
    if (!testP2PEndpointEquals()) {
        return 82;
    }
    if (!testP2PEndpointComparison()) {
        return 83;
    }
    if (!testP2PEndpointEmpty()) {
        return 84;
    }
    if (!testP2PEndpointToString()) {
        return 85;
    }
    if (!testP2PEndpointStream()) {
        return 86;
    }
    if (!testP2PEndpointOperator()) {
        return 87;
    }
    if (!testP2PEndpointStatic()) {
        return 88;
    }

    return 0;
}

