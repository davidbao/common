//
//  RedisClient.cpp
//  common
//
//  Created by baowei on 2022/2/16.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "net/RedisClient.h"
#include "data/ValueType.h"
#include "thread/Timer.h"
#include "diag/Trace.h"
#include "system/Delegate.h"
#include "thread/ThreadPool.h"

#ifdef WIN32
#include <winsock.h>
#else

#include <sys/time.h>

#endif

using namespace Diag;

namespace Net {
    RedisClient::ConnectOptions::ConnectOptions() {
        port = 6379;
        connectTimeout = TimeSpan::fromSeconds(10);
        commandTimeout = TimeSpan::fromSeconds(10);
    }

    RedisClient::ConnectOptions::ConnectOptions(const ConnectOptions &options) {
        this->operator=(options);
    }

    RedisClient::ConnectOptions::~ConnectOptions() {
    }

    void RedisClient::ConnectOptions::operator=(const ConnectOptions &value) {
        this->address = value.address;
        this->port = value.port;
        this->userName = value.userName;
        this->password = value.password;
        this->connectTimeout = value.connectTimeout;
        this->commandTimeout = value.commandTimeout;
    }

    bool RedisClient::ConnectOptions::operator==(const ConnectOptions &value) const {
        return this->address == value.address &&
               this->port == value.port &&
               this->userName == value.userName &&
               this->password == value.password &&
               this->connectTimeout == value.connectTimeout &&
               this->commandTimeout == value.commandTimeout;
    }

    bool RedisClient::ConnectOptions::operator!=(const ConnectOptions &value) const {
        return !operator==(value);
    }

    bool RedisClient::ConnectOptions::isEmpty() const {
        return address.isNullOrEmpty();
    }

    RedisClient::ErrorHandle::ErrorHandle() {
        detectionCount = 3;
        checkInterval = TimeSpan::fromSeconds(30);

        errorCount = 0;
    }

    void RedisClient::ErrorHandle::operator=(const ErrorHandle &value) {
        detectionCount = value.detectionCount;
        checkInterval = value.checkInterval;
    }

    bool RedisClient::ErrorHandle::operator==(const ErrorHandle &value) const {
        return detectionCount == value.detectionCount && checkInterval == value.checkInterval;
    }

    bool RedisClient::ErrorHandle::operator!=(const ErrorHandle &value) const {
        return !operator==(value);
    }

    RedisClient::RedisClient() : _context(nullptr), _checkTimer(nullptr), _connecting(false) {
    }

    RedisClient::~RedisClient() {
        if (_checkTimer != nullptr) {
            delete _checkTimer;
            _checkTimer = nullptr;
        }

        if (connected())
            disconnect();

        destroy();
    }

    bool RedisClient::connect(const ConnectOptions &options) {
        _connecting = true;

        if (_checkTimer == nullptr)
            _checkTimer = new Timer("redis.client.check.timer", checkCallback, this, _errorHandle.checkInterval);

        if (options.isEmpty()) {
            Trace::error("The redis connect option is empty!");
            _connecting = false;
            return false;
        }

        if (connected())
            disconnect();

        _connectOptions = options;
        Debug::writeFormatLine("connect._connectOptions.address: %s", _connectOptions.address.c_str());

        EventArgs args;
        _connectingDelegates.invoke(this, &args);

        _contextMutex.lock();
        redisOptions roptions = {0};
        REDIS_OPTIONS_SET_TCP(&roptions, options.address, options.port);
        uint32_t timeout = (uint32_t) options.connectTimeout.totalMilliseconds();
        struct timeval tv = {0};
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = 1000 * (timeout % 1000);
        roptions.connect_timeout = &tv;
        timeout = (uint32_t) options.commandTimeout.totalMilliseconds();
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = 1000 * (timeout % 1000);
        roptions.command_timeout = &tv;
        _context = redisConnectWithOptions(&roptions);
        if (_context == NULL || _context->err) {
            _contextMutex.unlock();
            Trace::error(String::format("Failed to connect redis server'%s:%d', error: %s.",
                                        options.address.c_str(), options.port,
                                        _context != nullptr ? _context->errstr : "Can't allocate redis context"));
            _connecting = false;
            return false;
        }

        if (!options.password.isNullOrEmpty()) {
            redisReply *reply = (redisReply *) redisCommand(_context, "AUTH %s", options.password.c_str());
            if (reply->type == REDIS_REPLY_ERROR) {
                _contextMutex.unlock();
                Trace::error(String::format("Failed to authorize redis server'%s:%d'.",
                                            options.address.c_str(), options.port));
                _connecting = false;
                return false;
            }
            freeReplyObject(reply);
        }
        _contextMutex.unlock();

        _connectedDelegates.invoke(this, &args);

        _connecting = false;
        return true;
    }

    void RedisClient::connectAsync(const ConnectOptions &options) {
        _connectOptions = options;
        ThreadPool::startAsync(connectAction, this);
    }

