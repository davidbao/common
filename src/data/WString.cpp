//
//  ValueType.cpp
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#include <ctype.h>
#include <stdarg.h>
#include <inttypes.h>
#include <locale.h>
#include "data/WString.h"
#include "data/ByteArray.h"
#include "IO/Stream.h"
#include "system/Math.h"

namespace Common {
//#if WIN32
//    WString WString::NewLine = L"\r\n";
//#else
    const WString WString::NewLine = L"\n";
//#endif
    const WString WString::Empty = L"";
    const WString WString::NA = L"N/A";     // Not applicable

    WString::WString(uint capacity) : _buffer(capacity) {
    }

    WString::WString(const WString &value) {
        this->operator=(value);
    }

    WString::WString(const wstring &value) : WString(value.c_str()) {
    }

//    WString::WString(const String &value) {
//        setString(WString::convert(value));
//    }

    WString::WString(const wchar_t *value, size_t count) {
        if (value != nullptr)
            setString(value, count == 0 ? wcslen(value) : count);
    }

    WString::WString(wchar_t ch, size_t count) : WString() {
        for (size_t i = 0; i < count; i++) {
            addString(ch);
        }
    }

    WString::~WString() {
    }

    void WString::setString(const wchar_t *value) {
        _buffer.clear();
        addString(value);
    }

    void WString::setString(const wchar_t *value, size_t count) {
        _buffer.clear();
        addString(value, count);
    }

    void WString::setString(const wstring &value) {
        setString(value.c_str());
    }

    void WString::addString(const wchar_t *value) {
        if (value != nullptr) {
            addString(value, (size_t) wcslen(value));
        }
    }

    void WString::addString(const wchar_t *value, size_t count) {
        if (value != nullptr) {
            size_t length = Math::min(wcslen(value), count);
            if (length > 0) {
                if (isNullOrEmpty()) {
                    _buffer.clear();
                    _buffer.addRange(value, length);
                } else {
                    // replace '\0' position.
                    _buffer.set(_buffer.count() - 1, value[0]);
                    _buffer.addRange(value + 1, length - 1);
                }
                _buffer.add('\0');
            }
        }
    }

    void WString::addString(const wstring &value) {
        addString(value.c_str());
    }

    void WString::setString(wchar_t value) {
        wchar_t buffer[2] = {value, '\0'};
        setString(buffer);
    }

    void WString::addString(wchar_t value) {
        wchar_t buffer[2] = {value, '\0'};
        addString(buffer);
    }

    const wchar_t *WString::getString() const {
        return _buffer.data();
    }

    WString WString::operator+=(const WString &value) {
        append(value.getString());
        return *this;
    }

    WString WString::operator+=(const wstring &value) {
        append(value);
        return *this;
    }

    WString WString::operator+=(const wchar_t *value) {
        append(value);
        return *this;
    }

    WString WString::operator+(const WString &value) const {
        WString result = getString();
        result.append(value.getString());
        return result;
    }

    WString WString::operator+(const wstring &value) const {
        WString result = getString();
        result.append(value);
        return result;
    }

    WString WString::operator+(const wchar_t *value) const {
        WString result = getString();
        result.append(value);
        return result;
    }

    WString &WString::operator=(const WString &value) {
        setString(value.getString());
        return *this;
    }

    WString &WString::operator=(const wstring &value) {
        setString(value.c_str());
        return *this;
    }

    WString &WString::operator=(const wchar_t *value) {
        setString(value);
        return *this;
    }

    WString &WString::operator=(const String &value) {
        setString(convert(value));
        return *this;
    }

    bool WString::operator==(const WString &value) const {
        return equals(*this, value, false);
    }

    bool WString::operator!=(const WString &value) const {
        return !operator==(value);
    }

    bool WString::operator==(const wchar_t *value) const {
        return equals(*this, value, false);
    }

    bool WString::operator!=(const wchar_t *value) const {
        return !operator==(value);
    }

    bool WString::operator==(const wstring &value) const {
        return equals(*this, value, false);
    }

    bool WString::operator!=(const wstring &value) const {
        return !operator==(value);
    }

    bool WString::operator>(const WString &value) const {
        return compare(*this, value) > 0;
    }

    bool WString::operator>(const wstring &value) const {
        return compare(*this, value) > 0;
    }

    bool WString::operator>(const wchar_t *value) const {
        return compare(*this, value) > 0;
    }

    bool WString::operator>=(const WString &value) const {
        return compare(*this, value) >= 0;
    }

    bool WString::operator>=(const wstring &value) const {
        return compare(*this, value) >= 0;
    }

