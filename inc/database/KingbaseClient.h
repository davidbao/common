//
//  KingbaseClient.h
//  common
//
// for V7 kingbase.
//  Created by baowei on 2022/12/30.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef KingbaseClient_h
#define KingbaseClient_h

#include "thread/Mutex.h"
#include "database/DbClient.h"
#include "net/NetType.h"

using namespace Net;

namespace Database {
    class KingbaseInner;
    class ResultInner;

    class KingbaseClient : public DbClient {
    public:
        using DbClient::open;
        using DbClient::executeSql;
        using DbClient::executeSqlInsert;
        using DbClient::executeSqlReplace;

        KingbaseClient();

        ~KingbaseClient() override;

        bool open(const StringMap &connections) override;

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

    protected:
        DbType getColumnType(int type) override;

        bool ping() override;

    private:
        bool openInner(const StringMap &connections);

        bool reopen();

        int executeInner(const String &sql, ResultInner &result);

        int executeSqlInner(const String &sql);

        int executeSqlInsertInner(const DataTable &table, bool replace = false);

        int executeSqlQueryInner(const String &sql, DataTable &table);

        void updateDataTable(void *tag, DataTable &table);

        int beginTransactionInner();

        int commitTransactionInner();

        int rollbackTransactionInner();

        void printErrorInfo(const String &methodName, const String &sql = String::Empty,
                            const ResultInner *result = nullptr);

        bool isConnected() const;

        static bool isSucceed(int result);

        static String toInsertStr(const DataTable &table, const DataRow &row);

        static String toMergeStr(const DataTable &table, const DataRow &row);

    private:
        KingbaseInner *_kingbaseDb;

        StringMap _connectionParams;
    };
}

#endif // KingbaseClient_h
