//
//  OpcuaClient.cpp
//  common
//
//  Created by baowei on 2019/11/25.
//  Copyright © 2019 com. All rights reserved.
//

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)

#include "net/OpcuaClient.h"
#include "diag/Trace.h"
#include "thread/ThreadPool.h"
#include "thread/Timer.h"

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/securitypolicy_default.h>
#include <open62541/client_subscriptions.h>
#include <open62541/client_highlevel_async.h>

using namespace Diag;

namespace Net {
    OpcuaClient::OpcuaClient() : _client(nullptr), _statusCode(UA_STATUSCODE_GOOD), _subscriptionTimer(nullptr) {
    }

    OpcuaClient::~OpcuaClient() {
        disconnect();
    }

    bool OpcuaClient::connect(const ConnectOptions &options) {
        disconnect();

        UA_Client *client = UA_Client_new();
        UA_ClientConfig *cc = UA_Client_getConfig(client);
        UA_ClientConfig_setDefault(cc);

        switch (options.securityMode) {
            case SM_Invalid:
                cc->securityMode = UA_MESSAGESECURITYMODE_INVALID;
                break;
            case SM_None:
                cc->securityMode = UA_MESSAGESECURITYMODE_NONE;
                break;
            case SM_Sign:
                cc->securityMode = UA_MESSAGESECURITYMODE_SIGN;
                break;
            case SM_SignAndEncrypt:
                cc->securityMode = UA_MESSAGESECURITYMODE_SIGNANDENCRYPT;
                break;
            default:
                break;
        }

//        config->securityPolicyUri = UA_STRING("http://opcfoundation.org/UA/SecurityPolicy#None");

//        cc->securityPolicies = (UA_SecurityPolicy*)UA_malloc(sizeof(UA_SecurityPolicy));
//        if(!cc->securityPolicies)
//            return UA_STATUSCODE_BADOUTOFMEMORY;
//        retval = UA_SecurityPolicy_Basic256(config->securityPolicies, NULL,
//                                                      localPrivateKey, UA_BYTESTRING_NULL, &config->logger);
//        if(retval != UA_STATUSCODE_GOOD) {
//            UA_free(config->securityPolicies);
//            cc->securityPolicies = NULL;
//            return retval;
//        }
//        cc->securityPoliciesSize = 1;

        /* Set stateCallback */
        cc->stateCallback = stateCallback;
        cc->subscriptionInactivityCallback = subscriptionInactivityCallback;

        UA_StatusCode status;
        if (!options.hasUserName())
            status = UA_Client_connect(client, options.endpoint);
        else
            status = UA_Client_connect_username(client, options.endpoint, options.userName, options.password);
        if (status != UA_STATUSCODE_GOOD) {
            UA_Client_delete(client);
            _statusCode = status;
            return false;
        }

        _client = client;
        return true;
    }

    void OpcuaClient::connectAsync(const ConnectOptions &options) {
        _connectOptions = options;
        ThreadPool::startAsync(connectAction, this);
    }

    void OpcuaClient::connectAction(ThreadHolder *holder) {
        OpcuaClient *oc = static_cast<OpcuaClient *>(holder->owner);
        assert(oc);

        oc->connect(oc->_connectOptions);

        delete holder;
    }

    bool OpcuaClient::disconnect(const TimeSpan &timeout) {
//        Debug::writeLine("OpcuaClient::disconnect");

        unsubscribe();

        if (_client != nullptr) {
            UA_Client_disconnect(_client);
            UA_Client_delete(_client); /* Disconnects the client internally */
            _client = nullptr;
        }
        return true;
    }

    bool OpcuaClient::connected() {
        if (_client != nullptr) {
            UA_ClientState state = UA_Client_getState(_client);
//            Debug::writeFormatLine("opcua connected state: %d", state);
            return state != UA_CLIENTSTATE_DISCONNECTED;
        }
        return false;
    }

    uint32_t OpcuaClient::statusCode() const {
        return _statusCode;
    }

