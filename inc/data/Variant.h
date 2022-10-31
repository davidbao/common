//
//  Variant.h
//  common
//
//  Created by baowei on 2017/11/25.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef Variant_h
#define Variant_h

#include "Array.h"
#include "data/ValueType.h"
#include "exception/Exception.h"
#include "IO/Stream.h"
#include "data/DateTime.h"
#include "system/Convert.h"
#include "data/ByteArray.h"

namespace Common
{
    struct Variant
    {
    public:
        union Value
        {
            bool bValue;
            int8_t cValue;
            uint8_t ucValue;
            int16_t sValue;
            uint16_t usValue;
            int32_t nValue;
            uint32_t unValue;
            int64_t lValue;
            uint64_t ulValue;
            float fValue;
            double dValue;
            char* strValue;
            uint64_t tValue;    // 100-nanosecond ticks
            uint8_t* blobValue;
        };
        enum Type : uint8_t
        {
            Null = 0,
            Digital = 1,
            Integer8 = 2,
            UInteger8 = 3,
            Integer16 = 4,
            UInteger16 = 5,
            Integer32 = 6,
            UInteger32 = 7,
            Integer64 = 8,
            UInteger64 = 9,
            Float32 = 10,
            Float64 = 11,
            Text = 12,
            Date = 13,
            Blob = 14
        };
        
        explicit Variant(Type type = Null);
        explicit Variant(bool value);
        explicit Variant(int8_t value);
        explicit Variant(uint8_t value);
        explicit Variant(int16_t value);
        explicit Variant(uint16_t value);
        explicit Variant(int32_t value);
        explicit Variant(uint32_t value);
        explicit Variant(int64_t value);
        explicit Variant(uint64_t value);
        explicit Variant(float value);
        explicit Variant(double value);
        explicit Variant(const String& value);
        explicit Variant(DateTime value);
        explicit Variant(const ByteArray& value);
        ~Variant();
        
        bool isEmpty() const;

        Variant& operator=(const Variant& tag);
        bool operator==(const Variant& tag) const;
        bool operator!=(const Variant& tag) const;

        Variant& operator=(const Value& value);
        bool operator==(const Value& value) const;
        bool operator!=(const Value& value) const;

        Variant& operator=(const bool& value);
        explicit operator bool() const;
        bool operator==(const bool& value) const;
        bool operator!=(const bool& value) const;

        Variant& operator=(const int8_t& value);
        explicit operator int8_t() const;
        bool operator==(const int8_t& value) const;
        bool operator!=(const int8_t& value) const;

        Variant& operator=(const uint8_t& value);
        explicit operator uint8_t() const;
        bool operator==(const uint8_t& value) const;
        bool operator!=(const uint8_t& value) const;

        Variant& operator=(const int16_t& value);
        explicit operator int16_t() const;
        bool operator==(const int16_t& value) const;
        bool operator!=(const int16_t& value) const;

        Variant& operator=(const uint16_t& value);
        explicit operator uint16_t() const;
        bool operator==(const uint16_t& value) const;
        bool operator!=(const uint16_t& value) const;

        Variant& operator=(const int32_t& value);
        explicit operator int32_t() const;
        bool operator==(const int32_t& value) const;
        bool operator!=(const int32_t& value) const;

        Variant& operator=(const uint32_t& value);
        explicit operator uint32_t() const;
        bool operator==(const uint32_t& value) const;
        bool operator!=(const uint32_t& value) const;

        Variant& operator=(const int64_t& value);
        explicit operator int64_t() const;
        bool operator==(const int64_t& value) const;
        bool operator!=(const int64_t& value) const;

        Variant& operator=(const uint64_t& value);
        explicit operator uint64_t() const;
        bool operator==(const uint64_t& value) const;
        bool operator!=(const uint64_t& value) const;

        Variant& operator=(const float& value);
        explicit operator float() const;
        bool operator==(const float& value) const;
        bool operator!=(const float& value) const;

        Variant& operator=(const double& value);
        explicit operator double() const;
        bool operator==(const double& value) const;
        bool operator!=(const double& value) const;

