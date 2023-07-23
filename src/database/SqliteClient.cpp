//
//  SqliteClient.cpp
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright (c) 2016 com. All rights reserved.
//

#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "database/SqliteClient.h"
#include "IO/FileInfo.h"
#include "sqlite3.h"

using namespace Diag;

namespace Database {
    class SqliteInner {
    public:
        sqlite3 *sqliteDb;

        SqliteInner() : sqliteDb(nullptr) {
        }
    };

    SqliteClient::SqliteClient() {
        _sqliteDb = new SqliteInner();
    }

    SqliteClient::~SqliteClient() {
        SqliteClient::close();

        delete _sqliteDb;
    }

    bool SqliteClient::open(const StringMap &connections) {
        Locker locker(&_dbMutex);

        return openInner(connections);
    }

    bool SqliteClient::open(const String &fileName) {
        bool error = false;
        String errorStr;
        FileInfo fi(fileName);
        if (!fi.exists() || fi.isWritable()) {
            bool result = openInner(fileName);
            if (!result) {
                error = true;
                errorStr = getErrorMsg();
            }
        } else {
            error = true;
            errorStr = String::convert("the file '%s' is readonly.", fileName.c_str());
        }

        if (error) {
            Trace::debug(String::format("Failed to open sqlite. file: %s, reason: '%s'",
                                        fileName.c_str(), errorStr.c_str()));
        } else {
            Trace::debug(String::format("Open sqlite successfully. file: %s", fileName.c_str()));
        }
        return !error;
    }

    bool SqliteClient::openInner(const StringMap &connections) {
        String file = connections["file"];
        return open(file);
    }

    bool SqliteClient::openInner(const String &fileName) {
#if WIN32
        String str = String::GBKtoUTF8(fileName);
#else
        String str = fileName;
#endif
        // connectionStr is the db file name.
        int result = sqlite3_open(str.c_str(), &_sqliteDb->sqliteDb);
        if (isSucceed(result)) {
            // http://blog.quibb.org/2010/08/fast-bulk-inserts-into-sqlite/

#ifdef DEBUG
            DataTable table;
            const int length = 8;
            String names[length] = {"PRAGMA synchronous", "PRAGMA count_changes", "PRAGMA journal_mode",
                                    "PRAGMA temp_store", "PRAGMA locking_mode", "PRAGMA fullfsync",
                                    "PRAGMA read_uncommitted", "PRAGMA recursive_triggers"};
            for (const auto &name: names) {
                table.clear();
                result = executeSqlQueryInner(name.c_str(), table);
                if (result == SQLITE_OK && table.rows().count() > 0) {
                    Debug::writeFormatLine("PRAGMA %s = %d",
                                           table.columns().at(0).name().c_str(),
                                           (int) table.rows().at(0).cells().at(0).value());
                }
            }
#endif

            executeSqlInner("PRAGMA synchronous=OFF");
            executeSqlInner("PRAGMA count_changes=OFF");
            executeSqlInner("PRAGMA journal_mode=MEMORY");
            executeSqlInner("PRAGMA temp_store=MEMORY");
            return true;
        }
        return false;
    }

    bool SqliteClient::close() {
        Locker locker(&_dbMutex);

        int result = sqlite3_close(_sqliteDb->sqliteDb);
        _sqliteDb->sqliteDb = nullptr;
        return isSucceed(result);
    }

    bool SqliteClient::executeSql(const String &sql, bool transaction) {
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

    bool SqliteClient::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool SqliteClient::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool SqliteClient::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    bool SqliteClient::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        int result = executeSqlQueryInner(sql, table);
        return isSucceed(result);
    }

