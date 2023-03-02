//
//  DataTableTest.cpp
//  common
//
//  Created by baowei on 2022/11/30.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "database/DataTable.h"

using namespace Database;

bool testDataColumnConstructor() {
    {
        DataColumn test;
        if (!test.isNull()) {
            return false;
        }
    }
    {
        DataColumn test("abc");
        if (!test.isNull()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Text);
        if (!test.isString()) {
            return false;
        }
        if (test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Text, true);
        if (!test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Integer64);
        if (!test.isInteger()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Digital);
        if (!test.isDigital()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Date);
        if (!test.isDateTime()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Float64);
        if (!test.isFloat()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Float32);
        if (!test.isFloat()) {
            return false;
        }
    }

    {
        DataColumn test("abc", "Text");
        if (!test.isString()) {
            return false;
        }
        if (test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Text", true);
        if (!test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Integer64");
        if (!test.isInteger()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Digital");
        if (!test.isDigital()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Date");
        if (!test.isDateTime()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float64");
        if (!test.isFloat()) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float32");
        if (!test.isFloat()) {
            return false;
        }
    }

    {
        DataColumn test("abc", "Float32");
        DataColumn test2(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float32");
        DataColumn test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testDataColumnEquals() {
    {
        DataColumn test("abc", "Float32");
        DataColumn test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float32");
        DataColumn test2;
        test2 = test;
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float32");
        DataColumn test2;
        test2.evaluates(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        DataColumn test("abc", "Float32");
        DataColumn test2;
        test2.evaluates(test);
        if (!(test2 == test)) {
            return false;
        }
    }

    return true;
}

bool testDataColumnProperty() {
    {
        DataColumn test("abc", ValueTypes::Text);
        if (!test.isString()) {
            return false;
        }
        if (test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Text, true);
        if (!test.primaryKey()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Integer64);
        if (!test.isInteger()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Digital);
        if (!test.isDigital()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Date);
        if (!test.isDateTime()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Float64);
        if (!test.isFloat()) {
            return false;
        }
    }
    {
        DataColumn test("abc", ValueTypes::Float32);
        if (!test.isFloat()) {
            return false;
        }
    }

    {
        DataColumn test("abc", ValueTypes::Float32, true);
        if (!test.primaryKey()) {
            return false;
        }
        if (test.name() != "abc") {
            return false;
        }
        if (test.type() != ValueTypes::Float32) {
            return false;
        }
    }

    {
        if (!DataColumn::Empty.isNull()) {
            return false;
        }
    }

    return true;
}

bool testDataColumnsConstructor() {
    {
        DataColumns test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        DataColumns test{
                DataColumn("id", ValueTypes::Integer32, true),
                DataColumn("name", ValueTypes::Text, false),
                DataColumn("score", ValueTypes::Float64, false),
        };
        if (test.count() != 3) {
            return false;
        }
        if (!(test[0].name() == "id" && test[1].name() == "name" && test[2].name() == "score")) {
            return false;
        }
    }

    {
        DataColumns test{
                DataColumn("id", ValueTypes::Integer32, true),
                DataColumn("name", ValueTypes::Text, false),
                DataColumn("score", ValueTypes::Float64, false),
        };
        DataColumns test2(test);
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].name() == "id" && test2[1].name() == "name" && test2[2].name() == "score")) {
            return false;
        }
    }
    {
        DataColumns test{
                DataColumn("id", ValueTypes::Integer32, true),
                DataColumn("name", ValueTypes::Text, false),
                DataColumn("score", ValueTypes::Float64, false),
        };
        DataColumns test2 = test;
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].name() == "id" && test2[1].name() == "name" && test2[2].name() == "score")) {
            return false;
        }
    }

    return true;
}

bool testDataColumnsAt() {
    {
        DataColumns test{
                DataColumn("id", ValueTypes::Integer32, true),
                DataColumn("name", ValueTypes::Text, false),
                DataColumn("score", ValueTypes::Float64, false),
        };
        const DataColumn &column = test.at(0);
        if (column.name() != "id") {
            return false;
        }
    }
    {
        DataColumns test{
                DataColumn("id", ValueTypes::Integer32, true),
                DataColumn("name", ValueTypes::Text, false),
                DataColumn("score", ValueTypes::Float64, false),
        };
        const DataColumn &column = test.at("id");
        if (column.name() != "id") {
            return false;
        }
    }

    return true;
}

bool testDataCellConstructor() {
    {
        DataCell test;
        if (!test.isNullValue()) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Bool);
        DataCell test(column, true);
        if (test.value() != true) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Integer8);
        DataCell test(column, (int8_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::UInteger8);
        DataCell test(column, (uint8_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Integer16);
        DataCell test(column, (int16_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::UInteger16);
        DataCell test(column, (uint16_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Integer32);
        DataCell test(column, (int32_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::UInteger32);
        DataCell test(column, (uint32_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Integer64);
        DataCell test(column, (int64_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::UInteger64);
        DataCell test(column, (uint64_t) 1);
        if (test.value() != 1) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Float32);
        DataCell test(column, 1.0f);
        if (test.value() != 1.0f) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Float64);
        DataCell test(column, 1.0);
        if (test.value() != 1.0) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Text);
        DataCell test(column, "test");
        if (test.value() != "test") {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Text);
        DataCell test(column, String("test"));
        if (test.value() != String("test")) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Date);
        DataCell test(column, DateTime(2010, 1, 2));
        if (test.value() != DateTime(2010, 1, 2)) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Timestamp);
        DataCell test(column, TimeSpan::fromHours(1));
        if (test.value() != TimeSpan::fromHours(1)) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Blob);
        DataCell test(column, ByteArray{1, 2, 3, 4});
        if (test.value() != ByteArray{1, 2, 3, 4}) {
            return false;
        }
    }

    {
        DataColumn column("abc", ValueTypes::Text);
        DataCell test(column, String("test"));
        DataCell test2(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        DataColumn column("abc", ValueTypes::Text);
        DataCell test(column, String("test"));
        DataCell test2 = test;
        if (test2 != test) {
            return false;
        }
    }

    return true;
}

bool testDataCellEquals() {
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        DataCell test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        DataCell test2;
        test2 = test;
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        DataCell test2;
        test2.evaluates(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        DataCell test2;
        test2.evaluates(test);
        if (!(test2 == test)) {
            return false;
        }
    }

    return true;
}

bool testDataCellProperty() {
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.type() != ValueTypes::Text) {
            return false;
        }
    }

    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.value() != "123") {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        const DbValue &value = test.value();
        String v;
        if (!value.getValue(v)) {
            return false;
        }
        if (v != "123") {
            return false;
        }
    }

    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.valueStr() != "123") {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.valueStr(false) != "123") {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.valueStr(true) != "'123'") {
            return false;
        }
    }

    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.columnName() != "abc") {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (!test.matchColumnName("abc")) {
            return false;
        }
    }

    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.column() != DataColumn("abc", ValueTypes::Text)) {
            return false;
        }
    }

    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        if (test.isNullValue()) {
            return false;
        }
    }
    {
        DataCell test(DataColumn("abc", ValueTypes::Text), "123");
        test.setNullValue();
        if (!test.isNullValue()) {
            return false;
        }
    }

    return true;
}

