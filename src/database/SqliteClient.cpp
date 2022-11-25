#include <stdlib.h>
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "thread/Locker.h"
#include <iconv.h>
#include "database/SqliteClient.h"
#include "sqlite3.h"

namespace Database
{
    class SqliteInner
    {
    public:
        sqlite3* sqliteDb;
        
        SqliteInner() : sqliteDb(nullptr)
        {
        }
    };
    
	SqliteClient::SqliteClient()
    {
        _sqliteDb = new SqliteInner();
	}

	SqliteClient::~SqliteClient()
	{
        close();
        
        delete _sqliteDb;
	}

	bool SqliteClient::open(const String& connectionStr)
	{
		Locker locker(&_dbMutex);

#if WIN32
		String str = String::GBKtoUTF8(connectionStr);
#else
		String str = connectionStr;
#endif
		// connectionStr is the db file name.
		int result = sqlite3_open(str.c_str(), &_sqliteDb->sqliteDb);
		if (isSuccessed(result))
		{
			// http://blog.quibb.org/2010/08/fast-bulk-inserts-into-sqlite/

#ifdef DEBUG
            int result;
            DataTable table;
            const int length = 8;
            String names[length] = {"PRAGMA synchronous", "PRAGMA count_changes", "PRAGMA journal_mode",
                "PRAGMA temp_store", "PRAGMA locking_mode", "PRAGMA fullfsync", "PRAGMA read_uncommitted", "PRAGMA recursive_triggers"};
            for (int i = 0; i < length; i++)
            {
                table.clear();
                result = executeSqlQueryInner(names[i].c_str(), table);
                if(result == SQLITE_OK && table.rows()->count() > 0)
                {
                    Debug::writeFormatLine("PRAGMA %s = %d",
                                           table.columns()->at(0)->name().c_str(),
                                           table.rows()->at(0)->cells()->at(0)->value().nValue);
                }
            }
#endif

            executeSqlInner("PRAGMA synchronous=OFF");
            executeSqlInner("PRAGMA count_changes=OFF");
            executeSqlInner("PRAGMA journal_mode=MEMORY");
            executeSqlInner("PRAGMA temp_store=MEMORY");
		}
		return isSuccessed(result);
	}

	bool SqliteClient::close()
	{
		Locker locker(&_dbMutex);

		int result = sqlite3_close(_sqliteDb->sqliteDb);
		_sqliteDb->sqliteDb = nullptr;
		return isSuccessed(result);
	}

	bool SqliteClient::executeSql(const String& sql, bool transaction)
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

	bool SqliteClient::beginTransaction()
	{
		Locker locker(&_dbMutex);

		int result = beginTransactionInner();
		return isSuccessed(result);
	}
	bool SqliteClient::commitTransaction()
	{
		Locker locker(&_dbMutex);

		int result = commitTransactionInner();
		return isSuccessed(result);
	}
	bool SqliteClient::rollbackTransaction()
	{
		Locker locker(&_dbMutex);

		int result = rollbackTransactionInner();
		return isSuccessed(result);
	}

	bool SqliteClient::executeSqlQuery(const String& sql, DataTable& table)
	{
		Locker locker(&_dbMutex);

		int result = executeSqlQueryInner(sql, table);
		return isSuccessed(result);
	}
	bool SqliteClient::executeSqlInsert(const DataTable& table, bool replace)
	{
		Locker locker(&_dbMutex);

		int result = executeSqlInsertInner(table, replace);
		return isSuccessed(result);
	}

