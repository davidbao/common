#include "database/DataTable.h"

namespace Database
{
	DataColumn::DataColumn(const String& name, const String& type, bool pkey) : DataColumn(name, convertType(type), pkey)
	{
	}
	DataColumn::DataColumn(const String& name, const ValueTypes& type, bool pkey)
	{
		_name = name;
		_type = type;
		_primaryKey = pkey;
	}
	DataColumn::~DataColumn()
	{
	}

	ValueTypes DataColumn::type() const
	{
		return _type;
	}

	int DataColumn::isInteger() const
	{
		return (_type == Integer32);
	}

	int DataColumn::isDateTime() const
	{
		return (_type == Date);
	}

	int DataColumn::isString() const
	{
		return (_type == Text);
	}

	const String& DataColumn::name() const
	{
		return _name;
	}

	bool DataColumn::primaryKey() const
	{
		return _primaryKey;
	}

	ValueTypes DataColumn::convertType(const String& type)
	{
		ValueTypes valueType = Null;
		if (type.find("int") >= 0)
		{
			valueType = Integer32;
		}
		else if (type.find("char") >= 0 ||
			type.find("varchar") >= 0 ||
			type.find("text") >= 0 ||
			type.find("nvarchar") >= 0)
		{
			valueType = Text;
		}
		else if (type.find("date") >= 0)
		{
			valueType = Date;
		}
		else if (type.find("float") >= 0)
		{
			valueType = Float32;
		}
		else
		{
		}
		return valueType;
	}

	DataColumns::DataColumns(bool autoDelete, uint capacity) : Vector<DataColumn>(autoDelete, capacity)
	{
	}

	DataColumn* DataColumns::operator[](const String& columnName) const
	{
		return at(columnName);
	}
//    DataColumn* DataColumns::operator[](size_t i) const
//    {
//        return at(i);
//    }
	DataColumn* DataColumns::at(size_t i) const
	{
		return Vector<DataColumn>::at(i);
	}
	DataColumn* DataColumns::at(const String& columnName) const
	{
		for (uint i = 0; i < count(); i++)
		{
			DataColumn* column = at(i);
			if (String::equals(column->name(), columnName, true))
			{
				return column;
			}
		}
		return nullptr;
	}

	DataCell::DataCell(const DataColumn* column)
	{
		_column = column;
		_value = DbValue::nullValue();
	}
	DataCell::DataCell(const DataColumn* column, const Value value)
	{
		_column = column;
		_value = value;
	}
	DataCell::DataCell(const DataColumn* column, const DbValue& value)
	{
		_column = column;
		if (type() == Text && !value.isNullValue())
		{
			setStringValue(value.value().strValue);
		}
		else
		{
			_value = value.value();
		}

	}
    DataCell::DataCell(const DataColumn* column, bool value)
    {
        _column = column;
        switch (type())
        {
            case Database::Digital:
                _value.bValue = value;
                break;
            case Database::Integer8:
                _value.cValue = value ? 1 : 0;
                break;
            case Database::Integer16:
                _value.sValue = value ? 1 : 0;
                break;
            case Database::Integer32:
                _value.nValue = value ? 1 : 0;
                break;
            case Database::Float64:
                _value.dValue = value ? 1.0 : 0;
                break;
            case Database::Float32:
                _value.fValue = value ? 1.0f : 0;
                break;
            case Database::Integer64:
                _value.lValue = value ? 1 : 0;
                break;
            default:
                _value = DbValue::nullValue();
                break;
        }
    }
	DataCell::DataCell(const DataColumn* column, uint8_t value)
	{
		_column = column;
		switch (type())
		{
        case Database::Integer8:
            _value.cValue = value;
            break;
        case Database::Integer16:
            _value.sValue = value;
            break;
		case Database::Integer32:
			_value.nValue = value;
			break;
		case Database::Float64:
			_value.dValue = value;
			break;
		case Database::Float32:
			_value.fValue = (float)value;
			break;
		case Database::Integer64:
			_value.lValue = value;
			break;
		default:
			_value = DbValue::nullValue();
			break;
		}
	}
    DataCell::DataCell(const DataColumn* column, short value)
    {
        _column = column;
        switch (type())
        {
            case Database::Integer8:
                _value.cValue = (uint8_t)value;
                break;
            case Database::Integer16:
                _value.sValue = value;
                break;
            case Database::Integer32:
                _value.nValue = value;
                break;
            case Database::Float64:
                _value.dValue = value;
                break;
            case Database::Float32:
                _value.fValue = (float)value;
                break;
            case Database::Integer64:
                _value.lValue = value;
                break;
            default:
                _value = DbValue::nullValue();
                break;
        }
    }
    DataCell::DataCell(const DataColumn* column, ushort value) : DataCell(column, (short)value)
    {
    }
    DataCell::DataCell(const DataColumn* column, int value)
    {
        _column = column;
        switch (type())
        {
            case Database::Integer8:
                _value.cValue = (uint8_t)value;
                break;
            case Database::Integer16:
                _value.sValue = (short)value;
                break;
            case Database::Integer32:
                _value.nValue = value;
                break;
            case Database::Float64:
                _value.dValue = value;
                break;
            case Database::Float32:
                _value.fValue = (float)value;
                break;
            case Database::Integer64:
                _value.lValue = value;
                break;
            default:
                _value = DbValue::nullValue();
                break;
        }
    }
	DataCell::DataCell(const DataColumn* column, uint value) : DataCell(column, (int)value)
	{
	}
	DataCell::DataCell(const DataColumn* column, int64_t value)
	{
		_column = column;
		switch (type())
		{
        case Database::Integer8:
            _value.cValue = (uint8_t)value;
            break;
        case Database::Integer16:
            _value.sValue = (short)value;
            break;
		case Database::Integer32:
			_value.nValue = (int)value;
			break;
		case Database::Float64:
			_value.dValue = (double)value;
			break;
		case Database::Float32:
			_value.fValue = (float)value;
			break;
		case Database::Integer64:
			_value.lValue = value;
			break;
		default:
			_value = DbValue::nullValue();
			break;
		}
	}
	DataCell::DataCell(const DataColumn* column, uint64_t value) : DataCell(column, (int64_t)value)
	{
	}
	DataCell::DataCell(const DataColumn* column, double value)
	{
		_column = column;
		_value.dValue = value;
	}
	DataCell::DataCell(const DataColumn* column, const char* value)
	{
		_column = column;
		setStringValue(value);
	}
	DataCell::DataCell(const DataColumn* column, const String& value)
	{
		_column = column;
		setStringValue(value.c_str());
	}
	DataCell::DataCell(const DataColumn* column, const DateTime& value)
	{
		_column = column;
		_value.tValue = value.ticks();
	}
	DataCell::DataCell(const DataColumn* column, const TimeSpan& value)
	{
		_column = column;
		_value.tValue = value.ticks();
	}
	DataCell::~DataCell()
	{
		ValueTypes t = type();
		if (t == Text && !isNullValue())
		{
			assert(_value.strValue);
			delete[] _value.strValue;
			_value.strValue = nullptr;
		}
		_column = nullptr;
	}

