//
//  MqttClient.cpp
//  common
//
//  Created by baowei on 2018/9/21.
//  Copyright (c) 2018 com. All rights reserved.
//

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)

#include "net/MqttClient.hpp"
#include "diag/Trace.h"
#include "thread/ThreadPool.h"

using namespace Diag;

namespace Net {
    MqttClient::ConnectOptions::ConnectOptions() {
        keepAliveInterval = TimeSpan::fromSeconds(10);
        connectTimeout = TimeSpan::fromSeconds(10);
        cleanSession = true;
    }

    MqttClient::ConnectOptions::ConnectOptions(const ConnectOptions &options) {
        this->operator=(options);
    }

    MqttClient::ConnectOptions::~ConnectOptions() {
    }

    void MqttClient::ConnectOptions::operator=(const ConnectOptions &value) {
        this->address = value.address;
        this->userName = value.userName;
        this->password = value.password;
        this->keepAliveInterval = value.keepAliveInterval;
        this->connectTimeout = value.connectTimeout;
        this->cleanSession = value.cleanSession;
    }

    bool MqttClient::ConnectOptions::operator==(const ConnectOptions &value) const {
        return this->address == value.address &&
               this->userName == value.userName &&
               this->password == value.password &&
               this->keepAliveInterval == value.keepAliveInterval &&
               this->connectTimeout == value.connectTimeout &&
               this->cleanSession == value.cleanSession;
    }

    bool MqttClient::ConnectOptions::operator!=(const ConnectOptions &value) const {
        return !operator==(value);
    }

    bool MqttClient::ConnectOptions::isEmpty() const {
        return address.isNullOrEmpty();
    }

    MqttClient::MessageArrivedEventArgs::MessageArrivedEventArgs(const String &topic, const String &payload) {
        this->topic = topic;
        this->topicFrame = MqttClient::Text;
        this->payloadText = payload;
    }

    MqttClient::MessageArrivedEventArgs::MessageArrivedEventArgs(const String &topic, const ByteArray &payload) {
        this->topic = topic;
        this->topicFrame = MqttClient::Binary;
        this->payloadArray = payload;
    }

    MqttClient::MessageArrivedEventArgs::MessageArrivedEventArgs(const Message *message) {
        this->topic = message->topic;
        this->topicFrame = message->topicFrame;
        this->payloadText = message->payloadText;
        this->payloadArray = message->payloadArray;
    }

    MqttClient::MessageArrivedEventArgs::~MessageArrivedEventArgs() {
    }

    MqttClient::Message::Message(const String &topic, const String &payload) {
        this->topic = topic;
        this->topicFrame = MqttClient::Text;
        this->payloadText = payload;
    }

    MqttClient::Message::Message(const String &topic, const ByteArray &payload) {
        this->topic = topic;
        this->topicFrame = MqttClient::Binary;
        this->payloadArray = payload;
    }

    MqttClient::Message::~Message() {
    }

    MqttClient::ErrorHandle::ErrorHandle() {
        detectionCount = 3;
        checkInterval = TimeSpan::fromSeconds(30);

        errorCount = 0;
    }

    void MqttClient::ErrorHandle::operator=(const ErrorHandle &value) {
        detectionCount = value.detectionCount;
        checkInterval = value.checkInterval;
    }

    bool MqttClient::ErrorHandle::operator==(const ErrorHandle &value) const {
        return detectionCount == value.detectionCount && checkInterval == value.checkInterval;
    }

    bool MqttClient::ErrorHandle::operator!=(const ErrorHandle &value) const {
        return !operator==(value);
    }

    MqttClient::MqttClient(const String &clientId) : _client(nullptr), _deliveredtoken(0), _clientId(clientId),
                                                     _connecting(false), _errorCode(MQTTCLIENT_SUCCESS) {
        _messageTimer = new Timer("mqtt.client.message.timer", messageCallback, this, 100);
        _checkTimer = nullptr;
    }

    MqttClient::~MqttClient() {
        delete _messageTimer;
        _messageTimer = nullptr;

        if (_checkTimer != nullptr) {
            delete _checkTimer;
            _checkTimer = nullptr;
        }

        if (connected())
            disconnect();

        destroy();
    }

