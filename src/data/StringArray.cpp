#include <stdarg.h>
#include "data/StringArray.h"
#include "system/Convert.h"
#include "data/ValueType.h"
#include "IO/Stream.h"
#include "exception/Exception.h"

namespace Common {
    const StringArray StringArray::Empty;

    StringArray::StringArray(bool autoDelete, uint capacity) : _strings(autoDelete, capacity) {
    }

    StringArray::StringArray(const char *item, ...) : StringArray() {
        const char *str = item;
        va_list ap;
        va_start(ap, item);
        while (str != nullptr && str[0] != '\0') {
            if (str != nullptr) {
                add(str);
            }
            str = va_arg(ap, const char*);
        }
        va_end(ap);
    }

    void StringArray::addArray(const char *item, ...) {
        const char *str = item;
        va_list ap;
        va_start(ap, item);
        while (str != nullptr && str[0] != '\0') {
            if (str != nullptr) {
                add(str);
            }
            str = va_arg(ap, const char*);
        }
        va_end(ap);
    }

    StringArray::StringArray(const StringArray &value) {
        this->operator=(value);
    }

    void StringArray::add(const String &str) {
        _strings.add(new String(str));
    }

    const String &StringArray::at(size_t pos) const {
        String *str = _strings.at(pos);
        return *str;
    }

    String &StringArray::at(size_t pos) {
        String *str = _strings.at(pos);
        return *str;
    }

    void StringArray::addRange(const String *strings, size_t count) {
        for (size_t i = 0; i < count; i++) {
            add(strings[i]);
        }
    }

    void StringArray::addRange(const StringArray *strings) {
        for (size_t i = 0; i < strings->count(); i++) {
            add(strings->at(i));
        }
    }

    bool StringArray::insert(size_t i, const String &str) {
        String *item = new String(str);
        bool result = _strings.insert(i, item);
        if (!result) {
            delete item;
        }
        return result;
    }

    bool StringArray::set(size_t i, const String &str) {
        String *item = new String(str);
        bool result = _strings.set(i, item);
        if (!result) {
            delete item;
        }
        return result;
    }

    bool StringArray::contains(const String &str, bool ignoreCase) const {
        String **strings = _strings.data();
        for (size_t i = 0; i < count(); i++) {
            if (String::equals(*strings[i], str, ignoreCase)) {
                return true;
            }
        }
        return false;
    }

    void StringArray::remove(const String &str, bool ignoreCase) {
        String **strings = _strings.data();
        for (ssize_t i = count() - 1; i >= 0; i--) {
            if (String::equals(*strings[i], str, ignoreCase)) {
                _strings.removeAt(i);
            }
        }
    }

    void StringArray::clear() {
        _strings.clear();
    }

    size_t StringArray::count() const {
        return _strings.count();
    }

    void StringArray::write(Stream *stream, int countLength, int strLength) const {
        int cl = countLength;
        if (!(cl == 0 || cl == 1 || cl == 2 || cl == 4)) {
            throw ArgumentException("countLength must be equal to 0, 1, 2, 4.");
        }

        size_t c = count();
        switch (cl) {
            case 0:
                break;
            case 1:
                stream->writeByte((uint8_t) c);
                break;
            case 2:
                stream->writeUInt16((ushort) c);
                break;
            case 4:
                stream->writeUInt32((uint32_t) c);
                break;
            default:
                break;
        }
        for (size_t i = 0; i < c; i++) {
            String value = this->at(i);
            stream->writeStr(value, strLength);
        }
    }

    void StringArray::read(Stream *stream, int countLength, int strLength) {
        clear();

        int cl = countLength;
        if (!(cl == 0 || cl == 1 || cl == 2 || cl == 4)) {
            throw ArgumentException("countLength must be equal to 0, 1, 2, 4.");
        }

        size_t c = count();
        switch (cl) {
            case 0:
                break;
            case 1:
                c = stream->readByte();
                break;
            case 2:
                c = stream->readUInt16();
                break;
            case 4:
                c = stream->readUInt32();
                break;
            default:
                break;
        }
        for (uint i = 0; i < c; i++) {
            String value = stream->readStr(strLength);
            add(value);
        }
    }

