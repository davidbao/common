//
//  SqlConnectionTest.cpp
//  common
//
//  Created by baowei on 2023/7/22.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/SqlConnection.h"
#include "system/Application.h"
#include "system/Environment.h"

using namespace Database;
using namespace System;

static String _host = "192.166.1.3";
static String _port = "3306";
static String _baseUrl(String::format("mysql://%s:%s", _host.c_str(), _port.c_str()));
static String _database = "SqlConnectionTest_db";
static String _url = _baseUrl + "/" + _database;
static String _username = "root";
static String _password = "123.com";

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
        SqlConnection connection;
        int minCount;
        if (!connection.getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 1) {
            return false;
        }

        int maxCount;
        if (!connection.getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 5) {
            return false;
        }

        TimeSpan pingCycle;
        if (!connection.getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(30)) {
            return false;
        }
    }

    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        SqlConnection connection(connectionStr);
        if (connection.url() != Url(_url)) {
            return false;
        }
        String user;
        if (!connection.getProperty("user", user)) {
            return false;
        }
        if (user != _username) {
            return false;
        }
        String password;
        if (!connection.getProperty("password", password)) {
            return false;
        }
        if (password != _password) {
            return false;
        }
    }
    {
        SqlConnection connection("minCount=0");
        int minCount;
        if (!connection.getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 1) {
            return false;
        }
    }
    {
        SqlConnection connection("minCount=6");
        int minCount;
        if (!connection.getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 6) {
            return false;
        }
    }
    {
        SqlConnection connection("minCount=15");
        int minCount;
        if (!connection.getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 1) {
            return false;
        }
    }

    {
        SqlConnection connection("maxCount=0");
        int maxCount;
        if (!connection.getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 5) {
            return false;
        }
    }
    {
        SqlConnection connection("maxCount=6");
        int maxCount;
        if (!connection.getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 6) {
            return false;
        }
    }
    {
        SqlConnection connection("maxCount=55");
        int maxCount;
        if (!connection.getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 5) {
            return false;
        }
    }

    {
        SqlConnection connection("pingCycle=00:00:50");
        TimeSpan pingCycle;
        if (!connection.getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(50)) {
            return false;
        }
    }
    {
        SqlConnection connection("pingCycle=00:00:00");
        TimeSpan pingCycle;
        if (!connection.getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(30)) {
            return false;
        }
    }
    {
        SqlConnection connection("pingCycle=01:00:00");
        TimeSpan pingCycle;
        if (!connection.getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(30)) {
            return false;
        }
    }

    {
        SqlConnection connection{
                {"minCount", "5"},
                {"maxCount", "10"},
                {"pingCycle", "00:01:00"},
        };
        int minCount;
        if (!connection.getProperty("minCount", minCount)) {
            return false;
        }
        if (minCount != 5) {
            return false;
        }
        int maxCount;
        if (!connection.getProperty("maxCount", maxCount)) {
            return false;
        }
        if (maxCount != 10) {
            return false;
        }
        TimeSpan pingCycle;
        if (!connection.getProperty("pingCycle", pingCycle)) {
            return false;
        }
        if (pingCycle != TimeSpan::fromSeconds(60)) {
            return false;
        }
    }

    return true;
}

bool testOpen() {
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }
    }
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "ds", Url(_url).scheme().c_str());
        connectionStr.appendFormat(fmt, "host", _host.c_str());
        connectionStr.appendFormat(fmt, "port", _port.c_str());
        connectionStr.appendFormat(fmt, "dbname", _database.c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }
    }
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "timeout", "5");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }
    }

    {
        uint64_t start = Environment::getTickCount();
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", "mysql://192.168.100.244:1000");
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "timeout", "2");
        SqlConnection connection(connectionStr);
        if (connection.open()) {
            return false;
        }
        uint64_t end = Environment::getTickCount();
        uint64_t elapsed = end - start;
        if (!(elapsed >= 2000 && elapsed <= 3000)) {
            return false;
        }
    }

    return true;
}

bool testCreateDatabase() {
    {
        SqlConnection connection(Url(_baseUrl), _username, _password);
        if (!connection.open()) {
            return false;
        }
        connection.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
        if(!connection.executeSql(String::format("CREATE DATABASE %s;", _database.c_str()))) {
            return false;
        }
    }

    return true;
}

bool testCreateTable() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        DataTable table;
        if (!connection.executeSqlQuery("select * from t_student;", table)) {
            return false;
        }

        connection.executeSql("drop table t_student;");
    }

    return true;
}

bool testInsertRecord() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        if (!connection.executeSql("INSERT INTO t_student (\n"
                             "            id,\n"
                             "                    name,\n"
                             "                    score\n"
                             "        )VALUES(1,'Xu',86);")) {
            return false;
        }
        DataTable table;
        if (!connection.executeSqlQuery("select * from t_student;", table)) {
            return false;
        }
        if (table.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table.rows().at(0).cells().at("name").value();
        if (value != "Xu") {
            return false;
        }

        connection.executeSql("drop table t_student;");
    }

    return true;
}

