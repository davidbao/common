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

using namespace System;
using namespace Net;
using namespace Diag;

namespace Database {
    SqlConnection::SqlConnection() : SqlConnection(5, TimeSpan::fromSeconds(30)) {
    }

    SqlConnection::SqlConnection(int maxConnectionCount) : SqlConnection(maxConnectionCount,
                                                                         TimeSpan::fromSeconds(30)) {
    }

    SqlConnection::SqlConnection(const TimeSpan &pingCycle) : SqlConnection(5, pingCycle) {
    }

    SqlConnection::SqlConnection(int maxConnectionCount, const TimeSpan &pingCycle) {
        if (maxConnectionCount < 1) {
            _maxConnectionCount = 1;
        } else if (maxConnectionCount > 100) {
            _maxConnectionCount = 100;
        } else {
            _maxConnectionCount = maxConnectionCount;
        }

        if (pingCycle == TimeSpan::Zero) {
            // disable the ping timer.
        } else if (pingCycle < TimeSpan::fromSeconds(5)) {
            _pingCycle = TimeSpan::fromSeconds(5);
        } else if (pingCycle > TimeSpan::fromSeconds(600)) {
            _pingCycle = TimeSpan::fromSeconds(600);
        } else {
            _pingCycle = pingCycle;
        }

        _pingTimer = nullptr;
    }

    SqlConnection::SqlConnection(std::initializer_list<KeyValuePair<String, String>> list)
            : _maxConnectionCount(5), _pingCycle(TimeSpan::fromSeconds(30)), _pingTimer(nullptr) {
        for (const auto &item: list) {
            if (String::equals(item.key, "maxConnectionCount")) {
                Int32::parse(item.value, _maxConnectionCount);
            } else if (String::equals(item.key, "pingCycle")) {
                Boolean pingEnable;
                if (Boolean::parse(item.value, pingEnable) && !pingEnable) {
                    // disable the ping timer.
                } else {
                    TimeSpan::parse(item.value, _pingCycle);
                }
            }
        }
    }

    SqlConnection::~SqlConnection() {
        if (_pingTimer != nullptr) {
            delete _pingTimer;
            _pingTimer = nullptr;
        }

        close();
    }

    bool SqlConnection::open(const String &connectionStr) {
        Url url;
        Port port;
        String scheme, host, dbname, user;
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
                           String::equals(key, "ds", true) ||
                           String::equals(key, "scheme", true)) {
                    scheme = value;
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
                }
            }
        }

        if (url.isEmpty()) {
            if (scheme.isNullOrEmpty()) {
                Trace::error("Can not find the database scheme!");
                return false;
            }
            url = Url(scheme, Endpoint(host, port), dbname);
            connections.add("url", url);
        } else {
            connections.add("host", url.address());
            connections.add("port", url.port());
            connections.add("dbname", url.relativeUrl());
        }

        if (url.isEmpty()) {
            Trace::error("Can not find the database url!");
            return false;
        }
        if (user.isNullOrEmpty()) {
            Trace::error("Can not find the database user!");
            return false;
        }

        auto client = DbClientFactory::create(url.scheme());
        if (client != nullptr) {
            if (client->open(connections)) {
                _connections = connections;

                if (_pingTimer == nullptr) {
                    if (hasPing()) {
                        _pingTimer = new Timer("sql.ping.timer", _pingCycle, _pingCycle,
                                               &SqlConnection::timerProc, this);
                    }
                }

                Locker locker(&_clients);
                _clients.add(client);
                return true;
            } else {
                delete client;
            }
        }
        return false;
    }

    bool SqlConnection::open(const Url &url, const String &user, const String &password) {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", user.c_str());
        connectionStr.appendFormat(fmt, "password", password.c_str());
        return open(connectionStr);
    }

    bool SqlConnection::isConnected() {
        DbClient *client = getClient();
        if (client != nullptr) {
            return client->isConnected();
        }
        return false;
    }

    bool SqlConnection::close() {
        Locker locker(&_clients);
        for (size_t i = 0; i < _clients.count(); ++i) {
            _clients[i]->close();
        }
        _clients.clear();
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

    int SqlConnection::maxConnectionCount() const {
        return _maxConnectionCount;
    }

    const TimeSpan &SqlConnection::pingCycle() const {
        return _pingCycle;
    }

    bool SqlConnection::hasPing() const {
        return _pingCycle != TimeSpan::Zero;
    }

    DbClient *SqlConnection::getClient() {
        Locker locker(&_clients);
        if (_clients.count() > 0) {
            return _clients[0];
        }
        return nullptr;
    }

    void SqlConnection::timerProc() {
        if (_clients.tryLock()) {
            for (size_t i = 0; i < _clients.count(); ++i) {
                if (!_clients[i]->ping()) {
                    _clients[i]->close();
                    _clients[i]->open(_connections);
                }
            }
            _clients.unlock();
        }
    }
}
