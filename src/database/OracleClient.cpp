//
//  OracleClient.cpp
//  common
//
//  Created by baowei on 2018/12/8.
//  Copyright © 2018 com. All rights reserved.
//

#include "database/OracleClient.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include "data/PList.h"
#include "data/Convert.h"
#include "oci/oci.h"

using namespace Diag;

namespace Database {
    class InnerColumn {
    public:
        Value datavalue;    //output variable
        ub4 datasize;
        ub2 datatype;

        InnerColumn() {
            datavalue.strValue = nullptr;
            datasize = 0;
            datatype = 0;
        }

        InnerColumn(ub2 type, ub4 size) {
            datatype = type;
            datasize = size;
            datavalue.strValue = new char[datasize];
            memset(datavalue.strValue, 0, datasize);
        }

        ~InnerColumn() {
            if (datavalue.strValue != nullptr) {
                delete[] datavalue.strValue;
                datavalue.strValue = nullptr;
            }
        }
    };

    typedef PList<InnerColumn> InnerColumns;

    class OracleInner {
    public:
        OCIEnv *env;
        OCIError *error;
        OCISvcCtx *context;

        uint32_t lastError;

        OracleInner() {
            context = nullptr;
            env = nullptr;
            error = nullptr;
            lastError = OCI_SUCCESS;
        }
    };

    OracleClient::OracleClient() {
        _oracleDb = new OracleInner();

        uint32_t result;
        result = OCIEnvCreate(&_oracleDb->env, OCI_THREADED, nullptr, nullptr, nullptr, nullptr, 0, nullptr);
        if (result) {
            _oracleDb->lastError = result;
            printErrorInfo("OCIEnvCreate", result);
        } else {
            result = OCIHandleAlloc(_oracleDb->env, (void **) &_oracleDb->error, OCI_HTYPE_ERROR, 0, nullptr);
            if (result) {
                _oracleDb->lastError = result;
                printErrorInfo("OCIHandleAlloc", result);
            }
        }
    }

    OracleClient::~OracleClient() {
        close();

        OCIHandleFree(_oracleDb->error, OCI_HTYPE_ERROR);
        _oracleDb->error = nullptr;
        OCIHandleFree(_oracleDb->env, OCI_HTYPE_ENV);
        _oracleDb->env = nullptr;

        delete _oracleDb;
    }

    bool OracleClient::open(const String &database, const String &username, const String &password) {
        Locker locker(&_dbMutex);

        uint32_t result = OCILogon(_oracleDb->env, _oracleDb->error, &_oracleDb->context,
                                   (const OraText *) username.c_str(), (ub4) username.length(),
                                   (const OraText *) password.c_str(), (ub4) password.length(),
                                   (const OraText *) database.c_str(), (ub4) database.length());
        if (result) {
            _oracleDb->lastError = result;
            printErrorInfo("OCILogon", result);
        } else {
            result = executeSqlInner("ALTER SESSION SET NLS_DATE_FORMAT = 'yyyy-mm-dd hh24:mi:ss'");
            if (result) {
                _oracleDb->lastError = result;
                printErrorInfo("ALTER SESSION SET NLS_DATE_FORMAT", result);
            }
            result = executeSqlInner("ALTER SESSION SET NLS_TIMESTAMP_FORMAT = 'yyyy-mm-dd hh24:mi:ss.ff3'");
            if (result) {
                _oracleDb->lastError = result;
                printErrorInfo("ALTER SESSION SET NLS_TIMESTAMP_FORMAT", result);
            }
        }
        return isSucceed(result);
    }

    bool OracleClient::open(const String &connectionStr) {
        StringArray texts;
        Convert::splitStr(connectionStr, ';', texts);
        if (texts.count() == 3) {
            return open(texts[0], texts[1], texts[2]);
        }
        return false;
    }

    bool OracleClient::close() {
        Locker locker(&_dbMutex);

        uint32_t result = 0;
        if (_oracleDb->context != nullptr) {
            result = OCILogoff(_oracleDb->context, _oracleDb->error);
            OCIHandleFree(_oracleDb->context, OCI_HTYPE_SVCCTX);
            _oracleDb->context = nullptr;
        }
        _oracleDb->lastError = result;
        return isSucceed(result);
    }

