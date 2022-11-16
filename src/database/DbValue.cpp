#include "database/DbValue.h"

namespace Database
{
	DbValue::DbValue(ValueTypes type)
	{
		_type = type;
		_value = nullValue();
	}
	DbValue::DbValue(ValueTypes type, const Value& value) : DbValue(type)
	{
		operator=(value);
	}
	DbValue::~DbValue()
	{
		if (_type == Text && !isNullValue())
		{
			assert(_value.strValue);
			delete[] _value.strValue;
			_value.strValue = NULL;
		}
	}

	ValueTypes DbValue::type() const
	{
		return _type;
	}
	const Value& DbValue::value() const
	{
		return _value;
	}

	bool DbValue::isNullValue(const Value& _value)
	{
		return (_value.ulValue == UINT64_MAX);
	}
	bool DbValue::isNullValue() const
	{
		return isNullValue(_value);
	}
	Value DbValue::nullValue()
	{
		Value _value;
		_value.ulValue = UINT64_MAX;
		return _value;
	}
	void DbValue::setNullValue()
	{
		_value = nullValue();
	}

	void DbValue::copyFrom(const DbValue* value)
	{
		_type = value->_type;
		operator=(value->value());
	}

	bool DbValue::operator==(const Value& value)
	{
		DbValue temp(type(), value);
		return operator==(temp);
	}
	bool DbValue::operator!=(const Value& value)
	{
		return !operator==(value);
	}
	bool DbValue::operator==(const DbValue& value)
	{
		if (type() != value.type())
			return false;

		if (isNullValue() && value.isNullValue())
			return true;
		else if (isNullValue() && !value.isNullValue())
			return false;
		else if (!isNullValue() && value.isNullValue())
			return false;
		else
		{
			switch (type())
			{
			case Database::Null:
				return true;
            case Database::Integer8:
                return _value.cValue && value._value.cValue;
                break;
            case Database::Integer16:
                return _value.sValue && value._value.sValue;
                break;
			case Database::Integer32:
				return _value.nValue && value._value.nValue;
				break;
			case Database::Float64:
				return _value.dValue && value._value.dValue;
			case Database::Text:
				return strcmp(_value.strValue, value._value.strValue) == 0;
			case Database::Float32:
				return _value.fValue && value._value.fValue;
				break;
			case Database::Blob:
				// todo: compare blob to stream.
				break;
			case Database::Date:
			case Database::Timestamp:
				return _value.tValue && value._value.tValue;
			case Database::Integer64:
				return _value.lValue && value._value.lValue;
				break;
			default:
				break;
			}
		}
		return false;
	}
	bool DbValue::operator!=(const DbValue& value)
	{
		return !operator==(value);
	}
	void DbValue::operator=(const Value& value)
	{
		if (_type == Text && !isNullValue(value))
		{
			setStringValue(value.strValue);
		}
		else
		{
			_value = value;
		}
	}
	void DbValue::operator=(const DbValue& value)
	{
		copyFrom(&value);
	}
	void DbValue::operator=(const String& value)
	{
		if (_type == Text)
		{
			setStringValue(value);
		}
		else if (_type == Date ||
			_type == Timestamp)
		{
			DateTime time;
			if (DateTime::parse(value, time))
			{
				_value.tValue = time.ticks();
			}
		}
		else if (_type == Float64)
		{
			Double::parse(value, _value.dValue);
		}
        else if (_type == Integer8)
        {
            Byte::parse(value, _value.cValue);
        }
        else if (_type == Integer16)
        {
            Int16::parse(value, _value.sValue);
        }
		else if (_type == Integer32)
		{
			Int32::parse(value, _value.nValue);
		}
		else if (_type == Integer64)
		{
			Int64::parse(value, _value.lValue);
		}
		else if (_type == Float32)
		{
			Float::parse(value, _value.fValue);
		}
		else if (_type == Null)
		{
			setNullValue();
		}
		else
		{
			assert(false);
		}
	}
	void DbValue::operator=(const uint8_t& value)
	{
        if (_type == Integer8)
        {
            _value.cValue = value;
        }
        else
        {
            this->operator=((int)value);
        }
	}
	void DbValue::operator=(const short& value)
	{
        if (_type == Integer16)
        {
            _value.sValue = value;
        }
        else
        {
            this->operator=((int)value);
        }
	}
	void DbValue::operator=(const int& value)
	{
		if (_type == Integer32)
		{
			_value.nValue = value;
		}
	}
	void DbValue::operator=(const int64_t& value)
	{
		if (_type == Integer64)
		{
			_value.lValue = value;
		}
	}
	void DbValue::operator=(const float& value)
	{
		if (_type == Float32)
		{
			_value.fValue = value;
		}
	}
	void DbValue::operator=(const double& value)
	{
		if (_type == Float64)
		{
			_value.dValue = value;
		}
	}
	void DbValue::operator=(const bool& value)
	{
		if (_type == Digital)
		{
			_value.bValue = value;
		}
	}
	void DbValue::operator=(const char* value)
	{
		setStringValue(value);
	}
	void DbValue::operator=(const DateTime& value)
	{
		if (_type == Date ||
			_type == Timestamp)
		{
			_value.tValue = value.ticks();
		}
	}
	void DbValue::setStringValue(const String& str)
	{
		setStringValue(str.c_str());
	}
	void DbValue::setStringValue(const char* str)
	{
		if (str == NULL)
			return;

		if (_type == Text)
		{
			if (!isNullValue() && _value.strValue != NULL)
			{
				delete[] _value.strValue;
			}
			size_t len = strlen(str);
			if (len > 0)
			{
				_value.strValue = new char[len + 1];
				strcpy(_value.strValue, str);
			}
			else
			{
				_value.strValue = new char[1];
				_value.strValue[0] = '\0';
			}
		}
	}

