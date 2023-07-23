//
//  Dm6Client.cpp
//  common
//
//  Created by baowei on 2023/7/6.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/Dm6Client.h"
#include "data/Convert.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "system/Application.h"
#include "IO/FileStream.h"

#ifdef DEBUG

#include "IO/Directory.h"

#endif

#include "IO/Path.h"
#include "dm6/api.h"

using namespace System;

namespace Database {
    class Dm6Inner {
    public:
        dm_henv henv;
        dm_hdbc hdbc;

        Dm6Inner() : henv(nullptr), hdbc(nullptr) {
        }

        ~Dm6Inner() {
            close();
        }

        void open() {
            if (isOpened()) {
                close();
            }

            dm_alloc_env(&henv);
            dm_alloc_connect(henv, &hdbc);
        }

        void close() {
            if (hdbc != nullptr) {
                dm_free_connect(hdbc);
                hdbc = nullptr;
            }
            if (henv != nullptr) {
                dm_free_env(henv);
                henv = nullptr;
            }
        }

        bool isOpened() const {
            return henv != nullptr && henv != nullptr;
        }
    };

    class HsmtInner {
    public:
        dm_hstmt hsmt;

        explicit HsmtInner(dm_hstmt hsmt = nullptr) : hsmt(hsmt) {
        }
    };

    Dm6Client::Dm6Client() {
        dm_api_init();
        _dm6Db = new Dm6Inner();
    }

    Dm6Client::~Dm6Client() {
        if (_dm6Db->isOpened()) {
            Dm6Client::close();
        }
        delete _dm6Db;
        _dm6Db = nullptr;
    }

    bool Dm6Client::open(const StringMap &connections) {
        Locker locker(&_dbMutex);

        return openInner(connections);
    }

    bool Dm6Client::openInner(const StringMap &connections) {
        _dm6Db->open();
        if (_dm6Db->isOpened()) {
            String host = connections["host"];
            String port = connections["port"];
            String dbname = connections["dbname"];
            String user = connections["user"];
            int p = 0;
            Int32::parse(port, p);
            dm_bool result = dm_login_port(_dm6Db->hdbc,
                                           (dm_char *) host.c_str(),
                                           (dm_char *) user.c_str(),
                                           (dm_char *) connections["password"].c_str(),
                                           p);
            if (result != 0) {
                if (!dbname.isNullOrEmpty()) {
                    String sql = String::format("SET CURRENT DATABASE %s;", dbname.c_str());
                    executeSqlInner(sql);
                }

                Trace::debug(String::format("Open dm6 successfully. host: %s, port: %s, dbname: %s, user name: %s",
                                            host.c_str(), port.c_str(), dbname.c_str(), user.c_str()));
                return true;
            } else {
                long errorCode = dm_con_get_errorcode(_dm6Db->hdbc);
                Trace::debug(String::format(
                        "Failed to open dm6. host: %s, port: %s, dbname: %s, user name: %s, error code: %d",
                        host.c_str(), port.c_str(), dbname.c_str(), user.c_str(), errorCode));
                _dm6Db->close();
            }
        } else {
            Trace::error("Failed to open db, error: can not init env!");
        }
        return false;
    }

    bool Dm6Client::reopen() {
        Trace::info("Reopen the dm6 client!");
        if (_dm6Db->isOpened()) {
            dm_logout(_dm6Db->hdbc);
        }
        return openInner(_connectionParams);
    }

    bool Dm6Client::close() {
        Locker locker(&_dbMutex);

        if (_dm6Db->isOpened()) {
            dm_logout(_dm6Db->hdbc);
            _dm6Db->close();
            return true;
        } else {
            return false;
        }
    }

