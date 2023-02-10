//
//  SqliteClient.h
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef SqliteClient_h
#define SqliteClient_h

#include "database/DbClient.h"

namespace Database {
    class SqliteInner;

    class SqliteClient : public DbClient {
    public:
        using DbClient::executeSql;
        using DbClient::executeSqlInsert;
        using DbClient::executeSqlReplace;

        SqliteClient();

        ~SqliteClient() override;

        bool open(const String &connectionStr) override;

        bool close() override;

        bool executeSql(const String &sql, bool transaction) override;

        bool executeSqlQuery(const String &sql, DataTable &table) override;

        bool executeSqlInsert(const DataTable &table, bool transaction) override;

        bool executeSqlReplace(const DataTable &table, bool transaction) override;

        String getErrorMsg() override;

        bool beginTransaction() override;

        bool commitTransaction() override;

        bool rollbackTransaction() override;

    protected:
        ValueTypes getColumnType(int type) override;

        static ValueTypes getColumnType(const String &type);

    private:
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
