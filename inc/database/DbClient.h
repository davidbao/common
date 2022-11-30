//
//  DbClient.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef DbClient_h
#define DbClient_h

#include "thread/Mutex.h"
#include "data/ValueType.h"
#include "DataTable.h"

using namespace Common;

namespace Database {
    class DbClient {
    public:
        DbClient();

        virtual ~DbClient();

        virtual bool open(const String &connectionStr) = 0;

        virtual bool close() = 0;

        virtual bool executeSql(const String &sql, bool transaction = true) = 0;

        virtual bool executeSqlQuery(const String &sql, DataTable &table) = 0;

        virtual bool executeSqlInsert(const DataTable &table, bool transaction = true) = 0;

        virtual bool executeSqlReplace(const DataTable &table, bool transaction = true) = 0;

        virtual String getErrorMsg() = 0;

        virtual bool beginTransaction() = 0;

        virtual bool commitTransaction() = 0;

        virtual bool rollbackTransaction() = 0;

        bool retrieveCount(const String &sql, int &count);

    protected:
        virtual ValueTypes getColumnType(int type) = 0;

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