    void StringArray::copyFrom(const StringArray *values, bool append) {
        if (!append) {
            clear();
        }

        for (uint i = 0; i < values->count(); i++) {
            add(values->at(i));
        }
    }

    const String StringArray::toString(const char symbol) const {
        String str;
        for (uint i = 0; i < count(); i++) {
            if (str.length() > 0)
                str.append(symbol);
            str.append(at(i));
        }
        return str;
    }

    bool StringArray::parse(const String &str, StringArray &texts, const char splitSymbol) {
        Convert::splitStr(str, texts, splitSymbol);
        return true;
    }

    bool StringArray::parseMultiSymbol(const String &str, StringArray &texts, const char splitSymbol1,
                                       const char splitSymbol2, const char splitSymbol3) {
        if (splitSymbol1 != '\0' && str.find(splitSymbol1) >= 0)
            StringArray::parse(str, texts, splitSymbol1);
        else if (splitSymbol2 != '\0' && str.find(splitSymbol2) >= 0)
            StringArray::parse(str, texts, splitSymbol2);
        else if (splitSymbol3 != '\0' && str.find(splitSymbol3) >= 0)
            StringArray::parse(str, texts, splitSymbol3);
        return StringArray::parse(str, texts, splitSymbol1);
    }

    void StringArray::operator=(const StringArray &array) {
        clear();
        addRange(&array);
    }

    void StringArray::operator=(const String &str) {
        clear();
        parse(str, *this);
    }

    void StringArray::operator=(const char *str) {
        if (str != nullptr) {
            this->operator=((String) str);
        }
    }

    bool StringArray::operator==(const StringArray &value) const {
        if (count() != value.count())
            return false;

        for (uint i = 0; i < count(); i++) {
            if (at(i) != value.at(i))
                return false;
        }
        return true;
    }

    bool StringArray::operator!=(const StringArray &value) const {
        return !operator==(value);
    }

    const StringMap StringMap::Empty;

    StringMap::StringMap(bool ignoreKeyCase) : _ignoreKeyCase(ignoreKeyCase) {
    }

    StringMap::StringMap(const KeyValue *item, bool ignoreKeyCase) : StringMap(ignoreKeyCase) {
        const KeyValue *kv = item;
        while (kv != nullptr && !kv->key.isNullOrEmpty()) {
            if (kv != nullptr) {
                add(kv->key, kv->value);
            }
            kv++;
        }
    }

    StringMap::StringMap(const StringMap &value) {
        _ignoreKeyCase = value._ignoreKeyCase;
        addRange(value);
    }

