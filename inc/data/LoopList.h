//
//  LoopList.h
//  common
//
//  Created by baowei on 2022/11/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef LoopList_h
#define LoopList_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/TypeInfo.h"
#include "data/PrimitiveInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

namespace Common {
    template<typename type>
    class LoopList : public IIndexGetter<type>, public IMutex {
    public:
        typedef type *typePtr;

        explicit LoopList(size_t size = DefaultSize) : _front(0), _rear(0) {
            _size = (size >= MinSize && size <= MaxSize) ? size : DefaultSize;
            _array = new typePtr[_size];

            makeNull();
        }

        ~LoopList() override {
            deleteArray();
        }

        inline size_t size() const {
            return _size;
        }

        inline void reserve(size_t size) {
            size = (size >= MinSize && size <= MaxSize) ? size : DefaultSize;
            if (size > _size) {
                size_t count = this->count();
                auto *temp = new typePtr[count];
                copyTo(temp);
                delete[] _array;
                _array = new typePtr[size];
                copy(_array, temp, count);
                zero(_array + count, _size - count);
                _size = size;
                _front = 0;
                _rear = count;
                delete[] temp;
            }
        }

        inline void makeNull() {
            _front = _rear = 0;
            _array = new typePtr[_size];
            zero(_array, _size);
        }

        inline void enqueue(const type &value) {
            bool f = isFull();
            size_t index = _rear >= _size ? 0 : _rear;
            if (f) {
                delete _array[index];
            }
            clone(_array + index, &value, 1);
            _rear = index + 1;
            if (f) {
                _front++;
                if (_front >= _size)
                    _front = 0;
            }
        }

        inline bool dequeue() {
            if (isEmpty()) {
                return false;
            }

            delete _array[_front];

            _front++;
            if (_front >= _size) {
                if(_front == _rear) {
                    _rear = 0;
                }
                _front = 0;
            }

            return true;
        }

        inline type at(size_t pos) const override {
            if (!isEmpty() && pos < count()) {
                if (_rear > _front) {
                    return *_array[_front + pos];
                } else {
                    if (_front + pos < _size) {
                        return *_array[_front + pos];
                    }
                    return *_array[_rear - (count() - pos)];
                }
            }
            return type();
        }

        inline type front() const {
            return at(0);
        }

        inline type back() const {
            return at(count() - 1);
        }

        inline bool isEmpty() const {
            return count() == 0;
        }

        inline bool isFull() const {
            return count() >= _size;
        }

        inline size_t count() const {
            if(_front == 0 && _rear == 0) {
                return 0;
            }
            return (_rear > _front) ? _rear - _front : _rear + _size - _front;
        }

        inline void copyTo(typePtr *value) const {
            if (!isEmpty()) {
                if (_rear > _front) {
                    copy(value, _array + _front, count());
                } else {
                    size_t n = 0;
                    if (_front < _size) {
                        n = _size - _front;
                        copy(value, _array + _front, n);
                    }
                    copy(value + n, _array, (count() - n));
                }
            }
        }

        inline void clear() {
            makeNull();
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
            deleteAllItems();
            delete[] _array;
            _array = nullptr;
        }

        inline void deleteAllItems() {
            if (!isEmpty()) {
                if (_rear > _front) {
                    deleteItem(_front, count());
                } else {
                    int n = 0;
                    if (_front < _size) {
                        n = _size - _front;
                        deleteItem(_front, n);
                    }
                    deleteItem(0, (count() - n));
                }
            }
        }

        inline void deleteItem(int start, int end) {
            for (int i = start; i < end; i++) {
                delete _array[i];
                _array[i] = nullptr;
            }
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

        static void zero(typePtr *dst, size_t count) {
            if (!TypeInfo<type>::isComplex) {
                memset(dst, 0, sizeof(typePtr) * count);
            }
        }

    private:
        size_t _size;
        size_t _front;
        size_t _rear;
        typePtr *_array;
        Mutex _mutex;

    private:
        static const size_t DefaultSize = 1024;         // 1K
        static const size_t MinSize = 16;               // 16 Bytes
        static const size_t MaxSize = 10 * 1024 * 1024; // 10 M
    };
}

#endif // LoopList_h
