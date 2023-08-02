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
#include "database/SqlConnection.h"
#include "system/ServiceFactory.h"

using namespace Data;
using namespace Database;
using namespace System;
using namespace Net;

namespace Microservice {
    class IDataSourceService : public IService {
    public:
        virtual SqlConnection *connection() = 0;

        // for DEBUG
        virtual void createSqlFile(const String &fileName, const String &sql) = 0;
    };

    class DataSourceService : public IDataSourceService {
    public:
        DataSourceService();

        ~DataSourceService() override;

        bool initialize();

        bool unInitialize();

        SqlConnection *connection() override;

        void createSqlFile(const String &fileName, const String &sql) override;

    private:
        SqlConnection *_connection;
    };
}

#endif /* DataSourceService_h */
