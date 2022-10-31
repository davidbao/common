//
//  PrimitiveInterface.h
//  common
//
//  Created by baowei on 2022/8/31.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef PrimitiveInterface_h
#define PrimitiveInterface_h

#include "system/OsDefine.h"
#include <type_traits>

namespace Common {
    template<typename T>
    class IEquatable {
    public:
        virtual ~IEquatable() {
        }

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
        virtual bool equals(const T &other) const = 0;

        bool operator==(const T &other) const {
            return this->equals(other);
        }

        bool operator!=(const T &other) const {
            return !this->operator==(other);
        }
    };

    template<typename T>
    class IEvaluation {
    public:
        virtual ~IEvaluation() {
        }

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
        virtual ~IComparable() {
        }

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
    class IComparer {
    public:
        virtual int compare(const T &x, const T &y) const = 0;
    };

    template<typename T>
    class ICloneable {
    public:
        virtual ~ICloneable() {
        }

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
        virtual ~IIndexGetter() {
        }

        virtual Getter at(size_t pos) const = 0;

        Getter operator[](int32_t pos) const {
            return this->at((size_t) pos);
        }

#ifdef OS_X64

        Getter operator[](size_t pos) const {
            return this->at(pos);
        }

        Getter operator[](uint32_t pos) const {
            return this->at((size_t) pos);
        }

        Getter operator[](ssize_t pos) const {
            return this->at((size_t) pos);
        }

#endif
    };

    template<class Setter>
    class IIndexSetter {
    public:
        virtual ~IIndexSetter() {
        }

        virtual Setter &at(size_t pos) = 0;

        virtual bool set(size_t pos, const Setter &value) = 0;

        Setter &operator[](int32_t pos) {
            return this->at((size_t) pos);
        }

        bool set(int32_t pos, const Setter &value) {
            return set((size_t) pos, value);
        }

#ifdef OS_X64

        Setter &operator[](size_t pos) const {
            return this->at(pos);
        }

        Setter &operator[](uint32_t pos) const {
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
        virtual ~IIndexable() {
        }

        virtual Getter at(size_t pos) const = 0;

        virtual Setter &at(size_t pos) = 0;

        virtual bool set(size_t pos, const Setter &value) = 0;

        Getter operator[](int32_t pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](int32_t pos) {
            return this->at((size_t) pos);
        }

        bool set(int32_t pos, const Setter &value) {
            return set((size_t) pos, value);
        }

#ifdef OS_X64

        Getter operator[](uint32_t pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](uint32_t pos) {
            return this->at((size_t) pos);
        }

        Getter operator[](size_t pos) const {
            return this->at(pos);
        }

        Setter &operator[](size_t pos) {
            return this->at(pos);
        }

        Getter operator[](ssize_t pos) const {
            return this->at((size_t) pos);
        }

        Setter &operator[](ssize_t pos) {
            return this->at((size_t) pos);
        }

#endif
    };

// Only use the Quicksort algorithm.
    template<typename T>
    class ISortable {
    public:
        // A signed integer that indicates the relative values of x and y, as shown in the following table.
        // Value	            Meaning
        // Less than 0	        x is less than y.
        // 0	                x equals y.
        // Greater than 0	    x is greater than y.
        typedef int(*Comparison)(const T &x, const T &y);

        virtual ~ISortable() {
        }

        virtual size_t count() const = 0;

        virtual T *data() const = 0;

        // type must be override operator< and operator>, IComparable is better.
        void sort(bool asc = true) {
            sort((size_t) 0, count(), asc);
        }

        // type must be override operator< and operator>, IComparable is better.
        void sort(size_t pos, size_t count, bool asc = true) {
            if (pos < count && count > 0 && count <= this->count())
                sort(data(), pos, count, asc);
        }

        void sort(Comparison comparison, bool asc = true) {
            sort((size_t) 0, count(), comparison, asc);
        }

        void sort(size_t pos, size_t count, Comparison comparison, bool asc = true) {
            if (pos < count && count > 0 && count <= this->count())
                sort(data(), pos, count, comparison, asc);
        }

        void sort(const IComparer<T> &comparer, bool asc = true) {
            sort((size_t) 0, count(), comparer, asc);
        }

        void sort(size_t pos, size_t count, const IComparer<T> &comparer, bool asc = true) {
            if (pos < count && count > 0 && count <= this->count())
                sort(data(), pos, count, comparer, asc);
        }

    public:
        // T must be override operator< and operator>, IComparable is better.
        static void sort(T array[], size_t count, bool asc = true) {
            sort(array, 0, count, asc);
        }

        // T must be override operator< and operator>, IComparable is better.
        static void sort(T array[], size_t pos, size_t count, bool asc = true) {
            quicksort(array, (int) (pos), (int) (count - pos - 1), DefaultComparison<T>::compare, asc);
        }

        static void sort(T array[], size_t count, Comparison comparison, bool asc = true) {
            sort(array, 0, count, comparison, asc);
        }

