//
//  LoopPList.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef LoopPList_h
#define LoopPList_h

#include <stdio.h>
#include <string.h>
#include "system/OsDefine.h"

namespace Common {
    template<class type>
    class LoopPList {
    public:
        LoopPList(int maxLength = DefaultMaxLength, bool autoDelete = true) {
            _maxLength = maxLength <= 0 ? 1 : maxLength;
            _array = new type *[_maxLength];
            setAutoDelete(autoDelete);

            makeNullInner(false);
        }

        ~LoopPList() {
            makeNull();
            delete[] _array;
            _array = nullptr;
        }

        inline void makeNull(bool autoDelete = true) {
            makeNullInner(autoDelete && _autoDelete);
        }

        inline void enqueue(const type *value) {
            bool f = full();
            int index = _rear >= _maxLength ? 0 : _rear;
            if (f) {
                if (_autoDelete) {
                    delete _array[index];
                }
            }
            _array[index] = (type *) value;
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

            if (_autoDelete) {
                delete _array[_front];
                _array[_front] = nullptr;
            }
            _front++;
            if (_front >= _maxLength)
                _front = 0;

            // _front = _rear = 0
            if (_front == _rear || (_front == 0 && _rear == _maxLength))
                makeNull();

            return true;
        }

        inline type *front() {
            if (empty()) {
                return nullptr;
            }
            return _array[_front];
        }

        inline bool empty() const {
            return (_front == 0 && _rear == 0);
        }

        inline bool full() const {
            return count() >= (size_t) _maxLength;
        }

        inline size_t count() const {
            if (empty())
                return 0;

            return (_rear > _front) ? _rear - _front : _rear + _maxLength - _front;
        }

        inline void copyTo(type **value) const {
            if (!empty()) {
                if (_rear > _front) {
                    memcpy(value, _array + _front, count() * sizeof(type *));
                } else {
                    int n = 0;
                    if (_front < _maxLength) {
                        n = _maxLength - _front;
                        memcpy(value, _array + _front, n * sizeof(type *));
                    }
                    memcpy(value + n, _array, (count() - n) * sizeof(type *));
                }
            }
        }

        inline type *at(size_t pos) const {
            if (!empty() && pos < count()) {
                if (_rear > _front) {
                    return _array[_front + pos];
                } else {
                    if (_front + (int) pos < _maxLength) {
                        return _array[_front + pos];
                    }
                    return _array[_rear - (count() - pos)];
                }
            }
            return nullptr;
        }

        inline void setAutoDelete(bool autoDelete) {
            _autoDelete = autoDelete;
        }

        inline bool autoDelete() const {
            return _autoDelete;
        }

        inline int maxLength() const {
            return _maxLength;
        }

        inline void setMaxLength(int maxLength) {
            if (maxLength != _maxLength) {
                delete[] _array;
                _maxLength = maxLength;
                _array = new type *[_maxLength];

                makeNull();
            }
        }

        inline void clear() {
            makeNull();
        }

    private:
        void makeNullInner(bool autoDelete = true) {
            if (autoDelete) {
                deleteAllItems();
            } else {
                memset(_array, 0, sizeof(type *) * (_maxLength));
            }
            _front = 0;
            _rear = 0;
        }

        void deleteAllItems() {
            if (!empty()) {
                if (_rear > _front) {
                    deleteItem(_front, (int)count());
                } else {
                    int n = 0;
                    if (_front < _maxLength) {
                        n = _maxLength - _front;
                        deleteItem(_front, n);
                    }
                    deleteItem(0, (int)(count() - n));
                }
            }
        }

        void deleteItem(int start, int end) {
            for (int i = start; i < end; i++) {
                delete _array[i];
                _array[i] = nullptr;
            }
        }

    protected:
        static const int DefaultMaxLength = 1024;

    private:
        int _maxLength;
        int _front;
        int _rear;
        type **_array;
        bool _autoDelete;
    };
}
#endif // LoopPList_h
