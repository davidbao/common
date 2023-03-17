//
//  PrimitiveInterface.h
//  common
//
//  Created by baowei on 2022/8/31.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef PrimitiveInterface_h
#define PrimitiveInterface_h

#include "system/OsDefine.h"

namespace Data {
    template<typename T, typename K = T>
    class IEquatable {
    public:
        virtual ~IEquatable() = default;

        //
        // Summary:
        //     Indicates whether the current object is equal to another object of the same type.
        //
        // Parameters:
        //   other:
        //     An object to compare with this object.
        //
        // Returns:
        //     true if the current object is equal to the other parameter; otherwise, false.
        virtual bool equals(const K &other) const = 0;

        friend bool operator==(const T &a, const K &b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(const T &a, K &b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(T &a, const K &b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(T &a, K &b) noexcept {
            return a.equals(b);
        }

        friend bool operator!=(const T &a, const K &b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(const T &a, K &b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(T &a, const K &b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(T &a, K &b) noexcept {
            return !a.equals(b);
        }
    };

    template<typename T, typename K>
    class IEquatable<T, K *> {
    public:
        virtual ~IEquatable() = default;

        //
        // Summary:
        //     Indicates whether the current object is equal to another object of the same type.
        //
        // Parameters:
        //   other:
        //     An object to compare with this object.
        //
        // Returns:
        //     true if the current object is equal to the other parameter; otherwise, false.
        virtual bool equals(const K *other) const = 0;

        friend bool operator==(const T &a, const K *b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(const T &a, K *b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(T &a, const K *b) noexcept {
            return a.equals(b);
        }

        friend bool operator==(T &a, K *b) noexcept {
            return a.equals(b);
        }

        friend bool operator!=(const T &a, const K *b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(const T &a, K *b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(T &a, const K *b) noexcept {
            return !a.equals(b);
        }

        friend bool operator!=(T &a, K *b) noexcept {
            return !a.equals(b);
        }
    };

    template<typename T>
    class IEvaluation {
    public:
        virtual ~IEvaluation() = default;

        //
        // Summary:
        //     Set the current object from another object of the same type.
        //
        // Parameters:
        //   other:
        //     An object to compare with this object.
        virtual void evaluates(const T &other) = 0;
    };

    template<typename T>
    class IComparable {
    public:
        virtual ~IComparable() = default;

        //
        // Summary:
        //     Compares the current instance with another object of the same type and returns
        //     an integer that indicates whether the current instance precedes, follows, or
        //     occurs in the same position in the sort order as the other object.
        //
        // Parameters:
        //   other:
        //     An object to compare with this instance.
        //
        // Returns:
        //     A value that indicates the relative order of the objects being compared. The
        //     return value has these meanings: Value Meaning Less than zero This instance precedes
        //     other in the sort order. Zero This instance occurs in the same position in the
        //     sort order as others. Greater than zero This instance follows other in the sort
        //     order.
        virtual int compareTo(const T &other) const = 0;

        bool operator>(const T &other) const {
            return compareTo(other) > 0;
        }

        bool operator>=(const T &other) const {
            return compareTo(other) >= 0;
        }

        bool operator<(const T &other) const {
            return compareTo(other) < 0;
        }

        bool operator<=(const T &other) const {
            return compareTo(other) <= 0;
        }
    };

    template<typename T>
    class IComparable<T *> {
    public:
        virtual ~IComparable() = default;

        //
        // Summary:
        //     Compares the current instance with another object of the same type and returns
        //     an integer that indicates whether the current instance precedes, follows, or
        //     occurs in the same position in the sort order as the other object.
        //
        // Parameters:
        //   other:
        //     An object to compare with this instance.
        //
        // Returns:
        //     A value that indicates the relative order of the objects being compared. The
        //     return value has these meanings: Value Meaning Less than zero This instance precedes
        //     other in the sort order. Zero This instance occurs in the same position in the
        //     sort order as others. Greater than zero This instance follows other in the sort
        //     order.
        virtual int compareTo(const T *other) const = 0;

        bool operator>(const T *other) const {
            return compareTo(other) > 0;
        }

        bool operator>=(const T *other) const {
            return compareTo(other) >= 0;
        }

        bool operator<(const T *other) const {
            return compareTo(other) < 0;
        }

        bool operator<=(const T *other) const {
            return compareTo(other) <= 0;
        }
    };

    template<typename T>
    class IComparer {
    public:
        virtual int compare(const T &x, const T &y) const = 0;
    };

    template<typename T>
    class ICloneable {
    public:
        virtual ~ICloneable() = default;

        //
        // Summary:
        //     Creates a new object that is a copy of the current instance.
        //
        // Returns:
        //   A new object that is a copy of this instance.
        virtual T *clone() const = 0;
    };

    template<class Getter>
    class IIndexGetter {
    public:
        virtual ~IIndexGetter() = default;

        virtual Getter at(size_t pos) const = 0;

        Getter operator[](int pos) const {
            return this->at((size_t) pos);
        }

        Getter operator[](unsigned int pos) const {
            return this->at((size_t) pos);
        }

        Getter operator[](long pos) const {
            return this->at(pos);
        }

        Getter operator[](unsigned long pos) const {
            return this->at(pos);
        }

#ifdef _WIN64
        Getter operator[](size_t pos) const {
            return this->at(pos);
        }

        Getter operator[](ssize_t pos) const {
            return this->at(pos);
        }
#endif
    };

    template<class Setter>
    class IIndexSetter {
    public:
        virtual ~IIndexSetter() = default;

        virtual Setter &at(size_t pos) = 0;

        virtual bool set(size_t pos, const Setter &value) = 0;

        Setter &operator[](int pos) {
            return this->at((size_t) pos);
        }

        Setter &operator[](unsigned int pos) {
            return this->at((size_t) pos);
        }

        Setter &operator[](long pos) {
            return this->at((size_t) pos);
        }

        Setter &operator[](unsigned long pos) {
            return this->at((size_t) pos);
        }

        bool set(signed pos, const Setter &value) {
            return set((size_t) pos, value);
        }

#ifdef _WIN64
        Setter &operator[](size_t pos) {
            return this->at((size_t) pos);
        }

        Setter &operator[](ssize_t pos) {
            return this->at((size_t) pos);
        }
#endif
    };

    template<class Getter, class Setter>
    class IIndexable {
    public:
        virtual ~IIndexable() = default;

        virtual Getter at(size_t pos) const = 0;

        virtual Setter &at(size_t pos) = 0;

        virtual bool set(size_t pos, const Setter &value) = 0;

        Getter operator[](int pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](int pos) {
            return this->at((size_t) pos);
        }

        Getter operator[](unsigned int pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](unsigned int pos) {
            return this->at((size_t) pos);
        }

        Getter operator[](long pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](long pos) {
            return this->at((size_t) pos);
        }

        Getter operator[](unsigned long pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](unsigned long pos) {
            return this->at((size_t) pos);
        }

        bool set(signed pos, const Setter &value) {
            return set((size_t) pos, value);
        }

#ifdef _WIN64
        Getter operator[](size_t pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](size_t pos) {
            return this->at((size_t) pos);
        }

        Getter operator[](ssize_t pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](ssize_t pos) {
            return this->at((size_t) pos);
        }
#endif
    };

    template<class valueType, class posType>
    class IPositionGetter {
    public:
        virtual ~IPositionGetter() = default;

        virtual valueType at(posType pos) const = 0;

        valueType operator[](posType pos) const {
            return this->at(pos);
        }
    };

    template<class valueType, class posType>
    class IPositionSetter {
    public:
        virtual ~IPositionSetter() = default;

        virtual valueType &at(posType pos) = 0;

        virtual bool set(posType pos, const valueType &value) = 0;

        valueType &operator[](posType pos) {
            return this->at(pos);
        }
    };

    template<class valueType, class posType>
    class IPositionable {
    public:
        virtual ~IPositionable() = default;

        virtual valueType at(posType pos) const = 0;

        valueType operator[](posType pos) const {
            return this->at(pos);
        }

        virtual valueType &at(posType pos) = 0;

        virtual bool set(posType pos, const valueType &value) = 0;

        valueType &operator[](posType pos) {
            return this->at(pos);
        }
    };

    template<typename T>
    class IFormatProvider {
    public:
        virtual ~IFormatProvider() = default;

        virtual const T *getFormat(const char *typeName) const = 0;
    };
}

#endif // PrimitiveInterface_h
