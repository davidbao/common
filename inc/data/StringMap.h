//
//  StringMap.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright (c) 2017 com. All rights reserved.
//

#ifndef StringMap_h
#define StringMap_h

#include "data/Dictionary.h"
#include "data/String.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Uuid.h"
#include "data/StringArray.h"

namespace Data {
    class StringMap : public IEquatable<StringMap>,
                      public IEvaluation<StringMap>,
                      public IComparable<StringMap>,
                      public PairIterator<String, String>,
                      public IMutex {
    public:
        class KeyValue {
        public:
            const String key;
            const String value;
        };

        typedef Vector<KeyValue> KeyValues;
        typedef pair<const String, String> ValueType;
        typedef ValueType &Reference;
        typedef const ValueType &ConstReference;

        explicit StringMap(bool ignoreKeyCase = false);

        StringMap(const StringMap &other);

        StringMap(StringMap &&other) noexcept;

        StringMap(std::initializer_list<ValueType> list, bool ignoreKeyCase = false);

        bool equals(const StringMap &other) const override;

        void evaluates(const StringMap &other) override;

        int compareTo(const StringMap &other) const override;

        StringMap &operator=(const StringMap &other);

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
            if (_ignoreKeyCase) {
                _map.add(key.toLower(), value.toString());
            } else {
                _map.add(key, value.toString());
            }
        }

        void addRange(const StringMap &other);

        bool contains(const String &key) const;

        bool contains(const String &key, const String &value) const;

        void clear();

        size_t count() const;

        bool isEmpty() const;

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

        String &at(const String &key);

        const String &at(const String &key) const;

        bool at(const String &key, String &value) const;

        template<class T>
        bool at(const String &key, T &value) const {
            String str;
            if (at(key, str)) {
                return T::parse(str, value);
            }
            return false;
        }

        const String &operator[](const String &key) const;

        String &operator[](const String &key);

        bool remove(const String &key);

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
            if (_ignoreKeyCase) {
                return _map.set(key.toLower(), value);
            } else {
                return _map.set(key, value);
            }
        }

        void keys(StringArray &k) const;

        void values(StringArray &v) const;

        typename PairIterator<String, String>::const_iterator begin() const override {
            return typename PairIterator<String, String>::const_iterator(_map.begin());
        }

        typename PairIterator<String, String>::const_iterator end() const override {
            return typename PairIterator<String, String>::const_iterator(_map.end());
        }

        typename PairIterator<String, String>::iterator begin() override {
            return typename PairIterator<String, String>::iterator(_map.begin());
        }

        typename PairIterator<String, String>::iterator end() override {
            return typename PairIterator<String, String>::iterator(_map.end());
        }

        typename PairIterator<String, String>::const_reverse_iterator rbegin() const override {
            return typename PairIterator<String, String>::const_reverse_iterator(_map.rbegin());
        }

        typename PairIterator<String, String>::const_reverse_iterator rend() const override {
            return typename PairIterator<String, String>::const_reverse_iterator(_map.rend());
        }

        typename PairIterator<String, String>::reverse_iterator rbegin() override {
            return typename PairIterator<String, String>::reverse_iterator(_map.rbegin());
        }

        typename PairIterator<String, String>::reverse_iterator rend() override {
            return typename PairIterator<String, String>::reverse_iterator(_map.rend());
        }

        void lock() override {
            _map.lock();
        }

        bool tryLock() override {
            return _map.tryLock();
        }

        void unlock() override {
            _map.unlock();
        }

    public:
        static const StringMap Empty;

    private:
        Dictionary<String, String> _map;

        bool _ignoreKeyCase;
    };
}

#endif    // StringMap_h
