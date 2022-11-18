//
//  MysqlClient.cpp
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright © 2017 com. All rights reserved.
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

namespace Database
{
    class MysqlInner
    {
    public:
        MYSQL* mysqlDb;
        
        static const int MYSQL_OK = 0;
        
        MysqlInner() : mysqlDb(nullptr)
        {
        }
    };
    
	MysqlClient::MysqlClient()
	{
        _mysqlDb = new MysqlInner();
        _mysqlDb->mysqlDb = mysql_init(nullptr);
	}

	MysqlClient::~MysqlClient()
	{
		if (_mysqlDb->mysqlDb != nullptr)
		{
			close();
		}
        delete _mysqlDb;
        _mysqlDb = nullptr;
	}

    bool MysqlClient::open(const String& host, int port, const String& database, const String& username, const String& password)
    {
        Locker locker(&_dbMutex);
        
        if (mysql_real_connect(_mysqlDb->mysqlDb, host.c_str(), username.c_str(),
                               password.c_str(), database.c_str(), port, NULL, CLIENT_MULTI_STATEMENTS))
        {
            int result = mysql_query(_mysqlDb->mysqlDb, "set names 'UTF8'");
            if(!isSuccessed(result))
            {
                printErrorInfo("mysql_query(_mysqlDb->mysqlDb, \"set names 'UTF8'\"");
                return false;
            }
            
            char value = 1;
            mysql_options(_mysqlDb->mysqlDb, MYSQL_OPT_RECONNECT, (char *)&value);
            
//            mysql_autocommit(_mysqlDb->mysqlDb, 1);
            
            return true;
        }
        else
        {
            printErrorInfo("mysql_real_connect");
            
        }
        return false;
    }
    bool MysqlClient::open(const String& connectionStr)
	{
        StringArray texts;
        Convert::splitStr(connectionStr, ';', texts);
        if (texts.count() == 5)
        {
            int port = 3306;
            Int32::parse(texts[1], port);
            return open(texts[0], port, texts[2], texts[3], texts[4]);
        }
        return false;
    }

	bool MysqlClient::close()
	{
		Locker locker(&_dbMutex);

		mysql_close(_mysqlDb->mysqlDb);
		_mysqlDb->mysqlDb = nullptr;
        return true;
	}

	bool MysqlClient::executeSql(const String& sql, bool transaction)
	{
		Locker locker(&_dbMutex);

		int result;
		if (transaction)
		{
			result = beginTransactionInner();
			if (!isSuccessed(result))
			{
				return false;
			}
		}
		result = executeSqlInner(sql);
		if (!isSuccessed(result))
		{
			if (transaction)
			{
				rollbackTransactionInner();
			}
			return false;
		}
		if (transaction)
		{
			result = commitTransactionInner();
			if (!isSuccessed(result))
			{
				return false;
			}
		}
		return true;
	}
    bool MysqlClient::executeSql(const String& sql, DataTable& table, bool transaction)
    {
        Locker locker(&_dbMutex);

        int result;
        if (transaction)
        {
            result = beginTransactionInner();
            if (!isSuccessed(result))
            {
                return false;
            }
        }
        result = executeSqlInner(sql, table);
        if (!isSuccessed(result))
        {
            if (transaction)
            {
                rollbackTransactionInner();
            }
            return false;
        }
        if (transaction)
        {
            result = commitTransactionInner();
            if (!isSuccessed(result))
            {
                return false;
            }
        }
        return true;
    }

	bool MysqlClient::beginTransaction()
	{
		Locker locker(&_dbMutex);

		int result = beginTransactionInner();
		return isSuccessed(result);
	}
	bool MysqlClient::commitTransaction()
	{
		Locker locker(&_dbMutex);

		int result = commitTransactionInner();
		return isSuccessed(result);
	}
	bool MysqlClient::rollbackTransaction()
	{
		Locker locker(&_dbMutex);

		int result = rollbackTransactionInner();
		return isSuccessed(result);
	}

	bool MysqlClient::executeSqlQuery(const String& sql, DataTable& table)
	{
		Locker locker(&_dbMutex);

		int result = executeSqlQueryInner(sql, table);
		return isSuccessed(result);
	}
	bool MysqlClient::executeSqlInsert(const DataTable& table, bool replace)
	{
		Locker locker(&_dbMutex);

		int result = executeSqlInsertInner(table, replace);
		return isSuccessed(result);
	}

