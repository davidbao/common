//
//  LoopVector.h
//  common
//
//  Created by baowei on 2022/11/10.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef LoopVector_h
#define LoopVector_h

#include <cinttypes>
#include <sys/types.h>
#include "system/OsDefine.h"
#include "data/TypeInfo.h"
#include "data/PrimitiveInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

namespace Common {
    template<typename type>
    class LoopVector : public IIndexGetter<type>, public IMutex {
    public:
        explicit LoopVector(size_t size = DefaultSize) : _front(0), _rear(0) {
            _size = (size >= MinSize && size <= MaxSize) ? size : DefaultSize;

            makeNull();
        }

        ~LoopVector() override {
            deleteArray();
        }

        inline size_t size() const {
            return _size;
        }

        inline void reserve(size_t size) {
            size = (size >= MinSize && size <= MaxSize) ? size : DefaultSize;
            if (size > _size) {
                size_t count = this->count();
                type *temp = new type[count];
                copyTo(temp);
                delete[] _array;
                _array = new type[size];
                copy(_array, temp, count);
                zero(_array + count, _size - count);
                _size = size;
                _front = 0;
                _rear = count;
                delete[] temp;
            }
        }

        inline void enqueue(const type &value) {
            bool f = isFull();
            size_t index = _rear >= _size ? 0 : _rear;
            copy(_array + index, &value, 1);
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
                    return _array[_front + pos];
                } else {
                    if (_front + pos < _size) {
                        return _array[_front + pos];
                    }
                    return _array[_rear - (count() - pos)];
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

        inline void copyTo(type *value) const {
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
            delete[] _array;
            _array = nullptr;
        }

        inline void makeNull() {
            _front = _rear = 0;
            _array = new type[_size];
            zero(_array, _size);
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

        static void zero(type *dst, size_t count) {
            if (!TypeInfo<type>::isComplex) {
                memset(dst, 0, sizeof(type) * count);
            }
        }

    private:
        size_t _size;
        size_t _front;
        size_t _rear;
        type *_array;
        Mutex _mutex;

    private:
        static const size_t DefaultSize = 1024;         // 1K
        static const size_t MinSize = 16;               // 16 Bytes
        static const size_t MaxSize = 10 * 1024 * 1024; // 10 M
    };
}

#endif // LoopVector_h
