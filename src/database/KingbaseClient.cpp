//
//  KingbaseClient.cpp
//  common
//
//  Created by baowei on 2022/12/30.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "database/KingbaseClient.h"
#include "data/Convert.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "system/Application.h"
#include "IO/FileStream.h"

#ifdef DEBUG

#include "IO/Directory.h"

#endif

#include "IO/Path.h"
#include "kci/libkci.h"

using namespace System;

namespace Database {
    class KingbaseInner {
    public:
        KCIConnection *kingbaseDb;

        KingbaseInner() : kingbaseDb(nullptr) {
        }
    };

    class ResultInner {
    public:
        KCIResult *res;

        explicit ResultInner(KCIResult *res = nullptr) : res(res) {
        }
    };

    KingbaseClient::KingbaseClient() {
        _kingbaseDb = new KingbaseInner();
    }

    KingbaseClient::~KingbaseClient() {
        if (_kingbaseDb->kingbaseDb != nullptr) {
            KingbaseClient::close();
        }
        delete _kingbaseDb;
        _kingbaseDb = nullptr;
    }

    bool KingbaseClient::open(const StringMap &connections) {
        Locker locker(&_dbMutex);

        return openInner(connections);
    }

    bool KingbaseClient::isConnected() {
        Locker locker(&_dbMutex);
        return isConnectedInner();
    }

    bool KingbaseClient::isConnectedInner() {
        if (_kingbaseDb->kingbaseDb != nullptr &&
            KCIConnectionGetStatus(_kingbaseDb->kingbaseDb) == CONNECTION_OK) {
            int sock = KCIConnectionGetSocket(_kingbaseDb->kingbaseDb);
            return sock >= 0;
        }
        return false;
    }

    bool KingbaseClient::openInner(const StringMap &connections) {
        String connStr;
        static const char *fmt = "%s='%s' ";
        String host = connections["host"];
        String port = connections["port"];
        String dbname = connections["dbname"];
        String user = connections["user"];
        connStr.appendFormat(fmt, "host", host.c_str());
        connStr.appendFormat(fmt, "port", port.c_str());
        connStr.appendFormat(fmt, "dbname", dbname.c_str());
        connStr.appendFormat(fmt, "user", user.c_str());
        connStr.appendFormat(fmt, "password", connections["password"].c_str());
        connStr.appendFormat(fmt, "connect_timeout", connections["timeout"].c_str());
        KCIConnection *conn = KCIConnectionCreate(connStr);
        if (KCIConnectionGetStatus(conn) == CONNECTION_OK) {
            _connectionParams = connections;
            _kingbaseDb->kingbaseDb = conn;

            Trace::debug(String::format("Open kingbase successfully. host: %s, port: %s, dbname: %s, user name: %s",
                                        host.c_str(), port.c_str(), dbname.c_str(), user.c_str()));
            return true;
        } else {
            Trace::debug(String::format(
                    "Failed to open kingbase. host: %s, port: %s, dbname: %s, user name: %s, reason: '%s'",
                    host.c_str(), port.c_str(), dbname.c_str(), user.c_str(), getErrorMsg().c_str()));

            _kingbaseDb->kingbaseDb = conn;
            printErrorInfo("KCIConnectionCreate");
            KCIConnectionDestory(conn);
            _kingbaseDb->kingbaseDb = nullptr;
        }
        return false;
    }

    bool KingbaseClient::reopen() {
        Trace::info("Reopen the kingbase client!");
        if (_kingbaseDb->kingbaseDb != nullptr) {
            KCIConnectionDestory(_kingbaseDb->kingbaseDb);
        }
        return openInner(_connectionParams);
    }

    bool KingbaseClient::close() {
        Locker locker(&_dbMutex);

        KCIConnectionDestory(_kingbaseDb->kingbaseDb);
        _kingbaseDb->kingbaseDb = nullptr;
        return true;
    }