    void RedisClient::connectAction(ThreadHolder *holder) {
        RedisClient *rc = static_cast<RedisClient *>(holder->owner);
        assert(rc);

        rc->connect(rc->_connectOptions);

        delete holder;
    }

    bool RedisClient::disconnect(TimeSpan timeout) {
        //        Trace::info(String::format("The redis client is disconnecting."));
        Locker locker(&_contextMutex);
        if (_context != nullptr) {
            EventArgs args;
            _disconnectingDelegates.invoke(this, &args);

            redisFree(_context);
            _context = nullptr;

            _disconnectedDelegates.invoke(this, &args);

            return true;
        }
        return false;
    }

    bool RedisClient::connected() {
        Locker locker(&_contextMutex);
        if (_context != nullptr) {
            return (_context->flags & REDIS_CONNECTED) == REDIS_CONNECTED;
        }
        return false;
    }

    bool RedisClient::set(const String &key, const String &value, const TimeSpan &expired) {
        if (key.isNullOrEmpty())
            return false;

        if (connected()) {
            redisReply *reply = (redisReply *) redisCommand(_context, "SET %s %s", key.c_str(), value.c_str());
            freeReplyObject(reply);
            if (reply->type != REDIS_REPLY_ERROR) {
                if (expired == TimeSpan::Zero)
                    return true;

                String str = UInt32((uint32_t) expired.totalMilliseconds()).toString();
                reply = (redisReply *) redisCommand(_context, "PEXPIRE %s %s", key.c_str(), str.c_str());
                return reply->type != REDIS_REPLY_ERROR;
            }
        }
        return false;
    }

    bool RedisClient::get(const String &key, String &value) {
        return getCommand("GET", key, value);
    }

    bool RedisClient::setCommand(const String &command, const String &key, const String &value) {
        if (command.isNullOrEmpty())
            return false;
        if (key.isNullOrEmpty())
            return false;

        if (connected()) {
            redisReply *reply = (redisReply *) redisCommand(_context, "%s %s %s", command.c_str(), key.c_str(),
                                                            value.c_str());
            freeReplyObject(reply);
            return reply->type != REDIS_REPLY_ERROR;
        }
        return false;
    }

    bool RedisClient::getCommand(const String &command, const String &key, String &value) {
        if (command.isNullOrEmpty())
            return false;
        if (key.isNullOrEmpty())
            return false;

        if (connected()) {
            bool result = false;
            redisReply *reply = (redisReply *) redisCommand(_context, "%s %s", command.c_str(), key.c_str());
            switch (reply->type) {
                case REDIS_REPLY_STRING:
                    result = true;
                    value = String(reply->str, reply->len);
                    break;
                case REDIS_REPLY_ARRAY:
                    break;
                case REDIS_REPLY_INTEGER:
                    result = true;
                    value = Int64(reply->integer).toString();
                    break;
                case REDIS_REPLY_NIL:
                    break;
                case REDIS_REPLY_STATUS:
                    break;
                case REDIS_REPLY_ERROR:
                    break;
                case REDIS_REPLY_DOUBLE:
                    result = true;
                    value = Double(reply->dval).toString();
                    break;
                case REDIS_REPLY_BOOL:
                    result = true;
                    value = Boolean(reply->integer != 0 ? true : false).toString();
                    break;
                case REDIS_REPLY_MAP:
                    break;
                case REDIS_REPLY_SET:
                    break;
                case REDIS_REPLY_ATTR:
                    break;
                case REDIS_REPLY_PUSH:
                    break;
                case REDIS_REPLY_BIGNUM:
                    break;
                case REDIS_REPLY_VERB:
                    break;

                default:
                    break;
            }
            freeReplyObject(reply);
            return result;
        }
        return false;
    }

    bool RedisClient::destroy() {
        Locker locker(&_contextMutex);
        if (_context != nullptr) {
            redisFree(_context);
            _context = nullptr;
            return true;
        }
        return false;
    }

    void RedisClient::checkCallback(void *state) {
        RedisClient *client = (RedisClient *) state;
        assert(client);
        client->check();
    }

    void RedisClient::check() {
        if (!connecting() && !connected()) {
            //            Debug::writeLine("Redis client is not be connected!");
            _errorHandle.errorCount++;
            if (_errorHandle.errorCount >= _errorHandle.detectionCount) {
                ConnectOptions options = _connectOptions;
                Trace::info(String::format("Redis client'%s' is reconnecting! user name: '%s', password: '%s'",
                                           options.address.c_str(), options.userName.c_str(),
                                           options.password.c_str()));
                _errorHandle.errorCount = 0;

                disconnect();
                destroy();
                connect(options);
            }
        }
    }

    void RedisClient::setErrorHandle(const RedisClient::ErrorHandle &errorHandle) {
        _errorHandle = errorHandle;
    }

    bool RedisClient::connecting() const {
        return _connecting;
    }
}
