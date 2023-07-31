//
//  MysqlClientTest.cpp
//  common
//
//  Created by baowei on 2022/11/24.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "database/MysqlClient.h"
#include "IO/Path.h"
#include "system/Application.h"
#include "system/Environment.h"

using namespace Database;
using namespace System;

static String _host = "192.166.1.3";
static String _port = "3306";
static String _baseUrl(String::format("mysql://%s:%s", _host.c_str(), _port.c_str()));
static String _database = "MysqlClientTest_db";
static String _url = _baseUrl + "/" + _database;
static String _username = "root";
static String _password = "123.com";

void setUp() {
    MysqlClient test;
    if (!test.open(Url(_baseUrl), _username, _password)) {
        return;
    }
    test.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
    if (!test.executeSql(String::format("CREATE DATABASE %s;", _database.c_str()))) {
        return;
    }
}

void cleanUp() {
    MysqlClient test;
    if (test.open(Url(_baseUrl), _username, _password)) {
        test.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
    }
}

bool testOpen() {
    {
        StringMap connections;
        connections.add("host", _host.c_str());
        connections.add("port", _port.c_str());
        connections.add("dbname", _database.c_str());
        connections.add("user", _username.c_str());
        connections.add("password", _password.c_str());
        MysqlClient test;
        if (!test.open(connections)) {
            return false;
        }
    }
    {
        StringMap connections;
        connections.add("host", _host.c_str());
        connections.add("port", _port.c_str());
        connections.add("dbname", _database.c_str());
        connections.add("user", _username.c_str());
        connections.add("password", _password.c_str());
        connections.add("timeout", "5");
        MysqlClient test;
        if (!test.open(connections)) {
            return false;
        }
    }

    {
        uint64_t start = Environment::getTickCount();
        StringMap connections;
        connections.add("host", "192.168.100.244");
        connections.add("port", 100);
        connections.add("dbname", _database.c_str());
        connections.add("user", _username.c_str());
        connections.add("password", _password.c_str());
        connections.add("timeout", "2");
        MysqlClient test;
        if (test.open(connections)) {
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
        MysqlClient test;
        if (!test.open(Url(_baseUrl), _username, _password)) {
            return false;
        }
        test.executeSql(String::format("DROP DATABASE IF EXISTS %s;", _database.c_str()));
        if (!test.executeSql(String::format("CREATE DATABASE %s;", _database.c_str()))) {
            return false;
        }
    }

    return true;
}

bool testCreateTable() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        DataTable table;
        if (!test.executeSqlQuery("select * from t_student;", table)) {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    return true;
}

bool testInsertRecord() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }
        if (!test.executeSql("INSERT INTO t_student (\n"
                             "            id,\n"
                             "                    name,\n"
                             "                    score\n"
                             "        )VALUES(1,'Xu',86);")) {
            return false;
        }
        DataTable table;
        if (!test.executeSqlQuery("select * from t_student;", table)) {
            return false;
        }
        if (table.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table.rows().at(0).cells().at("name").value();
        if (value != "Xu") {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    return true;
}

bool testInsertRecordByTable() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.executeSql("create table t_student(\n"
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
        if (!test.executeSqlInsert(table)) {
            return false;
        }

        DataTable table2;
        if (!test.executeSqlQuery("select * from t_student;", table2)) {
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
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (test.executeSql("create table t_student(\n"
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
        if (!test.executeSqlReplace(table)) {
            return false;
        }

        DataTable table2;
        if (!test.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table2.rows().at(0).cells().at("name").value();
        if (value != "Hu") {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    return true;
}

bool testRetrieveCount() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.executeSql("create table t_student(\n"
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
        if (!test.executeSqlInsert(table)) {
            return false;
        }

        int count;
        if (!test.retrieveCount("select count(*) t_student", count)) {
            return false;
        }
        if (count != 1) {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    return true;
}

bool testTransaction() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.beginTransaction();
        if (!test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");", false)) {
            return false;
        }
        test.commitTransaction();

        test.beginTransaction();
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
        if (!test.executeSqlInsert(table, false)) {
            return false;
        }
        test.commitTransaction();

        DataTable table2;
        if (!test.executeSqlQuery("select * from t_student;", table2)) {
            return false;
        }
        if (table2.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table2.rows().at(0).cells().at("name").value();
        if (value != "Xu") {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.beginTransaction();
        if (!test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");", false)) {
            return false;
        }
        test.commitTransaction();

        test.beginTransaction();
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
        if (!test.executeSqlInsert(table, false)) {
            return false;
        }
        test.rollbackTransaction();

        DataTable table2;
        if (!test.executeSqlQuery("select * from t_student;", table2)) {
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
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.executeSql("DROP TABLE IF EXISTS t_student;"
                             "create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }

        StringArray names = test.getColumnName("t_student");
        if (names.count() != 3) {
            return false;
        }
        if (!(names[0] == "id" && names[1] == "name" && names[2] == "score")) {
            return false;
        }
    }

    return true;
}

bool testIsConnected() {
    {
        MysqlClient test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        if (!test.isConnected()) {
            return false;
        }
    }
    {
        StringMap connections;
        connections.add("host", "192.168.100.244");
        connections.add("port", 100);
        connections.add("dbname", _database.c_str());
        connections.add("user", _username.c_str());
        connections.add("password", _password.c_str());
        connections.add("timeout", "1");
        MysqlClient test;
        test.open(connections);
        if (test.isConnected()) {
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
        puts("-d, --database=name   Database for test.");
        return false;
    }

    if (arguments.contains("host") || arguments.contains("h")) {
        host = arguments["host"];
        if (host.isNullOrEmpty()) {
            host = arguments["h"];
        }
    }
    if (arguments.contains("port") || arguments.contains("P")) {
        if (!Port::parse(arguments["port"], port)) {
            Port::parse(arguments["P"], port);
        }
    }
    if (arguments.contains("user") || arguments.contains("u")) {
        userName = arguments["user"];
        if (userName.isNullOrEmpty()) {
            userName = arguments["u"];
        }
    }
    if (arguments.contains("password") || arguments.contains("p")) {
        password = arguments["password"];
        if (password.isNullOrEmpty()) {
            password = arguments["p"];
        }
    }
    if (arguments.contains("database") || arguments.contains("d")) {
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

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -d=test_db
int main(int argc, const char *argv[]) {
    Application app(argc, argv);
    if (!parseArguments(app)) {
        return 0;
    }

    setUp();

    int result = 0;
    if (!testOpen()) {
        result = 1;
    }
    if (!testCreateDatabase()) {
        result = 2;
    }
    if (!testCreateTable()) {
        result = 3;
    }
    if (!testInsertRecord()) {
        result = 4;
    }
    if (!testInsertRecordByTable()) {
        result = 5;
    }
    if (!testReplaceRecordByTable()) {
        result = 6;
    }
    if (!testRetrieveCount()) {
        result = 7;
    }
    if (!testTransaction()) {
        result = 8;
    }
    if (!testGetColumnNames()) {
        result = 9;
    }
    if (!testIsConnected()) {
        result = 9;
    }

    cleanUp();

    return result;
}
