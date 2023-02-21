//
//  DataSourceService.h
//  common
//
//  Created by baowei on 2020/3/9.
//  Copyright (c) 2020 com. All rights reserved.
//

#ifndef DataSourceService_h
#define DataSourceService_h

#include "data/String.h"
#include "net/NetType.h"
#include "database/DbClient.h"
#include "system/ServiceFactory.h"

using namespace Data;
using namespace Database;
using namespace System;
using namespace Net;

namespace Microservice {
    class IDataSourceService : public IService {
    public:
        virtual DbClient *dbClient() const = 0;

        // for DEBUG
        virtual void createSqlFile(const String &fileName, const String &sql) = 0;
    };

    class DataSourceService : public IDataSourceService {
    public:
        DataSourceService();

        ~DataSourceService() override;

        bool initialize();

        bool unInitialize();

        DbClient *dbClient() const override;

        void createSqlFile(const String &fileName, const String &sql) override;

    public:
        static DbClient *open(const String &urlStr, const String &userName, const String &password);

    private:
        static DbClient *openMysql(const Url &url, const String &userName, const String &password);

        static DbClient *openSqlite(const String &urlStr);

#ifdef HAS_DB_KINGBASE
        static DbClient * openKingbase(const Url &url, const String &userName, const String &password);
#endif

    private:
        DbClient *_dbClient;
    };
}

#endif /* DataSourceService_h */
