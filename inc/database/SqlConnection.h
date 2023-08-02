//
//  SqlConnection.h
//  common
//
//  Created by baowei on 2023/7/21.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef SqlConnection_h
#define SqlConnection_h

#include "data/String.h"
#include "database/DbClient.h"
#include "thread/Timer.h"

using namespace Data;
using namespace Threading;

namespace Database {
    class SqlConnection {
    public:
        SqlConnection();

        explicit SqlConnection(const String &connectionStr);

        SqlConnection(const Url &url, const String &user, const String &password);

        SqlConnection(std::initializer_list<KeyValuePair<String, String>> list);

        ~SqlConnection();

        bool open();

        bool isConnected();

        bool close();

        bool executeSql(const String &sql, bool transaction = true);

        bool executeSqlQuery(const String &sql, DataTable &table);

        bool executeSqlInsert(const DataTable &table, bool transaction = true);

        bool executeSqlReplace(const DataTable &table, bool transaction = true);

        StringArray getColumnName(const String &tableName);

        bool retrieveCount(const String &sql, int &count);

        bool beginTransaction();

        bool commitTransaction();

        bool rollbackTransaction();

        template<class T>
        bool getProperty(const String &name, T &value) const {
            return _connections.at(name, value);
        }

        Url url() const;

        size_t numberOfConnections();

    private:
        void init(const String &connectionStr);

        DbClient *openInner(const StringMap &connections);

        DbClient *getClient();

        bool hasPing() const;

        void timerProc();

    private:
        class DataSource : public IMutex {
        public:
            DbClient *client;
            uint64_t openedTick;
            uint64_t executedTick;

            explicit DataSource(DbClient *client = nullptr);

            ~DataSource() override;

            void update();

            bool ping() const;

            bool reopen(const StringMap &connections);

            bool close();

            void lock() override;

            bool tryLock() override;

            void unlock() override;

        private:
            Mutex _mutex;
        };
        typedef PList<DataSource> DataSources;

        DataSources _dataSources;

        StringMap _connections;

        Timer* _pingTimer;
    };
}

#endif // SqlConnection_h