    bool MqttClient::connect(const ConnectOptions &options) {
        _connecting = true;

        if (_checkTimer == nullptr)
            _checkTimer = new Timer("mqtt.client.check.timer", checkCallback, this, _errorHandle.checkInterval);

        if (options.isEmpty()) {
            Trace::error("The mqtt connect option is empty!");
            _connecting = false;
            return false;
        }

        if (connected())
            disconnect();

        if (!_connectOptions.address.isNullOrEmpty() && _connectOptions.address != options.address)
            destroy();

        _connectOptions = options;
        Debug::writeFormatLine("connect._connectOptions.address: %s", _connectOptions.address.c_str());

        EventArgs args;
        _connectingDelegates.invoke(this, &args);

        int rc;
        _clientMutex.lock();
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

        if (_client == nullptr) {
            MQTTClient_create(&_client, options.address, _clientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);
            MQTTClient_setCallbacks(_client, this, connlost, msgarrvd, delivered);
        }
        if (_client == nullptr) {
            _clientMutex.unlock();
            Trace::error(String::format("Failed to connect mqtt broker'%s', address is incorrect.",
                                        _connectOptions.address.c_str()));
            _connecting = false;
            return false;
        }
        _clientMutex.unlock();

        if (!options.userName.isNullOrEmpty())
            conn_opts.username = options.userName.c_str();
        if (!options.password.isNullOrEmpty())
            conn_opts.password = options.password.c_str();
        conn_opts.connectTimeout = (int) options.connectTimeout.totalSeconds();
        conn_opts.keepAliveInterval = (int) options.keepAliveInterval.totalSeconds();
        conn_opts.cleansession = options.cleanSession ? 1 : 0;

        _clientMutex.lock();
        if ((rc = MQTTClient_connect(_client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
            _errorCode = rc;
            _clientMutex.unlock();
            Trace::error(String::format("Failed to connect mqtt broker'%s', return code %d.",
                                        _connectOptions.address.c_str(), rc));
            _connecting = false;
            return false;
        }
        _clientMutex.unlock();

        _connectedDelegates.invoke(this, &args);

        _connecting = false;
        return true;
    }

    void MqttClient::connectAsync(const ConnectOptions &options) {
        _connectOptions = options;
        ThreadPool::startAsync(connectAction, this);
    }

    void MqttClient::connectAction(ThreadHolder *holder) {
        MqttClient *mc = static_cast<MqttClient *>(holder->owner);
        assert(mc);

        mc->connect(mc->_connectOptions);

        delete holder;
    }

    bool MqttClient::disconnect(const TimeSpan &timeout) {
        //        Trace::info(String::format("The Mqtt client is disconnecting."));
        _clientMutex.lock();
        if (_client != nullptr) {
            EventArgs args;
            _disconnectingDelegates.invoke(this, &args);

            int rc = MQTTClient_disconnect(_client, (int) timeout.totalMilliseconds());
            _clientMutex.unlock();

            _disconnectedDelegates.invoke(this, &args);

            _errorCode = rc;
            return rc == MQTTCLIENT_SUCCESS;
        } else {
            _clientMutex.unlock();
        }
        return false;
    }

    bool MqttClient::connected() {
        Locker locker(&_clientMutex);
        if (_client != nullptr) {
            return MQTTClient_isConnected(_client) != 0;
        }
        return false;
    }

    bool MqttClient::destroy() {
        Locker locker(&_clientMutex);
        if (_client != nullptr) {
            MQTTClient_destroy(&_client);
            _client = nullptr;
            return true;
        }
        return false;
    }

    bool MqttClient::publishMessage(const String &topic, const String &payload, Qos qos, const TimeSpan &timeout) {
        if (!connected())
            return false;

        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        MQTTClient_deliveryToken token;

        int rc;
        pubmsg.payload = (void *) payload.c_str();
        pubmsg.payloadlen = (int) payload.length();
        pubmsg.qos = qos;
        pubmsg.retained = 0;

        Locker locker(&_clientMutex);
        rc = MQTTClient_publishMessage(_client, topic, &pubmsg, &token);
        if (rc == MQTTCLIENT_SUCCESS) {
            if (timeout != TimeSpan::Zero) {
                rc = MQTTClient_waitForCompletion(_client, token, (int) timeout.totalMilliseconds());
                if (rc != MQTTCLIENT_SUCCESS) {
                    _errorCode = rc;
                    Debug::writeLine(
                            String::format("MqttClient::publishMessage.waitForCompletion failed, error code: %d", rc));
                }
            }
        } else {
            _errorCode = rc;
            Debug::writeLine(String::format("MqttClient::publishMessage failed, error code: %d", rc));
        }
        return rc == MQTTCLIENT_SUCCESS;
    }

    bool MqttClient::publishMessage(const String &topic, const ByteArray &payload, Qos qos, const TimeSpan &timeout) {
        if (!connected())
            return false;

        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        MQTTClient_deliveryToken token;

        int rc;
        pubmsg.payload = (void *) payload.data();
        pubmsg.payloadlen = (int) payload.count();
        pubmsg.qos = qos;
        pubmsg.retained = 0;

        Locker locker(&_clientMutex);
        rc = MQTTClient_publishMessage(_client, topic, &pubmsg, &token);
        if (timeout != TimeSpan::Zero) {
            rc = MQTTClient_waitForCompletion(_client, token, (int) timeout.totalMilliseconds());
            if (rc != MQTTCLIENT_SUCCESS) {
                _errorCode = rc;
                Debug::writeLine(String::format("MqttClient::publishMessage failed, error code: %d", rc));
            }
        }
        return rc == MQTTCLIENT_SUCCESS;
    }

    bool MqttClient::publishMessageAsync(const String &topic, const String &payload, Qos qos) {
        return publishMessage(topic, payload, qos, TimeSpan::Zero);
    }

    bool MqttClient::publishMessageAsync(const String &topic, const ByteArray &payload, Qos qos) {
        return publishMessage(topic, payload, qos, TimeSpan::Zero);
    }

    bool MqttClient::subscribe(const String &topic, Qos qos, TopicFrame frame) {
        if (!connected())
            return false;

        _topicFrames.add(topic, frame);

        int rc;
        Locker locker(&_clientMutex);
        rc = MQTTClient_subscribe(_client, topic, qos);
        if (rc != MQTTCLIENT_SUCCESS) {
            _errorCode = rc;
            Debug::writeLine(String::format("MqttClient::subscribe'%s' failed, error code: %d", topic.c_str(), rc));
        }
        return rc == MQTTCLIENT_SUCCESS;
    }

    bool MqttClient::unsubscribe(const String &topic) {
        if (!connected())
            return false;

        _topicFrames.remove(topic);

        int rc;
        Locker locker(&_clientMutex);
        rc = MQTTClient_unsubscribe(_client, topic);
        if (rc != MQTTCLIENT_SUCCESS) {
            _errorCode = rc;
            Debug::writeLine(String::format("MqttClient::unsubscribe'%s' failed, error code: %d", topic.c_str(), rc));
        }
        return rc == MQTTCLIENT_SUCCESS;
    }

    Delegates *MqttClient::messageArrivedDelegates() {
        return &_messageArrivedDelegates;
    }

    Delegates *MqttClient::connectingDelegates() {
        return &_connectingDelegates;
    }

    Delegates *MqttClient::connectedDelegates() {
        return &_connectedDelegates;
    }

    Delegates *MqttClient::disconnectingDelegates() {
        return &_disconnectingDelegates;
    }

    Delegates *MqttClient::disconnectedDelegates() {
        return &_disconnectedDelegates;
    }

    void MqttClient::delivered(void *context, MQTTClient_deliveryToken dt) {
        //        Debug::writeFormatLine("Message with token value %d delivery confirmed.", dt);
        MqttClient *client = (MqttClient *) context;
        client->_deliveredtoken = dt;
    }

    int MqttClient::msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
        MqttClient *client = (MqttClient *) context;
        client->addMessage(topicName, topicLen, message);

        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
        return 1;
    }

    void MqttClient::addMessage(char *topicName, int topicLen, MQTTClient_message *message) {
        _messagesMutex.lock();
        TopicFrame frame;
        String topic(topicName, topicLen);
        //        Debug::writeFormatLine("Message arrived. topic: %s", topic.c_str());
        if (_topicFrames.at(topic, frame)) {
            if (frame == TopicFrame::Text) {
                String payload((const char *) message->payload, message->payloadlen);
                Message *m = new Message(topic, payload);
                _messages.enqueue(m);
            } else if (frame == TopicFrame::Binary) {
                ByteArray payload((const uint8_t *) message->payload, message->payloadlen);
                Message *m = new Message(topic, payload);
                _messages.enqueue(m);
            }
        } else {
            String payload((const char *) message->payload, message->payloadlen);
            Message *m = new Message(topic, payload);
            _messages.enqueue(m);
        }
        _messagesMutex.unlock();
    }

    void MqttClient::connlost(void *context, char *cause) {
        Debug::writeLine("Connection lost.");
        Debug::writeFormatLine("cause: %s", cause);
    }

    void MqttClient::messageCallback(void *state) {
        MqttClient *client = (MqttClient *) state;
        assert(client);
        client->processMessage();
    }

    void MqttClient::checkCallback(void *state) {
        MqttClient *client = (MqttClient *) state;
        assert(client);
        client->check();
    }

    void MqttClient::processMessage() {
        _messagesMutex.lock();
        size_t count = _messages.count();
        if (count > 0) {
            Message **messages = new Message *[count];
            _messages.copyTo(messages);
            _messages.makeNull(false);
            _messagesMutex.unlock();

            for (uint32_t i = 0; i < count; i++) {
                Message *message = messages[i];
                MessageArrivedEventArgs args(message);
                _messageArrivedDelegates.invoke(this, &args);
                delete message;
            }

            delete[] messages;
        } else {
            _messagesMutex.unlock();
        }
    }

    void MqttClient::check() {
        if (!connecting() && !connected()) {
            //            Debug::writeLine("Mqtt client is not be connected!");
            _errorHandle.errorCount++;
            if (_errorHandle.errorCount >= _errorHandle.detectionCount) {
                ConnectOptions options = _connectOptions;
                Trace::info(String::format("Mqtt client'%s' is reconnecting! user name: '%s', password: '%s'",
                                           options.address.c_str(), options.userName.c_str(),
                                           options.password.c_str()));
                _errorHandle.errorCount = 0;

                disconnect();
                destroy();
                connect(options);
            }
        }
    }

    void MqttClient::setErrorHandle(const MqttClient::ErrorHandle &errorHandle) {
        _errorHandle = errorHandle;
    }

    bool MqttClient::connecting() const {
        return _connecting;
    }

    int MqttClient::errorCode() const {
        return _errorCode;
    }

    String MqttClient::errorMsg() const {
        switch (errorCode()) {
            case MQTTCLIENT_SUCCESS:
                return "No error.";
            case MQTTCLIENT_FAILURE:
                return "A generic error code indicating the failure of an MQTT client operation.";
            case MQTTCLIENT_PERSISTENCE_ERROR:
                return "Application-specific persistence functions must return this error code if there is a problem executing the function. ";
            case MQTTCLIENT_DISCONNECTED:
                return "The client is disconnected.";
            case MQTTCLIENT_MAX_MESSAGES_INFLIGHT:
                return "The maximum number of messages allowed to be simultaneously in-flight has been reached.";
            case MQTTCLIENT_BAD_UTF8_STRING:
                return "An invalid UTF-8 string has been detected.";
            case MQTTCLIENT_NULL_PARAMETER:
                return "A NULL parameter has been supplied when this is invalid.";
            case MQTTCLIENT_TOPICNAME_TRUNCATED:
                return "The topic has been truncated (the topic string includes embedded NULL characters)";
            case MQTTCLIENT_BAD_STRUCTURE:
                return "A structure parameter does not have the correct eyecatcher and version number.";
            case MQTTCLIENT_BAD_QOS:
                return "A QoS value that falls outside of the acceptable range (0,1,2)";
            case MQTTCLIENT_SSL_NOT_SUPPORTED:
                return "Attempting SSL connection using non-SSL version of library";
            case MQTTCLIENT_BAD_MQTT_VERSION:
                return "unrecognized MQTT version";
            case MQTTCLIENT_BAD_PROTOCOL:
                return "protocol prefix in serverURI should be tcp:// or ssl://";
            case MQTTCLIENT_BAD_MQTT_OPTION:
                return "option not applicable to the requested version of MQTT";
            case MQTTCLIENT_WRONG_MQTT_VERSION:
                return "call not applicable to the requested version of MQTT";
            default:
                return String::format("Undefined code: %d", errorCode());
        }
    }
}
#endif