	void DbValue::write(Stream* stream, bool bigEndian) const
	{
		stream->writeByte(_type);
		ValueFlag flag = isNullValue(_value) ? NullFlag : NormalFlag;
		stream->writeByte(flag);
		if (flag == NormalFlag)
		{
			switch (_type)
			{
			case Database::Null:
				break;
            case Database::Integer8:
                stream->writeByte(_value.bValue);
                break;
            case Database::Integer16:
                stream->writeInt16(_value.sValue, bigEndian);
                break;
			case Database::Integer32:
				stream->writeInt32(_value.nValue, bigEndian);
				break;
			case Database::Float64:
				stream->writeDouble(_value.dValue, bigEndian);
				break;
			case Database::Text:
				stream->writeStr(_value.strValue);
				break;
			case Database::Float32:
				stream->writeFloat(_value.fValue, bigEndian);
				break;
			case Database::Blob:
				// todo: write blob to stream.
				break;
			case Database::Date:
			case Database::Timestamp:
			{
				DateTime time(_value.tValue);
				time.write(stream);
			}
				break;
			case Database::Integer64:
				stream->writeInt64(_value.lValue, bigEndian);
				break;
			default:
				break;
			}
		}
	}
	void DbValue::read(Stream* stream, bool bigEndian)
	{
		_type = (ValueTypes)stream->readByte();
		ValueFlag flag = (ValueFlag)stream->readByte();
		if (flag == NullFlag)
			_value = nullValue();
		else
		{
			switch (_type)
			{
			case Database::Null:
				break;
            case Database::Integer8:
                _value.cValue = stream->readByte();
                break;
            case Database::Integer16:
                _value.sValue = stream->readInt16(bigEndian);
                break;
			case Database::Integer32:
				_value.nValue = stream->readInt32(bigEndian);
				break;
			case Database::Float64:
				_value.dValue = stream->readDouble(bigEndian);
				break;
			case Database::Text:
				setStringValue(stream->readStr());
				break;
			case Database::Float32:
				_value.fValue = stream->readFloat(bigEndian);
				break;
			case Database::Blob:
				// todo: read blob from stream.
				break;
			case Database::Date:
			case Database::Timestamp:
			{
				DateTime time;
				time.read(stream, bigEndian);
				_value.tValue = time.ticks();
			}
				break;
			case Database::Integer64:
				_value.lValue = stream->readInt64(bigEndian);
				break;
			default:
				assert(false);
				break;
			}
		}
	}
	void DbValue::writeStr(Stream* stream, String::StreamLength lengthCount) const
	{
		assert(_type == Text);
		if (_type == Text)
		{
			stream->writeByte(_type);
			ValueFlag flag = isNullValue(_value) ? NullFlag : NormalFlag;
			stream->writeByte(flag);
			if (flag == NormalFlag)
			{
				stream->writeStr(_value.strValue, lengthCount);
			}
		}
	}
	void DbValue::readStr(Stream* stream, String::StreamLength lengthCount)
	{
		_type = (ValueTypes)stream->readByte();
		ValueFlag flag = (ValueFlag)stream->readByte();
		if (flag == NullFlag)
			_value = nullValue();
		else
		{
			setStringValue(stream->readStr(lengthCount));
		}
	}
	void DbValue::writeFixedLengthStr(Stream* stream, int length) const
	{
		assert(_type == Text);
		if (_type == Text)
		{
			stream->writeByte(_type);
			ValueFlag flag = isNullValue(_value) ? NullFlag : NormalFlag;
			stream->writeByte(flag);
			if (flag == NormalFlag)
			{
				stream->writeFixedLengthStr(_value.strValue, length);
			}
		}
	}
	void DbValue::readFixedLengthStr(Stream* stream, int length)
	{
		_type = (ValueTypes)stream->readByte();
		ValueFlag flag = (ValueFlag)stream->readByte();
		if (flag == NullFlag)
			_value = nullValue();
		else
		{
			setStringValue(stream->readFixedLengthStr(length));
		}
	}
	void DbValue::writeByte(Stream* stream) const
	{
        if (_type == Integer32)
		{
			stream->writeByte(_type);
			ValueFlag flag = isNullValue(_value) ? NullFlag : NormalFlag;
			stream->writeByte(flag);
			if (flag == NormalFlag)
			{
				stream->writeByte(_value.nValue);
			}
		}
	}
	void DbValue::readByte(Stream* stream)
	{
		_type = (ValueTypes)stream->readByte();
		ValueFlag flag = (ValueFlag)stream->readByte();
		if (flag == NullFlag)
			_value = nullValue();
		else
		{
			_value.nValue = stream->readByte();
		}
	}

	void DbValue::writeBCDDateTime(Stream* stream, bool includedSec, bool includedMs) const
	{
		assert(_type == Date ||
			_type == Timestamp);
		if (_type == Date ||
			_type == Timestamp)
		{
			stream->writeByte(_type);
			ValueFlag flag = isNullValue(_value) ? NullFlag : NormalFlag;
			stream->writeByte(flag);
			if (flag == NormalFlag)
			{
				DateTime time(_value.tValue);
				time.writeBCDDateTime(stream, includedSec, includedMs);
			}
		}
	}
	void DbValue::readBCDDateTime(Stream* stream, bool includedSec, bool includedMs)
	{
		_type = (ValueTypes)stream->readByte();
		ValueFlag flag = (ValueFlag)stream->readByte();
		if (flag == NullFlag)
			_value = nullValue();
		else
		{
			DateTime time;
			time.readBCDDateTime(stream, includedSec, includedMs);
			_value.tValue = time.ticks();
		}
	}
}