	int MysqlClient::executeSqlInner(const String& sql)
	{
#if DEBUG
        size_t len = sql.length();
		if (len > 32) len = 32;
		String info = String::convert("MysqlClient::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
		Stopwatch sw(info, 500);
#endif

        mysql_ping(_mysqlDb->mysqlDb);
		int result = mysql_query(_mysqlDb->mysqlDb, sql.c_str());
		if (!isSuccessed(result))
		{
			printErrorInfo("mysql_query", sql);
            return result;
		}
        
        int status;
        /* process each statement result */
        do {
            /* did current statement return data? */
            MYSQL_RES* result = mysql_store_result(_mysqlDb->mysqlDb);
            if (result)
            {
                /* yes; process rows and free the result set */
                mysql_free_result(result);
            }
            else          /* no result set or error */
            {
                if (mysql_field_count(_mysqlDb->mysqlDb) == 0)
                {
//                    printf("%lld rows affected\n",
//                           mysql_affected_rows(_mysqlDb->mysqlDb));
                }
                else  /* some error occurred */
                {
//                    printf("Could not retrieve result set\n");
                    break;
                }
            }
            /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
            if ((status = mysql_next_result(_mysqlDb->mysqlDb)) > 0)
            {
//                printf("Could not execute statement\n");
            }
        } while (status == 0);
        
		return 0;
	}
    int MysqlClient::executeSqlInner(const String& sql, DataTable& table)
    {
#if DEBUG
        size_t len = sql.length();
        if (len > 32) len = 32;
        String info = String::convert("MysqlClient::executeSqlInner, sql: %s", sql.substr(0, len).c_str());
        Stopwatch sw(info, 500);
#endif

        mysql_ping(_mysqlDb->mysqlDb);
        int result = mysql_query(_mysqlDb->mysqlDb, sql.c_str());
        if (!isSuccessed(result))
        {
            printErrorInfo("mysql_query", sql);
            return result;
        }
        
        int status;
        /* process each statement result */
        do {
            /* did current statement return data? */
            MYSQL_RES* result = mysql_store_result(_mysqlDb->mysqlDb);
            if (result)
            {
                /* yes; process rows and free the result set */
                updateDataTable(result, table);
                mysql_free_result(result);
            }
            else          /* no result set or error */
            {
                if (mysql_field_count(_mysqlDb->mysqlDb) == 0)
                {
//                    printf("%lld rows affected\n",
//                           mysql_affected_rows(_mysqlDb->mysqlDb));
                }
                else  /* some error occurred */
                {
//                    printf("Could not retrieve result set\n");
                    break;
                }
            }
            /* more results? -1 = no, >0 = error, 0 = yes (keep looping) */
            if ((status = mysql_next_result(_mysqlDb->mysqlDb)) > 0)
            {
//                printf("Could not execute statement\n");
            }
        } while (status == 0);
        
        return 0;
    }
	int MysqlClient::executeSqlInsertInner(const DataTable& table, bool replace)
	{
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
        for (uint32_t j = 0; j < table.columnCount(); j++)
        {
            if (!columsStr.isNullOrEmpty())
            {
                columsStr.append(", ");
            }
            columsStr.append(table.columns()->at(j)->name());
        }
        
        const DataRows* rows = table.rows();
        for (uint32_t i = 0; i < rows->count(); i++)
        {
            const DataRow* row = rows->at(i);
            
            if (!valuesStr.isNullOrEmpty())
            {
                valuesStr.append(", ");
            }
            valuesStr.append('(');
            for (uint32_t j = 0; j < row->cells()->count(); j++)
            {
                const DataCell* cell = row->cells()->at(j);
                
                if(j != 0)
                {
                    valuesStr.append(",");
                }
                valuesStr.append(cell->valueStr(true));
            }
            valuesStr.append(')');
        }

        String sql;
        if (replace)
        {
            sql = String::convert("REPLACE INTO %s(%s) VALUES %s", table.name().c_str(), columsStr.c_str(), valuesStr.c_str());
        }
        else
        {
            sql = String::convert("INSERT IGNORE INTO %s(%s) VALUES %s", table.name().c_str(), columsStr.c_str(), valuesStr.c_str());
        }

#ifdef DEBUG
        createSqlFile("mysql_insert.sql", sql);
#endif
        
        int result;
        result = beginTransactionInner();
        if (!isSuccessed(result))
        {
            return result;
        }
        result = executeSqlInner(sql);
        if (!isSuccessed(result))
        {
            rollbackTransactionInner();
            return result;
        }
        result = commitTransactionInner();
        if (!isSuccessed(result))
        {
            return result;
        }
        
        return MysqlInner::MYSQL_OK;
    }
    int MysqlClient::executeSqlQueryInner(const String& sql, DataTable& table)
    {
#if DEBUG
        String info = String::convert("MysqlClient::executeSqlQueryInner, table name: %s", table.name().c_str());
		Stopwatch sw(info, 100);
#endif
        
        mysql_ping(_mysqlDb->mysqlDb);
        int result = mysql_query(_mysqlDb->mysqlDb, sql.c_str());
		if (!isSuccessed(result))
		{
			printErrorInfo("mysql_query", sql);
			return result;
		}
        
        MYSQL_RES* result_set = mysql_store_result(_mysqlDb->mysqlDb);
        if(result_set == nullptr)
            return -1;

		updateDataTable(result_set, table);

		mysql_free_result(result_set);

		return MysqlInner::MYSQL_OK;
	}

    void MysqlClient::updateDataTable(void* tag, DataTable& table)
    {
        MYSQL_RES* result = (MYSQL_RES*)tag;
#if DEBUG
        String info = String::convert("MysqlClient::updateDataTable, table name: %s", table.name().c_str());
        Stopwatch sw(info, 100);
#endif
        
        if (table.name().isNullOrEmpty())
        {
            const char* name = result->field_count > 0 ? result->fields[0].table : "temp";
            table.setName(name);
        }

#if DEBUG
        sw.setInfo(String::convert("MysqlClient::updateDataTable, the table name is '%s'", table.name().c_str()));
#endif

        int columnCount = mysql_num_fields(result);
        int table_columnCount = table.columnCount();
        if ((table_columnCount > 0 && table_columnCount == columnCount) ||
            table_columnCount == 0)
        {
            if (table_columnCount == 0)
            {
                for (int i = 0; i < columnCount; i++)
                {
                    MYSQL_FIELD *fd = mysql_fetch_field(result);
                    char* nameStr = fd->name;
                    String name;
                    if (nameStr != nullptr)
                    {
                        name = nameStr;
                    }
                    else
                    {
                        char temp[32];
                        sprintf(temp, "tempCol%d", i);
                        name = temp;
                    }
                    
                    DataColumn* column = new DataColumn(name, getColumnType(fd->type));
                    table.addColumn(column);
                }
            }

            MYSQL_ROW sql_row;
            while ((sql_row = mysql_fetch_row(result)))
            {
                DataRow* row = new DataRow();
                for (int i = 0; i < columnCount; i++)
                {
                    DataColumn* column = table.columns()->at(i);
                    ValueTypes type = column->type();
                    const char* str = (const char*)sql_row[i];
                    Value value = convertValue(type, str);
                    DataCell* cell = new DataCell(column, value);
                    row->addCell(cell);
                }
                table.addRow(row);
            }
        }
    }

	int MysqlClient::beginTransactionInner()
	{
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "START TRANSACTION");     // mysql_autocommit(_mysqlDb->mysqlDb, 0);
	}
	int MysqlClient::commitTransactionInner()
	{
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "COMMIT");                // mysql_commit(_mysqlDb->mysqlDb);
	}
	int MysqlClient::rollbackTransactionInner()
	{
        mysql_ping(_mysqlDb->mysqlDb);
        return mysql_query(_mysqlDb->mysqlDb, "ROLLBACK");              // mysql_rollback(_mysqlDb->mysqlDb);
	}

	ValueTypes MysqlClient::getColumnType(int type)
	{
		switch (type)
		{
		case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_VAR_STRING:
        case MYSQL_TYPE_STRING:
			return ValueTypes::Text;
		case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_INT24:
			return ValueTypes::Integer32;
        case MYSQL_TYPE_LONGLONG:
            return ValueTypes::Integer64;
        case MYSQL_TYPE_FLOAT:
			return ValueTypes::Float32;
        case MYSQL_TYPE_DOUBLE:
        case MYSQL_TYPE_DECIMAL:
            return ValueTypes::Float64;
		case MYSQL_TYPE_BLOB:
			return ValueTypes::Text;
		case MYSQL_TYPE_NULL:
			return ValueTypes::Null;
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
            return ValueTypes::Date;
        case MYSQL_TYPE_TIMESTAMP:
            return ValueTypes::Timestamp;
        case MYSQL_TYPE_BIT:
            return ValueTypes::Digital;
        case MYSQL_TYPE_NEWDECIMAL:
            return ValueTypes::Integer64;
		default:
			assert(false);
			return ValueTypes::Null;
		}
	}

	const String MysqlClient::getErrorMsg()
	{
		return _mysqlDb->mysqlDb != nullptr ? (String)mysql_error(_mysqlDb->mysqlDb) : String::Empty;
	}
    
    bool MysqlClient::isSuccessed(int result) const
    {
        return result == MysqlInner::MYSQL_OK;
    }
    void MysqlClient::printErrorInfo(const String& methodName, const String& sql)
    {
#ifdef DEBUG
        String path = Path::combine(Application::instance()->rootPath(), "errorSql");
        if(!Directory::exists(path))
            Directory::createDirectory(path);
        String fileName = Path::combine(path, String::convert("%s.sql", Uuid::generate().toString().c_str()));
        FileStream fs(fileName.c_str(), FileMode::FileCreate, FileAccess::FileWrite);
        fs.writeText(sql);
        fs.close();
#endif
        DbClient::printErrorInfo(methodName, sql, getErrorMsg());
    }
}
