//
//  SqlConnection.cpp
//  common
//
//  Created by baowei on 2023/7/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/SqlConnection.h"
#include "net/NetType.h"
#include "diag/Trace.h"
#include "system/Environment.h"

using namespace System;
using namespace Net;
using namespace Diag;

namespace Database {
    SqlConnection::DataSource::DataSource(DbClient *client) : client(client) {
        openedTick = Environment::getTickCount();
        executedTick = openedTick;
    }

    SqlConnection::DataSource::~DataSource() {
        if (client != nullptr) {
            client->close();
            delete client;
            client = nullptr;
        }
    }

    void SqlConnection::DataSource::update() {
        executedTick = Environment::getTickCount();
    }

    bool SqlConnection::DataSource::ping() const {
        return client != nullptr && client->ping();
    }

    bool SqlConnection::DataSource::reopen(const StringMap &connections) {
        if (client != nullptr) {
            client->close();
            return client->open(connections);
        }
        return false;
    }

    bool SqlConnection::DataSource::close() {
        if (client != nullptr) {
            return client->close();
        }
        return false;
    }

    void SqlConnection::DataSource::lock() {
        _mutex.lock();
    }

    bool SqlConnection::DataSource::tryLock() {
        return _mutex.tryLock();
    }

    void SqlConnection::DataSource::unlock() {
        _mutex.unlock();
    }

    SqlConnection::SqlConnection() : _pingTimer(nullptr) {
        init(String::Empty);
    }

    SqlConnection::SqlConnection(const String &connectionStr) : _pingTimer(nullptr) {
        init(connectionStr);
    }

    SqlConnection::SqlConnection(const Url &url, const String &user, const String &password) : _pingTimer(nullptr) {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", user.c_str());
        connectionStr.appendFormat(fmt, "password", password.c_str());
        init(connectionStr);
    }

    SqlConnection::SqlConnection(std::initializer_list<KeyValuePair<String, String>> list) : _pingTimer(nullptr) {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        for (const auto &item: list) {
            connectionStr.appendFormat(fmt, item.key.c_str(), item.value.c_str());
        }
        init(connectionStr);
    }

    void SqlConnection::init(const String &connectionStr) {
        Url url;
        Port port;
        String ds, scheme, host, dbname, user;
        StringMap connections;
        StringArray texts;
        StringArray::parse(connectionStr, texts, ';');
        for (size_t i = 0; i < texts.count(); ++i) {
            StringArray keyValues;
            StringArray::parse(texts[i], keyValues, '=');
            if (keyValues.count() == 2) {
                String key = keyValues[0].trim(' ');
                String value = keyValues[1].trim(' ');
                if (String::equals(key, "url", true)) {
                    Url::parse(value, url);
                } else if (String::equals(key, "Data Source", true) ||
                           String::equals(key, "ds", true)) {
                    ds = value;
                } else if (String::equals(key, "user", true)) {
                    user = value;
                    connections.add("user", user);
                } else if (String::equals(key, "password", true)) {
                    connections.add("password", value);
                } else if (String::equals(key, "timeout", true) ||
                           String::equals(key, "Connection Timeout", true) ||
                           String::equals(key, "Connect Timeout", true)) {
                    int v;
                    if (Int32::parse(value, v)) {
                        connections.add("timeout", value);
                    }
                } else if (String::equals(key, "address", true) ||
                           String::equals(key, "host", true) ||
                           String::equals(key, "addr", true)) {
                    host = value;
                    connections.add("host", host);
                } else if (String::equals(key, "port", true)) {
                    Port v;
                    if (Port::parse(value, v)) {
                        port = v;
                        connections.add("port", port);
                    }
                } else if (String::equals(key, "dbname", true) ||
                           String::equals(key, "database", true) ||
                           String::equals(key, "db name", true)) {
                    dbname = value;
                    connections.add("dbname", dbname);
                } else if (String::equals(key, "scheme", true)) {
                    connections.add("scheme", scheme);
                } else if (String::equals(key, "minCount", true) ||
                           String::equals(key, "minConnectionCount", true)) {
                    int v;
                    if (Int32::parse(value, v) && v >= 1 && v <= 10) {
                        connections.add("minCount", v);
                    }
                } else if (String::equals(key, "maxCount", true) ||
                           String::equals(key, "maxConnectionCount", true)) {
                    int v;
                    if (Int32::parse(value, v) && v >= 1 && v <= 50) {
                        connections.add("maxCount", v);
                    }
                } else if (String::equals(key, "pingCycle", true) ||
                         String::equals(key, "ping cycle", true)) {
                    TimeSpan v;
                    if (TimeSpan::parse(value, v) &&
                        v >= TimeSpan::fromSeconds(3) && v <= TimeSpan::fromSeconds(600)) {
                        connections.add("pingCycle", v);
                    }
                } else if (String::equals(key, "idle", true)) {
                    TimeSpan v;
                    if (TimeSpan::parse(value, v) &&
                        v >= TimeSpan::fromMinutes(1) && v <= TimeSpan::fromMinutes(60)) {
                        connections.add("idle", v);
                    }
                } else if (String::equals(key, "local_code", true) ||
                        String::equals(key, "localCode", true) ||
                        String::equals(key, "encoding", true)) {
                    connections.add("encoding", value);
                }
            }
        }

        // combine url.
        if (url.isEmpty()) {
            url = Url(ds, Endpoint(host, port), dbname);
        } else {
            connections.add("host", url.address());
            connections.add("port", url.port());
            connections.add("dbname", url.relativeUrl());
        }
        if (!url.isEmpty()) {
            connections.add("url", url);
        }

        // default properties.
        if (!connections.contains("minCount")) {
            connections.add("minCount", 1);
        }
        if (!connections.contains("maxCount")) {
            connections.add("maxCount", 5);
        }
        if (!connections.contains("pingCycle")) {
            connections.add("pingCycle", TimeSpan::fromSeconds(30));
        }
        if (!connections.contains("idle")) {
            connections.add("idle", TimeSpan::fromMinutes(5));
        }

        _connections = connections;
    }

