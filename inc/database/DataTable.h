//
//  DataTable.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef DataTable_h
#define DataTable_h

#include "data/List.h"
#include "data/Dictionary.h"
#include "data/String.h"
#include "data/StringArray.h"
#include "data/DateTime.h"
#include "database/DbValue.h"
#include "IO/Stream.h"
#include "json/JsonNode.h"
#include "database/SqlSelectFilter.h"

using namespace Data;
using namespace Json;

namespace Database {
    class DataColumn : public IEvaluation<DataColumn>, public IEquatable<DataColumn> {
    public:
        explicit DataColumn(const String &name = String::Empty, const DbType &type = DbType::Null,
                            bool pkey = false);

        DataColumn(const String &name, const String &type, bool pkey = false);

        DataColumn(const DataColumn &column);

        ~DataColumn() override;

        void evaluates(const DataColumn &other) override;

        bool equals(const DataColumn &other) const override;

        DataColumn &operator=(const DataColumn &other);

        const String &name() const;

        DbType type() const;

        bool primaryKey() const;

        bool isNull() const;

        bool isInteger() const;

        bool isFloat() const;

        bool isString() const;

        bool isDateTime() const;

        bool isDigital() const;

    protected:
        String _name;
        DbType _type;
        bool _primaryKey;

    public:
        static const DataColumn Empty;
    };

    class DataColumns : public List<DataColumn>, public IPositionGetter<const DataColumn &, const String &> {
    public:
        using List<DataColumn>::at;
        using List<DataColumn>::operator[];
        using IPositionGetter<const DataColumn &, const String &>::operator[];

        explicit DataColumns(size_t capacity = DefaultCapacity);

        DataColumns(std::initializer_list<DataColumn> list);

        DataColumns(const DataColumns &columns);

        const DataColumn &at(const String &columnName) const override;
    };

    class DataCell : public IEvaluation<DataCell>, public IEquatable<DataCell> {
    public:
        explicit DataCell(const DataColumn &column = DataColumn::Empty);

        DataCell(const DataColumn &column, const DbValue &value);

        DataCell(const DataColumn &column, bool value);

        DataCell(const DataColumn &column, int8_t value);

        DataCell(const DataColumn &column, uint8_t value);

        DataCell(const DataColumn &column, int16_t value);

        DataCell(const DataColumn &column, uint16_t value);

        DataCell(const DataColumn &column, int32_t value);

        DataCell(const DataColumn &column, uint32_t value);

        DataCell(const DataColumn &column, int64_t value);

        DataCell(const DataColumn &column, uint64_t value);

        DataCell(const DataColumn &column, float value);

        DataCell(const DataColumn &column, double value);

        DataCell(const DataColumn &column, const char *value);

        DataCell(const DataColumn &column, const String &value);

        DataCell(const DataColumn &column, const DateTime &value);

        DataCell(const DataColumn &column, const TimeSpan &value);

        DataCell(const DataColumn &column, const ByteArray &value);

        DataCell(const DataCell &cell);

        ~DataCell() override;

        void evaluates(const DataCell &other) override;

        bool equals(const DataCell &other) const override;

        DataCell &operator=(const DataCell &other);

        DbType type() const;

        const DbValue &value() const;

        String valueStr(bool hasQuote = false) const;

        bool matchColumnName(const String &columnName) const;

        String columnName() const;

        const DataColumn &column() const;

        bool isNullValue() const;

        void setNullValue();

    private:
        DataCell(const DataColumn &column, const DbValue::Value &value);

    public:
        static const DataCell Empty;

    private:
        DbValue _value;
        DataColumn _column;
    };

    class DataCells : public List<DataCell>, public IPositionGetter<const DataCell &, const String &> {
    public:
        using List<DataCell>::at;
        using List<DataCell>::operator[];
        using IPositionGetter<const DataCell &, const String &>::operator[];

        explicit DataCells(size_t capacity = DefaultCapacity);

        DataCells(std::initializer_list<DataCell> list);

