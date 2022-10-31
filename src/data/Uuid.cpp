//
//  Uuid.cpp
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#include "data/Uuid.h"
#include "IO/Stream.h"
#if WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif

namespace Common
{
    const Uuid Uuid::Empty = Uuid();
    Uuid::Uuid()
    {
    	clear();
    }
    Uuid::Uuid(const Uuid& value)
    {
        this->operator=(value);
    }
    Uuid::Uuid(const String& value)
    {
        parse(value, *this);
    }
    Uuid::~Uuid()
    {
	}
    
    void Uuid::operator=(const Uuid& value)
    {
#if WIN32
		memcpy(&this->_value, &value._value, Size);
#else
        uuid_copy(this->_value, value._value);
#endif
        assert(this->operator==(value));
	}
    bool Uuid::operator==(const Uuid& value) const
    {
#if WIN32
		RPC_STATUS status;
		return UuidCompare((uuid_t*)&this->_value, (uuid_t*)&value._value, &status) == 0;
#else
        return uuid_compare(this->_value, value._value) == 0;
#endif
	}
    bool Uuid::operator!=(const Uuid& value) const
    {
        return !operator==(value);
    }
    void Uuid::operator=(const String& value)
    {
        Uuid id;
        if(Uuid::parse(value, id))
        {
            this->operator=(id);
        }
    }
    bool Uuid::operator==(const String& value) const
    {
        Uuid id;
        if(Uuid::parse(value, id))
        {
            return this->operator==(id);
        }
        return false;
    }
    bool Uuid::operator!=(const String& value) const
    {
        return !operator==(value);
    }
    
    bool Uuid::operator>(const Uuid& value) const
    {
#if WIN32
        RPC_STATUS status;
        return UuidCompare((uuid_t*)&this->_value, (uuid_t*)&value._value, &status) > 0;
#else
        return uuid_compare(this->_value, value._value) > 0;
#endif
    }
    bool Uuid::operator>=(const Uuid& value) const
    {
#if WIN32
        RPC_STATUS status;
        return UuidCompare((uuid_t*)&this->_value, (uuid_t*)&value._value, &status) >= 0;
#else
        return uuid_compare(this->_value, value._value) >= 0;
#endif
    }
    bool Uuid::operator<(const Uuid& value) const
    {
#if WIN32
        RPC_STATUS status;
        return UuidCompare((uuid_t*)&this->_value, (uuid_t*)&value._value, &status) < 0;
#else
        return uuid_compare(this->_value, value._value) < 0;
#endif
    }
    bool Uuid::operator<=(const Uuid& value) const
    {
#if WIN32
        RPC_STATUS status;
        return UuidCompare((uuid_t*)&this->_value, (uuid_t*)&value._value, &status) <= 0;
#else
        return uuid_compare(this->_value, value._value) <= 0;
#endif
    }
    
    void Uuid::write(Stream* stream) const
    {
#if WIN32
		stream->writeUInt32(_value.Data1, true);
		stream->writeUInt16(_value.Data2, true);
		stream->writeUInt16(_value.Data3, true);
		stream->write((uint8_t*)&_value.Data4, 0, sizeof(_value.Data4));
#else
        stream->write(_value, 0, Size);
#endif
	}
    void Uuid::read(Stream* stream)
    {
#if WIN32
		_value.Data1 = stream->readUInt32(true);
		_value.Data2 = stream->readUInt16(true);
		_value.Data3 = stream->readUInt16(true);
		stream->read((uint8_t*)&_value.Data4, 0, sizeof(_value.Data4));
#else
		stream->read(_value, 0, Size);
#endif
    }
    
    void Uuid::empty()
    {
        clear();
    }
    bool Uuid::isEmpty() const
    {
#if WIN32
		RPC_STATUS status;
		return UuidIsNil((uuid_t*)&_value, &status) != 0;
#else
        return uuid_is_null(_value) == 1;
#endif
    }
    void Uuid::clear()
    {
#if WIN32
        UuidCreateNil(&_value);
#else
        uuid_clear(_value);
#endif
    }
    
    String Uuid::toString(bool upper) const
    {
        if(isEmpty())
            return String::Empty;
        
#if WIN32
		char* buffer;
		if (UuidToString((uuid_t*)&_value, (RPC_CSTR*)&buffer) == RPC_S_OK)
		{
			String str = (String)buffer;
			RpcStringFree((RPC_CSTR*)&buffer);
            return upper ? str.toUpper() : str.toLower();
		}
        return String::Empty;
#else
        char buffer[64];
        if(upper)
            uuid_unparse_upper(_value, buffer);
        else
            uuid_unparse_lower(_value, buffer);
        return (String)buffer;
#endif
	}
    bool Uuid::parse(const String& str, Uuid& value)
    {
        if(str.isNullOrEmpty())
            return false;
#if WIN32
		return (UuidFromString((RPC_CSTR)str.c_str(), &value._value) == RPC_S_OK);
#else
        return uuid_parse(str, value._value) == 0;
#endif
	}

	Uuid Uuid::generate()
	{
		Uuid uuid;
#if WIN32
		UuidCreate(&uuid._value);
#else
		uuid_generate(uuid._value);
#endif
		assert(!uuid.isEmpty());
		return uuid;
	}
}
