//
//  SqlConnection.h
//  common
//
//  Created by baowei on 2023/7/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef SqlConnection_h
#define SqlConnection_h

#include "data/String.h"
#include "database/DbClient.h"
#include "thread/Timer.h"

using namespace Data;
using namespace Threading;

namespace Database {
    class SqlConnection {
    public:
        SqlConnection();

        explicit SqlConnection(int maxConnectionCount);

        explicit SqlConnection(const TimeSpan &pingCycle);

        SqlConnection(int maxConnectionCount, const TimeSpan &pingCycle);

        SqlConnection(std::initializer_list<KeyValuePair<String, String>> list);

        ~SqlConnection();

        bool open(const String &connectionStr);

        bool open(const Url &url, const String &user, const String &password);

        bool close();

        bool executeSql(const String &sql, bool transaction = true);

        bool executeSqlQuery(const String &sql, DataTable &table);

        bool executeSqlInsert(const DataTable &table, bool transaction = true);

        bool executeSqlReplace(const DataTable &table, bool transaction = true);

        StringArray getColumnName(const String &tableName);

        bool retrieveCount(const String &sql, int &count);

        bool beginTransaction();

        bool commitTransaction();

        bool rollbackTransaction();

        int maxConnectionCount() const;

        const TimeSpan &pingCycle() const;

        bool hasPing() const;

    private:
        DbClient *getClient();

        void timerProc();

    private:
        DbClients _clients;

        StringMap _connections;

        int _maxConnectionCount;
        TimeSpan _pingCycle;

        Timer* _pingTimer;
    };
}

#endif // SqlConnection_h