        static void sort(T array[], size_t pos, size_t count, Comparison comparison, bool asc = true) {
            quicksort(array, (int) (pos), (int) (count - pos - 1), comparison, asc);
        }

        static void sort(T array[], size_t count, const IComparer<T> &comparer, bool asc = true) {
            sort(array, 0, count, comparer, asc);
        }

        static void sort(T array[], size_t pos, size_t count, const IComparer<T> &comparer, bool asc = true) {
            quicksort(array, (int) (pos), (int) (count - pos - 1), comparer, asc);
        }

    private:
        template<typename type>
        struct DefaultComparison {
            static int compare(const type &x, const type &y) {
                if (x > y)
                    return 1;
                else if (x < y)
                    return -1;
                else
                    return 0;
            }
        };

        template<typename type>
        struct DefaultComparison<type *> {
            static int compare(type *const &x, type *const &y) {
                if (*x > *y)
                    return 1;
                else if (*x < *y)
                    return -1;
                else
                    return 0;
            }
        };

        static void quicksort(T array[], int left, int right, const IComparer<T> &comparer, bool asc = true) {
            if (left >= right) {
                return;
            }
            int first = left;
            int last = right;
            T key = array[first];

            while (first < last) {
                if (asc) {
                    while (first < last && comparer.compare(array[last], key) >= 0) {
                        --last;
                    }
                } else {
                    while (first < last && comparer.compare(array[last], key) <= 0) {
                        --last;
                    }
                }

                array[first] = array[last];

                if (asc) {
                    while (first < last && comparer.compare(array[first], key) <= 0) {
                        ++first;
                    }
                } else {
                    while (first < last && comparer.compare(array[first], key) >= 0) {
                        ++first;
                    }
                }

                array[last] = array[first];
            }
            array[first] = key;
            quicksort(array, left, first - 1, comparer, asc);
            quicksort(array, first + 1, right, comparer, asc);
        }

        static void quicksort(T array[], int left, int right, Comparison comparison, bool asc = true) {
            if (left >= right) {
                return;
            }
            int first = left;
            int last = right;
            T key = array[first];

            while (first < last) {
                if (asc) {
                    while (first < last && comparison(array[last], key) >= 0) {
                        --last;
                    }
                } else {
                    while (first < last && comparison(array[last], key) <= 0) {
                        --last;
                    }
                }

                array[first] = array[last];

                if (asc) {
                    while (first < last && comparison(array[first], key) <= 0) {
                        ++first;
                    }
                } else {
                    while (first < last && comparison(array[first], key) >= 0) {
                        ++first;
                    }
                }

                array[last] = array[first];
            }
            array[first] = key;
            quicksort(array, left, first - 1, comparison, asc);
            quicksort(array, first + 1, right, comparison, asc);
        }
    };

    template<typename type>
    class ICollection {
    public:
        virtual ~ICollection() {
        }

        virtual size_t count() const = 0;

        virtual type *data() const = 0;

        virtual void add(const type &value) = 0;

        virtual bool insert(size_t pos, const type &value) = 0;

        virtual bool set(size_t pos, const type &value) = 0;

        virtual bool remove(const type &value) = 0;

        virtual bool removeAt(size_t pos) = 0;

        virtual void clear() = 0;

        virtual bool contains(const type &value) const = 0;

        virtual ssize_t indexOf(const type &value) const = 0;
    };

    template<typename T>
    static constexpr bool _isRelocatable() {
#if defined(CC_CLANG) || !defined(CC_GNU) || CC_GNU >= 501
        return std::is_trivially_copyable<T>::value && std::is_trivially_destructible<T>::value;
#else
        return std::is_enum<T>::value || std::is_integral<T>::value;
#endif
    }

    template<typename T>
    static constexpr bool _isTrivial() {
#if defined(CC_CLANG) || !defined(CC_GNU) || CC_GNU >= 501
        return std::is_trivial<T>::value;
#else
        return std::is_enum<T>::value || std::is_integral<T>::value;
#endif
    }

    /*
  The catch-all template.
*/
    template<typename T>
    class TypeInfo {
    public:
        enum {
            isSpecialized = std::is_enum<T>::value, // don't require every enum to be marked manually
            isPointer = false,
            isIntegral = std::is_integral<T>::value,
            isComplex = !_isTrivial<T>(),
            isStatic = true,
            isRelocatable = _isRelocatable<T>(),
            isLarge = (sizeof(T) > sizeof(void *)),
            sizeOf = sizeof(T)
        };
    };

    template<>
    class TypeInfo<void> {
    public:
        enum {
            isSpecialized = true,
            isPointer = false,
            isIntegral = false,
            isComplex = false,
            isStatic = false,
            isRelocatable = false,
            isLarge = false,
            sizeOf = 0
        };
    };

    template<typename T>
    class TypeInfo<T *> {
    public:
        enum {
            isSpecialized = true,
            isPointer = true,
            isIntegral = false,
            isComplex = false,
            isStatic = false,
            isRelocatable = true,
            isLarge = false,
            sizeOf = sizeof(T *)
        };
    };
}

#endif /* PrimitiveInterface_h */
