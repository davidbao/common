//
//  Dm7ClientTest.cpp
//  common
//
//  Created by baowei on 2023/9/13.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/Dm7Client.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "system/Application.h"
#include "system/Environment.h"

using namespace Diag;
using namespace Database;
using namespace System;
using namespace IO;

static String _host = "192.166.1.3";
static String _port = "5237";
static String _baseUrl(String::format("dm7://%s:%s", _host.c_str(), _port.c_str()));
static String _database = "DM7CLIENTTEST_DB";
static String _url = _baseUrl + "/" + _database;
static String _username = "SYSDBA";
static String _password = "SYSDBA";
static String _schema = "TEST";

void setUp() {
    Trace::enableConsoleOutput();
    Trace::enableFlushConsoleOutput();

//    Dm7Client test;
//    if (!test.open(Url(_baseUrl), _username, _password)) {
//        Trace::error("Can not open dm7!");
//        return;
//    }
//    test.executeSql(String::format("DROP DATABASE %s CASCADE;", _database.c_str()));
//    String fileName = String::format("C:\\dmdbms6\\data\\%s.dbf", _database.c_str());
//    String sql = String::format("CREATE DATABASE %s DATAFILE '%s' SIZE 50;", _database.c_str(), fileName.c_str());
//    test.executeSql(sql);
}

void cleanUp() {
    Dm7Client test;
    if (!test.open(Url(_baseUrl), _username, _password)) {
        Dm7Client test2;
        if (test2.open(Url(_url), _username, _password)) {
            test2.executeSql(String::format("DROP SCHEMA %s CASCADE;", _schema.c_str()));
        }
        return;
    } else {
//        test.executeSql(String::format("DROP DATABASE %s CASCADE;", _database.c_str()));
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
        Dm7Client test;
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
        Dm7Client test;
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
        Dm7Client test;
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

bool testCreateSchema() {
    {
        Dm7Client test;
        if (!test.open(Url(_url), _username, _password)) {
            return false;
        }
        test.executeSql(String::format("DROP SCHEMA %s CASCADE;", _schema.c_str()));
        if(!test.executeSql(String::format("CREATE SCHEMA %s AUTHORIZATION %s;", _schema.c_str(), _username.c_str()))) {
            return false;
        }
    }

    return true;
}

bool testCreateTable() {
    {
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("drop table t_student;");
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

        if (!test.executeSql("drop table t_student;\n"
                             "create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        }

        test.executeSql("drop table t_student;");

    }

    return true;
}

bool testInsertRecord() {
    {
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("drop table t_student;");
        test.executeSql("create table t_student(\n"
                        "id int primary key not null,\n"
                        "name text not null,\n"
                        "score real\n"
                        ");");
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
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("drop table t_student;");
        test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");");
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
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("drop table t_student;");
        test.executeSql("create table t_student(\n"
                            "id int primary key not null,\n"
                            "name text not null,\n"
                            "score real\n"
                            ");");
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
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");");
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
        if(!test.retrieveCount("select count(*) t_student", count)) {
            return false;
        }
        if(count != 1) {
            return false;
        }

        test.executeSql("drop table t_student;");
    }

    return true;
}

bool testTransaction() {
    {
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("drop table t_student;");
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
        Dm7Client test;
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
        Dm7Client test;
        if (!test.open(_url, _username, _password)) {
            return false;
        }
        test.executeSql("create table t_student(\n"
                        "id int primary key not null,\n"
                        "name text not null,\n"
                        "score real\n"
                        ");");

        StringArray names = test.getColumnName("t_student");
        if (names.count() != 3) {
            return false;
        }
        if (!(names[0].toUpper() == "ID" && names[1].toUpper() == "NAME" && names[2].toUpper() == "SCORE")) {
            return false;
        }
    }

    return true;
}

bool testIsConnected() {
    {
        Dm7Client test;
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
        Dm7Client test;
        test.open(connections);
        if (test.isConnected()) {
            return false;
        }
    }

    return true;
}

bool parseArguments(const Application &app) {
    const Application::Arguments &arguments = app.arguments();
    String host, userName, password, schema, database;
    Port port;
    if (arguments.contains("help") || arguments.contains("?")) {
        puts("Usage:");
        puts("-?, --help            Display this help and exit.");
        puts("-h, --host=name       Connect to host.");
        puts("-P, --port=#          Port number to use for connection.");
        puts("-u, --user=name       User for login if not current user.");
        puts("-p, --password[=name] Password to use when connecting to server.");
        puts("-s, --schema=name     Schema for test.");
        puts("-d, --database=name   Database for test.");
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
    if(arguments.contains("schema") || arguments.contains("s")) {
        schema = arguments["database"];
        if (schema.isNullOrEmpty()) {
            schema = arguments["s"];
        }
    }
    if(arguments.contains("database") || arguments.contains("d")) {
        database = arguments["database"];
        if (database.isNullOrEmpty()) {
            database = arguments["d"];
        }
    }

    _baseUrl = String::format("dm7://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : _host.c_str(),
                              !port.isEmpty() ? port.toString().c_str() : _port.c_str());
    _schema = !schema.isNullOrEmpty() ? schema : "TEST";
    _database = !database.isNullOrEmpty() ? database : _database;
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : _username;
    _password = !password.isNullOrEmpty() ? password : _password;

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -s=test_schema -d=DM7CLIENTTEST_DB
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
    if (!testCreateSchema()) {
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
    if(!testGetColumnNames()) {
        result = 9;
    }
    if (!testIsConnected()) {
        result = 10;
    }

    cleanUp();

    return result;
}