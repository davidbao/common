//
//  DataSourceService.h
//  common
//
//  Created by baowei on 2020/3/9.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef DataSourceService_h
#define DataSourceService_h

#include "data/ValueType.h"
#include "net/NetType.h"
#include "database/DbClient.h"
#include "system/ServiceFactory.h"

using namespace Common;
using namespace Database;
using namespace Net;

namespace Microservice
{
    class IDataSourceService : public IService
    {
    public:
        virtual DbClient* dbClient() const = 0;
        
        // for DEBUG
        virtual void createSqlFile(const String& fileName, const String& sql) = 0;
    };

    class DataSourceService : public IDataSourceService
    {
    public:
        DataSourceService();
        ~DataSourceService() override;
        
        bool initialize();
        bool unInitialize();
        
        DbClient* dbClient() const override;
        
        void createSqlFile(const String& fileName, const String& sql) override;
        
    private:
        bool openMysql(const Url& url, const String& userName, const String& password);
        bool openSqlite(const String& fullFileName);
                
    private:
        DbClient* _dbClient;
    };
}

#endif /* DataSourceService_h */