    SqlConnection::~SqlConnection() {
        if (_pingTimer != nullptr) {
            delete _pingTimer;
            _pingTimer = nullptr;
        }

        close();
    }

    bool SqlConnection::open() {
        // check url.
        Url url;
        getProperty("url", url);
        if (url.isEmpty()) {
            Trace::error("Can not find the database url!");
            return false;
        }
        String user;
        getProperty("user", user);
        if (user.isNullOrEmpty()) {
            Trace::error("Can not find the database user!");
            return false;
        }

        // open it.
        const StringMap &connections = _connections;
        if (openInner(connections) != nullptr) {
            if (_pingTimer == nullptr) {
                if (hasPing()) {
                    TimeSpan pingCycle;
                    getProperty("pingCycle", pingCycle);
                    _pingTimer = new Timer("sql.ping.timer", pingCycle, pingCycle, &SqlConnection::timerProc, this);
                }
            }

            int minCount = 0;
            getProperty("minCount", minCount);
            for (int i = 1; i < minCount; ++i) {
                openInner(connections);
            }
            return true;
        } else {
            return false;
        }
    }

    DbClient *SqlConnection::openInner(const StringMap &connections) {
        Url url;
        Url::parse(connections["url"], url);
        auto client = DbClientFactory::create(url.scheme());
        if (client != nullptr) {
            if (client->open(connections)) {
                Locker locker(&_dataSources);
                _dataSources.add(new DataSource(client));
                return client;
            } else {
                delete client;
            }
        }
        return nullptr;
    }

    bool SqlConnection::isConnected() {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->isConnected();
        }
        return false;
    }

    bool SqlConnection::close() {
        Locker locker(&_dataSources);
        _dataSources.clear();
        return true;
    }

    bool SqlConnection::executeSql(const String &sql, bool transaction) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->executeSql(sql, transaction);
        }
        return false;
    }

    bool SqlConnection::executeSqlQuery(const String &sql, DataTable &table) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->executeSqlQuery(sql, table);
        }
        return false;
    }

    bool SqlConnection::executeSqlInsert(const DataTable &table, bool transaction) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->executeSqlInsert(table, transaction);
        }
        return false;
    }

    bool SqlConnection::executeSqlReplace(const DataTable &table, bool transaction) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->executeSqlReplace(table, transaction);
        }
        return false;
    }

    StringArray SqlConnection::getColumnName(const String &tableName) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->getColumnName(tableName);
        }
        return StringArray::Empty;
    }

    bool SqlConnection::beginTransaction() {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->beginTransaction();
        }
        return false;
    }

    bool SqlConnection::commitTransaction() {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->commitTransaction();
        }
        return false;
    }

    bool SqlConnection::rollbackTransaction() {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->rollbackTransaction();
        }
        return false;
    }


    bool SqlConnection::retrieveCount(const String &sql, int &count) {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->retrieveCount(sql, count);
        }
        return false;
    }

    Url SqlConnection::url() const {
        Url value;
        getProperty("url", value);
        return value;
    }

    String SqlConnection::dbName() const {
        String value;
        getProperty("dbname", value);
        return value;
    }

    String SqlConnection::scheme() const {
        String value;
        getProperty("scheme", value);
        return value;
    }

    size_t SqlConnection::numberOfConnections() {
        Locker locker(&_dataSources);
        return _dataSources.count();
    }

    bool SqlConnection::hasPing() const {
        TimeSpan pingCycle;
        if (getProperty("pingCycle", pingCycle)) {
            return pingCycle != TimeSpan::Zero;
        }
        return false;
    }

    DbClient *SqlConnection::getClient() {
        _dataSources.lock();
        size_t count = _dataSources.count();
        if (count > 0) {
            DataSource *ds = nullptr;
            for (size_t i = 0; i < _dataSources.count(); ++i) {
                auto temp = _dataSources[i];
                if (temp->client != nullptr) {
                    if (!temp->client->isExecuting()) {
                        temp->update();
                        ds = temp;
                        break;
                    }
                }
            }
            _dataSources.unlock();

            if (ds == nullptr) {
                int maxCount = 0;
                getProperty("maxCount", maxCount);
                if (count < maxCount) {
                    DbClient *client = openInner(_connections);
                    return client;
                }
            } else {
                return ds->client;
            }
        } else {
            _dataSources.unlock();
        }
        return nullptr;
    }

    void SqlConnection::timerProc() {
        if (_dataSources.tryLock()) {
            // keep the minimum number of connections if idle.
            TimeSpan idle;
            getProperty("idle", idle);
            if (idle != TimeSpan::Zero) {
                int minCount = 0;
                getProperty("minCount", minCount);
                DataSources removed(false);
                uint64_t current = Environment::getTickCount();
                for (size_t i = 0; i < _dataSources.count(); ++i) {
                    auto ds = _dataSources[i];
                    if (current - ds->executedTick >= (uint64_t) idle.totalMilliseconds()) {
                        if (_dataSources.count() - removed.count() > minCount) {
                            removed.add(ds);
                        }
                    }
                }
                for (size_t i = 0; i < removed.count(); ++i) {
                    _dataSources.remove(removed[i]);
                }
            }

            // ping.
            for (size_t i = 0; i < _dataSources.count(); ++i) {
                auto ds = _dataSources[i];
                if (!ds->ping()) {
                    ds->reopen(_connections);
                }
            }
            _dataSources.unlock();
        }
    }
}