    void StringMap::add(const String &key, const String &value) {
        _map.add(_ignoreKeyCase ? key.toLower() : key, value);
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

    void StringMap::add(const String &key, const uint8_t &value) {
        add(key, Byte(value).toString());
    }

    void StringMap::add(const String &key, const int &value) {
        add(key, Int32(value).toString());
    }

    void StringMap::add(const String &key, const uint &value) {
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

    void StringMap::add(const String &key, const DateTime &value) {
        add(key, value.toString());
    }

    void StringMap::add(const String &key, const TimeSpan &value) {
        add(key, value.toString());
    }

    void StringMap::add(const String &key, const Version &value) {
        add(key, value.toString());
    }

    void StringMap::add(const String &key, const Uuid &value) {
        add(key, value.toString());
    }

    void StringMap::addRange(const StringMap &values) {
        _map.addRange(values._map);
    }

    bool StringMap::containsKey(const String &key) const {
        return _map.containsKey(_ignoreKeyCase ? key.toLower() : key);
    }

    bool StringMap::contains(const String &key) const {
        return _map.contains(_ignoreKeyCase ? key.toLower() : key);
    }

    void StringMap::clear() {
        _map.clear();
    }

    size_t StringMap::count() const {
        return _map.count();
    }

    bool StringMap::at(const String &key, String &value) const {
        return _map.at(_ignoreKeyCase ? key.toLower() : key, value);
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

    bool StringMap::at(const String &key, uint8_t &value) const {
        String str;
        if (at(key, str) && Byte::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, short &value) const {
        String str;
        if (at(key, str) && Int16::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, ushort &value) const {
        String str;
        if (at(key, str) && UInt16::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, int &value) const {
        String str;
        if (at(key, str) && Int32::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, uint &value) const {
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

    bool StringMap::at(const String &key, DateTime &value) const {
        String str;
        if (at(key, str) && DateTime::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, TimeSpan &value) const {
        String str;
        if (at(key, str) && TimeSpan::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, Version &value) const {
        String str;
        if (at(key, str) && Version::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::at(const String &key, Uuid &value) const {
        String str;
        if (at(key, str) && Uuid::parse(str, value)) {
            return true;
        }
        return false;
    }

    bool StringMap::StringMap::set(const String &key, const String &value) {
        return _map.set(_ignoreKeyCase ? key.toLower() : key, value);
    }

    bool StringMap::set(const String &key, const string &value) {
        return set(key, String(value).toString());
    }

    bool StringMap::set(const String &key, const char *value) {
        return set(key, String(value).toString());
    }

    bool StringMap::set(const String &key, const bool &value) {
        return set(key, Boolean(value).toString());
    }

    bool StringMap::set(const String &key, const uint8_t &value) {
        return set(key, Byte(value).toString());
    }

    bool StringMap::set(const String &key, const int &value) {
        return set(key, Int32(value).toString());
    }

    bool StringMap::set(const String &key, const uint &value) {
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

    bool StringMap::set(const String &key, const DateTime &value) {
        return set(key, value.toString());
    }

    bool StringMap::set(const String &key, const TimeSpan &value) {
        return set(key, value.toString());
    }

    bool StringMap::set(const String &key, const Version &value) {
        return set(key, value.toString());
    }

    bool StringMap::set(const String &key, const Uuid &value) {
        return set(key, value.toString());
    }

    bool StringMap::remove(const String &key) {
        return _map.remove(_ignoreKeyCase ? key.toLower() : key);
    }

    String StringMap::operator[](const String &key) const {
        return _map[_ignoreKeyCase ? key.toLower() : key];
    }

    void StringMap::operator=(const StringMap &value) {
        clear();

        Vector<String> keys;
        value._map.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String *k = keys[i];
            String v;
            if (value.at(*k, v))
                add(*k, v);
        }
    }

    bool StringMap::operator==(const StringMap &value) const {
        if (count() != value.count())
            return false;

        Vector<String> keys;
        value._map.keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String *k = keys[i];
            String v1, v2;
            if (value.at(*k, v1) && this->at(*k, v2)) {
                if (v1 != v2)
                    return false;
            }
        }
        return true;
    }

    bool StringMap::operator!=(const StringMap &value) const {
        return !operator==(value);
    }

    void StringMap::keys(StringArray &keys) const {
        Vector<String> texts;
        _map.keys(texts);
        for (size_t i = 0; i < texts.count(); i++) {
            const String *k = texts[i];
            keys.add(*k);
        }
    }

    void StringMap::values(StringArray &values) const {
        Vector<String> texts;
        _map.values(texts);
        for (size_t i = 0; i < texts.count(); i++) {
            const String *v = texts[i];
            values.add(*v);
        }
    }

    void StringMap::keyValues(KeyValues &kvs) const {
        StringArray keys;
        this->keys(keys);
        for (size_t i = 0; i < keys.count(); i++) {
            const String &key = keys[i];
            String value;
            if (at(key, value)) {
                KeyValue *kv = new KeyValue{key, value};
                kvs.add(kv);
            }
        }
    }
}
