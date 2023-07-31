//
//  DbClient.cpp
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "database/DbClient.h"
#include "diag/Trace.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#ifdef HAS_DB_MYSQL
#include "database/MysqlClient.h"
#endif
#ifdef HAS_DB_ORACLE
#include "database/OracleClient.h"
#endif
#ifdef HAS_DB_SQLITE
#include "database/SqliteClient.h"
#endif
#ifdef HAS_DB_KINGBASE
#include "database/KingbaseClient.h"
#endif
#ifdef HAS_DB_DM6
#include "database/Dm6Client.h"
#endif

#ifdef DEBUG
#include "system/Application.h"
#include "IO/Path.h"
#endif

using namespace System;
using namespace Diag;

namespace Database {
    DbClient::DbClient() {
    }

    DbClient::~DbClient() = default;

    bool DbClient::open(const Url &url, const String &user, const String &password) {
        const String &host = url.address();
        int port = url.port();
        const String &dbname = url.relativeUrl();
        return open(host, port, dbname, user, password);
    }

    bool DbClient::open(const String &host, int port, const String &dbname,
                        const String &user, const String &password) {
        StringMap connections;
        connections["host"] = host;
        connections["port"] = Int32(port).toString();
        connections["dbname"] = dbname;
        connections["user"] = user;
        connections["password"] = password;
        return open(connections);
    }

    bool DbClient::open(const Endpoint &address, const String &dbname, const String &user, const String &password) {
        return open(address.address, address.port, dbname, user, password);
    }

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
        table.addColumn(DataColumn("count", DbType::Integer32));
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
        String sqlPath = Path::combine(Application::instance()->rootPath(), "infoSql");
        if (!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

#endif

    DbClient *DbClientFactory::create(const String &scheme) {
        DbClient *client = nullptr;
        if (scheme == "mysql" || scheme == "mysqls") {
#ifdef HAS_DB_MYSQL
            client = new MysqlClient();
#endif
        } else if (scheme == "oracle" || scheme == "oracles") {
#ifdef HAS_DB_ORACLE
            client = new OracleClient();
#endif
        } else if (scheme == "sqlite") {
#ifdef HAS_DB_SQLITE
            client = new SqliteClient();
#endif
        } else if (scheme == "kingbase") {
#ifdef HAS_DB_KINGBASE
            client = new KingbaseClient();
#endif
        } else if (scheme == "dm6") {
#ifdef HAS_DB_DM6
            client = new Dm6Client();
#endif
        }
        return client;
    }
}
