//
//  ConfigService.cpp
//  common
//
//  Created by baowei on 2020/2/29.
//  Copyright © 2020 com. All rights reserved.
//

#include "configuration/ConfigService.h"
#include "system/ServiceFactory.h"
#include "yml/YmlNode.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Directory.h"
#include "net/Dns.h"
#include "diag/Trace.h"
#include "crypto/RSAProvider.h"
#include "crypto/SmProvider.h"
#include "net/NetInterface.h"
#include "system/Application.h"
#include "system/Environment.h"

using namespace Net;
using namespace Crypto;

namespace Config {
    bool IConfigService::contains(const String &key) const {
        return properties().contains(key);
    }

    bool IConfigService::getProperty(const String &key, String &value) const {
        return properties().at(key, value);
    }

    bool IConfigService::getProperty(const String &key, bool &value) const {
        Boolean v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, uint8_t &value) const {
        Byte v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, char &value) const {
        Char v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, int16_t &value) const {
        Int16 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, uint16_t &value) const {
        UInt16 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, int32_t &value) const {
        Int32 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, uint32_t &value) const {
        UInt32 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, int64_t &value) const {
        Int64 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, uint64_t &value) const {
        UInt64 v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, float &value) const {
        Float v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::getProperty(const String &key, double &value) const {
        Double v;
        if (getProperty(key, v)) {
            value = v;
            return true;
        }
        return false;
    }

    bool IConfigService::setProperty(const String &key, const bool &value) {
        return setProperty(key, Boolean(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const uint8_t &value) {
        return setProperty(key, Byte(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const char &value) {
        return setProperty(key, Char(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const int16_t &value) {
        return setProperty(key, Int16(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const uint16_t &value) {
        return setProperty(key, UInt16(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const int32_t &value) {
        return setProperty(key, Int32(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const uint32_t &value) {
        return setProperty(key, UInt32(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const int64_t &value) {
        return setProperty(key, Int64(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const uint64_t &value) {
        return setProperty(key, UInt64(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const float &value) {
        return setProperty(key, Float(value).toString());
    }

    bool IConfigService::setProperty(const String &key, const double &value) {
        return setProperty(key, Double(value).toString());
    }

    const uint8_t ConfigService::Sm4Key[] = {0x81, 0x50, 0xD4, 0xB6, 0x68, 0xAA, 0xD8, 0xCC, 0x7E, 0x35, 0xFB, 0xA6,
                                             0x5D, 0x4C, 0x83, 0xB0};

    ConfigService::ConfigService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IConfigService>(this);
    }

    ConfigService::~ConfigService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IConfigService>();
    }

    bool ConfigService::initialize() {
        // init system variables, included environment variables.
        initSystemVariables();

        // load application.yml.
        if (!loadFile(this->fileName(), _properties)) {
            Trace::error(String::format("Can not find main yaml file'%s'.", this->fileName().c_str()));
            return false;
        }

        // try to load application_*.yml.
        loadProfile();

        // combine with command arguments.
        Application *app = Application::instance();
        assert(app);
        _properties.addRange(app->arguments());

        // update debug flag.
        bool debug = false;
        if (getProperty("debug", debug) && debug) {
            Trace::enableDebugOutput(debug);
        }

        // update the vars.
        updateVariables();

        // update the passwords.
        updatePasswords();

        // set app culture.
        setCulture();

        // show all properties.
        String show;
        if (getProperty("debug", show) && show == "config.show") {
            StringArray keys;
            _properties.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                const String &key = keys[i];
                String value;
                if (_properties.at(key, value)) {
                    Trace::info(String::format("%s=%s", key.c_str(), value.c_str()));
                }
            }
        }

        return true;
    }

    bool ConfigService::unInitialize() {
        _properties.clear();
        return true;
    }

    bool ConfigService::loadFile(const String &fileName, YmlNode::Properties &properties) const {
        String ymlFileName = Path::getFileName(fileName);
        if (!File::exists(fileName)) {
            Trace::error(String::format("Can not find application configuration file'%s'.", ymlFileName.c_str()));
            return false;
        }

        YmlNode doc;
        if (!YmlNode::parseFile(fileName, doc)) {
            Trace::error(String::format("Failed to load application configuration file'%s'.", ymlFileName.c_str()));
        }
        properties.clear();
        return doc.getProperties(properties);
    }

    void ConfigService::loadProfile() {
        String profile;
        if (getProperty("summer.profiles.active", profile) && !profile.isNullOrEmpty()) {
            String path;
            String basePath;
            getProperty("summer.profiles.path", basePath);
            if (String::equals(basePath, "home", true)) {
                Application *app = Application::instance();
                assert(app);
                path = app->rootPath();
            } else if (Path::isPathRooted(basePath))
                path = basePath;
            else
                path = Path::getDirectoryName(this->fileName());
            String fileName = Path::combine(path, String::format("application-%s.yml", profile.c_str()));
            YmlNode::Properties properties;
            if (loadFile(fileName, properties)) {
                _properties.addRange(properties);
            } else {
                Trace::error(String::format("Can not find profile yaml file'%s'.", fileName.c_str()));
            }
        } else {
            if (!profile.isNullOrEmpty())
                Trace::warn(String::format("Can not find profile yaml file'%s'.",
                                           String::format("application-%s.yml", profile.c_str()).c_str()));
        }
    }

    const YmlNode::Properties &ConfigService::properties() const {
        return _properties;
    }

    const StringMap &ConfigService::systemVariables() const {
        return _systemVariables;
    }

    String ConfigService::fileName() const {
        Application *app = Application::instance();
        assert(app);

        static const char *Application_yml = "application.yml";

        String ymlFileName;
        const String appPath = Directory::getAppPath();
        ymlFileName = Path::combine(appPath, Application_yml);
        if (File::exists(ymlFileName)) {
            return ymlFileName;
        } else {
            ymlFileName = Path::combine(app->rootPath(), Application_yml);
            return ymlFileName;
        }
    }

    bool ConfigService::setProperty(const String &key, const String &value) {
        _properties.add(key, value);
        return true;
    }

    void ConfigService::updateVariables() {
        // search and replace var.
        StringArray keys;
        _properties.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (_properties.at(key, value)) {
                String newValue;
                if (updateVariables(key, value, newValue))
                    setProperty(key, newValue);
            }
        }

        StringArray vars;
        _systemVariables.keys(vars);
        for (size_t i = 0; i < vars.count(); i++) {
            const String &var = vars[i];
            String value;
            if (_systemVariables.at(var, value)) {
                String varKey = String::trim(var, '$', '{', '}');
                if (!contains(varKey)) {
                    setProperty(varKey, value);
                }
            }
        }
    }

    bool ConfigService::updateVariables(const String &key, const String &value, String &newValue) {
        String var, defaultValue;
        if (retrieveVariable(value, var, defaultValue)) {
            // has variables.
            String varValue;
            if (getVariable(var, varValue)) {
                newValue = String::replace(value, var, varValue);
            } else if (!defaultValue.isNullOrEmpty()) {
                newValue = defaultValue;
            }

            updateVariables(key, newValue, newValue);

            return true;
        }
        return false;
    }

    void ConfigService::updatePasswords() {
        // search and replace password.
        StringArray keys;
        _properties.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (_properties.at(key, value)) {
                static const char *ENC = "ENC(";
                ssize_t start = value.find(ENC);
                if (start >= 0)  // has password
                {
                    value = String::replace(value, ENC, String::Empty);
                    if (!value.isNullOrEmpty() && value[value.length() - 1] == ')') {
                        value = value.substr(0, value.length() - 1);
                        setProperty(key, computePlainText(value));
                    }
                }
            }
        }
    }

    String ConfigService::computeCypherText(const String &plainText) {
        Sm4Provider sm4(Sm4Key);
        String sm4CypherText;
        if (sm4.encryptToBase64(plainText, sm4CypherText)) {
            return sm4CypherText;
        }

        // Use Rsa if sm4 failed.
        String rsaCypherText;
        if (RSAProvider::encryptToBase64(plainText, rsaCypherText, RSAProvider::Key256))
            return rsaCypherText;
        return String::Empty;
    }

    String ConfigService::computePlainText(const String &cypherText) {
        Sm4Provider sm4(Sm4Key);
        String sm4PlainText;
        if (sm4.decryptFromBase64(cypherText, sm4PlainText)) {
            return sm4PlainText;
        }

        // Use Rsa if sm4 failed.
        String rsaPlainText;
        if (RSAProvider::decryptFromBase64(cypherText, rsaPlainText, RSAProvider::Key256))
            return rsaPlainText;
        return String::Empty;
    }

    String ConfigService::createEncText(const String &text) {
        return String::format("ENC(%s)", computeCypherText(text).c_str());
    }

    String ConfigService::getFirstIpAddress() {
        StringArray ifaces;
        NetInterface::getInterfaceNames(ifaces);
        for (size_t i = 0; i < ifaces.count(); i++) {
            String iface = ifaces[i];
            if (iface.find("lo") < 0) {
                IPAddress addr;
                if (NetInterface::isInterfaceUp(iface)) {
                    addr = NetInterface::getIpAddress(iface);
                }

                if (!addr.isEmpty()) {
                    return addr.toString();
                }
            }
        }
        return String::Empty;
    }

    void ConfigService::initSystemVariables() {
        Application *app = Application::instance();
        assert(app);
        _systemVariables.add("${summer.application.name}", app->name());
        _systemVariables.add("${summer.cloud.client.ip-address}", getFirstIpAddress());
        _systemVariables.add("${summer.cloud.client.hostname}", Dns::getHostName());

        // add environment variables.
        StringMap variables;
        Environment::getVariables(variables);
        StringArray keys;
        variables.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (variables.at(key, value)) {
                _systemVariables.add(String::format("${%s}", key.c_str()), value);
            }
        }
    }

    void ConfigService::setCulture() {
        Application *app = Application::instance();
        assert(app);

        String culture("zh-CN");    // default is zh-CN
        if (getProperty("summer.application.culture", culture)) {
            app->setCulture(culture);
        }
    }

    bool ConfigService::updateConfigFile(const YmlNode::Properties &properties) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        bool result;
        String profile;
        const String mainFileName = fileName();
        if (cs->getProperty("summer.profiles.active", profile) && !profile.isNullOrEmpty()) {
            // save profile yaml file.
            String path = Path::getDirectoryName(mainFileName);
            String ymlFileName = Path::combine(path, String::format("application-%s.yml", profile.c_str()).c_str());
            result = YmlNode::updateFile(ymlFileName, properties);
        } else {
            // update main yaml file.
            result = YmlNode::updateFile(mainFileName, properties);
        }

        if (result) {
            // update properties in memory.
            StringArray keys;
            properties.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                const String &key = keys[i];
                String value;
                if (properties.at(key, value)) {
                    _properties.add(key, value);
                }
            }

            // update debug flag.
            bool debug = false;
            if (getProperty("debug", debug) && debug) {
                Trace::enableDebugOutput(debug);
            }

            // update the vars.
            updateVariables();

            // update the passwords.
            updatePasswords();
        }
        return result;
    }

    bool ConfigService::retrieveVariable(const String &str, String &variable, String &defaultValue) {
        ssize_t start = str.find("${");
        if (start >= 0)  // has variables
        {
            ssize_t end = str.find("}");
            if (end > start) {
                String text = str.substr(start, end - start + 1);
                ssize_t pos = text.find(':');
                if (pos > 0) {
                    variable = text.substr(0, pos + 1);
                    assert(variable[variable.length() - 1] == ':');
                    variable[variable.length() - 1] = '}';
                    defaultValue = text.substr(pos + 1, text.length() - (pos + 1) - 1);
                } else if (pos == 0) {
                    return false;
                } else {
                    variable = text;
                }
                return true;
            }
        }
        return false;
    }

    // get variable from system variables and property value.
    bool ConfigService::getVariable(const String &name, String &value) {
        if (_systemVariables.at(name, value)) {
            return true;
        } else {
            String varKey = String::trim(name, '$', '{', '}');
            if (getProperty(varKey, value)) {
                return true;
            }
        }
        return false;
    }
}
