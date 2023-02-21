//
//  Array.h
//  common
//
//  Created by baowei on 2022/11/26.
//  Copyright (c) 2022 com. All rights reserved.
//

#include "system/OsDefine.h"
#include "data/IEnumerable.h"
#include "data/ISortable.h"
#include "data/TypeInfo.h"
#include "data/PrimitiveInterface.h"
#include "thread/Mutex.h"
#include "thread/Locker.h"

using namespace Threading;

namespace Data {
    template<typename type, size_t size>
    class Array : public IEquatable<Array<type, size>>,
                  public IEvaluation<Array<type, size>>,
                  public Iterator<type>,
                  public IIndexGetter<const type &>,
                  public IMutex {
    public:
        Array(std::initializer_list<type> list) {
            copy(_array, list.begin(), list.size());
        }

        Array(const type array[], size_t count) {
            copy(_array, array, count);
        }

        Array(const Array &array) {
            copy(_array, array._array, array.count());
        }

        inline size_t count() const {
            return size;
        }

        inline const type *data() const {
            return _array;
        }

        inline type *data() {
            return _array;
        }

        inline bool isEmpty() const {
            return count() == 0;
        }

        Array &operator=(const Array &other) {
            if (this != &other) {
                evaluates(other);
            }
            return *this;
        }

        inline const type &at(size_t pos) const override {
            if (pos < count()) {
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
            return at(count() - 1);
        }

        inline type &back() {
            return at(count() - 1);
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

        inline void evaluates(const Array &other) override {
            copy(_array, other._array, other.count());
        }

        inline bool equals(const Array &other) const override {
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
        inline bool equalsValue(size_t pos, const type &value) const {
            if (pos < count()) {
                return _array[pos] == value;
            }
            return false;
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

    private:
        type _array[size];
        Mutex _mutex;
    };
}