	int SqliteClient::executeSqlInner(const String& sql)
	{
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
        const char* str = sql.c_str();
#endif
		int result = sqlite3_exec(_sqliteDb->sqliteDb, str, nullptr, nullptr, nullptr);
		if (!isSuccessed(result))
		{
			printErrorInfo("sqlite3_exec", sql);
		}
		return result;
	}
	int SqliteClient::executeSqlInsertInner(const DataTable& table, bool replace)
	{
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
		int columnCount = table.columnCount();
		for (int i = 0; i < columnCount; i++)
		{
			if (i != 0)
			{
				valuesStr += ", ";
			}
			valuesStr += String::convert("?%d", i + 1);
		}
		String sql;
		if (replace)
		{
			sql = String::convert("REPLACE INTO [%s] VALUES (%s)", table.name().c_str(), valuesStr.c_str());
		}
		else
		{
			sql = String::convert("INSERT OR IGNORE INTO [%s] VALUES (%s)", table.name().c_str(), valuesStr.c_str());
		}
		sqlite3_stmt *stmt;
		int result = sqlite3_prepare_v2(_sqliteDb->sqliteDb, sql.c_str(), (int)sql.length(), &stmt, 0);
		if (!isSuccessed(result))
		{
			printErrorInfo("sqlite3_prepare_v2", sql);
			return result;
		}

		result = beginTransactionInner();
		if (!isSuccessed(result))
		{
			return result;
		}
		int rowCount = table.rowCount();
		for (int i = 0; i < rowCount; i++)
		{
			const DataRow* row = table.rows()->at(i);
			for (int j = 0; j < columnCount; j++)
			{
				const DataCell* cell = row->cells()->at(j);
				if (cell != nullptr)
				{
					if (cell->isNullValue())
					{
						sqlite3_bind_null(stmt, j + 1);
					}
					else
					{
						ValueTypes type = cell->type();
						const Value& value = cell->value();
						switch (type)
						{
						case Null:
							sqlite3_bind_null(stmt, j + 1);
							break;
                        case Integer8:
                            sqlite3_bind_int(stmt, j + 1, value.cValue);
                            break;
                        case Integer16:
                            sqlite3_bind_int(stmt, j + 1, value.sValue);
                            break;
						case Integer32:
							sqlite3_bind_int(stmt, j + 1, value.nValue);
							break;
						case Date:
						{
							String str = DateTime(value.tValue).toString();
							sqlite3_bind_text(stmt, j + 1, str.c_str(), (int)str.length(), SQLITE_TRANSIENT);
							break;
						}
						case Timestamp:
						{
							String str = DateTime(value.tValue).toString("yyyy-MM-dd HH:mm:ss.fff");
							sqlite3_bind_text(stmt, j + 1, str.c_str(), (int)str.length(), SQLITE_TRANSIENT);
							break;
						}
						case Text:
						{
							if (value.strValue != nullptr)
							{
#if WIN32
								String str = String::GBKtoUTF8(value.strValue);
#else
                                String str = value.strValue;
#endif
								sqlite3_bind_text(stmt, j + 1, str.c_str(), (int)str.length(), SQLITE_TRANSIENT);
							}
							else
							{
								sqlite3_bind_null(stmt, j + 1);
							}
							break;
						}
						case Float32:
						case Float64:
							sqlite3_bind_double(stmt, j + 1, value.dValue);
							break;
						default:
							assert(false);
							break;
						}
					}
				}
			}

			result = sqlite3_step(stmt);
			if (result != SQLITE_DONE)
			{
				printErrorInfo("sqlite3_step", sql);
			}

			result = sqlite3_reset(stmt);
			if (!isSuccessed(result))
			{
				printErrorInfo("sqlite3_reset", sql);
			}
		}

		result = commitTransactionInner();
		if (!isSuccessed(result))
		{
			return result;
		}
		sqlite3_finalize(stmt);
		if (!isSuccessed(result))
		{
			printErrorInfo("sqlite3_finalize", sql);
			return result;
		}

		return SQLITE_OK;
	}
	int SqliteClient::executeSqlQueryInner(const String& sql, DataTable& table)
	{
#if DEBUG
		String info = String::convert("SqliteClient::executeSqlQueryInner, tabl name: %s", table.name().c_str());
		Stopwatch sw(info, 100);
#endif
		sqlite3_stmt *stmt;
		int result = sqlite3_prepare_v2(_sqliteDb->sqliteDb, sql.c_str(), (int)sql.length(), &stmt, 0);
		if (!isSuccessed(result))
		{
			printErrorInfo("sqlite3_prepare_v2", sql);
			return result;
		}

		if (table.name().isNullOrEmpty())
		{
			const char* name = sqlite3_column_table_name(stmt, 0);
			table.setName(name != nullptr ? name : "temp");
		}

#if DEBUG
		sw.setInfo(String::convert("SqliteClient::executeSqlQueryInner, the table name is '%s'", table.name().c_str()));
#endif

		int columnCount = sqlite3_column_count(stmt);
		int table_columnCount = table.columnCount();
		if ((table_columnCount > 0 && table_columnCount == columnCount) ||
			table_columnCount == 0)
		{
			if (table_columnCount == 0)
			{
				for (int i = 0; i < columnCount; i++)
				{
					char* nameStr = (char*)sqlite3_column_name(stmt, i);
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

					const char* typeStr = sqlite3_column_decltype(stmt, i);
					String type = typeStr != nullptr ? typeStr : "null";
					DataColumn* column = new DataColumn(name, getColumnType(type));
					table.addColumn(column);
				}
			}

			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				DataRow* row = new DataRow();
				for (int i = 0; i < columnCount; i++)
				{
					DataColumn* column = table.columns()->at(i);
					ValueTypes type = column->type();
					const char* str = (const char*)sqlite3_column_text(stmt, i);
					Value value = convertValue(type, str);
					const DataCell* cell = new DataCell(column, value);
					row->addCell(cell);
				}
				table.addRow(row);
			}
		}

