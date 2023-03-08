//
//  OpcuaClient.h
//  common
//
//  Created by baowei on 2019/11/25.
//  Copyright (c) 2019 com. All rights reserved.
//

#ifndef Openua_client_h
#define Openua_client_h

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)
#include "net/OpcuaContext.h"
#include "thread/Timer.h"

#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/securitypolicy_default.h>

namespace Net
{
    class OpcuaClient
    {
    public:
        OpcuaClient();
        ~OpcuaClient();
        
        bool connect(const ConnectOptions& options);
        void connectAsync(const ConnectOptions& options);
        bool disconnect(const TimeSpan& timeout = TimeSpan::fromSeconds(10));
        
        bool connected();
        
        uint32_t statusCode() const;
        
        bool read(const OpcuaNodeId* nodeId, bool& value);
        bool read(const OpcuaNodeId* nodeId, int8_t& value);
        bool read(const OpcuaNodeId* nodeId, uint8_t& value);
        bool read(const OpcuaNodeId* nodeId, int16_t& value);
        bool read(const OpcuaNodeId* nodeId, uint16_t& value);
        bool read(const OpcuaNodeId* nodeId, int32_t& value);
        bool read(const OpcuaNodeId* nodeId, uint32_t& value);
        bool read(const OpcuaNodeId* nodeId, int64_t& value);
        bool read(const OpcuaNodeId* nodeId, uint64_t& value);
        bool read(const OpcuaNodeId* nodeId, float& value);
        bool read(const OpcuaNodeId* nodeId, double& value);
        bool read(const OpcuaNodeId* nodeId, String& value);
        
        bool write(const OpcuaNodeId* nodeId, bool value);
        bool write(const OpcuaNodeId* nodeId, int8_t value);
        bool write(const OpcuaNodeId* nodeId, uint8_t value);
        bool write(const OpcuaNodeId* nodeId, int16_t value);
        bool write(const OpcuaNodeId* nodeId, uint16_t value);
        bool write(const OpcuaNodeId* nodeId, int32_t value);
        bool write(const OpcuaNodeId* nodeId, uint32_t value);
        bool write(const OpcuaNodeId* nodeId, int64_t value);
        bool write(const OpcuaNodeId* nodeId, uint64_t value);
        bool write(const OpcuaNodeId* nodeId, float value);
        bool write(const OpcuaNodeId* nodeId, double value);
        bool write(const OpcuaNodeId* nodeId, const String& value);
        bool write(const OpcuaNodeId* nodeId, const Variant& value);
        
        bool subscribe(const OpcuaNodeId* nodeId);
        bool subscribes(const NodeIds& nodeIds);
        bool unsubscribe();
        
        Delegates* messageArrivedDelegates();
        
        bool translateBrowsePathToNodeId(const String& browsePath, OpcuaNodeId& nodeId);
        bool translateBrowsePathsToNodeIds(const StringArray& browsePaths, NodeIds& nodeIds);
        
    private:
        template<class T>
        bool read(const OpcuaNodeId* nodeId, T& value, int type);
        template<class T>
        bool write(const OpcuaNodeId* nodeId, T value, int type);
        
    private:
//        static void connectAction(ThreadHolder* holder);

        static void valueChanged(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId, void *monContext, UA_DataValue *value);
        static void stateCallback(UA_Client *client, UA_ClientState clientState);
        static void subscriptionInactivityCallback(UA_Client *client, UA_UInt32 subscriptionId, void *subContext);

        static Variant toVariant(const UA_DataValue* value);
//        static bool toUADataValue(const Variant& from, UA_DataValue* to);
        
    private:
        UA_Client* _client;
        uint32_t _statusCode;
        Mutex _subscriptionMutex;
        
        ConnectOptions _connectOptions;
        
        Timer* _subscriptionTimer;
        Delegates _messageArrived;
        
        SubscriptionIds _subscriptionIds;
    };
}
#endif

#endif /* OpcuaClient_h */