    int Dm6Client::executeInner(const String &sql, HsmtInner &hsmt) {
        dm_bool result = dm_alloc_stmt(_dm6Db->hdbc, &hsmt.hsmt);
        if (result == 0) {
//            Fixed Bug: The error is 'msg uncompress fail!', so login again.
            dm_logout(_dm6Db->hdbc);
            const StringMap &values = _connectionParams;
            int port = 0;
            Int32::parse(values["port"], port);
            dm_login_port(_dm6Db->hdbc,
                          (dm_char *) values["host"].c_str(),
                          (dm_char *) values["user"].c_str(),
                          (dm_char *) values["password"].c_str(),
                          port);
            dm_alloc_stmt(_dm6Db->hdbc, &hsmt.hsmt);
        }

        result = dm_direct_exec(hsmt.hsmt, (dm_char *) sql.c_str());
        if (result == 0) {
            if (!isConnected()) {
                dm_free_stmt(hsmt.hsmt);

                reopen();

                // re-execute again.
                dm_alloc_stmt(_dm6Db->hdbc, &hsmt.hsmt);
                result = dm_direct_exec(hsmt.hsmt, (dm_char *) sql.c_str());
                if (result == 0) {
                    dm_free_stmt(hsmt.hsmt);
                }
            } else {
                dm_lint errorCode = dm_stmt_get_errorcode(hsmt.hsmt);
                const dm_char *errorStr = dm_stmt_get_errormsg(hsmt.hsmt);
#ifdef WIN32
                String error = String::GBKtoUTF8(errorStr);
#else
                String error = String((const char *) errorStr);
#endif
                Trace::error(String::format("dm6_execute'%s' error code: %d, error: %s",
                                            sql.c_str(), errorCode, error.c_str()));
                dm_free_stmt(hsmt.hsmt);
            }
        }
        return (int) result;
    }

    bool Dm6Client::executeSql(const String &sql, bool transaction) {
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

    bool Dm6Client::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool Dm6Client::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool Dm6Client::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    bool Dm6Client::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        int result = executeSqlQueryInner(sql, table);
        return isSucceed(result);
    }

