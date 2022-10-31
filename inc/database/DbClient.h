#ifndef DBCLIENT_H
#define DBCLIENT_H

#include <stdio.h>
#include "thread/Mutex.h"
#include "data/ValueType.h"
#include "DataTable.h"

using namespace Common;

namespace Database
{
	class DbClient
	{
	public:
		DbClient();
		virtual ~DbClient();

		virtual bool open(const String& connectionStr) = 0;
		virtual bool close() = 0;

		virtual bool executeSql(const String& sql, bool transaction = true) = 0;
        virtual bool executeSql(const String& sql, DataTable& table, bool transaction = true);
		virtual bool executeSqlQuery(const String& sql, DataTable& table) = 0;
		virtual bool executeSqlInsert(const DataTable& table, bool replace = false) = 0;

		virtual const String getErrorMsg() = 0;

		virtual bool beginTransaction() = 0;
		virtual bool commitTransaction() = 0;
		virtual bool rollbackTransaction() = 0;
        
        bool retriveCount(const String& sql, int& count);

	protected:
		virtual ValueTypes getColumnType(int type) = 0;

		Value convertValue(const ValueTypes type, const char* str);

	protected:
		void printErrorInfo(const String& methodName, const String& sql = String::Empty, const String& error = String::Empty);
        
#ifdef DEBUG
        void createSqlFile(const String& fileName, const String& sql) const;
#endif

	protected:
		Mutex _dbMutex;
	};
}

#endif // DBCLIENT_H
