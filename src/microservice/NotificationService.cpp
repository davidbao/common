//
//  NotificationService.cpp
//  common
//
//  Created by baowei on 2020/12/15.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/NotificationService.h"
#include "system/ServiceFactory.h"
#include "configuration/ConfigService.h"

using namespace Data;
using namespace Diag;
using namespace Config;

namespace Microservice {
    BaseNotification::BaseNotification(NotificationType type) : _type(type) {
    }

    BaseNotification::~BaseNotification() {
    }

    NotificationType BaseNotification::type() const {
        return _type;
    }

    HttpNotification::HttpNotification(const Url &url) : BaseNotification(NotificationType::Http), _url(url) {
        if (!_url.isEmpty())
            _webClient.start();
    }

    HttpNotification::~HttpNotification() {
        _webClient.stop();
    }

    void HttpNotification::push(const String &key, const String &value) {
        if (!_url.isEmpty())
            _webClient.postAsync(_url, HttpHeaders::JsonTypeHeaders, value);
    }

    MqttNotification::MqttNotification(const MqttClient::ConnectOptions &options) : BaseNotification(
            NotificationType::Mqtt) {
        _connectOptions = options;
        _service = new MqttClient(String::format("Notification-%s", Uuid::generate().toString().c_str()));

        _initTimerMutex.lock();
        TimeSpan interval = TimeSpan::fromSeconds(3);
        _initTimer = new Timer("mqtt.notification.init.timer", initTimeUp, this, interval);
        _initTimerMutex.unlock();
    }

    MqttNotification::~MqttNotification() {
        unInitClientService();

        delete _service;
        _service = nullptr;
    }

    void MqttNotification::push(const String &key, const String &value) {
        if (_service->connected() && !key.isNullOrEmpty())
            _service->publishMessageAsync(key, value);
    }

    bool MqttNotification::initClientService() {
        return _service->connect(_connectOptions);
    }

    bool MqttNotification::unInitClientService() {
        _initTimerMutex.lock();
        if (_initTimer != nullptr) {
            delete _initTimer;
            _initTimer = nullptr;
        }
        _initTimerMutex.unlock();

        _service->disconnect();

        return true;
    }

    void MqttNotification::initTimeUp(void *owner) {
        MqttNotification *ms = (MqttNotification *) owner;
        assert(ms);

        if (ms->initClientService()) {
            if (ms->_initTimer != nullptr)
                ms->_initTimer->stop();
        }
    }

    RedisNotification::RedisNotification() : BaseNotification(NotificationType::Redis) {
    }

    RedisNotification::~RedisNotification() {
    }

    void RedisNotification::push(const String &key, const String &value) {
//        String rdisKey = String::replace(key, "/", ":");
    }

    void INotificationService::push(const String &key, const String &value) {
        push(NotificationType::All, key, value);
    }

    NotificationService::ValueEntry::ValueEntry(NotificationType type, const String &key, const String &value) : type(
            type), key(key), value(value) {
    }

    NotificationService::NotificationService() : PoolService(false) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<INotificationService>(this);
    }

    NotificationService::~NotificationService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<INotificationService>();
    }

    bool NotificationService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        bool enabled = false;
        static const int MaxCount = 10;
        for (int i = 0; i < MaxCount; i++) {
            String prefix = String::format("summer.notification[%d]", i);
            if (!cs->getProperty(String::format("%s.enabled", prefix.c_str()), enabled)) {
                break;
            } else {
                if (enabled) {
                    BaseNotification *n = nullptr;

                    String type;
                    if (cs->getProperty(String::format("%s.type", prefix.c_str()), type)) {
                        if (String::equals(type, "http", true)) {
                            Url url;
                            if (cs->getProperty(String::format("%s.url", prefix.c_str()), url) && !url.isEmpty())
                                n = new HttpNotification(url);
                            else
                                Trace::warn("The notification http url is incorrect.");
                        } else if (String::equals(type, "mqtt", true)) {
                            MqttClient::ConnectOptions options;
                            cs->getProperty(String::format("%s.address", prefix.c_str()), options.address);
                            cs->getProperty(String::format("%s.userName", prefix.c_str()), options.userName);
                            cs->getProperty(String::format("%s.password", prefix.c_str()), options.password);
                            cs->getProperty(String::format("%s.keepAliveInterval", prefix.c_str()),
                                            options.keepAliveInterval);
                            cs->getProperty(String::format("%s.connectTimeout", prefix.c_str()),
                                            options.connectTimeout);
                            cs->getProperty(String::format("%s.cleansession", prefix.c_str()), options.cleansession);
                            n = new MqttNotification(options);
                        }
                    }

                    if (n != nullptr) {
                        _notificationsMutex.lock();
                        _notifications.add(n);
                        _notificationsMutex.unlock();
                    }
                }
            }
        }

        _notificationsMutex.lock();
        if (_notifications.count() == 0)
            Trace::debug("The notification is disabled.");
        else
            start();
        _notificationsMutex.unlock();

        return true;
    }

    bool NotificationService::unInitialize() {
        stop();

        _notificationsMutex.lock();
        _notifications.clear();
        _notificationsMutex.unlock();

        return true;
    }

    bool NotificationService::allowPush() const {
        return _notifications.count() > 0;
    }

    void NotificationService::push(NotificationType type, const String &key, const String &value) {
        add(new ValueEntry(type, key, value));
    }

    void NotificationService::process(const IPoolEntry *value) {
        const ValueEntry *entry = dynamic_cast<const ValueEntry *>(value);
        assert(entry);

        Locker locker(&_notificationsMutex);
        for (uint32_t i = 0; i < _notifications.count(); i++) {
            BaseNotification *n = _notifications[i];
            if ((entry->type & NotificationType::Http) == NotificationType::Http &&
                n->type() == NotificationType::Http) {
                n->push(entry->key, entry->value);
            }
            if ((entry->type & NotificationType::Mqtt) == NotificationType::Mqtt &&
                n->type() == NotificationType::Mqtt) {
                n->push(entry->key, entry->value);
            }
            if ((entry->type & NotificationType::Redis) == NotificationType::Redis &&
                n->type() == NotificationType::Redis) {
                n->push(entry->key, entry->value);
            }
            if ((entry->type & NotificationType::Kafka) == NotificationType::Kafka &&
                n->type() == NotificationType::Kafka) {
                n->push(entry->key, entry->value);
            }
        }
    }
}
