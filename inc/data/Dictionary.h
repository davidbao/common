//
//  Dictionary.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef Dictionary_h
#define Dictionary_h

#include <cstdio>
#include <map>
#include "Array.h"
#include "Vector.h"

using namespace std;

namespace Common {
    template<class TKey, class TValue>
    class Dictionary {
    public:
        Dictionary() {
        }

        ~Dictionary() {
            clear();
        }

        inline void add(const TKey &key, TValue value) {
            if (!containsKey(key))
                _map.insert(make_pair(key, value));
            else
                _map[key] = value;
        }

        inline void addRange(const Dictionary<TKey, TValue> &values) {
            Vector<TKey> keys;
            values.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                const TKey *key = keys[i];
                TValue value;
                if (values.at(*key, value))
                    add(*key, value);
            }
        }

        inline bool containsKey(const TKey &key) const {
            auto result = _map.find(key);
            return result != _map.end();
        }

        inline bool contains(const TKey &key) const {
            return containsKey(key);
        }

        inline void clear() {
            _map.clear();
        }

        inline size_t count() const {
            return _map.size();
        }

        inline bool at(const TKey &key, TValue &value) const {
            auto it = _map.find(key);
            if (it != _map.end()) {
                value = it->second;
                return true;
            } else {
                return false;
            }
//            if (!containsKey(key))
//                return false;
//
//            value = _map.at(key);
//            return true;
        }

        inline TValue operator[](const TKey &key) const {
            TValue value;
            if (this->at(key, value))
                return value;

            return TValue();
        }

        inline bool set(const TKey &key, TValue value) {
            if (!containsKey(key))
                return false;

            _map[key] = value;
            return true;
        }

        inline bool remove(const TKey &key) {
            return _map.erase(key) > 0;
        }

        inline void keys(Array<TKey> &k) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(it->first);
            }
        }

        inline void keys(Vector<TKey> &k) const {
            k.setAutoDelete(false);

            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(&it->first);
            }
        }

        inline void values(Vector<TValue> &v) const {
            v.setAutoDelete(false);

            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                v.add(&it->second);
            }
        }

    private:
        map <TKey, TValue> _map;
    };

    template<class TKey, class TValue>
    class Map {
    public:
        Map(bool autoDeleteValue = false) {
            _autoDeleteValue = autoDeleteValue;
        }

        ~Map() {
            clear();
        }

        inline void add(const TKey &key, TValue *value) {
            if (!containsKey(key))
                _map.insert(make_pair(key, value));
            else
                _map[key] = value;
        }

        inline void addRange(const Dictionary<TKey, TValue> &values) {
            Vector<TKey> keys;
            values.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                const TKey *key = keys[i];
                TValue value;
                if (values.at(*key, value))
                    add(*key, value);
            }
        }

        inline bool containsKey(const TKey &key) const {
            auto result = _map.find(key);
            return result != _map.end();
        }

        inline bool contains(const TKey &key) const {
            return containsKey(key);
        }

        inline void clear(bool autoDeleteValue = true) {
            if (autoDeleteValue && _autoDeleteValue) {
                auto it = _map.begin();
                for (; it != _map.end(); ++it) {
                    delete it->second;
                }
            }

            _map.clear();
        }

        inline size_t count() const {
            return _map.size();
        }

        inline bool at(const TKey &key, TValue *&value) const {
            auto it = _map.find(key);
            if (it != _map.end()) {
                value = it->second;
                return true;
            } else {
                return false;
            }
//            if (!containsKey(key))
//                return false;
//
//            value = _map.at(key);
//            return true;
        }

        inline TValue *operator[](const TKey &key) const {
            TValue *value;
            if (this->at(key, value))
                return value;

            return nullptr;
        }

        inline bool set(const TKey &key, TValue *value) {
            if (!containsKey(key))
                return false;

            _map[key] = value;
            return true;
        }

        inline bool remove(const TKey &key) {
            return _map.erase(key) > 0;
        }

        inline void keys(Array<TKey> &k) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(it->first);
            }
        }

        inline void keys(Vector<TKey> &k) const {
            k.setAutoDelete(false);

            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(&it->first);
            }
        }

        inline void values(Vector<TValue> &v) const {
            v.setAutoDelete(false);

            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                v.add(it->second);
            }
        }

    private:
        map<TKey, TValue *> _map;
        bool _autoDeleteValue;
    };
}

#endif // Dictionary_h
