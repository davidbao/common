//
//  Dm7Client.h
//  common
//
// for V7 Dameng.
//  Created by baowei on 2023/9/12.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef Dm7Client_h
#define Dm7Client_h

#include "thread/Mutex.h"
#include "database/DbClient.h"
#include "net/NetType.h"

using namespace Net;

namespace Database {
    class Dm7Inner;

    class HsmtInner;

    class Dm7Client : public DbClient {
    public:
        using DbClient::open;
        using DbClient::executeSql;
        using DbClient::executeSqlInsert;
        using DbClient::executeSqlReplace;

        Dm7Client();

        ~Dm7Client() override;

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

        bool reopen();

        int executeInner(const String &sql, HsmtInner &hsmt);

        int executeSqlInner(const String &sql);

        int executeSqlInsertInner(const DataTable &table, bool replace = false);

        int executeSqlQueryInner(const String &sql, DataTable &table);

        void updateDataTable(void *tag, DataTable &table);

        int beginTransactionInner();

        int commitTransactionInner();

        int rollbackTransactionInner();

        void printErrorInfo(const String &methodName, const String &sql = String::Empty,
                            const HsmtInner *hsmt = nullptr);

        bool isConnectedInner();

    private:
        static bool isSucceed(int result);

        static void getErrorInfo(signed short hndl_type, void *hndl, unsigned int &errorCode, String &errorMessage);

        static String toInsertStr(const DataTable &table, const DataRow &row);

        static String toMergeStr(const DataTable &table, const DataRow &row);

    private:
        Dm7Inner *_dm7Db;

        StringMap _connectionParams;
    };
}

#endif // Dm7Client_h
