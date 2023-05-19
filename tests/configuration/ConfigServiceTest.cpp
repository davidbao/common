//
//  ConfigServiceTest.cpp
//  common
//
//  Created by baowei on 2022/10/19.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "configuration/ConfigService.h"
#include "net/Dns.h"
#include "net/NetInterface.h"
#include "system/Application.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"

using namespace Data;
using namespace Net;
using namespace Config;

static String _rootPath = Path::combine(Path::getTempPath(), "common.ConfigServiceTest");

void cleanUp() {
    if(Directory::exists(_rootPath)) {
        Directory::deleteDirectory(_rootPath);
    }
}

bool testConstructor() {
    ConfigService cs;

    ServiceFactory *factory = ServiceFactory::instance();
    assert(factory);
    IConfigService *service = factory->getService<IConfigService>();
    if (service == nullptr) {
        return false;
    }
    return true;
}

bool testDestructor() {
    ConfigService *cs = new ConfigService;

    ServiceFactory *factory = ServiceFactory::instance();
    assert(factory);
    IConfigService *service = factory->getService<IConfigService>();
    if (service == nullptr) {
        return false;
    }
    delete cs;

    IConfigService *service2 = factory->getService<IConfigService>();
    if (service2 != nullptr) {
        return false;
    }
    return true;
}

bool testInitialize() {
    static int argc = 3;
    static const char *argv[] = {"ConfigServiceTest", "-q", "--debug=true"};
    Application app(argc, argv, _rootPath);
//    printf("_rootPath: %s\n", _rootPath.c_str());
    // create an application.yml & application_dev.yml.
    String ymlFileName = Path::combine(_rootPath, "application.yml");
    YmlNode::Properties properties;
    properties.add("debug", "config.show");
    properties.add("summer.application.name", "${summer.application.name}");
    properties.add("summer.application.description", "${summer.application.name}");
    properties.add("summer.application.version", Version(3, 1, 2));
    properties.add("summer.application.content", "${summer.application.name}-${summer.application.description}-${summer.application.version}");
    properties.add("test.config.defaultName", "${test.config.name:abc}");
    if(!YmlNode::updateFile(ymlFileName, properties)) {
        Trace::error("Can not create an application.yml.");
        return false;
    }

    ConfigService cs;
    if(!cs.initialize()) {
        return false;
    }
    String value;
    if(!(cs.getProperty("debug", value) && value == "true")) {
        return false;
    }
#ifdef PC_OS
    if(!(cs.getProperty("summer.application.name", value) && value == "ConfigServiceTest")) {
        Debug::writeLine("3");
        return false;
    }
    if(!(cs.getProperty("summer.application.description", value) && value == "ConfigServiceTest")) {
        Debug::writeLine("4");
        return false;
    }
    if(!(cs.getProperty("summer.application.content", value) && value == "ConfigServiceTest-ConfigServiceTest-3.1.2")) {
        Debug::writeLine("6");
        return false;
    }
#endif
    if(!(cs.getProperty("summer.application.version", value) && Version(value) == Version("3.1.2"))) {
        return false;
    }
    if(!(cs.getProperty("test.config.defaultName", value) && value == "abc")) {
        return false;
    }

    if(!cs.setProperty("test.config.setProperty", value)) {
        return false;
    }
    if(!(cs.getProperty("test.config.setProperty", value) && value == "abc")) {
        return false;
    }

    cs.unInitialize();

    return true;
}

bool testUninitialize() {
    ConfigService cs;
    if(!cs.unInitialize()) {
        return false;
    }

    return true;
}

bool testSystemVariables() {
    ConfigService cs;
    const StringMap &vars = cs.systemVariables();
    StringArray keys;
    vars.keys(keys);
    for (size_t i = 0; i < keys.count(); i++) {
        const String &key = keys[i];
        const String &value = vars[key];
        if (key == "${summer.application.name}") {
#ifdef PC_OS
            if(value != "ConfigServiceTest") {
                return false;
            }
#endif
        } else if (value == "${summer.cloud.client.ip-address}") {
            StringArray ifaces;
            NetInterface::getInterfaceNames(ifaces);
            for (size_t j = 0; j < ifaces.count(); j++) {
                String iface = ifaces[j];
                if (iface.find("lo") < 0) {
                    IPAddress addr;
                    if (NetInterface::isInterfaceUp(iface)) {
                        addr = NetInterface::getIpAddress(iface);
                    }

                    if (value != addr.toString()) {
                        return false;
                    }
                }
            }
        } else if (value == "${summer.cloud.client.hostname}") {
            if (value != Dns::getHostName()) {
                return false;
            }
        }
    }
    return true;
}

bool testCypher() {
    {
        String plainText = "abc123,)_";
        String cypherText = ConfigService::computeCypherText(plainText);
        if (cypherText.isNullOrEmpty()) {
            return false;
        }
        String plainText2 = ConfigService::computePlainText(cypherText);
        if (plainText2.isNullOrEmpty()) {
            return false;
        }
        if (plainText2 != plainText) {
            return false;
        }
    }

    {
        String cypherText3 = "UiBeBtjan2CUXufGSzSlUg==";    // sm4 cypher.
        String plainText4 = ConfigService::computePlainText(cypherText3);
        if (plainText4.isNullOrEmpty()) {
            return false;
        }
        if (plainText4 != "123.com") {
            return false;
        }
    }

    {
        String plainText, cypherText;
        plainText = "user";
        cypherText = ConfigService::computeCypherText(plainText);
        Trace::verb(String::format("plain: '%s', cypher: '%s'", plainText.c_str(), cypherText.c_str()));
        plainText = "admin";
        cypherText = ConfigService::computeCypherText(plainText);
        Trace::verb(String::format("plain: '%s', cypher: '%s'", plainText.c_str(), cypherText.c_str()));
        plainText = "123.com";
        cypherText = ConfigService::computeCypherText(plainText);
        Trace::verb(String::format("plain: '%s', cypher: '%s'", plainText.c_str(), cypherText.c_str()));
        plainText = "admin.com";
        cypherText = ConfigService::computeCypherText(plainText);
        Trace::verb(String::format("plain: '%s', cypher: '%s'", plainText.c_str(), cypherText.c_str()));
    }

    return true;
}

bool testRetrieveVariable() {
    String text = "${abc}";
    String var, value;
    if(!ConfigService::retrieveVariable(text, var, value)) {
        return false;
    }
    if(!(var == "${abc}" && value.isNullOrEmpty())) {
        return false;
    }

    String text2 = "${abc:123}";
    String var2, value2;
    if(!ConfigService::retrieveVariable(text2, var2, value2)) {
        return false;
    }
    if(!(var2 == "${abc}" && value2 == "123")) {
        return false;
    }

    return true;
}

int main() {
    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testDestructor()) {
        result = 2;
    }
    if (!testInitialize()) {
        result = 3;
    }
    if (!testUninitialize()) {
        result = 4;
    }
    if (!testSystemVariables()) {
        result = 5;
    }
    if (!testCypher()) {
        result = 6;
    }
    if (!testRetrieveVariable()) {
        result = 7;
    }

    cleanUp();

    if(result) {
        Debug::writeFormatLine("result: %d", result);
    }

    return result;
}