//
//  StringArray.cpp
//  common
//
//  Created by baowei on 2017/1/31.
//  Copyright © 2017 com. All rights reserved.
//

#include <cstdarg>
#include "data/StringArray.h"
#include "data/Convert.h"
#include "data/String.h"
#include "IO/Stream.h"

namespace Data {
    const StringArray StringArray::Empty;

    StringArray::StringArray(size_t capacity) : SortedVector<String>(capacity) {
    }

    StringArray::StringArray(const StringArray &array) = default;

    StringArray::StringArray(StringArray &&array) noexcept: SortedVector<String>(std::move(array)) {
    }

    StringArray::StringArray(const StringArray &array, off_t offset, size_t count) : SortedVector<String>(array, offset,
                                                                                                          count) {
    }

    StringArray::StringArray(const String *array, size_t count, size_t capacity) : SortedVector<String>(array, count,
                                                                                                        capacity) {
    }

    StringArray::StringArray(const String &value, size_t count) : SortedVector<String>(value, count) {
    }

    StringArray::StringArray(std::initializer_list<String> list) : SortedVector<String>(list) {
    }

    StringArray::StringArray(const char *item, ...) : StringArray(DefaultCapacity) {
        const char *str = item;
        va_list ap;
        va_start(ap, item);
        while (str != nullptr && str[0] != '\0') {
            add(str);
            str = va_arg(ap, const char*);
        }
        va_end(ap);
    }

    void StringArray::addArray(const char *item, ...) {
        const char *str = item;
        va_list ap;
        va_start(ap, item);
        while (str != nullptr && str[0] != '\0') {
            add(str);
            str = va_arg(ap, const char*);
        }
        va_end(ap);
    }

    bool StringArray::contains(const String &str, bool ignoreCase) const {
        for (size_t i = 0; i < count(); i++) {
            if (String::equals(data()[i], str, ignoreCase)) {
                return true;
            }
        }
        return false;
    }

    bool StringArray::remove(const String &str, bool ignoreCase) {
        for (ssize_t i = count() - 1; i >= 0; i--) {
            if (String::equals(data()[i], str, ignoreCase)) {
                return removeAt(i);
            }
        }
        return false;
    }

    void StringArray::write(Stream *stream, CountLength countLength, String::StreamLength streamLength) const {
        size_t c = count();
        switch (countLength) {
            case CountLength1:
                stream->writeByte((uint8_t) c);
                break;
            case CountLength2:
                stream->writeUInt16((uint16_t) c);
                break;
            case CountLength4:
                stream->writeUInt32((uint32_t) c);
                break;
            default:
                break;
        }
        for (size_t i = 0; i < c; i++) {
            String value = this->at(i);
            stream->writeStr(value, streamLength);
        }
    }

    void StringArray::read(Stream *stream, CountLength countLength, String::StreamLength streamLength) {
        clear();

        size_t c = count();
        switch (countLength) {
            case CountLength1:
                c = stream->readByte();
                break;
            case CountLength2:
                c = stream->readUInt16();
                break;
            case CountLength4:
                c = stream->readUInt32();
                break;
            default:
                break;
        }
        for (size_t i = 0; i < c; i++) {
            String value = stream->readStr(streamLength);
            add(value);
        }
    }

    String StringArray::toString(const char &symbol) const {
        String str;
        for (size_t i = 0; i < count(); i++) {
            if (str.length() > 0)
                str.append(symbol);
            str.append(at(i));
        }
        return str;
    }

    bool StringArray::parse(const String &str, StringArray &texts, const char &splitSymbol) {
        Convert::splitStr(str, texts, splitSymbol);
        return true;
    }

    bool StringArray::parseMultiSymbol(const String &str, StringArray &texts, const char &splitSymbol1,
                                       const char &splitSymbol2, const char &splitSymbol3) {
        if (splitSymbol1 != '\0' && str.find(splitSymbol1) >= 0)
            StringArray::parse(str, texts, splitSymbol1);
        else if (splitSymbol2 != '\0' && str.find(splitSymbol2) >= 0)
            StringArray::parse(str, texts, splitSymbol2);
        else if (splitSymbol3 != '\0' && str.find(splitSymbol3) >= 0)
            StringArray::parse(str, texts, splitSymbol3);
        return StringArray::parse(str, texts, splitSymbol1);
    }

    StringArray &StringArray::operator=(const String &str) {
        clear();
        parse(str, *this);
        return *this;
    }

    StringArray &StringArray::operator=(const char *str) {
        if (str != nullptr) {
            this->operator=((String) str);
        }
        return *this;
    }

    StringArray &StringArray::operator=(const StringArray &value) {
        this->evaluates(value);
        return *this;
    }
}
