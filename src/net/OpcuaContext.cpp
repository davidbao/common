//
//  OpcuaContext.cpp
//  common
//
//  Created by baowei on 2021/1/9.
//  Copyright © 2021 com. All rights reserved.
//

#include "net/OpcuaContext.h"
#include "system/Regex.h"

namespace Net
{
    ConnectOptions::ConnectOptions()
    {
        this->securityMode = SM_Invalid;
        this->samplingInterval = TimeSpan::fromMilliseconds(250);
    }
    ConnectOptions::ConnectOptions(const ConnectOptions& options)
    {
        this->operator=(options);
    }
    void ConnectOptions::operator=(const ConnectOptions& value)
    {
        this->endpoint = value.endpoint;
        this->userName = value.userName;
        this->password = value.password;
        this->securityMode = value.securityMode;
        this->securityPolicy = value.securityPolicy;
        this->samplingInterval = value.samplingInterval;
    }
    bool ConnectOptions::operator==(const ConnectOptions& value) const
    {
        return this->endpoint == value.endpoint &&
            this->userName == value.userName &&
            this->password == value.password &&
            this->securityMode == value.securityMode &&
            this->securityPolicy == value.securityPolicy &&
            this->samplingInterval == value.samplingInterval;
    }
    bool ConnectOptions::operator!=(const ConnectOptions& value) const
    {
        return !operator==(value);
    }
    bool ConnectOptions::hasUserName() const
    {
        return !this->userName.isNullOrEmpty();
    }

    OpcuaNodeId OpcuaNodeId::Empty;
    OpcuaNodeId::OpcuaNodeId(Types type, uint16_t nsIndex, uint32_t numeric) : type(type), nsIndex(nsIndex), numeric(numeric)
    {
    }
    OpcuaNodeId::OpcuaNodeId(Types type, uint16_t nsIndex, const String& string) : type(type), nsIndex(nsIndex), string(string)
    {
    }
    OpcuaNodeId::OpcuaNodeId(const OpcuaNodeId& nodeId)
    {
        copyFrom(&nodeId);
    }
    void OpcuaNodeId::copyFrom(const OpcuaNodeId* nodeId)
    {
        this->nsIndex = nodeId->nsIndex;
        this->type = nodeId->type;
        this->numeric = nodeId->numeric;
        this->string = nodeId->string;
    }
    OpcuaNodeId* OpcuaNodeId::clone() const
    {
        OpcuaNodeId* nodeId = new OpcuaNodeId();
        nodeId->copyFrom(this);
        return nodeId;
    }
    bool OpcuaNodeId::isEmpty() const
    {
        if(type == Types::NumericType)
            return nsIndex == 0 && numeric == 0;
        else if(type == Types::StringType)
            return nsIndex == 0 && string.isNullOrEmpty();
        else
            return true;
    }
    const String OpcuaNodeId::toString() const
    {
        if(type == NumericType) {
            return String::format("node type: numeric, nsIndex: %d, identifier: %d", nsIndex, numeric);
        } else if(type == StringType) {
            return String::format("node type: string, nsIndex: %d, identifier: %s", nsIndex, string.c_str());
        } else {
            return String::Empty;
        }
    }
    bool OpcuaNodeId::isNumeric() const
    {
        return type == NumericType;
    }
    bool OpcuaNodeId::isString() const
    {
        return type == StringType;
    }
    void OpcuaNodeId::operator=(const OpcuaNodeId& value)
    {
        copyFrom(&value);
    }
    bool OpcuaNodeId::operator==(const OpcuaNodeId& value) const
    {
        return this->nsIndex == value.nsIndex && this->type == value.type &&
                this->numeric == value.numeric && this->string == value.string;
    }
    bool OpcuaNodeId::operator!=(const OpcuaNodeId& value) const
    {
        return !operator==(value);
    }
    bool OpcuaNodeId::operator>(const OpcuaNodeId& value) const
    {
        return compareTo(value) > 0;
    }
    bool OpcuaNodeId::operator>=(const OpcuaNodeId& value) const
    {
        return compareTo(value) >= 0;
    }
    bool OpcuaNodeId::operator<(const OpcuaNodeId& value) const
    {
        return compareTo(value) < 0;
    }
    bool OpcuaNodeId::operator<=(const OpcuaNodeId& value) const
    {
        return compareTo(value) <= 0;
    }
    int OpcuaNodeId::compareTo(const OpcuaNodeId& value) const
    {
        if(this->type > value.type)
            return 1;
        else if(this->type < value.type)
            return -1;
        
        if(this->nsIndex > value.nsIndex)
            return 1;
        else if(this->nsIndex < value.nsIndex)
            return -1;
        
        if(isNumeric())
        {
            if(this->numeric > value.numeric)
                return 1;
            else if(this->numeric < value.numeric)
                return -1;
        }
        else if(isString())
        {
            if(this->string > value.string)
                return 1;
            else if(this->string < value.string)
                return -1;
        }
        return 0;
    }

