//
//  MysqlClient.h
//  common
//
//  Created by baowei on 2017/2/27.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef MysqlClient_h
#define MysqlClient_h

#include "thread/Mutex.h"
#include "data/Vector.h"
#include "DbClient.h"

namespace Database
{
    class MysqlInner;
	class MysqlClient : public DbClient
	{
	public:
		MysqlClient();
		~MysqlClient() override;

        bool open(const String& host, int port, const String& database, const String& username, const String& password);
		bool open(const String& connectionStr) override;
		bool close() override;

		bool executeSql(const String& sql, bool transaction = true) override;
        bool executeSql(const String& sql, DataTable& table, bool transaction = true) override;
		bool executeSqlQuery(const String& sql, DataTable& table) override;
		bool executeSqlInsert(const DataTable& table, bool replace = false) override;

		const String getErrorMsg() override;

		bool beginTransaction() override;
		bool commitTransaction() override;
		bool rollbackTransaction() override;

	protected:
		ValueTypes getColumnType(int type) override;

	private:
		int executeSqlInner(const String& sql);
        int executeSqlInner(const String& sql, DataTable& table);
		int executeSqlInsertInner(const DataTable& table, bool replace = false);
		int executeSqlQueryInner(const String& sql, DataTable& table);
        
        void updateDataTable(void* tag, DataTable& table);

		int beginTransactionInner();
		int commitTransactionInner();
		int rollbackTransactionInner();

        bool isSuccessed(int result) const;
        void printErrorInfo(const String& methodName, const String& sql = String::Empty);
        
	private:
        MysqlInner* _mysqlDb;
	};
}

#endif // MysqlClient_h