	ValueTypes DataCell::type() const
	{
		return _column != nullptr ? _column->type() : Null;
	}

	const Value& DataCell::value() const
	{
		return _value;
	}

	const String DataCell::valueStr(bool hasQuote) const
	{
        static const char* NullStr = "null";
        
		if (isNullValue())
			return NullStr;

		switch (type())
		{
		case Null:
			return NullStr;
        case Digital:
            return Boolean(_value.bValue).toString();
        case Integer8:
            return UInt8(_value.cValue).toString();
        case Integer16:
            return Int16(_value.sValue).toString();
		case Integer32:
			return Int32(_value.nValue).toString();
		case Integer64:
			return Int64(_value.lValue).toString();
		case Float64:
			return Double(_value.dValue).toString();
		case Text:
			return hasQuote ?
				String::convert("'%s'", _value.strValue) :
				_value.strValue;
		case Date:
		case Timestamp:
		{
			DateTime time(_value.tValue);
			return hasQuote ?
				String::convert("'%s'", time.toString().c_str()) :
				time.toString();
		}
		case Float32:
			return Float(_value.fValue).toString();
		default:
			assert(false);
            return NullStr;
		}
	}

	bool DataCell::matchColumnName(const char* columnName) const
	{
		return (_column != nullptr && columnName != nullptr) ?
			String::equals(_column->name(), columnName, true) : false;
	}

	const String DataCell::columnName() const
	{
        return _column != nullptr ? _column->name() : String::Empty;
	}
    const DataColumn* DataCell::column() const
    {
        return _column;
    }

	void DataCell::setStringValue(Value& value, const char* str)
	{
		if (str != nullptr)
		{
			value.strValue = new char[strlen(str) + 1];
			strcpy(value.strValue, str);
		}
		else
		{
			value.strValue = new char[1];
			value.strValue[0] = '\0';
		}
	}
	bool DataCell::isNullValue() const
	{
		return DbValue::isNullValue(_value);
	}
	void DataCell::setNullValue()
	{
		_value = DbValue::nullValue();
	}

    void DataCell::copyFrom(const DataCell* cell)
    {
        if(cell->type() != Text)
            _value = cell->_value;
        else
        {
            if (type() == Text && !isNullValue())
            {
                assert(_value.strValue);
                delete[] _value.strValue;
                _value.strValue = nullptr;
            }
            setStringValue(_value, cell->_value.strValue);
        }
    }

	void DataCell::setStringValue(const char* str)
	{
		setStringValue(_value, str);
	}

	DataCells::DataCells(bool autoDelete, uint capacity) : Vector<DataCell>(autoDelete, capacity)
	{
	}

