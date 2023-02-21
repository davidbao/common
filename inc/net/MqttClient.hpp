//
//  MqttClient.hpp
//  common
//
//  Created by baowei on 2018/9/21.
//  Copyright (c) 2019 com. All rights reserved.
//

#ifndef MqttClient_hpp
#define MqttClient_hpp

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__)

#include "data/String.h"
#include "data/ByteArray.h"
#include "data/Dictionary.h"
#include "data/TimeSpan.h"
#include "data/LoopPList.h"
#include "system/Delegate.h"
#include "thread/Thread.h"
#include "thread/Timer.h"
#include "MQTTClient.h"

using namespace Data;
using namespace System;

namespace Net {
    class MqttClient {
    public:
        enum Qos {
            Qos0 = 0,
            Qos1 = 1,
            Qos2 = 2
        };
        enum TopicFrame {
            Text = 0,
            Binary = 1
        };

        class ConnectOptions {
        public:
            ConnectOptions();

            ConnectOptions(const ConnectOptions &options);

            ~ConnectOptions();

            void operator=(const ConnectOptions &value);

            bool operator==(const ConnectOptions &value) const;

            bool operator!=(const ConnectOptions &value) const;

            bool isEmpty() const;

            String address;
            String userName;
            String password;
            TimeSpan keepAliveInterval;
            TimeSpan connectTimeout;
            bool cleanSession;
        };

        class Message {
        public:
            Message(const String &topic, const String &payload);

            Message(const String &topic, const ByteArray &payload);

            ~Message();

            String topic;
            MqttClient::TopicFrame topicFrame;
            String payloadText;
            ByteArray payloadArray;
        };

        class MessageArrivedEventArgs : public EventArgs {
        public:
            MessageArrivedEventArgs(const String &topic, const String &payload);

            MessageArrivedEventArgs(const String &topic, const ByteArray &payload);

            MessageArrivedEventArgs(const Message *message);

            ~MessageArrivedEventArgs() override;

            String topic;
            MqttClient::TopicFrame topicFrame;
            String payloadText;
            ByteArray payloadArray;
        };

        class ErrorHandle {
        public:
            int detectionCount;
            TimeSpan checkInterval;

            int errorCount;

            ErrorHandle();

            void operator=(const ErrorHandle &value);

            bool operator==(const ErrorHandle &value) const;

            bool operator!=(const ErrorHandle &value) const;
        };

        MqttClient(const String &clientId);

        ~MqttClient();

        bool connect(const ConnectOptions &options);

        void connectAsync(const ConnectOptions &options);

        bool disconnect(const TimeSpan &timeout = TimeSpan::fromSeconds(10));

        bool connected();

        bool publishMessage(const String &topic, const String &payload, Qos qos = Qos::Qos1,
                            const TimeSpan &timeout = TimeSpan::fromSeconds(10));

        bool publishMessage(const String &topic, const ByteArray &payload, Qos qos = Qos::Qos1,
                            const TimeSpan &timeout = TimeSpan::fromSeconds(10));

        bool publishMessageAsync(const String &topic, const String &payload, Qos qos = Qos::Qos1);

        bool publishMessageAsync(const String &topic, const ByteArray &payload, Qos qos = Qos::Qos1);

        bool subscribe(const String &topic, Qos qos = Qos::Qos1, TopicFrame frame = TopicFrame::Text);

        bool unsubscribe(const String &topic);

        Delegates *messageArrivedDelegates();

        Delegates *connectingDelegates();

        Delegates *connectedDelegates();

        Delegates *disconnectingDelegates();

        Delegates *disconnectedDelegates();

        void setErrorHandle(const ErrorHandle &errorHandle);

        int errorCode() const;

        String errorMsg() const;

    private:
        bool destroy();

        void addMessage(char *topicName, int topicLen, MQTTClient_message *message);

        void processMessage();

        void check();

        bool connecting() const;

    private:
        static void delivered(void *context, MQTTClient_deliveryToken dt);

        static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message);

        static void connlost(void *context, char *cause);

        static void connectAction(ThreadHolder *holder);

        static void messageCallback(void *state);

        static void checkCallback(void *state);

    private:
        MQTTClient _client;
        Mutex _clientMutex;

        MQTTClient_deliveryToken _deliveredtoken;
        ConnectOptions _connectOptions;
        ErrorHandle _errorHandle;

        String _clientId;

        Dictionary<String, TopicFrame> _topicFrames;

        Delegates _messageArrivedDelegates;
        Delegates _connectingDelegates;
        Delegates _connectedDelegates;
        Delegates _disconnectingDelegates;
        Delegates _disconnectedDelegates;

        Timer *_messageTimer;
        Mutex _messagesMutex;
        LoopPList<Message> _messages;

        Timer *_checkTimer;
        bool _connecting;

        int _errorCode;
    };
}
#endif

#endif /* MqttClient_h */