    template<class T>
    bool OpcuaClient::read(const OpcuaNodeId *nodeId, T &value, int type) {
        bool result = false;
        if (connected()) {
            /* Read the value attribute of the node. UA_Client_readValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_read. */
            UA_Variant v; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&v);
            UA_StatusCode status = nodeId->isString() ?
                                   UA_Client_readValueAttribute(_client, UA_NODEID_STRING(nodeId->nsIndex,
                                                                                          (char *) nodeId->string.c_str()),
                                                                &v) :
                                   UA_Client_readValueAttribute(_client,
                                                                UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric),
                                                                &v);
            if (status == UA_STATUSCODE_GOOD &&
                UA_Variant_hasScalarType(&v, &UA_TYPES[type])) {
                switch (type) {
                    case UA_TYPES_BOOLEAN:
                        value = *(UA_Boolean *) v.data;
                        break;
                    case UA_TYPES_BYTE:
                        value = *(UA_Byte *) v.data;
                        break;
                    case UA_TYPES_SBYTE:
                        value = *(UA_SByte *) v.data;
                        break;
                    case UA_TYPES_INT16:
                        value = *(UA_Int16 *) v.data;
                        break;
                    case UA_TYPES_UINT16:
                        value = *(UA_UInt16 *) v.data;
                        break;
                    case UA_TYPES_INT32:
                        value = *(UA_Int32 *) v.data;
                        break;
                    case UA_TYPES_UINT32:
                        value = *(UA_UInt32 *) v.data;
                        break;
                    case UA_TYPES_INT64:
                        value = *(UA_Int64 *) v.data;
                        break;
                    case UA_TYPES_UINT64:
                        value = *(UA_UInt64 *) v.data;
                        break;
                    case UA_TYPES_FLOAT:
                        value = *(UA_Float *) v.data;
                        break;
                    case UA_TYPES_DOUBLE:
                        value = *(UA_Double *) v.data;
                        break;
                    default:
                        break;
                }
                result = true;
            }

