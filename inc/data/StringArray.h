//
//  StringArray.h
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#ifndef StringArray_h
#define StringArray_h

#include "data/Vector.h"
#include "data/String.h"
#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Dictionary.h"
#include "data/Uuid.h"

namespace IO {
    class Stream;
}
using namespace IO;

namespace Data {
    class StringArray : public SortedVector<String> {
    public:
        enum CountLength {
            CountLength1 = 1,
            CountLength2 = 2,
            CountLength4 = 4
        };

        explicit StringArray(size_t capacity = DefaultCapacity);

        StringArray(const StringArray &array);

        StringArray(StringArray &&array) noexcept;

        StringArray(const StringArray &array, off_t offset, size_t count);

        StringArray(const String *array, size_t count, size_t capacity = DefaultCapacity);

        StringArray(const String &value, size_t count);

        StringArray(std::initializer_list<String> list);

        explicit StringArray(const char *item, ...);

        void addArray(const char *item, ...);

        bool contains(const String &str, bool ignoreCase = false) const;

        bool remove(const String &str, bool ignoreCase = false);

        void write(Stream *stream, CountLength countLength = CountLength2,
                   String::StreamLength streamLength = String::StreamLength2) const;

        void read(Stream *stream, CountLength countLength = CountLength2,
                  String::StreamLength streamLength = String::StreamLength2);

        String toString(const char &symbol = ';') const;

        StringArray &operator=(const StringArray &value);

        StringArray &operator=(const String &str);

        StringArray &operator=(const char *str);

    public:
        static bool parse(const String &str, StringArray &texts, const char &splitSymbol = ';');

        static bool parseMultiSymbol(const String &str, StringArray &texts, const char &splitSymbol1 = ';',
                                     const char &splitSymbol2 = '\0', const char &splitSymbol3 = '\0');

    public:
        static const StringArray Empty;
    };
}

#endif    // StringArray_h
