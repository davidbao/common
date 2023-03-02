//
//  Variant.h
//  common
//
//  Created by baowei on 2017/11/25.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef Variant_h
#define Variant_h

#include "data/String.h"
#include "exception/Exception.h"
#include "IO/Stream.h"
#include "data/DateTime.h"
#include "data/Convert.h"
#include "data/ByteArray.h"

namespace Data {
    struct Variant
            : public IEvaluation<Variant>,
              public IEquatable<Variant>,
              public IEquatable<Variant, bool>,
              public IEquatable<Variant, int8_t>,
              public IEquatable<Variant, uint8_t>,
              public IEquatable<Variant, int16_t>,
              public IEquatable<Variant, uint16_t>,
              public IEquatable<Variant, int32_t>,
              public IEquatable<Variant, uint32_t>,
              public IEquatable<Variant, int64_t>,
              public IEquatable<Variant, uint64_t>,
              public IEquatable<Variant, float>,
              public IEquatable<Variant, double>,
              public IEquatable<Variant, char *>,
              public IEquatable<Variant, String>,
              public IEquatable<Variant, DateTime>,
              public IEquatable<Variant, TimeSpan>,
              public IEquatable<Variant, ByteArray>,
              public IComparable<Variant> {
    public:
        enum Type : uint8_t {
            Null = 0,
            Digital = 1,
            Bool = Digital,
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
            Timestamp = 14,
            Blob = 15
        };

        union Value {
            bool bValue;
            int8_t cValue;
            uint8_t ucValue;
            int16_t sValue;
            uint16_t usValue;
            int32_t nValue;
            uint32_t unValue;
            int64_t lValue{};
            uint64_t ulValue;
            float fValue;
            double dValue;
            char *strValue;
            uint64_t dateValue;    // 100-nanosecond ticks
            int64_t timeValue;     // 100-nanosecond ticks
            uint8_t *blobValue;

            Value() : lValue(0) {
            }

            explicit Value(bool value) {
                bValue = value;
            }

            explicit Value(int8_t value) {
                cValue = value;
            }

            explicit Value(uint8_t value) {
                ucValue = value;
            }

            explicit Value(int16_t value) {
                sValue = value;
            }

            explicit Value(uint16_t value) {
                usValue = value;
            }

            explicit Value(int32_t value) {
                nValue = value;
            }

            explicit Value(uint32_t value) {
                unValue = value;
            }

            explicit Value(int64_t value) {
                lValue = value;
            }

            explicit Value(uint64_t value) {
                ulValue = value;
            }

            explicit Value(float value) {
                fValue = value;
            }

            explicit Value(double value) {
                dValue = value;
            }

            explicit Value(char *value) {
                strValue = value;
            }

            explicit Value(const DateTime &value) {
                dateValue = value.ticks();
            }

            explicit Value(const TimeSpan &value) {
                timeValue = value.ticks();
            }
        };

        explicit Variant(Type type = Null);

        explicit Variant(Type type, bool v);

        explicit Variant(Type type, int8_t v);

        explicit Variant(Type type, uint8_t v);

        explicit Variant(Type type, int16_t v);

        explicit Variant(Type type, uint16_t v);

        explicit Variant(Type type, int32_t v);

        explicit Variant(Type type, uint32_t v);

        explicit Variant(Type type, int64_t v);

        explicit Variant(Type type, uint64_t v);

        explicit Variant(Type type, float v);

        explicit Variant(Type type, double v);

        explicit Variant(Type type, const char *v);

        explicit Variant(Type type, const String &v);

        explicit Variant(Type type, const DateTime &v);

        explicit Variant(Type type, const TimeSpan &v);

        explicit Variant(Type type, const ByteArray &v);

        explicit Variant(Type type, const Value &v);

        explicit Variant(bool v);

        explicit Variant(int8_t v);

        explicit Variant(uint8_t v);

        explicit Variant(int16_t v);

        explicit Variant(uint16_t v);

        explicit Variant(int32_t v);

        explicit Variant(uint32_t v);

        explicit Variant(int64_t v);

        explicit Variant(uint64_t v);

        explicit Variant(float v);

        explicit Variant(double v);

        explicit Variant(const char *v);

        explicit Variant(const String &v);

        explicit Variant(const DateTime &v);

        explicit Variant(const TimeSpan &v);

        explicit Variant(const ByteArray &v);

        Variant(const Variant &v);

        ~Variant() override;

        void evaluates(const Variant &other) override;

        bool equals(const Variant &other) const override;

        bool equals(const bool &other) const override;

        bool equals(const int8_t &other) const override;

        bool equals(const uint8_t &other) const override;

        bool equals(const int16_t &other) const override;

        bool equals(const uint16_t &other) const override;

        bool equals(const int32_t &other) const override;

        bool equals(const uint32_t &other) const override;

        bool equals(const int64_t &other) const override;

        bool equals(const uint64_t &other) const override;

        bool equals(const float &other) const override;

        bool equals(const double &other) const override;

        bool equals(const char *other) const override;

        bool equals(const String &other) const override;

        bool equals(const DateTime &other) const override;

        bool equals(const TimeSpan &other) const override;

        bool equals(const ByteArray &other) const override;

        int compareTo(const Variant &other) const override;

        bool isNullType() const;

        bool isNullValue() const;

        void setNullValue();

        Type type() const;

        const Value &value() const;

        String valueStr() const;

        String typeStr() const;

        String toString() const;

        bool isAnalogValue() const;

        bool isDigitalValue() const;

        bool isIntegerValue() const;

        bool isFloatValue() const;

        bool isStringValue() const;

        bool isDateTimeValue() const;

        size_t valueSize() const;

        bool getValue(Type type, Value &v) const;

        bool getValue(bool &v) const;

        bool getValue(int8_t &v) const;

        bool getValue(uint8_t &v) const;

        bool getValue(int16_t &v) const;

        bool getValue(uint16_t &v) const;

        bool getValue(int32_t &v) const;

        bool getValue(uint32_t &v) const;

        bool getValue(int64_t &v) const;

        bool getValue(uint64_t &v) const;

        bool getValue(float &v) const;

        bool getValue(double &v) const;

        bool getValue(String &v) const;

        bool getValue(DateTime &v) const;

        bool getValue(TimeSpan &v) const;

        bool getValue(ByteArray &v) const;

        bool setValue(Type type, const Value &v);

        bool setValue(const bool &v);

        bool setValue(const int8_t &v);

        bool setValue(const uint8_t &v);

        bool setValue(const int16_t &v);

        bool setValue(const uint16_t &v);

        bool setValue(const int32_t &v);

        bool setValue(const uint32_t &v);

        bool setValue(const int64_t &v);

        bool setValue(const uint64_t &v);

        bool setValue(const float &v);

        bool setValue(const double &v);

        bool setValue(const String &v);

        bool setValue(const char *v);

        bool setValue(const DateTime &v);

        bool setValue(const TimeSpan &v);

        bool setValue(const ByteArray &v);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        Variant &operator=(const Variant &other);

        Variant &operator=(const bool &value);

        Variant &operator=(const int8_t &value);

        Variant &operator=(const uint8_t &value);

        Variant &operator=(const int16_t &value);

        Variant &operator=(const uint16_t &value);

        Variant &operator=(const int32_t &value);

        Variant &operator=(const uint32_t &value);

        Variant &operator=(const int64_t &value);

        Variant &operator=(const uint64_t &value);

        Variant &operator=(const float &value);

        Variant &operator=(const double &value);

        Variant &operator=(const String &value);

        Variant &operator=(const DateTime &value);

        Variant &operator=(const TimeSpan &value);

        Variant &operator=(const ByteArray &value);

        operator bool() const;

        operator int8_t() const;

        operator uint8_t() const;

        operator int16_t() const;

        operator uint16_t() const;

        operator int32_t() const;

        operator uint32_t() const;

        operator int64_t() const;

        operator uint64_t() const;

        operator float() const;

        operator double() const;

        operator String() const;

        operator DateTime() const;

        operator TimeSpan() const;

        operator ByteArray() const;

    public:
        static bool changeValue(Type type, const Value &value, Type destType, Value &destValue);

        static bool equals(Type type1, const Value &value1, Type type2, const Value &value2);

        static bool isNullType(Type type);

        static bool isAnalogValue(Type type);

        static bool isDigitalValue(Type type);

        static bool isIntegerValue(Type type);

        static bool isFloatValue(Type type);

        static bool isStringValue(Type type);

        static bool isDateTimeValue(Type type);

        static bool isNullType(const Variant &other);

        static bool isNullValue(const Variant &other);

        static String toTypeScriptStr(Type type);

        static String toTypeStr(Type type);

        static Type fromTypeStr(const String &str);

        static void getAllTypeStr(StringArray &array);

        static double toAnalogValue(Type type, const Value &value);

        static Value fromAnalogValue(Type type, const double &value);

        static size_t valueSize(Type type, const Value &value);

        static String toValueString(Type type, const Value &value);

        static bool parseValueString(const String &str, Type type, Value &value);

    private:
        template<class T>
        bool getValue(T &v) const;

        template<class T>
        bool setValue(const T &v);

        void setValue(const Value &v);

        bool setStringValue(const String &str);

        void setByteArrayValue(const uint8_t *buffer);

        void setValueInner(const Value &value);

        void clearValue();

    private:
        template<class T>
        static bool changeValue(const T &v, Type type, Value &value);

        static bool changeValue(const DateTime &v, Type type, Value &value);

        static bool changeValue(const TimeSpan &v, Type type, Value &value);

        static bool changeValue(const ByteArray &v, Type type, Value &value);

        static bool changeBlobValue(const uint8_t *v, Type type, Value &value);

        template<class T>
        static bool changeValue(Type type, const Value &value, T &v);

        static bool changeValue(const String &v, Type type, Value &value);

        static bool changeValue(Type type, const Value &value, String &v);

        static bool changeValue(Type type, const Value &value, DateTime &v);

        static bool changeValue(Type type, const Value &value, TimeSpan &v);

        static bool changeValue(Type type, const Value &value, ByteArray &v);

        template<class T>
        static bool equals(Type type, const Value &value, const T &v);

        static bool equals(Type type, const Value &value, const String &v);

        static bool equals(Type type, const Value &value, const DateTime &v);

        static bool equals(Type type, const Value &value, const TimeSpan &v);

        static bool equals(Type type, const Value &value, const ByteArray &v);

        static bool setStringValue(const String &str, Type type, Value &value);

        static void setByteArrayValue(const uint8_t *buffer, Type type, Value &value);

        static void setByteArrayValue(const ByteArray &buffer, Type type, Value &value);

        static bool equalsTextValue(const char *value1, const char *value2);

        static bool equalsBlobValue(const uint8_t *value1, const uint8_t *value2);

        static int compareTextValue(const char *value1, const char *value2);

        static int compareBlobValue(const uint8_t *value1, const uint8_t *value2);

        static size_t blobCount(const uint8_t *value);

        static const uint8_t *blobBuffer(const uint8_t *value);

        static void writeValue(Stream *stream, bool bigEndian, Type type, const Value &value);

        static void readValue(Stream *stream, bool bigEndian, Type type, Value &value);

        static void clearValue(Type type, Value &value);

    public:
        static const Variant NullValue;

    private:
        Type _type;
        Value _value;
        bool _isNullValue;

    private:
        static const size_t MaxBlobCount = 10 * 1024 * 1024;
    };
}


#endif // Variant_h
