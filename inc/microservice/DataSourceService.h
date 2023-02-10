//
//  DataSourceService.h
//  common
//
//  Created by baowei on 2020/3/9.
//  Copyright © 2020 com. All rights reserved.
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

    private:
        bool openMysql(const Url &url, const String &userName, const String &password);

        bool openSqlite(const String &urlStr);

#ifdef HAS_DB_KINGBASE
        bool openKingbase(const Url &url, const String &userName, const String &password);
#endif

    private:
        DbClient *_dbClient;
    };
}

#endif /* DataSourceService_h */
