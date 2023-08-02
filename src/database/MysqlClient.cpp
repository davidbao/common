//
//  MysqlClient.cpp
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "data/Convert.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "system/Application.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "database/MysqlClient.h"
#include "mysql/mysql.h"

using namespace System;

namespace Database {
    class MysqlInner {
    public:
        MYSQL *mysqlDb;

        static const int MYSQL_OK = 0;

        MysqlInner() : mysqlDb(nullptr) {
        }
    };

    MysqlClient::MysqlClient() {
        _mysqlDb = new MysqlInner();
        _mysqlDb->mysqlDb = mysql_init(nullptr);
    }

    MysqlClient::~MysqlClient() {
        if (_mysqlDb->mysqlDb != nullptr) {
            MysqlClient::close();
        }
        delete _mysqlDb;
        _mysqlDb = nullptr;
    }

    bool MysqlClient::open(const StringMap &connections) {
        Locker locker(&_dbMutex);

        return openInner(connections);
    }

    bool MysqlClient::openInner(const StringMap &connections) {
        String host = connections["host"];
        Port port;
        Port::parse(connections["port"], port);
        String dbname = connections["dbname"];
        String user = connections["user"];
        String password = connections["password"];
        int timeout = 60;
        Int32::parse(connections["timeout"], timeout);

        mysql_options(_mysqlDb->mysqlDb, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        if (mysql_real_connect(_mysqlDb->mysqlDb, host.c_str(), user.c_str(),
                               password.c_str(), dbname.c_str(), port, nullptr, CLIENT_MULTI_STATEMENTS)) {
            int result = mysql_query(_mysqlDb->mysqlDb, "set names 'UTF8'");
            if (!isSucceed(result)) {
                printErrorInfo("mysql_query(_mysqlDb->mysqlDb, \"set names 'UTF8'\"");
                return false;
            }

            char value = 1;
            mysql_options(_mysqlDb->mysqlDb, MYSQL_OPT_RECONNECT, (char *) &value);

            Trace::debug(String::format("Open mysql successfully. host: %s, port: %d, dbname: %s, user name: %s",
                                        host.c_str(), (int) port, dbname.c_str(), user.c_str()));

            DataTable table;
            if (executeSqlQueryInner(" select version();", table)) {
                if (table.rows().count() > 0) {
                    const DataCells &cells = table.rows().at(0).cells();
                    if (cells.count() > 0) {
                        Version mysqlVersion;
                        if (!cells.at(0).isNullValue() &&
                            Version::parse(cells.at(0).value(), mysqlVersion)) {
                            if (mysqlVersion >= "8.0") {
                                executeSql(
                                        "set @@sql_mode='STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';");
                            } else if (mysqlVersion >= "5.7.2") {
                                // https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html#sqlmode_only_full_group_by
                                // Reject queries for which the select list, HAVING condition, or ORDER BY list refer to nonaggregated columns that are neither named in the GROUP BY clause nor are functionally dependent on (uniquely determined by) GROUP BY columns.
                                // As of MySQL 5.7.5, the default SQL mode includes ONLY_FULL_GROUP_BY. (Before 5.7.5, MySQL does not detect functional dependency andONLY_FULL_GROUP_BY is not enabled by default. For a description of pre-5.7.5 behavior, see the MySQL 5.6 Reference Manual.)
                                executeSqlInner(
                                        "set @@sql_mode='STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION';");
                            }
                        }
                    }
                }
            }

            return true;
        } else {
            printErrorInfo("mysql_real_connect");

            Trace::debug(String::format(
                    "Failed to open mysql. host: %s, port: %d, dbname: %s, user name: %s, reason: '%s'",
                    host.c_str(), (int) port, dbname.c_str(), user.c_str(),
                    getErrorMsg().c_str()));
        }
        return false;
    }

    bool MysqlClient::isConnected() {
        Locker locker(&_dbMutex);
        int result = mysql_ping(_mysqlDb->mysqlDb);
        return result == 0;
    }

    bool MysqlClient::close() {
        Locker locker(&_dbMutex);

        mysql_close(_mysqlDb->mysqlDb);
        _mysqlDb->mysqlDb = nullptr;
        return true;
    }

    bool MysqlClient::executeSql(const String &sql, bool transaction) {
        Locker locker(&_dbMutex);

        int result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlInner(sql);
        if (!isSucceed(result)) {
            if (transaction) {
                rollbackTransactionInner();
            }
            return false;
        }
        if (transaction) {
            result = commitTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        return true;
    }

    bool MysqlClient::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool MysqlClient::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool MysqlClient::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    bool MysqlClient::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        int result = executeSqlQueryInner(sql, table);
        return isSucceed(result);
    }

    bool MysqlClient::executeSqlInsert(const DataTable &table, bool transaction) {
        Locker locker(&_dbMutex);

        int result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlInsertInner(table, false);
        if (!isSucceed(result)) {
            if (transaction) {
                rollbackTransactionInner();
            }
            return false;
        }
        if (transaction) {
            result = commitTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        return true;
    }

    bool MysqlClient::executeSqlReplace(const DataTable &table, bool transaction) {
        Locker locker(&_dbMutex);

        int result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlInsertInner(table, true);
        if (!isSucceed(result)) {
            if (transaction) {
                rollbackTransactionInner();
            }
            return false;
        }
        if (transaction) {
            result = commitTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        return true;
    }

    int MysqlClient::executeSqlInner(const String &sql) {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("MysqlClient::executeSqlInner, sql: '%s'", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif

        mysql_ping(_mysqlDb->mysqlDb);
        int result = mysql_query(_mysqlDb->mysqlDb, sql.c_str());
        if (!isSucceed(result)) {
            printErrorInfo("mysql_query", sql);
            return result;
        }

        int status;
        /* process each statement result */
        do {
            /* did current statement return data? */
            MYSQL_RES *mysqlRes = mysql_store_result(_mysqlDb->mysqlDb);
            if (mysqlRes) {
                /* yes; process rows and free the result set */
                mysql_free_result(mysqlRes);
            } else          /* no result set or error */
            {
                if (mysql_field_count(_mysqlDb->mysqlDb) == 0) {
//                    printf("%lld rows affected\n",
//                           mysql_affected_rows(_mysqlDb->mysqlDb));
                } else  /* some error occurred */
                {
//                    printf("Could not retrieve result set\n");
                    break;
                }
            }
            /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
            if ((status = mysql_next_result(_mysqlDb->mysqlDb)) > 0) {
//                printf("Could not execute statement\n");
            }
        } while (status == 0);

        return 0;
    }

    int MysqlClient::executeSqlInsertInner(const DataTable &table, bool replace) {
#if DEBUG
        String info = String::convert("MysqlClient::executeSqlInsertInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif
        if (table.name().isNullOrEmpty())
            return -1;
        if (table.columnCount() == 0)
            return -2;
        if (table.rowCount() == 0)
            return -3;

        // such like '"INSERT INTO users(name, age) VALUES (.....),(.....),(.....),(.....)";'
        String columsStr, valuesStr;
        for (size_t j = 0; j < table.columnCount(); j++) {
            if (!columsStr.isNullOrEmpty()) {
                columsStr.append(", ");
            }
            columsStr.append(table.columns().at(j).name());
        }

        const DataRows &rows = table.rows();
        for (size_t i = 0; i < rows.count(); i++) {
            const DataRow &row = rows.at(i);

            if (!valuesStr.isNullOrEmpty()) {
                valuesStr.append(", ");
            }
            valuesStr.append('(');
            for (size_t j = 0; j < row.cells().count(); j++) {
                const DataCell &cell = row.cells().at(j);

                if (j != 0) {
                    valuesStr.append(",");
                }
                valuesStr.append(cell.valueStr(true));
            }
            valuesStr.append(')');
        }

        String sql;
        if (replace) {
            sql = String::convert("REPLACE INTO %s(%s) VALUES %s", table.name().c_str(), columsStr.c_str(),
                                  valuesStr.c_str());
        } else {
            sql = String::convert("INSERT IGNORE INTO %s(%s) VALUES %s", table.name().c_str(), columsStr.c_str(),
                                  valuesStr.c_str());
        }

#ifdef DEBUG
        createSqlFile("mysql_insert.sql", sql);
#endif

        return executeSqlInner(sql);
    }

    int MysqlClient::executeSqlQueryInner(const String &sql, DataTable &table) {
#if DEBUG
        String info = String::convert("MysqlClient::executeSqlQueryInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        mysql_ping(_mysqlDb->mysqlDb);
        int result = mysql_query(_mysqlDb->mysqlDb, sql.c_str());
        if (!isSucceed(result)) {
            printErrorInfo("mysql_query", sql);
            return result;
        }

        MYSQL_RES *result_set = mysql_store_result(_mysqlDb->mysqlDb);
        if (result_set == nullptr)
            return -1;

        updateDataTable(result_set, table);

        mysql_free_result(result_set);

        return MysqlInner::MYSQL_OK;
    }

    void MysqlClient::updateDataTable(void *tag, DataTable &table) {
        auto *result = (MYSQL_RES *) tag;
#if DEBUG
        String info = String::convert("MysqlClient::updateDataTable, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        if (table.name().isNullOrEmpty()) {
            const char *name = result->field_count > 0 ? result->fields[0].table : "temp";
            table.setName(name);
        }

#if DEBUG
        sw.setInfo(String::convert("MysqlClient::updateDataTable, the table name is '%s'", table.name().c_str()));
#endif

        int columnCount = (int) mysql_num_fields(result);
        int table_columnCount = (int) table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0) {
            if (table_columnCount == 0) {
                for (int i = 0; i < columnCount; i++) {
                    MYSQL_FIELD *fd = mysql_fetch_field(result);
                    char *nameStr = fd->name;
                    String name;
                    if (nameStr != nullptr) {
                        name = nameStr;
                    } else {
                        char temp[32];
                        sprintf(temp, "tempCol%d", i);
                        name = temp;
                    }

                    table.addColumn(DataColumn(name, getColumnType(fd->type)));
                }
            }

            MYSQL_ROW sql_row;
            while ((sql_row = mysql_fetch_row(result))) {
                DataRow row;
                for (int i = 0; i < columnCount; i++) {
                    const DataColumn &column = table.columns().at(i);
                    DbType type = column.type();
                    const char *str = (const char *) sql_row[i];
                    row.addCell(DataCell(column, DbValue(type, str)));
                }
                table.addRow(row);
            }
        }
    }

    int MysqlClient::beginTransactionInner() {
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "START TRANSACTION");     // mysql_autocommit(_mysqlDb->mysqlDb, 0);
    }

    int MysqlClient::commitTransactionInner() {
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "COMMIT");                // mysql_commit(_mysqlDb->mysqlDb);
    }

    int MysqlClient::rollbackTransactionInner() {
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "ROLLBACK");              // mysql_rollback(_mysqlDb->mysqlDb);
    }

    StringArray MysqlClient::getColumnName(const String &tableName) {
        StringArray result;
        if (!tableName.isNullOrEmpty()) {
            String sql = String::format("SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME='%s'",
                                        tableName.c_str());
            DataTable table("columns");
            if (executeSqlQuery(sql, table) && table.rowCount() > 0) {
                for (size_t i = 0; i < table.rowCount(); ++i) {
                    result.add(table.rows()[i][0].valueStr());
                }
            }
        }
        return result;
    }

    DbType MysqlClient::getColumnType(int type) {
        switch (type) {
            case MYSQL_TYPE_VARCHAR:
            case MYSQL_TYPE_VAR_STRING:
            case MYSQL_TYPE_STRING:
            case MYSQL_TYPE_BLOB:
            case MYSQL_TYPE_JSON:
                return DbType::Text;
            case MYSQL_TYPE_TINY:
            case MYSQL_TYPE_SHORT:
            case MYSQL_TYPE_LONG:
            case MYSQL_TYPE_INT24:
                return DbType::Integer32;
            case MYSQL_TYPE_LONGLONG:
                return DbType::Integer64;
            case MYSQL_TYPE_FLOAT:
                return DbType::Float32;
            case MYSQL_TYPE_DOUBLE:
                return DbType::Float64;
            case MYSQL_TYPE_DECIMAL:
                return DbType::Decimal;
            case MYSQL_TYPE_NULL:
                return DbType::Null;
            case MYSQL_TYPE_DATE:
                return DbType::Date;
            case MYSQL_TYPE_TIME:
            case MYSQL_TYPE_TIME2:
                return DbType::Time;
            case MYSQL_TYPE_DATETIME:
            case MYSQL_TYPE_DATETIME2:
            case MYSQL_TYPE_TIMESTAMP:
            case MYSQL_TYPE_TIMESTAMP2:
                return DbType::Timestamp;
            case MYSQL_TYPE_BIT:
                return DbType::Digital;
            case MYSQL_TYPE_NEWDECIMAL:
                return DbType::Integer64;
            case MYSQL_TYPE_TINY_BLOB:
            case MYSQL_TYPE_MEDIUM_BLOB:
            case MYSQL_TYPE_LONG_BLOB:
                return DbType::Blob;
            default:
                return DbType::Null;
        }
    }

    bool MysqlClient::ping() {
        if (_dbMutex.tryLock()) {
            mysql_ping(_mysqlDb->mysqlDb);
            _dbMutex.unlock();
        }
        return true;
    }

    String MysqlClient::getErrorMsg() {
        return _mysqlDb->mysqlDb != nullptr ? (String) mysql_error(_mysqlDb->mysqlDb) : String::Empty;
    }

    bool MysqlClient::isSucceed(int result) {
        return result == MysqlInner::MYSQL_OK;
    }

    void MysqlClient::printErrorInfo(const String &methodName, const String &sql) {
#ifdef DEBUG
        String path = Path::combine(Application::instance()->rootPath(), "errorSql");
        if (!Directory::exists(path))
            Directory::createDirectory(path);
        String fileName = Path::combine(path, String::convert("%s.sql", Uuid::generate().toString().c_str()));
        FileStream fs(fileName.c_str(), FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(sql);
        fs.close();
#endif
        DbClient::printErrorInfo(methodName, sql, getErrorMsg());
    }
}
