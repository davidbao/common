//
//  Vector.h
//  common
//
//  Created by baowei on 2022/10/27.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef Vector_h
#define Vector_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/ISortable.h"
#include "data/TypeInfo.h"
#include "data/DataInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

using namespace Threading;

namespace Data {
    // type can be a struct or class.
    // default constructor need to be implemented.
    // copy constructor need to be implemented.
    // operator= need to be implemented.
    // IEvaluation and IEquatable interfaces need to be implemented.
    template<typename type>
    class Vector
            : public IEquatable<Vector<type>>,
              public IEvaluation<Vector<type>>,
              public Iterator<type>,
              public IIndexable<const type &, type>,
              public IMutex {
    public:
        explicit Vector(size_t capacity = DefaultCapacity) : _array(nullptr), _capacity(0), _count(0) {
            setCapacity(capacity);
        }

        Vector(const Vector &array) : Vector(array.capacity()) {
            addRange(array);
        }

        Vector(Vector &&array) noexcept {
            _array = array._array;
            _count = array._count;
            _capacity = array._capacity;
            array._array = nullptr;
            array._count = 0;
        }

        Vector(const Vector &array, off_t offset, size_t count) : Vector(array.capacity()) {
            addRange(array, offset, count);
        }

        Vector(const type *array, size_t count, size_t capacity = DefaultCapacity) : Vector(capacity) {
            addRange(array, count);
        }

        Vector(const type &value, size_t count) : Vector(DefaultCapacity) {
            for (size_t i = 0; i < count; i++) {
                add(value);
            }
        }

        Vector(std::initializer_list<type> list) : Vector(DefaultCapacity) {
            addRange(list.begin(), list.size());
        }

        ~Vector() override {
            deleteArray();

            _count = 0;
        }

        inline virtual size_t count() const {
            return _count;
        }

        inline virtual type *data() {
            return _array;
        }

        inline const type *data() const {
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
                    clear();
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

        Vector &operator=(const Vector &other) {
            if (this != &other) {
                evaluates(other);
            }
            return *this;
        }

        inline type &at(size_t pos) override {
            if (pos < _count) {
                return _array[pos];
            }
            static type value;
            return value;
        }

        inline const type &at(size_t pos) const override {
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
            addRange(&value, 1);
        }

        inline bool addRange(std::initializer_list<type> list) {
            return addRange(list.begin(), list.size());
        }

        inline bool addRange(const Vector &array) {
            return addRange(array._array, array.count());
        }

        inline bool addRange(const Vector &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return addRange(array._array, offset, count);
        }

        inline bool addRange(const type *array, size_t count) {
            if (count > 0) {
                if (_count + count > this->size()) {
                    type *temp = _array;
                    size_t size = this->size(_count + count);
                    _array = new type[size];
                    copy(_array, temp, _count);
                    copy(_array + _count, array, count);
                    zero(_array + (_count + count), size - _count - count);
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    copy(_array + _count, array, count);
                }
                _count += count;

                return true;
            }
            return false;
        }

        inline bool addRange(const type *array, off_t offset, size_t count) {
            return addRange(array + offset, count);
        }

        inline bool insertRange(size_t pos, std::initializer_list<type> list) {
            return insertRange(pos, list.begin(), list.size());
        }

        inline bool insertRange(size_t pos, const Vector &array) {
            return insertRange(pos, array, 0, array.count());
        }

        inline bool insertRange(size_t pos, const Vector &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return insertRange(pos, array._array + offset, count);
        }

        inline bool insertRange(size_t pos, const type *array, size_t count) {
            if (count > 0 && pos <= _count) {
                size_t size = this->size(_count + count);
                type *temp = _array;
                _array = new type[size];
                copy(_array, temp, pos);
                copy(_array + pos, array, count);
                copy(_array + (pos + count), temp + pos, _count - pos);
                zero(_array + (_count + pos + count), size - _count - pos - count);
                delete[] temp;
                _count += count;

                return true;
            }
            return false;
        }

        inline bool insertRange(size_t pos, const type *array, off_t offset, size_t count) {
            return insertRange(pos, array + offset, count);
        }

        inline bool insert(size_t pos, const type &value) {
            return insertRange(pos, &value, 1);
        }

        inline bool setRange(size_t pos, std::initializer_list<type> list) {
            return setRange(pos, list.begin(), list.size());
        }

        inline bool setRange(size_t pos, const Vector &array) {
            return setRange(pos, array, 0, array.count());
        }

        inline bool setRange(size_t pos, const Vector &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return setRange(pos, array._array + offset, count);
        }

        inline bool setRange(size_t pos, const type *array, size_t count) {
            if (pos + count <= _count) {
                copy(_array + pos, array, count);
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const type *array, off_t offset, size_t count) {
            return setRange(pos, array + offset, count);
        }

        inline bool set(size_t pos, const type &value) override {
            return setRange(pos, &value, 1);
        }

        inline bool remove(const type &value) {
            for (size_t i = 0; i < count(); i++) {
                if (equalsValue(i, value)) {
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
                if (to != _count - 1) {
                    move(_array + from, _array + to + 1, _count - to - 1);
                }
                zero(_array + _count - count, count);
                _count -= count;
                return true;
            }
            return false;
        }

        inline void clear() {
            deleteArray();

            makeNull();
        }

        inline void reserve(size_t size) {
            if (size > this->size()) {
                type *temp = _array;
                size_t count = this->size(size);
                _array = new type[count];
                copy(_array, temp, _count);
                zero(_array + _count, count - _count);
                delete[] temp;
            }
        }

        inline bool contains(const type &value) const {
            for (size_t i = 0; i < count(); i++) {
                if (equalsValue(i, value)) {
                    return true;
                }
            }
            return false;
        }

        inline ssize_t indexOf(const type &value) const {
            for (size_t i = 0; i < count(); i++) {
                if (equalsValue(i, value)) {
                    return (ssize_t) i;
                }
            }
            return -1;
        }

        inline ssize_t lastIndexOf(const type &value) const {
            for (ssize_t i = count() - 1; i >= 0; i--) {
                if (equalsValue(i, value)) {
                    return i;
                }
            }
            return -1;
        }

        inline void evaluates(const Vector &other) override {
            _capacity = other.capacity();
            clear();
            addRange(other);
        }

        inline bool equals(const Vector &other) const override {
            if (count() != other.count())
                return false;

            for (size_t i = 0; i < count(); i++) {
                if (!equalsValue(i, other.at(i)))
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

        inline typename Iterator<type>::reverse_iterator rend() override {
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
            zero(_array + _count, capacity);
            delete[] temp;
        }

        inline bool canResize() const {
            return (_count > 0 && (_count % _capacity) == 0);
        }

        inline void makeNull() {
            _count = 0;
            _array = new type[_capacity];
            zero(_array, _capacity);
        }

        inline size_t size() const {
            return size(count(), capacity());
        }

        inline size_t size(size_t count) const {
            return size(count, capacity());
        }

        inline bool equalsValue(size_t pos, const type &value) const {
            if (pos < _count) {
                return _array[pos] == value;
            }
            return false;
        }

        static size_t size(size_t count, size_t capacity) {
            size_t size = (count / capacity) + 1;
            return size * capacity;
        }

        static void copy(type *dst, const type *src, size_t count) {
            if (TypeInfo<type>::isComplex) {
                for (size_t i = 0; i < count; i++) {
                    dst[i].~type();
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

        static void zero(type *dst, size_t count) {
            if (!TypeInfo<type>::isComplex) {
                memset(dst, 0, sizeof(type) * count);
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

    // type can be a struct or class.
    // operator= need to be implemented.
    // copy constructor need to be implemented.
    // IComparable interfaces need to be implemented.
    template<typename type>
    class SortedVector : public Vector<type>, public ISortable<type> {
    public:
        using Vector<type>::data;

        explicit SortedVector(size_t capacity = Vector<type>::DefaultCapacity) : Vector<type>(capacity) {
        }

        SortedVector(const SortedVector &array) : Vector<type>(array) {
        }

        SortedVector(SortedVector &&array) noexcept: Vector<type>(std::move(array)) {
        }

        SortedVector(const SortedVector &array, off_t offset, size_t count) : Vector<type>(array, offset, count) {
        }

        SortedVector(const type *array, size_t count, size_t capacity = Vector<type>::DefaultCapacity) : Vector<type>(
                array,
                count,
                capacity) {
        }

        SortedVector(const type &value, size_t count) : Vector<type>(value, count) {
        }

        SortedVector(std::initializer_list<type> list) : Vector<type>(list) {
        }

        SortedVector &operator=(const SortedVector &other) {
            Vector<type>::evaluates(other);
            return *this;
        }

        inline size_t count() const override {
            return Vector<type>::count();
        }

        inline type *data() override {
            return Vector<type>::data();
        }
    };
}

#endif // Vector_h
