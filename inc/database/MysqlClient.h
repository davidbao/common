//
//  MysqlClient.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef MysqlClient_h
#define MysqlClient_h

#include "thread/Mutex.h"
#include "database/DbClient.h"
#include "net/NetType.h"

using namespace Net;

namespace Database {
    class MysqlInner;

    class MysqlClient : public DbClient {
    public:
        MysqlClient();

        ~MysqlClient() override;

        bool open(const Url &url, const String &username, const String &password);

        bool open(const String &host, int port, const String &database, const String &username, const String &password);

        bool open(const String &connectionStr) override;

        bool close() override;

        bool executeSql(const String &sql, bool transaction = true) override;

        bool executeSqlQuery(const String &sql, DataTable &table) override;

        bool executeSqlInsert(const DataTable &table, bool transaction = true) override;

        bool executeSqlReplace(const DataTable &table, bool transaction = true) override;

        String getErrorMsg() override;

        bool beginTransaction() override;

        bool commitTransaction() override;

        bool rollbackTransaction() override;

    protected:
        ValueTypes getColumnType(int type) override;

    private:
        int executeSqlInner(const String &sql);

        int executeSqlInsertInner(const DataTable &table, bool replace = false);

        int executeSqlQueryInner(const String &sql, DataTable &table);

        void updateDataTable(void *tag, DataTable &table);

        int beginTransactionInner();

        int commitTransactionInner();

        int rollbackTransactionInner();

        void printErrorInfo(const String &methodName, const String &sql = String::Empty);

        static bool isSucceed(int result);

    private:
        MysqlInner *_mysqlDb;
    };
}

#endif // MysqlClient_h