    bool WString::operator>=(const wchar_t *value) const {
        return compare(*this, value) >= 0;
    }

    bool WString::operator<(const WString &value) const {
        return compare(*this, value) < 0;
    }

    bool WString::operator<(const wstring &value) const {
        return compare(*this, value) < 0;
    }

    bool WString::operator<(const wchar_t *value) const {
        return compare(*this, value) < 0;
    }

    bool WString::operator<=(const WString &value) const {
        return compare(*this, value) <= 0;
    }

    bool WString::operator<=(const wstring &value) const {
        return compare(*this, value) <= 0;
    }

    bool WString::operator<=(const wchar_t *value) const {
        return compare(*this, value) <= 0;
    }

    wchar_t &WString::at(size_t pos) {
        return _buffer.at(pos);
    }

    wchar_t WString::at(size_t pos) const {
        return _buffer.at(pos);
    }

    bool WString::set(size_t pos, const wchar_t &value) {
        return _buffer.set(pos, value);
    }

    const wchar_t *WString::c_str() const {
        return getString();
    }

    wchar_t WString::front() const {
        return at(0);
    }

    wchar_t &WString::front() {
        return at(0);
    }

    wchar_t WString::back() const {
        return at(length() - 1);
    }

    wchar_t &WString::back() {
        return at(length() - 1);
    }

    const WString &WString::toString() const {
        return *this;
    }

    bool WString::equals(const WString &value1, const WString &value2, bool ignoreCase) {
        return compare(value1, value2, ignoreCase) == 0;
    }

    int WString::compare(const WString &value1, const WString &value2, bool ignoreCase) {
        if (!ignoreCase) {
            return wcscoll(value1.getString(), value2.getString());
        } else {
#ifdef WIN32
#define wcscasecmp wcsicmp
#endif
            return wcscasecmp(value1.getString(), value2.getString());
        }
    }

    void WString::write(Stream *stream, StreamLength streamLength) const {
        size_t len = length() * sizeof(wchar_t);
        if (streamLength == StreamLength1) {
            stream->writeByte((uint8_t) len);
        } else if (streamLength == StreamLength2) {
            stream->writeUInt16((uint16_t) len);
        } else if (streamLength == StreamLength4) {
            stream->writeUInt32((uint32_t) len);
        } else {
            assert(false);
            return;
        }
        writeFixedLengthStr(stream, length());
    }

    void WString::read(Stream *stream, StreamLength streamLength) {
        size_t len = 0;
        if (streamLength == StreamLength1) {
            len = stream->readByte();
        } else if (streamLength == StreamLength2) {
            len = stream->readUInt16();
        } else if (streamLength == StreamLength4) {
            len = stream->readUInt32();
        } else {
            assert(false);
            return;
        }
        readFixedLengthStr(stream, len / sizeof(wchar_t));
    }

    void WString::writeFixedLengthStr(Stream *stream, size_t length) const {
        length *= sizeof(wchar_t);
        size_t len = this->length() * sizeof(wchar_t);
        if (len < length) {
            stream->write((uint8_t *) getString(), 0, len);
            size_t count = length - len;
            static const size_t capacity = 65535 * sizeof(wchar_t);
            wchar_t buffer[capacity];
            wmemset(buffer, 0, capacity);
            size_t size = (count / capacity) + 1;
            for (size_t i = 0; i < size; i++) {
                stream->write((uint8_t *) buffer, 0, i < size - 1 ? capacity : (count % capacity));
            }
        } else {
            stream->write((uint8_t *) getString(), 0, length);
        }
    }

    void WString::readFixedLengthStr(Stream *stream, size_t length) {
        length *= sizeof(wchar_t);
        static const size_t capacity = 65535 * sizeof(wchar_t);
        wchar_t buffer[capacity];
        wmemset(buffer, 0, capacity);
        size_t size = (length / capacity) + 1;
        for (size_t i = 0; i < size; i++) {
            size_t len = stream->read((uint8_t *) buffer, 0, i < size - 1 ? capacity : (length % capacity));
            addString(buffer, len / sizeof(wchar_t));
        }
    }

    WString WString::toLower() const {
        if (isNullOrEmpty())
            return *this;

        const wchar_t *str = this->getString();
        size_t len = length();
        wchar_t *result = new wchar_t[len + 1];
        wmemset(result, 0, len + 1);
        for (size_t i = 0; i < len; i++) {
            result[i] = (wchar_t) tolower(str[i]);
        }
        WString resultStr = result;
        delete[] result;
        return resultStr;
    }

