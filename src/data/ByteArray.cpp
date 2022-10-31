//
//  ByteArray.h
//  common
//
//  Created by baowei on 2016/10/5.
//  Copyright © 2016 com. All rights reserved.
//

#include "data/ByteArray.h"
#include "system/Math.h"

namespace Common {
    const ByteArray ByteArray::Empty = ByteArray();
    const char *ByteArray::HexFormat = "%02X";
    const char *ByteArray::DecFormat = "%02d";
    const char *ByteArray::OtcFormat = "%02o";

    ByteArray::ByteArray(size_t capacity) : Array<uint8_t>(capacity) {
    }

    ByteArray::ByteArray(const Array<uint8_t> &array) : Array<uint8_t>(array) {
    }

    ByteArray::ByteArray(const Array<uint8_t> &array, off_t offset, size_t count) : Array<uint8_t>(array, offset,
                                                                                                   count) {
    }

    ByteArray::ByteArray(const uint8_t *array, size_t count, size_t capacity) : Array<uint8_t>(array, count, capacity) {
    }

    ByteArray::ByteArray(const uint8_t value, size_t count) : Array(value, count) {
    }

    ByteArray::ByteArray(const ByteArray &array) : Array<uint8_t>(array) {
    }

    ByteArray::ByteArray(const ByteArray &value, off_t offset, size_t count) : Array<uint8_t>(value, offset, count) {
    }

    void ByteArray::write(Stream *stream, bool bigEndian) const {
        uint32_t c = (uint32_t) count();
        stream->writeUInt32(c, bigEndian);
        if (c > 0) {
            stream->write(data(), 0, c);
        }
    }

    void ByteArray::read(Stream *stream, bool bigEndian) {
        uint32_t c = stream->readUInt32(bigEndian);
        if (c > 0) {
            const size_t size = 65535;
            for (size_t i = 0; i < c / size + 1; i++) {
                size_t cc = i == c / size ? (c % size) : size;
                uint8_t *buffer = new uint8_t[cc];
                stream->read(buffer, 0, cc);
                addRange(buffer, 0, cc);
                delete[] buffer;
            }
        }
    }

    const String ByteArray::toString(const char *format, const char *splitStr) const {
        String result;
        char temp[32];
        memset(temp, 0, sizeof(temp));
        for (size_t i = 0; i < count(); i++) {
            if (i > 0) {
                result.append(splitStr);
            }
            sprintf(temp, format, at(i));
            result.append(temp);
        }
        return result;
    }

    const String ByteArray::toLimitString(size_t length, const char *format, const char *splitStr) const {
        String str;
        if (count() > length) {
            ByteArray array(data(), Math::min(length, count()));
            str = array.toString(format, splitStr);
        } else {
            str = toString(format, splitStr);
        }
        return str;
    }

    const String ByteArray::toHexString() const {
        return toString(ByteArray::HexFormat, String::Empty);
    }

    bool ByteArray::parse(const String &str, ByteArray &array, const char *splitStr) {
        const char *format = "%02X%n";
        const int formatLen = 2;
        size_t splitStrLen = splitStr != nullptr ? strlen(splitStr) : 0;

        const char *buffer = str.c_str();
        size_t length = str.length();
        size_t index = 0;
        ByteArray temp;
        char valueStr[8];
        memset(valueStr, 0, sizeof(valueStr));
        char splitValueStr[8];
        memset(splitValueStr, 0, sizeof(splitValueStr));
        while (index < length) {
            strncpy(valueStr, &buffer[index], formatLen);
            int value;
            uint len = 0;
            if (sscanf(valueStr, format, &value, &len) == 1 && formatLen == len && value <= 255) {
                temp.add((uint8_t) value);
            } else {
                return false;
            }
            index += formatLen;

            if (splitStrLen > 0 && index < length) {
                strncpy(splitValueStr, &buffer[index], splitStrLen);
                if (strcmp(splitValueStr, splitStr) != 0) {
                    return false;
                }
                index += splitStrLen;
            }
        }

        array.addRange(temp);
        return array.count() > 0;
    }

    bool ByteArray::parseHexString(const String &str, ByteArray &array) {
        return parse(str, array, String::Empty);
    }

    ssize_t ByteArray::find(const ByteArray &array) const {
        return find(0, count(), array);
    }

    ssize_t ByteArray::find(off_t offset, size_t count, const ByteArray &array) const {
        return ByteArray::find(*this, offset, count, array);
    }

    ssize_t ByteArray::find(const ByteArray &buffer, const ByteArray &array) {
        return find(buffer, 0, buffer.count(), array);
    }

    ssize_t ByteArray::find(const ByteArray &buffer, off_t offset, size_t count, const ByteArray &array) {
        if (offset < 0) {
            return -1;
        }
        if ((buffer.count() - offset) < count) {
            return -1;
        }
        if (array.count() == 0) {
            return -1;
        }

        const uint8_t *first = buffer.data() + offset;
        const uint8_t *src = buffer.data() + offset;
        const uint8_t *dst = array.data();
        ssize_t index = 0;
        ssize_t n = 0;
        while (index < (ssize_t)count) {
            for (n = 0; *(src + n) == *(dst + n); n++)
                if (!*(dst + n + 1)) {
                    return (ssize_t) (src - first + offset);
                }
            src++;
            index++;
        }
        return -1;
    }

    ByteArray ByteArray::replace(off_t offset, size_t count, const ByteArray &src, const ByteArray &dst) {
        return replace(*this, offset, count, src, dst);
    }

    ByteArray ByteArray::replace(const ByteArray &src, const ByteArray &dst) {
        return replace(0, count(), src, dst);
    }

    ByteArray ByteArray::replace(const ByteArray &buffer, off_t offset, size_t count, const ByteArray &src,
                                 const ByteArray &dst) {
        if (offset < 0) {
            return buffer;
        }
        if ((buffer.count() - offset) < count) {
            return buffer;
        }
        if (src.count() == 0) {
            return buffer;
        }
        if (dst.count() == 0) {
            return buffer;
        }

        // Fixed it only use one loop in the future.
        ByteArray first(buffer, 0, offset);
        ByteArray middle(buffer, offset, count);
        ByteArray end(buffer, count + offset, buffer.count() - count - offset);
        ssize_t pos;
        do {
            pos = ByteArray::find(middle, src);
            if (pos >= 0) {
                middle.removeRange(pos, src.count());
                middle.insertRange(pos, dst);
            }
        } while (pos >= 0);
        ByteArray result;
        result.addRange(first);
        result.addRange(middle);
        result.addRange(end);
        return result;
    }

    ByteArray ByteArray::replace(const ByteArray &buffer, const ByteArray &src, const ByteArray &dst) {
        return replace(buffer, 0, buffer.count(), src, dst);
    }

    ByteArray &ByteArray::operator=(const ByteArray &array) {
        evaluates(array);
        return *this;
    }
}