    TargetName::TargetName(uint16_t nsIndex, const String& name) : nsIndex(nsIndex), name(name)
    {
    }

    TargetNames::TargetNames()
    {
    }
    void TargetNames::add(TargetName* name)
    {
        _names.add(name);
    }
    size_t TargetNames::count() const
    {
        return _names.count();
    }
    TargetName* TargetNames::at(size_t pos) const
    {
        return _names.at(pos);
    }
    bool TargetNames::parse(const String& browsePath, TargetNames& targetNames)
    {
        static const char* ObjectsStr = "/Objects/";
        int index = browsePath.find(ObjectsStr);
        if(index != 0)
            return false;
        
        String path = String::replace(browsePath, ObjectsStr, String::Empty);
        StringArray texts;
        StringArray::parse(path, texts, '/');
        for(uint i=0;i<texts.count();i++)
        {
            TargetName* tn = nullptr;
            const String& text = texts[i];
            if(text.find(':') > 0)
            {
                static Regex regex("(\\d+|\\s):(.+)");
                
                StringArray names;
                regex.match(text, names);
                if(names.count() == 1 && !names[0].isNullOrEmpty())
                    tn = new TargetName(0, names[0]);
                else if(names.count() == 2)
                {
                    uint16_t nsIndex;
                    if(UInt16::parse(names[0], nsIndex) && !names[1].isNullOrEmpty())
                        tn = new TargetName(nsIndex, names[1]);
                }
            }
            else
            {
                tn = new TargetName(0, text);
            }
            
            if(tn != nullptr)
                targetNames.add(tn);
        }
        
        return targetNames.count() > 0;
    }

    Subscription::Subscription(const OpcuaNodeId* nodeId) : monitoredItemId(0)
    {
        if(nodeId != nullptr)
            this->nodeId.copyFrom(nodeId);
    }

    SubscriptionId::SubscriptionId(uint32_t subscriptionId) : subscriptionId(subscriptionId)
    {        
    }

    MessageArrivedEventArgs::MessageArrivedEventArgs(const OpcuaNodeId& nodeId, const Variant& value,
                                                                  DateTime sourceTimestamp, uint32_t status) :
                                                                    nodeId(nodeId), value(value), sourceTimestamp(sourceTimestamp), status(status)
    {
    }
    MessageArrivedEventArgs::~MessageArrivedEventArgs()
    {
    }

    SecurityMode OpcuaHelper::parseSecurityMode(const String& value)
    {
        if(String::equals(value, "Invalid", true))
        {
            return SecurityMode::SM_Invalid;
        }
        else if(String::equals(value, "None", true))
        {
            return SecurityMode::SM_None;
        }
        else if(String::equals(value, "Sign", true))
        {
            return SecurityMode::SM_Sign;
        }
        else if(String::equals(value, "SignAndEncrypt", true))
        {
            return SecurityMode::SM_SignAndEncrypt;
        }
        else
        {
            return SecurityMode::SM_None;
        }
    }
    String OpcuaHelper::convertSecurityModeStr(SecurityMode value)
    {
        switch (value)
        {
            case SecurityMode::SM_Invalid:
                return "Invalid";
            case SecurityMode::SM_None:
                return "None";
            case SecurityMode::SM_Sign:
                return "Sign";
            case SecurityMode::SM_SignAndEncrypt:
                return "SignAndEncrypt";
            default:
                return "Invalid";
        }
    }

    SecurityPolicy OpcuaHelper::parseSecurityPolicy(const String& value)
    {
        if(String::equals(value, "None", true))
        {
            return SecurityPolicy::SP_None;
        }
        else if(String::equals(value, "Basic256", true))
        {
            return SecurityPolicy::SP_Basic256;
        }
        else if(String::equals(value, "Basic128Rsa15", true))
        {
            return SecurityPolicy::SP_Basic128Rsa15;
        }
        else if(String::equals(value, "Basic256Sha256", true))
        {
            return SecurityPolicy::SP_Basic256Sha256;
        }
        else
        {
            return SecurityPolicy::SP_None;
        }
    }
    String OpcuaHelper::convertSecurityPolicyStr(SecurityPolicy value)
    {
        switch (value)
        {
            case SecurityPolicy::SP_None:
                return "None";
            case SecurityPolicy::SP_Basic256:
                return "Basic256";
            case SecurityPolicy::SP_Basic128Rsa15:
                return "Basic128Rsa15";
            case SecurityPolicy::SP_Basic256Sha256:
                return "Basic256Sha256";
            default:
                return "None";
        }
    }
}