bool testInsertRecordByTable() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        DataTable table("t_student");
        table.addColumns({
                                 DataColumn("id", DbType::Integer32, true),
                                 DataColumn("name", DbType::Text, false),
                                 DataColumn("score", DbType::Float64, false),
                         });
        table.addRows({
                              DataRow({
                                              DataCell(table.columns()[0], 1),
                                              DataCell(table.columns()[1], "Xu"),
                                              DataCell(table.columns()[2], 86),
                                      })
                      });
        if (!connection.executeSqlInsert(table)) {
            return false;
        }

        DataTable table2;
        if (!connection.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table2.rows().at(0).cells().at("name").value();
        if (value != "Xu") {
            return false;
        }
    }

    return true;
}

bool testReplaceRecordByTable() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (connection.executeSql("create table t_student(\n"
                            "id int primary key not null,\n"
                            "name text not null,\n"
                            "score real\n"
                            ");")) {
            return false;
        }
        DataTable table("t_student");
        table.addColumns({
                                 DataColumn("id", DbType::Integer32, true),
                                 DataColumn("name", DbType::Text, false),
                                 DataColumn("score", DbType::Float64, false),
                         });
        table.addRows({
                              DataRow({
                                              DataCell(table.columns()[0], 1),
                                              DataCell(table.columns()[1], "Hu"),
                                              DataCell(table.columns()[2], 86),
                                      })
                      });
        if (!connection.executeSqlReplace(table)) {
            return false;
        }

        DataTable table2;
        if (!connection.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table2.rows().at(0).cells().at("name").value();
        if (value != "Hu") {
            return false;
        }

        connection.executeSql("drop table t_student;");
    }

    return true;
}

bool testRetrieveCount() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        DataTable table("t_student");
        table.addColumns({
                                 DataColumn("id", DbType::Integer32, true),
                                 DataColumn("name", DbType::Text, false),
                                 DataColumn("score", DbType::Float64, false),
                         });
        table.addRows({
                              DataRow({
                                              DataCell(table.columns()[0], 1),
                                              DataCell(table.columns()[1], "Xu"),
                                              DataCell(table.columns()[2], 86),
                                      })
                      });
        if (!connection.executeSqlInsert(table)) {
            return false;
        }

        int count;
        if(!connection.retrieveCount("select count(*) t_student", count)) {
            return false;
        }
        if(count != 1) {
            return false;
        }

        connection.executeSql("drop table t_student;");
    }

    return true;
}

bool testTransaction() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        connection.beginTransaction();
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");", false)) {
            return false;
        }
        connection.commitTransaction();

        connection.beginTransaction();
        DataTable table("t_student");
        table.addColumns({
                                 DataColumn("id", DbType::Integer32, true),
                                 DataColumn("name", DbType::Text, false),
                                 DataColumn("score", DbType::Float64, false),
                         });
        table.addRows({
                              DataRow({
                                              DataCell(table.columns()[0], 1),
                                              DataCell(table.columns()[1], "Xu"),
                                              DataCell(table.columns()[2], 86),
                                      })
                      });
        if (!connection.executeSqlInsert(table, false)) {
            return false;
        }
        connection.commitTransaction();

        DataTable table2;
        if (!connection.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table2.rows().at(0).cells().at("name").value();
        if (value != "Xu") {
            return false;
        }

        connection.executeSql("drop table t_student;");
    }

    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        connection.beginTransaction();
        if (!connection.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");", false)) {
            return false;
        }
        connection.commitTransaction();

        connection.beginTransaction();
        DataTable table("t_student");
        table.addColumns({
                                 DataColumn("id", DbType::Integer32, true),
                                 DataColumn("name", DbType::Text, false),
                                 DataColumn("score", DbType::Float64, false),
                         });
        table.addRows({
                              DataRow({
                                              DataCell(table.columns()[0], 1),
                                              DataCell(table.columns()[1], "Xu"),
                                              DataCell(table.columns()[2], 86),
                                      })
                      });
        if (!connection.executeSqlInsert(table, false)) {
            return false;
        }
        connection.rollbackTransaction();

        DataTable table2;
        if (!connection.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 0) {
            return false;
        }
    }

    return true;
}

bool testGetColumnNames() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.executeSql("DROP TABLE IF EXISTS t_student;"
                             "create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }

        StringArray names = connection.getColumnName("t_student");
        if (names.count() != 3) {
            return false;
        }
        if (!(names[0] == "id" && names[1] == "name" && names[2] == "score")) {
            return false;
        }
    }

    return true;
}

