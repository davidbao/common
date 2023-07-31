//
//  MysqlClient.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef MysqlClient_h
#define MysqlClient_h

#include "thread/Mutex.h"
#include "database/DbClient.h"

namespace Database {
    class MysqlInner;

    class MysqlClient : public DbClient {
    public:
        using DbClient::open;
        using DbClient::executeSql;
        using DbClient::executeSqlInsert;
        using DbClient::executeSqlReplace;

        MysqlClient();

        ~MysqlClient() override;

        bool open(const StringMap &connections) override;

        bool close() override;

        bool isConnected() override;

        bool executeSql(const String &sql, bool transaction) override;

        bool executeSqlQuery(const String &sql, DataTable &table) override;

        bool executeSqlInsert(const DataTable &table, bool transaction) override;

        bool executeSqlReplace(const DataTable &table, bool transaction) override;

        String getErrorMsg() override;

        bool beginTransaction() override;

        bool commitTransaction() override;

        bool rollbackTransaction() override;

        StringArray getColumnName(const String &tableName) override;

    protected:
        DbType getColumnType(int type) override;

        bool ping() override;

    private:
        bool openInner(const StringMap &connections);

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