        Variant& operator=(const String& value);
        explicit operator String() const;
        bool operator==(const String& value) const;
        bool operator!=(const String& value) const;

        Variant& operator=(const DateTime& value);
        explicit operator DateTime() const;
        bool operator==(const DateTime& value) const;
        bool operator!=(const DateTime& value) const;

        Variant& operator=(const ByteArray& value);
        explicit operator ByteArray() const;
        bool operator==(const ByteArray& value) const;
        bool operator!=(const ByteArray& value) const;
        
        bool isNullValue() const;
        void setNullValue();
        
        Type type() const;
        
        bool isAnalogValue() const;
        bool isDigitalValue() const;
        
        bool setValue(const bool& v);
        bool setValue(const int8_t& v);
        bool setValue(const uint8_t& v);
        bool setValue(const int16_t& v);
        bool setValue(const uint16_t& v);
        bool setValue(const int32_t& v);
        bool setValue(const uint32_t& v);
        bool setValue(const int64_t& v);
        bool setValue(const uint64_t& v);
        bool setValue(const float& v);
        bool setValue(const double& v);
        bool setValue(const String& v);
        bool setValue(const char* v);
        bool setValue(const DateTime& v);
        bool setValue(const ByteArray& v);
        
        Value value() const;
        bool getValue(Type type, Value& v) const;
        bool getValue(bool& v) const;
        bool getValue(int8_t& v) const;
        bool getValue(uint8_t& v) const;
        bool getValue(int16_t& v) const;
        bool getValue(uint16_t& v) const;
        bool getValue(int32_t& v) const;
        bool getValue(uint32_t& v) const;
        bool getValue(int64_t& v) const;
        bool getValue(uint64_t& v) const;
        bool getValue(float& v) const;
        bool getValue(double& v) const;
        bool getValue(String& v) const;
        const String valueStr() const;
        bool getValue(DateTime& v) const;
        bool getValue(ByteArray& v) const;
        template<class T>
        bool getValue(T& v) const;
        
        bool compareValue(const Value& v) const;
        bool compareValue(const bool& v) const;
        bool compareValue(const int8_t& v) const;
        bool compareValue(const uint8_t& v) const;
        bool compareValue(const int16_t& v) const;
        bool compareValue(const uint16_t& v) const;
        bool compareValue(const int32_t& v) const;
        bool compareValue(const uint32_t& v) const;
        bool compareValue(const int64_t& v) const;
        bool compareValue(const uint64_t& v) const;
        bool compareValue(const float& v) const;
        bool compareValue(const double& v) const;
        bool compareValue(const String& v) const;
        bool compareValue(const DateTime& v) const;
        bool compareValue(const ByteArray& v) const;
        bool compareValue(const Variant& v) const;
        
        void write(Stream* stream, bool bigEndian = true) const;
        void read(Stream* stream, bool bigEndian = true);
        
        const String typeStr() const;
        
        const String toString() const;
        
    public:
        static bool isNullValue(const Variant& tag);
        
        static bool isValueEqual(Type type, const Value& value1, const Value& value2);
        
        static String toTypeScriptStr(Type type);
        static String toTypeStr(Type type);
        static Type fromTypeStr(const String& str);
        static bool isValidType(Type type);
        static void getAllTypeStr(StringArray& array);
        static int getTypeLength(Type type);
        
        static bool isAnalogValue(Type type);
        static bool isDigitalValue(Type type);
        static bool isIntegerValue(Type type);
        static bool isDoubleValue(Type type);
        static bool isStringValue(Type type);
        
        static bool changeValue(const Variant* tag, Type destType, Value& destValue);
        static bool changeValue(Type type, const Value& value, Type destType, Value& destValue);
        
        static bool compareValue(Type type1, const Value& value1, Type type2, const Value& value2);
        template<class T>
        static bool compareValue(Type type, const Value& value, const T& v);
        static bool compareValue(Type type, const Value& value, const bool& v);
        static bool compareValue(Type type, const Value& value, const int8_t& v);
        static bool compareValue(Type type, const Value& value, const uint8_t& v);
        static bool compareValue(Type type, const Value& value, const int16_t& v);
        static bool compareValue(Type type, const Value& value, const uint16_t& v);
        static bool compareValue(Type type, const Value& value, const int32_t& v);
        static bool compareValue(Type type, const Value& value, const uint32_t& v);
        static bool compareValue(Type type, const Value& value, const int64_t& v);
        static bool compareValue(Type type, const Value& value, const uint64_t& v);
        static bool compareValue(Type type, const Value& value, const float& v);
        static bool compareValue(Type type, const Value& value, const double& v);
        static bool compareValue(Type type, const Value& value, const String& v);
        
