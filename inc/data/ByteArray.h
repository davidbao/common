//
//  ByteArray.h
//  common
//
//  Created by baowei on 2016/10/5.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef ByteArray_h
#define ByteArray_h

#include "data/Vector.h"
#include "data/ValueType.h"
#include "exception/Exception.h"
#include "IO/Stream.h"

namespace Common {
    class ByteArray : public Vector<uint8_t> {
    public:
        explicit ByteArray(size_t capacity = DefaultCapacity);

        explicit ByteArray(const Vector<uint8_t> &array);

        ByteArray(const Vector<uint8_t> &array, off_t offset, size_t count);

        ByteArray(const uint8_t *array, size_t count, size_t capacity = DefaultCapacity);

        ByteArray(const uint8_t &value, size_t count);

        ByteArray(const ByteArray &array);

        ByteArray(const ByteArray &array, off_t offset, size_t count);

        ByteArray(std::initializer_list<uint8_t> list);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const char *format = HexFormat, const char *splitStr = "-") const;

        String toLimitString(size_t length = 128, const char *format = HexFormat, const char *splitStr = "-") const;

        String toHexString() const;

        ssize_t find(const ByteArray &array) const;

        ssize_t find(off_t offset, size_t count, const ByteArray &array) const;

        ByteArray replace(off_t offset, size_t count, const ByteArray &src, const ByteArray &dst) const;

        ByteArray replace(const ByteArray &src, const ByteArray &dst) const;

        ByteArray &operator=(const ByteArray &value);

    public:
        static bool parse(const String &str, ByteArray &array, const char *splitStr = "-");

        static bool parseHexString(const String &str, ByteArray &array);

        static ssize_t find(const ByteArray &buffer, const ByteArray &array);

        static ssize_t find(const ByteArray &buffer, off_t offset, size_t count, const ByteArray &array);

        static ByteArray
        replace(const ByteArray &buffer, off_t offset, size_t count, const ByteArray &src, const ByteArray &dst);

        static ByteArray replace(const ByteArray &buffer, const ByteArray &src, const ByteArray &dst);

    public:
        static const ByteArray Empty;

        static const char *HexFormat;
        static const char *DecFormat;
        static const char *OtcFormat;
    };
}
#endif // ByteArray_h