	DataCell* DataCells::operator[](const char* i) const
	{
		return at(i);
	}
	DataCell* DataCells::at(uint i) const
	{
		return Vector < DataCell >::at(i);
	}
	DataCell* DataCells::at(const String& columnName) const
	{
		//for (size_t i = 0; i < count(); i++)
		//{
		//	DataCell* cell = at(i);
		//	if (cell->matchColumnName(columnName.c_str()))
		//	{
		//		return cell;
		//	}
		//}
		//return nullptr;
		uint position;
		if (_positions.at(columnName.toLower(), position))
		{
			return at(position);
		}
		return nullptr;
	}

	const Value DataCells::cellValue(const String& columnName) const
	{
		const DataCell* cell = at(columnName);
		if (cell == nullptr)
			return DbValue::nullValue();

		return cell->value();
	}
	bool DataCells::hasColumn(const String& columnName) const
	{
		// return at(columnName) != nullptr;
		return _positions.containsKey(columnName.toLower());
	}

	void DataCells::add(const DataCell* cell)
	{
		uint position = count();
		Vector < DataCell >::add(cell);

		_positions.add(cell->columnName().toLower(), position);
	}

	DataRow::DataRow()
	{
	}
	DataRow::~DataRow()
	{
	}

	void DataRow::addCell(const DataCell* cell)
	{
        _cells.add(cell);
	}

	const DataCells* DataRow::cells() const
	{
		return &_cells;
	}

    void DataRow::replaceCell(const DataCell* cell)
    {
        DataCell* old = _cells.at(cell->columnName());
        if(old == nullptr)
            _cells.add(cell);
        else
            old->copyFrom(cell);
    }

	DataTable::DataTable()
	{
        _name = String::Empty;
        _totalCount = 0;
	}
	DataTable::DataTable(const String& name)
	{
		setName(name);
	}
	DataTable::DataTable(const char* name)
	{
		setName(name);
	}
	DataTable::~DataTable()
	{
		clear();
	}

	const String& DataTable::name() const
	{
		return _name;
	}

	void DataTable::setName(const String& name)
	{
		_name = name;
	}

	void DataTable::addRow(const DataRow* row)
	{
		_rows.add(row);
	}
	const DataRows* DataTable::rows() const
	{
		return &_rows;
	}
	uint DataTable::rowCount() const
	{
		return _rows.count();
	}

	void DataTable::addColumn(const DataColumn* column)
	{
		_columns.add(column);
	}
	const DataColumns* DataTable::columns() const
	{
		return &_columns;
	}
    DataColumn* DataTable::getColumn(const String& columnName) const
    {
        for (uint i=0; i<_columns.count(); i++)
        {
            DataColumn* column = _columns[i];
            if(column->name() == columnName)
                return column;
        }
        return nullptr;
    }

	uint DataTable::columnCount() const
	{
		return _columns.count();
	}

	void DataTable::clear()
	{
        _name = String::Empty;
		_rows.clear();
		_columns.clear();
	}
    void DataTable::clearRows()
    {
        _rows.clear();
    }

	const String DataTable::anyColumnNameStr(bool hasTableName) const
	{
		String sb;
		if (hasTableName)
		{
			sb.append(name());
			sb.append(".");
		}
		sb.append("*");
		return sb;
	}
	const String DataTable::columnNameStr(bool hasTableName, const StringArray* excludedNames, const char* splitStr) const
	{
		String sb;
		for (uint i = 0; i < columnCount(); i++)
		{
			String cname = _columns.at(i)->name();
			if (excludedNames == nullptr ||
				(excludedNames != nullptr && !excludedNames->contains(cname, true)))
			{
				if (i > 0)
				{
					sb.append(splitStr);
				}
				if (hasTableName)
				{
					sb.append(name());
					sb.append(".");
				}
				sb.append(cname);
			}
		}
		return sb;
	}
    
    int DataTable::totalCount() const
    {
        return _totalCount;
    }
    void DataTable::setTotalCount(int totalCount)
    {
        _totalCount = totalCount >= 0 ? totalCount : 0;
    }
    void DataTable::toJsonNode(JsonNode& node) const
    {
        node = JsonNode(JsonNode::TypeArray);
        for (uint i=0; i<rowCount(); i++)
        {
            const DataRow* row = rows()->at(i);
            const DataCells* cells = row->cells();
            JsonNode rowNode;
            for (uint j=0; j<cells->count(); j++)
            {
                const DataCell* cell = cells->at(j);
                rowNode.add(JsonNode(cell->columnName(), !cell->isNullValue() ? cell->valueStr() : String::Empty));
            }
            node.add(rowNode);
        }
    }
    String DataTable::toJsonString() const
    {
        JsonNode node;
        toJsonNode(node);
        return node.toString();
    }

    DataTables::DataTables(uint capacity) : Vector<DataTable>(capacity)
    {
    }
    bool DataTables::contains(const String& tableName) const
    {
        for (uint i=0; i<count(); i++)
        {
            const DataTable* table = at(i);
            if(table->name() == tableName)
                return true;
        }
        return false;
    }
    DataTable* DataTables::getTable(const String& tableName) const
    {
        for (uint i=0; i<count(); i++)
        {
            DataTable* table = at(i);
            if(table->name() == tableName)
                return table;
        }
        return nullptr;
    }
}
