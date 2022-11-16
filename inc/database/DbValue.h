#ifndef DBVALUE_H
#define DBVALUE_H

#include <assert.h>
#include "data/PList.h"
#include "data/Dictionary.h"
#include "data/ValueType.h"
#include "data/DateTime.h"
#include "IO/Stream.h"

using namespace Common;

namespace Database
{
    union Value
    {
        bool bValue;
        uint8_t cValue;
        short sValue;
        int nValue;
        int64_t lValue;
        uint64_t ulValue;
        float fValue;
        double dValue;
        char* strValue;     // String
        uint64_t tValue;    // 100-nanosecond ticks, DateTime
        uint8_t* blobValue;    // ByteArray
    };
    
	enum ValueTypes : uint8_t
	{
		Null = 0,
        Digital = 1,
        Integer8 = 2,
        Integer16 = 3,
		Integer32 = 4,
		Integer64 = 5,
		Float32 = 6,
        Float64 = 7,
		Text = 8,
		Date = 9,
		Timestamp = 10,
        Blob = 11,
	};

	struct DbValue
	{
	public:
		DbValue(ValueTypes type);
		DbValue(ValueTypes type, const Value& value);
		~DbValue();

		ValueTypes type() const;
		const Value& value() const;

		static bool isNullValue(const Value& value);
		bool isNullValue() const;
		static Value nullValue();
		void setNullValue();

		void copyFrom(const DbValue* value);

		bool operator==(const Value& value);
		bool operator!=(const Value& value);
		bool operator==(const DbValue& value);
		bool operator!=(const DbValue& value);

		void operator=(const Value& value);
		void operator=(const DbValue& value);
		void operator=(const String& value);
		void operator=(const uint8_t& value);
		void operator=(const short& value);
		void operator=(const int& value);
		void operator=(const int64_t& value);
		void operator=(const float& value);
		void operator=(const double& value);
		void operator=(const bool& value);
		void operator=(const char* value);
		void operator=(const DateTime& value);

		void write(Stream* stream, bool bigEndian = true) const;
		void read(Stream* stream, bool bigEndian = true);

		void writeStr(Stream* stream, String::StreamLength lengthCount = String::StreamLength2) const;
		void readStr(Stream* stream, String::StreamLength lengthCount = String::StreamLength2);
		void writeFixedLengthStr(Stream* stream, int length) const;
		void readFixedLengthStr(Stream* stream, int length);

		void writeByte(Stream* stream) const;
		void readByte(Stream* stream);

		void writeBCDDateTime(Stream* stream, bool includedSec = true, bool includedMs = false) const;
		void readBCDDateTime(Stream* stream, bool includedSec = true, bool includedMs = false);

	private:
		void setStringValue(const String& str);
		void setStringValue(const char* str);

	private:
		enum ValueFlag : uint8_t
		{
			NullFlag = 0xF7,
			NormalFlag = 0x02
		};

		ValueTypes _type;
		Value _value;
	};
}
#endif // DBVALUE_H
