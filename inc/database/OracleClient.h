#ifndef ORACLECLIENT_H
#define ORACLECLIENT_H

#include <stdio.h>
#include "data/Vector.h"
#include "DbClient.h"

namespace Database
{
    class OracleInner;
	class OracleClient : public DbClient
	{
	public:
		OracleClient();
		~OracleClient() override;

		bool open(const String& database, const String& username, const String& password);
		bool open(const String& connectionStr) override;
		bool close() override;

		bool executeSql(const String& sql, bool transaction = true) override;
		bool executeSqlQuery(const String& sql, DataTable& table) override;
		bool executeSqlInsert(const DataTable& table, bool replace = false) override;

		const String getErrorMsg() override;

		bool beginTransaction() override;
		bool commitTransaction() override;
		bool rollbackTransaction() override;

		bool abortExecuting();

	protected:
		ValueTypes getColumnType(int type) override;

	private:
		uint executeSqlInner(const String& sql);
		uint executeSqlQueryInner(const String& sql, DataTable& table);
		uint executeSqlInsertInner(const DataTable& table);
		uint executeSqlMergeInner(const DataTable& table);

		String toInsertStr(const DataTable& table);
		String toInsertStr(const DataTable& table, const DataRow* row);
		String toMergeStr(const DataTable& table, const DataRow* row);

        bool isSuccessed(uint result) const;
        void printErrorInfo(const String& methodName, uint error, const String& sql = String::Empty);
		String getErrorMsg(uint error);

	private:
        OracleInner* _oracleDb;
	};
}

#endif // ORACLECLIENT_H
