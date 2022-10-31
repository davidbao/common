//
//  List.h
//  common
//
//  Created by baowei on 2022/10/27.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef List_h
#define List_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/PrimitiveInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

namespace Common {
    // type can be a struct.
    // type can be a class, IComparable and IEvaluation interfaces need to be implemented.
    // operator= need to be implemented.
    template<class type>
    class List
            : public IEquatable<List<type>>,
              public IEvaluation<List<type>>,
              public Iterator<type>,
              public IIndexable<type, type>,
              public ISortable<type>,
              public IMutex {
    public:
        explicit List(size_t capacity = DefaultCapacity) : _array(nullptr), _capacity(0), _count(0) {
            setCapacity(capacity);
        }

        List(const List &array) : List(array.capacity()) {
            addRange(array);
        }

        List(List &&array) noexcept: List(array.capacity()) {
            _array = array._array;
            _count = array._count;
            array._array = nullptr;
            array._count = 0;
        }

        List(const List &array, off_t offset, size_t count) : List(array.capacity()) {
            addRange(array, offset, count);
        }

        List(const type *array, size_t count, size_t capacity = DefaultCapacity) : List(capacity) {
            addRange(array, count);
        }

        List(const type &value, size_t count) : List(DefaultCapacity) {
            for (size_t i = 0; i < count; i++) {
                add(value);
            }
        }

        ~List() override {
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
                    copy(_array, temp, _count);
                    _capacity = capacity;
                    delete[] temp;
                }
            }
        }

        List &operator=(const List &other) {
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

        inline type front() const {
            return at(0);
        }

        inline type &front() {
            return at(0);
        }

        inline type back() const {
            return at(_count - 1);
        }

        inline type &back() {
            return at(_count - 1);
        }

        inline void add(const type &value) {
            if (canResize()) {
                autoResize();
            }
            copy(_array + (_count++), &value, 1);
        }

        inline bool addRange(const List &array) {
            return addRange(array._array, array.count());
        }

        inline bool addRange(const List &array, off_t offset, size_t count) {
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
                    copy(_array, temp, _count);
                    copy(_array + _count, array, c);
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    copy(_array + _count, array, c);
                }
                _count += c;

                return true;
            }
            return false;
        }

        inline bool addRange(const type *array, off_t offset, size_t count) {
            return addRange(array + offset, count);
        }

        inline bool insertRange(size_t pos, const List &array) {
            return insertRange(pos, array, 0, array.count());
        }

        inline bool insertRange(size_t pos, const List &array, off_t offset, size_t count) {
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
                copy(_array, temp, pos);
                copy(_array + pos, array, c);
                copy(_array + (pos + c), temp + pos, _count - pos);
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
                copy(_array, temp, pos);
                copy(_array + pos, &value, 1);
                copy(_array + (pos + 1), temp + pos, _count - pos);
                delete[] temp;
                _count++;

                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const List &array) {
            return setRange(pos, array, 0, array.count());
        }

        inline bool setRange(size_t pos, const List &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return setRange(pos, array._array + offset, count);
        }

        inline bool setRange(size_t pos, const type *array, size_t count) {
            if (pos < _count && pos + count < _count) {
                copy(_array + pos, array, count);
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
                copy(_array + pos, &value, 1);
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
                    move(_array + from, _array + to + 1, _count - to - 1);
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

        inline void evaluates(const List &other) override {
            _capacity = other.capacity();
            clear();
            addRange(other);
        }

        inline bool equals(const List &other) const override {
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

        inline typename Iterator<type>::reverse_const_iterator rbegin() const override {
            return typename Iterator<type>::reverse_const_iterator(data() + count() - 1);
        }

        inline typename Iterator<type>::reverse_const_iterator rend() const override {
            return typename Iterator<type>::reverse_const_iterator(data() - 1);
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
            copy(_array, temp, _count);
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

        static void copy(type *dst, const type *src, size_t count) {
            if (TypeInfo<type>::isComplex) {
                for (size_t i = 0; i < count; i++) {
                    new(&dst[i]) type(src[i]);
                }
            } else {
                memcpy((void *) dst, (const void *) src, sizeof(type) * count);
            }
        }

        static void move(type *dst, const type *src, size_t count) {
            if (TypeInfo<type>::isComplex) {
                for (size_t i = 0; i < count; i++) {
                    dst[i] = src[i];
                }
            } else {
                memmove((void *) dst, (const void *) src, sizeof(type) * count);
            }
        }

    protected:
        static const size_t DefaultCapacity = 256;

    private:
        type *_array;
        size_t _capacity;
        size_t _count;
        Mutex _mutex;
    };
}

#endif // List_h
