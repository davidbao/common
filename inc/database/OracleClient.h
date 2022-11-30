//
//  OracleClient.h
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#ifndef OracleClient_h
#define OracleClient_h

#include "DbClient.h"

namespace Database {
    class OracleInner;

    class OracleClient : public DbClient {
    public:
        OracleClient();

        ~OracleClient() override;

        bool open(const String &database, const String &username, const String &password);

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

        bool abortExecuting();

    protected:
        ValueTypes getColumnType(int type) override;

    private:
        uint32_t executeSqlInner(const String &sql);

        uint32_t executeSqlQueryInner(const String &sql, DataTable &table);

        uint32_t executeSqlInsertInner(const DataTable &table);

        uint32_t executeSqlMergeInner(const DataTable &table);

        uint32_t beginTransactionInner();

        uint32_t commitTransactionInner();

        uint32_t rollbackTransactionInner();

        String toInsertStr(const DataTable &table);

        String toInsertStr(const DataTable &table, DataRow *row);

        String toMergeStr(const DataTable &table, DataRow *row);

        bool isSucceed(uint32_t result) const;

        void printErrorInfo(const String &methodName, uint32_t error, const String &sql = String::Empty);

        String getErrorMsg(uint32_t error);

    private:
        OracleInner *_oracleDb;
    };
}

#endif // OracleClient_h
