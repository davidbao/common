//
//  Array.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef Array_h
#define Array_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/ISortable.h"
#include "data/PrimitiveInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

namespace Common {
    template<class type>
    class Array
            : public IEquatable<Array<type>>,
              public IEvaluation<Array<type>>,
              public Iterator<type>,
              public IIndexable<type, type>,
              public ISortable<type>,
              public IMutex {
    public:
        explicit Array(size_t capacity = DefaultCapacity) : _array(nullptr), _capacity(0), _count(0) {
            setCapacity(capacity);
        }

        Array(const Array &array) : Array(array.capacity()) {
            addRange(array);
        }

        Array(Array &&array)  noexcept : Array(array.capacity()) {
            _array = array._array;
            _count = array._count;
            array._array = nullptr;
            array._count = 0;
        }

        Array(const Array &array, off_t offset, size_t count) : Array(array.capacity()) {
            addRange(array, offset, count);
        }

        Array(const type *array, size_t count, size_t capacity = DefaultCapacity) : Array(capacity) {
            addRange(array, count);
        }

        Array(const type &value, size_t count) : Array(DefaultCapacity) {
            for (size_t i = 0; i < count; i++) {
                add(value);
            }
        }

        ~Array() override {
            deleteArray();

            _count = 0;
        }

        inline size_t count() const override {
            return _count;
        }

        inline type *data() const override {
            return _array;
        }

        inline bool isEmpty() const {
            return count() == 0;
        }

        inline size_t capacity() const {
            return _capacity;
        }

        inline void setCapacity(size_t capacity = DefaultCapacity) {
            if (capacity > 0 && capacity != _capacity) {
                if (count() == 0) {
                    _capacity = capacity;
                    makeNull();
                } else {
                    // have data.
                    size_t size = this->size(_count, capacity);
                    type *temp = _array;
                    _array = new type[size];
                    memcpy(_array, temp, sizeof(type) * _count);
                    _capacity = capacity;
                    delete[] temp;
                }
            }
        }

        Array& operator=(const Array &other) {
            evaluates(other);
            return *this;
        }

        inline type &at(size_t pos) override {
            if (pos < _count) {
                return _array[pos];
            }
            static type t;
            return t;
        }

        inline type at(size_t pos) const override {
            if (pos < _count) {
                return _array[pos];
            }
            static type t;
            return t;
        }

        inline void add(const type &value) {
            if (canResize()) {
                autoResize();
            }
            _array[_count++] = value;
        }

        inline bool addRange(const Array &array) {
            return addRange(array._array, array.count());
        }

        inline bool addRange(const Array &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return addRange(array._array, offset, count);
        }

        inline bool addRange(const type *array, size_t count) {
            if (count > 0) {
                size_t c = count;
                if (_count + c > this->size()) {
                    type *temp = _array;
                    size_t size = this->size(_count + c);
                    _array = new type[size];
                    memcpy(_array, temp, sizeof(type) * (_count));
                    memcpy(_array + _count, array, sizeof(type) * (c));
                    memset(_array + (_count + c), 0, sizeof(type) * (size - _count - c));
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    memcpy(_array + _count, array, sizeof(type) * (c));
                }
                _count += c;
                return true;
            }
            return false;
        }

        inline bool addRange(const type *array, off_t offset, size_t count) {
            return addRange(array + offset, count);
        }

        inline bool insertRange(size_t pos, const Array &array) {
            return insertRange(pos, array, 0, array.count());
        }

        inline bool insertRange(size_t pos, const Array &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return insertRange(pos, array._array + offset, count);
        }

        inline bool insertRange(size_t pos, const type *array, size_t count) {
            if (count > 0 && pos <= _count) {
                size_t c = count;
                size_t size = this->size(_count + c);
                type *temp = _array;
                _array = new type[size];
                memcpy(_array, temp, sizeof(type) * (pos));
                memcpy(_array + pos, array, sizeof(type) * (c));
                memcpy(_array + (pos + c), temp + pos, sizeof(type) * (_count - pos));
                memset(_array + (_count + pos + c), 0, sizeof(type) * (size - _count - pos - c));
                delete[] temp;
                _count += c;

                return true;
            }
            return false;
        }

        inline bool insertRange(size_t pos, const type *array, off_t offset, size_t count) {
            return insertRange(pos, array + offset, count);
        }

        inline bool insert(size_t pos, const type &value) {
            if (pos <= _count) {
                if (canResize()) {
                    autoResize();
                }

                type *temp = _array;
                _array = new type[size()];
                memcpy(_array, temp, sizeof(type) * (pos));
                _array[pos] = value;
                memcpy(_array + (pos + 1), temp + pos, sizeof(type) * (_count - pos));
                delete[] temp;
                _count++;
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const Array &array) {
            return setRange(pos, array, 0, array.count());
        }

        inline bool setRange(size_t pos, const Array &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return setRange(pos, array._array + offset, count);
        }

        inline bool setRange(size_t pos, const type *array, size_t count) {
            if (pos < _count && pos + count < _count) {
                memcpy(_array + pos, array, sizeof(type) * count);
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const type *array, off_t offset, size_t count) {
            return setRange(pos, array + offset, count);
        }

        inline bool set(size_t pos, const type &value) override {
            return set(pos, value, false);
        }

        inline bool set(size_t pos, const type &value, bool insertEmpty) {
            if (pos < _count) {
                _array[pos] = (type) value;
                return true;
            }

            if (insertEmpty) {
                size_t c = (pos / capacity() - count() / capacity() + 1) * capacity();
                if (c > 0) {
                    autoResize(c);
                }
                _array[pos] = (type) value;
                _count = pos + 1;
                return true;
            } else {
                return false;
            }
        }

        inline bool remove(const type &value) {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return removeAt(i);
                }
            }
            return false;
        }

        inline bool removeAt(size_t pos) {
            return removeRange(pos, 1);
        }

        inline bool removeRange(size_t pos, size_t count) {
            size_t from = pos;
            size_t to = count + from - 1;
            if (to < _count && to >= from) {
                if (to != _count - 1)    // except the last one.
                {
                    memmove(_array + from, _array + to + 1, sizeof(type) * (_count - to - 1));
                    memset(_array + _count - count, 0, sizeof(type) * count);
                }
                _count -= count;
                return true;
            }
            return false;
        }

        inline void clear() {
            deleteArray();

            makeNull();
        }

        inline bool contains(const type &value) const {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return true;
                }
            }
            return false;
        }

        inline ssize_t indexOf(const type &value) const {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return (ssize_t) i;
                }
            }
            return -1;
        }

        inline ssize_t lastIndexOf(const type &value) const {
            for (ssize_t i = count() - 1; i >= 0; i--) {
                if (_array[i] == value) {
                    return i;
                }
            }
            return -1;
        }

        inline void evaluates(const Array &other) override {
            _capacity = other.capacity();
            clear();
            addRange(other);
        }

        inline bool equals(const Array &other) const override {
            if (count() != other.count())
                return false;

            for (size_t i = 0; i < count(); i++) {
                if (at(i) != other.at(i))
                    return false;
            }
            return true;
        }

        inline typename Iterator<type>::const_iterator begin() const override {
            return typename Iterator<type>::const_iterator(data());
        }

        inline typename Iterator<type>::const_iterator end() const override {
            return typename Iterator<type>::const_iterator(data() + count());
        }

        inline typename Iterator<type>::iterator begin() override {
            return typename Iterator<type>::iterator(data());
        }

        inline typename Iterator<type>::iterator end() override {
            return typename Iterator<type>::iterator(data() + count());
        }

        inline typename Iterator<type>::const_reverse_iterator rbegin() const override {
            return typename Iterator<type>::const_reverse_iterator(data() + count() - 1);
        }

        inline typename Iterator<type>::const_reverse_iterator rend() const override {
            return typename Iterator<type>::const_reverse_iterator(data() - 1);
        }

        inline typename Iterator<type>::reverse_iterator rbegin() override {
            return typename Iterator<type>::reverse_iterator(data() + count() - 1);
        }

        typename Iterator<type>::reverse_iterator rend() override {
            return typename Iterator<type>::reverse_iterator(data() - 1);
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
        inline void deleteArray() {
            delete[] _array;
            _array = nullptr;
        }

        inline void autoResize() {
            if (canResize()) {
                autoResize(_capacity);
            }
        }

        inline void autoResize(size_t capacity) {
            type *temp = _array;
            _array = new type[_count + capacity];
            memcpy((void *) _array, (void *) temp, sizeof(type) * (_count));
            memset((void *) (_array + _count), 0, sizeof(type) * (capacity));
            delete[] temp;
        }

        inline bool canResize() const {
            return (_count > 0 && (_count % _capacity) == 0);
        }

        inline void makeNull() {
            if (_count > 0 || _array == nullptr) {
                if (_array != nullptr) {
                    deleteArray();
                }
                _count = 0;
                _array = new type[_capacity];
                memset((void *) _array, 0, sizeof(type) * (_capacity));
            }
        }

        inline size_t size() const {
            return size(count(), capacity());
        }

        inline size_t size(size_t count) const {
            return size(count, capacity());
        }

        static size_t size(size_t count, size_t capacity) {
            size_t size = (count / capacity) + 1;
            return size * capacity;
        }

    protected:
        static const size_t DefaultCapacity = 256;

    private:
        type *_array;
        size_t _capacity;
        size_t _count;
        Mutex _mutex;
    };

    typedef Array<int8_t> Int8Array;
    typedef Array<int16_t> Int16Array;
    typedef Array<int32_t> Int32Array;
    typedef Int32Array IntArray;
    typedef Array<int64_t> Int64Array;
    typedef Array<uint8_t> UInt8Array;
    typedef Array<uint16_t> UInt16Array;
    typedef Array<uint32_t> UInt32Array;
    typedef UInt32Array UIntArray;
    typedef Array<uint64_t> UInt64Array;
    typedef Array<float> FloatArray;
    typedef Array<double> DoubleArray;
}

#endif // Array_h