        DataCells(const DataCells &cells);

        const DataCell &at(const String &columnName) const override;

        const DbValue &cellValue(const String &columnName) const;

        bool hasColumn(const String &columnName) const;
    };

    class DataRow : public IEvaluation<DataRow>, public IEquatable<DataRow>,
                    public IIndexGetter<const DataCell &>,
                    public IPositionGetter<const DataCell &, const String &> {
    public:
        using IIndexGetter<const DataCell &>::at;
        using IIndexGetter<const DataCell &>::operator[];
        using IPositionGetter<const DataCell &, const String &>::operator[];

        DataRow();

        explicit DataRow(const DataCells &cells);

        DataRow(std::initializer_list<DataCell> list);

        DataRow(const DataRow &row);

        ~DataRow() override;

        void evaluates(const DataRow &other) override;

        bool equals(const DataRow &other) const override;

        const DataCell &at(size_t pos) const override;

        const DataCell &at(const String &pos) const override;

        DataRow &operator=(const DataRow &other);

        bool isEmpty() const;

        void addCell(const DataCell &cell);

        void addCells(const DataCells &cells);

        const DataCells &cells() const;

        size_t cellCount() const;

    public:
        static const DataRow Empty;

    private:
        DataCells _cells;
    };

    typedef List<DataRow> DataRows;

    class DataTable : public IEvaluation<DataTable>, public IEquatable<DataTable> {
    public:
        explicit DataTable(const String &name = String::Empty);

        DataTable(const DataTable &table);

        ~DataTable() override;

        void evaluates(const DataTable &other) override;

        bool equals(const DataTable &other) const override;

        DataTable &operator=(const DataTable &other);

        bool isEmpty() const;

        const String &name() const;

        void setName(const String &name);

        void addRow(const DataRow &row);

        void addRows(const DataRows &rows);

        bool removeRow(size_t pos);

        bool setRow(size_t pos, const DataRow &row);

        const DataRows &rows() const;

        size_t rowCount() const;

        void addColumn(const DataColumn &column);

        void addColumns(const DataColumns &columns);

        bool removeColumn(size_t pos);

        bool setColumn(size_t pos, const DataColumn &column);

        const DataColumns &columns() const;

        const DataColumn &getColumn(const String &columnName) const;

        size_t columnCount() const;

        void clear();

        void clearRows();

        String anyColumnNameStr(bool hasTableName = false) const;

        String columnNameStr(bool hasTableName = false, const StringArray &excludedNames = StringArray::Empty,
                             const char *splitStr = ",") const;

        int totalCount() const;

        void setTotalCount(int totalCount);

        void toJsonNode(JsonNode &node, const String &format = String::Empty) const;

        String toJsonString() const;

        void sort(const String &orderBy);

        void sort(const OrderByItems &items);

    private:
        // A signed integer that indicates the relative values of x and y, as shown in the following table.
        // Value	            Meaning
        // Less than 0	        x is less than y.
        // 0	                x equals y.
        // Greater than 0	    x is greater than y.
        typedef int(*Comparison)(const DataRow *x, const DataRow *y, const OrderByItems &items);

        static void quicksort(DataRow *array[], int left, int right, Comparison comparison, const OrderByItems &items);

        static int compare(const DataRow *x, const DataRow *y, const OrderByItems &items);

    private:
        String _name;
        DataRows _rows;
        DataColumns _columns;
        int _totalCount;

    public:
        static const DataTable Empty;
    };

    class DataTables : public List<DataTable>, public IPositionGetter<const DataTable &, const String &> {
    public:
        using List<DataTable>::at;
        using List<DataTable>::operator[];
        using IPositionGetter<const DataTable &, const String &>::operator[];

        explicit DataTables(size_t capacity = DefaultCapacity);

        DataTables(std::initializer_list<DataTable> list);

        DataTables(const DataTables &tables);

        bool contains(const String &tableName) const;

        const DataTable &at(const String &tableName) const override;
    };
}
#endif // DataTable_h
