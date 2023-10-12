//
//  Dm7Client.cpp
//  common
//
//  Created by baowei on 2023/9/12.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/Dm7Client.h"
#include "data/Convert.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "system/Application.h"
#include "IO/FileStream.h"

#ifdef DEBUG

#include "IO/Directory.h"

#endif

#include "IO/Path.h"
#include "dm7/DPI.h"
#include "dm7/DPIext.h"
#include "dm7/DPItypes.h"

#ifdef WIN32
#pragma warning(disable: 4312)
#else
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

using namespace System;

namespace Database {
    class Dm7Inner {
    public:
        dhenv henv;
        dhcon hdbc;

        Dm7Inner() : henv(nullptr), hdbc(nullptr) {
        }

        ~Dm7Inner() {
            close();
        }

        void open() {
            if (isOpened()) {
                close();
            }

            dpi_alloc_env(&henv);
            dpi_alloc_con(henv, &hdbc);
        }

        void close() {
            if (hdbc != nullptr) {
                dpi_free_con(hdbc);
                hdbc = nullptr;
            }
            if (henv != nullptr) {
                dpi_free_env(henv);
                henv = nullptr;
            }
        }

        bool isOpened() const {
            return henv != nullptr && henv != nullptr;
        }
    };

    class HsmtInner {
    public:
        dhstmt hsmt;

        explicit HsmtInner(dhstmt hsmt = nullptr) : hsmt(hsmt) {
        }
    };

    Dm7Client::Dm7Client() {
        _dm7Db = new Dm7Inner();
    }

    Dm7Client::~Dm7Client() {
        if (_dm7Db->isOpened()) {
            Dm7Client::close();
        }
        delete _dm7Db;
        _dm7Db = nullptr;
    }

    bool Dm7Client::open(const StringMap &connections) {
        Locker locker(&_dbMutex);

        return openInner(connections);
    }

    bool Dm7Client::openInner(const StringMap &connections) {
        _dm7Db->open();
        if (_dm7Db->isOpened()) {
            String host = connections["host"];
            String port = connections["port"];
            String dbname = connections["dbname"];
            String user = connections["user"];
            String password = connections["password"];
            String encoding = connections["encoding"];

            udint4 timeout; // seconds.
            if (UInt32::parse(connections["timeout"], timeout)) {
                timeout *= 1000;
                dpi_set_con_attr(_dm7Db->hdbc, DSQL_ATTR_LOGIN_TIMEOUT,
                                 (dpointer) timeout, sizeof(timeout));
            }
            if (!encoding.isNullOrEmpty()) {
                udint4 code = PG_GBK;
                if (String::equals(encoding, "utf8", true) ||
                    String::equals(encoding, "utf-8", true)) {
                    code = PG_UTF8;
                } else if (String::equals(encoding, "gbk", true)) {
                    code = PG_GBK;
                } else if (String::equals(encoding, "big5", true)) {
                    code = PG_BIG5;
                } else if (String::equals(encoding, "gb18030", true)) {
                    code = PG_GB18030;
                } else if (String::equals(encoding, "ISO_8859_9", true)) {
                    code = PG_ISO_8859_9;
                } else if (String::equals(encoding, "EUC_JP", true)) {
                    code = PG_EUC_JP;
                } else if (String::equals(encoding, "EUC_KR", true)) {
                    code = PG_EUC_KR;
                } else if (String::equals(encoding, "KOI8R", true)) {
                    code = PG_KOI8R;
                } else if (String::equals(encoding, "ISO_8859_1", true)) {
                    code = PG_ISO_8859_1;
                } else if (String::equals(encoding, "SQL_ASCII", true)) {
                    code = PG_SQL_ASCII;
                } else if (String::equals(encoding, "ISO_8859_11", true)) {
                    code = PG_ISO_8859_11;
                }
                dpi_set_con_attr(_dm7Db->hdbc, DSQL_ATTR_LOCAL_CODE, (dpointer) code, sizeof(code));
            }

            String svr = host + ":" + port;
            DPIRETURN result;
            result = dpi_login(_dm7Db->hdbc,
                               (sdbyte *) svr.c_str(),
                               (sdbyte *) user.c_str(),
                               (sdbyte *) password.c_str());
            if (isSucceed(result)) {
                Trace::debug(String::format("Open dm7 successfully. host: %s, port: %s, dbname: %s, user name: %s",
                                            host.c_str(), port.c_str(), dbname.c_str(), user.c_str()));
                return true;
            } else {
                unsigned int errorCode;
                String errorMessage;
                getErrorInfo(DSQL_HANDLE_DBC, _dm7Db->hdbc, errorCode, errorMessage);
                Trace::error(String::format(
                        "Failed to open dm7. host: %s, port: %s, dbname: %s,"
                        " user name: %s, error code: %d, error message: %s",
                        host.c_str(), port.c_str(), dbname.c_str(), user.c_str(), errorCode, errorMessage.c_str()));
                _dm7Db->close();
            }
        } else {
            Trace::error("Failed to open db, error: can not init env!");
        }
        return false;
    }