    WString WString::toUpper() const {
        if (isNullOrEmpty())
            return *this;

        const wchar_t *str = this->getString();
        size_t len = length();
        wchar_t *result = new wchar_t[len + 1];
        wmemset(result, 0, len + 1);
        for (size_t i = 0; i < len; i++) {
            result[i] = (wchar_t) toupper(str[i]);
        }
        WString resultStr = result;
        delete[] result;
        return resultStr;
    }

    bool WString::isNullOrEmpty() const {
        return WString::isNullOrEmpty(*this);
    }

    bool WString::isNullOrEmpty(const WString &value) {
        return value._buffer.count() <= 1;
    }

    size_t WString::length() const {
        if (isNullOrEmpty())
            return 0;
        return _buffer.count() - 1;
    }

    void WString::empty() {
        _buffer.clear();
    }

    WString::operator const wchar_t *() const {
        return getString();
    }

    WString::operator const String() const {
        return WString::convert(*this);
    }

    bool WString::equals(const WString &other) const {
        return WString::equals(*this, other);
    }

    void WString::evaluates(const WString &other) {
        setString(other);
    }

    int WString::compareTo(const WString &other) const {
        return WString::compare(*this, other);
    }

    int WString::compareTo(const WString &other, bool ignoreCase) const {
        return WString::compare(*this, other, ignoreCase);
    }

    WString
    WString::trim(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3, const wchar_t trimChar4) {
        return WString::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::trimStart(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                               const wchar_t trimChar4) {
        return WString::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::trimEnd(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                             const wchar_t trimChar4) {
        return WString::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString
    WString::trim(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                  const wchar_t trimChar4) {
        Vector<wchar_t> trimChars;
        const wchar_t temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimBoth);
    }

    WString
    WString::trimStart(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                       const wchar_t trimChar4) {
        Vector<wchar_t> trimChars;
        const wchar_t temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimHead);
    }

    WString
    WString::trimEnd(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                     const wchar_t trimChar4) {
        Vector<wchar_t> trimChars;
        const wchar_t temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimTail);
    }

    WString
    WString::trimInner(const WString &str, const Vector<wchar_t> &trimChars, TrimType trimType) {
        if (str.isNullOrEmpty())
            return str;

        //end will point to the first non-trimmed wchar_tacter on the right
        //start will point to the first non-trimmed wchar_tacter on the Left
        size_t end = str.length() - 1;
        size_t start = 0;

        //Trim specified wchar_tacters.
        if (trimType != TrimTail) {
            for (start = 0; start < str.length(); start++) {
                size_t i = 0;
                wchar_t ch = str[start];
                for (i = 0; i < trimChars.count(); i++) {
                    if (trimChars[i] == ch) break;
                }
                if (i == trimChars.count()) { // the wchar_tacter is not white space
                    break;
                }
            }
        }

        if (trimType != TrimHead) {
            for (end = str.length() - 1; end >= start; end--) {
                size_t i = 0;
                wchar_t ch = str[end];
                for (i = 0; i < trimChars.count(); i++) {
                    if (trimChars[i] == ch) break;
                }
                if (i == trimChars.count()) { // the wchar_tacter is not white space
                    break;
                }
            }
        }
        return str.substr((off_t) start, end - start + 1);
    }

    WString WString::GBKtoUTF8() const {
        return GBKtoUTF8(*this);
    }

    WString WString::UTF8toGBK() const {
        return UTF8toGBK(*this);
    }

    WString WString::GBKtoUTF8(const WString &value) {
        if (value.isNullOrEmpty())
            return value;
        if (WString::isUTF8(value))
            return value;

        return encoding(L"GBK", L"UTF-8", value.getString());
    }

    WString WString::UTF8toGBK(const WString &value) {
        if (value.isNullOrEmpty())
            return value;
        if (!WString::isUTF8(value))
            return value;

        return encoding(L"UTF-8", L"GBK", value.getString());
    }

    WString WString::encoding(const wchar_t *fromCode, const wchar_t *toCode, const wchar_t *str) {
        if (str == nullptr || wcslen(str) == 0)
            return Empty;

        String fromStr = WString(fromCode);
        String toStr = WString(toCode);
        String temp = WString(str);
        if (String::encoding(fromStr, toStr, temp)) {
            return WString(temp);
        }
        return Empty;
    }

