//
//  Dm6ClientTest.cpp
//  common
//
//  Created by baowei on 2023/7/6.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/Dm6Client.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "system/Application.h"

using namespace Diag;
using namespace Database;
using namespace System;
using namespace IO;

static String _baseUrl("dm6://192.166.1.3:12345");
static String _database = "DM6CLIENTTEST_DB";
static String _url = _baseUrl + "/" + _database;
static String _username = "SYSDBA";
static String _password = "SYSDBA";
static String _schema = "TEST";

void setUp() {
    Trace::enableConsoleOutput();
    Trace::enableFlushConsoleOutput();

    Dm6Client test;
    if (!test.open(Url(_baseUrl), _username, _password)) {
        Trace::error("Can not open dm6!");
        return;
    }
    test.executeSql(String::format("DROP DATABASE %s CASCADE;", _database.c_str()));
    String fileName = String::format("C:\\dmdbms6\\data\\%s.dbf", _database.c_str());
    String sql = String::format("CREATE DATABASE %s DATAFILE '%s' SIZE 50;", _database.c_str(), fileName.c_str());
    test.executeSql(sql);
}

void cleanUp() {
    Dm6Client test;
    if (!test.open(Url(_baseUrl), _username, _password)) {
        Dm6Client test2;
        if (test2.open(Url(_url), _username, _password)) {
            test2.executeSql(String::format("DROP SCHEMA %s CASCADE;", _schema.c_str()));
        }
        return;
    } else {
        test.executeSql(String::format("DROP DATABASE %s CASCADE;", _database.c_str()));
    }
}

bool testCreateSchema() {
    {
        Dm6Client test;
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
        Dm6Client test;
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
        Dm6Client test;
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
        Dm6Client test;
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
        Dm6Client test;
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
        Dm6Client test;
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
        Dm6Client test;
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

//    {
//        Dm6Client test;
//        if (!test.open(_url, _username, _password)) {
//            return false;
//        }
//        test.beginTransaction();
//        if (!test.executeSql("create table t_student(\n"
//                             "id int primary key not null,\n"
//                             "name text not null,\n"
//                             "score real\n"
//                             ");", false)) {
//            return false;
//        }
//        test.commitTransaction();
//
//        test.beginTransaction();
//        DataTable table("t_student");
//        table.addColumns({
//                                 DataColumn("id", DbType::Integer32, true),
//                                 DataColumn("name", DbType::Text, false),
//                                 DataColumn("score", DbType::Float64, false),
//                         });
//        table.addRows({
//                              DataRow({
//                                              DataCell(table.columns()[0], 1),
//                                              DataCell(table.columns()[1], "Xu"),
//                                              DataCell(table.columns()[2], 86),
//                                      })
//                      });
//        if (!test.executeSqlInsert(table, false)) {
//            return false;
//        }
//        test.rollbackTransaction();
//
//        DataTable table2;
//        if (!test.executeSqlQuery("select * from t_student;", table2)) {
//            return false;
//        }
//        if (table2.rowCount() != 0) {
//            return false;
//        }
//    }

    return true;
}

bool testGetColumnNames() {
    {
        Dm6Client test;
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

    _baseUrl = String::format("dm6://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : "192.166.1.3",
                              !port.isEmpty() ? port.toString().c_str() : "12345");
    _schema = !schema.isNullOrEmpty() ? schema : "TEST";
    _database = !database.isNullOrEmpty() ? database : _database;
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : _username;
    _password = !password.isNullOrEmpty() ? password : _password;

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -s=test_schema -d=DM6CLIENTTEST_DB
int main(int argc, const char *argv[]) {
    Application app(argc, argv);
    if (!parseArguments(app)) {
        return 0;
    }

    setUp();

    int result = 0;
    if (!testCreateSchema()) {
        result = 1;
    }
    if (!testCreateTable()) {
        result = 2;
    }
    if (!testInsertRecord()) {
        result = 3;
    }
    if (!testInsertRecordByTable()) {
        result = 4;
    }
    if (!testReplaceRecordByTable()) {
        result = 5;
    }
    if (!testRetrieveCount()) {
        result = 6;
    }
    if (!testTransaction()) {
        result = 7;
    }
    if(!testGetColumnNames()) {
        result = 8;
    }

    cleanUp();

    return result;
}