		result = sqlite3_finalize(stmt);
		if (!isSuccessed(result))
		{
			printErrorInfo("sqlite3_finalize", sql);
			return result;
		}

		return SQLITE_OK;
	}

	int SqliteClient::beginTransactionInner()
	{
//#if DEBUG
//		Debug::writeLine("sqlite: BEGIN TRANSACTION");
//#endif
		return executeSqlInner("BEGIN TRANSACTION");
	}
	int SqliteClient::commitTransactionInner()
	{
//#if DEBUG
//		Debug::writeLine("sqlite: COMMIT TRANSACTION");
//#endif
		return executeSqlInner("COMMIT TRANSACTION");
	}
	int SqliteClient::rollbackTransactionInner()
	{
//#if DEBUG
//		Debug::writeLine("sqlite: ROLLBACK TRANSACTION");
//#endif
		return executeSqlInner("ROLLBACK TRANSACTION");
	}

	ValueTypes SqliteClient::getColumnType(int type)
	{
		switch (type)
		{
		case SQLITE_TEXT:
			return ValueTypes::Text;
		case SQLITE_INTEGER:
			return ValueTypes::Integer32;
		case SQLITE_FLOAT:
			return ValueTypes::Float32;
		case SQLITE_BLOB:
			return ValueTypes::Blob;
		case SQLITE_NULL:
			return ValueTypes::Null;
		default:
			assert(false);
			return ValueTypes::Null;
		}
	}
	ValueTypes SqliteClient::getColumnType(const String& type)
	{
		ValueTypes valueType = Null;
        String temp = type.toLower();
		if (temp.find("int") >= 0)
		{
			valueType = Integer32;
		}
		else if (temp.find("char") >= 0 ||
			temp.find("varchar") >= 0 ||
			temp.find("nvarchar") >= 0 ||
			temp.find("text") >= 0)
		{
			valueType = Text;
		}
		else if (temp.find("date") >= 0)
		{
			valueType = Date;
		}
		else if (temp.find("timestamp") >= 0)
		{
			valueType = Timestamp;
		}
		else if (temp.find("float") >= 0 ||
			temp.find("number") >= 0)
		{
			valueType = Float32;
		}
		else if (temp.find("null") >= 0)
		{
			valueType = Null;
		}
        else if (temp.find("double") >= 0)
        {
            valueType = Float64;
        }
		else
		{
			assert(false);
		}
		return valueType;
	}

	const String SqliteClient::getErrorMsg()
	{
		return _sqliteDb->sqliteDb != nullptr ? (String)sqlite3_errmsg(_sqliteDb->sqliteDb) : String::Empty;
	}
    
    bool SqliteClient::isSuccessed(int result) const
    {
        return result == SQLITE_OK;
    }
}
