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
    DataSourceService::DataSourceService() : _connection(nullptr) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IDataSourceService>(this);
    }

    DataSourceService::~DataSourceService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IDataSourceService>();

        delete _connection;
        _connection = nullptr;
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
        String encoding;
        cs->getProperty("summer.datasource.encoding", encoding);
        String timeout;
        cs->getProperty("summer.datasource.timeout", timeout);
        int minCount = 0, maxCount = 0;
        cs->getProperty("summer.datasource.pool.minCount", minCount);
        cs->getProperty("summer.datasource.pool.maxCount", maxCount);
        TimeSpan pingCycle, idle;
        cs->getProperty("summer.datasource.pingCycle", pingCycle);
        cs->getProperty("summer.datasource.pool.idle", idle);

        String url = cs->getProperty("summer.datasource.url");
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", url.c_str());
        connectionStr.appendFormat(fmt, "user", user.c_str());
        connectionStr.appendFormat(fmt, "password", password.c_str());
        connectionStr.appendFormat(fmt, "timeout", timeout.c_str());
        connectionStr.appendFormat(fmt, "pingCycle", pingCycle.toString().c_str());
        connectionStr.appendFormat(fmt, "minCount", Int32(minCount).toString().c_str());
        connectionStr.appendFormat(fmt, "maxCount", Int32(maxCount).toString().c_str());
        connectionStr.appendFormat(fmt, "idle", idle.toString().c_str());
        connectionStr.appendFormat(fmt, "encoding", encoding.c_str());
        _connection = new SqlConnection(connectionStr);
        return _connection->open();
    }

    bool DataSourceService::unInitialize() {
        if (_connection != nullptr) {
            _connection->close();
            return true;
        }
        return false;
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
        return _connection;
    }
}
