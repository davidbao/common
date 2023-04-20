//
//  SqliteClientTest.cpp
//  common
//
//  Created by baowei on 2022/11/24.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "database/SqliteClient.h"
#include "IO/Directory.h"
#include "IO/Path.h"
#include "IO/File.h"

using namespace Database;

static String _path = Path::combine(Path::getTempPath(), "SqliteClientTest");
static String _fileName = Path::combine(_path, "test.db");

void setUp() {
    if (!Directory::exists(_path)) {
        Directory::createDirectory(_path);
    }
}

void cleanUp() {
    if (Directory::exists(_path)) {
        Directory::deleteDirectory(_path);
    }
}

bool testCreateDatabase() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
            return false;
        }
        if (!File::exists(_fileName)) {
            return false;
        }
    }
    File::deleteFile(_fileName);

    return true;
}

bool testCreateTable() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
        if (!test.executeSqlQuery("select name from sqlite_master "
                                  "where type='table' AND name='t_student'", table)) {
            return false;
        }
        if (table.rowCount() != 1) {
            return false;
        }
        const DbValue &value = table.rows().at(0).cells().at("name").value();
        if (value != "t_student") {
            return false;
        }
    }
    File::deleteFile(_fileName);

    return true;
}

bool testInsertRecord() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
    }
    File::deleteFile(_fileName);

    return true;
}

bool testInsertRecordByTable() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
        SqliteClient test;
        if (!test.open(_fileName)) {
            return false;
        }
        if (test.executeSql("create table t_student(\n"
                             "id int primary key not null,\n"
                             "name text not null,\n"
                             "score real\n"
                             ");")) {
            return false;
        } else {
            String error = test.getErrorMsg();
            if(error.isNullOrEmpty()) {
                return false;
            }
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
    }
    File::deleteFile(_fileName);

    return true;
}

bool testRetrieveCount() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
    }
    File::deleteFile(_fileName);

    return true;
}

bool testTransaction() {
    {
        File::deleteFile(_fileName);

        SqliteClient test;
        if (!test.open(_fileName)) {
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
    }
    File::deleteFile(_fileName);

    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
    File::deleteFile(_fileName);

    return true;
}

bool testGenerateSnowFlakeId() {
    {
        uint64_t id = DbClient::generateSnowFlakeId();
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 1000; ++i) {
            id = DbClient::generateSnowFlakeId();
//            printf("id = %lld\n", id);
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 5000; ++i) {
            id = DbClient::generateSnowFlakeId();
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }
    {
        uint64_t id, prevId = 0;
        for (int i = 0; i < 1000 * 1000; ++i) {
            id = DbClient::generateSnowFlakeId();
            if (id == prevId) {
                return false;
            }
            prevId = id;
        }
    }

    {
        uint64_t id = DbClient::generateSnowFlakeId(1, 2);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = DbClient::generateSnowFlakeId(1, 2);
        if (id == 0) {
            return false;
        }
    }
    {
        uint64_t id = DbClient::generateSnowFlakeId(33, 2);
        if (id != 0) {
            return false;
        }
    }
    {
        uint64_t id = DbClient::generateSnowFlakeId(1, 33);
        if (id != 0) {
            return false;
        }
    }

    return true;
}

bool testGetColumnNames() {
    {
        SqliteClient test;
        if (!test.open(_fileName)) {
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
//    if(!testGetColumnNames()) {
//        result = 8;
//    }
    if(!testGenerateSnowFlakeId()) {
        result = 9;
    }

    cleanUp();

    return result;
}