    bool Dm7Client::isConnected() {
        Locker locker(&_dbMutex);
        return isConnectedInner();
    }

    bool Dm7Client::isConnectedInner() {
        if (_dm7Db->isOpened()) {
            sdint4 connection = 0, len = 0;
            DPIRETURN result = dpi_get_con_attr(_dm7Db->hdbc, DSQL_ATTR_CONNECTION_DEAD,
                             &connection, sizeof(connection), &len);
            if (isSucceed(result)) {
                return connection == 0;
            }
        }
        return false;
    }

    bool Dm7Client::reopen() {
        Trace::info("Reopen the dm7 client!");
        if (_dm7Db->isOpened()) {
            dpi_logout(_dm7Db->hdbc);
        }
        return openInner(_connectionParams);
    }

    bool Dm7Client::close() {
        Locker locker(&_dbMutex);

        if (_dm7Db->isOpened()) {
            dpi_logout(_dm7Db->hdbc);
            _dm7Db->close();
            return true;
        } else {
            return false;
        }
    }

    int Dm7Client::executeInner(const String &sql, HsmtInner &hsmt) {
        DPIRETURN result;
        result = dpi_alloc_stmt(_dm7Db->hdbc, &hsmt.hsmt);
        result = dpi_exec_direct(hsmt.hsmt, (sdbyte *) sql.c_str());
        if (!isSucceed(result)) {
            if (!isConnectedInner()) {
                dpi_free_stmt(hsmt.hsmt);

                reopen();

                // re-execute again.
                dpi_alloc_stmt(_dm7Db->hdbc, &hsmt.hsmt);
                result = dpi_exec_direct(hsmt.hsmt, (sdbyte *) sql.c_str());
                if (isSucceed(result)) {
                    dpi_free_stmt(hsmt.hsmt);
                }
            } else {
                unsigned int errorCode;
                String errorMessage;
                getErrorInfo(DSQL_HANDLE_STMT, hsmt.hsmt, errorCode, errorMessage);
#ifdef WIN32
                String error = String::GBKtoUTF8(errorMessage);
#else
                String error = errorMessage;
#endif
                Trace::error(String::format("dm7_execute'%s' error code: %d, error: %s",
                                            sql.c_str(), errorCode, error.c_str()));
                dpi_free_stmt(hsmt.hsmt);
            }
        }
        return (int) result;
    }

    bool Dm7Client::executeSql(const String &sql, bool transaction) {
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

    bool Dm7Client::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool Dm7Client::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool Dm7Client::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    bool Dm7Client::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        int result = executeSqlQueryInner(sql, table);
        return isSucceed(result);
    }

