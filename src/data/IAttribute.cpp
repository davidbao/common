//
//  IAttribute.h
//  common
//
//  Created by baowei on 2022/12/4.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/IAttribute.h"

namespace Common {
    String IAttributeGetter::getAttribute(const String &name) const {
        String value;
        getAttribute(name, value);
        return value;
    }

    bool IAttributeGetter::getAttribute(const String &name, bool &value) const {
        return Boolean::parse(getAttribute(name), value);
    }

    bool IAttributeGetter::getAttribute(const String &name, char &value, char minValue, char maxValue) const {
        char temp;
        if (Char::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, int8_t &value, int8_t minValue, int8_t maxValue) const {
        int8_t temp;
        if (Int8::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, uint8_t &value, uint8_t minValue, uint8_t maxValue) const {
        uint8_t temp;
        if (UInt8::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, int16_t &value, int16_t minValue, int16_t maxValue) const {
        int16_t temp;
        if (Int16::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, uint16_t &value, uint16_t minValue, uint16_t maxValue) const {
        uint16_t temp;
        if (UInt16::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, int &value, int minValue, int maxValue) const {
        int32_t temp;
        if (Int32::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, uint32_t &value, uint32_t minValue, uint32_t maxValue) const {
        uint32_t temp;
        if (UInt32::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, int64_t &value, int64_t minValue, int64_t maxValue) const {
        int64_t temp;
        if (Int64::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, uint64_t &value, uint64_t minValue, uint64_t maxValue) const {
        uint64_t temp;
        if (UInt64::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, float &value, float minValue, float maxValue) const {
        float temp;
        if (Float::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, double &value, double minValue, double maxValue) const {
        double temp;
        if (Double::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue)) {
            value = temp;
            return true;
        }
        return false;
    }

    bool IAttributeGetter::getAttribute(const String &name, DateTime &value, const DateTime &minValue,
                                     const DateTime &maxValue) const {
        return DateTime::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }

    bool IAttributeGetter::getAttribute(const String &name, TimeSpan &value, const TimeSpan &minValue,
                                     const TimeSpan &maxValue) const {
        TimeSpan temp;
        if (TimeSpan::parse(getAttribute(name), temp) && temp >= minValue && temp <= maxValue) {
            value = temp;
            return true;
        }
        return false;
    }
}