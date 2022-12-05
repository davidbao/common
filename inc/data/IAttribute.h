//
//  IAttribute.h
//  common
//
//  Created by baowei on 2022/12/4.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef IAttribute_h
#define IAttribute_h

#include "data/String.h"
#include "data/ValueType.h"
#include "data/DateTime.h"
#include "data/TimeZone.h"

namespace Common {
    class IAttributeGetter {
    public:
        virtual ~IAttributeGetter() = default;

        virtual bool getAttribute(const String &name, String &value) const = 0;

        String getAttribute(const String &name) const;

        bool getAttribute(const String &name, bool &value) const;

        bool getAttribute(const String &name, char &value,
                          char minValue = Char::MinValue, char maxValue = Char::MaxValue) const;

        bool getAttribute(const String &name, int8_t &value,
                          int8_t minValue = Int8::MinValue, int8_t maxValue = Int8::MaxValue) const;

        bool getAttribute(const String &name, uint8_t &value,
                          uint8_t minValue = UInt8::MinValue, uint8_t maxValue = UInt8::MaxValue) const;

        bool getAttribute(const String &name, int16_t &value,
                          int16_t minValue = Int16::MinValue, int16_t maxValue = Int16::MaxValue) const;

        bool getAttribute(const String &name, uint16_t &value,
                          uint16_t minValue = UInt16::MinValue, uint16_t maxValue = UInt16::MaxValue) const;

        bool getAttribute(const String &name, int32_t &value,
                          int32_t minValue = Int32::MinValue, int maxValue = Int32::MaxValue) const;

        bool getAttribute(const String &name, uint32_t &value,
                          uint32_t minValue = UInt32::MinValue, uint32_t maxValue = UInt32::MaxValue) const;

        bool getAttribute(const String &name, int64_t &value,
                          int64_t minValue = Int64::MinValue, int64_t maxValue = Int64::MaxValue) const;

        bool getAttribute(const String &name, uint64_t &value,
                          uint64_t minValue = UInt64::MinValue, uint64_t maxValue = UInt64::MaxValue) const;

        bool getAttribute(const String &name, float &value,
                          float minValue = Float::MinValue, float maxValue = Float::MaxValue) const;

        bool getAttribute(const String &name, double &value,
                          double minValue = Double::MinValue, double maxValue = Double::MaxValue) const;

        bool getAttribute(const String &name, DateTime &value,
                          const DateTime &minValue = DateTime::MinValue,
                          const DateTime &maxValue = DateTime::MaxValue) const;

        bool getAttribute(const String &name, TimeSpan &value,
                          const TimeSpan &minValue = TimeSpan::MinValue,
                          const TimeSpan &maxValue = TimeSpan::MaxValue) const;

        template<typename T>
        bool getAttribute(const String &name, T &value) const {
            return T::parse(getAttribute(name), value);
        }

        template<typename T>
        bool getAttribute(const String &name, T &value, const T &minValue, const T &maxValue) const {
            return getAttribute(name, value) && (value >= minValue && value <= maxValue);
        }
    };
}
#endif // IAttribute_h