    bool Dm7Client::executeSqlInsert(const DataTable &table, bool transaction) {
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

    bool Dm7Client::executeSqlReplace(const DataTable &table, bool transaction) {
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

    int Dm7Client::executeSqlInner(const String &sql) {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("Dm7Client::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif

        DPIRETURN result;
        dhstmt hsmt;
        result = dpi_alloc_stmt(_dm7Db->hdbc, &hsmt);
        StringArray sqls;
        StringArray::parse(sql, sqls, ';');
        for (size_t i = 0; i < sqls.count(); ++i) {
            String str = sqls[i].trim('\r', '\n');
            if (!str.isNullOrEmpty()) {
                result = dpi_exec_direct(hsmt, (sdbyte *) sqls[i].c_str());
                if (!isSucceed(result)) {
                    unsigned int errorCode;
                    String errorMessage;
                    getErrorInfo(DSQL_HANDLE_STMT, hsmt, errorCode, errorMessage);
#ifdef WIN32
                    String error = String::GBKtoUTF8(errorMessage);
#else
                    String error = errorMessage;
#endif
                    Trace::error(String::format("dm7_execute'%s' error code: %d, error: %s",
                                                sql.c_str(), errorCode, error.c_str()));
                    break;
                }
            }
        }
        dpi_free_stmt(hsmt);
        return result;
    }

    String Dm7Client::toInsertStr(const DataTable &table, const DataRow &row) {
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

    String Dm7Client::toMergeStr(const DataTable &table, const DataRow &row) {
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
                                        "UPDATE %s SET (%s)=(SELECT %s) WHERE %s;\n";

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

    int Dm7Client::executeSqlInsertInner(const DataTable &table, bool replace) {
#if DEBUG
        String info = String::convert("Dm7Client::executeSqlInsertInner, table name: %s", table.name().c_str());
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
        createSqlFile("dm7_insert.sql", sql);
#endif

        return executeSqlInner(sql);
    }

    int Dm7Client::executeSqlQueryInner(const String &sql, DataTable &table) {
#if DEBUG
        String info = String::convert("Dm7Client::executeSqlQueryInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        HsmtInner hsmt;
        int status = executeInner(sql, hsmt);
        if (isSucceed(status)) {
            updateDataTable(hsmt.hsmt, table);
            dpi_free_stmt(hsmt.hsmt);
        }
        return status;
    }

    void Dm7Client::updateDataTable(void *tag, DataTable &table) {
        auto hsmt = (dhstmt) tag;
#if DEBUG
        String info = String::convert("Dm7Client::updateDataTable, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        if (table.name().isNullOrEmpty()) {
            table.setName("temp");
        }

#if DEBUG
        sw.setInfo(String::convert("Dm7Client::updateDataTable, the table name is '%s'", table.name().c_str()));
#endif

        Vector<sdint2> dataTypes;
        sdint2 columnCount = 0;
        dpi_number_columns(hsmt, &columnCount);
        int table_columnCount = (int) table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0) {
            if (table_columnCount == 0) {
                for (sdint2 i = 0; i < columnCount; i++) {
                    sdbyte colname[255];
                    sdint2 colnamesize;
                    sdint2 dataType;
                    ulength collen;
                    sdint2 digitlen;
                    sdint2 nullable;
                    DPIRETURN result = dpi_desc_column(hsmt,
                                                       (sdint2)(i + 1),
                                                       colname,
                                                       (sdint2)(sizeof(colname) - 1),
                                                       &colnamesize,
                                                       &dataType,
                                                       &collen,
                                                       &digitlen,
                                                       &nullable);
                    if (isSucceed(result)) {
                        String name((const char *) colname);
                        name = !name.isNullOrEmpty() ? name : String::format("temp%s", i);
                        table.addColumn(DataColumn(name, getColumnType((int) dataType)));
                        dataTypes.add(dataType);
                    }
                }
            }

            int i = 0;
            ulength row_num;
            slength val_len;
            char value[65535] = {0};
            while (dpi_fetch_scroll(hsmt, DSQL_FETCH_NEXT, 0, &row_num) != DSQL_NO_DATA) {
                DataRow row;
                for (int j = 0; j < columnCount; j++) {
                    const DataColumn &column = table.columns().at(j);
                    DbType type = column.type();
                    DPIRETURN result = dpi_get_data(hsmt, j + 1, DSQL_C_CHAR, value, sizeof(value), &val_len);
                    if (isSucceed(result)) {
                        row.addCell(DataCell(column, DbValue(type, String(value, val_len))));
                    } else {
                        row.addCell(DataCell(column));
                    }
                }
                if (row.cellCount() > 0) {
                    table.addRow(row);
                }

                i++;
            }
        }
    }

    int Dm7Client::beginTransactionInner() {
        udint4 value = DSQL_AUTOCOMMIT_OFF;
        DPIRETURN result = dpi_set_con_attr(_dm7Db->hdbc, DSQL_ATTR_AUTOCOMMIT, (dpointer) value, sizeof(value));
        return result;
    }

    int Dm7Client::commitTransactionInner() {
        DPIRETURN result = dpi_commit(_dm7Db->hdbc);
        udint4 value = DSQL_AUTOCOMMIT_ON;
        dpi_set_con_attr(_dm7Db->hdbc, DSQL_ATTR_AUTOCOMMIT, (dpointer) value, sizeof(value));
        return result;
    }

    int Dm7Client::rollbackTransactionInner() {
        DPIRETURN result = dpi_rollback(_dm7Db->hdbc);
        udint4 value = DSQL_AUTOCOMMIT_ON;
        dpi_set_con_attr(_dm7Db->hdbc, DSQL_ATTR_AUTOCOMMIT, (dpointer) value, sizeof(value));
        return result;
    }

    StringArray Dm7Client::getColumnName(const String &tableName) {
        StringArray result;
        if (!tableName.isNullOrEmpty()) {
            String sql = String::format("SELECT NAME FROM SYSCOLUMNS WHERE ID IN (\n"
                                        "\tSELECT ID FROM SYSOBJECTS WHERE NAME='%s'\n"
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

    DbType Dm7Client::getColumnType(int type) {
        if (type == DSQL_CHAR)           /* CHAR     */
            return DbType::Text;
        else if (type == DSQL_VARCHAR)           /* VARCHAR  */
            return DbType::Text;
        else if (type == DSQL_BIT)           /* BIT STRING */
            return DbType::Text;
        else if (type == DSQL_TINYINT)           /* 1 byte INT */
            return DbType::Integer8;
        else if (type == DSQL_SMALLINT)           /* SMALL INTEGER, 2 bytes */
            return DbType::Integer16;
        else if (type == DSQL_INT)           /* INTEGER 4 bytes */
            return DbType::Integer32;
        else if (type == DSQL_BIGINT)           /* INTEGER 8 bytes */
            return DbType::Integer64;
        else if (type == DSQL_DEC)           /* DECIMAL */
            return DbType::Decimal;
        else if (type == DSQL_FLOAT)          /* FLOAT, SINGLE */
            return DbType::Float32;
        else if (type == DSQL_DOUBLE)          /* DOUBLE */
            return DbType::Float64;
        else if (type == DSQL_BLOB)          /* BINARY LARGE OBJECT */
            return DbType::Blob;
        else if (type == DSQL_DATE)          /* DATE*/
            return DbType::Date;
        else if (type == DSQL_TIME)          /* TIME*/
            return DbType::Time;
        else if (type == DSQL_TIMESTAMP)          /* DATE TIME */
            return DbType::Timestamp;
        else if (type == DSQL_BINARY)          /* BINARY */
            return DbType::Blob;
        else if (type == DSQL_VARBINARY)          /* VAR BINARY */
            return DbType::Blob;
        else if (type == DSQL_CLOB)          /* TEXT */
            return DbType::Text;
        else if (type == DSQL_TIME_TZ)          /* TIME WITH TIME ZONE*/
            return DbType::Time;
        else if (type == DSQL_TIMESTAMP_TZ)          /* TIMESTAMP WITH TIME ZONE*/
            return DbType::Timestamp;
        else if (type == DSQL_CLASS)
            return DbType::Blob;
        else if (type == DSQL_RECORD)
            return DbType::Blob;
        else if (type == DSQL_ARRAY)
            return DbType::Blob;
        else if (type == DSQL_SARRAY)
            return DbType::Blob;
        else if (type == DSQL_RSET)         /* RESULT SET*/
            return DbType::Blob;
        else if (type == DSQL_BFILE)        /* BFILE*/
            return DbType::Blob;
        else if (DSQL_IS_TYPE_INTERVAL(type))
            return DbType::Interval;
        return DbType::Null;
    }

    bool Dm7Client::ping() {
        if (_dbMutex.tryLock()) {
            if (_dm7Db->isOpened() && isConnectedInner()) {
                _dbMutex.unlock();
                return true;
            } else {
                _dbMutex.unlock();
                return false;
            }
        }
        return true;
    }

    String Dm7Client::getErrorMsg() {
        if (_dm7Db->isOpened()) {
            unsigned int errorCode;
            String errorMessage;
            getErrorInfo(DSQL_HANDLE_DBC, _dm7Db->hdbc, errorCode, errorMessage);
#ifdef WIN32
            return String::GBKtoUTF8(errorMessage);
#else
            return errorMessage;
#endif
        } else {
            return String::Empty;
        }
    }

    bool Dm7Client::isSucceed(int result) {
        return DSQL_SUCCEEDED((signed short) result);
    }

    void Dm7Client::getErrorInfo(signed short hndl_type, void *hndl, unsigned int &errorCode, String &errorMessage) {
        sdint4 err_code = 0;
        sdint2 msg_len = 0;
        sdbyte err_msg[SDBYTE_MAX] = {0};
//        dpi_get_diag_field(hndl_type, hndl, 1, DSQL_DIAG_MESSAGE_TEXT, err_msg, sizeof(err_msg), NULL); printf("err_msg = %s\n", err_msg);
        DPIRETURN result = dpi_get_diag_rec(hndl_type, hndl, 1, &err_code, err_msg, sizeof(err_msg), &msg_len);
        if (isSucceed(result)) {
            errorCode = err_code;
            errorMessage = String((const char *) err_msg);
        } else {
            errorCode = 0;
            errorMessage = String::Empty;
        }
    }

    void Dm7Client::printErrorInfo(const String &methodName, const String &sql, const HsmtInner *hsmt) {
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

        String errorMessage;
        if (hsmt != nullptr && hsmt->hsmt != nullptr) {
            unsigned int errorCode;
            getErrorInfo(DSQL_HANDLE_STMT, hsmt->hsmt, errorCode, errorMessage);
        } else {
            errorMessage = getErrorMsg();
        }
        DbClient::printErrorInfo(methodName, sql, errorMessage);
    }
}