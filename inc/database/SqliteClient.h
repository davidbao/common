//
//  SqliteClient.h
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef SqliteClient_h
#define SqliteClient_h

#include "database/DbClient.h"

namespace Database {
    class SqliteInner;

    class SqliteClient : public DbClient {
    public:
        using DbClient::open;
        using DbClient::executeSql;
        using DbClient::executeSqlInsert;
        using DbClient::executeSqlReplace;

        SqliteClient();

        ~SqliteClient() override;

        bool open(const StringMap &connections) override;

        bool isConnected() override;

        bool close() override;

        bool executeSql(const String &sql, bool transaction) override;

        bool executeSqlQuery(const String &sql, DataTable &table) override;

        bool executeSqlInsert(const DataTable &table, bool transaction) override;

        bool executeSqlReplace(const DataTable &table, bool transaction) override;

        String getErrorMsg() override;

        bool beginTransaction() override;

        bool commitTransaction() override;

        bool rollbackTransaction() override;

        StringArray getColumnName(const String &tableName) override;

    public:
        bool open(const String &fileName);

    protected:
        DbType getColumnType(int type) override;

        bool ping() override;

        static DbType getColumnType(const String &type);

    private:
        bool openInner(const StringMap &connections);

        bool openInner(const String &fileName);

        int executeSqlInner(const String &sql);

        int executeSqlInsertInner(const DataTable &table, bool replace = false);

        int executeSqlQueryInner(const String &sql, DataTable &table);

        int beginTransactionInner();

        int commitTransactionInner();

        int rollbackTransactionInner();

        static bool isSucceed(int result);

    private:
        SqliteInner *_sqliteDb;
    };
}
#endif // SqliteClient_h
