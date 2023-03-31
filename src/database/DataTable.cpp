//
//  DataTable.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "database/DataTable.h"

namespace Database {
    const DataColumn DataColumn::Empty("", ValueTypes::Null, false);

    DataColumn::DataColumn(const String &name, const ValueTypes &type, bool pkey) : _name(name), _type(type),
                                                                                    _primaryKey(pkey) {
    }

    DataColumn::DataColumn(const String &name, const String &type, bool pkey) : DataColumn(name,
                                                                                           DbValue::fromTypeStr(type),
                                                                                           pkey) {
    }

    DataColumn::DataColumn(const DataColumn &column) : DataColumn(String::Empty, ValueTypes::Null, false) {
        DataColumn::evaluates(column);
    }

    DataColumn::~DataColumn() = default;

    void DataColumn::evaluates(const DataColumn &other) {
        _name = other._name;
        _type = other._type;
        _primaryKey = other._primaryKey;
    }

    bool DataColumn::equals(const DataColumn &other) const {
        return _name == other._name && _type == other._type && _primaryKey == other._primaryKey;
    }

    const String &DataColumn::name() const {
        return _name;
    }

    ValueTypes DataColumn::type() const {
        return _type;
    }

    bool DataColumn::primaryKey() const {
        return _primaryKey;
    }

    bool DataColumn::isNull() const {
        return DbValue::isNullType(_type);
    }

    bool DataColumn::isInteger() const {
        return DbValue::isIntegerValue(_type);
    }

    bool DataColumn::isFloat() const {
        return DbValue::isFloatValue(_type);
    }

    bool DataColumn::isString() const {
        return DbValue::isStringValue(_type);
    }

    bool DataColumn::isDateTime() const {
        return DbValue::isDateTimeValue(_type);
    }

    bool DataColumn::isDigital() const {
        return DbValue::isDigitalValue(_type);
    }

    DataColumn &DataColumn::operator=(const DataColumn &other) {
        if (this != &other) {
            evaluates(other);
        }
        return *this;
    }

    DataColumns::DataColumns(size_t capacity) : List<DataColumn>(capacity) {
    }

    DataColumns::DataColumns(std::initializer_list<DataColumn> list) : List<DataColumn>(list) {
    }

    DataColumns::DataColumns(const DataColumns &columns) : List<DataColumn>(columns) {
    }

    const DataColumn &DataColumns::at(const String &columnName) const {
        for (size_t i = 0; i < count(); i++) {
            const DataColumn &column = at(i);
            if (String::equals(column.name(), columnName, true)) {
                return column;
            }
        }
        return DataColumn::Empty;
    }

    const DataCell DataCell::Empty;

    DataCell::DataCell(const DataColumn &column) : _column(column), _value(column.type()) {
    }

    DataCell::DataCell(const DataColumn &column, const Value &value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const DbValue &value) : _column(column), _value(value) {
    }

