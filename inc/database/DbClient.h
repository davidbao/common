//
//  DbClient.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef DbClient_h
#define DbClient_h

#include "thread/Mutex.h"
#include "data/String.h"
#include "data/StringMap.h"
#include "database/DataTable.h"
#include "net/NetType.h"

using namespace Data;
using namespace Net;

namespace Database {
    class SqlConnection;

    class DbClient {
    public:
        DbClient();

        virtual ~DbClient();

        virtual bool open(const StringMap &connections) = 0;

        virtual bool close() = 0;

        virtual bool executeSql(const String &sql, bool transaction) = 0;

        virtual bool executeSqlQuery(const String &sql, DataTable &table) = 0;

        virtual bool executeSqlInsert(const DataTable &table, bool transaction) = 0;

        virtual bool executeSqlReplace(const DataTable &table, bool transaction) = 0;

        virtual String getErrorMsg() = 0;

        virtual bool beginTransaction() = 0;

        virtual bool commitTransaction() = 0;

        virtual bool rollbackTransaction() = 0;

        virtual StringArray getColumnName(const String &tableName) = 0;

    public:
        bool open(const Url &url, const String &user, const String &password);

        bool open(const String &host, int port, const String &dbname, const String &user, const String &password);

        bool open(const Endpoint &address, const String &dbname, const String &user, const String &password);

        bool executeSql(const String &sql);

        bool executeSqlInsert(const DataTable &table);

        bool executeSqlReplace(const DataTable &table);

        bool retrieveCount(const String &sql, int &count);

    protected:
        virtual DbType getColumnType(int type) = 0;

        virtual bool ping() = 0;

    protected:
        void printErrorInfo(const String &methodName, const String &sql = String::Empty,
                            const String &error = String::Empty);

#ifdef DEBUG

        static void createSqlFile(const String &fileName, const String &sql);

#endif

    protected:
        Mutex _dbMutex;

    private:
        friend SqlConnection;
    };

    typedef PList<DbClient> DbClients;

    class DbClientFactory {
    public:
        static DbClient *create(const String &scheme);
    };
}

#endif // DbClient_h