    bool OracleClient::executeSql(const String &sql, bool transaction) {
        Locker locker(&_dbMutex);

        uint32_t result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlInner(sql);
        _oracleDb->lastError = result;
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

    bool OracleClient::executeSqlQuery(const String &sql, DataTable &table) {
        Locker locker(&_dbMutex);

        uint32_t result = executeSqlQueryInner(sql, table);
        _oracleDb->lastError = result;
        return isSucceed(result);
    }

    bool OracleClient::executeSqlInsert(const DataTable &table, bool transaction) {
        Locker locker(&_dbMutex);

        uint32_t result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlInsertInner(table);
        _oracleDb->lastError = result;
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

    bool OracleClient::executeSqlReplace(const DataTable &table, bool transaction) {
        Locker locker(&_dbMutex);

        uint32_t result;
        if (transaction) {
            result = beginTransactionInner();
            if (!isSucceed(result)) {
                return false;
            }
        }
        result = executeSqlMergeInner(table);
        _oracleDb->lastError = result;
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

    bool OracleClient::beginTransaction() {
        Locker locker(&_dbMutex);

        int result = beginTransactionInner();
        return isSucceed(result);
    }

    bool OracleClient::commitTransaction() {
        Locker locker(&_dbMutex);

        int result = commitTransactionInner();
        return isSucceed(result);
    }

    bool OracleClient::rollbackTransaction() {
        Locker locker(&_dbMutex);

        int result = rollbackTransactionInner();
        return isSucceed(result);
    }

    uint32_t OracleClient::beginTransactionInner() {
        return OCITransStart(_oracleDb->context, _oracleDb->error, 3, 0);
    }

    uint32_t OracleClient::commitTransactionInner() {
        return OCITransCommit(_oracleDb->context, _oracleDb->error, 0);
    }

    uint32_t OracleClient::rollbackTransactionInner() {
        return OCITransRollback(_oracleDb->context, _oracleDb->error, 0);
    }

    bool OracleClient::abortExecuting() {
        if (_oracleDb->context != nullptr) {
            uint32_t result = OCIBreak(_oracleDb->context, _oracleDb->error);
            if (result) {
                printErrorInfo("OCIBreak", result);
                return false;
            }
            return true;
        }
        return false;
    }

    ValueTypes OracleClient::getColumnType(int type) {
        switch (type) {
            case SQLT_CHR:
            case SQLT_AFC:
            case SQLT_STR:
                return ValueTypes::Text;
            case SQLT_DAT:
                return ValueTypes::Date;
            case SQLT_INT:
                return ValueTypes::Integer32;
            case SQLT_LNG:
                return ValueTypes::Integer64;
            case SQLT_NUM:
                return ValueTypes::Float64;
            case SQLT_FLT:
                return ValueTypes::Float32;
            case SQLT_TIMESTAMP:
            case SQLT_TIMESTAMP_TZ:
            case SQLT_TIMESTAMP_LTZ:
                return ValueTypes::Timestamp;
            case SQLT_BOL:
                return ValueTypes::Digital;
            default:
                assert(false);
                return ValueTypes::Null;
        }
    }

    String OracleClient::getErrorMsg() {
        return getErrorMsg(_oracleDb->lastError);
    }

    String OracleClient::getErrorMsg(uint32_t error) {
        int result = (int) error;
        uint32_t origin_error = result;
        char error_buffer[1024];
        result = OCIErrorGet(_oracleDb->error, 1, nullptr, &result, (OraText *) error_buffer, sizeof(error_buffer),
                             OCI_HTYPE_ERROR);
        if (!result) {
            return error_buffer;
        } else {
            return String::convert("cannot get error text, original error is %d", origin_error);
        }
    }

    uint32_t OracleClient::executeSqlInner(const String &sql) {
#ifdef DEBUG
        Stopwatch sw("OracleClient::executeSqlInner", 100);
#endif
        OCIStmt *statement;
        uint32_t result = OCIHandleAlloc(_oracleDb->env, (void **) &statement, OCI_HTYPE_STMT, 0, nullptr);
        if (result) {
            printErrorInfo("OCIHandleAlloc", result, sql);
            return result;
        }
        result = OCIStmtPrepare(statement, _oracleDb->error, (const OraText *) sql.c_str(), (ub4) sql.length(),
                                OCI_NTV_SYNTAX, OCI_DEFAULT);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIStmtPrepare", result, sql);
            return result;
        }
        result = OCIStmtExecute(_oracleDb->context, statement, _oracleDb->error, 1, 0, nullptr, nullptr, OCI_DEFAULT);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIStmtExecute", result, sql);
            return result;
        }
        result = OCITransCommit(_oracleDb->context, _oracleDb->error, 0);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCITransCommit", result, sql);
            return result;
        }
        OCIHandleFree(statement, OCI_HTYPE_STMT);
        return OCI_SUCCESS;
    }

