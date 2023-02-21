//
//  Variant.cpp
//  common
//
//  Created by baowei on 2017/11/25.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "data/Variant.h"

namespace Data {
    const Variant Variant::NullValue;

    Variant::Variant(Type type) : _type(type), _value(), _isNullValue(true) {
    }

    Variant::Variant(Type type, bool value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, int8_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, uint8_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, int16_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, uint16_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, int32_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, uint32_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, int64_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, uint64_t value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, float value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, double value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const char *value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const String &value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const DateTime &value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const TimeSpan &value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const ByteArray &value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(Type type, const Value &value) : Variant(type) {
        setValue(value);
    }

    Variant::Variant(bool value) : Variant(Digital, value) {
    }

    Variant::Variant(int8_t value) : Variant(Integer8, value) {
    }

    Variant::Variant(uint8_t value) : Variant(UInteger8, value) {
    }

    Variant::Variant(int16_t value) : Variant(Integer16, value) {
    }

    Variant::Variant(uint16_t value) : Variant(UInteger16, value) {
    }

    Variant::Variant(int32_t value) : Variant(Integer32, value) {
    }

    Variant::Variant(uint32_t value) : Variant(UInteger32, value) {
    }

    Variant::Variant(int64_t value) : Variant(Integer64, value) {
    }

    Variant::Variant(uint64_t value) : Variant(UInteger64, value) {
    }

    Variant::Variant(float value) : Variant(Float32, value) {
    }

    Variant::Variant(double value) : Variant(Float64, value) {
    }

    Variant::Variant(const char *value) : Variant(Text, value) {
    }

    Variant::Variant(const String &value) : Variant(Text, value) {
    }

    Variant::Variant(const DateTime &value) : Variant(Date, value) {
    }

    Variant::Variant(const TimeSpan &value) : Variant(Timestamp, value) {
    }

    Variant::Variant(const ByteArray &value) : Variant(Blob, value) {
    }

    Variant::Variant(const Variant &v) : Variant(Null) {
        operator=(v);
    }

    Variant::~Variant() {
        clearValue();
    }

    void Variant::evaluates(const Variant &other) {
        clearValue();

        _type = other._type;
        _isNullValue = other._isNullValue;
        changeValue(other.type(), other.value(), _type, _value);
    }

    bool Variant::equals(const Variant &other) const {
        return _isNullValue == other._isNullValue &&
               equals(type(), value(), other.type(), other.value());
    }

    bool Variant::equals(const bool &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const int8_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const uint8_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const int16_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const uint16_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const int32_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const uint32_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const int64_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const uint64_t &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const float &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const double &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const char *other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), String(other));
    }

    bool Variant::equals(const String &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const DateTime &other) const {
        return equals(type(), value(), other);
    }

    bool Variant::equals(const TimeSpan &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    bool Variant::equals(const ByteArray &other) const {
        if (_isNullValue) {
            return false;
        }
        return equals(type(), value(), other);
    }

    Variant &Variant::operator=(const Variant &other) {
        evaluates(other);
        return *this;
    }

    Variant &Variant::operator=(const bool &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const int8_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const uint8_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const int16_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const uint16_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const int32_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const uint32_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const int64_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const uint64_t &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const float &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const double &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const String &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const DateTime &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const TimeSpan &value) {
        setValue(value);
        return *this;
    }

    Variant &Variant::operator=(const ByteArray &value) {
        setValue(value);
        return *this;
    }

    Variant::operator bool() const {
        bool v;
        getValue(v);
        return v;
    }

    Variant::operator int8_t() const {
        int8_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator uint8_t() const {
        uint8_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator int16_t() const {
        int16_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator uint16_t() const {
        uint16_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator int32_t() const {
        int32_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator uint32_t() const {
        uint32_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator int64_t() const {
        int64_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator uint64_t() const {
        uint64_t v = 0;
        getValue(v);
        return v;
    }

    Variant::operator float() const {
        float v = 0.0f;
        getValue(v);
        return v;
    }

    Variant::operator double() const {
        double v = 0.0;
        getValue(v);
        return v;
    }

    Variant::operator String() const {
        String v;
        getValue(v);
        return v;
    }

    Variant::operator DateTime() const {
        DateTime v;
        getValue(v);
        return v;
    }

    Variant::operator TimeSpan() const {
        TimeSpan v;
        getValue(v);
        return v;
    }

    Variant::operator ByteArray() const {
        ByteArray v;
        getValue(v);
        return v;
    }

    bool Variant::isValueEqual(Type type, const Value &value1, const Value &value2) {
        switch (type) {
            case Null:
                return true;
            case Digital:
                return value1.bValue == value2.bValue;
            case Integer8:
                return value1.cValue == value2.cValue;
            case UInteger8:
                return value1.ucValue == value2.ucValue;
            case Integer16:
                return value1.sValue == value2.sValue;
            case UInteger16:
                return value1.usValue == value2.usValue;
            case Integer32:
                return value1.nValue == value2.nValue;
            case UInteger32:
                return value1.unValue == value2.unValue;
            case Integer64:
                return value1.lValue == value2.lValue;
            case UInteger64:
                return value1.ulValue == value2.ulValue;
            case Float32:
                return value1.fValue == value2.fValue;
            case Float64:
                return value1.dValue == value2.dValue;
            case Text:
                return equalsTextValue(value1.strValue, value2.strValue);
            case Date:
                return value1.dateValue == value2.dateValue;
            case Timestamp:
                return value1.timeValue == value2.timeValue;
            case Blob:
                return equalsBlobValue(value1.blobValue, value2.blobValue);
            default:
                break;
        }
        return false;
    }

    bool Variant::setStringValue(const String &str) {
        return setStringValue(str, _type, _value);
    }

    bool Variant::setStringValue(const String &str, Type type, Value &value) {
        if (type == Text) {
            delete[] value.strValue;
            value.strValue = new char[str.length() + 1];
            strcpy(value.strValue, str);
            return true;
        }
        return false;
    }

    void Variant::setByteArrayValue(const uint8_t *buffer) {
        setByteArrayValue(buffer, _type, _value);
    }

    void Variant::setByteArrayValue(const uint8_t *buffer, Type type, Value &value) {
        if (type == Blob) {
            delete[] value.blobValue;
            size_t count = blobCount(buffer);
            if (count > 0) {
                ByteArray array(buffer + 4, count);
                setByteArrayValue(array, type, value);
            }
        }
    }

    void Variant::setByteArrayValue(const ByteArray &buffer) {
        setByteArrayValue(buffer, _type, _value);
    }

    void Variant::setByteArrayValue(const ByteArray &buffer, Type type, Value &value) {
        if (type == Blob) {
            delete[] value.blobValue;
            size_t count = buffer.count();
            if (count > 0) {
                uint8_t countBuffer[4];
                countBuffer[0] = (uint8_t) ((count & 0xFF000000) >> 24);
                countBuffer[1] = (uint8_t) ((count & 0x00FF0000) >> 16);
                countBuffer[2] = (uint8_t) ((count & 0x0000FF00) >> 8);
                countBuffer[3] = (uint8_t) ((count & 0x000000FF) >> 0);
                value.blobValue = new uint8_t[count + 4];
                memcpy(value.blobValue, countBuffer, sizeof(countBuffer));
                memcpy(value.blobValue + 4, buffer.data(), count);
            }
        }
    }

    bool Variant::isNullType(const Variant &other) {
        return isNullType(other.type());
    }

    bool Variant::isNullValue(const Variant &other) {
        return other._isNullValue;
    }

    bool Variant::isNullType() const {
        return isNullType(*this);
    }

    bool Variant::isNullValue() const {
        return isNullValue(*this);
    }

    void Variant::setNullValue() {
        _isNullValue = true;
        clearValue();
    }

    Variant::Type Variant::type() const {
        return _type;
    }

    void Variant::setValue(const Value &v) {
        if (type() != Null) {
            _isNullValue = false;

//            Variant::Value oldValue{.lValue = 0};
//            changeValue(type(), value(), type(), oldValue);

            setValueInner(v);

//            clearValue(type(), oldValue);
        } else {
            _isNullValue = true;

            clearValue(type(), _value);
        }
    }

    void Variant::clearValue() {
        clearValue(_type, _value);
    }

    template<class T>
    bool Variant::setValue(const T &v) {
        if (equals(v))
            return false;

        Value temp;
        if (changeValue(v, type(), temp)) {
            setValue(temp);

            clearValue(type(), temp);
            return true;
        }
        return false;
    }

    bool Variant::setValue(Type type, const Value &v) {
        return changeValue(type, v, _type, _value);
    }

    bool Variant::setValue(const bool &v) {
        return setValue<bool>(v);
    }

    bool Variant::setValue(const int8_t &v) {
        return setValue<int8_t>(v);
    }

    bool Variant::setValue(const uint8_t &v) {
        return setValue<uint8_t>(v);
    }

    bool Variant::setValue(const int16_t &v) {
        return setValue<int16_t>(v);
    }

    bool Variant::setValue(const uint16_t &v) {
        return setValue<uint16_t>(v);
    }

    bool Variant::setValue(const int32_t &v) {
        return setValue<int32_t>(v);
    }

    bool Variant::setValue(const uint32_t &v) {
        return setValue<uint32_t>(v);
    }

    bool Variant::setValue(const int64_t &v) {
        return setValue<int64_t>(v);
    }

    bool Variant::setValue(const uint64_t &v) {
        return setValue<uint64_t>(v);
    }

    bool Variant::setValue(const float &v) {
        return setValue<float>(v);
    }

    bool Variant::setValue(const double &v) {
        return setValue<double>(v);
    }

    bool Variant::setValue(const String &v) {
        return setValue<String>(v);
    }

    bool Variant::setValue(const char *v) {
        if (v != nullptr) {
            return setValue(String(v));
        } else {
            setNullValue();
            return true;
        }
    }

    bool Variant::setValue(const DateTime &v) {
        return setValue<DateTime>(v);
    }

    bool Variant::setValue(const TimeSpan &v) {
        return setValue<TimeSpan>(v);
    }

    bool Variant::setValue(const ByteArray &v) {
        return setValue<ByteArray>(v);
    }

    void Variant::setValueInner(const Value &value) {
        if (_type == Text) {
            setStringValue(value.strValue);
        } else if (_type == Blob) {
            setByteArrayValue(value.blobValue);
        } else {
            _value = value;
        }
    }

    bool Variant::isAnalogValue() const {
        return isAnalogValue(type());
    }

    bool Variant::isDigitalValue() const {
        return isDigitalValue(type());
    }

    bool Variant::isIntegerValue() const {
        return isIntegerValue(type());
    }

    bool Variant::isFloatValue() const {
        return isFloatValue(type());
    }

    bool Variant::isStringValue() const {
        return isStringValue(type());
    }

    bool Variant::isDateTimeValue() const {
        return isDateTimeValue(type());
    }

    size_t Variant::valueSize() const {
        return valueSize(type(), value());
    }

    const Variant::Value &Variant::value() const {
        return _value;
    }

    bool Variant::getValue(Variant::Type type, Variant::Value &value) const {
        switch (_type) {
            case Null:
                break;
            case Digital:
                return changeValue<bool>(_value.bValue, type, value);
            case Integer8:
                return changeValue<int8_t>(_value.cValue, type, value);
            case UInteger8:
                return changeValue<uint8_t>(_value.ucValue, type, value);
            case Integer16:
                return changeValue<int16_t>(_value.sValue, type, value);
            case UInteger16:
                return changeValue<uint16_t>(_value.usValue, type, value);
            case Integer32:
                return changeValue<int32_t>(_value.nValue, type, value);
            case UInteger32:
                return changeValue<uint32_t>(_value.unValue, type, value);
            case Integer64:
                return changeValue<int64_t>(_value.lValue, type, value);
            case UInteger64:
                return changeValue<uint64_t>(_value.ulValue, type, value);
            case Float32:
                return changeValue<float>(_value.fValue, type, value);
            case Float64:
                return changeValue<double>(_value.dValue, type, value);
            case Text:
                return changeValue(String(_value.strValue), type, value);
            case Date:
                return changeValue(_value.dateValue, type, value);
            case Timestamp:
                return changeValue(_value.timeValue, type, value);
            case Blob:
                return changeBlobValue(_value.blobValue, type, value);
            default:
                break;
        }
        return false;
    }

    template<class T>
    bool Variant::getValue(T &v) const {
        return changeValue(type(), value(), v);
    }

    bool Variant::getValue(bool &value) const {
        return getValue<bool>(value);
    }

    bool Variant::getValue(int8_t &value) const {
        return getValue<int8_t>(value);
    }

    bool Variant::getValue(uint8_t &value) const {
        return getValue<uint8_t>(value);
    }

    bool Variant::getValue(int16_t &value) const {
        return getValue<int16_t>(value);
    }

    bool Variant::getValue(uint16_t &value) const {
        return getValue<uint16_t>(value);
    }

    bool Variant::getValue(int32_t &value) const {
        return getValue<int32_t>(value);
    }

    bool Variant::getValue(uint32_t &value) const {
        return getValue<uint32_t>(value);
    }

    bool Variant::getValue(int64_t &value) const {
        return getValue<int64_t>(value);
    }

    bool Variant::getValue(uint64_t &value) const {
        return getValue<uint64_t>(value);
    }

    bool Variant::getValue(float &value) const {
        return getValue<float>(value);
    }

    bool Variant::getValue(double &value) const {
        return getValue<double>(value);
    }

    bool Variant::getValue(String &v) const {
        return changeValue(type(), value(), v);
    }

    String Variant::valueStr() const {
        String value;
        if (getValue(value))
            return value;
        return String::Empty;
    }

    bool Variant::getValue(DateTime &v) const {
        return changeValue(type(), value(), v);
    }

    bool Variant::getValue(TimeSpan &v) const {
        return changeValue(type(), value(), v);
    }

    bool Variant::getValue(ByteArray &v) const {
        return changeValue(type(), value(), v);
    }

    bool Variant::equals(Type type, const Value &value, const String &v) {
        switch (type) {
            case Null:
                return false;
            case Digital: {
                bool temp;
                return Boolean::parse(v, temp) && value.bValue == temp;
            }
            case Integer8: {
                int8_t temp;
                return Int8::parse(v, temp) && value.cValue == temp;
            }
            case UInteger8: {
                uint8_t temp;
                return UInt8::parse(v, temp) && value.ucValue == temp;
            }
            case Integer16: {
                int16_t temp;
                return Int16::parse(v, temp) && value.sValue == temp;
            }
            case UInteger16: {
                uint16_t temp;
                return UInt16::parse(v, temp) && value.usValue == temp;
            }
            case Integer32: {
                int32_t temp;
                return Int32::parse(v, temp) && value.nValue == temp;
            }
            case UInteger32: {
                uint32_t temp;
                return UInt32::parse(v, temp) && value.unValue == temp;
            }
            case Integer64: {
                int64_t temp;
                return Int64::parse(v, temp) && value.lValue == temp;
            }
            case UInteger64: {
                uint64_t temp;
                return UInt64::parse(v, temp) && value.ulValue == temp;
            }
            case Float32: {
                float temp;
                return Float::parse(v, temp) && value.fValue == temp;
            }
            case Float64: {
                double temp;
                return Double::parse(v, temp) && value.dValue == temp;
            }
            case Text:
                return equalsTextValue(value.strValue, v.c_str());
            case Date: {
                DateTime temp;
                return DateTime::parse(v, temp) && value.dateValue == temp.ticks();
            }
            case Timestamp: {
                TimeSpan temp;
                return TimeSpan::parse(v, temp) && value.timeValue == temp.ticks();
            }
            case Blob:
            default:
                return false;
        }
        return true;
    }

    bool Variant::equals(Type type, const Value &value, const DateTime &v) {
        if (type == Date) {
            return value.dateValue == v.ticks();
        } else if (type == Text) {
            DateTime time;
            if (DateTime::parse(value.strValue, time)) {
                return v == time;
            }
        }
        return false;
    }

    bool Variant::equals(Type type, const Value &value, const TimeSpan &v) {
        if (type == Timestamp) {
            return value.timeValue == v.ticks();
        } else if (type == Text) {
            TimeSpan time;
            if (TimeSpan::parse(value.strValue, time)) {
                return v == time;
            }
        }
        return false;
    }

    bool Variant::equals(Type type, const Value &value, const ByteArray &v) {
        if (type == Blob) {
            size_t count = blobCount(value.blobValue);
            if (v.count() == count) {
                const uint8_t *buffer = blobBuffer(value.blobValue);
                return memcmp(buffer, v.data(), count) == 0;
            }
        }
        return false;
    }

    bool Variant::changeValue(Type type, const Value &value, Type destType, Value &destValue) {
        if (type == destType) {
            if (type == Text) {
                setStringValue(value.strValue, destType, destValue);
            } else if (type == Blob) {
                setByteArrayValue(value.blobValue, destType, destValue);
            } else {
                destValue = value;
            }
            return true;
        } else {
            switch (type) {
                case Null:
                    return false;
                case Digital:
                    return changeValue<bool>(value.bValue, destType, destValue);
                case Integer8:
                    return changeValue<int8_t>(value.cValue, destType, destValue);
                case UInteger8:
                    return changeValue<uint8_t>(value.ucValue, destType, destValue);
                case Integer16:
                    return changeValue<int16_t>(value.sValue, destType, destValue);
                case UInteger16:
                    return changeValue<uint16_t>(value.usValue, destType, destValue);
                case Integer32:
                    return changeValue<int32_t>(value.nValue, destType, destValue);
                case UInteger32:
                    return changeValue<uint32_t>(value.unValue, destType, destValue);
                case Integer64:
                    return changeValue<int64_t>(value.lValue, destType, destValue);
                case UInteger64:
                    return changeValue<uint64_t>(value.ulValue, destType, destValue);
                case Float32:
                    return changeValue<float>(value.fValue, destType, destValue);
                case Float64:
                    return changeValue<double>(value.dValue, destType, destValue);
                case Text:
                    return changeValue(String(value.strValue), destType, destValue);
                case Date:
                    return changeValue(DateTime(value.dateValue), destType, destValue);
                case Timestamp:
                    return changeValue(TimeSpan(value.timeValue), destType, destValue);
                case Blob:
                default:
                    return false;
            }
        }
        return false;
    }

    bool Variant::equals(Type type1, const Value &value1, Type type2, const Value &value2) {
        if (type1 == Null && type2 == Null) {
            return true;
        }

        Value destValue;
        if (!changeValue(type1, value1, type2, destValue))
            return false;

        switch (type2) {
            case Null:
                return false;
            case Digital:
                return value2.bValue == destValue.bValue;
            case Integer8:
                return value2.cValue == destValue.cValue;
            case UInteger8:
                return value2.ucValue == destValue.ucValue;
            case Integer16:
                return value2.sValue == destValue.sValue;
            case UInteger16:
                return value2.usValue == destValue.usValue;
            case Integer32:
                return value2.nValue == destValue.nValue;
            case UInteger32:
                return value2.unValue == destValue.unValue;
            case Integer64:
                return value2.lValue == destValue.lValue;
            case UInteger64:
                return value2.ulValue == destValue.ulValue;
            case Float32:
                return value2.fValue == destValue.fValue;
            case Float64:
                return value2.dValue == destValue.dValue;
            case Text:
                return equalsTextValue(value2.strValue, destValue.strValue);
            case Date:
                return value2.dateValue == destValue.dateValue;
            case Timestamp:
                return value2.timeValue == destValue.timeValue;
            case Blob:
                return equalsBlobValue(value2.blobValue, destValue.blobValue);
            default:
                return false;
        }
        return true;
    }

    bool Variant::changeValue(const String &v, Type type, Value &value) {
        if (v.isNullOrEmpty())
            return false;

        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = Boolean::parse(v, value.bValue);
                break;
            case Integer8:
                changed = Int8::parse(v, value.cValue);
                break;
            case UInteger8:
                changed = UInt8::parse(v, value.ucValue);
                break;
            case Integer16:
                changed = Int16::parse(v, value.sValue);
                break;
            case UInteger16:
                changed = UInt16::parse(v, value.usValue);
                break;
            case Integer32:
                changed = Int32::parse(v, value.nValue);
                break;
            case UInteger32:
                changed = UInt32::parse(v, value.unValue);
                break;
            case Integer64:
                changed = Int64::parse(v, value.lValue);
                break;
            case UInteger64:
                changed = UInt64::parse(v, value.ulValue);
                break;
            case Float32:
                changed = Float::parse(v, value.fValue);
                break;
            case Float64:
                changed = Double::parse(v, value.dValue);
                break;
            case Text:
                changed = setStringValue(v, type, value);
                break;
            case Date: {
                DateTime time;
                if (DateTime::parse(v, time)) {
                    value.dateValue = time.ticks();
                    changed = true;
                }
                break;
            }
            case Timestamp: {
                TimeSpan time;
                if (TimeSpan::parse(v, time)) {
                    value.timeValue = time.ticks();
                    changed = true;
                }
                break;
            }
            case Blob: {
                ByteArray array;
                if (ByteArray::parse(v, array)) {
                    setByteArrayValue(array, type, value);
                    changed = true;
                }
                break;
            }
            default:
                break;
        }
        return changed;
    }

    bool Variant::changeValue(const DateTime &v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                value.dValue = (int8_t) v.ticks();
                changed = true;
                break;
            case Integer8:
                value.dValue = (int8_t) v.ticks();
                changed = true;
                break;
            case UInteger8:
                value.dValue = (uint8_t) v.ticks();
                changed = true;
                break;
            case Integer16:
                value.dValue = (int16_t) v.ticks();
                changed = true;
                break;
            case UInteger16:
                value.dValue = (uint16_t) v.ticks();
                changed = true;
                break;
            case Integer32:
                value.dValue = (int32_t) v.ticks();
                changed = true;
                break;
            case UInteger32:
                value.dValue = (uint32_t) v.ticks();
                changed = true;
                break;
            case Integer64:
                value.lValue = (int64_t) v.ticks();
                changed = true;
                break;
            case UInteger64:
                value.ulValue = (uint64_t) v.ticks();
                changed = true;
                break;
            case Float32:
                value.dValue = (float) v.ticks();
                changed = true;
                break;
            case Float64:
                value.dValue = (double) v.ticks();
                changed = true;
                break;
            case Text:
                setStringValue(v.toString(), type, value);
                changed = true;
                break;
            case Date:
                value.dateValue = v.ticks();
                changed = true;
                break;
            case Timestamp:
                value.timeValue = (int64_t) v.ticks();
                changed = true;
                break;
            case Blob:
                changed = false;
                break;
            default:
                break;
        }
        return changed;
    }

    bool Variant::changeValue(const TimeSpan &v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
            case Digital:
                changed = false;
                break;
            case Integer8:
                value.dValue = (int8_t) v.ticks();
                changed = true;
                break;
            case UInteger8:
                value.dValue = (uint8_t) v.ticks();
                changed = true;
                break;
            case Integer16:
                value.dValue = (int16_t) v.ticks();
                changed = true;
                break;
            case UInteger16:
                value.dValue = (uint16_t) v.ticks();
                changed = true;
                break;
            case Integer32:
                value.dValue = (int32_t) v.ticks();
                changed = true;
                break;
            case UInteger32:
                value.dValue = (uint32_t) v.ticks();
                changed = true;
                break;
            case Integer64:
                value.lValue = (int64_t) v.ticks();
                changed = true;
                break;
            case UInteger64:
                value.ulValue = (uint64_t) v.ticks();
                changed = true;
                break;
            case Float32:
                value.dValue = (float) v.ticks();
                changed = true;
                break;
            case Float64:
                value.dValue = (double) v.ticks();
                changed = true;
                break;
            case Text:
                setStringValue(v.toString(), type, value);
                changed = true;
                break;
            case Date:
                value.dateValue = (uint64_t) v.ticks();
                changed = true;
                break;
            case Timestamp:
                value.timeValue = v.ticks();
                changed = true;
                break;
            case Blob:
                changed = false;
                break;
            default:
                break;
        }
        return changed;
    }

    bool Variant::changeValue(const ByteArray &v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
            case Digital:
            case Integer8:
            case UInteger8:
            case Integer16:
            case UInteger16:
            case Integer32:
            case UInteger32:
            case Integer64:
            case UInteger64:
            case Float32:
            case Float64:
            case Text:
            case Date:
            case Timestamp:
                changed = false;
                break;
            case Blob:
                setByteArrayValue(v, type, value);
                changed = true;
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeBlobValue(const uint8_t *v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
            case Digital:
            case Integer8:
            case UInteger8:
            case Integer16:
            case UInteger16:
            case Integer32:
            case UInteger32:
            case Integer64:
            case UInteger64:
            case Float32:
            case Float64:
            case Text:
            case Date:
            case Timestamp:
                changed = false;
                break;
            case Blob:
                setByteArrayValue(v, type, value);
                changed = true;
                break;
            default:
                break;
        }
        return changed;
    }

    bool Variant::changeValue(Type type, const Value &value, String &v) {
        bool changed = true;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                v = Convert::convertStr(value.bValue);
                break;
            case Integer8:
                v = Convert::convertStr(value.cValue);
                break;
            case UInteger8:
                v = Convert::convertStr(value.ucValue);
                break;
            case Integer16:
                v = Convert::convertStr(value.sValue);
                break;
            case UInteger16:
                v = Convert::convertStr(value.usValue);
                break;
            case Integer32:
                v = Convert::convertStr(value.nValue);
                break;
            case UInteger32:
                v = Convert::convertStr(value.unValue);
                break;
            case Integer64:
                v = Convert::convertStr(value.lValue);
                break;
            case UInteger64:
                v = Convert::convertStr(value.ulValue);
                break;
            case Float32:
                v = Convert::convertStr(value.fValue);
                break;
            case Float64:
                v = Convert::convertStr(value.dValue);
                break;
            case Text:
                v = value.strValue;
                break;
            case Date: {
                DateTime time(value.dateValue);
                v = time.toString();
                break;
            }
            case Timestamp: {
                TimeSpan time(value.timeValue);
                v = time.toString();
                break;
            }
            case Blob: {
                size_t count = blobCount(value.blobValue);
                if (count <= MaxBlobCount) {
                    const uint8_t *buffer = blobBuffer(value.blobValue);
                    ByteArray array(buffer, count);
                    v = array.toString();
                } else {
                    changed = false;
                }
            }
                break;
            default:
                break;
        }
        return changed;
    }

    bool Variant::changeValue(Type type, const Value &value, DateTime &v) {
        bool changed = false;
        switch (type) {
            case Null:
            case Digital:
            case Integer8:
            case UInteger8:
            case Integer16:
            case UInteger16:
            case Integer32:
            case UInteger32:
            case Integer64:
            case UInteger64:
            case Float32:
            case Float64:
                changed = false;
                break;
            case Text: {
                DateTime time;
                if (DateTime::parse(value.strValue, time)) {
                    v = time;
                    changed = true;
                }
                break;
            }
            case Date:
                v = DateTime(value.dateValue);
                changed = true;
                break;
            case Timestamp:
            case Blob:
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeValue(Type type, const Value &value, TimeSpan &v) {
        bool changed = false;
        switch (type) {
            case Null:
            case Digital:
            case Integer8:
            case UInteger8:
            case Integer16:
            case UInteger16:
            case Integer32:
            case UInteger32:
            case Integer64:
            case UInteger64:
            case Float32:
            case Float64:
                changed = false;
                break;
            case Text: {
                TimeSpan time;
                if (TimeSpan::parse(value.strValue, time)) {
                    v = time;
                    changed = true;
                }
                break;
            }
            case Date:
                changed = false;
                break;
            case Timestamp:
                v = TimeSpan(value.timeValue);
                changed = true;
                break;
            case Blob:
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeValue(Type type, const Value &value, ByteArray &v) {
        if (type == Blob) {
            size_t count = blobCount(value.blobValue);
            const uint8_t *buffer = blobBuffer(value.blobValue);
            v = ByteArray(buffer, count);
            return true;
        }
        return false;
    }

    double Variant::toAnalogValue(Type type, const Value &value) {
        if (isAnalogValue(type)) {
            double result = 0.0;
            changeValue<double>(type, value, result);
            return result;
        }
        return Double::NaN;
    }

    Variant::Value Variant::fromAnalogValue(Type type, const double &value) {
        Value result;
        changeValue<double>(value, type, result);
        return result;
    }

    String Variant::typeStr() const {
        return toTypeStr(_type);
    }

    void Variant::writeValue(Stream *stream, bool bigEndian, Variant::Type type, const Variant::Value &value) {
        switch (type) {
            case Null:
                break;
            case Digital:
                stream->writeBoolean(value.bValue);
                break;
            case Integer8:
                stream->writeInt8(value.cValue);
                break;
            case UInteger8:
                stream->writeUInt8(value.ucValue);
                break;
            case Integer16:
                stream->writeInt16(value.sValue, bigEndian);
                break;
            case UInteger16:
                stream->writeUInt16(value.usValue, bigEndian);
                break;
            case Integer32:
                stream->writeInt32(value.nValue, bigEndian);
                break;
            case UInteger32:
                stream->writeUInt32(value.unValue, bigEndian);
                break;
            case Integer64:
                stream->writeInt64(value.lValue, bigEndian);
                break;
            case UInteger64:
                stream->writeUInt64(value.ulValue, bigEndian);
                break;
            case Float32:
                stream->writeFloat(value.fValue, bigEndian);
                break;
            case Float64:
                stream->writeDouble(value.dValue, bigEndian);
                break;
            case Text:
                stream->writeStr(value.strValue, String::StreamLength4);
                break;
            case Date: {
                DateTime date(value.dateValue);
                date.write(stream, bigEndian);
            }
                break;
            case Timestamp: {
                TimeSpan time(value.timeValue);
                time.write(stream, bigEndian);
            }
                break;
            case Blob: {
                size_t count = blobCount(value.blobValue);
                stream->write(value.blobValue, 0, count + 4);
            }
                break;
            default:
                break;
        }
    }

    void Variant::readValue(Stream *stream, bool bigEndian, Variant::Type type, Variant::Value &value) {
        switch (type) {
            case Null:
                break;
            case Digital:
                value.bValue = stream->readBoolean();
                break;
            case Integer8:
                value.cValue = stream->readInt8();
                break;
            case UInteger8:
                value.ucValue = stream->readUInt8();
                break;
            case Integer16:
                value.sValue = stream->readInt16(bigEndian);
                break;
            case UInteger16:
                value.usValue = stream->readUInt16(bigEndian);
                break;
            case Integer32:
                value.nValue = stream->readInt32(bigEndian);
                break;
            case UInteger32:
                value.unValue = stream->readUInt32(bigEndian);
                break;
            case Integer64:
                value.lValue = stream->readInt64(bigEndian);
                break;
            case UInteger64:
                value.ulValue = stream->readUInt64(bigEndian);
                break;
            case Float32:
                value.fValue = stream->readFloat(bigEndian);
                break;
            case Float64:
                value.dValue = stream->readDouble(bigEndian);
                break;
            case Text:
                setStringValue(stream->readStr(String::StreamLength4), type, value);
                break;
            case Date: {
                DateTime time;
                time.read(stream, bigEndian);
                value.dateValue = time.ticks();
            }
                break;
            case Timestamp: {
                TimeSpan time;
                time.read(stream, bigEndian);
                value.timeValue = time.ticks();
            }
                break;
            case Blob: {
                uint8_t countBuffer[4];
                stream->read(countBuffer, 0, sizeof(countBuffer));
                size_t count = blobCount(countBuffer);
                value.blobValue = new uint8_t[count + 4];
                memcpy(value.blobValue, countBuffer, sizeof(countBuffer));
                stream->read(value.blobValue + 4, 0, count);
            }
                break;
            default:
                break;
        }
    }

    size_t Variant::valueSize(Type type, const Value &value) {
        size_t size = 0;
        switch (type) {
            case Null:
                size = 0;
                break;
            case Digital:
                size = sizeof(value.bValue);
                break;
            case Integer8:
                size = sizeof(value.cValue);
                break;
            case UInteger8:
                size = sizeof(value.ucValue);
                break;
            case Integer16:
                size = sizeof(value.sValue);
                break;
            case UInteger16:
                size = sizeof(value.usValue);
                break;
            case Integer32:
                size = sizeof(value.nValue);
                break;
            case UInteger32:
                size = sizeof(value.unValue);
                break;
            case Integer64:
                size = sizeof(value.lValue);
                break;
            case UInteger64:
                size = sizeof(value.ulValue);
                break;
            case Float32:
                size = sizeof(value.fValue);
                break;
            case Float64:
                size = sizeof(value.dValue);
                break;
            case Text:
                size = 4 + strlen(value.strValue);
                break;
            case Date:
                size = sizeof(value.dateValue);
                break;
            case Timestamp:
                size = sizeof(value.timeValue);
                break;
            case Blob: {
                size_t count = blobCount(value.blobValue);
                size = 4 + count;
            }
                break;
            default:
                break;
        }
        return size;
    }

    String Variant::toTypeScriptStr(Type type) {
        switch (type) {
            case Null:
                return String::Empty;
            case Digital:
                return "bool";
            case Integer8:
                return "int8_t";
            case UInteger8:
                return "uint8_t";
            case Integer16:
                return "int16_t";
            case UInteger16:
                return "uint16_t";
            case Integer32:
                return "int32_t";
            case UInteger32:
                return "uint32_t";
            case Integer64:
                return "int64_t";
            case UInteger64:
                return "uint64_t";
            case Float32:
                return "float";
            case Float64:
                return "double";
            case Text:
                return "String";
            case Date:
                return "DateTime";
            case Timestamp:
                return "TimeSpan";
            default:
                return String::Empty;
        }
    }

    String Variant::toTypeStr(Type type) {
        switch (type) {
            case Null:
                return "Null";
            case Digital:
                return "Digital";
            case Integer8:
                return "Integer8";
            case UInteger8:
                return "UInteger8";
            case Integer16:
                return "Integer16";
            case UInteger16:
                return "UInteger16";
            case Integer32:
                return "Integer32";
            case UInteger32:
                return "UInteger32";
            case Integer64:
                return "Integer64";
            case UInteger64:
                return "UInteger64";
            case Float32:
                return "Float32";
            case Float64:
                return "Float64";
            case Text:
                return "Text";
            case Date:
                return "Date";
            case Timestamp:
                return "Timestamp";
            case Blob:
                return "Blob";
            default:
                return "Null";
        }
    }

    Variant::Type Variant::fromTypeStr(const String &str) {
        if (String::equals(str, "Digital", true) ||
            String::equals(str, "Boolean", true) ||
            String::equals(str, "Bool", true) ||
            String::equals(str, "Bit", true)) {
            return Type::Digital;
        } else if (String::equals(str, "Int8", true) ||
                   String::equals(str, "Char", true) ||
                   String::equals(str, "Integer8", true)) {
            return Type::Integer8;
        } else if (String::equals(str, "UInt8", true) ||
                   String::equals(str, "Byte", true) ||
                   String::equals(str, "UInteger8", true) ||
                   String::equals(str, "UChar", true)) {
            return Type::UInteger8;
        } else if (String::equals(str, "Int16", true) ||
                   String::equals(str, "Short", true) ||
                   String::equals(str, "Integer16", true)) {
            return Type::Integer16;
        } else if (String::equals(str, "UInt16", true) ||
                   String::equals(str, "UShort", true) ||
                   String::equals(str, "WORD", true) ||
                   String::equals(str, "UInteger16", true)) {
            return Type::UInteger16;
        } else if (String::equals(str, "Int", true) ||
                   String::equals(str, "Int32", true) ||
                   String::equals(str, "Integer32", true) ||
                   String::equals(str, "Integer", true)) {
            return Type::Integer32;
        } else if (String::equals(str, "UInt32", true) ||
                   String::equals(str, "UInteger32", true) ||
                   String::equals(str, "DWORD", true) ||
                   String::equals(str, "UInteger", true)) {
            return Type::UInteger32;
        } else if (String::equals(str, "Integer64", true) ||
                   String::equals(str, "Int64", true) ||
                   String::equals(str, "Long", true)) {
            return Type::Integer64;
        } else if (String::equals(str, "UInteger64", true) ||
                   String::equals(str, "UInt64", true)) {
            return Type::UInteger64;
        } else if (String::equals(str, "Float32", true) ||
                   String::equals(str, "Single", true) ||
                   String::equals(str, "Float", true)) {
            return Type::Float32;
        } else if (String::equals(str, "Float64", true) ||
                   String::equals(str, "Double", true) ||
                   String::equals(str, "Real", true)) {
            return Type::Float64;
        } else if (String::equals(str, "Text", true) ||
                   String::equals(str, "String", true) ||
                   String::equals(str, "varchar", true) ||
                   String::equals(str, "vstring", true) ||
                   String::equals(str, "nvarchar", true)) {
            return Type::Text;
        } else if (String::equals(str, "Date", true) ||
                   String::equals(str, "DateTime", true) ||
                   String::equals(str, "Time", true)) {
            return Type::Date;
        } else if (String::equals(str, "Timestamp", true)) {
            return Type::Timestamp;
        } else if (String::equals(str, "Blob", true) ||
                   String::equals(str, "ByteArray", true) ||
                   String::equals(str, "Bytes", true)) {
            return Type::Blob;
        }
        return Type::Null;
    }

    void Variant::getAllTypeStr(StringArray &array) {
        array = {"Null", "Digital", "Integer8", "UInteger8", "Integer16", "UInteger16", "Integer32", "UInteger32",
                 "Integer64", "UInteger64", "Float32", "Float64", "Text", "Date", "Timestamp", "Blob"};
    }

    bool Variant::isNullType(Type type) {
        return (type == Null);
    }

    bool Variant::isAnalogValue(Type type) {
        return (isIntegerValue(type) || isFloatValue(type));
    }

    bool Variant::isDigitalValue(Type type) {
        return (type == Variant::Digital);
    }

    bool Variant::isIntegerValue(Type type) {
        return (type == Variant::Integer8 || type == Variant::UInteger8 ||
                type == Variant::Integer16 || type == Variant::UInteger16 ||
                type == Variant::Integer32 || type == Variant::UInteger32 ||
                type == Variant::Integer64 || type == Variant::UInteger64);
    }

    bool Variant::isFloatValue(Type type) {
        return (type == Variant::Float32 || type == Variant::Float64);
    }

    bool Variant::isStringValue(Type type) {
        return (type == Variant::Text);
    }

    bool Variant::isDateTimeValue(Type type) {
        return (type == Variant::Date);
    }

    void Variant::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt8(_type);
        writeValue(stream, bigEndian, _type, _value);
        stream->writeBoolean(_isNullValue);
    }

    void Variant::read(Stream *stream, bool bigEndian) {
        _type = (Type) stream->readUInt8();
        readValue(stream, bigEndian, _type, _value);
        _isNullValue = stream->readBoolean();
    }

    String Variant::toValueString(Type type, const Value &value) {
        String str;
        if (changeValue(type, value, str))
            return str;
        return String::Empty;
    }

    bool Variant::parseValueString(const String &str, Type type, Value &value) {
        return changeValue(str, type, value);
    }

    bool Variant::equalsTextValue(const char *value1, const char *value2) {
        if (value1 != nullptr && value2 != nullptr)
            return strcmp(value1, value2) == 0;
        else if (value1 == nullptr && value2 == nullptr)
            return true;
        else
            return false;
    }

    bool Variant::equalsBlobValue(const uint8_t *value1, const uint8_t *value2) {
        size_t count1 = blobCount(value1);
        const uint8_t *buffer1 = blobBuffer(value1);
        size_t count2 = blobCount(value2);
        const uint8_t *buffer2 = blobBuffer(value2);
        if (count1 == count2 &&
            buffer1 != nullptr && buffer2 != nullptr) {
            return memcmp(buffer1, buffer2, count1) == 0;
        }
        return false;
    }

    size_t Variant::blobCount(const uint8_t *value) {
        if (value != nullptr) {
            size_t count = ((value[0] << 24) & 0xFF000000) |
                           ((value[1] << 16) & 0x00FF0000) |
                           ((value[2] << 8) & 0x0000FF00) |
                           ((value[3] << 0) & 0x000000FF);
            return count;
        }
        return 0;
    }

    const uint8_t *Variant::blobBuffer(const uint8_t *value) {
        if (value != nullptr) {
            return value + 4;
        }
        return nullptr;
    }

    String Variant::toString() const {
        return valueStr();
    }

    void Variant::clearValue(Type type, Value &value) {
        if (type == Text) {
            delete[] value.strValue;
            value.strValue = nullptr;
        } else if (type == Blob) {
            delete[] value.blobValue;
            value.blobValue = nullptr;
        }
    }

    template<class T>
    bool Variant::changeValue(const T &v, Type type, Value &value) {
        switch (type) {
            case Null:
                return false;
            case Digital:
                value.bValue = v != 0;
                break;
            case Integer8:
                value.cValue = (int8_t) v;
                break;
            case UInteger8:
                value.ucValue = (uint8_t) v;
                break;
            case Integer16:
                value.sValue = (int16_t) v;
                break;
            case UInteger16:
                value.usValue = (uint16_t) v;
                break;
            case Integer32:
                value.nValue = (int32_t) v;
                break;
            case UInteger32:
                value.unValue = (uint32_t) v;
                break;
            case Integer64:
                value.lValue = (int64_t) v;
                break;
            case UInteger64:
                value.ulValue = (uint64_t) v;
                break;
            case Float32:
                value.fValue = (float) v;
                break;
            case Float64:
                value.dValue = (double) v;
                break;
            case Text:
                setStringValue(Convert::convertStr(v), type, value);
                break;
            case Date:
            case Timestamp:
            case Blob:
            default:
                return false;
        }
        return true;
    }

    template<class T>
    bool Variant::changeValue(Type type, const Value &value, T &v) {
        switch (type) {
            case Null:
                return false;
            case Digital:
                v = (T) value.bValue;
                break;
            case Integer8:
                v = (T) value.cValue;
                break;
            case UInteger8:
                v = (T) value.ucValue;
                break;
            case Integer16:
                v = (T) value.sValue;
                break;
            case UInteger16:
                v = (T) value.usValue;
                break;
            case Integer32:
                v = (T) value.nValue;
                break;
            case UInteger32:
                v = (T) value.unValue;
                break;
            case Integer64:
                v = (T) value.lValue;
                break;
            case UInteger64:
                v = (T) value.ulValue;
                break;
            case Float32:
                v = (T) value.fValue;
                break;
            case Float64:
                v = (T) value.dValue;
                break;
            case Text:
                return Convert::parseStr(value.strValue, v);
            case Date:
            case Timestamp:
            case Blob:
            default:
                return false;
        }
        return true;
    }

    template<class T>
    bool Variant::equals(Type type, const Value &value, const T &v) {
        switch (type) {
            case Null:
                return false;
            case Digital:
                return value.bValue == (bool) (v != 0);
            case Integer8:
                return value.cValue == (int8_t) v;
            case UInteger8:
                return value.ucValue == (uint8_t) v;
            case Integer16:
                return value.sValue == (int16_t) v;
            case UInteger16:
                return value.usValue == (uint16_t) v;
            case Integer32:
                return value.nValue == (int32_t) v;
            case UInteger32:
                return value.unValue == (uint32_t) v;
            case Integer64:
                return value.lValue == (int64_t) v;
            case UInteger64:
                return value.ulValue == (uint64_t) v;
            case Float32:
                return value.fValue == (float) v;
            case Float64:
                return value.dValue == (double) v;
            case Text:
                return equalsTextValue(value.strValue, Convert::convertStr(v).c_str());
            case Date:
            case Timestamp:
            case Blob:
            default:
                return false;
        }
        return true;
    }
}
