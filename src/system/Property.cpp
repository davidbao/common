//
//  Property.cpp
//  common
//
//  Created by baowei on 16/8/24.
//  Copyright © 2016年 com. All rights reserved.
//

#include "system/Property.h"
#include <assert.h>
#include "system/Convert.h"

namespace Common
{
    Property::Value::Value()
    {
    }
    Property::Value::~Value()
    {
    }
    
    Property Property::Empty = Property();
    Property::Property() : Property(0, String::Empty, Type::Null)
    {
    }
    Property::Property(const Property& property)
    {
        this->operator=(property);
    }
    Property::Property(uint ownerId, const String& name, Type type, const FixedValue& value) : Property(ownerId, name, type)
    {
        setValue(type, value);
    }
    Property::Property(uint ownerId, const String& name, Type type)
    {
        this->ownerId = ownerId;
        this->name = name;
        this->type = type;
        this->value.lValue = 0;
    }
    Property::Property(uint ownerId, const String& name, bool value) : Property(ownerId, name, Type::Digital)
    {
        this->value.bValue = value;
    }
    Property::Property(uint ownerId, const String& name, char value) : Property(ownerId, name, Type::Integer8)
    {
        this->value.cValue = value;
    }
    Property::Property(uint ownerId, const String& name, uint8_t value) : Property(ownerId, name, Type::Integer8)
    {
        this->value.cValue = value;
    }
    Property::Property(uint ownerId, const String& name, short value) : Property(ownerId, name, Type::Integer16)
    {
        this->value.sValue = value;
    }
    Property::Property(uint ownerId, const String& name, ushort value) : Property(ownerId, name, Type::Integer16)
    {
        this->value.sValue = value;
    }
    Property::Property(uint ownerId, const String& name, int value) : Property(ownerId, name, Type::Integer32)
    {
        this->value.nValue = value;
    }
    Property::Property(uint ownerId, const String& name, uint value) : Property(ownerId, name, Type::Integer32)
    {
        this->value.nValue = value;
    }
    Property::Property(uint ownerId, const String& name, int64_t value) : Property(ownerId, name, Type::Integer64)
    {
        this->value.lValue = value;
    }
    Property::Property(uint ownerId, const String& name, uint64_t value) : Property(ownerId, name, Type::Integer64)
    {
        this->value.tValue = value;
    }
    Property::Property(uint ownerId, const String& name, float value) : Property(ownerId, name, Type::Float32)
    {
        this->value.fValue = value;
    }
    Property::Property(uint ownerId, const String& name, double value) : Property(ownerId, name, Type::Float64)
    {
        this->value.dValue = value;
    }    
    Property::Property(uint ownerId, const String& name, const String& value) : Property(ownerId, name, Type::Text)
    {
        this->value.strValue = nullptr;
        setStringValue(value, Type::Text, this->value);
    }
    Property::Property(uint ownerId, const String& name, const DateTime& value) : Property(ownerId, name, Type::Date)
    {
        this->value.tValue = value.ticks();
    }
    Property::Property(uint ownerId, const String& name, const TimeSpan& value) : Property(ownerId, name, Type::TimeRange)
    {
        this->value.tValue = value.ticks();
    }
    Property::Property(uint ownerId, const String& name, const Value& value) : Property(ownerId, name, Type::Custom)
    {
        this->value.objValue = value.clone();
    }
    Property::~Property()
    {
        if (type == Text)
        {
            assert(value.strValue);
            delete[] value.strValue;
            value.strValue = nullptr;
        }
        else if(type == Custom)
        {
            assert(value.objValue);
            delete value.objValue;
            value.objValue = nullptr;
        }
    }
    
    void Property::setValue(Type type, const FixedValue& value)
    {
        this->type = type;
        if (type == Text)
        {
            setStringValue(value.strValue, type, this->value);
        }
        else if(type == Custom)
        {
            this->value.objValue = value.objValue->clone();
        }
        else
        {
            this->value = value;
        }
    }
    
