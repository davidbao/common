//
//  StringMap.cpp
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright (c) 2017 com. All rights reserved.
//

#include "data/StringMap.h"
#include "data/ValueType.h"

namespace Data {
    const StringMap StringMap::Empty;

    StringMap::StringMap(bool ignoreKeyCase) : _ignoreKeyCase(ignoreKeyCase) {
    }

    StringMap::StringMap(const StringMap &other) : Dictionary<String, String>(other),
                                                   _ignoreKeyCase(other._ignoreKeyCase) {
    }

    StringMap::StringMap(StringMap &&other) noexcept: Dictionary<String, String>(std::move(other)),
                                                      _ignoreKeyCase(other._ignoreKeyCase) {
    }

    StringMap::StringMap(std::initializer_list<ValueType> list, bool ignoreKeyCase) : Dictionary<String, String>(list),
                                                                                      _ignoreKeyCase(ignoreKeyCase) {
    }

    StringMap::StringMap(const KeyValue *item, bool ignoreKeyCase) : StringMap(ignoreKeyCase) {
        const KeyValue *kv = item;
        while (kv != nullptr && !kv->key.isNullOrEmpty()) {
            add(kv->key, kv->value);
            kv++;
        }
    }

    void StringMap::add(const String &key, const string &value) {
        add(key, String(value).toString());
    }

    void StringMap::add(const String &key, char *value) {
        add(key, String(value).toString());
    }

    void StringMap::add(const String &key, const char *value) {
        add(key, String(value).toString());
    }

    void StringMap::add(const String &key, const bool &value) {
        add(key, Boolean(value).toString());
    }

    void StringMap::add(const String &key, const int8_t &value) {
        add(key, Int8(value).toString());
    }

    void StringMap::add(const String &key, const uint8_t &value) {
        add(key, UInt8(value).toString());
    }

    void StringMap::add(const String &key, const int16_t &value) {
        add(key, Int16(value).toString());
    }

    void StringMap::add(const String &key, const uint16_t &value) {
        add(key, UInt16(value).toString());
    }

    void StringMap::add(const String &key, const int32_t &value) {
        add(key, Int32(value).toString());
    }

    void StringMap::add(const String &key, const uint32_t &value) {
        add(key, UInt32(value).toString());
    }

    void StringMap::add(const String &key, const int64_t &value) {
        add(key, Int64(value).toString());
    }

    void StringMap::add(const String &key, const uint64_t &value) {
        add(key, UInt64(value).toString());
    }

    void StringMap::add(const String &key, const float &value) {
        add(key, Float(value).toString());
    }

    void StringMap::add(const String &key, const double &value) {
        add(key, Double(value).toString());
    }

    bool StringMap::at(const String &key, string &value) const {
        String str;
        if (at(key, str)) {
            value = str;
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, bool &value) const {
        String str;
        if (at(key, str) && Boolean::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, int8_t &value) const {
        String str;
        if (at(key, str) && Int8::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, uint8_t &value) const {
        String str;
        if (at(key, str) && UInt8::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, int16_t &value) const {
        String str;
        if (at(key, str) && Int16::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, uint16_t &value) const {
        String str;
        if (at(key, str) && UInt16::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, int32_t &value) const {
        String str;
        if (at(key, str) && Int32::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, uint32_t &value) const {
        String str;
        if (at(key, str) && UInt32::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, int64_t &value) const {
        String str;
        if (at(key, str) && Int64::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, uint64_t &value) const {
        String str;
        if (at(key, str) && UInt64::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, float &value) const {
        String str;
        if (at(key, str) && Float::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, double &value) const {
        String str;
        if (at(key, str) && Double::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::set(const String &key, const string &value) {
        return set(key, String(value).toString());
    }

    bool StringMap::set(const String &key, const char *value) {
        return set(key, String(value).toString());
    }

    bool StringMap::set(const String &key, char *value) {
        return set(key, String(value).toString());
    }

    bool StringMap::set(const String &key, const bool &value) {
        return set(key, Boolean(value).toString());
    }

    bool StringMap::set(const String &key, const int8_t &value) {
        return set(key, Int8(value).toString());
    }

    bool StringMap::set(const String &key, const uint8_t &value) {
        return set(key, UInt8(value).toString());
    }

    bool StringMap::set(const String &key, const int16_t &value) {
        return set(key, Int16(value).toString());
    }

    bool StringMap::set(const String &key, const uint16_t &value) {
        return set(key, UInt16(value).toString());
    }

    bool StringMap::set(const String &key, const int32_t &value) {
        return set(key, Int32(value).toString());
    }

    bool StringMap::set(const String &key, const uint32_t &value) {
        return set(key, UInt32(value).toString());
    }

    bool StringMap::set(const String &key, const int64_t &value) {
        return set(key, Int64(value).toString());
    }

    bool StringMap::set(const String &key, const uint64_t &value) {
        return set(key, UInt64(value).toString());
    }

    bool StringMap::set(const String &key, const float &value) {
        return set(key, Float(value).toString());
    }

    bool StringMap::set(const String &key, const double &value) {
        return set(key, Double(value).toString());
    }

    void StringMap::evaluates(const Dictionary &other) {
        Dictionary<String, String>::evaluates(other);
        auto value = dynamic_cast<const StringMap *>(&other);
        _ignoreKeyCase = value->_ignoreKeyCase;
    }

    StringMap &StringMap::operator=(const StringMap &other) {
        evaluates(other);
        return *this;
    }

    void StringMap::keys(StringArray &keys) const {
        Vector<String> texts;
        Dictionary<String, String>::keys(texts);
        for (size_t i = 0; i < texts.count(); i++) {
            const String &k = texts[i];
            keys.add(k);
        }
    }

    void StringMap::values(StringArray &values) const {
        Vector<String> texts;
        Dictionary<String, String>::values(texts);
        for (size_t i = 0; i < texts.count(); i++) {
            const String &v = texts[i];
            values.add(v);
        }
    }
}