    bool WString::isUTF8(const WString &str) {
        size_t i, ix, c, n, j;
        for (i = 0, ix = str.length(); i < ix; i++) {
            c = (wchar_t) str[i];
            //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
            if (0x00 <= c && c <= 0x7f) n = 0; // 0bbbbbbb
            else if ((c & 0xE0) == 0xC0) n = 1; // 110bbbbb
            else if (c == 0xed && i < (ix - 1) && ((wchar_t) str[i + 1] & 0xa0) == 0xa0)
                return false; //U+d800 to U+dfff
            else if ((c & 0xF0) == 0xE0) n = 2; // 1110bbbb
            else if ((c & 0xF8) == 0xF0) n = 3; // 11110bbb
                //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //uint8_t 5, unnecessary in 4 uint8_t UTF-8
                //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //uint8_t 6, unnecessary in 4 uint8_t UTF-8
            else return false;
            for (j = 0; j < n && i < ix; j++) { // n bytes matching 10bbbbbb follow ?
                if ((++i == ix) || (((wchar_t) str[i] & 0xC0) != 0x80))
                    return false;
            }
        }
        return true;
    }

    bool WString::isUTF8() const {
        return WString::isUTF8(*this);
    }

    WString WString::toBase64() const {
        return WString::toBase64(*this);
    }

    WString WString::toBase64(const WString &value) {
        String str = value;
        return String::toBase64(str);
    }

    WString WString::fromBase64() const {
        return WString::fromBase64(*this);
    }

    WString WString::fromBase64(const WString &value) {
        String str = value;
        return String::fromBase64(str);
    }

    bool WString::fromBase64(const WString &value, ByteArray &array) {
        if (value.isNullOrEmpty())
            return false;

        String str = value;
        return String::fromBase64(str.c_str(), str.length(), array);
    }

    ssize_t WString::find(const WString &str, off_t offset) const {
        if (isNullOrEmpty())
            return -1;
        if (str.isNullOrEmpty())
            return -1;
        if (offset < 0)
            offset = 0;

        const wchar_t *current = &getString()[0];
        const wchar_t *substring = current + offset;
        const wchar_t *first = wcsstr(substring, str);
        if (nullptr == first) return -1;
        return (first - current);
    }

    ssize_t WString::find(const wchar_t ch, off_t offset) const {
        wchar_t buffer[2] = {ch, '\0'};
        return find(buffer, offset);
    }

    ssize_t WString::findLastOf(const WString &str) const {
        if (isNullOrEmpty())
            return -1;
        if (str.isNullOrEmpty())
            return -1;

        return ((wstring) getString()).find_last_of(str);
    }

    ssize_t WString::findLastOf(const wchar_t ch) const {
        wchar_t buffer[2] = {ch, '\0'};
        return findLastOf(buffer);
    }

    bool WString::contains(const WString &str) const {
        return find(str) != -1;
    }

    bool WString::contains(const wchar_t ch) const {
        wchar_t buffer[2] = {ch, '\0'};
        return contains(buffer);
    }

    void WString::append(const wchar_t ch) {
        addString(ch);
    }

    void WString::append(const wchar_t *str, size_t count) {
        addString(str, count);
    }

    void WString::append(const WString &str) {
        addString(str);
    }

    void WString::append(const WString &str, off_t offset, size_t count) {
        const wchar_t *buffer = str._buffer.data();
        append(buffer + offset, count);
    }

    void WString::appendLine(const wchar_t ch) {
        append(ch);
        append(NewLine);
    }

    void WString::appendLine(const wchar_t *str, size_t count) {
        append(str, count);
        append(NewLine);
    }

    void WString::appendLine(const WString &str) {
        append(str);
        append(NewLine);
    }

    void WString::appendLine(const WString &str, off_t offset, size_t count) {
        append(str, offset, count);
        append(NewLine);
    }

    WString WString::replace(const WString &src, const WString &dst) {
        WString str = replace(*this, src, dst);
        setString(str);
        return str;
    }

    WString WString::replace(const WString &str, const WString &src, const WString &dst) {
        if (str.isNullOrEmpty())
            return str;
        if (src.isNullOrEmpty())
            return str;
        if (src == dst)
            return str;

        wstring strBig = str.getString();
        std::wstring::size_type pos = 0;
        while ((pos = strBig.find(src, pos)) != wstring::npos) {
            const wchar_t *dstStr = dst.c_str();
            strBig.replace(pos, src.length(), dstStr != nullptr ? dstStr : L"");
            pos += dst.length();
        }
        return strBig;
    }

    WString WString::substr(off_t offset, size_t count) const {
        return substr(*this, offset, count);
    }

    WString WString::substr(off_t offset) const {
        return substr(offset, length() - offset);
    }