bool testProperties() {
    {
        SqlConnection connection(_url, _username, _password);
        if (!connection.open()) {
            return false;
        }
        if (!connection.isConnected()) {
            return false;
        }
    }
    {
        SqlConnection connection(_url, _username, _password);
        if (connection.url() != Url(_url)) {
            return false;
        }
    }

    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "minConnectionCount", "2");
        connectionStr.appendFormat(fmt, "maxConnectionCount", "10");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }
        if (connection.numberOfConnections() != 2) {
            return false;
        }
    }
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "minConnectionCount", "2");
        connectionStr.appendFormat(fmt, "maxConnectionCount", "10");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }

        auto runFunc = [](SqlConnection *connection) {
            connection->executeSql("select sleep(2);");
        };
        Thread thread1("sqlConnection.thread1", runFunc, &connection);
        thread1.start();
        Thread thread2("sqlConnection.thread2", runFunc, &connection);
        thread2.start();
        Thread::msleep(100);
        connection.executeSql("select now();");
        if (connection.numberOfConnections() != 3) {
            return false;
        }
    }

    return true;
}

bool testPool() {
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "minConnectionCount", "2");
        connectionStr.appendFormat(fmt, "maxConnectionCount", "10");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }

        auto runFunc = [](SqlConnection *connection) {
            connection->executeSql("select sleep(2);");
        };
        Thread thread("sqlConnection.thread", runFunc, &connection);
        thread.start();
        uint64_t start = Environment::getTickCount();
        connection.executeSql("select now();");
        uint64_t end = Environment::getTickCount();
        if (end - start > 1000) {
            return false;
        }
    }

    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "minConnectionCount", "2");
        connectionStr.appendFormat(fmt, "maxConnectionCount", "10");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }

        auto runFunc = [](SqlConnection *connection) {
            connection->executeSql("select sleep(2);");
        };
        Thread thread1("sqlConnection.thread1", runFunc, &connection);
        thread1.start();
        Thread thread2("sqlConnection.thread2", runFunc, &connection);
        thread2.start();
        uint64_t start = Environment::getTickCount();
        connection.executeSql("select now();");
        uint64_t end = Environment::getTickCount();
        if (end - start > 1000) {
            return false;
        }
    }

    return true;
}

bool testIdle() {
    {
        String connectionStr;
        static const char *fmt = "%s=%s; ";
        connectionStr.appendFormat(fmt, "url", _url.toString().c_str());
        connectionStr.appendFormat(fmt, "user", _username.c_str());
        connectionStr.appendFormat(fmt, "password", _password.c_str());
        connectionStr.appendFormat(fmt, "minConnectionCount", "2");
        connectionStr.appendFormat(fmt, "maxConnectionCount", "10");
        connectionStr.appendFormat(fmt, "idle", "00:01:00");
        SqlConnection connection(connectionStr);
        if (!connection.open()) {
            return false;
        }

        auto runFunc = [](SqlConnection *connection) {
            connection->executeSql("select sleep(2);");
        };
        Thread thread1("sqlConnection.thread1", runFunc, &connection);
        thread1.start();
        Thread thread2("sqlConnection.thread2", runFunc, &connection);
        thread2.start();
        Thread thread3("sqlConnection.thread3", runFunc, &connection);
        thread3.start();
        Thread::msleep(100);
        connection.executeSql("select now();");
        if (connection.numberOfConnections() != 4) {
            return false;
        }
        Thread::msleep(61 * 1000);
        if (connection.numberOfConnections() != 2) {
            return false;
        }
    }

    return true;
}

bool parseArguments(const Application &app) {
    const Application::Arguments &arguments = app.arguments();
    String host, userName, password, database;
    Port port;
    if (arguments.contains("help") || arguments.contains("?")) {
        puts("Usage:");
        puts("-?, --help            Display this help and exit.");
        puts("-h, --host=name       Connect to host.");
        puts("-P, --port=#          Port number to use for connection.");
        puts("-u, --user=name       User for login if not current user.");
        puts("-p, --password[=name] Password to use when connecting to server.");
        puts("-d, --database=name   Database for connection.");
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

    _baseUrl = String::format("mysql://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : _host.c_str(),
                              !port.isEmpty() ? port.toString().c_str() : _port.c_str());
    _database = !database.isNullOrEmpty() ? database : _database;
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : _username;
    _password = !password.isNullOrEmpty() ? password : _password;

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -d=connection_db
int main(int argc, const char *argv[]) {
    Application app(argc, argv);
    if (!parseArguments(app)) {
        return 0;
    }

    setUp();

    int result = 0;
    if (!testConstructor()) {
        result = 1;
    }
    if (!testOpen()) {
        result = 2;
    }
    if (!testCreateDatabase()) {
        result = 11;
    }
    if (!testCreateTable()) {
        result = 12;
    }
    if (!testInsertRecord()) {
        result = 13;
    }
    if (!testInsertRecordByTable()) {
        result = 14;
    }
    if (!testReplaceRecordByTable()) {
        result = 15;
    }
    if (!testRetrieveCount()) {
        result = 16;
    }
    if (!testTransaction()) {
        result = 17;
    }
    if(!testGetColumnNames()) {
        result = 18;
    }
    if(!testProperties()) {
        result = 19;
    }
    if(!testPool()) {
        result = 20;
    }

    // testIdle took too long, so it was commented out.
//    if(!testIdle()) {
//        result = 21;
//    }

    cleanUp();

    return result;
}