bool testDataCellsConstructor() {
    {
        DataCells test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        if (test.count() != 3) {
            return false;
        }
        if (!(test[0].columnName() == "id" && test[1].columnName() == "name" && test[2].columnName() == "score")) {
            return false;
        }
    }

    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        DataCells test2(test);
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].columnName() == "id" && test2[1].columnName() == "name" && test2[2].columnName() == "score")) {
            return false;
        }
    }
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        DataCells test2 = test;
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].columnName() == "id" && test2[1].columnName() == "name" && test2[2].columnName() == "score")) {
            return false;
        }
    }

    return true;
}

bool testDataCellsAt() {
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        const DataCell &column = test.at(0);
        if (column.columnName() != "id") {
            return false;
        }
    }
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        const DataCell &column = test.at("id");
        if (column.columnName() != "id") {
            return false;
        }
    }

    return true;
}

bool tetDataCellsColumn() {
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        if (!test.hasColumn("name")) {
            return false;
        }
    }
    {
        DataCells test{
                DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
        };
        const DbValue &value = test.cellValue("name");
        if (value != "abc") {
            return false;
        }
    }

    return true;
}

bool testDataRowConstructor() {
    {
        DataRow test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        if (test.cellCount() != 3) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2(test);
        if (test2.cellCount() != 3) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2 = test;
        if (test2.cellCount() != 3) {
            return false;
        }
    }

    return true;
}

