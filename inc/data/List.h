//
//  List.h
//  common
//
//  Created by baowei on 2022/10/27.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef List_h
#define List_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/ISortable.h"
#include "data/TypeInfo.h"
#include "data/PrimitiveInterface.h"
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
    class List
            : public IEquatable<List<type>>,
              public IEvaluation<List<type>>,
              public Iterator<type *>,
              public IIndexable<const type &, type>,
              public IMutex {
    public:
        typedef type *typePtr;

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

        List(std::initializer_list<type> list) : List(DefaultCapacity) {
            addRange(list.begin(), list.size());
        }

        ~List() override {
            deleteArray();

            _count = 0;
        }

        inline virtual size_t count() const {
            return _count;
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
                    typePtr *temp = _array;
                    _array = new typePtr[size];
                    copy(_array, temp, _count);
                    _capacity = capacity;
                    delete[] temp;
                }
            }
        }

        List &operator=(const List &other) {
            if (this != &other) {
                evaluates(other);
            }
            return *this;
        }

        inline type &at(size_t pos) override {
            if (pos < _count) {
                return *_array[pos];
            }
            static type t;
            return t;
        }

        inline const type &at(size_t pos) const override {
            if (pos < _count) {
                return *_array[pos];
            }
            static type value;
            return value;
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

        inline bool addRange(const List &array) {
            return addRange(array, 0, array.count());
        }

        inline bool addRange(const List &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            if (count > 0) {
                if (_count + count > this->size()) {
                    typePtr *temp = _array;
                    size_t size = this->size(_count + count);
                    _array = new typePtr[size];
                    copy(_array, temp, _count);
                    clone(_array + _count, array._array + offset, count);
                    zero(_array + (_count + count), size - _count - count);
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    clone(_array + _count, array._array + offset, count);
                }
                _count += count;

                return true;
            }
            return false;
        }

        inline bool addRange(const type *array, size_t count) {
            if (count > 0) {
                if (_count + count > this->size()) {
                    typePtr *temp = _array;
                    size_t size = this->size(_count + count);
                    _array = new typePtr[size];
                    copy(_array, temp, _count);
                    clone(_array + _count, array, count);
                    zero(_array + (_count + count), size - _count - count);
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    clone(_array + _count, array, count);
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

        inline bool insertRange(size_t pos, const List &array) {
            return insertRange(pos, array, 0, array.count());
        }

        inline bool insertRange(size_t pos, const List &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            if (count > 0 && pos <= _count) {
                size_t size = this->size(_count + count);
                typePtr *temp = _array;
                _array = new typePtr[size];
                copy(_array, temp, pos);
                clone(_array + pos, array._array + offset, count);
                copy(_array + (pos + count), temp + pos, _count - pos);
                zero(_array + (_count + pos + count), size - _count - pos - count);
                delete[] temp;
                _count += count;

                return true;
            }
            return false;
        }

        inline bool insertRange(size_t pos, const type *array, size_t count) {
            if (count > 0 && pos <= _count) {
                size_t size = this->size(_count + count);
                typePtr *temp = _array;
                _array = new typePtr[size];
                copy(_array, temp, pos);
                clone(_array + pos, array, count);
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

        inline bool setRange(size_t pos, const List &array) {
            return setRange(pos, array, 0, array.count());
        }

        inline bool setRange(size_t pos, const List &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            if (pos + count <= _count) {
                for (size_t i = pos; i < count; i++) {
                    delete _array[i];
                }
                clone(_array + pos, array._array + offset, count);
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const type *array, size_t count) {
            if (pos + count <= _count) {
                clone(_array + pos, array, count);
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
                for (size_t i = from; i <= to; i++) {
                    delete _array[i];
                }
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
                typePtr *temp = _array;
                size_t count = this->size(size);
                _array = new typePtr[count];
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

        inline void evaluates(const List &other) override {
            _capacity = other.capacity();
            clear();
            addRange(other);
        }

        inline bool equals(const List &other) const override {
            if (count() != other.count())
                return false;

            for (size_t i = 0; i < count(); i++) {
                if (!equalsValue(i, other.at(i)))
                    return false;
            }
            return true;
        }

        inline typename Iterator<type *>::const_iterator begin() const override {
            return typename Iterator<type *>::const_iterator(data());
        }

        inline typename Iterator<type *>::const_iterator end() const override {
            return typename Iterator<type *>::const_iterator(data() + count());
        }

        inline typename Iterator<type *>::iterator begin() override {
            return typename Iterator<type *>::iterator(data());
        }

        inline typename Iterator<type *>::iterator end() override {
            return typename Iterator<type *>::iterator(data() + count());
        }

        inline typename Iterator<type *>::const_reverse_iterator rbegin() const override {
            return typename Iterator<type *>::const_reverse_iterator(data() + count() - 1);
        }

        inline typename Iterator<type *>::const_reverse_iterator rend() const override {
            return typename Iterator<type *>::const_reverse_iterator(data() - 1);
        }

        inline typename Iterator<type *>::reverse_iterator rbegin() override {
            return typename Iterator<type *>::reverse_iterator(data() + count() - 1);
        }

        inline typename Iterator<type *>::reverse_iterator rend() override {
            return typename Iterator<type *>::reverse_iterator(data() - 1);
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

        inline virtual typePtr *data() {
            return _array;
        }

        inline const typePtr *data() const {
            return _array;
        }

    private:
        inline void deleteArray() {
            for (size_t i = 0; i < _count; i++) {
                delete _array[i];
            }
            delete[] _array;
            _array = nullptr;
        }

        inline void autoResize() {
            if (canResize()) {
                autoResize(_capacity);
            }
        }

        inline void autoResize(size_t capacity) {
            typePtr *temp = _array;
            _array = new typePtr[_count + capacity];
            copy(_array, temp, _count);
            zero(_array + _count, capacity);
            delete[] temp;
        }

        inline bool canResize() const {
            return (_count > 0 && (_count % _capacity) == 0);
        }

        inline void makeNull() {
            _count = 0;
            _array = new typePtr[_capacity];
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
                return *_array[pos] == value;
            }
            return false;
        }

        static size_t size(size_t count, size_t capacity) {
            size_t size = (count / capacity) + 1;
            return size * capacity;
        }

        static void clone(typePtr *dst, const type *src, size_t count) {
            for (size_t i = 0; i < count; i++) {
                dst[i] = new type(src[i]);
            }
        }

        static void clone(typePtr *dst, const typePtr *src, size_t count) {
            for (size_t i = 0; i < count; i++) {
                dst[i] = new type(*src[i]);
            }
        }

        static void copy(typePtr *dst, const typePtr *src, size_t count) {
            memcpy((void *) dst, (const void *) src, sizeof(typePtr) * count);
        }

        static void move(typePtr *dst, const typePtr *src, size_t count) {
            memmove((void *) dst, (const void *) src, sizeof(typePtr) * count);
        }

        static void zero(typePtr *dst, size_t count) {
            if (!TypeInfo<type>::isComplex) {
                memset(dst, 0, sizeof(typePtr) * count);
            }
        }

    protected:
        static const size_t DefaultCapacity = 256;

    private:
        typePtr *_array;
        size_t _capacity;
        size_t _count;
        Mutex _mutex;
    };

    // type can be a struct or class.
    // operator= need to be implemented.
    // copy constructor need to be implemented.
    // IComparable interfaces need to be implemented.
    template<typename type>
    class SortedList : public List<type>, public ISortable<type *> {
    public:
        using List<type>::data;

        explicit SortedList(size_t capacity = List<type>::DefaultCapacity) : List<type>(capacity) {
        }

        SortedList(const SortedList &array) : List<type>(array) {
        }

        SortedList(SortedList &&array) noexcept: List<type>(array) {
        }

        SortedList(const SortedList &array, off_t offset, size_t count) :
                List<type>(array, offset, count) {
        }

        SortedList(const type *array, size_t count, size_t capacity = List<type>::DefaultCapacity) :
                List<type>(array, count, capacity) {
        }

        SortedList(const type &value, size_t count) : List<type>(value, count) {
        }

        SortedList &operator=(const SortedList &other) {
            List<type>::evaluates(other);
            return *this;
        }

        inline size_t count() const override {
            return List<type>::count();
        }

        inline typename List<type>::typePtr *data() override {
            return List<type>::data();
        }
    };
}

#endif // List_h