    void Property::operator=(const Property& property)
    {
        this->ownerId = property.ownerId;
        this->name = property.name;
        this->type = property.type;
        this->value.strValue = nullptr;
        this->setValue(property.type, property.value);
    }
    bool Property::operator==(const Property& property) const
    {
        if(this->ownerId != property.ownerId)
            return false;
        if(this->name != property.name)
            return false;
        if(this->type != property.type)
            return false;
        
        switch (type)
        {
            case Null:
                return false;
            case Digital:
                return this->value.bValue == property.value.bValue;
            case Integer8:
                return this->value.cValue == property.value.cValue;
            case Integer16:
                return this->value.sValue == property.value.sValue;
            case Integer32:
                return this->value.nValue == property.value.nValue;
            case Integer64:
                return this->value.lValue == property.value.lValue;
            case Float32:
                return this->value.fValue == property.value.fValue;
            case Float64:
                return this->value.dValue == property.value.dValue;
            case Text:
                if(this->value.strValue == nullptr ||
                   property.value.strValue == nullptr)
                {
                    return false;
                }
                return strcmp(this->value.strValue, property.value.strValue) == 0;
            case Date:
                return this->value.tValue == property.value.tValue;
            case TimeRange:
                return this->value.tValue == property.value.tValue;
            case Custom:
                return this->value.objValue->equals(property.value.objValue);
            default:
                break;
        }
        return false;
    }
    bool Property::operator!=(const Property& property) const
    {
        return !operator==(property);
    }
    
    void Property::setStringValue(const String& str, Type type, FixedValue& value)
    {
        if (type == Text)
        {
            if (value.strValue != nullptr)
            {
                delete[] value.strValue;
            }
            size_t len = strlen(str);
            if (len > 0)
            {
                value.strValue = new char[len + 1];
                strcpy(value.strValue, str);
            }
            else
            {
                value.strValue = new char[1];
                value.strValue[0] = '\0';
            }
        }
    }
    
    void Property::write(Stream* stream, bool bigEndian) const
    {
        UInt32(this->ownerId).write(stream, bigEndian);
        this->name.write(stream);
        Byte(this->type).write(stream);
        switch (type)
        {
            case Null:
                break;
            case Digital:
                Boolean(this->value.bValue).write(stream);
                break;
            case Integer8:
                Byte(this->value.cValue).write(stream);
                break;
            case Integer16:
                Int16(this->value.sValue).write(stream, bigEndian);
                break;
            case Integer32:
                Int32(this->value.nValue).write(stream, bigEndian);
                break;
            case Integer64:
                Int64(this->value.lValue).write(stream, bigEndian);
                break;
            case Float32:
                Float(this->value.fValue).write(stream, bigEndian);
                break;
            case Float64:
                Double(this->value.dValue).write(stream, bigEndian);
                break;
            case Text:
            {
                String str = value.strValue;
                str.write(stream);
            }
                break;
            case Date:
                UInt64(this->value.tValue).write(stream, bigEndian);
                break;
            case TimeRange:
                UInt64(this->value.tValue).write(stream, bigEndian);
                break;
            case Custom:
                this->value.objValue->write(stream, bigEndian);
                break;
            default:
                break;
        }
    }
    void Property::read(Stream* stream, bool bigEndian)
    {
        this->ownerId = stream->readUInt32(bigEndian);
        this->name.read(stream);
        this->type = (Type)stream->readByte();
        switch (type)
        {
            case Null:
                break;
            case Digital:
                this->value.bValue = stream->readBoolean();
                break;
            case Integer8:
                this->value.cValue = stream->readByte();
                break;
            case Integer16:
                this->value.sValue = stream->readInt16(bigEndian);
                break;
            case Integer32:
                this->value.nValue = stream->readInt32(bigEndian);
                break;
            case Integer64:
                this->value.lValue = stream->readInt64(bigEndian);
                break;
            case Float32:
                this->value.fValue = stream->readFloat(bigEndian);
                break;
            case Float64:
                this->value.dValue = stream->readDouble(bigEndian);
                break;
            case Text:
            {
                String str = stream->readStr();
                setStringValue(str, type, value);
            }
                break;
            case Date:
                this->value.tValue = stream->readUInt64(bigEndian);
                break;
            case TimeRange:
                this->value.tValue = stream->readUInt64(bigEndian);
                break;
            case Custom:
                this->value.objValue->read(stream, bigEndian);
                break;
            default:
                break;
        }
    }
    void Property::copyFrom(const Property* property)
    {
        this->ownerId = property->ownerId;
        this->name = property->name;
        this->type = property->type;
        this->setValue(property->type, property->value);
    }
    
    Property* Property::clone() const
    {
        Property* property = new Property();
        property->copyFrom(this);
        return property;
    }
    
