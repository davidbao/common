#ifndef SQLITECLIENT_H
#define SQLITECLIENT_H

#include <stdio.h>
#include "thread/Mutex.h"
#include "system/Convert.h"
#include "data/PList.h"
#include "DbClient.h"

namespace Database
{
    class SqliteInner;
	class SqliteClient : public DbClient
	{
	public:
		SqliteClient();
		~SqliteClient() override;

		bool open(const String& connectionStr) override;
		bool close() override;

		bool executeSql(const String& sql, bool transaction = true) override;
		bool executeSqlQuery(const String& sql, DataTable& table) override;
		bool executeSqlInsert(const DataTable& table, bool replace = false) override;

		const String getErrorMsg() override;

		bool beginTransaction() override;
		bool commitTransaction() override;
		bool rollbackTransaction() override;

	protected:
		ValueTypes getColumnType(int type) override;
		ValueTypes getColumnType(const String& type);

	private:
		int executeSqlInner(const String& sql);
		int executeSqlInsertInner(const DataTable& table, bool replace = false);
		int executeSqlQueryInner(const String& sql, DataTable& table);

		int beginTransactionInner();
		int commitTransactionInner();
		int rollbackTransactionInner();

        bool isSuccessed(int result) const;

	private:
		SqliteInner* _sqliteDb;
	};
}
#endif // SQLITECLIENT_H
