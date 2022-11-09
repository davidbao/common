//
//  StringMap.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef StringMap_h
#define StringMap_h

#include "data/Dictionary.h"
#include "data/ValueType.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Uuid.h"
#include "data/StringArray.h"

namespace Common {
    class StringMap : public Dictionary<String, String> {
    public:
        class KeyValue {
        public:
            const String key;
            const String value;
        };

        typedef Vector<KeyValue> KeyValues;

        explicit StringMap(bool ignoreKeyCase = false);

        StringMap(const StringMap &other);

        StringMap(StringMap &&other) noexcept;

        StringMap(std::initializer_list<ValueType> list, bool ignoreKeyCase = false);

        COMMON_ATTR_DEPRECATED("use StringMap(std::initializer_list<ValueType> list, bool ignoreKeyCase = false)")
        StringMap(const KeyValue *item, bool ignoreKeyCase = false);

        void add(const String &key, const String &value);

        void add(const String &key, const string &value);

        void add(const String &key, const char *value);

        void add(const String &key, char *value);

        void add(const String &key, const bool &value);

        void add(const String &key, const int8_t &value);

        void add(const String &key, const uint8_t &value);

        void add(const String &key, const int16_t &value);

        void add(const String &key, const uint16_t &value);

        void add(const String &key, const int32_t &value);

        void add(const String &key, const uint32_t &value);

        void add(const String &key, const int64_t &value);

        void add(const String &key, const uint64_t &value);

        void add(const String &key, const float &value);

        void add(const String &key, const double &value);

        template<class T>
        void add(const String &key, const T &value) {
            add(key, value.toString());
        }

        bool at(const String &key, String &value) const;

        bool at(const String &key, string &value) const;

        bool at(const String &key, bool &value) const;

        bool at(const String &key, int8_t &value) const;

        bool at(const String &key, uint8_t &value) const;

        bool at(const String &key, int16_t &value) const;

        bool at(const String &key, uint16_t &value) const;

        bool at(const String &key, int32_t &value) const;

        bool at(const String &key, uint32_t &value) const;

        bool at(const String &key, int64_t &value) const;

        bool at(const String &key, uint64_t &value) const;

        bool at(const String &key, float &value) const;

        bool at(const String &key, double &value) const;

        template<class T>
        bool at(const String &key, T &value) const {
            String str;
            if (at(key, str)) {
                return T::parse(str, value);
            }
            return false;
        }

        String &at(const String &key);

        const String &at(const String &key) const;

        bool set(const String &key, const String &value);

        bool set(const String &key, const string &value);

        bool set(const String &key, const char *value);

        bool set(const String &key, char *value);

        bool set(const String &key, const bool &value);

        bool set(const String &key, const int8_t &value);

        bool set(const String &key, const uint8_t &value);

        bool set(const String &key, const int16_t &value);

        bool set(const String &key, const uint16_t &value);

        bool set(const String &key, const int32_t &value);

        bool set(const String &key, const uint32_t &value);

        bool set(const String &key, const int64_t &value);

        bool set(const String &key, const uint64_t &value);

        bool set(const String &key, const float &value);

        bool set(const String &key, const double &value);

        template<class T>
        bool set(const String &key, const T &value) {
            return set(key, value.toString());
        }

        void evaluates(const Dictionary &other) override;

        StringMap &operator=(const StringMap &other);

        void keys(StringArray &keys) const;

        void values(StringArray &values) const;

    public:
        static const StringMap Empty;

    private:
        bool _ignoreKeyCase;
    };
}

#endif    // StringMap_h