    int KingbaseClient::executeInner(const String &sql, ResultInner &result) {
        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, sql.c_str());
        KCIExecuteStatus status = KCIResultGetStatusCode(res);
        if (!isSucceed(status)) {
            if (status == EXECUTE_FATAL_ERROR && !isConnectedInner()) {
                KCIResultDealloc(res);

                reopen();

                // re-execute again.
                res = KCIStatementExecute(_kingbaseDb->kingbaseDb, sql.c_str());
                status = KCIResultGetStatusCode(res);
                if (!isSucceed(status)) {
                    KCIResultDealloc(res);
                    return status;
                } else {
                    result.res = res;
                }
            } else {
                auto r = ResultInner(res);
                printErrorInfo("kingbase_execute", sql, &r);
                KCIResultDealloc(res);
            }
        } else {
            result.res = res;
        }
        return status;
    }

    bool KingbaseClient::executeSql(const String &sql, bool transaction) {
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

    bool KingbaseClient::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool KingbaseClient::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool KingbaseClient::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    bool KingbaseClient::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        int result = executeSqlQueryInner(sql, table);
        return isSucceed(result);
    }

    bool KingbaseClient::executeSqlInsert(const DataTable &table, bool transaction) {
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

    bool KingbaseClient::executeSqlReplace(const DataTable &table, bool transaction) {
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

    int KingbaseClient::executeSqlInner(const String &sql) {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("KingbaseClient::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif

        ResultInner result;
        int status = executeInner(sql, result);
        if (isSucceed(status)) {
            KCIResultDealloc(result.res);
        }
        return status;
//        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, sql.c_str());
//        KCIExecuteStatus result = KCIResultGetStatusCode(res);
//        if (!isSucceed(result)) {
//            if (result == EXECUTE_FATAL_ERROR && !isConnected()) {
//                reopen();
//            } else {
//                auto r = ResultInner(res);
//                printErrorInfo("kingbase_execute", sql, &r);
//            }
//        }
//        KCIResultDealloc(res);
//        return result;
    }

    String KingbaseClient::toInsertStr(const DataTable &table, const DataRow &row) {
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

    String KingbaseClient::toMergeStr(const DataTable &table, const DataRow &row) {
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

        // MERGE INTO T T1
        //	USING(SELECT '1001' AS a, 2 AS b FROM dual) T2
        //	ON(T1.a = T2.a)
        //	WHEN MATCHED THEN
        //	UPDATE SET T1.b = T2.b
        //	WHEN NOT MATCHED THEN
        //	INSERT(a, b) VALUES(T2.a, T2.b);
        static const char *mergeStr = "MERGE INTO %s T1\
								USING(SELECT %s FROM dual) T2\
								ON(T1.%s = T2.%s)\
								WHEN MATCHED THEN\
								UPDATE SET %s\
								WHEN NOT MATCHED THEN\
								INSERT(%s) VALUES(%s);";

        String sValuesStr;
        String uValuesStr;
        String iColumnsStr;
        String iValuesStr;
        for (size_t j = 0; j < columnCount; j++) {
            const DataCell &cell = row.cells().at(j);
            if (!cell.isNullValue()) {
                String name = table.columns().at(j).name();
                String value = cell.valueStr(true);

                if (!sValuesStr.isNullOrEmpty()) {
                    sValuesStr.append(", ");
                }
                String sValue = String::convert("%s AS %s", value.c_str(), name.c_str());
                sValuesStr.append(sValue);

                if (name != primaryKey) {
                    if (!uValuesStr.isNullOrEmpty()) {
                        uValuesStr.append(", ");
                    }
                    String uValue = String::convert("T1.%s = T2.%s", name.c_str(), name.c_str());
                    uValuesStr.append(uValue);
                }

                if (!iColumnsStr.isNullOrEmpty()) {
                    iColumnsStr.append(", ");
                }
                iColumnsStr.append(name.c_str());

                if (!iValuesStr.isNullOrEmpty()) {
                    iValuesStr.append(", ");
                }
                String iValue = String::convert("T2.%s", name.c_str());
                iValuesStr.append(iValue);
            }
        }
        String sql = String::convert(mergeStr, table.name().c_str(), sValuesStr.c_str(),
                                     primaryKey.c_str(), primaryKey.c_str(),
                                     uValuesStr.c_str(), iColumnsStr.c_str(), iValuesStr.c_str());
        return sql;
    }

    int KingbaseClient::executeSqlInsertInner(const DataTable &table, bool replace) {
#if DEBUG
        String info = String::convert("KingbaseClient::executeSqlInsertInner, table name: %s", table.name().c_str());
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
        createSqlFile("kingbase_insert.sql", sql);
#endif

        return executeSqlInner(sql);
    }

    int KingbaseClient::executeSqlQueryInner(const String &sql, DataTable &table) {
#if DEBUG
        String info = String::convert("KingbaseClient::executeSqlQueryInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        ResultInner result;
        int status = executeInner(sql, result);
        if (isSucceed(status)) {
            updateDataTable(result.res, table);
            KCIResultDealloc(result.res);
        }
        return status;
//        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, sql.c_str());
//        KCIExecuteStatus result = KCIResultGetStatusCode(res);
//        if (!isSucceed(result)) {
//            if (result == EXECUTE_FATAL_ERROR && !isConnected()) {
//                reopen();
//            } else {
//                auto r = ResultInner(res);
//                printErrorInfo("kingbase_execute", sql, &r);
//                KCIResultDealloc(res);
//            }
//        } else {
//            updateDataTable(res, table);
//            KCIResultDealloc(res);
//        }
//        return result;
    }

    void KingbaseClient::updateDataTable(void *tag, DataTable &table) {
        auto *res = (KCIResult *) tag;
#if DEBUG
        String info = String::convert("KingbaseClient::updateDataTable, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif

        if (table.name().isNullOrEmpty()) {
            table.setName("temp");
        }

#if DEBUG
        sw.setInfo(String::convert("KingbaseClient::updateDataTable, the table name is '%s'", table.name().c_str()));
#endif

        int columnCount = KCIResultGetColumnCount(res);
        int table_columnCount = (int) table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0) {
            if (table_columnCount == 0) {
                for (int i = 0; i < columnCount; i++) {
                    const char *nameStr = KCIResultGetColumnName(res, i);
                    String name;
                    if (nameStr != nullptr) {
                        name = nameStr;
                    } else {
                        char temp[32];
                        sprintf(temp, "tempCol%d", i);
                        name = temp;
                    }
                    table.addColumn(DataColumn(name, getColumnType((int) KCIResultGetColumnType(res, i))));
                }
            }

            for (int i = 0; i < KCIResultGetRowCount(res); i++) {
                DataRow row;
                for (int j = 0; j < columnCount; j++) {
                    const DataColumn &column = table.columns().at(j);
                    DbType type = column.type();
                    const char *str = (const char *) KCIResultGetColumnValue(res, i, j);
                    row.addCell(DataCell(column, DbValue(type, String::GBKtoUTF8(str))));
                }
                table.addRow(row);
            }
        }
    }

    int KingbaseClient::beginTransactionInner() {
        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, "BEGIN TRANSACTION");
        KCIExecuteStatus result = KCIResultGetStatusCode(res);
        KCIResultDealloc(res);
        return result;
    }

    int KingbaseClient::commitTransactionInner() {
        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, "COMMIT TRANSACTION");
        KCIExecuteStatus result = KCIResultGetStatusCode(res);
        KCIResultDealloc(res);
        return result;
    }

    int KingbaseClient::rollbackTransactionInner() {
        KCIResult *res = KCIStatementExecute(_kingbaseDb->kingbaseDb, "ROLLBACK TRANSACTION");
        KCIExecuteStatus result = KCIResultGetStatusCode(res);
        KCIResultDealloc(res);
        return result;
    }

    StringArray KingbaseClient::getColumnName(const String &tableName) {
        StringArray result;
        if (!tableName.isNullOrEmpty()) {
            String sql = String::format("SELECT COLUMN_NAME FROM ALL_TAB_COLUMNS WHERE TABLE_NAME='%s'",
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

    DbType KingbaseClient::getColumnType(int type) {
        switch (type) {
            case KCI_BOOLOID:        /* bool */
                return DbType::Digital;
            case KCI_BYTEAOID:        /* varbinary,bytea */
            case KCI_INT2OID:        /* smallint */
            case KCI_INT4OID:        /* int */
            case KCI_TINYINTOID:      /* tinyint */
                return DbType::Integer32;
            case KCI_INT8OID:        /* bigint  */
                return DbType::Integer64;
            case KCI_FLOAT4OID:       /* real */
                return DbType::Float32;
            case KCI_FLOAT8OID:       /* double，float(default) */
                return DbType::Float64;
            case KCI_NUMERICOID:      /* numeric,decimal */
                return DbType::Decimal;
            case KCI_BPCHAROID:      /* char */
            case KCI_VARCHAROID:      /* varchar */
            case KCI_TEXTOID:        /* text */
                return DbType::Text;
            case KCI_DATEOID:      /* date */
                return DbType::Date;
            case KCI_TIMEOID:      /* time */
            case KCI_TIMETZOID:      /* time with time zone */
                return DbType::Time;
            case KCI_DATETIMEOID:      /* datetime */
            case KCI_TIMESTAMPOID:      /* timestamp */
            case KCI_TIMESTAMPTZOID:      /* timestamp with time zone */
                return DbType::Timestamp;
            case KCI_INTERVALOID:      /* interval day,interval day to second, \
 							 interval hour,interval minute,interval \
 							 month,interval second,interval year, \
 							 interval year to month */
                return DbType::Interval;
            case KCI_ZPBITOID:      /* bit */
            case KCI_VARBITOID:      /* bitvarying */
                return DbType::Integer32;
            case KCI_XMLOID:      /* xml */
            case KCI_TsqueryOID:      /* tsquery */
            case KCI_tsvectorOID:      /* tsvector */
            case KCI_BLOBOID:      /* blob */
            case KCI_CLOBOID:      /* clob */
                return DbType::Blob;
            default:
                assert(false);
                return DbType::Null;
        }
    }

    bool KingbaseClient::ping() {
        if (_dbMutex.tryLock()) {
            if (_kingbaseDb->kingbaseDb != nullptr &&
                KCIConnectionGetStatus(_kingbaseDb->kingbaseDb) == CONNECTION_OK) {
                int sock = KCIConnectionGetSocket(_kingbaseDb->kingbaseDb);
                _dbMutex.unlock();
                return sock >= 0;
            } else {
                _dbMutex.unlock();
                return false;
            }
        }
        return true;
    }

    String KingbaseClient::getErrorMsg() {
        if (_kingbaseDb->kingbaseDb != nullptr) {
            const char *error = KCIConnectionGetLastError(_kingbaseDb->kingbaseDb);
#ifdef WIN32
            return String::GBKtoUTF8(error);
#else
            return error;
#endif
        } else {
            return String::Empty;
        }
    }

    bool KingbaseClient::isSucceed(int result) {
        return result == EXECUTE_COMMAND_OK || result == EXECUTE_TUPLES_OK;
    }

    void KingbaseClient::printErrorInfo(const String &methodName, const String &sql, const ResultInner *result) {
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
        if (result != nullptr && result->res != nullptr) {
            error = KCIResultGetErrorString(result->res);
        } else {
            error = getErrorMsg();
        }
        DbClient::printErrorInfo(methodName, sql, error);
    }
}