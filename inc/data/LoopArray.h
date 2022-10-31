//
//  LoopArray.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef LoopArray_h
#define LoopArray_h

#include <stdio.h>
#include <string.h>

namespace Common {
    template<class type>
    class LoopArray {
    public:
        LoopArray(int maxLength = DefaultMaxLength) {
            _maxLength = maxLength <= 0 ? 1 : maxLength;
            _array = new type[_maxLength];

            makeNull();
        }

        virtual ~LoopArray() {
            delete[] _array;
            _array = nullptr;
        }

        inline void makeNull() {
            memset(_array, 0, sizeof(type) * (_maxLength));
            _front = 0;
            _rear = 0;
        }

        inline void enqueue(type value) {
            bool f = full();
            int index = _rear >= _maxLength ? 0 : _rear;
            _array[index] = value;
            _rear = index + 1;
            if (f) {
                _front++;
                if (_front >= _maxLength)
                    _front = 0;
            }
        }

        inline bool dequeue() {
            if (empty()) {
                return false;
            }

            _front++;
            if (_front >= _maxLength)
                _front = 0;

            // _front = _rear = 0
            if (_front == _rear || (_front == 0 && _rear == _maxLength))
                makeNull();

            return true;
        }

        inline type front() {
            if (empty()) {
                return type();
            }
            return _array[_front];
        }

        inline bool empty() const {
            return (_front == 0 && _rear == 0);
        }

        inline bool full() const {
            return count() >= (uint) _maxLength;
        }

        inline size_t count() const {
            if (empty())
                return 0;

            return (_rear > _front) ? _rear - _front : _rear + _maxLength - _front;
        }

        inline void copyTo(type *value) const {
            if (!empty()) {
                if (_rear > _front) {
                    memcpy(value, _array + _front, count() * sizeof(type));
                } else {
                    int n = 0;
                    if (_front < _maxLength) {
                        n = _maxLength - _front;
                        memcpy(value, _array + _front, n * sizeof(type));
                    }
                    memcpy(value + n, _array, (count() - n) * sizeof(type));
                }
            }
        }

        inline type at(uint i) const {
            if (!empty() && i < count()) {
                if (_rear > _front) {
                    return _array[_front + i];
                } else {
                    if (_front + i < _maxLength) {
                        return _array[_front + i];
                    }
                    return _array[_rear - (count() - i)];
                }
            }
            return type();
        }

        inline int maxLength() const {
            return _maxLength;
        }

        inline void setMaxLength(int maxLength) {
            if (maxLength != _maxLength) {
                delete[] _array;
                _maxLength = maxLength;
                _array = new type[_maxLength];

                makeNull();
            }
        }

        inline void clear() {
            makeNull();
        }

    protected:
        static const int DefaultMaxLength = 1024;

    private:
        int _maxLength;
        int _front;
        int _rear;
        type *_array;
    };
}

#endif // LoopArray_h