bool testDataRowEquals() {
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2;
        test2.evaluates(test);
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2;
        test2 = test;
        if (!test2.equals(test)) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2;
        test2.evaluates(test);
        if (test2 != test) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        DataRow test2;
        test2.evaluates(test);
        if (!(test2 == test)) {
            return false;
        }
    }

    return true;
}

bool testDataRowProperty() {
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        if (test.isEmpty()) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                     });
        test.addCell(DataCell(DataColumn("score", ValueTypes::Float64, false), 87));
        if (test.cellCount() != 3) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                     }
        );
        test.addCells({
                              DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                              DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                      });
        if (test.cellCount() != 3) {
            return false;
        }
    }
    {
        DataRow test({
                             DataCell(DataColumn("id", ValueTypes::Integer32, true), 1),
                             DataCell(DataColumn("name", ValueTypes::Text, false), "abc"),
                             DataCell(DataColumn("score", ValueTypes::Float64, false), 87),
                     });
        const DataCells &cells = test.cells();
        if (cells.count() != 3) {
            return false;
        }
    }

    return true;
}

bool testDataRowsConstructor() {
    DataColumns columns{
            DataColumn("id", ValueTypes::Integer32, true),
            DataColumn("name", ValueTypes::Text, false),
            DataColumn("score", ValueTypes::Float64, false),
    };

    {
        DataRows test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        DataRows test{
                DataRow({
                                DataCell(columns[0], 1),
                                DataCell(columns[1], "abc"),
                                DataCell(columns[2], 87),
                        }),
                DataRow({
                                DataCell(columns[0], 2),
                                DataCell(columns[1], "bcd"),
                                DataCell(columns[2], 91),
                        }),
                DataRow({
                                DataCell(columns[0], 3),
                                DataCell(columns[1], "cde"),
                                DataCell(columns[2], 76),
                        })
        };
        DataRows test2(test);
        if (test2.count() != 3) {
            return false;
        }
        if (test != test2) {
            return false;
        }
    }
    {
        DataRows test{
                DataRow({
                                DataCell(columns[0], 1),
                                DataCell(columns[1], "abc"),
                                DataCell(columns[2], 87),
                        }),
                DataRow({
                                DataCell(columns[0], 2),
                                DataCell(columns[1], "bcd"),
                                DataCell(columns[2], 91),
                        }),
                DataRow({
                                DataCell(columns[0], 3),
                                DataCell(columns[1], "cde"),
                                DataCell(columns[2], 76),
                        })
        };
        if (test.count() != 3) {
            return false;
        }
        if (!(test[0].cellCount() == 3 && test[1].cellCount() == 3 && test[2].cellCount() == 3)) {
            return false;
        }
    }

    return true;
}

bool testDataTableConstructor() {
    {
        DataTable test;
        if (!test.isEmpty()) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        if (test.rowCount() != 3) {
            return false;
        }
        if (test.columnCount() != 3) {
            return false;
        }
    }

    return true;
}

bool testDataTableEquals() {
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2(test);
        if (test != test2) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2 = test;
        if (test != test2) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2 = test;
        if (!test.equals(test2)) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2;
        test2 = test;
        if (!(test == test2)) {
            return false;
        }
    }
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        if (test.rowCount() != 3) {
            return false;
        }
        if (test.columnCount() != 3) {
            return false;
        }

        const DataRows &rows = test.rows();
        if (!(rows[0].cellCount() == 3 && rows[1].cellCount() == 3 && rows[2].cellCount() == 3)) {
            return false;
        }

        const DataColumns &columns = test.columns();
        if (!(columns[0].name() == "id" && columns[1].name() == "name" && columns[2].name() == "score")) {
            return false;
        }

        const DataColumn &column = test.getColumn("id");
        if (!column.primaryKey()) {
            return false;
        }
        if (column.type() != ValueTypes::Integer32) {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.clear();
        if (!(test.columnCount() == 0 && test.rowCount() == 0)) {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.clearRows();
        if (!(test.columnCount() == 3 && test.rowCount() == 0)) {
            return false;
        }
    }

    return true;
}

