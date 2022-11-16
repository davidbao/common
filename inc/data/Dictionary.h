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
#include "data/Vector.h"
#include "data/IEnumerable.h"
#include "data/PrimitiveInterface.h"

namespace Common {
    template<typename TKey, typename TValue>
    class KeyValuePair
            : public IEquatable<KeyValuePair<TKey, TValue>>,
              public IEvaluation<KeyValuePair<TKey, TValue>>,
              public IComparable<KeyValuePair<TKey, TValue>> {
    public:
        TKey key;
        TValue value;

        KeyValuePair() = default;

        KeyValuePair(const TKey &key, const TValue &value) : key(key), value(value) {
        }

        KeyValuePair(const KeyValuePair &pair) : key(pair.key), value(pair.value) {
        }

        bool equals(const KeyValuePair &other) const override {
            return key == other.key && value == other.value;
        }

        void evaluates(const KeyValuePair &other) override {
            key = other.key;
            value = other.value;
        }

        int compareTo(const KeyValuePair &other) const override {
            if (key != other.key) {
                if (key > other.key) {
                    return 1;
                }
                return -1;
            }
            if (value != other.value) {
                if (value > other.value) {
                    return 1;
                }
                return -1;
            }
            return 0;
        }

        KeyValuePair &operator=(const KeyValuePair &other) {
            evaluates(other);
            return *this;
        }
    };

    template<typename TKey, typename TValue>
    class Dictionary : public IEquatable<Dictionary<TKey, TValue>>,
                       public IEvaluation<Dictionary<TKey, TValue>>,
                       public IComparable<Dictionary<TKey, TValue>>,
                       public PairIterator<TKey, TValue>,
                       public IMutex {
    public:
        typedef pair<const TKey, TValue> ValueType;
        typedef ValueType &Reference;
        typedef const ValueType &ConstReference;

        explicit Dictionary() = default;

        Dictionary(const Dictionary &other) : _map(other._map) {
        }

        Dictionary(Dictionary &&other) noexcept : _map(std::move(other._map)) {
        }

        Dictionary(std::initializer_list<ValueType> list) : _map(list) {
        }

        ~Dictionary() override {
            clear();
        }

        inline bool equals(const Dictionary &other) const override {
            return _map == other._map;
        }

        inline void evaluates(const Dictionary &other) override {
            _map = other._map;
        }

        inline int compareTo(const Dictionary &other) const override {
            if (_map != other._map) {
                if (_map > other._map) {
                    return 1;
                }
                return -1;
            }
            return 0;
        }

        inline Dictionary &operator=(const Dictionary &other) {
            evaluates(other);
            return *this;
        }

        inline void add(const TKey &key, const TValue &value) {
            if (!contains(key))
                _map.insert(make_pair(key, value));
            else
                _map[key] = value;
        }

        inline void addRange(const Dictionary &other) {
            for (auto it = other.begin(); it != other.end(); ++it) {
                add(it.key(), it.value());
            }
        }

        inline bool contains(const TKey &key) const {
            return _map.find(key) != _map.end();
        }

        inline bool contains(const TKey &key, const TValue &value) const {
            return value == at(key);
        }

        inline void clear() {
            _map.clear();
        }

        inline size_t count() const {
            return _map.size();
        }

        inline bool isEmpty() const {
            return _map.empty();
        }

        virtual inline bool at(const TKey &key, TValue &value) const {
            auto it = _map.find(key);
            if (it != _map.end()) {
                value = it->second;
                return true;
            } else {
                return false;
            }
        }

        virtual inline TValue &at(const TKey &key) {
            if (contains(key)) {
                return _map.at(key);
            } else {
                static TValue value;
                return value;
            }
        }

        virtual inline const TValue &at(const TKey &key) const {
            if (contains(key)) {
                return _map.at(key);
            } else {
                static TValue value;
                return value;
            }
        }

        inline const TValue &operator[](const TKey &key) const {
            return at(key);
        }

        inline TValue &operator[](const TKey &key) {
            return at(key);
        }

        inline bool set(const TKey &key, const TValue &value) {
            if (!contains(key))
                return false;

            _map[key] = value;
            return true;
        }

        inline bool remove(const TKey &key) {
            return _map.erase(key) > 0;
        }

        inline void keys(Vector<TKey> &k) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(it->first);
            }
        }

        inline void values(Vector<TValue> &v) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                v.add(it->second);
            }
        }

        inline typename PairIterator<TKey, TValue>::const_iterator begin() const override {
            return typename PairIterator<TKey, TValue>::const_iterator(_map.begin());
        }

        inline typename PairIterator<TKey, TValue>::const_iterator end() const override {
            return typename PairIterator<TKey, TValue>::const_iterator(_map.end());
        }

        inline typename PairIterator<TKey, TValue>::iterator begin() override {
            return typename PairIterator<TKey, TValue>::iterator(_map.begin());
        }

        inline typename PairIterator<TKey, TValue>::iterator end() override {
            return typename PairIterator<TKey, TValue>::iterator(_map.end());
        }

        inline typename PairIterator<TKey, TValue>::const_reverse_iterator rbegin() const override {
            return typename PairIterator<TKey, TValue>::const_reverse_iterator(_map.rbegin());
        }

        inline typename PairIterator<TKey, TValue>::const_reverse_iterator rend() const override {
            return typename PairIterator<TKey, TValue>::const_reverse_iterator(_map.rend());
        }

        inline typename PairIterator<TKey, TValue>::reverse_iterator rbegin() override {
            return typename PairIterator<TKey, TValue>::reverse_iterator(_map.rbegin());
        }

        typename PairIterator<TKey, TValue>::reverse_iterator rend() override {
            return typename PairIterator<TKey, TValue>::reverse_iterator(_map.rend());
        }

        void lock() override {
            _mutex.lock();
        }

        bool tryLock() override {
            return _mutex.tryLock();
        }

        void unlock() override {
            _mutex.unlock();
        }

    private:
        std::map<TKey, TValue> _map;
        Mutex _mutex;
    };
}

#endif // Dictionary_h
