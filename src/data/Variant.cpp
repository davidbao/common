//
//  Variant.cpp
//  common
//
//  Created by baowei on 2017/11/25.
//  Copyright © 2017 com. All rights reserved.
//

#include "data/Variant.h"

namespace Common {
    Variant::Variant(Type type) {
        _type = type;
        _value.lValue = 0;
    }

    Variant::Variant(bool value) : Variant(Digital) {
        setValue(value);
    }

    Variant::Variant(int8_t value) : Variant(Integer8) {
        setValue(value);
    }

    Variant::Variant(uint8_t value) : Variant(UInteger8) {
        setValue(value);
    }

    Variant::Variant(int16_t value) : Variant(Integer16) {
        setValue(value);
    }

    Variant::Variant(uint16_t value) : Variant(UInteger16) {
        setValue(value);
    }

    Variant::Variant(int32_t value) : Variant(Integer32) {
        setValue(value);
    }

    Variant::Variant(uint32_t value) : Variant(UInteger32) {
        setValue(value);
    }

    Variant::Variant(int64_t value) : Variant(Integer64) {
        setValue(value);
    }

    Variant::Variant(uint64_t value) : Variant(UInteger64) {
        setValue(value);
    }

    Variant::Variant(float value) : Variant(Float32) {
        setValue(value);
    }

    Variant::Variant(double value) : Variant(Float64) {
        setValue(value);
    }

    Variant::Variant(const String &value) : Variant(Text) {
        setValue(value);
    }

    Variant::Variant(DateTime value) : Variant(Date) {
        setValue(value);
    }

    Variant::Variant(const ByteArray &value) : Variant(Blob) {
        setValue(value);
    }

    Variant::~Variant() {
        if (type() == Text) {
            if (_value.strValue != nullptr) {
                delete[] _value.strValue;
                _value.strValue = nullptr;
            }
        } else if (type() == Blob) {
            if (_value.blobValue != nullptr) {
                delete[] _value.blobValue;
                _value.blobValue = nullptr;
            }
        }
    }

    bool Variant::isEmpty() const {
        return _type == Null;
    }

    Variant &Variant::operator=(const Variant &tag) {
        this->_type = tag._type;
        setValue(tag.value());
        return *this;
    }

    bool Variant::operator==(const Variant &tag) const {
        if (this->_type != tag._type) {
            return false;
        }

        return isValueEqual(_type, _value, tag._value);
    }

    bool Variant::operator!=(const Variant &tag) const {
        return !operator==(tag);
    }

    Variant &Variant::operator=(const Value &value) {
        setValue(value);
        return *this;
    }

    bool Variant::operator==(const Value &value) const {
        return isValueEqual(_type, _value, value);
    }

    bool Variant::operator!=(const Value &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const bool &value) {
        setValue(value);
        return *this;
    }

