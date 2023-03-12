//
//  Property.hpp
//  common
//
//  Created by baowei on 16/8/24.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Property_hpp
#define Property_hpp

#include "data/String.h"
#include "data/TimeSpan.h"
#include "data/DateTime.h"
#include "data/PList.h"
#include "system/ServiceFactory.h"
#include "system/Delegate.h"

namespace System {
    class Property {
    public:
        enum Type : uint8_t {
            Null = 0,
            Digital,        // bValue
            Integer8,       // cValue
            Integer16,      // sValue
            Integer32,      // nValue
            Integer64,      // lValue
            Float32,        // fValue
            Float64,        // dValue
            Text,           // strValue
            Date,           // tValue
            TimeRange,      // tValue
            Custom,         // objValue
        };

        class Value {
        public:
            Value();

            virtual ~Value();

            virtual void write(Stream *stream, bool bigEndian = true) const = 0;

            virtual void read(Stream *stream, bool bigEndian = true) = 0;

            virtual void copyFrom(const Value *value) = 0;

            virtual Value *clone() const = 0;

            virtual bool equals(const Value *value) = 0;

            virtual const String toString() const = 0;
        };

        union FixedValue {
            bool bValue;
            uint8_t cValue;
            short sValue;
            int nValue;
            int64_t lValue;
            float fValue;
            double dValue;
            char *strValue;
            uint64_t tValue;    // 100-nanosecond ticks
            Value *objValue;
        };

        Property();

        Property(const Property &property);

        Property(uint32_t ownerId, const String &name, bool value);

        Property(uint32_t ownerId, const String &name, char value);

        Property(uint32_t ownerId, const String &name, uint8_t value);

        Property(uint32_t ownerId, const String &name, short value);

        Property(uint32_t ownerId, const String &name, uint16_t value);

        Property(uint32_t ownerId, const String &name, int value);

        Property(uint32_t ownerId, const String &name, uint32_t value);

        Property(uint32_t ownerId, const String &name, int64_t value);

        Property(uint32_t ownerId, const String &name, uint64_t value);

        Property(uint32_t ownerId, const String &name, float value);

        Property(uint32_t ownerId, const String &name, double value);

        Property(uint32_t ownerId, const String &name, const String &value);

        Property(uint32_t ownerId, const String &name, const DateTime &value);

        Property(uint32_t ownerId, const String &name, const TimeSpan &value);

        Property(uint32_t ownerId, const String &name, const Value &value);

        ~Property();

        void operator=(const Property &property);

        bool operator==(const Property &property) const;

        bool operator!=(const Property &property) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        void copyFrom(const Property *property);

        Property *clone() const;

        const String toString() const;

        uint32_t ownerId;
        String name;
        Type type;
        FixedValue value;

    private:
        Property(uint32_t ownerId, const String &name, Type type);

        Property(uint32_t ownerId, const String &name, Type type, const FixedValue &value);

        void setValue(Type type, const FixedValue &value);

    private:
        static void setStringValue(const String &str, Type type, FixedValue &value);

        static String toTypeStr(Type type);

        static Type fromTypeStr(const String &str);

        static bool toString(Type type, const FixedValue &value, String &v);

    public:
        static Property Empty;
    };

    class IPropertyChanged {
    public:
        IPropertyChanged();

        virtual ~IPropertyChanged();

        Delegates *propertyChangedDelegates();

    protected:
        void propertyChanged(const Property &property);

        void propertyChanged(uint32_t ownerId, const String &propName, bool value);

        void propertyChanged(uint32_t ownerId, const String &propName, char value);

        void propertyChanged(uint32_t ownerId, const String &propName, uint8_t value);

        void propertyChanged(uint32_t ownerId, const String &propName, short value);

        void propertyChanged(uint32_t ownerId, const String &propName, uint16_t value);

        void propertyChanged(uint32_t ownerId, const String &propName, int value);

        void propertyChanged(uint32_t ownerId, const String &propName, uint32_t value);

        void propertyChanged(uint32_t ownerId, const String &propName, int64_t value);

        void propertyChanged(uint32_t ownerId, const String &propName, uint64_t value);

        void propertyChanged(uint32_t ownerId, const String &propName, float value);

        void propertyChanged(uint32_t ownerId, const String &propName, double value);

        void propertyChanged(uint32_t ownerId, const String &propName, const String &value);

        void propertyChanged(uint32_t ownerId, const String &propName, const DateTime &value);

        void propertyChanged(uint32_t ownerId, const String &propName, const TimeSpan &value);

        void propertyChanged(uint32_t ownerId, const String &propName, const Property::Value &value);

    protected:
        Delegates _propertyChangedDelegates;
    };

    typedef PList<IPropertyChanged> IPropertyChangeds;

    class IPropertyChangedVisitor : public IService {
    public:
        virtual bool getPropertyChangeds(const String &expression, IPropertyChangeds &propChangeds) = 0;
    };

    class PropertyChangedEventArgs : public EventArgs {
    public:
        PropertyChangedEventArgs(const Property &property) {
            this->property = property;
        }

    public:
        Property property;
    };

    typedef void (*PropertyChangedEventHandler)(void *, void *, PropertyChangedEventArgs *);
}

#endif /* Property_hpp */
