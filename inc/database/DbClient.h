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
#include "DataTable.h"

using namespace Data;

namespace Database {
    class DbClient {
    public:
        DbClient();

        virtual ~DbClient();

        virtual bool open(const String &connectionStr) = 0;

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
        bool executeSql(const String &sql);

        bool executeSqlInsert(const DataTable &table);

        bool executeSqlReplace(const DataTable &table);

        bool retrieveCount(const String &sql, int &count);

    public:
        static uint64_t generateSnowFlakeId(int dataCenterId, int workerId);

        static uint64_t generateSnowFlakeId(int workerId);

        static uint64_t generateSnowFlakeId();

    protected:
        virtual DbType getColumnType(int type) = 0;

    protected:
        void printErrorInfo(const String &methodName, const String &sql = String::Empty,
                            const String &error = String::Empty);

#ifdef DEBUG

        static void createSqlFile(const String &fileName, const String &sql);

#endif

    protected:
        Mutex _dbMutex;
    };
}

#endif // DbClient_h