    const String Property::toString() const
    {
        String valueStr;
        toString(type, value, valueStr);
        return String::convert("ownerId: %d, name: %s, type: %s, value: %s",
                               ownerId, name.c_str(), toTypeStr(type).c_str(), valueStr.c_str());
    }
    
    bool Property::toString(Type type, const FixedValue& value, String& v)
    {
        bool changed = true;
        switch (type)
        {
            case Null:
                changed = false;
                break;
            case Digital:
                v = Convert::convertStr(value.bValue);
                break;
            case Integer8:
                v = Convert::convertStr(value.cValue);
                break;
            case Integer16:
                v = Convert::convertStr(value.sValue);
                break;
            case Integer32:
                v = Convert::convertStr(value.nValue);
                break;
            case Integer64:
                v = Convert::convertStr(value.lValue);
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
            case Date:
            {
                DateTime time(value.tValue);
                v = time.toString();
                break;
            }
            case TimeRange:
            {
                TimeSpan ts(value.tValue);
                v = ts.toString();
                break;
            }
            case Custom:
                v = value.objValue->toString();
                break;
            default:
                changed = false;
                break;
        }
        return changed;
    }
    String Property::toTypeStr(Type type)
    {
        switch (type)
        {
            case Null:
                return "Null";
            case Digital:
                return "Digital";
            case Integer8:
                return "Integer8";
            case Integer16:
                return "Integer16";
            case Integer32:
                return "Integer32";
            case Integer64:
                return "Integer64";
            case Float32:
                return "Float32";
            case Float64:
                return "Float64";
            case Text:
                return "Text";
            case Date:
                return "Date";
            case TimeRange:
                return "TimeRange";
            case Custom:
                return "Custom";
            default:
                return "Null";
        }
    }
    Property::Type Property::fromTypeStr(const String& str)
    {
        if(String::equals(str, "Digital", true) ||
           String::equals(str, "Boolean", true) ||
           String::equals(str, "Bit", true))
        {
            return Type::Digital;
        }
        else if(String::equals(str, "Int8", true) ||
                String::equals(str, "Byte", true) ||
                String::equals(str, "Integer8", true))
        {
            return Type::Integer8;
        }
        else if(String::equals(str, "Int16", true) ||
                String::equals(str, "Short", true) ||
                String::equals(str, "WORD", true) ||
                String::equals(str, "Integer16", true))
        {
            return Type::Integer16;
        }
        else if(String::equals(str, "Int32", true) ||
                String::equals(str, "Integer32", true) ||
                String::equals(str, "DWORD", true) ||
                String::equals(str, "Integer", true))
        {
            return Type::Integer32;
        }
        else if(String::equals(str, "Integer64", true))
        {
            return Type::Integer64;
        }
        else if(String::equals(str, "Float32", true) ||
                String::equals(str, "Single", true) ||
                String::equals(str, "Float", true))
        {
            return Type::Float32;
        }
        else if(String::equals(str, "Float64", true) ||
                String::equals(str, "Double", true))
        {
            return Type::Float64;
        }
        else if(String::equals(str, "Text", true))
        {
            return Type::Text;
        }
        else if(String::equals(str, "Date", true) ||
                String::equals(str, "DateTime", true) ||
                String::equals(str, "Time", true))
        {
            return Type::Date;
        }
        else if(String::equals(str, "TimeRange", true) ||
                String::equals(str, "TimeSpan", true))
        {
            return Type::TimeRange;
        }
        else if(String::equals(str, "Custom", true))
        {
            return Type::Custom;
        }
        return Type::Null;
    }
    
    IPropertyChanged::IPropertyChanged()
    {
    }
    IPropertyChanged::~IPropertyChanged()
    {
    }
    Delegates* IPropertyChanged::propertyChangedDelegates()
    {
        return &_propertyChangedDelegates;
    }
    void IPropertyChanged::propertyChanged(const Property& property)
    {
        PropertyChangedEventArgs args(property);
        _propertyChangedDelegates.invoke(this, &args);
    }
    
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, bool value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, char value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, uint8_t value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, short value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, ushort value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, int value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, uint value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, int64_t value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, uint64_t value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, float value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, double value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, const String& value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, const DateTime& value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, const TimeSpan& value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
    void IPropertyChanged::propertyChanged(uint ownerId, const String& propName, const Property::Value& value)
    {
        propertyChanged(Property(ownerId, propName, value));
    }
}