    bool Dm6Client::executeSqlInsert(const DataTable &table, bool transaction) {
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

    bool Dm6Client::executeSqlReplace(const DataTable &table, bool transaction) {
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

    int Dm6Client::executeSqlInner(const String &sql) {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("Dm6Client::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif

        HsmtInner hsmt;
        int result = executeInner(sql, hsmt);
        if (isSucceed(result)) {
            dm_free_stmt(hsmt.hsmt);
        }
        return result;
    }

    String Dm6Client::toInsertStr(const DataTable &table, const DataRow &row) {
        // such like '"INSERT INTO users(name, age) VALUES (.....),(.....),(.....),(.....)";'
        static const char *insertStr = "INSERT INTO %s(%s) VALUES (%s);";
        size_t columnCount = table.columnCount();

        String columnsStr;
        String valuesStr;
        for (size_t j = 0; j < columnCount; j++) {
            const DataCell &cell = row.cells().at(j);
            if (!cell.isNullValue()) {
                if (!columnsStr.isNullOrEmpty()) {
                    columnsStr.append(", ");
                }
                columnsStr.append(table.columns().at(j).name());

                if (!valuesStr.isNullOrEmpty()) {
                    valuesStr.append(", ");
                }
                valuesStr.append(cell.valueStr(true));
            }
        }
        String sql = String::convert(insertStr, table.name().c_str(),
                                     columnsStr.c_str(), valuesStr.c_str());
        return sql;
    }

    String Dm6Client::toMergeStr(const DataTable &table, const DataRow &row) {
        size_t columnCount = table.columnCount();
        String primaryKey = String::Empty;
        for (size_t i = 0; i < columnCount; i++) {
            if (table.columns().at(i).primaryKey()) {
                primaryKey = table.columns().at(i).name();
                break;
            }
        }
        if (primaryKey.isNullOrEmpty())
            return String::Empty;

        // such like '"INSERT INTO users(name, age) SELECT 'Hu', 86 WHERE NOT EXISTS (SELECT name FROM users WHERE name='Hu');";
        // UPDATE users SET (NAME, AGE)=('TEST', 11) WHERE NAME='TEST''
        static const char *replaceStr = "INSERT INTO %s(%s) SELECT %s WHERE NOT EXISTS (SELECT %s FROM %s WHERE %s);\n"
                                        "UPDATE %s SET (%s)=(%s) WHERE %s;\n";

        String columnsStr;
        String valuesStr;
        for (size_t j = 0; j < columnCount; j++) {
            const DataCell &cell = row.cells().at(j);
            if (!cell.isNullValue()) {
                if (!columnsStr.isNullOrEmpty()) {
                    columnsStr.append(", ");
                }
                columnsStr.append(table.columns().at(j).name());

                if (!valuesStr.isNullOrEmpty()) {
                    valuesStr.append(", ");
                }
                valuesStr.append(cell.valueStr(true));
            }
        }
        String sql;
        for (size_t i = 0; i < table.rowCount(); i++) {
            const DataRow &r = table.rows()[i];
            const DataCell &cell = r.at(primaryKey);
            if (!cell.isNullValue()) {
                String whereStr = String::format("%s=%s", primaryKey.c_str(), cell.valueStr(true).c_str());
                sql += String::convert(replaceStr,
                                       table.name().c_str(), columnsStr.c_str(), valuesStr.c_str(),
                                       primaryKey.c_str(), table.name().c_str(), whereStr.c_str(),
                                       table.name().c_str(), columnsStr.c_str(), valuesStr.c_str(), whereStr.c_str());
            }
        }
        return sql;
    }

    int Dm6Client::executeSqlInsertInner(const DataTable &table, bool replace) {
#if DEBUG
        String info = String::convert("Dm6Client::executeSqlInsertInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif
        if (table.name().isNullOrEmpty())
            return -1;
        if (table.columnCount() == 0)
            return -2;
        if (table.rowCount() == 0)
            return -3;

        String sql;
        const DataRows &rows = table.rows();
        for (size_t i = 0; i < rows.count(); i++) {
            const DataRow &row = rows.at(i);

            if (replace) {
                sql.appendLine(toMergeStr(table, row));
            } else {
                sql.appendLine(toInsertStr(table, row));
            }
        }

#ifdef DEBUG
        createSqlFile("dm6_insert.sql", sql);
#endif

        return executeSqlInner(sql);
    }

    int Dm6Client::executeSqlQueryInner(const String &sql, DataTable &table) {
#if DEBUG
        String info = String::convert("Dm6Client::executeSqlQueryInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        HsmtInner hsmt;
        int status = executeInner(sql, hsmt);
        if (isSucceed(status)) {
            updateDataTable(hsmt.hsmt, table);
            dm_free_stmt(hsmt.hsmt);
        }
        return status;
    }

    void Dm6Client::updateDataTable(void *tag, DataTable &table) {
        auto hsmt = (dm_hstmt) tag;
#if DEBUG
        String info = String::convert("Dm6Client::updateDataTable, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        if (table.name().isNullOrEmpty()) {
            table.setName("temp");
        }

#if DEBUG
        sw.setInfo(String::convert("Dm6Client::updateDataTable, the table name is '%s'", table.name().c_str()));
#endif

        Vector<dm_ulint> dataTypes;
        dm_usint columnCount = 0;
        dm_describe_col_num(hsmt, &columnCount);
        int table_columnCount = (int) table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0) {
            if (table_columnCount == 0) {
                for (int i = 0; i < columnCount; i++) {
                    dm_char colname[255];
                    dm_usint colnamesize;
                    dm_ulint dataType;
                    dm_ulint collen;
                    dm_usint digitlen;
                    dm_usint nullable;
                    dm_ulint column_size;
                    dm_bool result = dm_describe_column(hsmt,
                                                        i + 1,
                                                        colname,
                                                        sizeof(colname),
                                                        &colnamesize,
                                                        &dataType,
                                                        &collen,
                                                        &digitlen,
                                                        &nullable,
                                                        &column_size);
                    if (result != 0) {
                        String name((const char *) colname);
                        name = !name.isNullOrEmpty() ? name : String::format("temp%s", i);
                        table.addColumn(DataColumn(name, getColumnType((int) dataType)));
                        dataTypes.add(dataType);
                    }
                }
            }

            char value[65535];
#ifdef WIN32
            __int64 row_count = 0;
#else
            long long int row_count = 0;
#endif
            dm_describe_row_num(hsmt, &row_count);
            for (int i = 0; i < (int) row_count + 1; i++) {  // It has a bug: the first row is always null.
                DataRow row;
                for (int j = 0; j < columnCount; j++) {
                    const DataColumn &column = table.columns().at(j);
                    DbType type = column.type();
                    dm_bool result = dm_get_data(hsmt, j + 1, dataTypes[j], TYPE_CHAR, value, sizeof(value), nullptr);
                    if (i > 0) {
                        if (result != 0) {
                            row.addCell(DataCell(column, DbValue(type, value)));
                        } else {
                            row.addCell(DataCell(column));
                        }
                    }
                }
                if (row.cellCount() > 0) {
                    table.addRow(row);
                }
                dm_fetch_scroll(hsmt, DM_FETCH_NEXT, 0);
            }
        }
    }

    int Dm6Client::beginTransactionInner() {
        return 1;
//        return executeSqlInner("BEGIN TRANSACTION");
    }

    int Dm6Client::commitTransactionInner() {
        return dm_commit(_dm6Db->hdbc);
    }

    int Dm6Client::rollbackTransactionInner() {
        return dm_rollback(_dm6Db->hdbc);
    }

    StringArray Dm6Client::getColumnName(const String &tableName) {
        StringArray result;
        if (!tableName.isNullOrEmpty()) {
            String sql = String::format("SELECT NAME FROM SYSCOLUMNS WHERE ID IN (\n"
                                        "\tSELECT ID FROM SYSTABLES WHERE NAME='%s'\n"
                                        ")",
                                        tableName.toUpper().c_str());
            DataTable table("columns");
            if (executeSqlQuery(sql, table) && table.rowCount() > 0) {
                for (size_t i = 0; i < table.rowCount(); ++i) {
                    result.add(table.rows()[i][0].valueStr());
                }
            }
        }
        return result;
    }

    DbType Dm6Client::getColumnType(int type) {
        if (type == DM_DATA_CHAR)  // CHAR
            return DbType::Text;
        else if (type == DM_DATA_VARCHAR2)  // VARCHAR
            return DbType::Text;
        else if (type == DM_DATA_VARCHAR)  // VARCHAR
            return DbType::Text;
        else if (type == DM_DATA_BIT)  // BIT STRING
            return DbType::Text;
        else if (type == DM_DATA_VARBIT)  // VAR BIT STRING
            return DbType::Text;
        else if (type == DM_DATA_TINYINT)  // SMALL INTEGER, 2 bytes
            return DbType::Integer16;
        else if (type == DM_DATA_SMALLINT)  // SMALL INTEGER, 2 bytes
            return DbType::Integer16;
        else if (type == DM_DATA_INT)  // INTEGER 4 bytes
            return DbType::Integer32;
        else if (type == DM_DATA_INT64)  // INTEGER 8 bytes
            return DbType::Integer64;
        else if (type == DM_DATA_DEC)  // DECIMAL
            return DbType::Float64;
        else if (type == DM_DATA_FLOAT)  // FLOAT, SINGLE
            return DbType::Float32;
        else if (type == DM_DATA_DOUBLE)  // DOUBLE
            return DbType::Float64;
        else if (type == DM_DATA_BLOB)  // BINARY LARGE OBJECT
            return DbType::Null;  // Not supported
        else if (type == DM_DATA_DATE)  // DAT
            return DbType::Date;
        else if (type == DM_DATA_TIME)  // TIM
            return DbType::Time;
        else if (type == DM_DATA_DATETIME)  // DATE TIME
            return DbType::Timestamp;
        else if (type == DM_DATA_BINARY)  // BINARY
            return DbType::Null;  // Not supported
        else if (type == DM_DATA_VARBINARY)  // VAR BINARY
            return DbType::Null;  // Not supported
        else if (type == DM_DATA_TEXT)  // TEXT
            return DbType::Text;
        else if (type == DM_DATA_INTERVAL_YM)  // INTERVAL YEAR MONTH
            return DbType::Interval;
        else if (type == DM_DATA_INTERVAL_DT)  // INTERVAL DAY TIME
            return DbType::Interval;
        else if (type == DM_DATA_NULL)  // NULL
            return DbType::Null;
        return DbType::Null;
    }

    bool Dm6Client::ping() {
        return true;
    }

    String Dm6Client::getErrorMsg() {
        if (_dm6Db->isOpened()) {
            long errorCode = dm_con_get_errorcode(_dm6Db->hdbc);
            String error = String::format("error code: %", errorCode);
#ifdef WIN32
            return String::GBKtoUTF8(error);
#else
            return error;
#endif
        } else {
            return String::Empty;
        }
    }

    bool Dm6Client::isSucceed(int result) {
        return result != 0;
    }

    void Dm6Client::printErrorInfo(const String &methodName, const String &sql, const HsmtInner *hsmt) {
#ifdef DEBUG
        if (!sql.isNullOrEmpty()) {
            String path = Path::combine(Application::instance()->rootPath(), "errorSql");
            if (!Directory::exists(path))
                Directory::createDirectory(path);
            String fileName = Path::combine(path, String::convert("%s.sql", Uuid::generate().toString().c_str()));
            FileStream fs(fileName.c_str(), FileMode::FileCreate, FileAccess::FileWrite);
            fs.writeText(sql);
            fs.close();
        }
#endif

        String error;
        if (hsmt != nullptr && hsmt->hsmt != nullptr) {
            error = (const char *) dm_stmt_get_errormsg(hsmt->hsmt);
        } else {
            error = getErrorMsg();
        }
        DbClient::printErrorInfo(methodName, sql, error);
    }

    bool Dm6Client::isConnected() const {
        if (_dm6Db->isOpened() &&
            dm_con_get_errorcode(_dm6Db->hdbc) == 0) {
            return true;
        }
        return false;
    }
}