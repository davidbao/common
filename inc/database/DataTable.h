#ifndef DATATABLE_H
#define DATATABLE_H

#include <assert.h>
#include "data/PList.h"
#include "data/Dictionary.h"
#include "data/ValueType.h"
#include "data/StringArray.h"
#include "data/DateTime.h"
#include "IO/Stream.h"
#include "json/JsonNode.h"
#include "DbValue.h"

using namespace Common;

namespace Database {
    class DataColumn {
    public:
        DataColumn(const String &name, const String &type, bool pkey = false);

        DataColumn(const String &name, const ValueTypes &type, bool pkey = false);

        ~DataColumn();

        ValueTypes type() const;

        int isInteger() const;

        int isDateTime() const;

        int isString() const;

        const String &name() const;

        bool primaryKey() const;

    private:
        ValueTypes convertType(const String &type);

    protected:
        String _name;
        ValueTypes _type;
        bool _primaryKey;
    };

    class DataColumns : public PList<DataColumn> {
    public:
        DataColumns(bool autoDelete = true, uint32_t capacity = PList<DataColumn>::DefaultCapacity);

        DataColumn *operator[](size_t pos) const;

        DataColumn *operator[](const String &columnName) const;

        DataColumn *at(size_t i) const override;

        DataColumn *at(const String &columnName) const;
    };

    class DataCell {
    public:
        DataCell(const DataColumn *column);

        DataCell(const DataColumn *column, const DbValue &value);

        DataCell(const DataColumn *column, bool value);

        DataCell(const DataColumn *column, uint8_t value);

        DataCell(const DataColumn *column, short value);

        DataCell(const DataColumn *column, uint16_t value);

        DataCell(const DataColumn *column, int value);

        DataCell(const DataColumn *column, uint32_t value);

        DataCell(const DataColumn *column, int64_t value);

        DataCell(const DataColumn *column, uint64_t value);

        DataCell(const DataColumn *column, double value);

        DataCell(const DataColumn *column, const char *value);

        DataCell(const DataColumn *column, const String &value);

        DataCell(const DataColumn *column, const DateTime &value);

        DataCell(const DataColumn *column, const TimeSpan &value);

        ~DataCell();

        ValueTypes type() const;

        const Value &value() const;

        const String valueStr(bool hasQuote = false) const;

        bool matchColumnName(const char *columnName) const;

        const String columnName() const;

        const DataColumn *column() const;

        static void setStringValue(Value &value, const char *str);

        bool isNullValue() const;

        void setNullValue();

        void copyFrom(const DataCell *cell);

    private:
        DataCell(const DataColumn *column, const Value value);

        void setStringValue(const char *str);

    private:
        friend class SqliteClient;

        friend class OracleClient;

        friend class MysqlClient;

        Value _value;
        const DataColumn *_column;
    };

    class DataCells : public PList<DataCell> {
    public:
        DataCells(bool autoDelete = true, uint32_t capacity = PList<DataCell>::DefaultCapacity);

        DataCell *operator[](const char *columnName) const;

        DataCell *at(size_t pos) const;

        DataCell *at(const String &columnName) const;

        const Value cellValue(const String &columnName) const;

        bool hasColumn(const String &columnName) const;

        void add(const DataCell *cell);

    private:
        Dictionary<String, size_t> _positions;
    };

    class DataRow {
    public:
        DataRow();

        ~DataRow();

        void addCell(const DataCell *cell);

        const DataCells *cells() const;

        void replaceCell(const DataCell *cell);

    private:
        DataCells _cells;
    };

    typedef PList<DataRow> DataRows;

    class DataTable {
    public:
        DataTable();

        DataTable(const String &name);

        DataTable(const char *name);

        ~DataTable();

        const String &name() const;

        void setName(const String &name);

        void addRow(const DataRow *row);

        const DataRows *rows() const;

        size_t rowCount() const;

        void addColumn(const DataColumn *column);

        const DataColumns *columns() const;

        DataColumn *getColumn(const String &columnName) const;

        size_t columnCount() const;

        void clear();

        void clearRows();

        const String anyColumnNameStr(bool hasTableName = false) const;

        const String columnNameStr(bool hasTableName = false, const StringArray *excludedNames = NULL,
                                   const char *splitStr = ",") const;

        int totalCount() const;

        void setTotalCount(int totalCount);

        void toJsonNode(JsonNode &node) const;

        String toJsonString() const;

    private:
        String _name;
        DataRows _rows;
        DataColumns _columns;
        int _totalCount;
    };

//    typedef PList<DataTable> DataTables;
    class DataTables : public PList<DataTable> {
    public:
        DataTables(uint32_t capacity = PList<DataTable>::DefaultCapacity);

        bool contains(const String &tableName) const;

        DataTable *getTable(const String &tableName) const;
    };
}
#endif // DATATABLE_H