    uint32_t OracleClient::executeSqlQueryInner(const String &sql, DataTable &table) {
#ifdef DEBUG
        Stopwatch sw("OracleClient::executeSqlQueryInner", 100);

        if (table.name().isNullOrEmpty())
            table.setName("temp");
        sw.setInfo(String::convert("executeSqlQueryInner, the table name is '%s'", table.name().c_str()));
#endif

        uint32_t result;
        OCIStmt *statement;
        result = OCIHandleAlloc(_oracleDb->env, (void **) &statement, OCI_HTYPE_STMT, 0, nullptr);
        if (result) {
            printErrorInfo("OCIHandleAlloc", result, sql);
            return result;
        }

        result = OCIStmtPrepare(statement, _oracleDb->error, (const OraText *) sql.c_str(), (ub4) sql.length(),
                                OCI_NTV_SYNTAX, OCI_DEFAULT);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIStmtPrepare", result, sql);
            return result;
        }

        result = OCIStmtExecute(_oracleDb->context, statement, _oracleDb->error, 0, 0, nullptr, nullptr, OCI_DEFAULT);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIStmtExecute", result, sql);
            return result;
        }

        int numcols = 0;
        result = OCIAttrGet(statement, OCI_HTYPE_STMT, &numcols, 0, OCI_ATTR_PARAM_COUNT, _oracleDb->error);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIAttrGet", result, sql);
            return result;
        }

        InnerColumns icolumns;
        int table_columnCount = table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == numcols) ||
            table_columnCount == 0) {
            OCIParam *paramhp;
            const ub4 DEFAULT_LONG_SIZE = 32768;
            ub2 datatype = 0;
            ub4 datasize = 0;
            ub4 dispsize = 0;
            text *col_name;
            ub4 col_name_len;
            ub4 charSemantics;
            char name[512];
            for (int i = 0; i < numcols; i++) {
                /* get parameter for column col*/
                result = OCIParamGet(statement, OCI_HTYPE_STMT, _oracleDb->error, (void **) &paramhp, i + 1);
                /* Retrieve the column name attribute */
                result = OCIAttrGet((dvoid *) paramhp, (ub4) OCI_DTYPE_PARAM,
                                    (dvoid **) &col_name, (ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME,
                                    (OCIError *) _oracleDb->error);
                result = OCIAttrGet((dvoid *) paramhp, (ub4) OCI_DTYPE_PARAM,
                                    (dvoid **) &datatype, 0, (ub4) OCI_ATTR_DATA_TYPE, (OCIError *) _oracleDb->error);

                /* Retrieve the length semantics for the column */
                charSemantics = 0;
                OCIAttrGet((dvoid *) paramhp, OCI_DTYPE_PARAM,
                           (dvoid *) &charSemantics, 0, OCI_ATTR_CHAR_USED, (OCIError *) _oracleDb->error);
                if (charSemantics) {
                    /* Retrieve the column width in characters */
                    OCIAttrGet((dvoid *) paramhp, OCI_DTYPE_PARAM,
                               (dvoid *) &datasize, 0, OCI_ATTR_CHAR_SIZE, (OCIError *) _oracleDb->error);
                } else {
                    result = OCIAttrGet((dvoid *) paramhp, (ub4) OCI_DTYPE_PARAM,
                                        (dvoid **) &datasize, 0, (ub4) OCI_ATTR_DATA_SIZE,
                                        (OCIError *) _oracleDb->error);
                }
                if (datasize > DEFAULT_LONG_SIZE || datasize == 0)
                    datasize = DEFAULT_LONG_SIZE;
                /* add one more uint8_t to store the ternimal char of String */
                datasize = datasize + 1;
                result = OCIAttrGet((dvoid *) paramhp, (ub4) OCI_DTYPE_PARAM,
                                    (dvoid **) &dispsize, 0, (ub4) OCI_ATTR_DISP_SIZE, (OCIError *) _oracleDb->error);
                if (dispsize > DEFAULT_LONG_SIZE || dispsize == 0)
                    dispsize = DEFAULT_LONG_SIZE;
                /* add one more uint8_t to store the ternimal char of String */
                dispsize = dispsize + 1;

                bool useDispSize = datatype == SQLT_DAT ||
                                   datatype == SQLT_TIMESTAMP ||
                                   datatype == SQLT_TIMESTAMP_TZ ||
                                   datatype == SQLT_TIMESTAMP_LTZ;
                InnerColumn *icolumn = new InnerColumn(datatype, useDispSize ? dispsize : datasize);
                icolumns.add(icolumn);

                strncpy(name, (const char *) col_name, col_name_len);
                name[col_name_len] = '\0';
                if (table_columnCount == 0) {
                    table.addColumn(DataColumn(name, getColumnType(datatype)));
                }
            }

            OCIDefine *define;
            for (int i = 0; i < numcols; i++) {
                const InnerColumn *icolumn = icolumns[i];
                if (!(icolumn->datatype == SQLT_BLOB || icolumn->datatype == SQLT_CLOB)) {
                    result = OCIDefineByPos(statement, &define, _oracleDb->error, i + 1,
                                            (dvoid *) icolumn->datavalue.strValue, icolumn->datasize, SQLT_STR, nullptr,
                                            nullptr, nullptr, OCI_DEFAULT);
                    if (result) {
                        OCIHandleFree(statement, OCI_HTYPE_STMT);
                        printErrorInfo("OCIDefineByPos", result, sql);
                        return result;
                    }
                }
            }
        }

        while (true) {
            result = OCIStmtFetch2(statement, _oracleDb->error, 1, OCI_DEFAULT, 0, OCI_DEFAULT);
            //result = OCIStmtFetch(statement, _oracleDb->error, 1, 0, OCI_DEFAULT);
            if (result == OCI_NO_DATA) {
                break;
            }
            if (result) {
                //printErrorInfo("OCIStmtFetch2", result, sql);
            }

            DataRow row;
            for (size_t i = 0; i < table.columnCount(); i++) {
                const DataColumn &column = table.columns().at(i);
                const InnerColumn *icolumn = icolumns[i];
                ValueTypes type = column.type();
                DataCell *cell = new DataCell(column, DbValue(type, icolumn->datavalue.strValue));
                row.addCell(DataCell(column, DbValue(type, icolumn->datavalue.strValue)));
            }
            table.addRow(row);
        }

        OCIHandleFree(statement, OCI_HTYPE_STMT);

        return OCI_SUCCESS;
    }

    String OracleClient::toInsertStr(const DataTable &table) {
        const char *insertStr = "INSERT INTO %s(%s) VALUES (%s)";
        uint32_t columnCount = table.columnCount();

        String columsStr;
        String valuesStr;
        for (size_t j = 0; j < columnCount; j++) {
            if (!columsStr.isNullOrEmpty()) {
                columsStr.append(", ");
            }
            columsStr.append(table.columns().at(j).name());

            if (!valuesStr.isNullOrEmpty()) {
                valuesStr.append(", ");
            }
            //valuesStr.append(Convert::convertStr(":%s", table.columns()->at(j)->name().c_str()));
            valuesStr.append(String::convert(":%d", j + 1));
        }
        String sql = String::convert(insertStr, table.name().c_str(),
                                     columsStr.c_str(), valuesStr.c_str());
        return sql;
    }

    String OracleClient::toInsertStr(const DataTable &table, DataRow *row) {
        static const char *insertStr = "INSERT INTO %s(%s) VALUES (%s)";
        size_t columnCount = table.columnCount();

        String columsStr;
        String valuesStr;
        for (size_t j = 0; j < columnCount; j++) {
            const DataCell &cell = row->cells().at(j);
            if (!cell.isNullValue()) {
                if (!columsStr.isNullOrEmpty()) {
                    columsStr.append(", ");
                }
                columsStr.append(table.columns().at(j).name());

                if (!valuesStr.isNullOrEmpty()) {
                    valuesStr.append(", ");
                }
                valuesStr.append(cell.valueStr(true));
            }
        }
        String sql = String::convert(insertStr, table.name().c_str(),
                                     columsStr.c_str(), valuesStr.c_str());
        return sql;
    }

    String OracleClient::toMergeStr(const DataTable &table, DataRow *row) {
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
								INSERT(%s) VALUES(%s)";

        String svaluesStr;
        String uvaluesStr;
        String icolumsStr;
        String ivaluesStr;
        for (size_t j = 0; j < columnCount; j++) {
            const DataCell &cell = row->cells().at(j);
            if (!cell.isNullValue()) {
                String name = table.columns().at(j).name();
                String value = cell.valueStr(true);

                if (!svaluesStr.isNullOrEmpty()) {
                    svaluesStr.append(", ");
                }
                String svalue = String::convert("%s AS %s", value.c_str(), name.c_str());
                svaluesStr.append(svalue);

                if (name != primaryKey) {
                    if (!uvaluesStr.isNullOrEmpty()) {
                        uvaluesStr.append(", ");
                    }
                    String uvalue = String::convert("T1.%s = T2.%s", name.c_str(), name.c_str());
                    uvaluesStr.append(uvalue);
                }

                if (!icolumsStr.isNullOrEmpty()) {
                    icolumsStr.append(", ");
                }
                icolumsStr.append(name.c_str());

                if (!ivaluesStr.isNullOrEmpty()) {
                    ivaluesStr.append(", ");
                }
                String ivalue = String::convert("T2.%s", name.c_str());
                ivaluesStr.append(ivalue);
            }
        }
        String sql = String::convert(mergeStr, table.name().c_str(), svaluesStr.c_str(),
                                     primaryKey.c_str(), primaryKey.c_str(),
                                     uvaluesStr.c_str(), icolumsStr.c_str(), ivaluesStr.c_str());
        return sql;
    }

    uint32_t OracleClient::executeSqlInsertInner(const DataTable &table) {
#ifdef DEBUG
        Stopwatch sw("OracleClient::executeSqlInsertInner", 100);
#endif
        if (table.name().isNullOrEmpty())
            return -1;
        if (table.columnCount() == 0)
            return -2;
        if (table.rowCount() == 0)
            return -3;

        OCIStmt *statement;
        uint32_t result = OCIHandleAlloc(_oracleDb->env, (void **) &statement, OCI_HTYPE_STMT, 0, nullptr);
        if (result) {
            printErrorInfo("OCIHandleAlloc", result);
            return result;
        }

        uint32_t columnCount = table.columnCount();
        uint32_t rowCount = table.rowCount();
        String sql = toInsertStr(table);
        const char *sqlStr = sql.c_str();
        result = OCIStmtPrepare(statement, _oracleDb->error, (const OraText *) sqlStr, (ub4) sql.length(),
                                OCI_NTV_SYNTAX, OCI_DEFAULT);
        if (result) {
            OCIHandleFree(statement, OCI_HTYPE_STMT);
            printErrorInfo("OCIStmtPrepare", result, sqlStr);
            return result;
        }

        const int StrLength = 65535;
        char **buffer = new char *[columnCount];
        ub2 **alenps = new ub2 *[columnCount];
        OCIBind **binds = new OCIBind *[columnCount];
        for (size_t j = 0; j < columnCount; j++) {
            char(*values)[StrLength] = new char[rowCount][StrLength];
            memset(values, 0, rowCount * StrLength);
            buffer[j] = (char *) values;

            ub2 *alenp = new ub2[rowCount];
            alenps[j] = alenp;
            for (size_t i = 0; i < rowCount; i++) {
                const DataRow &row = table.rows().at(i);
                const DataCell &cell = row.cells().at(j);
                String value;
                if (!cell.isNullValue()) {
                    value = !cell.isNullValue() ? cell.valueStr() : String::Empty;
                } else {
                    value = String::Empty;
                }
                strcpy(values[i], value.c_str());
                alenp[i] = value.length();
            }

            result = OCIBindByPos(statement, &binds[j], _oracleDb->error, j + 1, (dvoid *) values[0], StrLength,
                                  SQLT_CHR, 0, alenp, 0, 0, 0, OCI_DEFAULT);
            if (result) {
                printErrorInfo("OCIBindByPos", result, sqlStr);
            }
        }

        result = OCIStmtExecute(_oracleDb->context, statement, _oracleDb->error, rowCount, 0, nullptr, nullptr,
                                OCI_BATCH_ERRORS);
        if (!(result == OCI_SUCCESS || result == OCI_SUCCESS_WITH_INFO)) {
            printErrorInfo("OCIStmtExecute", result, sqlStr);
            goto destroy;
        }
        result = OCITransCommit(_oracleDb->context, _oracleDb->error, 0);
        if (result) {
            printErrorInfo("OCITransCommit", result, sqlStr);
            goto destroy;
        }

        destroy:
        for (uint32_t j = 0; j < columnCount; j++) {
            delete[] buffer[j];
            delete[] alenps[j];
        }
        delete[] binds;
        delete[] buffer;
        delete[] alenps;
        OCIHandleFree(statement, OCI_HTYPE_STMT);
        return result;
    }

    uint32_t OracleClient::executeSqlMergeInner(const DataTable &table) {
#ifdef DEBUG
        Stopwatch sw("OracleClient::executeSqlMergeInner", 100);
#endif
        if (table.name().isNullOrEmpty())
            return -1;
        if (table.columnCount() == 0)
            return -2;
        if (table.rowCount() == 0)
            return -3;
        bool primaryKey = false;
        for (size_t i = 0; i < table.columnCount(); i++) {
            if (table.columns().at(i).primaryKey()) {
                primaryKey = true;
                break;
            }
        }
        if (!primaryKey)
            return -4;

        OCIStmt *statement;
        uint32_t result = OCIHandleAlloc(_oracleDb->env, (void **) &statement, OCI_HTYPE_STMT, 0, nullptr);
        if (result) {
            printErrorInfo("OCIHandleAlloc", result);
            return result;
        }

        const int patchCount = 500;
        uint32_t rowCount = table.rowCount();
        String sql;
        for (size_t i = 0; i < rowCount; i++) {
            const DataRow &row = table.rows().at(i);
            sql = toMergeStr(table, (DataRow*)&row);

            const char *sqlStr = sql.c_str();
            result = OCIStmtPrepare(statement, _oracleDb->error, (const OraText *) sqlStr, (ub4) sql.length(),
                                    OCI_NTV_SYNTAX, OCI_DEFAULT);
            if (result) {
                OCIHandleFree(statement, OCI_HTYPE_STMT);
                printErrorInfo("OCIStmtPrepare", result, sqlStr);
                return result;
            }
            result = OCIStmtExecute(_oracleDb->context, statement, _oracleDb->error, 1, 0, nullptr, nullptr,
                                    OCI_DEFAULT);
            if (result) {
                OCIHandleFree(statement, OCI_HTYPE_STMT);
                printErrorInfo("OCIStmtExecute", result, sqlStr);
                return result;
            }
            if ((i == rowCount - 1) ||
                (i > 0 && i % patchCount == 0)) {
                result = OCITransCommit(_oracleDb->context, _oracleDb->error, OCI_DEFAULT);
                if (result) {
                    OCIHandleFree(statement, OCI_HTYPE_STMT);
                    printErrorInfo("OCITransCommit", result, sqlStr);
                    return result;
                }
            }
        }

        OCIHandleFree(statement, OCI_HTYPE_STMT);
        return OCI_SUCCESS;
    }

    bool OracleClient::isSucceed(uint32_t result) const {
        return result == OCI_SUCCESS;
    }

    void OracleClient::printErrorInfo(const String &methodName, uint32_t error, const String &sql) {
        DbClient::printErrorInfo(methodName, sql, getErrorMsg(error));
    }
}
