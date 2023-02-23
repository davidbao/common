//
//  DbClient.cpp
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "database/DbClient.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "system/Application.h"
#include "thread/TickTimeout.h"

using namespace System;

namespace Database {
    DbClient::DbClient() = default;

    DbClient::~DbClient() = default;

    bool DbClient::executeSql(const String &sql) {
        return executeSql(sql, false);
    }

    bool DbClient::executeSqlInsert(const DataTable &table) {
        return executeSqlInsert(table, false);
    }

    bool DbClient::executeSqlReplace(const DataTable &table) {
        return executeSqlReplace(table, false);
    }

    bool DbClient::retrieveCount(const String &sql, int &count) {
        DataTable table("countTable");
        table.addColumn(DataColumn("count", ValueTypes::Integer32));
        if (executeSqlQuery(sql, table)) {
            if (table.rowCount() == 1) {
                count = table.rows().at(0).cells().at(0).value();
                return true;
            }
        }
        return false;
    }

    void DbClient::printErrorInfo(const String &methodName, const String &sql, const String &error) {
        const int maxCount = 512;
        const char *sqlStr;
        String temp;
        if (sql.length() > maxCount) {
            temp = sql.substr(0, maxCount);
            sqlStr = temp.c_str();
        } else {
            sqlStr = sql.c_str();
        }

        String errorStr = error.isNullOrEmpty() ? getErrorMsg() : error;
        if (sql.length() > 0) {
            Trace::writeFormatLine("Method'%s' error, msg: %s, sql: %s", methodName.c_str(), errorStr.c_str(), sqlStr);
        } else {
            Trace::writeFormatLine("Method'%s' error, msg: %s", methodName.c_str(), errorStr.c_str());
        }
    }

    // Twitter snowflake
    /*-------high----------------------------------------64 bits-------------------------------------------low-------|
    |----------------------------------------------------------------------------------------------------------------|
    |   0  | 0000000000 0000000000 0000000000 0000000000 0 |         00000       |       00000      |   000000000000 |
    |unused|             41 bits timestamp                 |5 bits data center ID| 5 bits worker ID | 12 bits seq ID |
    |----------------------------------------------------------------------------------------------------------------|
    |unused|             41 bits timestamp                 |5 bits data center ID| 5 bits worker ID | 12 bits seq ID |
    |---------------------------------------------------------------------------------------------------------------*/
    // https://www.cnblogs.com/Keeping-Fit/p/15025402.html
    uint64_t DbClient::generateSnowFlakeId(int dataCenterId, int workerId) {
        constexpr int SEQUENCE_BITS = 12;
        constexpr int WORKER_ID_BITS = 5;
        constexpr int DATA_CENTER_ID_BITS = 5;

        constexpr int SEQUENCE_ID_SHIFT = 0;
        constexpr int WORK_ID_SHIFT = SEQUENCE_BITS;
        constexpr int DATA_CENTER_ID_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS;
        constexpr int TIMESTAMP_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS + DATA_CENTER_ID_BITS;

        constexpr uint64_t MAX_WORKER_ID = (1 << WORKER_ID_BITS) - 1;
        constexpr uint64_t MAX_DATA_CENTER_ID = (1 << DATA_CENTER_ID_BITS) - 1;

        constexpr uint64_t SEQUENCE_MASK = (1 << SEQUENCE_BITS) - 1;

        if (dataCenterId < 0 || MAX_DATA_CENTER_ID < dataCenterId) {
            return 0;
        }
        if (workerId < 0 || MAX_WORKER_ID < workerId) {
            return 0;
        }

        static Mutex mutex;
        Locker locker(&mutex);

        static uint32_t startTick = TickTimeout::getCurrentTickCount();
        static uint64_t startMilliseconds = DateTime::total2010Milliseconds(DateTime::now());
        static uint64_t lastTimestamp = 0;
        static uint64_t sequenceId = 0;

        uint32_t currentTick = TickTimeout::getCurrentTickCount();
        if (currentTick <= startTick) {
            startTick = TickTimeout::getCurrentTickCount();
            startMilliseconds = DateTime::total2010Milliseconds(DateTime::now());
            currentTick = startTick;
        }

        uint64_t timestamp = startMilliseconds + (currentTick - startTick);
        if (timestamp == lastTimestamp) {
            sequenceId = (sequenceId + 1) & SEQUENCE_MASK;

            if (0 == sequenceId) {
                Thread::msleep(1);
                timestamp++;
            }
        } else {
            sequenceId = 0;
        }
        lastTimestamp = timestamp;
        return (timestamp << TIMESTAMP_SHIFT)
               | (dataCenterId << DATA_CENTER_ID_SHIFT)
               | (workerId << WORK_ID_SHIFT)
               | (sequenceId << SEQUENCE_ID_SHIFT);
    }

    uint64_t DbClient::generateSnowFlakeId(int workerId) {
        return generateSnowFlakeId(0, workerId);
    }
    uint64_t DbClient::generateSnowFlakeId() {
        return generateSnowFlakeId(0, 0);
    }

#ifdef DEBUG

    void DbClient::createSqlFile(const String &fileName, const String &sql) {
        String sqlPath = Path::combine(Application::instance()->rootPath(), "infoSql");
        if (!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

#endif
}