            /* Clean up */
            UA_Variant_deleteMembers(&v);
        }
        return result;
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, bool &value) {
        return read<bool>(nodeId, value, UA_TYPES_BOOLEAN);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, int8_t &value) {
        return read<int8_t>(nodeId, value, UA_TYPES_BYTE);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, uint8_t &value) {
        return read<uint8_t>(nodeId, value, UA_TYPES_SBYTE);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, int16_t &value) {
        return read<int16_t>(nodeId, value, UA_TYPES_INT16);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, uint16_t &value) {
        return read<uint16_t>(nodeId, value, UA_TYPES_UINT16);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, int32_t &value) {
        return read<int32_t>(nodeId, value, UA_TYPES_INT32);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, uint32_t &value) {
        return read<uint32_t>(nodeId, value, UA_TYPES_UINT32);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, int64_t &value) {
        return read<int64_t>(nodeId, value, UA_TYPES_INT64);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, uint64_t &value) {
        return read<uint64_t>(nodeId, value, UA_TYPES_UINT64);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, float &value) {
        return read<float>(nodeId, value, UA_TYPES_FLOAT);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, double &value) {
        return read<double>(nodeId, value, UA_TYPES_DOUBLE);
    }

    bool OpcuaClient::read(const OpcuaNodeId *nodeId, String &value) {
        bool result = false;
        if (connected()) {
            /* Read the value attribute of the node. UA_Client_readValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_read. */
            UA_Variant v; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&v);
            UA_StatusCode status = nodeId->isString() ?
                                   UA_Client_readValueAttribute(_client, UA_NODEID_STRING(nodeId->nsIndex,
                                                                                          (char *) nodeId->string.c_str()),
                                                                &v) :
                                   UA_Client_readValueAttribute(_client,
                                                                UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric),
                                                                &v);
            if (status == UA_STATUSCODE_GOOD &&
                UA_Variant_hasScalarType(&v, &UA_TYPES[UA_TYPES_STRING])) {
                UA_String *str = (UA_String *) v.data;
                value = String((const char *) str->data, str->length);
                result = true;
            }

            /* Clean up */
            UA_Variant_deleteMembers(&v);
        }
        return result;
    }

    template<class T>
    bool OpcuaClient::write(const OpcuaNodeId *nodeId, T value, int type) {
        bool result = false;
        if (connected()) {
            /* Write the value attribute of the node. UA_Client_writeValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_write. */
            UA_Variant v; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&v);
            UA_Variant_setScalarCopy(&v, &value, &UA_TYPES[type]);
            UA_StatusCode status = UA_STATUSCODE_BADDATATYPEIDUNKNOWN;
            if (nodeId->isString()) {
                status = UA_Client_writeValueAttribute(_client, UA_NODEID_STRING(nodeId->nsIndex,
                                                                                 (char *) nodeId->string.c_str()), &v);
            } else if (nodeId->isNumeric()) {
                status = UA_Client_writeValueAttribute(_client, UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric),
                                                       &v);
            }
            if (status == UA_STATUSCODE_GOOD) {
                result = true;
            }

            /* Clean up */
            UA_Variant_deleteMembers(&v);
        }
        return result;
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, bool value) {
        return write<bool>(nodeId, value, UA_TYPES_BOOLEAN);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, int8_t value) {
        return write<int8_t>(nodeId, value, UA_TYPES_BYTE);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, uint8_t value) {
        return write<uint8_t>(nodeId, value, UA_TYPES_SBYTE);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, int16_t value) {
        return write<int16_t>(nodeId, value, UA_TYPES_INT16);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, uint16_t value) {
        return write<uint16_t>(nodeId, value, UA_TYPES_UINT16);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, int32_t value) {
        return write<int32_t>(nodeId, value, UA_TYPES_INT32);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, uint32_t value) {
        return write<uint32_t>(nodeId, value, UA_TYPES_UINT32);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, int64_t value) {
        return write<int64_t>(nodeId, value, UA_TYPES_INT64);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, uint64_t value) {
        return write<uint64_t>(nodeId, value, UA_TYPES_UINT64);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, float value) {
        return write<float>(nodeId, value, UA_TYPES_FLOAT);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, double value) {
        return write<double>(nodeId, value, UA_TYPES_DOUBLE);
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, const String &value) {
        bool result = false;
        if (connected()) {
            /* Write the value attribute of the node. UA_Client_writeValueAttribute is a
             * wrapper for the raw read service available as UA_Client_Service_write. */
            UA_Variant v; /* Variants can hold scalar values and arrays of any type */
            UA_Variant_init(&v);
            UA_Variant_setScalarCopy(&v, value.c_str(), &UA_TYPES[UA_TYPES_STRING]);
            UA_StatusCode status = UA_STATUSCODE_BADDATATYPEIDUNKNOWN;
            if (nodeId->isString()) {
                status = UA_Client_writeValueAttribute(_client, UA_NODEID_STRING(nodeId->nsIndex,
                                                                                 (char *) nodeId->string.c_str()), &v);
            } else if (nodeId->isNumeric()) {
                status = UA_Client_writeValueAttribute(_client, UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric),
                                                       &v);
            }
            if (status == UA_STATUSCODE_GOOD) {
                result = true;
            }
            if (status == UA_STATUSCODE_GOOD) {
                result = true;
            }

            /* Clean up */
            UA_Variant_deleteMembers(&v);
        }
        return result;
    }

    bool OpcuaClient::write(const OpcuaNodeId *nodeId, const Variant &value) {
        switch (value.type()) {
            case Variant::Null:
                return false;
            case Variant::Digital: {
                bool temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Integer8: {
                int8_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::UInteger8: {
                uint8_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Integer16: {
                int16_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::UInteger16: {
                uint16_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Integer32: {
                int32_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::UInteger32: {
                uint32_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Integer64: {
                int64_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::UInteger64: {
                uint64_t temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Float32: {
                float temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Float64: {
                double temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Text: {
                String temp;
                if (value.getValue(temp))
                    return write(nodeId, temp);
            }
            case Variant::Date:
                break;
            case Variant::Blob:
                break;
            default:
                break;
        }
        return false;
    }

    bool OpcuaClient::subscribe(const OpcuaNodeId *nodeId) {
        if (connected()) {
            Locker locker(&_subscriptionMutex);

            /* Create a subscription */
            UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
            UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(_client, request, this, NULL, NULL);

            UA_UInt32 subId = response.subscriptionId;
            if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
                Debug::writeFormatLine("Create subscription succeeded, subId: %u.", subId);
            } else {
                Debug::writeFormatLine("Failed to create subscription, result: %d.",
                                       response.responseHeader.serviceResult);
                UA_Client_Subscriptions_deleteSingle(_client, subId);
                return false;
            }
            Trace::info(String::format("The subscription id is '%d'.", subId));
            SubscriptionId *id = new SubscriptionId(subId);
            _subscriptionIds.add(id);

            UA_MonitoredItemCreateRequest monRequest;
            if (nodeId->type == OpcuaNodeId::NumericType) {
                monRequest = UA_MonitoredItemCreateRequest_default(UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric));
            } else if (nodeId->type == OpcuaNodeId::StringType) {
                monRequest = UA_MonitoredItemCreateRequest_default(
                        UA_NODEID_STRING(nodeId->nsIndex, (char *) nodeId->string.c_str()));
            } else {
                Debug::writeFormatLine("Can not support the node type'%d'", nodeId->type);
                UA_Client_Subscriptions_deleteSingle(_client, subId);
                return false;
            }

            Subscription *s = new Subscription(nodeId);
            UA_MonitoredItemCreateResult monResponse = UA_Client_MonitoredItems_createDataChange(_client, subId,
                                                                                                 UA_TIMESTAMPSTORETURN_BOTH,
                                                                                                 monRequest, s,
                                                                                                 valueChanged, NULL);
            if (monResponse.statusCode == UA_STATUSCODE_GOOD) {
                Debug::writeFormatLine("Monitoring '%s', id %u", nodeId->toString().c_str(),
                                       monResponse.monitoredItemId);
            } else {
                delete s;
                UA_Client_Subscriptions_deleteSingle(_client, subId);
                UA_Client_MonitoredItems_deleteSingle(_client, subId, monResponse.monitoredItemId);
                return false;
            }
            s->monitoredItemId = monResponse.monitoredItemId;
            id->subscriptions.add(s);

            /* The first publish request should return the initial value of the variable */
            UA_Client_run_iterate(_client, _connectOptions.samplingInterval.totalMilliseconds());
            if (_subscriptionTimer == nullptr) {
                _subscriptionTimer = new Timer("OpcUA.Subscription", subscriptionCallback, this,
                                               _connectOptions.samplingInterval.totalMilliseconds());
                _subscriptionTimer->start();
            }

            return true;
        }
        return false;
    }

    bool OpcuaClient::subscribes(const NodeIds &nodeIds) {
        size_t count = nodeIds.count();
        if (count == 0)
            return false;

        if (connected()) {
            Locker locker(&_subscriptionMutex);

            /* Create a subscription */
            UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
            UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(_client, request, this, NULL, NULL);

            UA_UInt32 subId = response.subscriptionId;
            if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
                Debug::writeFormatLine("Create subscription succeeded, subId: %u.", subId);
            } else {
                Debug::writeFormatLine("Failed to create subscription, result: %d.",
                                       response.responseHeader.serviceResult);
                UA_Client_Subscriptions_deleteSingle(_client, subId);
                return false;
            }
            Trace::info(String::format("The subscription id is '%d'.", subId));
            SubscriptionId *id = new SubscriptionId(subId);
            _subscriptionIds.add(id);

            UA_MonitoredItemCreateRequest *items = new UA_MonitoredItemCreateRequest[count];
            UA_Client_DataChangeNotificationCallback *callbacks = new UA_Client_DataChangeNotificationCallback[count];
            UA_Client_DeleteMonitoredItemCallback *deleteCallbacks = new UA_Client_DeleteMonitoredItemCallback[count];
            Subscription **contexts = new Subscription *[count];
            for (uint32_t i = 0; i < count; i++) {
                const OpcuaNodeId *nodeId = nodeIds[i];
                if (nodeId->type == OpcuaNodeId::NumericType) {
                    items[i] = UA_MonitoredItemCreateRequest_default(
                            UA_NODEID_NUMERIC(nodeId->nsIndex, nodeId->numeric));
                } else if (nodeId->type == OpcuaNodeId::StringType) {
                    items[i] = UA_MonitoredItemCreateRequest_default(
                            UA_NODEID_STRING(nodeId->nsIndex, (char *) nodeId->string.c_str()));
                }
                callbacks[i] = valueChanged;
                deleteCallbacks[i] = nullptr;
                contexts[i] = new Subscription(nodeId);
            }
            UA_CreateMonitoredItemsRequest createRequest;
            UA_CreateMonitoredItemsRequest_init(&createRequest);
            createRequest.subscriptionId = subId;
            createRequest.timestampsToReturn = UA_TIMESTAMPSTORETURN_BOTH;
            createRequest.itemsToCreate = items;
            createRequest.itemsToCreateSize = count;
            UA_CreateMonitoredItemsResponse monResponse = UA_Client_MonitoredItems_createDataChanges(_client,
                                                                                                     createRequest,
                                                                                                     (void **) contexts,
                                                                                                     callbacks,
                                                                                                     deleteCallbacks);
            if (monResponse.responseHeader.serviceResult == UA_STATUSCODE_GOOD &&
                monResponse.resultsSize == count) {
                Trace::info(String::format("Monitoring count'%d'.", monResponse.resultsSize));

                for (uint32_t i = 0; i < monResponse.resultsSize; i++) {
                    const UA_MonitoredItemCreateResult &result = monResponse.results[i];
                    Subscription *s = contexts[i];
                    s->monitoredItemId = result.monitoredItemId;
                    id->subscriptions.add(s);
                }
                delete[] items;
                delete[] callbacks;
                delete[] deleteCallbacks;
                delete[] contexts;
                UA_CreateMonitoredItemsResponse_deleteMembers(&monResponse);
            } else {
                Trace::error(String::format("Monitoring request count'%d', response count'%d'.", count,
                                            monResponse.resultsSize));

                delete[] items;
                delete[] callbacks;
                delete[] deleteCallbacks;
                for (uint32_t i = 0; i < count; i++) {
                    delete contexts[i];
                }
                delete[] contexts;
                UA_CreateMonitoredItemsResponse_deleteMembers(&monResponse);
                return false;
            }

            /* The first publish request should return the initial value of the variable */
            UA_Client_run_iterate(_client, _connectOptions.samplingInterval.totalMilliseconds());
            if (_subscriptionTimer == nullptr) {
                _subscriptionTimer = new Timer("OpcUA.Subscription", subscriptionCallback, this,
                                               _connectOptions.samplingInterval.totalMilliseconds());
                _subscriptionTimer->start();
            }

            return true;
        }
        return false;
    }

    bool OpcuaClient::unsubscribe() {
        if (_subscriptionTimer != nullptr) {
            delete _subscriptionTimer;
            _subscriptionTimer = nullptr;
        }

        if (!connected())
            return false;

        Locker locker(&_subscriptionMutex);

        Trace::info(String::format("unsubscribe, count: %d", _subscriptionIds.count()));
        for (uint32_t i = 0; i < _subscriptionIds.count(); i++) {
            SubscriptionId *id = _subscriptionIds[i];
            uint32_t subscriptionId = id->subscriptionId;
            const Subscriptions &subscriptions = id->subscriptions;

            UA_UInt32 *newMonitoredItemIds = new UA_UInt32[subscriptions.count()];
            UA_DeleteMonitoredItemsRequest deleteRequest;
            UA_DeleteMonitoredItemsRequest_init(&deleteRequest);
            deleteRequest.subscriptionId = subscriptionId;
            deleteRequest.monitoredItemIds = newMonitoredItemIds;
            deleteRequest.monitoredItemIdsSize = subscriptions.count();
            for (uint32_t i = 0; i < subscriptions.count(); i++) {
                newMonitoredItemIds[i] = subscriptions[i]->monitoredItemId;
            }

            UA_DeleteMonitoredItemsResponse deleteResponse = UA_Client_MonitoredItems_delete(_client, deleteRequest);
            UA_DeleteMonitoredItemsResponse_deleteMembers(&deleteResponse);
            delete[] newMonitoredItemIds;

            UA_Client_Subscriptions_deleteSingle(_client, subscriptionId);
        }
        return true;
    }

    Delegates *OpcuaClient::messageArrivedDelegates() {
        return &_messageArrived;
    }

    void
    OpcuaClient::valueChanged(UA_Client *client, UA_UInt32 subId, void *subContext, UA_UInt32 monId, void *monContext,
                              UA_DataValue *value) {
        String result;
        Variant v = toVariant(value);
        if (!v.isNullType()) {
            OpcuaClient *oc = static_cast<OpcuaClient *>(subContext);
            assert(oc);
            Subscription *s = static_cast<Subscription *>(monContext);
            assert(s);
            if (s->monitoredItemId == monId) {
//                January 1, 1601
                static DateTime start(1601, 1, 1, 0, 0, 0, 0, DateTime::Kind::Utc);
                DateTime sourceTimestamp = start + TimeSpan(value->sourceTimestamp);
//                UA_DateTimeStruct temp = UA_DateTime_toStruct(value->sourceTimestamp);
//#ifdef DEBUG
//                if(Variant::isIntegerValue(v.type()))
//                {
//                    Debug::writeFormatLine("The Answer has changed! %s, value type: %s, value: '%s', sourceTimestamp: '%s', status: '0x%x'",
//                                           s->nodeId.toString().c_str(), v.typeStr().c_str(), v.toString().c_str(), sourceTimestamp.toString().c_str(), value->status);
//                }
//#endif
                MessageArrivedEventArgs e(s->nodeId, v, sourceTimestamp, value->status);
                oc->_messageArrived.invoke(oc, &e);
            }
        }
    }

    void OpcuaClient::stateCallback(UA_Client *client, UA_ClientState clientState) {
        switch (clientState) {
            case UA_CLIENTSTATE_DISCONNECTED:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "The client is disconnected");
                break;
            case UA_CLIENTSTATE_WAITING_FOR_ACK:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Waiting for ack");
                break;
            case UA_CLIENTSTATE_CONNECTED:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A TCP connection to the server is open");
                break;
            case UA_CLIENTSTATE_SECURECHANNEL:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A SecureChannel to the server is open");
                break;
            case UA_CLIENTSTATE_SESSION: {
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A session with the server is open");
            }
                break;
            case UA_CLIENTSTATE_SESSION_RENEWED:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A session with the server is open (renewed)");
                /* The session was renewed. We don't need to recreate the subscription. */
                break;
            case UA_CLIENTSTATE_SESSION_DISCONNECTED:
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Session disconnected");
                break;
        }
        return;
    }

    void OpcuaClient::subscriptionInactivityCallback(UA_Client *client, UA_UInt32 subscriptionId, void *subContext) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Subscription Id %u was deleted", subscriptionId);
    }

    void OpcuaClient::subscriptionCallback(void *state) {
        OpcuaClient *oc = (OpcuaClient *) state;
        if (oc->connected()) {
            Locker locker(&oc->_subscriptionMutex);

            UA_Client_run_iterate(oc->_client, 100);
        }
    }

    Variant OpcuaClient::toVariant(const UA_DataValue *from) {
        if (from != nullptr && from->hasValue) {
            if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_STRING])) {
                UA_String *v = (UA_String *) from->value.data;
                return Variant(String((const char *) v->data, v->length));
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
                UA_Boolean v = *(UA_Boolean *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_BYTE])) {
                UA_Byte v = *(UA_Byte *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_SBYTE])) {
                UA_SByte v = *(UA_SByte *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_INT16])) {
                UA_Int16 v = *(UA_Int16 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_UINT16])) {
                UA_UInt16 v = *(UA_UInt16 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_INT32])) {
                UA_Int32 v = *(UA_Int32 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_UINT32])) {
                UA_UInt32 v = *(UA_UInt32 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_INT64])) {
                UA_Int64 v = *(UA_Int64 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_UINT64])) {
                UA_UInt64 v = *(UA_UInt64 *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_FLOAT])) {
                UA_Float v = *(UA_Float *) from->value.data;
                return Variant(v);
            } else if (UA_Variant_hasScalarType(&from->value, &UA_TYPES[UA_TYPES_DOUBLE])) {
                UA_Double v = *(UA_Double *) from->value.data;
                return Variant(v);
            }
        }
        return Variant();
    }

    bool OpcuaClient::translateBrowsePathsToNodeIds(const StringArray &browsePaths, NodeIds &nodeIds) {
        if (!connected())
            return false;

        if (browsePaths.count() == 0)
            return false;

        UA_TranslateBrowsePathsToNodeIdsRequest request;
        UA_TranslateBrowsePathsToNodeIdsRequest_init(&request);

        size_t count = browsePaths.count();
        UA_BrowsePath *bps = new UA_BrowsePath[count];
        for (uint32_t i = 0; i < count; i++) {
            const String &path = browsePaths[i];
            Debug::writeFormatLine("opcua.translate, path: %s", path.c_str());

            TargetNames names;
            TargetNames::parse(path, names);

            size_t pathSize = names.count();
            UA_BrowsePath &browsePath = bps[i];
            UA_BrowsePath_init(&browsePath);
            browsePath.startingNode = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
            browsePath.relativePath.elements = (UA_RelativePathElement *) UA_Array_new(pathSize,
                                                                                       &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]);
            browsePath.relativePath.elementsSize = pathSize;

            for (uint32_t j = 0; j < pathSize; j++) {
                const TargetName *name = names[j];
                Debug::writeFormatLine("opcua.translate, ns: %d, name: %s", name->nsIndex, name->name.c_str());
                UA_RelativePathElement *elem = &browsePath.relativePath.elements[j];
                UA_RelativePathElement_init(elem);
                elem->targetName = UA_QUALIFIEDNAME_ALLOC(name->nsIndex, name->name);
                //            elem->includeSubtypes = true;
                //            elem->isInverse = false;
            }
        }
        request.browsePaths = bps;
        request.browsePathsSize = count;

        bool result = false;
        UA_TranslateBrowsePathsToNodeIdsResponse response = UA_Client_Service_translateBrowsePathsToNodeIds(_client,
                                                                                                            request);
        Debug::writeFormatLine("opcua.translate, result: %d, result size: %d", response.responseHeader.serviceResult,
                               response.resultsSize);
        if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD && response.resultsSize == count) {
            for (uint32_t i = 0; i < response.resultsSize; i++) {
                OpcuaNodeId *nodeId = new OpcuaNodeId();
                const UA_BrowsePathResult *bresult = response.results;
                if (bresult[i].statusCode == UA_STATUSCODE_GOOD && bresult[i].targetsSize == 1) {
                    const UA_BrowsePathTarget *targets = bresult[i].targets;
                    UA_NodeIdType identifierType = targets[0].targetId.nodeId.identifierType;
                    if (identifierType == UA_NODEIDTYPE_NUMERIC) {
                        nodeId->type = OpcuaNodeId::NumericType;
                        nodeId->nsIndex = targets[0].targetId.nodeId.namespaceIndex;
                        nodeId->numeric = targets[0].targetId.nodeId.identifier.numeric;
                        Debug::writeFormatLine("opcua.translate, %d, identifierType: %d, numeric: %d",
                                               i, targets[0].targetId.nodeId.identifierType, nodeId->numeric);
                    } else if (identifierType == UA_NODEIDTYPE_STRING) {
                        nodeId->type = OpcuaNodeId::StringType;
                        nodeId->nsIndex = targets[0].targetId.nodeId.namespaceIndex;
                        const UA_String &str = targets[0].targetId.nodeId.identifier.string;
                        Debug::writeFormatLine("opcua.translate, str info: %d.", str.length);
                        nodeId->string = String((const char *) str.data, str.length);
                        Debug::writeFormatLine("opcua.translate, %d, identifierType: %d, string: %s",
                                               i, targets[0].targetId.nodeId.identifierType, str.data);
                    } else {
                        Debug::writeFormatLine("opcua.translate, index: %d, unknown identifierType: %d",
                                               i, targets[0].targetId.nodeId.identifierType);
                    }
                } else {
                    Debug::writeFormatLine(
                            "opcua.translate, target error, index: %d, status code: 0x%x, target size: %d",
                            i, bresult[i].statusCode, bresult[i].targetsSize);
                }

                if (nodeId != nullptr) {
                    nodeIds.add(nodeId);
                }
            }
            result = true;
        }

        for (uint32_t i = 0; i < count; i++) {
            UA_BrowsePath &browsePath = bps[i];
            UA_BrowsePath_deleteMembers(&browsePath);
        }
        delete[] bps;

        UA_TranslateBrowsePathsToNodeIdsResponse_deleteMembers(&response);

        return result;
    }

    bool OpcuaClient::translateBrowsePathToNodeId(const String &browsePath, OpcuaNodeId &nodeId) {
        NodeIds nodeIds;
        bool result = translateBrowsePathsToNodeIds(StringArray(browsePath, nullptr), nodeIds);
        if (result && nodeIds.count() == 1)
            nodeId.copyFrom(nodeIds[0]);
        return result;
    }
}
#endif