    Variant::operator bool() const {
        bool v = false;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const bool &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const bool &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const int8_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator int8_t() const {
        int8_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const int8_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const int8_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const uint8_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator uint8_t() const {
        uint8_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const uint8_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const uint8_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const int16_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator int16_t() const {
        int16_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const int16_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const int16_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const uint16_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator uint16_t() const {
        uint16_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const uint16_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const uint16_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const int32_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator int32_t() const {
        int32_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const int32_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const int32_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const uint32_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator uint32_t() const {
        uint32_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const uint32_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const uint32_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const int64_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator int64_t() const {
        int64_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const int64_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const int64_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const uint64_t &value) {
        setValue(value);
        return *this;
    }

    Variant::operator uint64_t() const {
        uint64_t v = 0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const uint64_t &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const uint64_t &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const float &value) {
        setValue(value);
        return *this;
    }

    Variant::operator float() const {
        float v = 0.0f;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const float &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const float &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const double &value) {
        setValue(value);
        return *this;
    }

    Variant::operator double() const {
        double v = 0.0;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const double &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const double &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const String &value) {
        setValue(value);
        return *this;
    }

    Variant::operator String() const {
        String v = String::Empty;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const String &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const String &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const DateTime &value) {
        setValue(value);
        return *this;
    }

    Variant::operator DateTime() const {
        DateTime v = DateTime::MinValue;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const DateTime &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const DateTime &value) const {
        return !operator==(value);
    }

    Variant &Variant::operator=(const ByteArray &value) {
        setValue(value);
        return *this;
    }

    Variant::operator ByteArray() const {
        ByteArray v;
        getValue(v);
        return v;
    }

    bool Variant::operator==(const ByteArray &value) const {
        return compareValue(value);
    }

    bool Variant::operator!=(const ByteArray &value) const {
        return !operator==(value);
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
                return compareTextValue(value1.strValue, value2.strValue);
            case Date:
                return value1.tValue == value2.tValue;
            case Blob:
                return compareBlobValue(value1.blobValue, value2.blobValue);
            default:
                break;
        }
        return false;
    }

    void Variant::setStringValue(const String &str) {
        setStringValue(str, _type, _value);
    }

    void Variant::setStringValue(const String &str, Type type, Value &value) {
        if (type == Text) {
            if (value.strValue != nullptr) {
                delete[] value.strValue;
            }
            if (str.isNullOrEmpty()) {
                value.strValue = nullptr;
                return;
            }

            size_t len = strlen(str);
            if (len > 0) {
                value.strValue = new char[len + 1];
                strcpy(value.strValue, str);
            } else {
                value.strValue = new char[1];
                value.strValue[0] = '\0';
            }
        }
    }

    void Variant::setByteArrayValue(const uint8_t *buffer) {
        setByteArrayValue(buffer, _type, _value);
    }

    void Variant::setByteArrayValue(const uint8_t *buffer, Type type, Value &value) {
        if (type == Blob) {
            if (value.blobValue != nullptr) {
                delete[] value.blobValue;
            }
            if (buffer == nullptr) {
                value.blobValue = nullptr;
                return;
            }

            size_t count = blobCount(buffer);
            if (count > 0) {
                value.blobValue = new uint8_t[2 + count];
                uint8_t high = (count >> 8) & 0xFF;
                uint8_t low = count & 0xFF;
                value.blobValue[0] = high;
                value.blobValue[1] = low;
                memcpy((value.blobValue + 2), blobBuffer(buffer), count);
            }
        }
    }

    void Variant::setByteArrayValue(const ByteArray &buffer) {
        setByteArrayValue(buffer, _type, _value);
    }

    void Variant::setByteArrayValue(const ByteArray &buffer, Type type, Value &value) {
        if (type == Blob) {
            if (value.blobValue != nullptr) {
                delete[] value.blobValue;
            }
            if (buffer.count() == 0) {
                value.blobValue = nullptr;
                return;
            }

            size_t count = buffer.count();
            if (count > 0) {
                value.blobValue = new uint8_t[2 + count];
                uint8_t high = (count >> 8) & 0xFF;
                uint8_t low = count & 0xFF;
                value.blobValue[0] = high;
                value.blobValue[1] = low;
                memcpy((value.blobValue + 2), buffer.data(), count);
            }
        }
    }

    bool Variant::isNullValue(const Variant &tag) {
        return (tag.type() == Null);
    }

    bool Variant::isNullValue() const {
        return isNullValue(*this);
    }

    void Variant::setNullValue() {
        _type = Null;
    }

    Variant::Type Variant::type() const {
        return _type;
    }

    void Variant::setValue(const Value &v) {
        Variant::Value oldValue;
        oldValue.lValue = 0;
        Variant::changeValue(type(), value(), type(), oldValue);

        setValueInner(v);

        if (type() == Text) {
            if (oldValue.strValue != nullptr) {
                delete[] oldValue.strValue;
            }
        } else if (type() == Blob) {
            if (oldValue.blobValue != nullptr) {
                delete[] oldValue.blobValue;
            }
        }
    }

    template<class T>
    bool Variant::setValue(const T &v) {
        if (compareValue(v))
            return false;

        Value temp;
        temp.lValue = 0;
        if (changeValueInner(v, type(), temp)) {
            setValue(temp);
            if (type() == Text) {
                delete[] temp.strValue;
            } else if (type() == Blob) {
                delete[] temp.blobValue;
            }
            return true;
        }
        return false;
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
        if (compareValue(v))
            return false;

        Value temp;
        temp.lValue = 0;
        if (changeStringValue(v, type(), temp)) {
            setValue(temp);
            if (type() == Text) {
                delete[] temp.strValue;
            } else if (type() == Blob) {
                delete[] temp.blobValue;
            }
            return true;
        }
        return false;
    }

    bool Variant::setValue(const char *v) {
        if (v != nullptr) {
            return setValue((String) v);
        }
        return false;
    }

    bool Variant::setValue(const DateTime &v) {
        if (compareValue(v))
            return false;

        Value temp;
        temp.lValue = 0;
        if (changeDateValue(v, type(), temp)) {
            setValue(temp);
            if (type() == Text) {
                delete[] temp.strValue;
            } else if (type() == Blob) {
                delete[] temp.blobValue;
            }
            return true;
        }
        return false;
    }

    bool Variant::setValue(const ByteArray &v) {
        if (compareValue(v))
            return false;

        Value temp;
        temp.lValue = 0;
        if (changeByteArrayValue(v, type(), temp)) {
            setValue(temp);
            if (type() == Text) {
                delete[] temp.strValue;
            } else if (type() == Blob) {
                delete[] temp.blobValue;
            }
            return true;
        }
        return false;
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

    Variant::Value Variant::value() const {
        return _value;
    }

    bool Variant::getValue(Variant::Type type, Variant::Value &value) const {
        switch (_type) {
            case Null:
                break;
            case Digital:
                return changeDigitalValue(_value.bValue, type, value);
            case Integer8:
                return changeInt8Value(_value.cValue, type, value);
            case UInteger8:
                return changeUInt8Value(_value.ucValue, type, value);
            case Integer16:
                return changeInt16Value(_value.sValue, type, value);
            case UInteger16:
                return changeUInt16Value(_value.usValue, type, value);
            case Integer32:
                return changeInt32Value(_value.nValue, type, value);
            case UInteger32:
                return changeUInt32Value(_value.unValue, type, value);
            case Integer64:
                return changeInt64Value(_value.lValue, type, value);
            case UInteger64:
                return changeUInt64Value(_value.ulValue, type, value);
            case Float64:
                return changeFloat64Value(_value.dValue, type, value);
            case Text:
                return changeStringValue(_value.strValue, type, value);
            case Float32:
                return changeFloat32Value(_value.fValue, type, value);
            case Date:
                return changeDateValue(_value.tValue, type, value);
            case Blob:
                return changeByteArrayValue(_value.blobValue, type, value);
            default:
                break;
        }
        return false;
    }

    template<class T>
    bool Variant::getValue(T &v) const {
        return changeValueInner(type(), value(), v);
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
        return changeStringValue(type(), value(), v);
    }

    const String Variant::valueStr() const {
        String value;
        if (getValue(value))
            return value;
        return String::Empty;
    }

    bool Variant::getValue(DateTime &v) const {
        return changeDateValue(type(), value(), v);
    }

    bool Variant::getValue(ByteArray &v) const {
        return changeByteArrayValue(type(), value(), v);
    }

    bool Variant::compareValue(const Value &v) const {
        switch (type()) {
            case Null:
                return false;
            case Digital:
                return _value.bValue == v.bValue;
            case Integer8:
                return _value.cValue == v.cValue;
            case UInteger8:
                return _value.ucValue == v.ucValue;
            case Integer16:
                return _value.sValue == v.sValue;
            case UInteger16:
                return _value.usValue == v.usValue;
            case Integer32:
                return _value.nValue == v.nValue;
            case UInteger32:
                return _value.unValue == v.unValue;
            case Integer64:
                return _value.lValue == v.lValue;
            case UInteger64:
                return _value.ulValue == v.ulValue;
            case Float32:
                return _value.fValue == v.fValue;
            case Float64:
                return _value.dValue == v.dValue;
            case Text:
                return compareTextValue(_value.strValue, v.strValue);
            case Date:
                return _value.tValue == v.tValue;
            case Blob:
                return compareBlobValue(_value.blobValue, v.blobValue);
            default:
                return false;
        }
    }

    bool Variant::compareValue(const bool &v) const {
        return compareValue<bool>(type(), value(), v);
    }

    bool Variant::compareValue(const int8_t &v) const {
        return compareValue<int8_t>(type(), value(), v);
    }

    bool Variant::compareValue(const uint8_t &v) const {
        return compareValue<uint8_t>(type(), value(), v);
    }

    bool Variant::compareValue(const int16_t &v) const {
        return compareValue<int16_t>(type(), value(), v);
    }

    bool Variant::compareValue(const uint16_t &v) const {
        return compareValue<uint16_t>(type(), value(), v);
    }

    bool Variant::compareValue(const int32_t &v) const {
        return compareValue<int32_t>(type(), value(), v);
    }

    bool Variant::compareValue(const uint32_t &v) const {
        return compareValue<uint32_t>(type(), value(), v);
    }

    bool Variant::compareValue(const int64_t &v) const {
        return compareValue<int64_t>(type(), value(), v);
    }

    bool Variant::compareValue(const uint64_t &v) const {
        return compareValue<uint64_t>(type(), value(), v);
    }

    bool Variant::compareValue(const float &v) const {
        return compareValue<float>(type(), value(), v);
    }

    bool Variant::compareValue(const double &v) const {
        return compareValue<double>(type(), value(), v);
    }

    bool Variant::compareValue(const String &v) const {
        String str;
        if (changeStringValue(type(), value(), str)) {
            return str == v;
        }
        return false;
    }

    bool Variant::compareValue(const DateTime &v) const {
        DateTime temp;
        if (changeDateValue(type(), value(), temp)) {
            return temp == v;
        }
        return false;
    }

    bool Variant::compareValue(const ByteArray &v) const {
        ByteArray temp;
        if (changeByteArrayValue(type(), value(), temp)) {
            return temp == v;
        }
        return false;
    }

    template<class T>
    bool Variant::compareValue(Type type, const Value &value, const T &v) {
        switch (type) {
            case Null:
                return false;
            case Digital: {
                bool temp = v != 0 ? true : false;
                return value.bValue == temp;
            }
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
                return compareTextValue(value.strValue, Convert::convertStr(v).c_str());
            case Date:
                return false;
            case Blob:
                return false;
            default:
                return false;
        }
        return true;
    }

    bool Variant::compareValue(Type type, const Value &value, const bool &v) {
        return compareValue<bool>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const int8_t &v) {
        return compareValue<int8_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const uint8_t &v) {
        return compareValue<uint8_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const int16_t &v) {
        return compareValue<int16_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const uint16_t &v) {
        return compareValue<uint16_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const int32_t &v) {
        return compareValue<int32_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const uint32_t &v) {
        return compareValue<uint32_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const int64_t &v) {
        return compareValue<int64_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const uint64_t &v) {
        return compareValue<uint64_t>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const float &v) {
        return compareValue<float>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const double &v) {
        return compareValue<double>(type, value, v);
    }

    bool Variant::compareValue(Type type, const Value &value, const String &v) {
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
                Int64 temp;
                return Int64::parse(v, temp) && value.lValue == temp;
            }
            case UInteger64: {
                UInt64 temp;
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
                return compareTextValue(value.strValue, v.c_str());
            case Date:
                return false;
            case Blob:
                return false;
            default:
                return false;
        }
        return true;
    }

    bool Variant::changeValue(const Variant *tag, Type destType, Value &destValue) {
        if (tag == nullptr)
            return false;

        return changeValue(tag->type(), tag->value(), destType, destValue);
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
                    return changeDigitalValue(value.bValue, destType, destValue);
                case Integer8:
                    return changeInt8Value(value.cValue, destType, destValue);
                case UInteger8:
                    return changeUInt8Value(value.ucValue, destType, destValue);
                case Integer16:
                    return changeInt16Value(value.sValue, destType, destValue);
                case UInteger16:
                    return changeUInt16Value(value.usValue, destType, destValue);
                case Integer32:
                    return changeInt32Value(value.nValue, destType, destValue);
                case UInteger32:
                    return changeUInt32Value(value.unValue, destType, destValue);
                case Integer64:
                    return changeInt64Value(value.lValue, destType, destValue);
                case UInteger64:
                    return changeUInt64Value(value.ulValue, destType, destValue);
                case Float32:
                    return changeFloat32Value(value.fValue, destType, destValue);
                case Float64:
                    return changeFloat64Value(value.dValue, destType, destValue);
                case Text:
                    return changeStringValue(value.strValue, destType, destValue);
                case Date:
                    return changeDateValue(DateTime(value.lValue), destType, destValue);
                case Blob:
                    return false;
                default:
                    return false;
            }
        }
        return false;
    }

    bool Variant::compareValue(Type type1, const Value &value1, Type type2, const Value &value2) {
        Value destValue;
        destValue.lValue = 0;
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
            case Text: {
                if (value2.strValue != nullptr && destValue.strValue != nullptr) {
                    bool result = strcmp(value2.strValue, destValue.strValue) == 0;
                    delete[] destValue.strValue;
                    return result;
                } else {
                    return value2.strValue == nullptr && destValue.strValue == nullptr ? true : false;
                }
            }
            case Date:
                return value2.tValue == destValue.tValue;
            case Blob: {
                if (value2.blobValue != nullptr && destValue.blobValue != nullptr) {
                    bool result = compareBlobValue(value2.blobValue, destValue.blobValue);
                    delete[] destValue.blobValue;
                    return result;
                } else {
                    return value2.blobValue == nullptr && destValue.blobValue == nullptr ? true : false;
                }
            }
            default:
                return false;
        }
        return true;
    }

    bool Variant::changeDigitalValue(bool v, Type type, Value &value) {
        switch (type) {
            case Null:
                return false;
            case Digital:
                value.bValue = v;
                break;
            case Integer8:
                value.cValue = v ? 1 : 0;
                break;
            case UInteger8:
                value.ucValue = v ? 1 : 0;
                break;
            case Integer16:
                value.sValue = v ? 1 : 0;
                break;
            case UInteger16:
                value.usValue = v ? 1 : 0;
                break;
            case Integer32:
                value.nValue = v ? 1 : 0;
                break;
            case UInteger32:
                value.unValue = v ? 1 : 0;
                break;
            case Integer64:
                value.lValue = v ? 1 : 0;
                break;
            case UInteger64:
                value.ulValue = v ? 1 : 0;
                break;
            case Float32:
                value.fValue = v ? 1.f : 0.f;
                break;
            case Float64:
                value.dValue = v ? 1.0 : 0.0;
                break;
            case Text:
                setStringValue(Convert::convertStr(v), type, value);
                break;
            case Date:
                return false;
            case Blob:
                return false;
            default:
                return false;
        }
        return true;
    }

    bool Variant::changeInt8Value(int8_t v, Type type, Value &value) {
        return changeValueInner<int8_t>(v, type, value);
    }

    bool Variant::changeUInt8Value(uint8_t v, Type type, Value &value) {
        return changeValueInner<uint8_t>(v, type, value);
    }

    bool Variant::changeInt16Value(int16_t v, Type type, Value &value) {
        return changeValueInner<int16_t>(v, type, value);
    }

    bool Variant::changeUInt16Value(uint16_t v, Type type, Value &value) {
        return changeValueInner<uint16_t>(v, type, value);
    }

    bool Variant::changeInt32Value(int32_t v, Type type, Value &value) {
        return changeValueInner<int32_t>(v, type, value);
    }

    bool Variant::changeUInt32Value(uint32_t v, Type type, Value &value) {
        return changeValueInner<uint32_t>(v, type, value);
    }

    bool Variant::changeInt64Value(int64_t v, Type type, Value &value) {
        return changeValueInner<int64_t>(v, type, value);
    }

    bool Variant::changeUInt64Value(uint64_t v, Type type, Value &value) {
        return changeValueInner<uint64_t>(v, type, value);
    }

    bool Variant::changeFloat32Value(float v, Type type, Value &value) {
        return changeValueInner<float>(v, type, value);
    }

    bool Variant::changeFloat64Value(double v, Type type, Value &value) {
        return changeValueInner<double>(v, type, value);
    }

    bool Variant::changeStringValue(const String &v, Type type, Value &value) {
        return changeStringValue(v.c_str(), type, value);
    }

    bool Variant::changeStringValue(const char *v, Type type, Value &value) {
        if (v == nullptr)
            return false;

        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital: {
                if (Boolean::parse(v, value.bValue))
                    changed = true;
                int n;
                if (Int32::parse(v, n)) {
                    value.bValue = n != 0 ? true : false;
                    changed = true;
                }
            }
                break;
            case Integer8:
                if (Int8::parse(v, value.cValue))
                    changed = true;
                break;
            case UInteger8:
                if (UInt8::parse(v, value.ucValue))
                    changed = true;
                break;
            case Integer16:
                if (Int16::parse(v, value.sValue))
                    changed = true;
                break;
            case UInteger16:
                if (UInt16::parse(v, value.usValue))
                    changed = true;
                break;
            case Integer32:
                if (Int32::parse(v, value.nValue))
                    changed = true;
                break;
            case UInteger32:
                if (UInt32::parse(v, value.unValue))
                    changed = true;
                break;
            case Integer64:
                if (Int64::parse(v, value.lValue))
                    changed = true;
                break;
            case UInteger64:
                if (UInt64::parse(v, value.ulValue))
                    changed = true;
                break;
            case Float32:
                if (Float::parse(v, value.fValue))
                    changed = true;
                break;
            case Float64:
                if (Int32::parse(v, value.nValue))
                    changed = true;
                break;
            case Text:
                setStringValue(v, type, value);
                changed = true;
                break;
            case Date: {
                DateTime time;
                if (DateTime::parse(v, time)) {
                    value.tValue = time.ticks();
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
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeDateValue(const DateTime &v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = false;
                break;
            case Integer8:
                changed = false;
                break;
            case UInteger8:
                changed = false;
                break;
            case Integer16:
                changed = false;
                break;
            case UInteger16:
                changed = false;
                break;
            case Integer32:
                changed = false;
                break;
            case UInteger32:
                changed = false;
                break;
            case Integer64:
                changed = false;
                break;
            case UInteger64:
                changed = false;
                break;
            case Float32:
                changed = false;
                break;
            case Float64:
                changed = false;
                break;
            case Text:
                setStringValue(v.toString(), type, value);
                changed = true;
                break;
            case Date:
                value.tValue = v.ticks();
                changed = true;
                break;
            case Blob:
                changed = false;
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeByteArrayValue(const ByteArray &v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = false;
                break;
            case Integer8:
                changed = false;
                break;
            case UInteger8:
                changed = false;
                break;
            case Integer16:
                changed = false;
                break;
            case UInteger16:
                changed = false;
                break;
            case Integer32:
                changed = false;
                break;
            case UInteger32:
                changed = false;
                break;
            case Integer64:
                changed = false;
                break;
            case UInteger64:
                changed = false;
                break;
            case Float32:
                changed = false;
                break;
            case Float64:
                changed = false;
                break;
            case Text:
                changed = false;
                break;
            case Date:
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

    bool Variant::changeByteArrayValue(const uint8_t *v, Type type, Value &value) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = false;
                break;
            case Integer8:
                changed = false;
                break;
            case UInteger8:
                changed = false;
                break;
            case Integer16:
                changed = false;
                break;
            case UInteger16:
                changed = false;
                break;
            case Integer32:
                changed = false;
                break;
            case UInteger32:
                changed = false;
                break;
            case Integer64:
                changed = false;
                break;
            case UInteger64:
                changed = false;
                break;
            case Float32:
                changed = false;
                break;
            case Float64:
                changed = false;
                break;
            case Text:
                changed = false;
                break;
            case Date:
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

    bool Variant::changeDigitalValue(Type type, const Value &value, bool &v) {
        return changeValueInner<bool>(type, value, v);
    }

    bool Variant::changeInt8Value(Type type, const Value &value, int8_t &v) {
        return changeValueInner<int8_t>(type, value, v);
    }

    bool Variant::changeUInt8Value(Type type, const Value &value, uint8_t &v) {
        return changeValueInner<uint8_t>(type, value, v);
    }

    bool Variant::changeInt16Value(Type type, const Value &value, int16_t &v) {
        return changeValueInner<int16_t>(type, value, v);
    }

    bool Variant::changeUInt16Value(Type type, const Value &value, uint16_t &v) {
        return changeValueInner<uint16_t>(type, value, v);
    }

    bool Variant::changeInt32Value(Type type, const Value &value, int32_t &v) {
        return changeValueInner<int32_t>(type, value, v);
    }

    bool Variant::changeUInt32Value(Type type, const Value &value, uint32_t &v) {
        return changeValueInner<uint32_t>(type, value, v);
    }

    bool Variant::changeInt64Value(Type type, const Value &value, int64_t &v) {
        return changeValueInner<int64_t>(type, value, v);
    }

    bool Variant::changeUInt64Value(Type type, const Value &value, uint64_t &v) {
        return changeValueInner<uint64_t>(type, value, v);
    }

    bool Variant::changeFloat32Value(Type type, const Value &value, float &v) {
        return changeValueInner<float>(type, value, v);
    }

    bool Variant::changeFloat64Value(Type type, const Value &value, double &v) {
        return changeValueInner<double>(type, value, v);
    }

    bool Variant::changeStringValue(Type type, const Value &value, String &v) {
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
                DateTime time(value.tValue);
                v = time.toString();
                break;
            }
            case Blob: {
                size_t count = blobCount(value.blobValue);
                if (count > 0 && count < 128) {
                    const uint8_t *buffer = blobBuffer(value.blobValue);
                    ByteArray array(buffer, count);
                    v = array.toString();
                } else {
                    changed = false;
                }
            }
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeDateValue(Type type, const Value &value, DateTime &v) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = false;
                break;
            case Integer8:
                changed = false;
                break;
            case UInteger8:
                changed = false;
                break;
            case Integer16:
                changed = false;
                break;
            case UInteger16:
                changed = false;
                break;
            case Integer32:
                changed = false;
                break;
            case UInteger32:
                changed = false;
                break;
            case Integer64:
                changed = false;
                break;
            case UInteger64:
                changed = false;
                break;
            case Float32:
                changed = false;
                break;
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
                v = DateTime(value.tValue);
                changed = true;
                break;
            case Blob:
                changed = false;
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }

    bool Variant::changeByteArrayValue(Type type, const Value &value, ByteArray &v) {
        bool changed = false;
        switch (type) {
            case Null:
                changed = false;
                break;
            case Digital:
                changed = false;
                break;
            case Integer8:
                changed = false;
                break;
            case UInteger8:
                changed = false;
                break;
            case Integer16:
                changed = false;
                break;
            case UInteger16:
                changed = false;
                break;
            case Integer32:
                changed = false;
                break;
            case UInteger32:
                changed = false;
                break;
            case Integer64:
                changed = false;
                break;
            case UInteger64:
                changed = false;
                break;
            case Float32:
                changed = false;
                break;
            case Float64:
                changed = false;
                break;
            case Text:
                changed = false;
                break;
            case Date:
                changed = false;
                break;
            case Blob: {
                size_t count = blobCount(value.blobValue);
                if (count > 0) {
                    const uint8_t *buffer = blobBuffer(value.blobValue);
                    v = ByteArray(buffer, count);
                }
            }
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }

    double Variant::toAnalogValue(Type type, const Value &value) {
        if (isAnalogValue(type)) {
            double result = 0.0;
            changeValueInner<double>(type, value, result);
            return result;
        }
        return 0.0;
    }

    Variant::Value Variant::fromAnalogValue(Type type, double value) {
        Value result;
        result.lValue = 0;
        changeValueInner<double>(value, type, result);
        return result;
    }

    template<class T>
    bool Variant::changeValueInner(T v, Type type, Value &value) {
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
                return false;
            case Blob:
                return false;
            default:
                return false;
        }
        return true;
    }

    template<class T>
    bool Variant::changeValueInner(Type type, const Value &value, T &v) {
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
            case Text: {
                if (Convert::parseStr(value.strValue, v)) {
                    return true;
                }
                return false;
            }
            case Date:
                return false;
            case Blob:
                return false;
            default:
                return false;
        }
        return true;
    }

    const String Variant::typeStr() const {
        return toTypeStr(_type);
    }

    void Variant::writeValue(Stream *stream, Variant::Type type, const Variant::Value &value) {
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
                stream->writeInt16(value.sValue);
                break;
            case UInteger16:
                stream->writeUInt16(value.usValue);
                break;
            case Integer32:
                stream->writeInt32(value.nValue);
                break;
            case UInteger32:
                stream->writeUInt32(value.unValue);
                break;
            case Integer64:
                stream->writeInt64(value.lValue);
                break;
            case UInteger64:
                stream->writeInt64(value.ulValue);
                break;
            case Float32:
                stream->writeFloat(value.fValue);
                break;
            case Float64:
                stream->writeDouble(value.dValue);
                break;
            case Text:
                stream->writeStr(value.strValue);
                break;
            case Date:
                stream->writeUInt64(value.tValue);
                break;
            case Blob: {
                size_t count = blobCount(value.blobValue);
                stream->writeUInt16((uint16_t) count);
                if (count > 0) {
                    const uint8_t *buffer = blobBuffer(value.blobValue);
                    stream->write(buffer, 0, count);
                }
            }
                break;
            default:
                break;
        }
    }

    void Variant::readValue(Stream *stream, Variant::Type type, Variant::Value &value) {
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
                value.sValue = stream->readInt16();
                break;
            case UInteger16:
                value.usValue = stream->readUInt16();
                break;
            case Integer32:
                value.nValue = stream->readInt32();
                break;
            case UInteger32:
                value.unValue = stream->readUInt32();
                break;
            case Integer64:
                value.lValue = stream->readInt64();
                break;
            case UInteger64:
                value.ulValue = stream->readUInt64();
                break;
            case Float32:
                value.fValue = stream->readFloat();
                break;
            case Float64:
                value.dValue = stream->readDouble();
                break;
            case Text:
                setStringValue(stream->readStr(), Text, value);
                break;
            case Date:
                value.tValue = stream->readUInt64();
                break;
            case Blob: {
                size_t count = stream->readUInt16();
                if (count > 0) {
                    value.blobValue = new uint8_t[2 + count];
                    uint8_t high = (count >> 8) & 0xFF;
                    uint8_t low = count & 0xFF;
                    value.blobValue[0] = high;
                    value.blobValue[1] = low;
                    stream->read((value.blobValue + 2), 0, count);
                }
            }
                break;
            default:
                break;
        }
    }

    int Variant::valueSize(Type type, const Value &value) {
        int size = 0;
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
                size = sizeof(short) + (int) strlen(value.strValue);
                break;
            case Date:
                size = sizeof(value.tValue);
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
            case Blob:
                return "Blob";
            default:
                return "Null";
        }
    }

    Variant::Type Variant::fromTypeStr(const String &str) {
        if (String::equals(str, "Digital", true) ||
            String::equals(str, "Boolean", true) ||
            String::equals(str, "Bit", true)) {
            return Type::Digital;
        } else if (String::equals(str, "Int8", true) ||
                   String::equals(str, "Char", true) ||
                   String::equals(str, "Integer8", true)) {
            return Type::Integer8;
        } else if (String::equals(str, "UInt8", true) ||
                   String::equals(str, "Byte", true) ||
                   String::equals(str, "UInteger8", true)) {
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
        } else if (String::equals(str, "Int32", true) ||
                   String::equals(str, "Integer32", true) ||
                   String::equals(str, "Integer", true)) {
            return Type::Integer32;
        } else if (String::equals(str, "UInt32", true) ||
                   String::equals(str, "UInteger32", true) ||
                   String::equals(str, "DWORD", true) ||
                   String::equals(str, "UInteger", true)) {
            return Type::UInteger32;
        } else if (String::equals(str, "Integer64", true) ||
                   String::equals(str, "Int64", true)) {
            return Type::Integer64;
        } else if (String::equals(str, "UInteger64", true) ||
                   String::equals(str, "UInt64", true)) {
            return Type::UInteger64;
        } else if (String::equals(str, "Float32", true) ||
                   String::equals(str, "Single", true) ||
                   String::equals(str, "Float", true)) {
            return Type::Float32;
        } else if (String::equals(str, "Float64", true) ||
                   String::equals(str, "Double", true)) {
            return Type::Float64;
        } else if (String::equals(str, "Text", true) ||
                   String::equals(str, "String", true)) {
            return Type::Text;
        } else if (String::equals(str, "Date", true) ||
                   String::equals(str, "DateTime", true) ||
                   String::equals(str, "Time", true)) {
            return Type::Date;
        } else if (String::equals(str, "Blob", true) ||
                   String::equals(str, "ByteArray", true) ||
                   String::equals(str, "Bytes", true)) {
            return Type::Blob;
        }
        return Type::Null;
    }

    bool Variant::isValidType(Type type) {
        return type != Type::Null;
    }

    void Variant::getAllTypeStr(StringArray &array) {
        array.addArray("Digital", "Integer8", "UInteger8", "Integer16", "UInteger16", "Integer32", "UInteger32",
                       "Integer64", "UInteger64",
                       "Float32", "Float64", "Text", "Date", "Blob", nullptr);
    }

    int Variant::getTypeLength(Type type) {
        switch (type) {
            case Null:
                return 0;
            case Digital:
                return 1;
            case Integer8:
            case UInteger8:
                return 1;
            case Integer16:
            case UInteger16:
                return 2;
            case Integer32:
            case UInteger32:
                return 4;
            case Integer64:
            case UInteger64:
                return 8;
            case Float32:
                return 4;
            case Float64:
                return 8;
            case Text:
                throw ArgumentException("Text type is not allow to get type length.");
                break;
            case Date:
                return 8;
            case Blob:
                throw ArgumentException("Blob type is not allow to get type length.");
                break;
            default:
                break;
        }
        return 0;
    }

    bool Variant::isAnalogValue(Type type) {
        return (type == Variant::Integer8 || type == Variant::UInteger8 ||
                type == Variant::Integer16 || type == Variant::UInteger16 ||
                type == Variant::Integer32 || type == Variant::UInteger32 ||
                type == Variant::Integer64 || type == Variant::UInteger64 ||
                type == Variant::Float32 || type == Variant::Float64);
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

    bool Variant::isDoubleValue(Type type) {
        return (type == Variant::Float32 || type == Variant::Float64);
    }

    bool Variant::isStringValue(Type type) {
        return (type == Variant::Text);
    }

    void Variant::write(Stream *stream, bool bigEndian) const {
        stream->writeByte(_type);

        switch (_type) {
            case Null:
                break;
            case Digital:
                stream->writeBoolean(_value.bValue);
                break;
            case Integer8:
                stream->writeInt8(_value.cValue);
                break;
            case UInteger8:
                stream->writeUInt8(_value.ucValue);
                break;
            case Integer16:
                stream->writeInt16(_value.sValue, bigEndian);
                break;
            case UInteger16:
                stream->writeUInt16(_value.usValue, bigEndian);
                break;
            case Integer32:
                stream->writeInt32(_value.nValue, bigEndian);
                break;
            case UInteger32:
                stream->writeUInt32(_value.unValue, bigEndian);
                break;
            case Integer64:
                stream->writeInt64(_value.lValue, bigEndian);
                break;
            case UInteger64:
                stream->writeUInt64(_value.ulValue, bigEndian);
                break;
            case Float32:
                stream->writeFloat(_value.fValue);
                break;
            case Float64:
                stream->writeDouble(_value.dValue);
                break;
            case Text:
                stream->writeStr(_value.strValue);
                break;
            case Date: {
                DateTime time(_value.tValue);
                time.write(stream);
            }
                break;
            default:
                break;
        }
    }

    void Variant::read(Stream *stream, bool bigEndian) {
        _type = (Type) stream->readByte();

        switch (_type) {
            case Null:
                break;
            case Digital:
                _value.bValue = stream->readBoolean();
                break;
            case Integer8:
                _value.cValue = stream->readInt8();
                break;
            case UInteger8:
                _value.ucValue = stream->readUInt8();
                break;
            case Integer16:
                _value.sValue = stream->readInt16(bigEndian);
                break;
            case UInteger16:
                _value.usValue = stream->readUInt16(bigEndian);
                break;
            case Integer32:
                _value.nValue = stream->readInt32(bigEndian);
                break;
            case UInteger32:
                _value.unValue = stream->readUInt32(bigEndian);
                break;
            case Integer64:
                _value.lValue = stream->readInt64(bigEndian);
                break;
            case UInteger64:
                _value.ulValue = stream->readUInt64(bigEndian);
                break;
            case Float32:
                _value.fValue = stream->readFloat();
                break;
            case Float64:
                _value.dValue = stream->readDouble();
                break;
            case Text:
                setStringValue(stream->readStr());
                break;
            case Date: {
                DateTime time;
                time.read(stream);
                _value.tValue = time.ticks();
            }
                break;
            default:
                break;
        }
    }

    String Variant::toValueString(Type type, const Value &value) {
        String str;
        if (Variant::changeStringValue(type, value, str))
            return str;
        return String::Empty;
    }

    bool Variant::parseValueString(const String &str, Type type, Value &value) {
        return Variant::changeStringValue(str, type, value);
    }

    bool Variant::compareTextValue(const char *value1, const char *value2) {
        if (value1 != nullptr && value2 != nullptr)
            return strcmp(value1, value2) == 0;
        else if (value1 == nullptr && value2 == nullptr)
            return true;
        else
            return false;
    }

    bool Variant::compareBlobValue(const uint8_t *value1, const uint8_t *value2) {
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
            uint8_t high = value[0];
            uint8_t low = value[1];
            size_t count = (((uint32_t) high >> 8) & 0xFF00) + low;
            return count;
        }
        return 0;
    }

    const uint8_t *Variant::blobBuffer(const uint8_t *value) {
        if (value != nullptr) {
            return value + 2;
        }
        return nullptr;
    }

    const String Variant::toString() const {
        String value;
        getValue(value);
        return value;
    }
}
