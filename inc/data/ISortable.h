//
//  ISortable.h
//  common
//
//  Created by baowei on 2022/8/31.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef ISortable_h
#define ISortable_h

#include "data/PrimitiveInterface.h"

namespace Data {
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

        virtual ~ISortable() = default;

        virtual size_t count() const = 0;

        virtual T *data() = 0;

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

    template<typename T>
    class ISortable<T *> {
    public:
        // A signed integer that indicates the relative values of x and y, as shown in the following table.
        // Value	            Meaning
        // Less than 0	        x is less than y.
        // 0	                x equals y.
        // Greater than 0	    x is greater than y.
        typedef int(*Comparison)(const T &x, const T &y);

        virtual ~ISortable() = default;

        virtual size_t count() const = 0;

        virtual T **data() = 0;

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
        static void sort(T *array[], size_t count, bool asc = true) {
            sort(array, 0, count, asc);
        }

        // T must be override operator< and operator>, IComparable is better.
        static void sort(T *array[], size_t pos, size_t count, bool asc = true) {
            quicksort(array, (int) (pos), (int) (count - pos - 1), DefaultComparison<T>::compare, asc);
        }

        static void sort(T *array[], size_t count, Comparison comparison, bool asc = true) {
            sort(array, 0, count, comparison, asc);
        }

        static void sort(T *array[], size_t pos, size_t count, Comparison comparison, bool asc = true) {
            quicksort(array, (int) (pos), (int) (count - pos - 1), comparison, asc);
        }

        static void sort(T *array[], size_t count, const IComparer<T> &comparer, bool asc = true) {
            sort(array, 0, count, comparer, asc);
        }

        static void sort(T *array[], size_t pos, size_t count, const IComparer<T> &comparer, bool asc = true) {
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

        static void quicksort(T *array[], int left, int right, const IComparer<T> &comparer, bool asc = true) {
            if (left >= right) {
                return;
            }
            int first = left;
            int last = right;
            T key = *array[first];

            while (first < last) {
                if (asc) {
                    while (first < last && comparer.compare(*array[last], key) >= 0) {
                        --last;
                    }
                } else {
                    while (first < last && comparer.compare(*array[last], key) <= 0) {
                        --last;
                    }
                }

                *array[first] = *array[last];

                if (asc) {
                    while (first < last && comparer.compare(*array[first], key) <= 0) {
                        ++first;
                    }
                } else {
                    while (first < last && comparer.compare(*array[first], key) >= 0) {
                        ++first;
                    }
                }

                *array[last] = *array[first];
            }
            *array[first] = key;
            quicksort(array, left, first - 1, comparer, asc);
            quicksort(array, first + 1, right, comparer, asc);
        }

        static void quicksort(T *array[], int left, int right, Comparison comparison, bool asc = true) {
            if (left >= right) {
                return;
            }
            int first = left;
            int last = right;
            T key = *array[first];

            while (first < last) {
                if (asc) {
                    while (first < last && comparison(*array[last], key) >= 0) {
                        --last;
                    }
                } else {
                    while (first < last && comparison(*array[last], key) <= 0) {
                        --last;
                    }
                }

                *array[first] = *array[last];

                if (asc) {
                    while (first < last && comparison(*array[first], key) <= 0) {
                        ++first;
                    }
                } else {
                    while (first < last && comparison(*array[first], key) >= 0) {
                        ++first;
                    }
                }

                *array[last] = *array[first];
            }
            *array[first] = key;
            quicksort(array, left, first - 1, comparison, asc);
            quicksort(array, first + 1, right, comparison, asc);
        }
    };
}

#endif // ISortable_h
