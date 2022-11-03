//
//  OpcuaContext.h
//  common
//
//  Created by baowei on 2021/1/9.
//  Copyright © 2021 com. All rights reserved.
//

#ifndef OpcuaContext_h
#define OpcuaContext_h

#include "data/ValueType.h"
#include "data/ByteArray.h"
#include "data/Dictionary.h"
#include "data/TimeSpan.h"
#include "system/Delegate.h"
#include "thread/Thread.h"
#include "data/Variant.h"

namespace Common
{
    enum SecurityMode
    {
        SM_Invalid = 0,
        SM_None = 1,
        SM_Sign = 2,
        SM_SignAndEncrypt = 3
    };
    enum SecurityPolicy
    {
        SP_None,
        SP_Basic256,
        SP_Basic128Rsa15,
        SP_Basic256Sha256
    };
    struct ConnectOptions
    {
    public:
        ConnectOptions();
        ConnectOptions(const ConnectOptions& options);
        
        void operator=(const ConnectOptions& value);
        bool operator==(const ConnectOptions& value) const;
        bool operator!=(const ConnectOptions& value) const;
        
        bool hasUserName() const;
        
        String endpoint;
        String userName;
        String password;
        SecurityMode securityMode;
        SecurityPolicy securityPolicy;
        TimeSpan samplingInterval;
    };

    class OpcuaNodeId
    {
    public:
        enum Types
        {
            NoneType = 0,
            NumericType = 1,
            StringType = 2
        };
        uint16_t nsIndex;
        Types type;
        uint32_t numeric;
        String string;
        
        OpcuaNodeId(Types type = NoneType, uint16_t nsIndex = 0, uint32_t numeric = 0);
        OpcuaNodeId(Types type, uint16_t nsIndex, const String& string);
        OpcuaNodeId(const OpcuaNodeId& nodeId);
        
        void copyFrom(const OpcuaNodeId* nodeId);
        OpcuaNodeId* clone() const;
        
        bool isEmpty() const;
        
        const String toString() const;
        
        bool isNumeric() const;
        bool isString() const;
        
        void operator=(const OpcuaNodeId& value);
        bool operator==(const OpcuaNodeId& value) const;
        bool operator!=(const OpcuaNodeId& value) const;
        
        bool operator>(const OpcuaNodeId& value) const;
        bool operator>=(const OpcuaNodeId& value) const;
        bool operator<(const OpcuaNodeId& value) const;
        bool operator<=(const OpcuaNodeId& value) const;
        
        int compareTo(const OpcuaNodeId& value) const;
        
    public:
        static OpcuaNodeId Empty;
    };
    typedef PList<OpcuaNodeId> NodeIds;

    class TargetName
    {
    public:
        uint16_t nsIndex;
        String name;
        
        TargetName(uint16_t nsIndex = 0, const String& name = String::Empty);
    };

    class TargetNames : public IIndexGetter<TargetName*>
    {
    public:
        TargetNames();
        
        void add(TargetName* name);
        size_t count() const;
        TargetName* at(size_t pos) const override;

    public:
        static bool parse(const String& path, TargetNames& targetNames);
        
    private:
        PList<TargetName> _names;
    };

    class Subscription
    {
    public:
        OpcuaNodeId nodeId;
        uint32_t monitoredItemId;
        
        Subscription(const OpcuaNodeId* nodeId = nullptr);
    };
    typedef PList<Subscription> Subscriptions;

    class SubscriptionId
    {
    public:
        uint32_t subscriptionId;
        Subscriptions subscriptions;
        
        SubscriptionId(uint32_t subscriptionId);
    };
    typedef PList<SubscriptionId> SubscriptionIds;

    class MessageArrivedEventArgs : public EventArgs
    {
    public:
        MessageArrivedEventArgs(const OpcuaNodeId& nodeId, const Variant& value, DateTime sourceTimestamp, uint32_t status);
        ~MessageArrivedEventArgs() override;
        
        OpcuaNodeId nodeId;
        Variant value;
        DateTime sourceTimestamp;
        uint32_t status;
    };

    class OpcuaHelper
    {
    public:
        static SecurityMode parseSecurityMode(const String& value);
        static String convertSecurityModeStr(SecurityMode value);
        static SecurityPolicy parseSecurityPolicy(const String& value);
        static String convertSecurityPolicyStr(SecurityPolicy value);
    };
}

#endif /* OpcuaContext_h */