    WString WString::substr(const WString &str, off_t offset, size_t count) {
        if (str.isNullOrEmpty())
            return Empty;
        size_t strLength = str.length();
        if (offset < 0 || offset >= (int) strLength)
            return Empty;
        if (count > strLength)
            count = strLength;

        WString result;
        result._buffer.addRange(str.c_str(), offset, count);
        result._buffer.add('\0');
        return result;
    }

    WString WString::insert(off_t offset, const WString &str) {
        if (offset >= 0 && offset < (int) length()) {
            WString s1 = substr(0, offset);
            WString s2 = substr(offset, length() - offset);

            WString text = s1;
            text.append(str);
            text.append(s2);
            setString(text);

            return text;
        }
        return WString::Empty;
    }

    WString WString::insert(off_t offset, const wchar_t ch) {
        wchar_t buffer[2] = {ch, '\0'};
        return insert(offset, buffer);
    }

    bool WString::removeAt(size_t pos) {
        return _buffer.removeAt(pos);
    }

    bool WString::removeRange(size_t pos, size_t count) {
        return _buffer.removeRange(pos, count);
    }

    WString WString::trim(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                          const wchar_t trimChar4) const {
        return WString::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::trimStart(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                               const wchar_t trimChar4) const {
        return WString::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::trimEnd(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                             const wchar_t trimChar4) const {
        return WString::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::convert(const wchar_t *format, ...) {
        wchar_t *message = new wchar_t[MaxFormatStrLength];
        wmemset(message, 0, MaxFormatStrLength);
        va_list ap;
        va_start(ap, format);
        vswprintf(message, MaxFormatStrLength, format, ap);
        va_end(ap);
        WString result = message;
        delete[] message;
        return result;
    }

    WString WString::format(const wchar_t *format, ...) {
        wchar_t *message = new wchar_t[MaxFormatStrLength];
        wmemset(message, 0, MaxFormatStrLength);
        va_list ap;
        va_start(ap, format);
        vswprintf(message, MaxFormatStrLength, format, ap);
        va_end(ap);
        WString result = message;
        delete[] message;
        return result;
    }

    Iterator<wchar_t>::const_iterator WString::begin() const {
        return Iterator<wchar_t>::const_iterator(_buffer.data());
    }

    Iterator<wchar_t>::const_iterator WString::end() const {
        return Iterator<wchar_t>::const_iterator(_buffer.data() + length());
    }

    Iterator<wchar_t>::iterator WString::begin() {
        return Iterator<wchar_t>::iterator(_buffer.data());
    }

    Iterator<wchar_t>::iterator WString::end() {
        return Iterator<wchar_t>::iterator(_buffer.data() + length());
    }

    Iterator<wchar_t>::const_reverse_iterator WString::rbegin() const {
        return Iterator<wchar_t>::const_reverse_iterator(_buffer.data() + length() - 1);
    }

    Iterator<wchar_t>::const_reverse_iterator WString::rend() const {
        return Iterator<wchar_t>::const_reverse_iterator(_buffer.data() - 1);
    }

    Iterator<wchar_t>::reverse_iterator WString::rbegin() {
        return Iterator<wchar_t>::reverse_iterator(_buffer.data() + length() - 1);
    }

    Iterator<wchar_t>::reverse_iterator WString::rend() {
        return Iterator<wchar_t>::reverse_iterator(_buffer.data() - 1);
    }

    WString WString::convert(const String& str) {
        if (str.length() > 0) {
            std::string strLocale = setlocale(LC_ALL, "");
            const char *chSrc = str.c_str();
            size_t nDestSize = mbstowcs(nullptr, chSrc, 0) + 1;
            wchar_t *wchDest = new wchar_t[nDestSize];
            wmemset(wchDest, 0, nDestSize);
            mbstowcs(wchDest, chSrc, nDestSize);
            WString wstrResult = wchDest;
            delete[]wchDest;
            setlocale(LC_ALL, strLocale.c_str());
            return wstrResult;
        }
        return WString::Empty;
    }
    String WString::convert(const WString& str) {
        if (str.length() > 0) {
            const wchar_t *src = str.c_str();
            std::string strLocale = setlocale(LC_ALL, "");
            size_t nDestSize = wcstombs(nullptr, src, 0) + 1;
            char *chDest = new char[nDestSize];
            memset(chDest, 0, nDestSize);
            wcstombs(chDest, src, nDestSize);
            String strResult = chDest;
            delete[]chDest;
            setlocale(LC_ALL, strLocale.c_str());
            return strResult;
        }
        return String::Empty;
    }
}