bool testDataTableProperty() {
    {
        DataTable test("abc");
        if (test.name() != "abc") {
            return false;
        }
        test.setName("bcd");
        if (test.name() != "bcd") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        DataTable test2;
        test2.evaluates(test);
        if (!(test == test2)) {
            return false;
        }
    }

    {
        DataTable test("abc");
        if (test.anyColumnNameStr(false) != "*") {
            return false;
        }
        if (test.anyColumnNameStr() != "*") {
            return false;
        }
        if (test.anyColumnNameStr(true) != "abc.*") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        String str = test.columnNameStr();
        if (str != "id,name,score") {
            return false;
        }
        String str2 = test.columnNameStr(false);
        if (str2 != "id,name,score") {
            return false;
        }
        String str3 = test.columnNameStr(true);
        if (str3 != "abc.id,abc.name,abc.score") {
            return false;
        }

        String str4 = test.columnNameStr(false, StringArray{"id"});
        if (str4 != "name,score") {
            return false;
        }
        String str5 = test.columnNameStr(true, StringArray{"id"});
        if (str5 != "abc.name,abc.score") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.setTotalCount(10);
        if (test.totalCount() != 10) {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        JsonNode node;
        test.toJsonNode(node);
        String str = node.toString();
        if (str.isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testDataTablesConstructor() {
    DataTable table("abc");
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
                                  }),
                          DataRow({
                                          DataCell(table.columns()[0], 2),
                                          DataCell(table.columns()[1], "Hu"),
                                          DataCell(table.columns()[2], 91),
                                  }),
                          DataRow({
                                          DataCell(table.columns()[0], 3),
                                          DataCell(table.columns()[1], "Yu"),
                                          DataCell(table.columns()[2], 78),
                                  })
                  });

    {
        DataTables test;
        if (test.count() != 0) {
            return false;
        }
    }
    {
        DataTables test{
                table,
                table,
                table,
        };
        if (test.count() != 3) {
            return false;
        }
        if (!(test[0].name() == "abc" && test[1].name() == "abc" && test[2].name() == "abc")) {
            return false;
        }
    }

    {
        DataTables test{
                table,
                table,
                table,
        };
        DataTables test2(test);
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].name() == "abc" && test2[1].name() == "abc" && test2[2].name() == "abc")) {
            return false;
        }
    }
    {
        DataTables test{
                table,
                table,
                table,
        };
        DataTables test2 = test;
        if (test2.count() != 3) {
            return false;
        }
        if (!(test2[0].name() == "abc" && test2[1].name() == "abc" && test2[2].name() == "abc")) {
            return false;
        }
    }

    return true;
}

bool testDataTablesAt() {
    DataTable table("abc");
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
                                  }),
                          DataRow({
                                          DataCell(table.columns()[0], 2),
                                          DataCell(table.columns()[1], "Hu"),
                                          DataCell(table.columns()[2], 91),
                                  }),
                          DataRow({
                                          DataCell(table.columns()[0], 3),
                                          DataCell(table.columns()[1], "Yu"),
                                          DataCell(table.columns()[2], 78),
                                  })
                  });

    {
        DataTables test{
                table,
                table,
                table,
        };
        const DataTable &table1 = test.at(0);
        if (table1.name() != "abc") {
            return false;
        }
    }
    {
        DataTables test{
                table,
                table,
                table,
        };
        const DataTable &table1 = test.at("abc");
        if (table1.name() != "abc") {
            return false;
        }
        if (!test.contains("abc")) {
            return false;
        }
    }

    return true;
}

