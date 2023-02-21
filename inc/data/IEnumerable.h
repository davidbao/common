//
//  IEnumerable.h
//  common
//
//  Created by baowei on 2022/8/31.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef IEnumerable_h
#define IEnumerable_h

#include <map>

namespace Data {
    template<typename T>
    class Iterator {
    public:
        Iterator() = default;

        virtual ~Iterator() = default;

        struct iterator;

        struct const_iterator {
            inline const_iterator &operator++() {
                ++it;
                return *this;
            }

            inline const_iterator &operator--() {
                --it;
                return *this;
            }

            inline const_iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline const_iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline const_iterator operator++(int) {
                const_iterator result(*this);
                ++it;
                return result;
            }

            inline const_iterator operator--(int) {
                const_iterator result(*this);
                --it;
                return result;
            }

            inline const_iterator operator+(long i) const {
                const_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const_iterator operator-(long i) const {
                const_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const T &operator[](size_t pos) const { return const_cast<const T &>(*it[pos]); };

            inline const T &operator*() const { return const_cast<const T &>((*it)); }

            inline const T *operator->() const { return const_cast<const T *>(*it); }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline const_iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_iterator(const const_iterator &orig) : it(orig.it) {}

            const_iterator(const T *starter) : it(starter) {}

        private:
            const T *it;
            friend struct iterator;
        };

        struct iterator {
            inline iterator &operator++() {
                ++it;
                return *this;
            }

            inline iterator &operator--() {
                --it;
                return *this;
            }

            inline iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline iterator operator++(int) {
                iterator result(*this);
                ++it;
                return result;
            }

            inline iterator operator--(int) {
                iterator result(*this);
                --it;
                return result;
            }

            inline iterator operator+(long i) const {
                iterator result(*this);
                result.it += i;
                return result;
            }

            inline iterator operator-(long i) const {
                iterator result(*this);
                result.it -= i;
                return result;
            }

            inline T &operator[](size_t pos) const { return *it[pos]; };

            inline T &operator*() const { return (*it); }

            inline T *operator->() const { return *it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline operator const_iterator() const { return const_iterator(it); }

            iterator(const iterator &orig) : it(orig.it) {}

            iterator(T *starter) : it(starter) {}

        private:
            T *it;
            friend struct const_iterator;
        };

        struct reverse_iterator;

        struct const_reverse_iterator {
            inline const_reverse_iterator &operator++() {
                --it;
                return *this;
            }

            inline const_reverse_iterator &operator--() {
                ++it;
                return *this;
            }

            inline const_reverse_iterator &operator+=(long i) {
                it -= i;
                return *this;
            }

            inline const_reverse_iterator &operator-=(long i) {
                it += i;
                return *this;
            }

            inline const_reverse_iterator operator++(int) {
                const_reverse_iterator result(*this);
                --it;
                return result;
            }

            inline const_reverse_iterator operator--(int) {
                const_reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline const_reverse_iterator operator+(long i) const {
                const_reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const_reverse_iterator operator-(long i) const {
                const_reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const T &operator[](size_t pos) const { return const_cast<const T &>(*it[pos]); };

            inline const T &operator*() const { return const_cast<const T &>((*it)); }

            inline const T *operator->() const { return const_cast<const T *>(*it); }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline const_reverse_iterator &operator=(const const_reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_reverse_iterator(const const_reverse_iterator &orig) : it(orig.it) {}

            const_reverse_iterator(const T *starter) : it(starter) {}

        private:
            const T *it;
            friend struct reverse_iterator;
        };

        struct reverse_iterator {
            inline reverse_iterator &operator++(void) {
                --it;
                return *this;
            }

            inline reverse_iterator &operator--(void) {
                ++it;
                return *this;
            }

            inline reverse_iterator &operator+=(long i) {
                it -= i;
                return *this;
            }

            inline reverse_iterator &operator-=(long i) {
                it += i;
                return *this;
            }

            inline reverse_iterator operator++(int) {
                reverse_iterator result(*this);
                --it;
                return result;
            }

            inline reverse_iterator operator--(int) {
                reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline reverse_iterator operator+(long i) const {
                reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline reverse_iterator operator-(long i) const {
                reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline T &operator[](size_t pos) const { return *it[pos]; };

            inline T &operator*(void) const { return (*it); }

            inline T *operator->(void) const { return *it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline reverse_iterator &operator=(const reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            reverse_iterator(const reverse_iterator &orig) : it(orig.it) {}

            inline operator const_reverse_iterator() const { return const_reverse_iterator(it); }

            reverse_iterator(T *starter) : it(starter) {}

        private:
            T *it;
            friend struct const_reverse_iterator;
        };

        virtual const_iterator begin() const = 0;

        virtual const_iterator end() const = 0;

        virtual iterator begin() = 0;

        virtual iterator end() = 0;

        virtual const_reverse_iterator rbegin() const = 0;

        virtual const_reverse_iterator rend() const = 0;

        virtual reverse_iterator rbegin() = 0;

        virtual reverse_iterator rend() = 0;
    };

    template<typename T>
    class Iterator<T *> {
    public:
        Iterator() {
        }

        virtual ~Iterator() {
        }

        struct iterator;

        struct const_iterator {
            inline const_iterator &operator++() {
                ++it;
                return *this;
            }

            inline const_iterator &operator--() {
                --it;
                return *this;
            }

            inline const_iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline const_iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline const_iterator operator++(int) {
                const_iterator result(*this);
                ++it;
                return result;
            }

            inline const_iterator operator--(int) {
                const_iterator result(*this);
                --it;
                return result;
            }

            inline const_iterator operator+(long i) const {
                const_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const_iterator operator-(long i) const {
                const_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const T &operator[](size_t pos) const { return const_cast<const T &>(*it[pos]); };

            inline const T &operator*() const { return const_cast<const T &>(*(*it)); }

            inline const T *operator->() const { return const_cast<const T *>(*it); }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline const_iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_iterator(const const_iterator &orig) : it(orig.it) {}

            const_iterator(T* const *starter) : it(starter) {}

        private:
            T* const *it;
            friend struct iterator;
        };

        struct iterator {
            inline iterator &operator++() {
                ++it;
                return *this;
            }

            inline iterator &operator--() {
                --it;
                return *this;
            }

            inline iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline iterator operator++(int) {
                iterator result(*this);
                ++it;
                return result;
            }

            inline iterator operator--(int) {
                iterator result(*this);
                --it;
                return result;
            }

            inline iterator operator+(long i) const {
                iterator result(*this);
                result.it += i;
                return result;
            }

            inline iterator operator-(long i) const {
                iterator result(*this);
                result.it -= i;
                return result;
            }

            inline T &operator[](size_t pos) const { return *it[pos]; };

            inline T &operator*() const { return *(*it); }

            inline T *operator->() const { return *it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline operator const_iterator() const { return const_iterator(it); }

            iterator(const iterator &orig) : it(orig.it) {}

            iterator(T **starter) : it(starter) {}

        private:
            T **it;
            friend struct const_iterator;
        };

        struct reverse_iterator;

        struct const_reverse_iterator {
            inline const_reverse_iterator &operator++() throw() {
                --it;
                return *this;
            }

            inline const_reverse_iterator &operator--() throw() {
                ++it;
                return *this;
            }

            inline const_reverse_iterator &operator+=(long i) throw() {
                it -= i;
                return *this;
            }

            inline const_reverse_iterator &operator-=(long i) throw() {
                it += i;
                return *this;
            }

            inline const_reverse_iterator operator++(int) throw() {
                const_reverse_iterator result(*this);
                --it;
                return result;
            }

            inline const_reverse_iterator operator--(int) throw() {
                const_reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline const_reverse_iterator operator+(long i) const {
                const_reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const_reverse_iterator operator-(long i) const {
                const_reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const T &operator[](size_t pos) const { return const_cast<const T &>(*it[pos]); };

            inline const T &operator*() const { return const_cast<const T &>(*(*it)); }

            inline const T *operator->() const { return const_cast<const T *>(*it); }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline const_reverse_iterator &operator=(const const_reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_reverse_iterator(const const_reverse_iterator &orig) : it(orig.it) {}

            const_reverse_iterator(T* const *starter) : it(starter) {}

        private:
            T* const *it;
            friend struct reverse_iterator;
        };

        struct reverse_iterator {
            inline reverse_iterator &operator++(void) {
                --it;
                return *this;
            }

            inline reverse_iterator &operator--(void) {
                ++it;
                return *this;
            }

            inline reverse_iterator &operator+=(long i) {
                it -= i;
                return *this;
            }

            inline reverse_iterator &operator-=(long i) {
                it += i;
                return *this;
            }

            inline reverse_iterator operator++(int) {
                reverse_iterator result(*this);
                --it;
                return result;
            }

            inline reverse_iterator operator--(int) {
                reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline reverse_iterator operator+(long i) const {
                reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline reverse_iterator operator-(long i) const {
                reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline T &operator[](size_t pos) const { return *it[pos]; };

            inline T &operator*(void) const { return *(*it); }

            inline T *operator->(void) const { return *it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline reverse_iterator &operator=(const reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            reverse_iterator(const reverse_iterator &orig) : it(orig.it) {}

            inline operator const_reverse_iterator() const { return const_reverse_iterator(it); }

            reverse_iterator(T **starter) : it(starter) {}

        private:
            T **it;
            friend struct const_reverse_iterator;
        };

        virtual const_iterator begin() const = 0;

        virtual const_iterator end() const = 0;

        virtual iterator begin() = 0;

        virtual iterator end() = 0;

        virtual const_reverse_iterator rbegin() const = 0;

        virtual const_reverse_iterator rend() const = 0;

        virtual reverse_iterator rbegin() = 0;

        virtual reverse_iterator rend() = 0;
    };

    template<class TKey, class TValue>
    class PairIterator {
    public:
        PairIterator() {
        }

        virtual ~PairIterator() {
        }

        struct iterator;

        struct const_iterator {
            const TKey &key() const { return it->first; }

            const TValue &value() const { return it->second; }

            inline const_iterator &operator++() {
                ++it;
                return *this;
            }

            inline const_iterator &operator--() {
                --it;
                return *this;
            }

            inline const_iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline const_iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline const_iterator operator++(int) {
                const_iterator result(*this);
                ++it;
                return result;
            }

            inline const_iterator operator--(int) {
                const_iterator result(*this);
                --it;
                return result;
            }

            inline const_iterator operator+(long i) const {
                const_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const_iterator operator-(long i) const {
                const_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const TValue &operator*() const { return const_cast<const TValue &>((*it)); }

            inline const TValue *operator->() const { return const_cast<const TValue *>(*it); }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline const_iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_iterator(const const_iterator &orig) : it(orig.it) {}

            const_iterator(const typename std::map<TKey, TValue>::const_iterator &starter) : it(starter) {}

        private:
            typename std::map<TKey, TValue>::const_iterator it;
            friend struct iterator;
        };

        struct iterator {
            const TKey &key() const { return it->first; }

            const TValue &value() const { return it->second; }

            inline iterator &operator++() {
                ++it;
                return *this;
            }

            inline iterator &operator--() {
                --it;
                return *this;
            }

            inline iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline iterator operator++(int) {
                iterator result(*this);
                ++it;
                return result;
            }

            inline iterator operator--(int) {
                iterator result(*this);
                --it;
                return result;
            }

            inline iterator operator+(long i) const {
                iterator result(*this);
                result.it += i;
                return result;
            }

            inline iterator operator-(long i) const {
                iterator result(*this);
                result.it -= i;
                return result;
            }

            inline TValue &operator*() const { return (*it); }

            inline TValue *operator->() const { return *it; }

            inline bool operator==(const iterator &other) const { return it == other.it; }

            inline bool operator!=(const iterator &other) const { return it != other.it; }

            inline bool operator>(const iterator &other) const { return it > other.it; }

            inline bool operator>=(const iterator &other) const { return it >= other.it; }

            inline bool operator<(const iterator &other) const { return it < other.it; }

            inline bool operator<=(const iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline bool operator==(const const_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_iterator &other) const { return it != other.it; }

            inline bool operator>(const const_iterator &other) const { return it > other.it; }

            inline bool operator>=(const const_iterator &other) const { return it >= other.it; }

            inline bool operator<(const const_iterator &other) const { return it < other.it; }

            inline bool operator<=(const const_iterator &other) const { return it <= other.it; }

            inline iterator &operator=(const const_iterator &orig) {
                it = orig.it;
                return *this;
            }

            inline operator const_iterator() const { return const_iterator(it); }

            iterator(const iterator &orig) : it(orig.it) {}

            iterator(const typename std::map<TKey, TValue>::iterator &starter) : it(starter) {}

        private:
            typename std::map<TKey, TValue>::iterator it;
            friend struct const_iterator;
        };

        struct reverse_iterator;

        struct const_reverse_iterator {
            const TKey &key() const { return it->first; }

            const TValue &value() const { return it->second; }

            inline const_reverse_iterator &operator++() {
                ++it;
                return *this;
            }

            inline const_reverse_iterator &operator--() {
                ++it;
                return *this;
            }

            inline const_reverse_iterator &operator+=(long i) {
                it += i;
                return *this;
            }

            inline const_reverse_iterator &operator-=(long i) {
                it -= i;
                return *this;
            }

            inline const_reverse_iterator operator++(int) {
                const_reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline const_reverse_iterator operator--(int) {
                const_reverse_iterator result(*this);
                --it;
                return result;
            }

            inline const_reverse_iterator operator+(long i) const {
                const_reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline const_reverse_iterator operator-(long i) const {
                const_reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline const TValue &operator*() const { return const_cast<const TValue &>((*it)); }

            inline const TValue *operator->() const { return const_cast<const TValue *>(*it); }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline const_reverse_iterator &operator=(const const_reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            const_reverse_iterator(const const_reverse_iterator &orig) : it(orig.it) {}

            const_reverse_iterator(const typename std::map<TKey, TValue>::const_reverse_iterator &starter) : it(
                    starter) {}

        private:
            typename std::map<TKey, TValue>::const_reverse_iterator it;
            friend struct reverse_iterator;
        };

        struct reverse_iterator {
            const TKey &key() const { return it->first; }

            const TValue &value() const { return it->second; }

            inline reverse_iterator &operator++(void) {
                ++it;
                return *this;
            }

            inline reverse_iterator &operator--(void) {
                --it;
                return *this;
            }

            inline reverse_iterator &operator+=(long i) {
                it -= i;
                return *this;
            }

            inline reverse_iterator &operator-=(long i) {
                it += i;
                return *this;
            }

            inline reverse_iterator operator++(int) {
                reverse_iterator result(*this);
                --it;
                return result;
            }

            inline reverse_iterator operator--(int) {
                reverse_iterator result(*this);
                ++it;
                return result;
            }

            inline reverse_iterator operator+(long i) const {
                reverse_iterator result(*this);
                result.it += i;
                return result;
            }

            inline reverse_iterator operator-(long i) const {
                reverse_iterator result(*this);
                result.it -= i;
                return result;
            }

            inline TValue &operator*(void) const { return (*it); }

            inline TValue *operator->(void) const { return *it; }

            inline bool operator==(const reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const reverse_iterator &other) const { return it <= other.it; }

            inline bool operator==(const const_reverse_iterator &other) const { return it == other.it; }

            inline bool operator!=(const const_reverse_iterator &other) const { return it != other.it; }

            inline bool operator<(const const_reverse_iterator &other) const { return it > other.it; }

            inline bool operator<=(const const_reverse_iterator &other) const { return it >= other.it; }

            inline bool operator>(const const_reverse_iterator &other) const { return it < other.it; }

            inline bool operator>=(const const_reverse_iterator &other) const { return it <= other.it; }

            inline reverse_iterator &operator=(const reverse_iterator &orig) {
                it = orig.it;
                return *this;
            }

            reverse_iterator(const reverse_iterator &orig) : it(orig.it) {}

            inline operator const_reverse_iterator() const { return const_reverse_iterator(it); }

            reverse_iterator(const typename std::map<TKey, TValue>::reverse_iterator &starter) : it(starter) {}

        private:
            typename std::map<TKey, TValue>::reverse_iterator it;
            friend struct const_reverse_iterator;
        };

        virtual const_iterator begin() const = 0;

        virtual const_iterator end() const = 0;

        virtual iterator begin() = 0;

        virtual iterator end() = 0;

        virtual const_reverse_iterator rbegin() const = 0;

        virtual const_reverse_iterator rend() const = 0;

        virtual reverse_iterator rbegin() = 0;

        virtual reverse_iterator rend() = 0;
    };
}

#endif // IEnumerable_h
