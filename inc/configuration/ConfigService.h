//
//  ConfigService.h
//  common
//
//  Created by baowei on 2020/2/29.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef ConfigService_h
#define ConfigService_h

#include "data/String.h"
#include "yml/YmlNode.h"
#include "system/ServiceFactory.h"

using namespace Yml;
using namespace System;

namespace Config {
    class IConfigService : public IService {
    public:
        virtual const YmlNode::Properties &properties() const = 0;

        virtual bool updateConfigFile(const YmlNode::Properties &properties) = 0;

        virtual bool setProperty(const String &key, const String &value) = 0;

        bool contains(const String &key) const;

        void printProperties() const;

        String getProperty(const String &key) const;

        bool getProperty(const String &key, String &value) const;

        bool getProperty(const String &key, bool &value) const;

        bool getProperty(const String &key, uint8_t &value) const;

        bool getProperty(const String &key, char &value) const;

        bool getProperty(const String &key, int16_t &value) const;

        bool getProperty(const String &key, uint16_t &value) const;

        bool getProperty(const String &key, int32_t &value) const;

        bool getProperty(const String &key, uint32_t &value) const;

        bool getProperty(const String &key, int64_t &value) const;

        bool getProperty(const String &key, uint64_t &value) const;

        bool getProperty(const String &key, float &value) const;

        bool getProperty(const String &key, double &value) const;

        template<class T>
        bool getProperty(const String &key, T &value) const {
            String str;
            if (getProperty(key, str)) {
                return T::parse(str, value);
            }
            return false;
        }

        bool setProperty(const String &key, const bool &value);

        bool setProperty(const String &key, const uint8_t &value);

        bool setProperty(const String &key, const char &value);

        bool setProperty(const String &key, const int16_t &value);

        bool setProperty(const String &key, const uint16_t &value);

        bool setProperty(const String &key, const int32_t &value);

        bool setProperty(const String &key, const uint32_t &value);

        bool setProperty(const String &key, const int64_t &value);

        bool setProperty(const String &key, const uint64_t &value);

        bool setProperty(const String &key, const float &value);

        bool setProperty(const String &key, const double &value);

        template<class T>
        bool setProperty(const String &key, const T &value) {
            return setProperty(key, value.toString());
        }
    };

    class ConfigService : public IConfigService {
    public:
        ConfigService();

        ~ConfigService() override;

        bool initialize();

        bool unInitialize();

        const YmlNode::Properties &properties() const override;

        const StringMap &systemVariables() const;

        bool setProperty(const String &key, const String &value) override;

    private:
        bool updateConfigFile(const YmlNode::Properties &properties) final;

        String fileName() const;

        void updateVariables();

        bool updateVariables(const String &key, const String &value, String &newValue);

        void updatePasswords();

        bool loadFile(const String &fileName, YmlNode::Properties &properties) const;

        void loadProfile();

        void initSystemVariables();

        void setCulture();

        bool getVariable(const String &name, String &value);

    public:
        static String computeCypherText(const String &plainText);

        static String computePlainText(const String &cypherText);

        static String createEncText(const String &text);

        static bool retrieveVariable(const String &str, String &variable, String &defaultValue);

    private:
        static String getFirstIpAddress();

    private:
        YmlNode::Properties _properties;

        StringMap _systemVariables;

    private:
        static const uint8_t Sm4Key[];
    };
}

#endif /* ConfigService_h */