bool testSort() {
    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort(OrderByItems{
                OrderByItem("score", true)
        });
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort(OrderByItems{
                OrderByItem("score", true),
                OrderByItem("name", true)
        });
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "78" &&
            test.rows()[0].cells()["name"].valueStr() != "Ai") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "78" &&
            test.rows()[1].cells()["name"].valueStr() != "Yu") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78" &&
            test.rows()[2].cells()["name"].valueStr() != "Zh") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort(OrderByItems{
                OrderByItem("score", false),
                OrderByItem("name", false)
        });
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "78" &&
            test.rows()[4].cells()["name"].valueStr() != "Ai") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "78" &&
            test.rows()[3].cells()["name"].valueStr() != "Yu") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78" &&
            test.rows()[2].cells()["name"].valueStr() != "Zh") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort("score ASC");
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort("score, name asc");
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "78" &&
            test.rows()[0].cells()["name"].valueStr() != "Ai") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "78" &&
            test.rows()[1].cells()["name"].valueStr() != "Yu") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78" &&
            test.rows()[2].cells()["name"].valueStr() != "Zh") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    {
        DataTable test("abc");
        test.addColumns({
                                DataColumn("id", ValueTypes::Integer32, true),
                                DataColumn("name", ValueTypes::Text, false),
                                DataColumn("score", ValueTypes::Float64, false),
                        });
        test.addRows({
                             DataRow({
                                             DataCell(test.columns()[0], 1),
                                             DataCell(test.columns()[1], "Xu"),
                                             DataCell(test.columns()[2], 86),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 2),
                                             DataCell(test.columns()[1], "Hu"),
                                             DataCell(test.columns()[2], 91),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 3),
                                             DataCell(test.columns()[1], "Yu"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 4),
                                             DataCell(test.columns()[1], "Zh"),
                                             DataCell(test.columns()[2], 78),
                                     }),
                             DataRow({
                                             DataCell(test.columns()[0], 5),
                                             DataCell(test.columns()[1], "Ai"),
                                             DataCell(test.columns()[2], 78),
                                     })
                     });
        test.sort("score desc, name desc");
        if (test.rowCount() != 5) {
            return false;
        }
        if (test.rows()[4].cells()["score"].valueStr() != "78" &&
            test.rows()[4].cells()["name"].valueStr() != "Ai") {
            return false;
        }
        if (test.rows()[3].cells()["score"].valueStr() != "78" &&
            test.rows()[3].cells()["name"].valueStr() != "Yu") {
            return false;
        }
        if (test.rows()[2].cells()["score"].valueStr() != "78" &&
            test.rows()[2].cells()["name"].valueStr() != "Zh") {
            return false;
        }
        if (test.rows()[1].cells()["score"].valueStr() != "86") {
            return false;
        }
        if (test.rows()[0].cells()["score"].valueStr() != "91") {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testDataColumnConstructor()) {
        return 1;
    }
    if (!testDataColumnEquals()) {
        return 2;
    }
    if (!testDataColumnProperty()) {
        return 3;
    }

    if (!testDataColumnsConstructor()) {
        return 11;
    }
    if (!testDataColumnsAt()) {
        return 12;
    }

    if (!testDataCellConstructor()) {
        return 21;
    }
    if (!testDataCellEquals()) {
        return 22;
    }
    if (!testDataCellProperty()) {
        return 23;
    }

    if (!testDataCellsConstructor()) {
        return 31;
    }
    if (!testDataCellsAt()) {
        return 32;
    }
    if (!tetDataCellsColumn()) {
        return 33;
    }

    if (!testDataRowConstructor()) {
        return 41;
    }
    if (!testDataRowEquals()) {
        return 42;
    }
    if (!testDataRowProperty()) {
        return 43;
    }

    if (!testDataRowsConstructor()) {
        return 51;
    }

    if (!testDataTableConstructor()) {
        return 61;
    }
    if (!testDataTableEquals()) {
        return 62;
    }
    if (!testDataTableProperty()) {
        return 63;
    }

    if (!testDataTablesConstructor()) {
        return 71;
    }
    if (!testDataTablesAt()) {
        return 72;
    }

    if (!testSort()) {
        return 80;
    }

    return 0;
}