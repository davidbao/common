//
//  DbClient.cpp
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright © 2017 com. All rights reserved.
//

#include "database/DbClient.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "system/Application.h"

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

#ifdef DEBUG

    void DbClient::createSqlFile(const String &fileName, const String &sql) {
        String sqlPath = Path::combine(Application::instance()->rootPath(), "logsSql");
        if (!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

#endif
}
