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
        String connectionStr = createConnectionStr(DatasourcePrefix);
        if (connectionStr.isNullOrEmpty()) {
            return false;
        }
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

    String DataSourceService::createConnectionStr(const String &prefix) {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        auto *cs = factory->getService<IConfigService>();
        assert(cs);

        if (prefix.isNullOrEmpty()) {
            return String::Empty;
        }

        String temp = prefix[prefix.length() - 1] == '.' ? prefix.substr(0, prefix.length() - 1) : prefix;
        const char *prefixStr = temp.c_str();

        bool enable = true;
        if (cs->getProperty(String::format("%s.enabled", prefixStr), enable) && !enable) {
            return String::Empty;
        }
        String user;
        if (!cs->getProperty(String::format("%s.username", prefixStr), user))
            return String::Empty;
        if (user.isNullOrEmpty()) {
            Trace::warn("database user name is incorrect.");
            return String::Empty;
        }
        String password;
        if (!cs->getProperty(String::format("%s.password", prefixStr), password)) {
            Trace::warn("database password is incorrect.");
            return String::Empty;
        }
        String encoding;
        cs->getProperty(String::format("%s.encoding", prefixStr), encoding);
        String timeout;
        cs->getProperty(String::format("%s.timeout", prefixStr), timeout);
        int minCount = 0, maxCount = 0;
        cs->getProperty(String::format("%s.pool.minCount", prefixStr), minCount);
        cs->getProperty(String::format("%s.pool.maxCount", prefixStr), maxCount);
        TimeSpan pingCycle, idle;
        cs->getProperty(String::format("%s.pingCycle", prefixStr), pingCycle);
        cs->getProperty(String::format("%s.pool.idle", prefixStr), idle);

        String url = cs->getProperty(String::format("%s.url", prefixStr));
        String schema = cs->getProperty(String::format("%s.schema", prefixStr));
        static const char *fmt = "%s=%s; ";
        String connectionStr;
        connectionStr.appendFormat(fmt, "url", url.c_str());
        connectionStr.appendFormat(fmt, "schema", schema.c_str());
        connectionStr.appendFormat(fmt, "user", user.c_str());
        connectionStr.appendFormat(fmt, "password", password.c_str());
        connectionStr.appendFormat(fmt, "timeout", timeout.c_str());
        connectionStr.appendFormat(fmt, "pingCycle", pingCycle.toString().c_str());
        connectionStr.appendFormat(fmt, "minCount", Int32(minCount).toString().c_str());
        connectionStr.appendFormat(fmt, "maxCount", Int32(maxCount).toString().c_str());
        connectionStr.appendFormat(fmt, "idle", idle.toString().c_str());
        connectionStr.appendFormat(fmt, "encoding", encoding.c_str());
        return connectionStr;
    }
}
