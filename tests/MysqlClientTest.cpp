//
//  MysqlClientTest.cpp
//  common
//
//  Created by baowei on 2022/11/24.
//  Copyright © 2022 com. All rights reserved.
//

#include "database/MysqlClient.h"
#include "IO/Path.h"

using namespace Database;

static const String _baseUrl("mysql://192.167.0.6:3306");
static const String _url = _baseUrl + "/MysqlClientTest_db";
static const String _username = "root";
static const String _password = "123456.com";

void setUp() {
}

void cleanUp() {
    MysqlClient test;
    if (test.open(Url(_baseUrl), _username, _password)) {
        test.executeSql("DROP DATABASE IF EXISTS MysqlClientTest_db;");
    }
}

bool testCreateDatabase() {
    {
        MysqlClient test;
        if (!test.open(Url(_baseUrl), _username, _password)) {
            return false;
        }
        test.executeSql("DROP DATABASE IF EXISTS MysqlClientTest_db;");
        if(!test.executeSql("CREATE DATABASE MysqlClientTest_db;")) {
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
                                 DataColumn("id", ValueTypes::Integer32, true),
                                 DataColumn("name", ValueTypes::Text, false),
                                 DataColumn("score", ValueTypes::Float64, false),
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
                                 DataColumn("id", ValueTypes::Integer32, true),
                                 DataColumn("name", ValueTypes::Text, false),
                                 DataColumn("score", ValueTypes::Float64, false),
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
                                 DataColumn("id", ValueTypes::Integer32, true),
                                 DataColumn("name", ValueTypes::Text, false),
                                 DataColumn("score", ValueTypes::Float64, false),
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
                                 DataColumn("id", ValueTypes::Integer32, true),
                                 DataColumn("name", ValueTypes::Text, false),
                                 DataColumn("score", ValueTypes::Float64, false),
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
                                 DataColumn("id", ValueTypes::Integer32, true),
                                 DataColumn("name", ValueTypes::Text, false),
                                 DataColumn("score", ValueTypes::Float64, false),
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

bool testExecuteSql() {
    {

    }

    return true;
}

int main() {
    setUp();

    int result = 0;
    if (!testCreateDatabase()) {
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
    if(!testExecuteSql()) {
        result = 8;
    }

    cleanUp();

    return result;
}