    bool SqliteClient::executeSqlInsert(const DataTable &table, bool transaction) {
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

    bool SqliteClient::executeSqlReplace(const DataTable &table, bool transaction) {
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

    int SqliteClient::executeSqlInner(const String &sql) {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("SqliteClient::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif
#if WIN32
        String temp = String::GBKtoUTF8(sql);
        const char* str = temp.c_str();
#else
        const char *str = sql.c_str();
#endif
        int result = sqlite3_exec(_sqliteDb->sqliteDb, str, nullptr, nullptr, nullptr);
        if (!isSucceed(result)) {
            printErrorInfo("sqlite3_exec", sql);
        }
        return result;
    }

    int SqliteClient::executeSqlInsertInner(const DataTable &table, bool replace) {
#if DEBUG
        String info = String::convert("SqliteClient::executeSqlInsertInner, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif
        if (table.name().isNullOrEmpty())
            return -1;
        if (table.columnCount() == 0)
            return -2;
        if (table.rowCount() == 0)
            return -3;

        // such like '"INSERT INTO example VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7)";'
        String valuesStr;
        int columnCount = (int) table.columnCount();
        for (int i = 0; i < columnCount; i++) {
            if (i != 0) {
                valuesStr += ", ";
            }
            valuesStr += String::convert("?%d", i + 1);
        }
        String sql;
        if (replace) {
            sql = String::convert("REPLACE INTO [%s] VALUES (%s)", table.name().c_str(), valuesStr.c_str());
        } else {
            sql = String::convert("INSERT OR IGNORE INTO [%s] VALUES (%s)", table.name().c_str(), valuesStr.c_str());
        }
        sqlite3_stmt *stmt;
        int result = sqlite3_prepare_v2(_sqliteDb->sqliteDb, sql.c_str(), (int) sql.length(), &stmt, nullptr);
        if (!isSucceed(result)) {
            printErrorInfo("sqlite3_prepare_v2", sql);
            return result;
        }

        int rowCount = (int) table.rowCount();
        for (int i = 0; i < rowCount; i++) {
            const DataRow &row = table.rows().at(i);
            for (int j = 0; j < columnCount; j++) {
                const DataCell &cell = row.cells().at(j);
                if (cell.isNullValue()) {
                    sqlite3_bind_null(stmt, j + 1);
                } else {
                    DbType type = cell.type();
                    const DbValue &value = cell.value();
                    switch (type) {
                        case DbValue::Null:
                            sqlite3_bind_null(stmt, j + 1);
                            break;
                        case DbValue::Integer8:
                        case DbValue::Integer16:
                        case DbValue::Integer32:
                            sqlite3_bind_int(stmt, j + 1, (int32_t) value);
                            break;
                        case DbValue::UInteger8:
                        case DbValue::UInteger16:
                        case DbValue::UInteger32:
                            sqlite3_bind_int(stmt, j + 1, (uint32_t) value);
                            break;
                        case DbValue::Integer64:
                        case DbValue::UInteger64:
                            sqlite3_bind_int64(stmt, j + 1, (int64_t) value);
                            break;
                        case DbValue::Date: {
                            DateTime time = value;
                            String str = time.toString();
                            sqlite3_bind_text(stmt, j + 1, str.c_str(), (int) str.length(), SQLITE_TRANSIENT);
                            break;
                        }
                        case DbValue::Timestamp: {
                            TimeSpan time = value;
                            String str = time.toString("yyyy-MM-dd HH:mm:ss.fff");
                            sqlite3_bind_text(stmt, j + 1, str.c_str(), (int) str.length(), SQLITE_TRANSIENT);
                            break;
                        }
                        case DbValue::Text: {
                            if (!value.isNullValue()) {
                                String str;
                                value.getValue(str);
#if WIN32
                                str = String::GBKtoUTF8(str);
#endif
                                sqlite3_bind_text(stmt, j + 1, str.c_str(), (int) str.length(), SQLITE_TRANSIENT);
                            } else {
                                sqlite3_bind_null(stmt, j + 1);
                            }
                            break;
                        }
                        case DbValue::Float32:
                        case DbValue::Float64:
                            sqlite3_bind_double(stmt, j + 1, value);
                            break;
                        case DbValue::Blob: {
                            ByteArray buffer = value;
                            sqlite3_bind_blob(stmt, j + 1, buffer.data(), (int) buffer.count(), SQLITE_TRANSIENT);
                        }
                            break;
                        default:
                            assert(false);
                            break;
                    }
                }
            }

            result = sqlite3_step(stmt);
            if (result != SQLITE_DONE) {
                printErrorInfo("sqlite3_step", sql);
            }

            result = sqlite3_reset(stmt);
            if (!isSucceed(result)) {
                printErrorInfo("sqlite3_reset", sql);
            }
        }

        sqlite3_finalize(stmt);
        if (!isSucceed(result)) {
            printErrorInfo("sqlite3_finalize", sql);
            return result;
        }

        return SQLITE_OK;
    }

    int SqliteClient::executeSqlQueryInner(const String &sql, DataTable &table) {
#if DEBUG
        String info = String::convert("SqliteClient::executeSqlQueryInner, tabl name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif
        sqlite3_stmt *stmt;
        int result = sqlite3_prepare_v2(_sqliteDb->sqliteDb, sql.c_str(), (int) sql.length(), &stmt, nullptr);
        if (!isSucceed(result)) {
            printErrorInfo("sqlite3_prepare_v2", sql);
            return result;
        }

        if (table.name().isNullOrEmpty()) {
            table.setName("temp");
        }

#if DEBUG
        sw.setInfo(String::convert("SqliteClient::executeSqlQueryInner, the table name is '%s'", table.name().c_str()));
#endif

        int columnCount = sqlite3_column_count(stmt);
        int table_columnCount = (int) table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0) {
            if (table_columnCount == 0) {
                for (int i = 0; i < columnCount; i++) {
                    const char *nameStr = sqlite3_column_name(stmt, i);
                    String name;
                    if (nameStr != nullptr) {
                        name = nameStr;
                    } else {
                        char temp[32];
                        sprintf(temp, "tempCol%d", i);
                        name = temp;
                    }

                    const char *typeStr = sqlite3_column_decltype(stmt, i);
                    String type = typeStr != nullptr ? typeStr : "text";
                    table.addColumn(DataColumn(name, getColumnType(type)));
                }
            }

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                DataRow row;
                for (int i = 0; i < columnCount; i++) {
                    const DataColumn &column = table.columns().at(i);
                    DbType type = column.type();
                    const char *str = (const char *) sqlite3_column_text(stmt, i);
                    row.addCell(DataCell(column, DbValue(type, str)));
                }
                table.addRow(row);
            }
        }

        result = sqlite3_finalize(stmt);
        if (!isSucceed(result)) {
            printErrorInfo("sqlite3_finalize", sql);
            return result;
        }

        return SQLITE_OK;
    }

    int SqliteClient::beginTransactionInner() {
//#if DEBUG
//		Debug::writeLine("sqlite: BEGIN TRANSACTION");
//#endif
        return executeSqlInner("BEGIN TRANSACTION");
    }

    int SqliteClient::commitTransactionInner() {
//#if DEBUG
//		Debug::writeLine("sqlite: COMMIT TRANSACTION");
//#endif
        return executeSqlInner("COMMIT TRANSACTION");
    }

    int SqliteClient::rollbackTransactionInner() {
//#if DEBUG
//		Debug::writeLine("sqlite: ROLLBACK TRANSACTION");
//#endif
        return executeSqlInner("ROLLBACK TRANSACTION");
    }

    StringArray SqliteClient::getColumnName(const String &tableName) {
        StringArray result;
        if (!tableName.isNullOrEmpty()) {
            String sql = String::format("SELECT name FROM PRAGMA_TABLE_INFO('%s')",
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

    DbType SqliteClient::getColumnType(int type) {
        switch (type) {
            case SQLITE_TEXT:
                return DbType::Text;
            case SQLITE_INTEGER:
                return DbType::Integer32;
            case SQLITE_FLOAT:
                return DbType::Float32;
            case SQLITE_BLOB:
                return DbType::Blob;
            case SQLITE_NULL:
                return DbType::Null;
            default:
                assert(false);
                return DbType::Null;
        }
    }

    bool SqliteClient::ping() {
        return true;
    }

    DbType SqliteClient::getColumnType(const String &type) {
        DbType valueType = DbValue::Null;
        String temp = type.toLower();
        if (temp.find("int") >= 0) {
            valueType = DbValue::Integer32;
        } else if (temp.find("char") >= 0 ||
                   temp.find("varchar") >= 0 ||
                   temp.find("nvarchar") >= 0 ||
                   temp.find("text") >= 0) {
            valueType = DbValue::Text;
        } else if (temp.find("date") >= 0) {
            valueType = DbValue::Date;
        } else if (temp.find("time") >= 0) {
            valueType = DbValue::Time;
        } else if (temp.find("timestamp") >= 0) {
            valueType = DbValue::Timestamp;
        } else if (temp.find("timespan") >= 0) {
            valueType = DbValue::Interval;
        } else if (temp.find("float") >= 0 ||
                   temp.find("number") >= 0) {
            valueType = DbValue::Float32;
        } else if (temp.find("null") >= 0) {
            valueType = DbValue::Null;
        } else if (temp.find("double") >= 0 ||
                   temp.find("real") >= 0) {
            valueType = DbValue::Float64;
        } else {
            assert(false);
        }
        return valueType;
    }

    String SqliteClient::getErrorMsg() {
        return _sqliteDb->sqliteDb != nullptr ? (String) sqlite3_errmsg(_sqliteDb->sqliteDb) : String::Empty;
    }

    bool SqliteClient::isSucceed(int result) {
        return result == SQLITE_OK;
    }
}