    DataCell::DataCell(const DataColumn &column, bool value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, int8_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, uint8_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, int16_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, uint16_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, int32_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, uint32_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, int64_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, uint64_t value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, float value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, double value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const char *value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const String &value) : _column(column), _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const DateTime &value) : _column(column),
                                                                          _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const TimeSpan &value) : _column(column),
                                                                          _value(column.type(), value) {
    }

    DataCell::DataCell(const DataColumn &column, const ByteArray &value) : _column(column),
                                                                           _value(column.type(), value) {
    }

    DataCell::DataCell(const DataCell &cell) : _column(cell._column), _value(cell._value) {
    }

    DataCell::~DataCell() = default;

    void DataCell::evaluates(const DataCell &other) {
        _column = other._column;
        _value = other._value;
    }

    bool DataCell::equals(const DataCell &other) const {
        return _column == other._column && _value == other._value;
    }

    ValueTypes DataCell::type() const {
        return _column.type();
    }

    const DbValue &DataCell::value() const {
        return _value;
    }

    String DataCell::valueStr(bool hasQuote) const {
        String valueStr = _value.valueStr();
        if (type() == DbValue::Text || type() == DbValue::Date || type() == DbValue::Timestamp) {
            return hasQuote ? String::convert("'%s'", valueStr.c_str()) : valueStr;
        }
        return valueStr;
    }

    bool DataCell::matchColumnName(const String &columnName) const {
        return String::equals(this->columnName(), columnName, true);
    }

    String DataCell::columnName() const {
        return _column.name();
    }

    const DataColumn &DataCell::column() const {
        return _column;
    }

    bool DataCell::isNullValue() const {
        return DbValue::isNullValue(_value);
    }

    void DataCell::setNullValue() {
        _value = DbValue::NullValue;
    }

    DataCell &DataCell::operator=(const DataCell &other) {
        if (this != &other) {
            evaluates(other);
        }
        return *this;
    }

    DataCells::DataCells(size_t capacity) : List<DataCell>(capacity) {
    }

    DataCells::DataCells(std::initializer_list<DataCell> list) : List<DataCell>(list) {
    }

    DataCells::DataCells(const DataCells &cells) : List<DataCell>(cells) {
    }

    const DataCell &DataCells::at(const String &columnName) const {
        for (size_t i = 0; i < count(); i++) {
            const DataCell &cell = at(i);
            if (cell.matchColumnName(columnName)) {
                return cell;
            }
        }
        return DataCell::Empty;
    }

    const DbValue &DataCells::cellValue(const String &columnName) const {
        const DataCell &cell = at(columnName);
        return cell.value();
    }

    bool DataCells::hasColumn(const String &columnName) const {
        for (size_t i = 0; i < count(); i++) {
            const DataCell &cell = at(i);
            if (cell.matchColumnName(columnName)) {
                return true;
            }
        }
        return false;
    }

    const DataRow DataRow::Empty;

    DataRow::DataRow() = default;

    DataRow::DataRow(const DataCells &cells) : _cells(cells) {
    }

    DataRow::DataRow(std::initializer_list<DataCell> list) {
        _cells.addRange(list);
    }

    DataRow::DataRow(const DataRow &row) : _cells(row._cells) {
    }

    DataRow::~DataRow() = default;

    void DataRow::evaluates(const DataRow &other) {
        _cells = other._cells;
    }

    bool DataRow::equals(const DataRow &other) const {
        return _cells == other._cells;
    }

    const DataCell &DataRow::at(size_t pos) const {
        return _cells[pos];
    }

    const DataCell &DataRow::at(const String &pos) const {
        return _cells[pos];
    }

    bool DataRow::isEmpty() const {
        return _cells.count() == 0;
    }

    void DataRow::addCell(const DataCell &cell) {
        _cells.add(cell);
    }

    void DataRow::addCells(const DataCells &cells) {
        _cells.addRange(cells);
    }

    const DataCells &DataRow::cells() const {
        return _cells;
    }

    size_t DataRow::cellCount() const {
        return _cells.count();
    }

    DataRow &DataRow::operator=(const DataRow &other) {
        if (this != &other) {
            evaluates(other);
        }
        return *this;
    }

    const DataTable DataTable::Empty("");

    DataTable::DataTable(const String &name) : _name(name), _totalCount(0) {
    }

    DataTable::DataTable(const DataTable &table) : _name(table.name()), _totalCount(table.totalCount()) {
        DataTable::evaluates(table);
    }

    DataTable::~DataTable() {
        clear();
    }

    void DataTable::evaluates(const DataTable &other) {
        _name = other._name;
        _rows = other._rows;
        _columns = other._columns;
        _totalCount = other._totalCount;
    }

    bool DataTable::equals(const DataTable &other) const {
        return _name == other._name &&
               _rows == other._rows &&
               _columns == other._columns &&
               _totalCount == other._totalCount;
    }

    DataTable &DataTable::operator=(const DataTable &other) {
        if (this != &other) {
            evaluates(other);
        }
        return *this;
    }

    bool DataTable::isEmpty() const {
        return _name.isNullOrEmpty();
    }

    const String &DataTable::name() const {
        return _name;
    }

    void DataTable::setName(const String &name) {
        _name = name;
    }

    void DataTable::addRow(const DataRow &row) {
        _rows.add(row);
    }

    void DataTable::addRows(const DataRows &rows) {
        _rows.addRange(rows);
    }

    bool DataTable::removeRow(size_t pos) {
        return _rows.removeAt(pos);
    }

    bool DataTable::setRow(size_t pos, const DataRow &row) {
        return _rows.set(pos, row);
    }

    const DataRows &DataTable::rows() const {
        return _rows;
    }

    size_t DataTable::rowCount() const {
        return _rows.count();
    }

    void DataTable::addColumn(const DataColumn &column) {
        _columns.add(column);
    }

    void DataTable::addColumns(const DataColumns &columns) {
        _columns.addRange(columns);
    }

    bool DataTable::removeColumn(size_t pos) {
        return _columns.removeAt(pos);
    }

    bool DataTable::setColumn(size_t pos, const DataColumn &column) {
        return _columns.set(pos, column);
    }

    const DataColumns &DataTable::columns() const {
        return _columns;
    }

    const DataColumn &DataTable::getColumn(const String &columnName) const {
        return _columns.at(columnName);
    }

    size_t DataTable::columnCount() const {
        return _columns.count();
    }

    void DataTable::clear() {
        _name = String::Empty;
        _rows.clear();
        _columns.clear();
    }

    void DataTable::clearRows() {
        _rows.clear();
    }

    String DataTable::anyColumnNameStr(bool hasTableName) const {
        String sb;
        if (hasTableName) {
            sb.append(name());
            sb.append(".");
        }
        sb.append("*");
        return sb;
    }

    String DataTable::columnNameStr(bool hasTableName, const StringArray &excludedNames, const char *splitStr) const {
        String sb;
        for (size_t i = 0; i < columnCount(); i++) {
            const String &cname = _columns.at(i).name();
            if (!excludedNames.contains(cname, true)) {
                if (sb.length() > 0) {
                    sb.append(splitStr);
                }
                if (hasTableName) {
                    sb.append(name());
                    sb.append(".");
                }
                sb.append(cname);
            }
        }
        return sb;
    }

    int DataTable::totalCount() const {
        return _totalCount;
    }

    void DataTable::setTotalCount(int totalCount) {
        _totalCount = totalCount >= 0 ? totalCount : 0;
    }

    void DataTable::toJsonNode(JsonNode &node, const String &format) const {
        String fmt = format.isNullOrEmpty() ? "g" : format;
        node = JsonNode(name(), JsonNode::TypeArray);
        if (String::equals(fmt, "g", true) || String::equals(fmt, "l", true)) {
            for (size_t i = 0; i < rowCount(); i++) {
                const DataRow &row = rows().at(i);
                const DataCells &cells = row.cells();
                JsonNode rowNode;
                for (size_t j = 0; j < cells.count(); j++) {
                    const DataCell &cell = cells.at(j);
                    rowNode.add(JsonNode(cell.columnName(), !cell.isNullValue() ? cell.valueStr() : String::Empty));
                }
                node.add(rowNode);
            }
        } else if (String::equals(fmt, "s", true)) {
            for (size_t i = 0; i < rowCount(); i++) {
                const DataRow &row = rows().at(i);
                const DataCells &cells = row.cells();
                StringArray texts;
                for (size_t j = 0; j < cells.count(); j++) {
                    const DataCell &cell = cells.at(j);
                    texts.add(!cell.isNullValue() ? cell.valueStr() : String::Empty);
                }
                node.add(JsonNode("row", texts));
            }
        }
    }

    String DataTable::toJsonString() const {
        JsonNode node;
        toJsonNode(node);
        return node.toString();
    }

    void DataTable::sort(const String &orderBy) {
        OrderByItems items;
        if (SqlSelectFilter::parseOrderBy(orderBy, items)) {
            sort(items);
        }
    }

    void DataTable::sort(const OrderByItems &items) {
        OrderByItems existItems;
        for (size_t i = 0; i < items.count(); ++i) {
            const OrderByItem &item = items[i];
            const DataColumn &column = _columns.at(item.name);
            if (!column.isNull()) {
                existItems.add(item);
            }
        }
        if (items.count() > 0) {
            quicksort(_rows.data(), 0, (int) _rows.count() - 1, compare, items);
        }
    }

    void DataTable::quicksort(DataRow *array[], int left, int right, Comparison comparison, const OrderByItems &items) {
        if (items.count() == 0) {
            return;
        }

        if (left >= right) {
            return;
        }
        int first = left;
        int last = right;
        bool asc = items[0].asc;
        DataRow *key = array[first];

        while (first < last) {
            if (asc) {
                while (first < last && comparison(array[last], key, items) >= 0) {
                    --last;
                }
            } else {
                while (first < last && comparison(array[last], key, items) <= 0) {
                    --last;
                }
            }

            array[first] = array[last];

            if (asc) {
                while (first < last && comparison(array[first], key, items) <= 0) {
                    ++first;
                }
            } else {
                while (first < last && comparison(array[first], key, items) >= 0) {
                    ++first;
                }
            }

            array[last] = array[first];
        }
        array[first] = key;
        quicksort(array, left, first - 1, comparison, items);
        quicksort(array, first + 1, right, comparison, items);
    }

    int DataTable::compare(const DataRow *x, const DataRow *y, const OrderByItems &items) {
        if (items.count() == 0) {
            return 0;
        }
        const OrderByItem &firstItem = items[0];
        const DataCell &cell1 = x->cells().at(firstItem.name);
        const DataCell &cell2 = y->cells().at(firstItem.name);
        if (cell1.value() > cell2.value()) {
            return 1;
        } else if (cell1.value() < cell2.value()) {
            return -1;
        } else {
            for (size_t i = 1; i < items.count(); ++i) {
                const OrderByItem &item = items[i];
                const DataCell &cellX = x->cells().at(item.name);
                const DataCell &cellY = y->cells().at(item.name);
                if (cellX.value() > cellY.value()) {
                    return 1;
                } else if (cellX.value() < cellY.value()) {
                    return -1;
                } else {
                }
            }
            return 0;
        }
    }

    DataTables::DataTables(size_t capacity) : List<DataTable>(capacity) {
    }

    DataTables::DataTables(std::initializer_list<DataTable> list) : List<DataTable>(list) {
    }

    DataTables::DataTables(const DataTables &tables) : List<DataTable>(tables) {
    }

    bool DataTables::contains(const String &tableName) const {
        for (size_t i = 0; i < count(); i++) {
            const DataTable &table = at(i);
            if (table.name() == tableName)
                return true;
        }
        return false;
    }

    const DataTable &DataTables::at(const String &tableName) const {
        for (size_t i = 0; i < count(); i++) {
            const DataTable &table = at(i);
            if (table.name() == tableName)
                return table;
        }
        return DataTable::Empty;
    }
}
