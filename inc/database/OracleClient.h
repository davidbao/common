#ifndef ORACLECLIENT_H
#define ORACLECLIENT_H

#include <stdio.h>
#include "data/PList.h"
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
		uint32_t executeSqlInner(const String& sql);
		uint32_t executeSqlQueryInner(const String& sql, DataTable& table);
		uint32_t executeSqlInsertInner(const DataTable& table);
		uint32_t executeSqlMergeInner(const DataTable& table);

		String toInsertStr(const DataTable& table);
		String toInsertStr(const DataTable& table, const DataRow* row);
		String toMergeStr(const DataTable& table, const DataRow* row);

        bool isSuccessed(uint32_t result) const;
        void printErrorInfo(const String& methodName, uint32_t error, const String& sql = String::Empty);
		String getErrorMsg(uint32_t error);

	private:
        OracleInner* _oracleDb;
	};
}

#endif // ORACLECLIENT_H
