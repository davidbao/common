//
//  PList.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef PList_h
#define PList_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/ISortable.h"
#include "data/DataInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

using namespace Threading;

namespace Data {
    template<class type>
    class PList : public Iterator<type *>,
                  public IIndexGetter<type *>,
                  public ISortable<type *>,
                  public IMutex {
    public:
        typedef type *typePtr;

        explicit PList(bool autoDelete = true, size_t capacity = DefaultCapacity) : _array(nullptr),
                                                                                    _autoDelete(autoDelete),
                                                                                    _capacity(0), _count(0) {
            setCapacity(capacity);
        }

        // Never use autoDelete in copy constructor.
        PList(const PList &array) : PList(false, array.capacity()) {
            addRange(array);
        }

        PList(PList &&array)  noexcept : PList(array.autoDelete(), array.capacity()) {
            _array = array._array;
            _count = array._count;
            array._array = nullptr;
            array._count = 0;
        }

        PList(const typePtr *array, size_t count, bool autoDelete = true, size_t capacity = DefaultCapacity) : PList(
                autoDelete, capacity) {
            addRange(array, count);
        }

        ~PList() override {
            deleteArray();

            _count = 0;
        }

        inline size_t count() const override {
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
                    memcpy(_array, temp, sizeof(typePtr) * _count);
                    _capacity = capacity;
                    delete[] temp;
                }
            }
        }

        inline bool autoDelete() const {
            return _autoDelete;
        }

        inline void setAutoDelete(bool autoDelete) {
            _autoDelete = autoDelete;
        }

        inline typePtr at(size_t pos) const override {
            if (pos < _count) {
                return _array[pos];
            }
            return nullptr;
        }

        inline void add(const type *value) {
            if (canResize()) {
                autoResize();
            }
            _array[_count++] = (type *) value;
        }

        inline bool addRange(const PList &array) {
            return addRange(array, 0, array.count());
        }

        inline bool addRange(const PList &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return addRange(array._array, offset, count);
        }

        inline bool addRange(const typePtr *array, size_t count) {
            if (count > 0) {
                size_t c = count;
                if (_count + c > this->size()) {
                    size_t size = this->size(_count + c);
                    typePtr *temp = _array;
                    _array = new typePtr[size];
                    memcpy(_array, temp, sizeof(typePtr) * (_count));
                    memcpy(_array + _count, array, sizeof(typePtr) * (c));
                    memset(_array + (_count + c), 0, sizeof(typePtr) * (size - _count - c));
                    delete[] temp;
                } else {
                    if (canResize()) {
                        autoResize();
                    }
                    memcpy(_array + _count, array, sizeof(typePtr) * (c));
                }
                _count += c;
                return true;
            }
            return false;
        }

        inline bool addRange(const typePtr *array, off_t offset, size_t count) {
            return addRange(array + offset, count);
        }

        inline bool insertRange(size_t pos, const PList &array) {
            return insertRange(pos, array._array, array.count());
        }

        inline bool insertRange(size_t pos, const PList &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return insertRange(pos, array._array + offset, count);
        }

        inline bool insertRange(size_t pos, const typePtr *array, size_t count) {
            if (count > 0 && pos <= _count) {
                size_t c = count;
                size_t size = this->size(_count + c);
                typePtr *temp = _array;
                _array = new typePtr[size];
                memcpy(_array, temp, sizeof(typePtr) * (pos));
                memcpy(_array + pos, array, sizeof(typePtr) * (c));
                memcpy(_array + (pos + c), temp + pos, sizeof(typePtr) * (_count - pos));
                memset(_array + (_count + pos + c), 0, sizeof(typePtr) * (size - _count - pos - c));
                delete[] temp;
                _count += c;

                return true;
            }
            return false;
        }

        inline bool insertRange(size_t pos, const typePtr *array, off_t offset, size_t count) {
            return insertRange(pos, array + offset, count);
        }

        inline bool insert(size_t pos, const typePtr value) {
            if ((size_t) pos <= _count) {
                if (canResize()) {
                    autoResize();
                }

                typePtr *temp = _array;
                _array = new typePtr[size()];
                memcpy(_array, temp, sizeof(typePtr) * (pos));
                _array[pos] = (typePtr) value;
                memcpy(_array + (pos + 1), temp + pos, sizeof(typePtr) * (_count - pos));
                delete[] temp;
                _count++;
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const PList &array) {
            return setRange(pos, array, 0, array.count());
        }

        inline bool setRange(size_t pos, const PList &array, off_t offset, size_t count) {
            if (offset + count > array.count())
                return false;

            return setRange(pos, array._array + offset, count);
        }

        inline bool setRange(size_t pos, const typePtr *array, size_t count) {
            if (pos < _count && pos + count < _count) {
                for (size_t i = pos; i < count; i++) {
                    if (_autoDelete) {
                        delete _array[i];
                    }
                }
                memcpy(_array + pos, array, sizeof(typePtr) * count);
                return true;
            }
            return false;
        }

        inline bool setRange(size_t pos, const typePtr *array, off_t offset, size_t count) {
            return setRange(pos, array + offset, count);
        }

        inline bool set(size_t pos, const typePtr value, bool insertEmpty = false) {
            if (pos < _count) {
                if (_autoDelete) {
                    delete _array[pos];
                }
                _array[pos] = (typePtr) value;
                return true;
            }

            if (insertEmpty) {
                size_t c = (pos / capacity() - count() / capacity() + 1) * capacity();
                if (c > 0) {
                    autoResize(c);
                }
                _array[pos] = (typePtr) value;
                _count = pos + 1;
                return true;
            } else {
                return false;
            }
        }

        inline bool remove(const typePtr value, bool autoDelete = true) {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return removeAt(i, autoDelete);
                }
            }
            return false;
        }

        inline bool removeAt(size_t pos, bool autoDelete = true) {
            if (pos < _count) {
                if (_autoDelete && autoDelete) {
                    delete _array[pos];
                }
                if (pos != _count - 1)    // except the last one.
                {
                    memmove(_array + pos, _array + pos + 1, sizeof(typePtr) * (_count - pos - 1));
                    memset(_array + _count - 1, 0, sizeof(typePtr) * 1);
                }
                _count--;
                return true;
            }
            return false;
        }

        inline bool removeRange(size_t pos, size_t count) {
            size_t from = pos;
            size_t to = count + from - 1;
            if (to < _count && to >= from) {
                if (_autoDelete) {
                    for (size_t i = from; i <= to; i++) {
                        delete _array[i];
                    }
                }
                if (to != _count - 1)    // except the last one.
                {
                    memmove(_array + from, _array + to + 1, sizeof(typePtr) * (_count - to - 1));
                    memset(_array + _count - count, 0, sizeof(typePtr) * count);
                }
                _count -= count;
                return true;
            }
            return false;
        }

        inline void clear(bool autoDelete = true) {
            deleteArray(autoDelete);

            makeNull();
        }

        inline bool contains(const typePtr value) const {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return true;
                }
            }
            return false;
        }

        inline int indexOf(const typePtr value) const {
            for (size_t i = 0; i < count(); i++) {
                if (_array[i] == value) {
                    return (int) i;
                }
            }
            return -1;
        }

        inline ssize_t lastIndexOf(const typePtr value) const {
            for (ssize_t i = count() - 1; i >= 0; i--) {
                if (_array[i] == value) {
                    return i;
                }
            }
            return -1;
        }

        inline typePtr *data() override {
            return _array;
        }

        inline const typePtr *data() const {
            return _array;
        }

        inline typename Iterator<typePtr>::const_iterator begin() const override {
            return typename Iterator<typePtr>::const_iterator(data());
        }

        inline typename Iterator<typePtr>::const_iterator end() const override {
            return typename Iterator<typePtr>::const_iterator(data() + count());
        }

        inline typename Iterator<typePtr>::iterator begin() override {
            return typename Iterator<typePtr>::iterator(data());
        }

        inline typename Iterator<typePtr>::iterator end() override {
            return typename Iterator<typePtr>::iterator(data() + count());
        }

        typename Iterator<typePtr>::const_reverse_iterator rbegin() const override {
            return typename Iterator<typePtr>::const_reverse_iterator(data() + count() - 1);
        }

        inline typename Iterator<typePtr>::const_reverse_iterator rend() const override {
            return typename Iterator<typePtr>::const_reverse_iterator(data() - 1);
        }

        inline typename Iterator<typePtr>::reverse_iterator rbegin() override {
            return typename Iterator<typePtr>::reverse_iterator(data() + count() - 1);
        }

        inline typename Iterator<typePtr>::reverse_iterator rend() override {
            return typename Iterator<typePtr>::reverse_iterator(data() - 1);
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
        inline void autoResize() {
            if (canResize()) {
                autoResize(_capacity);
            }
        }

        inline void autoResize(size_t capacity) {
            typePtr *temp = _array;
            _array = new typePtr[_count + capacity];
            memcpy(_array, temp, sizeof(typePtr) * (_count));
            memset(_array + _count, 0, sizeof(typePtr) * (capacity));
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
                _array = new typePtr[_capacity];
                memset(_array, 0, sizeof(typePtr) * (_capacity));
            }
        }

        inline void deleteArray(bool autoDelete = true) {
            for (size_t i = 0; i < count(); i++) {
                if (_autoDelete && autoDelete) {
                    delete _array[i];
                }
            }
            delete[] _array;
            _array = nullptr;
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
        typePtr *_array;
        bool _autoDelete;
        size_t _capacity;
        size_t _count;
        Mutex _mutex;
    };

    template<class type>
    class CopyPList : public PList<type> {
    public:
        explicit CopyPList(bool autoDelete = true, size_t capacity = PList<type>::DefaultCapacity) : PList<type>(autoDelete,
                                                                                                        capacity) {
        }

        ~CopyPList() override {
        }

        virtual void copyFrom(const CopyPList *values, bool append) {
            if (!append) {
                this->clear();
            }

            for (size_t i = 0; i < values->count(); i++) {
                auto value = new type();
                value->copyFrom(values->at(i));
                this->add(value);
            }
        }

        void copyFrom(const CopyPList *values) {
            return copyFrom(values, false);
        }

        virtual void copyContextFrom(const CopyPList *values) {
        }
    };

    template<class type>
    class ClonePList : public PList<type> {
    public:
        explicit ClonePList(bool autoDelete = true, size_t capacity = PList<type>::DefaultCapacity) : PList<type>(autoDelete,
                                                                                                         capacity) {
        }

        ~ClonePList() override {
        }

        ClonePList *clone() const {
            auto values = new ClonePList<type>();
            for (size_t i = 0; i < this->count(); i++) {
                values->add(this->at(i)->clone());
            }
            return values;
        }

        virtual void copyFrom(const ClonePList *values, bool append) {
            if (!append) {
                this->clear();
            }

            for (size_t i = 0; i < values->count(); i++) {
                this->add(values->at(i)->clone());
            }
        }

        void copyFrom(const ClonePList *values) {
            return copyFrom(values, false);
        }
    };
}

#endif // PList_h
