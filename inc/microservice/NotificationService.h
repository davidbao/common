//
//  NotificationService.h
//  common
//
//  Created by baowei on 2020/12/15.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef NotificationService_h
#define NotificationService_h

#include "http/HttpClient.h"
#include "data/Dictionary.h"
#ifdef HAS_MQTTCLIENT
#include "net/MqttClient.hpp"
#endif
#include "system/ServiceFactory.h"

using namespace Data;
using namespace Http;

namespace Microservice {
    enum NotificationType {
        Http = 1,
        Mqtt = 2,
        Redis = 4,
        Kafka = 8,
        All = Http | Mqtt | Redis | Kafka
    };

    class INotificationService : public IService {
    public:
        virtual bool allowPush() const = 0;

        virtual void push(NotificationType type, const String &key, const String &value) = 0;

        void push(const String &key, const String &value);
    };

    class BaseNotification {
    public:
        explicit BaseNotification(NotificationType type);

        virtual ~BaseNotification();

        virtual void push(const String &key, const String &value) = 0;

        NotificationType type() const;

    private:
        NotificationType _type;
    };

    class HttpNotification : public BaseNotification {
    public:
        explicit HttpNotification(const Url &url);

        ~HttpNotification() override;

        void push(const String &key, const String &value) override;

    private:
        Url _url;
        HttpClient _webClient;
    };

#ifdef HAS_MQTTCLIENT
    class MqttNotification : public BaseNotification {
    public:
        explicit MqttNotification(const MqttClient::ConnectOptions &options);

        ~MqttNotification() override;

        void push(const String &key, const String &value) override;

    private:
        bool initClientService();

        bool unInitClientService();

    private:
        Mutex _initTimerMutex;
        Timer *_initTimer;

        MqttClient::ConnectOptions _connectOptions;
        MqttClient *_service;
    };
#endif

#ifdef HAS_REDISCLIENT
    class RedisNotification : public BaseNotification {
    public:
        RedisNotification();

        ~RedisNotification() override;

        void push(const String &key, const String &value) override;

    private:
    };
#endif

    typedef PList<BaseNotification> Notifications;

    class NotificationService : public INotificationService, public PoolService {
    public:
        NotificationService();

        ~NotificationService() override;

        bool initialize();

        bool unInitialize();

        bool allowPush() const override;

        void push(NotificationType type, const String &key, const String &value) override;

    protected:
        void process(const IPoolEntry *value) override;

    private:
        Mutex _notificationsMutex;
        Notifications _notifications;

        class ValueEntry : public IPoolEntry {
        public:
            NotificationType type;
            String key;
            String value;

            ValueEntry(NotificationType type, const String &key, const String &value);
        };
    };
}

#endif /* NotificationService_h */
