//
//  DataSourceService.cpp
//  common
//
//  Created by baowei on 2020/3/9.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/DataSourceService.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "database/SqlConnection.h"
#include "configuration/ConfigService.h"
#include "system/Application.h"

using namespace Config;

namespace Microservice {
    DataSourceService::DataSourceService() : _connection(100) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IDataSourceService>(this);
    }

    DataSourceService::~DataSourceService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IDataSourceService>();
    }

    bool DataSourceService::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto *cs = factory->getService<IConfigService>();
        assert(cs);

        bool enable = true;
        if (cs->getProperty("summer.datasource.enabled", enable) && !enable) {
            return false;
        }
        String user;
        if (!cs->getProperty("summer.datasource.username", user))
            return false;
        if (user.isNullOrEmpty()) {
            Trace::warn("database user name is incorrect.");
            return false;
        }
        String password;
        if (!cs->getProperty("summer.datasource.password", password)) {
            Trace::warn("database password is incorrect.");
            return false;
        }
        String timeout;
        cs->getProperty("summer.datasource.timeout", timeout);

        String url = cs->getProperty("summer.datasource.url");
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", url.c_str());
        connectionStr.appendFormat(fmt, "user", user.c_str());
        connectionStr.appendFormat(fmt, "password", password.c_str());
        connectionStr.appendFormat(fmt, "timeout", timeout.c_str());
        return _connection.open(connectionStr);
    }

    bool DataSourceService::unInitialize() {
        _connection.close();
        return true;
    }

    void DataSourceService::createSqlFile(const String &fileName, const String &sql) {
        Application *app = Application::instance();
        assert(app);
        String sqlPath = Path::combine(app->rootPath(), "infoSql");
        if (!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

    SqlConnection *DataSourceService::connection() {
        return &_connection;
    }
}
