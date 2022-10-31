//
//  DataSourceService.cpp
//  common
//
//  Created by baowei on 2020/3/9.
//  Copyright © 2020 com. All rights reserved.
//

#include "microservice/DataSourceService.h"
#include "diag/Trace.h"
#include "diag/Stopwatch.h"
#include "IO/Path.h"
#include "IO/FileInfo.h"
#include "IO/Directory.h"
#include "IO/FileStream.h"
#include "database/MysqlClient.h"
#include "database/SqliteClient.h"
#include "configuration/ConfigService.h"
#include "microservice/SummerApplication.h"

namespace Microservice
{
    DataSourceService::DataSourceService() : _dbClient(nullptr)
    {
        ServiceFactory* factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IDataSourceService>(this);
    }

    DataSourceService::~DataSourceService()
    {
        ServiceFactory* factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IDataSourceService>();
    }

    bool DataSourceService::initialize()
    {
        ServiceFactory* factory = ServiceFactory::instance();
        assert(factory);
        IConfigService* cs = factory->getService<IConfigService>();
        assert(cs);
        
        bool enable = true;
        if(cs->getProperty("summer.datasource.enabled", enable) && !enable)
        {
            return false;
        }
        String userName;
        if(!cs->getProperty("summer.datasource.username", userName))
            return false;
        if(userName.isNullOrEmpty())
        {
            Trace::warn("database user name is incorrect.");
            return false;
        }
        String password;
        if(!cs->getProperty("summer.datasource.password", password))
        {
            Trace::warn("database password is incorrect.");
            return false;
        }
        Url url;
        if(!cs->getProperty("summer.datasource.url", url))
        {
            Trace::warn("database url is incorrect.");
            return false;
        }
        if(url.scheme() == "mysql" || url.scheme() == "mysqls")
        {
            bool result = openMysql(url, userName, password);
            if(result)
            {
                return true;
            }
            else
            {
                Application::instance()->exit(-2);
                return false;
            }
        }

        return false;
    }
    bool DataSourceService::unInitialize()
    {
        if (_dbClient != nullptr)
        {
            _dbClient->close();
            delete _dbClient;
            _dbClient = nullptr;
        }
        return true;
    }

    bool DataSourceService::openMysql(const Url& url, const String& userName, const String& password)
    {
        String host = url.address();
        int port = url.port();
        String database = url.relativeUrl();
        MysqlClient* client = new MysqlClient();
        if(client->open(host, port, database, userName, password))
        {
            Trace::debug(String::format("Open mysql successfully. host: %s, port: %d, database: %s, user name: %s",
                                   host.c_str(), port, database.c_str(), userName.c_str()));
            
            _dbClient = client;
            
            DataTable table;
            if(_dbClient->executeSqlQuery(" select version();", table))
            {
                if(table.rows()->count() > 0)
                {
                    const DataCells* cells = table.rows()->at(0)->cells();
                    if(cells->count() > 0)
                    {
                        Version mysqlVersion;
                        if(!cells->at(0)->isNullValue() &&
                           Version::parse(cells->at(0)->value().strValue, mysqlVersion))
                        {
                            if(mysqlVersion >= "8.0")
                            {
                                _dbClient->executeSql("set @@sql_mode='STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';");
                            }
                            else if(mysqlVersion >= "5.7.2")
                            {
                                // https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html#sqlmode_only_full_group_by
                                // Reject queries for which the select list, HAVING condition, or ORDER BY list refer to nonaggregated columns that are neither named in the GROUP BYclause nor are functionally dependent on (uniquely determined by) GROUP BY columns.
                                // As of MySQL 5.7.5, the default SQL mode includes ONLY_FULL_GROUP_BY. (Before 5.7.5, MySQL does not detect functional dependency andONLY_FULL_GROUP_BY is not enabled by default. For a description of pre-5.7.5 behavior, see the MySQL 5.6 Reference Manual.)
                                _dbClient->executeSql("set @@sql_mode='STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION';");
                            }
                        }
                    }
                }
            }
            return true;
        }
        else
        {
            Trace::debug(String::format("Failed to open mysql. host: %s, port: %d, database: %s, user name: %s, reason: '%s'",
                                   host.c_str(), port, database.c_str(), userName.c_str(),
                                   client->getErrorMsg().c_str()));
            delete client;
            return false;
        }
    }

    bool DataSourceService::openSqlite(const String& fullFileName)
    {
#if DEBUG
        Stopwatch sw("Create or open database file");
#endif
        String filename = Path::getFileName(fullFileName);
        
        bool error = false;
        String errorStr;
        FileInfo fi(fullFileName);
        if (!fi.exists() || fi.isWritable())
        {
            _dbClient = new SqliteClient();
            bool result = _dbClient->open(fullFileName);
            if (!result)
            {
                error = true;
                errorStr = _dbClient->getErrorMsg();
            }
        }
        else
        {
            error = true;
            errorStr = String::convert("the file '%s' is readonly.", filename.c_str());
        }
        
        if (error)
        {
            // can not open the db file, so exit.
            Trace::debug(String::format("Can not open the db file, reason: %s", errorStr.c_str()));
            return false;
        }
        
        return true;
    }

    void DataSourceService::createSqlFile(const String& fileName, const String& sql)
    {
        Application* app = Application::instance();
        assert(app);
        String sqlPath = Path::combine(app->rootPath(), "sql");
        if(!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

    DbClient* DataSourceService::dbClient() const
    {
        return _dbClient;
    }
}
