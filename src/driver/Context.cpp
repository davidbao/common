#include "driver/Context.h"

namespace Drivers
{
    ValueSet::ValueSet() : type(ValueEmpty)
    {
        Value.iValue = 0;
    }
    ValueSet::ValueSet(const String& value)
    {
        type = ValueStr;
        strcpy(Value.strValue, value.c_str());
    }
    ValueSet::ValueSet(int value)
    {
        type = ValueInt;
        Value.iValue = value;
    }
    bool ValueSet::isEmpty() const
    {
        return type == ValueEmpty;
    }
    
    Data::Data()
    {
        _name = String::Empty;
    }
    Data::Data(const String& name, ValueSet value)
    {
        _name = name;
        _value = value;
    }

    const String& Data::name() const
    {
        return _name;
    }
    
    ValueSet Data::getValue() const
    {
        return _value;
    }
    
	Context::Context()
	{
//        _data = new Datas();
	}

	Context::~Context()
	{
//        delete _data;
//        _data = NULL;
	}

//    void Context::setValue(const String& name, ValueSet value)
//    {
//        _data->add(new Data(name, value));
//    }
//
//    ValueSet Context::getValue(const String& name)
//    {
//        Data* data = find(name);
//        return data != NULL ? data->getValue() : ValueSet();
//    }
//
//    Data* Context::find(const String& name)
//    {
//        for (size_t i = 0; i < _data->count(); i++)
//        {
//            Data *data=_data->at(i);
//            if(data->name() == name)
//            {
//                return data;
//            }
//        }
//        return NULL;
//    }
}
