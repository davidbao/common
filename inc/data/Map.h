//
//  Map.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Map_h
#define Map_h

#include <cstdio>
#include <map>
#include "data/Vector.h"
#include "data/IEnumerable.h"
#include "data/PrimitiveInterface.h"

using namespace Threading;

namespace Data {
    template<class TKey, class TValue>
    class Map : public PairIterator<TKey, TValue *>, public IMutex {
    public:
        typedef TValue *TValuePtr;
        typedef pair<const TKey, TValuePtr> ValueType;
        typedef ValueType &Reference;
        typedef const ValueType &ConstReference;

        explicit Map(bool autoDeleteValue = false) : _autoDeleteValue(autoDeleteValue) {
        }

        Map(const Map &other) : _map(other._map), _autoDeleteValue(other._autoDeleteValue) {
        }

        Map(Map &&other)  noexcept : _map(std::move(other._map)), _autoDeleteValue(other._autoDeleteValue) {
        }

        Map(std::initializer_list<ValueType> list, bool autoDeleteValue = false) : _map(list),
                                                                                   _autoDeleteValue(autoDeleteValue) {
        }

        ~Map() override {
            clear();
        }

        void setAutoDeleteValue(bool autoDeleteValue) {
            _autoDeleteValue = autoDeleteValue;
        }

        inline void add(const TKey &key, TValuePtr value) {
            TValuePtr temp;
            if (at(key, temp)) {
                delete temp;
                _map[key] = value;
            } else {
                _map.insert(make_pair(key, value));
            }
        }

        inline void addRange(const Map<TKey, TValue> &values) {
            Vector<TKey> keys;
            values.keys(keys);
            for (size_t i = 0; i < keys.count(); i++) {
                const TKey *key = keys[i];
                TValue value;
                if (values.at(*key, value))
                    add(*key, value);
            }
        }

        inline bool contains(const TKey &key) const {
            return _map.find(key) != _map.end();
        }

        inline bool contains(const TKey &key, const TValue &value) const {
            TValuePtr temp = nullptr;
            if (at(key, temp) && temp != nullptr) {
                return *temp == value;
            }
            return false;
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

        inline bool isEmpty() const {
            return _map.empty();
        }

        inline bool at(const TKey &key, TValuePtr &value) const {
            auto it = _map.find(key);
            if (it != _map.end()) {
                value = it->second;
                return true;
            } else {
                return false;
            }
        }

        inline TValuePtr at(const TKey &key) const {
            if (contains(key)) {
                return _map.at(key);
            } else {
                return nullptr;
            }
        }

        inline TValuePtr operator[](const TKey &key) const {
            TValuePtr value;
            if (this->at(key, value))
                return value;

            return nullptr;
        }

        inline bool set(const TKey &key, TValuePtr value) {
            if (!contains(key))
                return false;

            return add(key, value);
        }

        inline bool remove(const TKey &key) {
            if (!contains(key))
                return false;

            if (_autoDeleteValue) {
                delete _map[key];
            }
            return _map.erase(key) > 0;
        }

        inline void keys(Vector<TKey> &k) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                k.add(it->first);
            }
        }

        inline void values(Vector<TValuePtr> &v) const {
            auto it = _map.begin();
            for (; it != _map.end(); ++it) {
                v.add(it->second);
            }
        }

        inline typename PairIterator<TKey, TValuePtr>::const_iterator begin() const override {
            return typename PairIterator<TKey, TValuePtr>::const_iterator(_map.begin());
        }

        inline typename PairIterator<TKey, TValuePtr>::const_iterator end() const override {
            return typename PairIterator<TKey, TValuePtr>::const_iterator(_map.end());
        }

        inline typename PairIterator<TKey, TValuePtr>::iterator begin() override {
            return typename PairIterator<TKey, TValuePtr>::iterator(_map.begin());
        }

        inline typename PairIterator<TKey, TValuePtr>::iterator end() override {
            return typename PairIterator<TKey, TValuePtr>::iterator(_map.end());
        }

        inline typename PairIterator<TKey, TValuePtr>::const_reverse_iterator rbegin() const override {
            return typename PairIterator<TKey, TValuePtr>::const_reverse_iterator(_map.rbegin());
        }

        inline typename PairIterator<TKey, TValuePtr>::const_reverse_iterator rend() const override {
            return typename PairIterator<TKey, TValuePtr>::const_reverse_iterator(_map.rend());
        }

        inline typename PairIterator<TKey, TValuePtr>::reverse_iterator rbegin() override {
            return typename PairIterator<TKey, TValuePtr>::reverse_iterator(_map.rbegin());
        }

        typename PairIterator<TKey, TValuePtr>::reverse_iterator rend() override {
            return typename PairIterator<TKey, TValuePtr>::reverse_iterator(_map.rend());
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
        std::map<TKey, TValuePtr> _map;
        Mutex _mutex;
        bool _autoDeleteValue;
    };
}

#endif // Map_h
