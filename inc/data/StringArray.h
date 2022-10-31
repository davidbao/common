//
//  StringArray.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef StringArray_h
#define StringArray_h

#include "data/Vector.h"
#include "data/ValueType.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Dictionary.h"
#include "data/Uuid.h"

namespace Common {
    class Stream;

    class StringArray : public IIndexable<const String &, String> {
    public:
        StringArray(bool autoDelete = true, uint capacity = 256);

        StringArray(const char *item, ...);

        StringArray(const StringArray &value);

        const String &at(size_t pos) const override;

        String &at(size_t pos) override;

        bool set(size_t pos, const String &str) override;

        void add(const String &str);

        void addArray(const char *item, ...);

        void addRange(const String *strings, size_t count);

        void addRange(const StringArray *strings);

        bool insert(size_t i, const String &str);

        bool contains(const String &str, bool ignoreCase = false) const;

        void remove(const String &str, bool ignoreCase = false);

        void clear();

        size_t count() const;

        void write(Stream *stream, int countLength = 2, int strLength = 2) const;

        void read(Stream *stream, int countLength = 2, int strLength = 2);

        void copyFrom(const StringArray *values, bool append = false);

        const String toString(const char symbol = ';') const;

        void operator=(const StringArray &value);

        void operator=(const String &str);

        void operator=(const char *str);

        bool operator==(const StringArray &value) const;

        bool operator!=(const StringArray &value) const;

    public:
        static bool parse(const String &str, StringArray &texts, const char splitSymbol = ';');

        static bool parseMultiSymbol(const String &str, StringArray &texts, const char splitSymbol1 = ';',
                                     const char splitSymbol2 = '\0', const char splitSymbol3 = '\0');

    public:
        static const StringArray Empty;

    private:
        Vector<String> _strings;
    };

    class StringMap {
    public:
        class KeyValue {
        public:
            const String key;
            const String value;
        };

        typedef Vector<KeyValue> KeyValues;

        StringMap(bool ignoreKeyCase = false);

        StringMap(const KeyValue *item, bool ignoreKeyCase = false);

        StringMap(const StringMap &value);

        void add(const String &key, const String &value);

        void add(const String &key, const string &value);

        void add(const String &key, const char *value);

        void add(const String &key, char *value);

        void add(const String &key, const bool &value);

        void add(const String &key, const uint8_t &value);

        void add(const String &key, const int &value);

        void add(const String &key, const uint &value);

        void add(const String &key, const int64_t &value);

        void add(const String &key, const uint64_t &value);

        void add(const String &key, const float &value);

        void add(const String &key, const double &value);

        void add(const String &key, const DateTime &value);

        void add(const String &key, const TimeSpan &value);

        void add(const String &key, const Version &value);

        void add(const String &key, const Uuid &value);

        template<class T>
        void add(const String &key, const T &value) {
            add(key, value.toString());
        }

        void addRange(const StringMap &values);

        bool containsKey(const String &key) const;

        bool contains(const String &key) const;

        void clear();

        size_t count() const;

        bool at(const String &key, String &value) const;

        bool at(const String &key, string &value) const;

        bool at(const String &key, bool &value) const;

        bool at(const String &key, uint8_t &value) const;

        bool at(const String &key, short &value) const;

        bool at(const String &key, ushort &value) const;

        bool at(const String &key, int &value) const;

        bool at(const String &key, uint &value) const;

        bool at(const String &key, int64_t &value) const;

        bool at(const String &key, uint64_t &value) const;

        bool at(const String &key, float &value) const;

        bool at(const String &key, double &value) const;

        bool at(const String &key, DateTime &value) const;

        bool at(const String &key, TimeSpan &value) const;

        bool at(const String &key, Version &value) const;

        bool at(const String &key, Uuid &value) const;

        template<class T>
        bool at(const String &key, T &value) const {
            String str;
            if (at(key, str)) {
                return T::parse(str, value);
            }
            return false;
        }

        bool set(const String &key, const String &value);

        bool set(const String &key, const string &value);

        bool set(const String &key, const char *value);

        bool set(const String &key, const bool &value);

        bool set(const String &key, const uint8_t &value);

        bool set(const String &key, const int &value);

        bool set(const String &key, const uint &value);

        bool set(const String &key, const int64_t &value);

        bool set(const String &key, const uint64_t &value);

        bool set(const String &key, const float &value);

        bool set(const String &key, const double &value);

        bool set(const String &key, const DateTime &value);

        bool set(const String &key, const TimeSpan &value);

        bool set(const String &key, const Version &value);

        bool set(const String &key, const Uuid &value);

        bool remove(const String &key);

        String operator[](const String &key) const;

        void operator=(const StringMap &value);

        bool operator==(const StringMap &value) const;

        bool operator!=(const StringMap &value) const;

        void keys(StringArray &keys) const;

        void values(StringArray &values) const;

        void keyValues(KeyValues &kvs) const;

    public:
        static const StringMap Empty;

    private:
        bool _ignoreKeyCase;

        Dictionary<String, String> _map;
    };
}

#endif    // StringArray_h
