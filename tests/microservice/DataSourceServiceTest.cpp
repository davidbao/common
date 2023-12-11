//
//  DataSourceServiceTest.cpp
//  common
//
//  Created by baowei on 2023/8/2.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "microservice/DataSourceService.h"
#include "configuration/ConfigService.h"
#include "system/Application.h"

using namespace Microservice;
using namespace Config;

static String _host = "192.166.1.3";
static String _port = "3306";
static String _baseUrl(String::format("mysql://%s:%s", _host.c_str(), _port.c_str()));
static String _database = "DataSourceService_db";
static String _url = _baseUrl + "/" + _database;
static String _schema = "test";
static String _username = "root";
static String _password = "123.com";

class TestConfigService : public IConfigService {
public:
    using IConfigService::getProperty;
    using IConfigService::setProperty;

    TestConfigService() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->addService<IConfigService>(this);
    }

    ~TestConfigService() override {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        factory->removeService<IConfigService>();
    }

    const YmlNode::Properties &properties() const override {
        return _properties;
    }

    bool updateConfigFile(const YmlNode::Properties &properties) override {
        return true;
    }

    bool setProperty(const String &key, const String &value) override {
        _properties.add(key, value);
        return true;
    }

private:
    YmlNode::Properties _properties;
};

void setUp() {
    SqlConnection connection(Url(_baseUrl), _username, _password);
    if (!connection.open()) {
        return;
    }
    connection.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
    if(!connection.executeSql(String::format("CREATE DATABASE %s;", _database.c_str()))) {
        return;
    }
}

void cleanUp() {
    SqlConnection connection(Url(_baseUrl), _username, _password);
    if (connection.open()) {
        connection.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
    }
}

bool testConstructor() {
    {
        DataSourceService ds;
        if (!ds.initialize()) {
            return false;
        }
        SqlConnection *connection = ds.connection();
        if (connection->schema() != _schema) {
            return false;
        }

        int minCount;
        if (!connection->getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 2) {
            return false;
        }

        int maxCount;
        if (!connection->getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 10) {
            return false;
        }

        TimeSpan pingCycle;
        if (!connection->getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(60)) {
            return false;
        }

        TimeSpan idle;
        if (!connection->getProperty("idle", idle)) {
            return false;
        }
        if (idle != TimeSpan::fromSeconds(120)) {
            return false;
        }
    }

    return true;
}

bool parseArguments(const Application &app) {
    const Application::Arguments &arguments = app.arguments();
    String host, userName, password, database, schema;
    Port port;
    if (arguments.contains("help") || arguments.contains("?")) {
        puts("Usage:");
        puts("-?, --help            Display this help and exit.");
        puts("-h, --host=name       Connect to host.");
        puts("-P, --port=#          Port number to use for connection->");
        puts("-s, --schema=name     The database schema.");
        puts("-u, --user=name       User for login if not current user.");
        puts("-p, --password[=name] Password to use when connecting to server.");
        puts("-d, --database=name   Database for connection->");
        return false;
    }

    if(arguments.contains("host") || arguments.contains("h")) {
        host = arguments["host"];
        if (host.isNullOrEmpty()) {
            host = arguments["h"];
        }
    }
    if(arguments.contains("port") || arguments.contains("P")) {
        if (!Port::parse(arguments["port"], port)) {
            Port::parse(arguments["P"], port);
        }
    }
    if(arguments.contains("user") || arguments.contains("u")) {
        userName = arguments["user"];
        if (userName.isNullOrEmpty()) {
            userName = arguments["u"];
        }
    }
    if(arguments.contains("password") || arguments.contains("p")) {
        password = arguments["password"];
        if (password.isNullOrEmpty()) {
            password = arguments["p"];
        }
    }
    if(arguments.contains("database") || arguments.contains("d")) {
        database = arguments["database"];
        if (database.isNullOrEmpty()) {
            database = arguments["d"];
        }
    }
    if(arguments.contains("schema") || arguments.contains("s")) {
        schema = arguments["schema"];
        if (schema.isNullOrEmpty()) {
            schema = arguments["s"];
        }
    }

    _baseUrl = String::format("mysql://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : _host.c_str(),
                              !port.isEmpty() ? port.toString().c_str() : _port.c_str());
    _database = !database.isNullOrEmpty() ? database : _database;
    _schema = schema;
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : _username;
    _password = !password.isNullOrEmpty() ? password : _password;

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -d=connection_db -s=test
int main(int argc, const char *argv[]) {
    Application app(argc, argv);
    if (!parseArguments(app)) {
        return 0;
    }

    setUp();

    static TestConfigService ts;
    ts.setProperty("summer.datasource.enabled", true);
    ts.setProperty("summer.datasource.url", _url);
    ts.setProperty("summer.datasource.schema", _schema);
    ts.setProperty("summer.datasource.username", _username);
    ts.setProperty("summer.datasource.password", _password);
    ts.setProperty("summer.datasource.timeout", "00:00:30");
    ts.setProperty("summer.datasource.pingCycle", "00:01:00");
    ts.setProperty("summer.datasource.pool.minCount", 2);
    ts.setProperty("summer.datasource.pool.maxCount", 10);
    ts.setProperty("summer.datasource.pool.idle", "00:02:00");

    int result = 0;
    if(!testConstructor()) {
        result = 1;
    }

    cleanUp();

    return result;
}