        static bool changeDigitalValue(bool v, Type type, Value& value);
        static bool changeInt8Value(int8_t v, Type type, Value& value);
        static bool changeUInt8Value(uint8_t v, Type type, Value& value);
        static bool changeInt16Value(int16_t v, Type type, Value& value);
        static bool changeUInt16Value(uint16_t v, Type type, Value& value);
        static bool changeInt32Value(int32_t v, Type type, Value& value);
        static bool changeUInt32Value(uint32_t v, Type type, Value& value);
        static bool changeInt64Value(int64_t v, Type type, Value& value);
        static bool changeUInt64Value(uint64_t v, Type type, Value& value);
        static bool changeFloat64Value(double v, Type type, Value& value);
        static bool changeFloat32Value(float v, Type type, Value& value);
        static bool changeStringValue(const char* v, Type type, Value& value);
        static bool changeStringValue(const String& v, Type type, Value& value);
        static bool changeDateValue(const DateTime& v, Type type, Value& value);
        static bool changeByteArrayValue(const ByteArray& v, Type type, Value& value);
        static bool changeByteArrayValue(const uint8_t* v, Type type, Value& value);
        
        static bool changeDigitalValue(Type type, const Value& value, bool& v);
        static bool changeInt8Value(Type type, const Value& value, int8_t& v);
        static bool changeUInt8Value(Type type, const Value& value, uint8_t& v);
        static bool changeInt16Value(Type type, const Value& value, int16_t& v);
        static bool changeUInt16Value(Type type, const Value& value, uint16_t& v);
        static bool changeInt32Value(Type type, const Value& value, int32_t& v);
        static bool changeUInt32Value(Type type, const Value& value, uint32_t& v);
        static bool changeInt64Value(Type type, const Value& value, int64_t& v);
        static bool changeUInt64Value(Type type, const Value& value, uint64_t& v);
        static bool changeFloat32Value(Type type, const Value& value, float& v);
        static bool changeFloat64Value(Type type, const Value& value, double& v);
        static bool changeStringValue(Type type, const Value& value, String& v);
        static bool changeDateValue(Type type, const Value& value, DateTime& v);
        static bool changeByteArrayValue(Type type, const Value& value, ByteArray& v);
        
        static double toAnalogValue(Type type, const Value& value);
        static Value fromAnalogValue(Type type, double value);
        
        static void writeValue(Stream* stream, Type type, const Value& value);
        static void readValue(Stream* stream, Type type, Value& value);
        static int valueSize(Type type, const Value& value);
        
        static String toValueString(Type type, const Value& value);
        static bool parseValueString(const String& str, Type type, Value& value);
        static void setStringValue(const String& str, Type type, Value& value);
        static void setByteArrayValue(const uint8_t* buffer, Type type, Value& value);
        static void setByteArrayValue(const ByteArray& buffer, Type type, Value& value);
        
        static bool compareTextValue(const char* value1, const char* value2);
        static bool compareBlobValue(const uint8_t* value1, const uint8_t* value2);
        static size_t blobCount(const uint8_t* value);
        static const uint8_t* blobBuffer(const uint8_t* value);
        
    private:
        void setValue(const Value& v);
        void setStringValue(const String& str);
        void setByteArrayValue(const uint8_t* buffer);
        void setByteArrayValue(const ByteArray& buffer);
        
        void setValueInner(const Value& value);
        
        template<class T>
        bool setValue(const T& v);
        
    private:
        template<class T>
        static bool changeValueInner(T v, Type type, Value& value);
        template<class T>
        static bool changeValueInner(Type type, const Value& value, T& v);
        
    private:
        Type _type;
        Value _value;
    };
}


#endif /* Variant_h */
