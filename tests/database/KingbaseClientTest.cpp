//
//  KingbaseClientTest.cpp
//  common
//
//  Created by baowei on 2023/1/3.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "database/KingbaseClient.h"
#include "diag/Trace.h"
#include "system/Application.h"

using namespace Diag;
using namespace Database;
using namespace System;

static String _baseUrl("kingbase://192.166.1.19:54321");
static String _database = "KINGBASECLIENTTEST_DB";
static String _url = _baseUrl + "/" + _database;
static String _username = "SYSTEM";
static String _password = "MANAGER";
static String _schema = "SYSTEM";

void setUp() {
    Trace::enableConsoleOutput();
    Trace::enableFlushConsoleOutput();
}

void cleanUp() {
    KingbaseClient test;
    if (test.open(Url(_url), _username, _password)) {
        test.executeSql(String::format("DROP SCHEMA %s CASCADE;", _schema.c_str()));
    }
}

bool testCreateSchema() {
    {
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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
        KingbaseClient test;
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

        StringArray names = test.getColumnName("T_STUDENT");
        if (names.count() != 3) {
            return false;
        }
        if (!(names[0] == "ID" && names[1] == "NAME" && names[2] == "SCORE")) {
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

    _baseUrl = String::format("kingbase://%s:%s",
                              !host.isNullOrEmpty() ? host.c_str() : "192.166.1.19",
                              !port.isEmpty() ? port.toString().c_str() : "54321");
    _schema = !schema.isNullOrEmpty() ? schema : "SYSTEM";
    _database = !database.isNullOrEmpty() ? database : "KINGBASECLIENTTEST_DB";
    _url = _baseUrl + "/" + _database;
    _username = !userName.isNullOrEmpty() ? userName : "SYSTEM";
    _password = !password.isNullOrEmpty() ? password : "MANAGER";

    return true;
}

// argv: -h=192.167.0.6 -P=3306 -u=root -p=123456.com -s=test_schema -d=KINGBASECLIENTTEST_DB
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