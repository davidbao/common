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
#include "iconv.h"
#include "data/ValueType.h"
#include "data/ByteArray.h"
#include "system/Convert.h"
#include "IO/Stream.h"
#include "system/Math.h"
#ifdef WIN32
#include <Rpc.h>
#endif

namespace Common {
//#if WIN32
//    String String::NewLine = "\r\n";
//#else
    const String String::NewLine = "\n";
//#endif
    const String String::Empty = "";
    const String String::NA = "N/A";     // Not applicable

    const char String::base64Table[65] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                                          'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
                                          'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
                                          't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
                                          '8', '9', '+', '/', '='};

    String::String(uint capacity) : _buffer(capacity) {
    }

    String::String(const String &value) {
        this->operator=(value);
    }

    String::String(const string &value) : String(value.c_str()) {
    }

    String::String(const char *value, size_t count) {
        if (value != nullptr)
            setString(value, count == 0 ? strlen(value) : count);
    }

    String::String(char ch, size_t count) : String() {
        for (size_t i = 0; i < count; i++) {
            addString(ch);
        }
    }

    String::~String() {
    }

    void String::setString(const char *value) {
        _buffer.clear();
        addString(value);
    }

    void String::setString(const char *value, size_t count) {
        _buffer.clear();
        addString(value, count);
    }

    void String::setString(const string &value) {
        setString(value.c_str());
    }

    void String::addString(const char *value) {
        if (value != nullptr) {
            addString(value, (size_t) strlen(value));
        }
    }

    void String::addString(const char *value, size_t count) {
        if (value != nullptr) {
            size_t length = Math::min(strlen(value), count);
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

    void String::addString(const string &value) {
        addString(value.c_str());
    }

    void String::setString(char value) {
        char buffer[2] = {value, '\0'};
        setString(buffer);
    }

    void String::addString(char value) {
        char buffer[2] = {value, '\0'};
        addString(buffer);
    }

    const char *String::getString() const {
        return _buffer.data();
    }

    String String::operator+=(const String &value) {
        append(value.getString());
        return *this;
    }

    String String::operator+=(const string &value) {
        append(value);
        return *this;
    }

    String String::operator+=(const char *value) {
        append(value);
        return *this;
    }

    String String::operator+(const String &value) const {
        String result = getString();
        result.append(value.getString());
        return result;
    }

    String String::operator+(const string &value) const {
        String result = getString();
        result.append(value);
        return result;
    }

    String String::operator+(const char *value) const {
        String result = getString();
        result.append(value);
        return result;
    }

    String &String::operator=(const String &value) {
        setString(value.getString());
        return *this;
    }

    String &String::operator=(const string &value) {
        setString(value.c_str());
        return *this;
    }

    String &String::operator=(const char *value) {
        setString(value);
        return *this;
    }

    bool String::operator==(const String &value) const {
        return equals(*this, value, false);
    }

    bool String::operator!=(const String &value) const {
        return !operator==(value);
    }

    bool String::operator==(const char *value) const {
        return equals(*this, value, false);
    }

    bool String::operator!=(const char *value) const {
        return !operator==(value);
    }

    bool String::operator==(const string &value) const {
        return equals(*this, value, false);
    }

    bool String::operator!=(const string &value) const {
        return !operator==(value);
    }

    bool String::operator>(const String &value) const {
        return compare(*this, value) > 0;
    }

    bool String::operator>(const string &value) const {
        return compare(*this, value) > 0;
    }

    bool String::operator>(const char *value) const {
        return compare(*this, value) > 0;
    }

    bool String::operator>=(const String &value) const {
        return compare(*this, value) >= 0;
    }

    bool String::operator>=(const string &value) const {
        return compare(*this, value) >= 0;
    }

    bool String::operator>=(const char *value) const {
        return compare(*this, value) >= 0;
    }

    bool String::operator<(const String &value) const {
        return compare(*this, value) < 0;
    }

    bool String::operator<(const string &value) const {
        return compare(*this, value) < 0;
    }

    bool String::operator<(const char *value) const {
        return compare(*this, value) < 0;
    }

    bool String::operator<=(const String &value) const {
        return compare(*this, value) <= 0;
    }

    bool String::operator<=(const string &value) const {
        return compare(*this, value) <= 0;
    }

    bool String::operator<=(const char *value) const {
        return compare(*this, value) <= 0;
    }

    char &String::at(size_t pos) {
        return _buffer.at(pos);
    }

    char String::at(size_t pos) const {
        return _buffer.at(pos);
    }

    bool String::set(size_t pos, const char &value) {
        return _buffer.set(pos, value);
    }

    const char *String::c_str() const {
        return getString();
    }

    char String::front() const {
        return at(0);
    }

    char &String::front() {
        return at(0);
    }

    char String::back() const {
        return at(length() - 1);
    }

    char &String::back() {
        return at(length() - 1);
    }

    const String &String::toString() const {
        return *this;
    }

//    bool String::parse(const String &str, String &value) {
//        value = str;
//        return true;
//    }

    bool String::equals(const String &value1, const String &value2, bool ignoreCase) {
        return compare(value1, value2, ignoreCase) == 0;
    }

    int String::compare(const String &value1, const String &value2, bool ignoreCase) {
        if (!ignoreCase) {
            return strcoll(value1.getString(), value2.getString());
        } else {
#ifdef WIN32
#define strcasecmp stricmp
#endif
            return strcasecmp(value1.getString(), value2.getString());
        }
    }

    void String::write(Stream *stream, StreamLength streamLength) const {
        size_t len = length();
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
        stream->write((uint8_t *) getString(), 0, len);
    }

    void String::read(Stream *stream, StreamLength streamLength) {
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
        readFixedLengthStr(stream, len);
    }

    void String::writeFixedLengthStr(Stream *stream, size_t length) {
        size_t len = this->length();
        if (len < length) {
            stream->write((uint8_t *) getString(), 0, len);
            size_t count = length - len;
            static const size_t capacity = 65535;
            char buffer[capacity];
            memset(buffer, 0, capacity);
            size_t size = (count / capacity) + 1;
            for (size_t i = 0; i < size; i++) {
                stream->write((uint8_t *) buffer, 0, i < size - 1 ? capacity : (count % capacity));
            }
        } else {
            stream->write((uint8_t *) getString(), 0, length);
        }
    }

    void String::readFixedLengthStr(Stream *stream, size_t length) {
        static const size_t capacity = 65535;
        char buffer[capacity];
        memset(buffer, 0, capacity);
        size_t size = (length / capacity) + 1;
        for (size_t i = 0; i < size; i++) {
            size_t len = stream->read((uint8_t *) buffer, 0, i < size - 1 ? capacity : (length % capacity));
            addString(buffer, len);
        }
    }

    String String::toLower() const {
        if (isNullOrEmpty())
            return *this;

        const char *str = this->getString();
        size_t len = length();
        char *result = new char[len + 1];
        memset(result, 0, len + 1);
        for (size_t i = 0; i < len; i++) {
            result[i] = (char) tolower(str[i]);
        }
        String resultStr = result;
        delete[] result;
        return resultStr;
    }

    String String::toUpper() const {
        if (isNullOrEmpty())
            return *this;

        const char *str = this->getString();
        size_t len = length();
        char *result = new char[len + 1];
        memset(result, 0, len + 1);
        for (size_t i = 0; i < len; i++) {
            result[i] = (char) toupper(str[i]);
        }
        String resultStr = result;
        delete[] result;
        return resultStr;
    }

    bool String::isNullOrEmpty() const {
        return String::isNullOrEmpty(*this);
    }

    bool String::isNullOrEmpty(const String &value) {
        return value._buffer.count() <= 1;
    }

    size_t String::length() const {
        if (isNullOrEmpty())
            return 0;
        return _buffer.count() - 1;
    }

    void String::empty() {
        _buffer.clear();
    }

    String::operator const char *() const {
        return getString();
    }

    bool String::equals(const String &other) const {
        return String::equals(*this, other);
    }

    void String::evaluates(const String &other) {
        setString(other);
    }

    int String::compareTo(const String &other) const {
        return String::compare(*this, other);
    }

    int String::compareTo(const String &other, bool ignoreCase) const {
        return String::compare(*this, other, ignoreCase);
    }

    String String::trim(const char trimChar1, const char trimChar2, const char trimChar3, const char trimChar4) {
        return String::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimStart(const char trimChar1, const char trimChar2, const char trimChar3, const char trimChar4) {
        return String::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimEnd(const char trimChar1, const char trimChar2, const char trimChar3, const char trimChar4) {
        return String::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String
    String::trim(const String &str, const char trimChar1, const char trimChar2, const char trimChar3,
                 const char trimChar4) {
        Array<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimBoth);
    }

    String String::trimStart(const String &str, const char trimChar1, const char trimChar2, const char trimChar3,
                             const char trimChar4) {
        Array<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimHead);
    }

    String
    String::trimEnd(const String &str, const char trimChar1, const char trimChar2, const char trimChar3,
                    const char trimChar4) {
        Array<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (size_t i = 0; i < 4; i++) {
            if (temp[i] != '\0') {
                trimChars.add(temp[i]);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimTail);
    }

    String
    String::trimInner(const String &str, const Array<char> &trimChars, TrimType trimType) {
        if (str.isNullOrEmpty())
            return str;

        //end will point to the first non-trimmed character on the right
        //start will point to the first non-trimmed character on the Left
        size_t end = str.length() - 1;
        size_t start = 0;

        //Trim specified characters.
        if (trimType != TrimTail) {
            for (start = 0; start < str.length(); start++) {
                size_t i = 0;
                char ch = str[start];
                for (i = 0; i < trimChars.count(); i++) {
                    if (trimChars[i] == ch) break;
                }
                if (i == trimChars.count()) { // the character is not white space
                    break;
                }
            }
        }

        if (trimType != TrimHead) {
            for (end = str.length() - 1; end >= start; end--) {
                size_t i = 0;
                char ch = str[end];
                for (i = 0; i < trimChars.count(); i++) {
                    if (trimChars[i] == ch) break;
                }
                if (i == trimChars.count()) { // the character is not white space
                    break;
                }
            }
        }
        return str.substr((off_t) start, end - start + 1);
    }

    String String::GBKtoUTF8() const {
        return GBKtoUTF8(*this);
    }

    String String::UTF8toGBK() const {
        return UTF8toGBK(*this);
    }

    String String::GBKtoUTF8(const String &value) {
        if (value.isNullOrEmpty())
            return value;
        if (String::isUTF8(value))
            return value;

        return encoding("GBK", "UTF-8", value.getString());
    }

    String String::UTF8toGBK(const String &value) {
        if (value.isNullOrEmpty())
            return value;
        if (!String::isUTF8(value))
            return value;

        return encoding("UTF-8", "GBK", value.getString());
    }

    String String::encoding(const char *fromCode, const char *toCode, const char *str) {
        if (str == nullptr || strlen(str) == 0)
            return Empty;

        String result;
        size_t length = 0;
        char *buffer = new char[strlen(str) * 4];
        if (encoding(fromCode, toCode, str, buffer, length)) {
            result = buffer;
        } else {
            result = str;
        }
        delete[] buffer;
        return result;
    }

    bool
    String::encoding(const char *fromCode, const char *toCode, const char *str, char *&buffer, size_t &length) {
        if (str == nullptr || strlen(str) == 0)
            return false;

        length = 0;
        iconv_t cd = iconv_open(toCode, fromCode);
        if (cd != (iconv_t) - 1) {
            const char *inbuf = str;
            size_t inlen = strlen(inbuf);
            char *outbuf = (char *) malloc(inlen * 4);
            memset(outbuf, 0, inlen * 4);
            char *in = (char *) inbuf;
            char *out = outbuf;
            size_t outlen = inlen * 4;
            iconv(cd, &in, (size_t *) &inlen, &out, &outlen);
            outlen = strlen(outbuf);

            strcpy(buffer, outbuf);
            length = (int) outlen;

            free(outbuf);
            iconv_close(cd);
        }
        return length > 0;
    }

    bool String::isUTF8(const String &str) {
        size_t i, ix, c, n, j;
        for (i = 0, ix = str.length(); i < ix; i++) {
            c = (unsigned char) str[i];
            //if (c==0x09 || c==0x0a || c==0x0d || (0x20 <= c && c <= 0x7e) ) n = 0; // is_printable_ascii
            if (0x00 <= c && c <= 0x7f) n = 0; // 0bbbbbbb
            else if ((c & 0xE0) == 0xC0) n = 1; // 110bbbbb
            else if (c == 0xed && i < (ix - 1) && ((unsigned char) str[i + 1] & 0xa0) == 0xa0)
                return false; //U+d800 to U+dfff
            else if ((c & 0xF0) == 0xE0) n = 2; // 1110bbbb
            else if ((c & 0xF8) == 0xF0) n = 3; // 11110bbb
                //else if (($c & 0xFC) == 0xF8) n=4; // 111110bb //uint8_t 5, unnecessary in 4 uint8_t UTF-8
                //else if (($c & 0xFE) == 0xFC) n=5; // 1111110b //uint8_t 6, unnecessary in 4 uint8_t UTF-8
            else return false;
            for (j = 0; j < n && i < ix; j++) { // n bytes matching 10bbbbbb follow ?
                if ((++i == ix) || (((unsigned char) str[i] & 0xC0) != 0x80))
                    return false;
            }
        }
        return true;
    }

    bool String::isUTF8() const {
        return String::isUTF8(*this);
    }

    String String::toBase64() const {
        return String::toBase64(*this);
    }

    String String::toBase64(const String &value) {
        if (value.isNullOrEmpty())
            return value;

        return toBase64((const uint8_t *) value.getString(), 0, value.length());
    }

    size_t String::toBase64_CalculateAndValidateOutputLength(size_t inputLength, bool insertLineBreaks) {
        size_t outlen = (inputLength) / 3 * 4;          // the base length - we want integer division here.
        outlen += ((inputLength % 3) != 0) ? 4 : 0;         // at most 4 more chars for the remainder

        if (outlen == 0)
            return 0;

        if (insertLineBreaks) {
            ssize_t newLines = outlen / base64LineBreakPosition;
            if ((outlen % base64LineBreakPosition) == 0) {
                --newLines;
            }
            outlen += newLines * 2;              // the number of line break chars we'll add, "\r\n"
        }

        return outlen;
    }

    size_t String::convertToBase64Array(char *outChars, const uint8_t *inData, off_t offset, size_t length,
                                        bool insertLineBreaks) {
        size_t lengthmod3 = length % 3;
        size_t calcLength = offset + (length - lengthmod3);
        size_t j = 0;
        size_t charcount = 0;
        //Convert three bytes at a time to base64 notation.  This will consume 4 chars.
        size_t i;

        // get a pointer to the base64Table to avoid unnecessary range checking
        const char *base64 = base64Table;
        for (i = offset; i < calcLength; i += 3) {
            if (insertLineBreaks) {
                if (charcount == base64LineBreakPosition) {
                    outChars[j++] = '\r';
                    outChars[j++] = '\n';
                    charcount = 0;
                }
                charcount += 4;
            }
            outChars[j] = base64[(inData[i] & 0xfc) >> 2];
            outChars[j + 1] = base64[((inData[i] & 0x03) << 4) | ((inData[i + 1] & 0xf0) >> 4)];
            outChars[j + 2] = base64[((inData[i + 1] & 0x0f) << 2) | ((inData[i + 2] & 0xc0) >> 6)];
            outChars[j + 3] = base64[(inData[i + 2] & 0x3f)];
            j += 4;
        }

        //Where we left off before
        i = calcLength;

        if (insertLineBreaks && (lengthmod3 != 0) && (charcount == base64LineBreakPosition)) {
            outChars[j++] = '\r';
            outChars[j++] = '\n';
        }

        switch (lengthmod3) {
            case 2: //One character padding needed
                outChars[j] = base64[(inData[i] & 0xfc) >> 2];
                outChars[j + 1] = base64[((inData[i] & 0x03) << 4) | ((inData[i + 1] & 0xf0) >> 4)];
                outChars[j + 2] = base64[(inData[i + 1] & 0x0f) << 2];
                outChars[j + 3] = base64[64]; //Pad
                j += 4;
                break;
            case 1: // Two character padding needed
                outChars[j] = base64[(inData[i] & 0xfc) >> 2];
                outChars[j + 1] = base64[(inData[i] & 0x03) << 4];
                outChars[j + 2] = base64[64]; //Pad
                outChars[j + 3] = base64[64]; //Pad
                j += 4;
                break;
        }

        return j;
    }

    bool String::toBase64(const uint8_t *inArray, off_t offset, size_t length, String &str,
                          Base64FormattingOptions options) {
        //		int inA-rrayLength;
        size_t stringLength;

        //		inArrayLength = length;

        bool insertLineBreaks = (options == String::InsertLineBreaks);
        //Create the new string.  This is the maximally required length.
        stringLength = toBase64_CalculateAndValidateOutputLength(length, insertLineBreaks);
        if (stringLength > 0) {
            char *outChars = new char[stringLength + 1];
            memset(outChars, 0, stringLength + 1);
            size_t j = convertToBase64Array(outChars, inArray, offset, length, insertLineBreaks);
            str.append(outChars, stringLength);
            delete[] outChars;
            return j > 0;
        }
        return false;
    }

    String
    String::toBase64(const uint8_t *inArray, off_t offset, size_t length, Base64FormattingOptions options) {
        String str;
        if (toBase64(inArray, offset, length, str, options))
            return str;
        return Empty;
    }

    String String::fromBase64() const {
        return String::fromBase64(*this);
    }

    String String::fromBase64(const String &value) {
        if (value.isNullOrEmpty())
            return value;

        ByteArray array;
        if (fromBase64(value, array)) {
            return String((const char *) array.data(), array.count());
        }
        return Empty;
    }

    bool String::fromBase64(const String &value, ByteArray &array) {
        if (value.isNullOrEmpty())
            return false;

        return fromBase64(value.getString(), value.length(), array);
    }

    bool String::fromBase64(const char *inputPtr, size_t inputLength, ByteArray &array) {
        // We need to get rid of any trailing white spaces.
        // Otherwise, we would be rejecting input such as "abc= ":
        while (inputLength > 0) {
            int lastChar = inputPtr[inputLength - 1];
            if (lastChar != (int) ' ' && lastChar != (int) '\n' && lastChar != (int) '\r' && lastChar != (int) '\t')
                break;
            inputLength--;
        }

        // Compute the output length:
        size_t resultLength = fromBase64_ComputeResultLength(inputPtr, inputLength);

        if (0 <= resultLength) {
            // resultLength can be zero. We will still enter FromBase64_Decode and process the input.
            // It may either simply write no bytes (e.g. input = " ") or throw (e.g. input = "ab").

            // Create result uint8_t blob:
            uint8_t *decodedBytes = new uint8_t[resultLength];

            // Convert Base64 chars into bytes:
            size_t actualResultLength = fromBase64_Decode(inputPtr, inputLength, decodedBytes, resultLength);
            array.addRange(decodedBytes, resultLength);
            delete[] decodedBytes;

            return actualResultLength > 0;
        }
        // Note that actualResultLength can differ from resultLength if the caller is modifying the array
        // as it is being converted. Silently ignore the failure.
        // Consider throwing exception in an non in-place release.

        // We are done:
        return false;
    }

    /// <summary>
    /// Compute the number of bytes encoded in the specified Base 64 char array:
    /// Walk the entire input counting white spaces and padding chars, then compute result length
    /// based on 3 bytes per 4 chars.
    /// </summary>
    size_t String::fromBase64_ComputeResultLength(const char *inputPtr, size_t inputLength) {
        const uint intEq = (uint) '=';
        const uint intSpace = (uint) ' ';

        assert(0 <= inputLength);

        const char *inputEndPtr = inputPtr + inputLength;
        size_t usefulInputLength = inputLength;
        int padding = 0;

        while (inputPtr < inputEndPtr) {

            uint c = (uint) (*inputPtr);
            inputPtr++;

            // We want to be as fast as possible and filter out spaces with as few comparisons as possible.
            // We end up accepting a number of illegal chars as legal white-space chars.
            // This is ok: as soon as we hit them during actual decode we will recognise them as illegal and throw.
            if (c <= intSpace)
                usefulInputLength--;

            else if (c == intEq) {
                usefulInputLength--;
                padding++;
            }
        }

        assert(0 <= usefulInputLength);

        // For legal input, we can assume that 0 <= padding < 3. But it may be more for illegal input.
        // We will notice it at decode when we see a '=' at the wrong place.
        assert(0 <= padding);

        // Perf: reuse the variable that stored the number of '=' to store the number of bytes encoded by the
        // last group that contains the '=':
        if (padding != 0) {

            if (padding == 1)
                padding = 2;
            else if (padding == 2)
                padding = 1;
            else
                return 0;// throw Exception("Format_BadBase64Char");
        }

        // Done:
        return (usefulInputLength / 4) * 3 + padding;
    }

    /// <summary>
    /// Decode characters representing a Base64 encoding into bytes:
    /// Walk the input. Every time 4 chars are read, convert them to the 3 corresponding output bytes.
    /// This method is a bit lengthy on purpose. We are trying to avoid jumps to helpers in the loop
    /// to aid performance.
    /// </summary>
    /// <param name="inputPtr">Pointer to first input char</param>
    /// <param name="inputLength">Number of input chars</param>
    /// <param name="destPtr">Pointer to location for teh first result uint8_t</param>
    /// <param name="destLength">Max length of the preallocated result buffer</param>
    /// <returns>If the result buffer was not large enough to write all result bytes, return -1;
    /// Otherwise return the number of result bytes actually produced.</returns>
    size_t
    String::fromBase64_Decode(const char *startInputPtr, size_t inputLength, uint8_t *startDestPtr,
                              size_t destLength) {
        // You may find this method weird to look at. It's written for performance, not aesthetics.
        // You will find unrolled loops label jumps and bit manipulations.

        const uint intA = (uint) 'A';
        const uint inta = (uint) 'a';
        const uint int0 = (uint) '0';
        const uint intEq = (uint) '=';
        const uint intPlus = (uint) '+';
        const uint intSlash = (uint) '/';
        const uint intSpace = (uint) ' ';
        const uint intTab = (uint) '\t';
        const uint intNLn = (uint) '\n';
        const uint intCRt = (uint) '\r';
        const uint intAtoZ = (uint) ('Z' - 'A');  // = ('z' - 'a')
        const uint int0to9 = (uint) ('9' - '0');

        const char *inputPtr = startInputPtr;
        uint8_t *destPtr = startDestPtr;

        // Pointers to the end of input and output:
        const char *endInputPtr = inputPtr + inputLength;
        uint8_t *endDestPtr = destPtr + destLength;

        // Current char code/value:
        uint currCode;

        // This 4-uint8_t integer will contain the 4 codes of the current 4-char group.
        // Eeach char codes for 6 bits = 24 bits.
        // The remaining uint8_t will be FF, we use it as a marker when 4 chars have been processed.
        uint currBlockCodes = 0x000000FFu;

        while (true) {
            // break when done:
            if (inputPtr >= endInputPtr)
                goto _AllInputConsumed;

            // Get current char:
            currCode = (uint) (*inputPtr);
            inputPtr++;

            // Determine current char code:

            if (currCode - intA <= intAtoZ)
                currCode -= intA;

            else if (currCode - inta <= intAtoZ)
                currCode -= (inta - 26u);

            else if (currCode - int0 <= int0to9)
                currCode -= (int0 - 52u);

            else {
                // Use the slower switch for less common cases:
                switch (currCode) {

                    // Significant chars:
                    case intPlus:
                        currCode = 62u;
                        break;

                    case intSlash:
                        currCode = 63u;
                        break;

                        // Legal no-value chars (we ignore these):
                    case intCRt:
                    case intNLn:
                    case intSpace:
                    case intTab:
                        continue;

                        // The equality char is only legal at the end of the input.
                        // Jump after the loop to make it easier for the JIT register predictor to do a good job for the loop itself:
                    case intEq:
                        goto _EqualityCharEncountered;

                        // Other chars are illegal:
                    default:
                        return 0;    // throw Exception("Format_BadBase64Char");
                }
            }

            // Ok, we got the code. Save it:
            currBlockCodes = (currBlockCodes << 6) | currCode;

            // Last bit in currBlockCodes will be on after in shifted right 4 times:
            if ((currBlockCodes & 0x80000000u) != 0u) {

                if ((int) (endDestPtr - destPtr) < 3)
                    return -1;

                *(destPtr) = (uint8_t) (currBlockCodes >> 16);
                *(destPtr + 1) = (uint8_t) (currBlockCodes >> 8);
                *(destPtr + 2) = (uint8_t) (currBlockCodes);
                destPtr += 3;

                currBlockCodes = 0x000000FFu;
            }

        }  // while

        // 'd be nice to have an assert that we never get here, but CS0162: Unreachable code detected.
        // Contract.Assert(false, "We only leave the above loop by jumping; should never get here.");

        // We jump here out of the loop if we hit an '=':
        _EqualityCharEncountered:

        // Recall that inputPtr is now one position past where '=' was read.
        // '=' can only be at the last input pos:
        if (inputPtr == endInputPtr) {

            // Code is zero for trailing '=':
            currBlockCodes <<= 6;

            // The '=' did not complete a 4-group. The input must be bad:
            if ((currBlockCodes & 0x80000000u) == 0u)
                return 0;    // throw Exception("Format_BadBase64CharArrayLength");

            if ((int) (endDestPtr - destPtr) < 2)  // Autch! We underestimated the output length!
                return -1;

            // We are good, store bytes form this past group. We had a single "=", so we take two bytes:
            *(destPtr++) = (uint8_t) (currBlockCodes >> 16);
            *(destPtr++) = (uint8_t) (currBlockCodes >> 8);

            currBlockCodes = 0x000000FFu;

        } else { // '=' can also be at the pre-last position iff the last is also a '=' excluding the white spaces:

            // We need to get rid of any intermediate white spaces.
            // Otherwise we would be rejecting input such as "abc= =":
            while (inputPtr < (endInputPtr - 1)) {
                int lastChar = *(inputPtr);
                if (lastChar != (int) ' ' && lastChar != (int) '\n' && lastChar != (int) '\r' &&
                    lastChar != (int) '\t')
                    break;
                inputPtr++;
            }

            if (inputPtr == (endInputPtr - 1) && *(inputPtr) == '=') {

                // Code is zero for each of the two '=':
                currBlockCodes <<= 12;

                // The '=' did not complete a 4-group. The input must be bad:
                if ((currBlockCodes & 0x80000000u) == 0u)
                    return 0;    // throw Exception("Format_BadBase64CharArrayLength");

                if ((int) (endDestPtr - destPtr) < 1)  // Autch! We underestimated the output length!
                    return -1;

                // We are good, store bytes form this past group. We had a "==", so we take only one uint8_t:
                *(destPtr++) = (uint8_t) (currBlockCodes >> 16);

                currBlockCodes = 0x000000FFu;

            } else  // '=' is not ok at places other than the end:
                return 0;    // throw Exception("Format_BadBase64Char");

        }

        // We get here either from above or by jumping out of the loop:
        _AllInputConsumed:

        // The last block of chars has less than 4 items
        if (currBlockCodes != 0x000000FFu)
            return 0;    // throw Exception("Format_BadBase64CharArrayLength");

        // Return how many bytes were actually recovered:
        return (int) (destPtr - startDestPtr);

    } // int FromBase64_Decode(...)

    ssize_t String::find(const String &str, off_t offset) const {
        if (isNullOrEmpty())
            return -1;
        if (str.isNullOrEmpty())
            return -1;
        if (offset < 0)
            offset = 0;

        const char *current = &getString()[0];
        const char *substring = current + offset;
        const char *first = strstr(substring, str);
        if (nullptr == first) return -1;
        return (first - current);
    }

    ssize_t String::find(const char ch, off_t offset) const {
        char buffer[2] = {ch, '\0'};
        return find(buffer, offset);
    }

    ssize_t String::findLastOf(const String &str) const {
        if (isNullOrEmpty())
            return -1;
        if (str.isNullOrEmpty())
            return -1;

        return ((string) getString()).find_last_of(str);
    }

    ssize_t String::findLastOf(const char ch) const {
        char buffer[2] = {ch, '\0'};
        return findLastOf(buffer);
    }

    bool String::contains(const String &str) const {
        return find(str) != -1;
    }

    bool String::contains(const char ch) const {
        char buffer[2] = {ch, '\0'};
        return contains(buffer);
    }

    void String::append(const char ch) {
        addString(ch);
    }

    void String::append(const char *str, size_t count) {
        addString(str, count);
    }

    void String::append(const String &str) {
        addString(str);
    }

    void String::append(const String &str, off_t offset, size_t count) {
        const char *buffer = str._buffer.data();
        append(buffer + offset, count);
    }

    void String::appendLine(const char ch) {
        append(ch);
        append(NewLine);
    }

    void String::appendLine(const char *str, size_t count) {
        append(str, count);
        append(NewLine);
    }

    void String::appendLine(const String &str) {
        append(str);
        append(NewLine);
    }

    void String::appendLine(const String &str, off_t offset, size_t count) {
        append(str, offset, count);
        append(NewLine);
    }

    String String::replace(const String &src, const String &dst) {
        String str = replace(*this, src, dst);
        setString(str);
        return str;
    }

    String String::replace(const String &str, const String &src, const String &dst) {
        if (str.isNullOrEmpty())
            return str;
        if (src.isNullOrEmpty())
            return str;
        if (src == dst)
            return str;

        string strBig = str.getString();
        std::string::size_type pos = 0;
        while ((pos = strBig.find(src, pos)) != string::npos) {
            const char *dstStr = dst.c_str();
            strBig.replace(pos, src.length(), dstStr != nullptr ? dstStr : "");
            pos += dst.length();
        }
        return strBig;
    }

    String String::substr(off_t offset, size_t count) const {
        return substr(*this, offset, count);
    }

    String String::substr(off_t offset) const {
        return substr(offset, length() - offset);
    }

    String String::substr(const String &str, off_t offset, size_t count) {
        if (str.isNullOrEmpty())
            return Empty;
        size_t strLength = str.length();
        if (offset < 0 || offset >= (int) strLength)
            return Empty;
        if (count > strLength)
            count = strLength;

        String result;
        result._buffer.addRange(str.c_str(), offset, count);
        result._buffer.add('\0');
        return result;
    }

    String String::insert(off_t offset, const String &str) {
        if (offset >= 0 && offset < (int) length()) {
            String s1 = substr(0, offset);
            String s2 = substr(offset, length() - offset);

            String text = s1;
            text.append(str);
            text.append(s2);
            setString(text);

            return text;
        }
        return String::Empty;
    }

    String String::insert(off_t offset, const char ch) {
        char buffer[2] = {ch, '\0'};
        return insert(offset, buffer);
    }

    bool String::removeAt(size_t pos) {
        return _buffer.removeAt(pos);
    }

    bool String::removeRange(size_t pos, size_t count) {
        return _buffer.removeRange(pos, count);
    }

    String String::trim(const char trimChar1, const char trimChar2, const char trimChar3,
                        const char trimChar4) const {
        return String::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimStart(const char trimChar1, const char trimChar2, const char trimChar3,
                             const char trimChar4) const {
        return String::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimEnd(const char trimChar1, const char trimChar2, const char trimChar3,
                           const char trimChar4) const {
        return String::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::convert(const char *format, ...) {
        char *message = new char[MaxFormatStrLength];
        memset(message, 0, MaxFormatStrLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);
        String result = message;
        delete[] message;
        return result;
    }

    String String::format(const char *format, ...) {
        char *message = new char[MaxFormatStrLength];
        memset(message, 0, MaxFormatStrLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);
        String result = message;
        delete[] message;
        return result;
    }

    String String::unicode2String(const String &unicode) {
        static const char *prefix = "&#x";
        static const char *suffix = ";";
        static const int length = 4;
        static const size_t prefixLength = strlen(prefix);
        static const size_t suffixLength = strlen(suffix);

        String result;
        off_t start = 0, end = 0;
        do {
            start = unicode.find(prefix, start);
            if (start >= 0) {
                result.append(unicode, end, end > 0 ? start - end : start);

                end = unicode.find(suffix, start);
                if (end - start - prefixLength == length) {
                    String str = unicode.substr((off_t) (start + prefixLength), length);
                    wchar_t value;
                    WChar::parse(str, value, false);
                    WString temp(value);
                    result.append(temp);
                }
                start += (off_t) prefixLength;
                end += (off_t) suffixLength;
            } else {
                result.append(unicode, end, unicode.length() - end);
            }
        } while (start > 0);
        return result;
    }

    Iterator<char>::const_iterator String::begin() const {
        return Iterator<char>::const_iterator(_buffer.data());
    }

    Iterator<char>::const_iterator String::end() const {
        return Iterator<char>::const_iterator(_buffer.data() + length());
    }

    Iterator<char>::iterator String::begin() {
        return Iterator<char>::iterator(_buffer.data());
    }

    Iterator<char>::iterator String::end() {
        return Iterator<char>::iterator(_buffer.data() + length());
    }

    Iterator<char>::reverse_const_iterator String::rbegin() const {
        return Iterator<char>::reverse_const_iterator(_buffer.data() + length() - 1);
    }

    Iterator<char>::reverse_const_iterator String::rend() const {
        return Iterator<char>::reverse_const_iterator(_buffer.data() - 1);
    }

    Iterator<char>::reverse_iterator String::rbegin() {
        return Iterator<char>::reverse_iterator(_buffer.data() + length() - 1);
    }

    Iterator<char>::reverse_iterator String::rend() {
        return Iterator<char>::reverse_iterator(_buffer.data() - 1);
    }

//#if WIN32
//    String String::NewLine = L"\r\n";
//#else
    const WString WString::NewLine = L"\n";
//#endif
    const WString WString::Empty = L"";
    const WString WString::NA = L"N/A";     // Not applicable

    WString::WString(uint
                     capacity) : _buffer(capacity) {
    }

    WString::WString(
            const WString &value) {
        this->operator=(value);
    }

    WString::WString(
            const wstring &value) : WString(value.c_str()) {
    }

    WString::WString(
            const String &value) {
#ifdef WIN32
        size_t len = value.length();
        int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, nullptr, 0);
        wchar_t *pUnicode;
        pUnicode = new wchar_t[unicodeLen + 1];
        memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
        ::MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, (LPWSTR) pUnicode, unicodeLen);
        addString(pUnicode, (uint) wcslen(pUnicode));
        delete pUnicode;
#else
        const char *src = value.c_str();
        int w_size = mbstowcs(nullptr, src, 0) + 1;
        if (w_size > 0) {
            wchar_t *dest = new wchar_t[w_size];
            int ret = mbstowcs(dest, src, value.length() + 1);
            if (ret > 0) {
                addString(dest, (uint) wcslen(dest));
            }
            delete[] dest;
        }
#endif
    }

    WString::WString(
            const wchar_t *value, size_t
    count) {
        if (value != nullptr)
            addString(value, count == 0 ? (uint) wcslen(value) : count);
    }

    WString::WString(wchar_t
                     ch, size_t
                     count) : WString() {
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
            addString(value, (uint) wcslen(value));
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
        if (length() > 0) {
            const wchar_t *src = getString();
#ifdef WIN32
            size_t len = WideCharToMultiByte(CP_ACP, 0, src, (int) wcslen(src), nullptr, 0, nullptr, nullptr);
            char *dest = new char[len + 1];
            memset(dest, 0, len + 1);
            WideCharToMultiByte(CP_ACP, 0, src, (int) wcslen(src), dest, (int) len, nullptr, nullptr);
            String result = dest;
            delete[] dest;
            return result;
#else
            String result;
            size_t len = wcslen(src) * 2;
            char *dest = new char[len + 1];
            memset(dest, 0, len + 1);
            size_t ret = wcstombs(dest, src, sizeof(dest));
            if (ret != (size_t) -1)
                result = dest;
            delete[] dest;
            return result;
#endif
        }
        return String::Empty;
    }

    const wchar_t *WString::c_str() const {
        return getString();
    }

    bool WString::equals(const WString &value1, const WString &value2, bool ignoreCase) {
        return compare(value1, value2, ignoreCase) == 0;
    }

    int WString::compare(const WString &value1, const WString &value2, bool ignoreCase) {
        if (!ignoreCase) {
            return wcscoll(value1.getString(), value2.getString());
        } else {
#if WIN32
#define wcscasecmp wcsicmp
#endif
            return wcscasecmp(value1.getString(), value2.getString());
        }
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

    bool WString::operator>=(const WString &value) const {
        return compare(*this, value) >= 0;
    }

    bool WString::operator<(const WString &value) const {
        return compare(*this, value) < 0;
    }

    bool WString::operator<=(const WString &value) const {
        return compare(*this, value) <= 0;
    }

    wchar_t &WString::at(size_t
                         pos) {
        return _buffer.at(pos);
    }

    wchar_t WString::at(size_t
                        pos) const {
        return _buffer.at(pos);
    }

    bool WString::set(size_t
                      pos,
                      const wchar_t &value) {
        return _buffer.set(pos, value);
    }

    WString
    WString::trim(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                  const wchar_t trimChar4) const {
        return WString::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString
    WString::trimStart(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                       const wchar_t trimChar4) const {
        return WString::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString
    WString::trimEnd(const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                     const wchar_t trimChar4) const {
        return WString::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    WString WString::trim(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                          const wchar_t trimChar4) {
        if (str.isNullOrEmpty())
            return str;

        WString result;
        for (uint i = 0; i < str.length(); i++) {
            wchar_t ch = str._buffer[i];
            if (!(ch == trimChar1 || ch == trimChar2 || ch == trimChar3 || ch == trimChar4))
                result.append(ch);
        }
        return result;
    }

    WString
    WString::trimStart(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                       const wchar_t trimChar4) {
        if (str.isNullOrEmpty())
            return str;

        WString result;
        for (uint i = 0; i < str.length(); i++) {
            wchar_t ch = str._buffer[i];
            if (!((ch == trimChar1 || ch == trimChar2 || ch == trimChar3 || ch == trimChar4) && i == 0))
                result.append(ch);
        }
        return result;
    }

    WString
    WString::trimEnd(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2, const wchar_t trimChar3,
                     const wchar_t trimChar4) {
        if (str.isNullOrEmpty())
            return str;

        WString result;
        for (uint i = 0; i < str.length(); i++) {
            wchar_t ch = str._buffer[i];
            if (!((ch == trimChar1 || ch == trimChar2 || ch == trimChar3 || ch == trimChar4) && i == str.length() - 1))
                result.append(ch);
        }
        return result;
    }

    ssize_t WString::find(const wchar_t *substring, off_t offset) const {
        if (isNullOrEmpty())
            return -1;
        if (substring == nullptr)
            return -1;
        if (offset < 0)
            offset = 0;

        const wchar_t *orginal = &getString()[0];
        const wchar_t *str = orginal + offset;
        const wchar_t *first = wcsstr(str, substring);
        if (nullptr == first) return -1;
        return (first - orginal);
    }

    ssize_t WString::find(const wchar_t ch) const {
        wchar_t buffer[2] = {ch, '\0'};
        return find(buffer);
    }

    ssize_t WString::findLastOf(const wchar_t *substring) const {
        if (isNullOrEmpty())
            return -1;
        if (substring == nullptr)
            return -1;

        // todo: modify to c wstring
        return ((wstring) getString()).find_last_of(substring);

        //        int temp, result = -1, pos = 0;
        //        do
        //        {
        //            temp = find(substring, pos);
        //            if(temp >= 0)
        //            {
        //                pos = temp + strlen(substring);
        //                result = temp;
        //            }
        //        }while(temp >= 0);
        //        return result;
    }

    ssize_t WString::findLastOf(const wchar_t ch) const {
        wchar_t buffer[2] = {ch, '\0'};
        return findLastOf(buffer);
    }

    bool WString::contains(const wchar_t *substring) const {
        return find(substring) != -1;
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

    const Boolean Boolean::TrueValue = true;
    const Boolean Boolean::FalseValue = false;

    Boolean::Boolean() : _value(false) {
    }

    Boolean::Boolean(
            const Boolean &value) : _value(value._value) {
    }

    Boolean::Boolean(
            const bool &value) : _value(value) {
    }

    Boolean::~Boolean() {
    }

    Boolean::operator bool() const {
        return _value;
    }

    Boolean &Boolean::operator=(const Boolean &value) {
        _value = value._value;
        return *this;
    }

    bool Boolean::operator==(const Boolean &value) const {
        return _value == value._value;
    }

    bool Boolean::operator!=(const Boolean &value) const {
        return !operator==(value);
    }

    void Boolean::write(Stream *stream) const {
        stream->writeBoolean(_value);
    }

    void Boolean::read(Stream *stream) {
        _value = stream->readBoolean();
    }

    const String Boolean::toString() const {
        return _value ? "true" : "false";
    }

    bool Boolean::parse(const String &str, Boolean &value) {
        if (String::equals(str, "true", true)) {
            value._value = true;
            return true;
        } else if (String::equals(str, "false", true)) {
            value._value = false;
            return true;
        } else {
            int temp;
            if (Int32::parse(str, temp)) {
                value._value = temp != 0;
                return true;
            }
        }
        return false;
    }

    bool Boolean::parse(const String &str, bool &value) {
        Boolean temp;
        if (parse(str, temp)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const Char Char::MaxValue = Char(INT8_MAX);
    const Char Char::MinValue = Char(INT8_MIN);
    const Char Char::NewLine = '\n';
    const uint8_t Char::categoryForLatin1[] = {
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0000 - 0007
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0008 - 000F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0010 - 0017
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0018 - 001F
            (uint8_t) UnicodeCategory::SpaceSeparator, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,    // 0020 - 0027
            (uint8_t) UnicodeCategory::OpenPunctuation, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::DashPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,    // 0028 - 002F
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber,    // 0030 - 0037
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::OtherPunctuation,    // 0038 - 003F
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0040 - 0047
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0048 - 004F
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0050 - 0057
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::OpenPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::ModifierSymbol,
            (uint8_t) UnicodeCategory::ConnectorPunctuation,    // 0058 - 005F
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0060 - 0067
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0068 - 006F
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0070 - 0077
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::OpenPunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::Control,    // 0078 - 007F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0080 - 0087
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0088 - 008F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0090 - 0097
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0098 - 009F
            (uint8_t) UnicodeCategory::SpaceSeparator, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::CurrencySymbol,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::CurrencySymbol,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::OtherSymbol,    // 00A0 - 00A7
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::OtherSymbol,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::InitialQuotePunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::DashPunctuation,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::ModifierSymbol,    // 00A8 - 00AF
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::OtherPunctuation,    // 00B0 - 00B7
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::FinalQuotePunctuation,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherPunctuation,    // 00B8 - 00BF
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 00C0 - 00C7
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 00C8 - 00CF
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::MathSymbol,    // 00D0 - 00D7
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00D8 - 00DF
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00E0 - 00E7
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00E8 - 00EF
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::MathSymbol,    // 00F0 - 00F7
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00F8 - 00FF
    };

    Char::Char() : _value(0) {
    }

    Char::Char(
            const Char &value) : _value(value._value) {
    }

    Char::Char(
            const char &value) : _value(value) {
    }

    Char::~Char() {
    }

    Char::operator char() const {
        return _value;
    }

    Char &Char::operator=(const Char &value) {
        _value = value._value;
        return *this;
    }

    bool Char::operator==(const Char &value) const {
        return _value == value._value;
    }

    bool Char::operator!=(const Char &value) const {
        return !operator==(value);
    }

    bool Char::operator>(const Char &value) const {
        return _value > value._value;
    }

    bool Char::operator>=(const Char &value) const {
        return _value >= value._value;
    }

    bool Char::operator<(const Char &value) const {
        return _value < value._value;
    }

    bool Char::operator<=(const Char &value) const {
        return _value <= value._value;
    }

    Char Char::operator+=(const Char &value) {
        _value += value._value;
        return *this;
    }

    Char Char::operator-=(const Char &value) {
        _value -= value._value;
        return *this;
    }

    Char Char::operator+(const Char &value) {
        Char result = _value;
        result._value += value._value;
        return result;
    }

    Char Char::operator-(const Char &value) {
        Char result = _value;
        result._value -= value._value;
        return result;
    }

    Char &Char::operator=(const char &value) {
        _value = value;
        return *this;
    }

    bool Char::operator==(const char &value) const {
        return _value == value;
    }

    bool Char::operator!=(const char &value) const {
        return !operator==(value);
    }

    bool Char::operator>(const char &value) const {
        return _value > value;
    }

    bool Char::operator>=(const char &value) const {
        return _value >= value;
    }

    bool Char::operator<(const char &value) const {
        return _value < value;
    }

    bool Char::operator<=(const char &value) const {
        return _value <= value;
    }

    Char Char::operator+=(const char &value) {
        _value += value;
        return *this;
    }

    Char Char::operator-=(const char &value) {
        _value -= value;
        return *this;
    }

    Char Char::operator+(const char &value) {
        Char result = _value;
        result._value += value;
        return result;
    }

    Char Char::operator-(const char &value) {
        Char result = _value;
        result._value -= value;
        return result;
    }

    char Char::operator++()  // ++i
    {
        return ++_value;
    }

    char Char::operator++(int)   // i++
    {
        return _value++;
    }

    char Char::operator--()  // --i
    {
        return --_value;
    }

    char Char::operator--(int)   // i--
    {
        return _value--;
    }

    void Char::write(Stream *stream) const {
        stream->writeByte(_value);
    }

    void Char::read(Stream *stream) {
        _value = stream->readByte();
    }

    const String Char::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRId8, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int Char::compareTo(Char value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    const Char Char::toLower() const {
        return (char) ::tolower(_value);
    }

    const Char Char::toUpper() const {
        return (char) ::toupper(_value);
    }

    bool Char::parse(const String &str, Char &value, bool decimal) {
//        uint len = 0;
//        if(sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId8 "%n" : "%" PRId8 "%n", &value._value, &len) == 1 && str.length() == len)
//        {
//            return true;
//        }
//        return false;
        Int16 temp;
        if (Int16::parse(str, temp, decimal) &&
            temp >= (Int16) MinValue && temp <= (Int16) MaxValue) {
            value._value = (char) temp;
            return true;
        }
        return false;
    }

    bool Char::parse(const String &str, char &value, bool decimal) {
        Char temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    bool Char::parse(const String &str, int8_t &value, bool decimal) {
        Char temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    char Char::toChar(uint8_t value) {
        char result;
        if (value <= 9)
            result = value + 0x30;
        else if ((value >= 10) && (value <= 15))
            result = value + 0x37;
        else
            result = (char) 0xff;
        return result;
    }

    uint8_t Char::toHex(char value) {
        uint8_t result;
        if ((value >= 0x30) && (value <= 0x39))
            result = value - 0x30;
        else if ((value >= 0x41) && (value <= 0x46))
            result = value - 0x37;
        else if ((value >= 0x61) && (value <= 0x66))
            result = value - 0x57;
        else
            result = 0xff;
        return result;
    }

    // Return true for all characters below or equal U+00ff, which is ASCII + Latin-1 Supplement.
    bool Char::isLatin1(char ch) {
        return ((uint) ch <= 0xFF);
    }

    // Return true for all characters below or equal U+007f, which is ASCII.
    bool Char::isAscii(char ch) {
        return ((uint) ch <= 0x7F);
    }
    /*=================================ISDIGIT======================================
     **A wrapper for Char.  Returns a boolean indicating whether    **
     **character c is considered to be a digit.                                    **
     ==============================================================================*/
    // Determines whether a character is a digit.
    bool Char::isDigit(char c) {
        if (isLatin1(c)) {
            return (c >= '0' && c <= '9');
        }
        return false;
    }
    /*=================================ISLETTER=====================================
     **A wrapper for Char.  Returns a boolean indicating whether    **
     **character c is considered to be a letter.                                   **
     ==============================================================================*/
    // Determines whether a character is a letter.
    bool Char::isLetter(char c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                c |= (char) 0x20;
                return ((c >= 'a' && c <= 'z'));
            }
            return false;
        }
        return false;
    }

    bool Char::isWhiteSpaceLatin1(char c) {
        // There are characters which belong to UnicodeCategory::Control but are considered as white spaces.
        // We use code point comparisons for these characters here as a temporary fix.

        // U+0009 = <control> HORIZONTAL TAB
        // U+000a = <control> LINE FEED
        // U+000b = <control> VERTICAL TAB
        // U+000c = <contorl> FORM FEED
        // U+000d = <control> CARRIAGE RETURN
        // U+0085 = <control> NEXT LINE
        // U+00a0 = NO-BREAK SPACE
        if ((c == ' ') || (c >= '\x0009' && c <= '\x000d') || c == '\x00a0' || c == '\x0085') {
            return (true);
        }
        return (false);
    }
    /*===============================ISWHITESPACE===================================
     **A wrapper for Char.  Returns a boolean indicating whether    **
     **character c is considered to be a whitespace character.                     **
     ==============================================================================*/
    // Determines whether a character is whitespace.
    bool Char::isWhiteSpace(char c) {
        if (isLatin1(c)) {
            return (isWhiteSpaceLatin1(c));
        }
        return false;
    }
    /*===================================IsUpper====================================
     **Arguments: c -- the characater to be checked.
     **Returns:  True if c is an uppercase character.
     ==============================================================================*/
    // Determines whether a character is upper-case.
    bool Char::isUpper(char c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                return (c >= 'A' && c <= 'Z');
            }
            return false;
        }
        return false;
    }
    /*===================================IsLower====================================
     **Arguments: c -- the characater to be checked.
     **Returns:  True if c is an lowercase character.
     ==============================================================================*/
    // Determines whether a character is lower-case.
    bool Char::isLower(char c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                return (c >= 'a' && c <= 'z');
            }
            return false;
        }
        return false;
    }

    // Determines whether a character is a letter or a digit.
    bool Char::isLetterOrDigit(char c) {
        if (isLatin1(c)) {
            return (checkLetterOrDigit(getLatin1UnicodeCategory(c)));
        }
        return false;
    }

    bool Char::isSymbol(char c) {
        if (isLatin1(c)) {
            return (checkSymbol(getLatin1UnicodeCategory(c)));
        }
        return false;
    }

    bool Char::checkSymbol(UnicodeCategory uc) {
        switch ((uint8_t) uc) {
            case UnicodeCategory::MathSymbol:
            case UnicodeCategory::CurrencySymbol:
            case UnicodeCategory::ModifierSymbol:
            case UnicodeCategory::OtherSymbol:
            case UnicodeCategory::ConnectorPunctuation:
            case UnicodeCategory::DashPunctuation:
            case UnicodeCategory::OpenPunctuation:
            case UnicodeCategory::ClosePunctuation:
            case UnicodeCategory::InitialQuotePunctuation:
            case UnicodeCategory::FinalQuotePunctuation:
            case UnicodeCategory::OtherPunctuation:
                return (true);
        }
        return (false);
    }

    Char::UnicodeCategory Char::getLatin1UnicodeCategory(char ch) {
        assert(isLatin1(ch));
        return (UnicodeCategory) (categoryForLatin1[(int) ch]);
    }

    bool Char::checkLetterOrDigit(UnicodeCategory uc) {
        switch ((uint8_t) uc) {
            case UnicodeCategory::UppercaseLetter:
            case UnicodeCategory::LowercaseLetter:
            case UnicodeCategory::TitlecaseLetter:
            case UnicodeCategory::ModifierLetter:
            case UnicodeCategory::OtherLetter:
            case UnicodeCategory::DecimalDigitNumber:
                return (true);
        }
        return (false);
    }

    const WChar WChar::MaxValue = WChar(WCHAR_MAX);
    const WChar WChar::MinValue = WChar(WCHAR_MIN);
    const uint8_t WChar::categoryForLatin1[] = {
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0000 - 0007
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0008 - 000F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0010 - 0017
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0018 - 001F
            (uint8_t) UnicodeCategory::SpaceSeparator, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,    // 0020 - 0027
            (uint8_t) UnicodeCategory::OpenPunctuation, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::DashPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation,    // 0028 - 002F
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::DecimalDigitNumber,    // 0030 - 0037
            (uint8_t) UnicodeCategory::DecimalDigitNumber, (uint8_t) UnicodeCategory::DecimalDigitNumber,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::OtherPunctuation,    // 0038 - 003F
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0040 - 0047
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0048 - 004F
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 0050 - 0057
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::OpenPunctuation,
            (uint8_t) UnicodeCategory::OtherPunctuation, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::ModifierSymbol,
            (uint8_t) UnicodeCategory::ConnectorPunctuation,    // 0058 - 005F
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0060 - 0067
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0068 - 006F
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 0070 - 0077
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::OpenPunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::ClosePunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::Control,    // 0078 - 007F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0080 - 0087
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0088 - 008F
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0090 - 0097
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control,
            (uint8_t) UnicodeCategory::Control, (uint8_t) UnicodeCategory::Control,    // 0098 - 009F
            (uint8_t) UnicodeCategory::SpaceSeparator, (uint8_t) UnicodeCategory::OtherPunctuation,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::CurrencySymbol,
            (uint8_t) UnicodeCategory::CurrencySymbol, (uint8_t) UnicodeCategory::CurrencySymbol,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::OtherSymbol,    // 00A0 - 00A7
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::OtherSymbol,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::InitialQuotePunctuation,
            (uint8_t) UnicodeCategory::MathSymbol, (uint8_t) UnicodeCategory::DashPunctuation,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::ModifierSymbol,    // 00A8 - 00AF
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::MathSymbol,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::OtherSymbol, (uint8_t) UnicodeCategory::OtherPunctuation,    // 00B0 - 00B7
            (uint8_t) UnicodeCategory::ModifierSymbol, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::FinalQuotePunctuation,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherNumber,
            (uint8_t) UnicodeCategory::OtherNumber, (uint8_t) UnicodeCategory::OtherPunctuation,    // 00B8 - 00BF
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 00C0 - 00C7
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,    // 00C8 - 00CF
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::MathSymbol,    // 00D0 - 00D7
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter, (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::UppercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00D8 - 00DF
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00E0 - 00E7
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00E8 - 00EF
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::MathSymbol,    // 00F0 - 00F7
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter, (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,
            (uint8_t) UnicodeCategory::LowercaseLetter,    // 00F8 - 00FF
    };

    WChar::WChar() : _value(0) {
    }

    WChar::WChar(
            const WChar &value) : _value(value._value) {
    }

    WChar::WChar(
            const wchar_t &value) : _value(value) {
    }

    WChar::~WChar() {
    }

    WChar::operator wchar_t() const {
        return _value;
    }

    WChar &WChar::operator=(const WChar &value) {
        _value = value._value;
        return *this;
    }

    bool WChar::operator==(const WChar &value) const {
        return _value == value._value;
    }

    bool WChar::operator!=(const WChar &value) const {
        return !operator==(value);
    }

    bool WChar::operator>(const WChar &value) const {
        return _value > value._value;
    }

    bool WChar::operator>=(const WChar &value) const {
        return _value >= value._value;
    }

    bool WChar::operator<(const WChar &value) const {
        return _value < value._value;
    }

    bool WChar::operator<=(const WChar &value) const {
        return _value <= value._value;
    }

    WChar WChar::operator+=(const WChar &value) {
        _value += value._value;
        return *this;
    }

    WChar WChar::operator-=(const WChar &value) {
        _value -= value._value;
        return *this;
    }

    WChar WChar::operator+(const WChar &value) {
        WChar result = _value;
        result._value += value._value;
        return result;
    }

    WChar WChar::operator-(const WChar &value) {
        WChar result = _value;
        result._value -= value._value;
        return result;
    }

    wchar_t WChar::operator++()  // ++i
    {
        return ++_value;
    }

    wchar_t WChar::operator++(int)   // i++
    {
        return _value++;
    }

    wchar_t WChar::operator--()  // --i
    {
        return --_value;
    }

    wchar_t WChar::operator--(int)   // i--
    {
        return _value--;
    }

    void WChar::write(Stream *stream) const {
        stream->writeByte(_value);
    }

    void WChar::read(Stream *stream) {
        _value = stream->readByte();
    }

    const String WChar::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRId16, (short) _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int WChar::compareTo(WChar value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool WChar::parse(const String &str, WChar &value, bool decimal) {
        //        uint len = 0;
        //        if(sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId8 "%n" : "%" PRId8 "%n", &value._value, &len) == 1 && str.length() == len)
        //        {
        //            return true;
        //        }
        //        return false;
        Int32 temp;
        if (Int32::parse(str, temp, decimal) &&
            temp >= (Int32) MinValue && temp <= (Int32) MaxValue) {
            value._value = (wchar_t) temp;
            return true;
        }
        return false;
    }

    bool WChar::parse(const String &str, wchar_t &value, bool decimal) {
        WChar temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    // Return true for all wchar_tacters below or equal U+00ff, which is ASCII + Latin-1 Supplement.
    bool WChar::isLatin1(wchar_t ch) {
        return ((int) ch <= 0xFF);
    }

    // Return true for all wchar_tacters below or equal U+007f, which is ASCII.
    bool WChar::isAscii(wchar_t ch) {
        return ((int) ch <= 0x7F);
    }
    /*=================================ISDIGIT======================================
     **A wrapper for WChar.  Returns a boolean indicating whether    **
     **wchar_tacter c is considered to be a digit.                                    **
     ==============================================================================*/
    // Determines whether a wchar_tacter is a digit.
    bool WChar::isDigit(wchar_t c) {
        if (isLatin1(c)) {
            return (c >= '0' && c <= '9');
        }
        return false;
    }
    /*=================================ISLETTER=====================================
     **A wrapper for WChar.  Returns a boolean indicating whether    **
     **wchar_tacter c is considered to be a letter.                                   **
     ==============================================================================*/
    // Determines whether a wchar_tacter is a letter.
    bool WChar::isLetter(wchar_t c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                c |= (wchar_t) 0x20;
                return ((c >= 'a' && c <= 'z'));
            }
            return false;
        }
        return false;
    }

    bool WChar::isWhiteSpaceLatin1(wchar_t c) {
        // There are wchar_tacters which belong to UnicodeCategory::Control but are considered as white spaces.
        // We use code point comparisons for these wchar_tacters here as a temporary fix.

        // U+0009 = <control> HORIZONTAL TAB
        // U+000a = <control> LINE FEED
        // U+000b = <control> VERTICAL TAB
        // U+000c = <contorl> FORM FEED
        // U+000d = <control> CARRIAGE RETURN
        // U+0085 = <control> NEXT LINE
        // U+00a0 = NO-BREAK SPACE
        if ((c == ' ') || (c >= '\x0009' && c <= '\x000d') || c == '\x00a0' || c == '\x0085') {
            return (true);
        }
        return (false);
    }
    /*===============================ISWHITESPACE===================================
     **A wrapper for WChar.  Returns a boolean indicating whether    **
     **wchar_tacter c is considered to be a whitespace wchar_tacter.                     **
     ==============================================================================*/
    // Determines whether a wchar_tacter is whitespace.
    bool WChar::isWhiteSpace(wchar_t c) {
        if (isLatin1(c)) {
            return (isWhiteSpaceLatin1(c));
        }
        return false;
    }
    /*===================================IsUpper====================================
     **Arguments: c -- the wchar_tacater to be checked.
     **Returns:  True if c is an uppercase wchar_tacter.
     ==============================================================================*/
    // Determines whether a wchar_tacter is upper-case.
    bool WChar::isUpper(wchar_t c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                return (c >= 'A' && c <= 'Z');
            }
            return false;
        }
        return false;
    }
    /*===================================IsLower====================================
     **Arguments: c -- the wchar_tacater to be checked.
     **Returns:  True if c is an lowercase wchar_tacter.
     ==============================================================================*/
    // Determines whether a wchar_tacter is lower-case.
    bool WChar::isLower(wchar_t c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                return (c >= 'a' && c <= 'z');
            }
            return false;
        }
        return false;
    }

    // Determines whether a wchar_tacter is a letter or a digit.
    bool WChar::isLetterOrDigit(wchar_t c) {
        if (isLatin1(c)) {
            return (checkLetterOrDigit(getLatin1UnicodeCategory(c)));
        }
        return false;
    }

    bool WChar::isSymbol(wchar_t c) {
        if (isLatin1(c)) {
            return (checkSymbol(getLatin1UnicodeCategory(c)));
        }
        return false;
    }

    bool WChar::isHanzi(wchar_t c) {
        return c >= 0x4e00 && c <= 0x9fa5;
    }

    bool WChar::checkSymbol(UnicodeCategory uc) {
        switch ((uint8_t) uc) {
            case UnicodeCategory::MathSymbol:
            case UnicodeCategory::CurrencySymbol:
            case UnicodeCategory::ModifierSymbol:
            case UnicodeCategory::OtherSymbol:
            case UnicodeCategory::ConnectorPunctuation:
            case UnicodeCategory::DashPunctuation:
            case UnicodeCategory::OpenPunctuation:
            case UnicodeCategory::ClosePunctuation:
            case UnicodeCategory::InitialQuotePunctuation:
            case UnicodeCategory::FinalQuotePunctuation:
            case UnicodeCategory::OtherPunctuation:
                return (true);
        }
        return (false);
    }

    WChar::UnicodeCategory WChar::getLatin1UnicodeCategory(wchar_t ch) {
        assert(isLatin1(ch));
        return (UnicodeCategory) (categoryForLatin1[(int) ch]);
    }

    bool WChar::checkLetterOrDigit(UnicodeCategory uc) {
        switch ((uint8_t) uc) {
            case UnicodeCategory::UppercaseLetter:
            case UnicodeCategory::LowercaseLetter:
            case UnicodeCategory::TitlecaseLetter:
            case UnicodeCategory::ModifierLetter:
            case UnicodeCategory::OtherLetter:
            case UnicodeCategory::DecimalDigitNumber:
                return (true);
        }
        return (false);
    }

    const Int8 Int8::MaxValue = Int8(INT8_MAX);
    const Int8 Int8::MinValue = Int8(INT8_MIN);

    Int8::Int8() : _value(0) {
    }

    Int8::Int8(
            const Int8 &value) : _value(value._value) {
    }

    Int8::Int8(
            const int8_t &value) : _value(value) {
    }

    Int8::~Int8() {
    }

    Int8::operator int8_t() const {
        return _value;
    }

    Int8 &Int8::operator=(const Int8 &value) {
        _value = value._value;
        return *this;
    }

    bool Int8::operator==(const Int8 &value) const {
        return _value == value._value;
    }

    bool Int8::operator!=(const Int8 &value) const {
        return !operator==(value);
    }

    bool Int8::operator>(const Int8 &value) const {
        return _value > value._value;
    }

    bool Int8::operator>=(const Int8 &value) const {
        return _value >= value._value;
    }

    bool Int8::operator<(const Int8 &value) const {
        return _value < value._value;
    }

    bool Int8::operator<=(const Int8 &value) const {
        return _value <= value._value;
    }

    Int8 Int8::operator+=(const Int8 &value) {
        _value += value._value;
        return *this;
    }

    Int8 Int8::operator-=(const Int8 &value) {
        _value -= value._value;
        return *this;
    }

    Int8 Int8::operator+(const Int8 &value) {
        Int8 result = _value;
        result._value += value._value;
        return result;
    }

    Int8 Int8::operator-(const Int8 &value) {
        Int8 result = _value;
        result._value -= value._value;
        return result;
    }

    Int8 &Int8::operator=(const int8_t &value) {
        _value = value;
        return *this;
    }

    bool Int8::operator==(const int8_t &value) const {
        return _value == value;
    }

    bool Int8::operator!=(const int8_t &value) const {
        return !operator==(value);
    }

    bool Int8::operator>(const int8_t &value) const {
        return _value > value;
    }

    bool Int8::operator>=(const int8_t &value) const {
        return _value >= value;
    }

    bool Int8::operator<(const int8_t &value) const {
        return _value < value;
    }

    bool Int8::operator<=(const int8_t &value) const {
        return _value <= value;
    }

    Int8 Int8::operator+=(const int8_t &value) {
        _value += value;
        return *this;
    }

    Int8 Int8::operator-=(const int8_t &value) {
        _value -= value;
        return *this;
    }

    Int8 Int8::operator+(const int8_t &value) {
        Int8 result = _value;
        result._value += value;
        return result;
    }

    Int8 Int8::operator-(const int8_t &value) {
        Int8 result = _value;
        result._value -= value;
        return result;
    }

    int8_t Int8::operator++()  // ++i
    {
        return ++_value;
    }

    int8_t Int8::operator++(int)   // i++
    {
        return _value++;
    }

    int8_t Int8::operator--()  // --i
    {
        return --_value;
    }

    int8_t Int8::operator--(int)   // i--
    {
        return _value--;
    }

    void Int8::write(Stream *stream) const {
        stream->writeInt8(_value);
    }

    void Int8::read(Stream *stream) {
        _value = stream->readInt8();
    }

    const String Int8::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRIu8, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int Int8::compareTo(Int8 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool Int8::parse(const String &str, Int8 &value, bool decimal) {
        UInt16 temp;
        if (UInt16::parse(str, temp, decimal) &&
            temp >= (UInt16) MinValue && temp <= (UInt16) MaxValue) {
            value._value = (uint8_t) temp;
            return true;
        }
        return false;
    }

    bool Int8::parse(const String &str, int8_t &value, bool decimal) {
        Int8 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const UInt8 UInt8::MaxValue = UInt8(UINT8_MAX);
    const UInt8 UInt8::MinValue = UInt8(0);

    UInt8::UInt8() : _value(0) {
    }

    UInt8::UInt8(
            const UInt8 &value) : _value(value._value) {
    }

    UInt8::UInt8(
            const uint8_t &value) : _value(value) {
    }

    UInt8::~UInt8() {
    }

    UInt8::operator uint8_t() const {
        return _value;
    }

    UInt8 &UInt8::operator=(const UInt8 &value) {
        _value = value._value;
        return *this;
    }

    bool UInt8::operator==(const UInt8 &value) const {
        return _value == value._value;
    }

    bool UInt8::operator!=(const UInt8 &value) const {
        return !operator==(value);
    }

    bool UInt8::operator>(const UInt8 &value) const {
        return _value > value._value;
    }

    bool UInt8::operator>=(const UInt8 &value) const {
        return _value >= value._value;
    }

    bool UInt8::operator<(const UInt8 &value) const {
        return _value < value._value;
    }

    bool UInt8::operator<=(const UInt8 &value) const {
        return _value <= value._value;
    }

    UInt8 UInt8::operator+=(const UInt8 &value) {
        _value += value._value;
        return *this;
    }

    UInt8 UInt8::operator-=(const UInt8 &value) {
        _value -= value._value;
        return *this;
    }

    UInt8 UInt8::operator+(const UInt8 &value) {
        UInt8 result = _value;
        result._value += value._value;
        return result;
    }

    UInt8 UInt8::operator-(const UInt8 &value) {
        UInt8 result = _value;
        result._value -= value._value;
        return result;
    }

    UInt8 &UInt8::operator=(const uint8_t &value) {
        _value = value;
        return *this;
    }

    bool UInt8::operator==(const uint8_t &value) const {
        return _value == value;
    }

    bool UInt8::operator!=(const uint8_t &value) const {
        return !operator==(value);
    }

    bool UInt8::operator>(const uint8_t &value) const {
        return _value > value;
    }

    bool UInt8::operator>=(const uint8_t &value) const {
        return _value >= value;
    }

    bool UInt8::operator<(const uint8_t &value) const {
        return _value < value;
    }

    bool UInt8::operator<=(const uint8_t &value) const {
        return _value <= value;
    }

    UInt8 UInt8::operator+=(const uint8_t &value) {
        _value += value;
        return *this;
    }

    UInt8 UInt8::operator-=(const uint8_t &value) {
        _value -= value;
        return *this;
    }

    UInt8 UInt8::operator+(const uint8_t &value) {
        UInt8 result = _value;
        result._value += value;
        return result;
    }

    UInt8 UInt8::operator-(const uint8_t &value) {
        UInt8 result = _value;
        result._value -= value;
        return result;
    }

    uint8_t UInt8::operator++()  // ++i
    {
        return ++_value;
    }

    uint8_t UInt8::operator++(int)   // i++
    {
        return _value++;
    }

    uint8_t UInt8::operator--()  // --i
    {
        return --_value;
    }

    uint8_t UInt8::operator--(int)   // i--
    {
        return _value--;
    }

    void UInt8::write(Stream *stream) const {
        stream->writeUInt8(_value);
    }

    void UInt8::read(Stream *stream) {
        _value = stream->readUInt8();
    }

    const String UInt8::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRIu8, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int UInt8::compareTo(UInt8 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool UInt8::parse(const String &str, UInt8 &value, bool decimal) {
        UInt16 temp;
        if (UInt16::parse(str, temp, decimal) &&
            temp >= (UInt16) MinValue && temp <= (UInt16) MaxValue) {
            value._value = (uint8_t) temp;
            return true;
        }
        return false;
    }

    bool UInt8::parse(const String &str, uint8_t &value, bool decimal) {
        UInt8 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const Int16 Int16::MaxValue = Int16(INT16_MAX);
    const Int16 Int16::MinValue = Int16(INT16_MIN);

    Int16::Int16() : _value(0) {
    }

    Int16::Int16(
            const Int16 &value) : _value(value._value) {
    }

    Int16::Int16(
            const int16_t &value) : _value(value) {
    }

    Int16::~Int16() {
    }

    Int16::operator int16_t() const {
        return _value;
    }

    Int16 &Int16::operator=(const Int16 &value) {
        _value = value._value;
        return *this;
    }

    bool Int16::operator==(const Int16 &value) const {
        return _value == value._value;
    }

    bool Int16::operator!=(const Int16 &value) const {
        return !operator==(value);
    }

    bool Int16::operator>(const Int16 &value) const {
        return _value > value._value;
    }

    bool Int16::operator>=(const Int16 &value) const {
        return _value >= value._value;
    }

    bool Int16::operator<(const Int16 &value) const {
        return _value < value._value;
    }

    bool Int16::operator<=(const Int16 &value) const {
        return _value <= value._value;
    }

    Int16 Int16::operator+=(const Int16 &value) {
        _value += value._value;
        return *this;
    }

    Int16 Int16::operator-=(const Int16 &value) {
        _value -= value._value;
        return *this;
    }

    Int16 Int16::operator+(const Int16 &value) {
        Int16 result = _value;
        result._value += value._value;
        return result;
    }

    Int16 Int16::operator-(const Int16 &value) {
        Int16 result = _value;
        result._value -= value._value;
        return result;
    }

    Int16 &Int16::operator=(const int16_t &value) {
        _value = value;
        return *this;
    }

    bool Int16::operator==(const int16_t &value) const {
        return _value == value;
    }

    bool Int16::operator!=(const int16_t &value) const {
        return !operator==(value);
    }

    bool Int16::operator>(const int16_t &value) const {
        return _value > value;
    }

    bool Int16::operator>=(const int16_t &value) const {
        return _value >= value;
    }

    bool Int16::operator<(const int16_t &value) const {
        return _value < value;
    }

    bool Int16::operator<=(const int16_t &value) const {
        return _value <= value;
    }

    Int16 Int16::operator+=(const int16_t &value) {
        _value += value;
        return *this;
    }

    Int16 Int16::operator-=(const int16_t &value) {
        _value -= value;
        return *this;
    }

    Int16 Int16::operator+(const int16_t &value) {
        Int16 result = _value;
        result._value += value;
        return result;
    }

    Int16 Int16::operator-(const int16_t &value) {
        Int16 result = _value;
        result._value -= value;
        return result;
    }

    int16_t Int16::operator++()  // ++i
    {
        return ++_value;
    }

    int16_t Int16::operator++(int)   // i++
    {
        return _value++;
    }

    int16_t Int16::operator--()  // --i
    {
        return --_value;
    }

    int16_t Int16::operator--(int)   // i--
    {
        return _value--;
    }

    void Int16::write(Stream *stream, bool bigEndian) const {
        stream->writeInt16(_value, bigEndian);
    }

    void Int16::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt16(bigEndian);
    }

    const String Int16::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRId16, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int Int16::compareTo(Int16 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool Int16::parse(const String &str, Int16 &value, bool decimal) {
        //        uint len = 0;
        //        if(sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId16 "%n" : "%" PRIx16 "%n", &value._value, &len) == 1 && str.length() == len)
        //        {
        //            return true;
        //        }
        //        return false;
        Int32 temp;
        if (Int32::parse(str, temp, decimal) &&
            temp >= (int32_t) MinValue && temp <= (int32_t) MaxValue) {
            value._value = (short) temp;
            return true;
        }
        return false;
    }

    bool Int16::parse(const String &str, int16_t &value, bool decimal) {
        Int16 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const UInt16 UInt16::MaxValue = UInt16(UINT16_MAX);
    const UInt16 UInt16::MinValue = UInt16(0);

    UInt16::UInt16() : _value(0) {
    }

    UInt16::UInt16(
            const UInt16 &value) : _value(value._value) {
    }

    UInt16::UInt16(
            const uint16_t &value) : _value(value) {
    }

    UInt16::~UInt16() {
    }

    UInt16::operator uint16_t() const {
        return _value;
    }

    UInt16 &UInt16::operator=(const UInt16 &value) {
        _value = value._value;
        return *this;
    }

    bool UInt16::operator==(const UInt16 &value) const {
        return _value == value._value;
    }

    bool UInt16::operator!=(const UInt16 &value) const {
        return !operator==(value);
    }

    bool UInt16::operator>(const UInt16 &value) const {
        return _value > value._value;
    }

    bool UInt16::operator>=(const UInt16 &value) const {
        return _value >= value._value;
    }

    bool UInt16::operator<(const UInt16 &value) const {
        return _value < value._value;
    }

    bool UInt16::operator<=(const UInt16 &value) const {
        return _value <= value._value;
    }

    UInt16 UInt16::operator+=(const UInt16 &value) {
        _value += value._value;
        return *this;
    }

    UInt16 UInt16::operator-=(const UInt16 &value) {
        _value -= value._value;
        return *this;
    }

    UInt16 UInt16::operator+(const UInt16 &value) {
        UInt16 result = _value;
        result._value += value._value;
        return result;
    }

    UInt16 UInt16::operator-(const UInt16 &value) {
        UInt16 result = _value;
        result._value -= value._value;
        return result;
    }

    UInt16 &UInt16::operator=(const uint16_t &value) {
        _value = value;
        return *this;
    }

    bool UInt16::operator==(const uint16_t &value) const {
        return _value == value;
    }

    bool UInt16::operator!=(const uint16_t &value) const {
        return !operator==(value);
    }

    bool UInt16::operator>(const uint16_t &value) const {
        return _value > value;
    }

    bool UInt16::operator>=(const uint16_t &value) const {
        return _value >= value;
    }

    bool UInt16::operator<(const uint16_t &value) const {
        return _value < value;
    }

    bool UInt16::operator<=(const uint16_t &value) const {
        return _value <= value;
    }

    UInt16 UInt16::operator+=(const uint16_t &value) {
        _value += value;
        return *this;
    }

    UInt16 UInt16::operator-=(const uint16_t &value) {
        _value -= value;
        return *this;
    }

    UInt16 UInt16::operator+(const uint16_t &value) {
        UInt16 result = _value;
        result._value += value;
        return result;
    }

    UInt16 UInt16::operator-(const uint16_t &value) {
        UInt16 result = _value;
        result._value -= value;
        return result;
    }

    uint16_t UInt16::operator++()  // ++i
    {
        return ++_value;
    }

    uint16_t UInt16::operator++(int)   // i++
    {
        return _value++;
    }

    uint16_t UInt16::operator--()  // --i
    {
        return --_value;
    }

    uint16_t UInt16::operator--(int)   // i--
    {
        return _value--;
    }

    void UInt16::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt16(_value, bigEndian);
    }

    void UInt16::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt16(bigEndian);
    }

    const String UInt16::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRIu16, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int UInt16::compareTo(UInt16 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool UInt16::parse(const String &str, UInt16 &value, bool decimal) {
        //        uint len = 0;
        //        if(sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId16 "%n" : "%" PRIx16 "%n", &value._value, &len) == 1 && str.length() == len)
        //        {
        //            return true;
        //        }
        //        return false;
        UInt32 temp;
        if (UInt32::parse(str, temp, decimal) &&
            temp >= (uint32_t) MinValue && temp <= (uint32_t) MaxValue) {
            value._value = (uint32_t) temp;
            return true;
        }
        return false;
    }

    bool UInt16::parse(const String &str, uint16_t &value, bool decimal) {
        UInt16 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const Int32 Int32::MaxValue = Int32(INT32_MAX);
    const Int32 Int32::MinValue = Int32(INT32_MIN);

    Int32::Int32() : _value(0) {
    }

    Int32::Int32(
            const Int32 &value) : _value(value._value) {
    }

    Int32::Int32(
            const int32_t &value) : _value(value) {
    }

    Int32::~Int32() {
    }

    Int32::operator int32_t() const {
        return _value;
    }

    Int32 &Int32::operator=(const Int32 &value) {
        _value = value._value;
        return *this;
    }

    bool Int32::operator==(const Int32 &value) const {
        return _value == value._value;
    }

    bool Int32::operator!=(const Int32 &value) const {
        return !operator==(value);
    }

    bool Int32::operator>(const Int32 &value) const {
        return _value > value._value;
    }

    bool Int32::operator>=(const Int32 &value) const {
        return _value >= value._value;
    }

    bool Int32::operator<(const Int32 &value) const {
        return _value < value._value;
    }

    bool Int32::operator<=(const Int32 &value) const {
        return _value <= value._value;
    }

    Int32 Int32::operator+=(const Int32 &value) {
        _value += value._value;
        return *this;
    }

    Int32 Int32::operator-=(const Int32 &value) {
        _value -= value._value;
        return *this;
    }

    Int32 Int32::operator+(const Int32 &value) {
        Int32 result = _value;
        result._value += value._value;
        return result;
    }

    Int32 Int32::operator-(const Int32 &value) {
        Int32 result = _value;
        result._value -= value._value;
        return result;
    }

    Int32 &Int32::operator=(const int32_t &value) {
        _value = value;
        return *this;
    }

    bool Int32::operator==(const int32_t &value) const {
        return _value == value;
    }

    bool Int32::operator!=(const int32_t &value) const {
        return !operator==(value);
    }

    bool Int32::operator>(const int32_t &value) const {
        return _value > value;
    }

    bool Int32::operator>=(const int32_t &value) const {
        return _value >= value;
    }

    bool Int32::operator<(const int32_t &value) const {
        return _value < value;
    }

    bool Int32::operator<=(const int32_t &value) const {
        return _value <= value;
    }

    Int32 Int32::operator+=(const int32_t &value) {
        _value += value;
        return *this;
    }

    Int32 Int32::operator-=(const int32_t &value) {
        _value -= value;
        return *this;
    }

    Int32 Int32::operator+(const int32_t &value) {
        Int32 result = _value;
        result._value += value;
        return result;
    }

    Int32 Int32::operator-(const int32_t &value) {
        Int32 result = _value;
        result._value -= value;
        return result;
    }

    int32_t Int32::operator++()  // ++i
    {
        return ++_value;
    }

    int32_t Int32::operator++(int)   // i++
    {
        return _value++;
    }

    int32_t Int32::operator--()  // --i
    {
        return --_value;
    }

    int32_t Int32::operator--(int)   // i--
    {
        return _value--;
    }

    void Int32::write(Stream *stream, bool bigEndian) const {
        stream->writeInt32(_value, bigEndian);
    }

    void Int32::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt32(bigEndian);
    }

    const String Int32::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRId32, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format.c_str(), _value);
            return temp;
        }
    }

    int Int32::compareTo(Int32 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool Int32::parse(const String &str, Int32 &value, bool decimal) {
        uint len = 0;
        if (sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId32 "%n" : "%" PRIx32 "%n",
                   &value._value, &len) == 1 && str.length() == len) {
            return true;
        }
        return false;
    }

    bool Int32::parse(const String &str, int32_t &value, bool decimal) {
        Int32 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    bool Int32::isHexInteger(const String &str) {
        if (str.length() <= 2)
            return false;

        if (!String::equals(str.substr(0, 2), "0x", false)) {
            return false;
        }

        Int32 value;
        return Int32::parse(str.substr(2, str.length() - 2), value);
    }

    const UInt32 UInt32::MaxValue = UInt32(UINT32_MAX);
    const UInt32 UInt32::MinValue = UInt32(0);

    UInt32::UInt32() : _value(0) {
    }

    UInt32::UInt32(
            const UInt32 &value) : _value(value._value) {
    }

    UInt32::UInt32(
            const uint32_t &value) : _value(value) {
    }

    UInt32::~UInt32() {
    }

    UInt32::operator uint32_t() const {
        return _value;
    }

    UInt32 &UInt32::operator=(const UInt32 &value) {
        _value = value._value;
        return *this;
    }

    bool UInt32::operator==(const UInt32 &value) const {
        return _value == value._value;
    }

    bool UInt32::operator!=(const UInt32 &value) const {
        return !operator==(value);
    }

    bool UInt32::operator>(const UInt32 &value) const {
        return _value > value._value;
    }

    bool UInt32::operator>=(const UInt32 &value) const {
        return _value >= value._value;
    }

    bool UInt32::operator<(const UInt32 &value) const {
        return _value < value._value;
    }

    bool UInt32::operator<=(const UInt32 &value) const {
        return _value <= value._value;
    }

    UInt32 UInt32::operator+=(const UInt32 &value) {
        _value += value._value;
        return *this;
    }

    UInt32 UInt32::operator-=(const UInt32 &value) {
        _value -= value._value;
        return *this;
    }

    UInt32 UInt32::operator+(const UInt32 &value) {
        UInt32 result = _value;
        result._value += value._value;
        return result;
    }

    UInt32 UInt32::operator-(const UInt32 &value) {
        UInt32 result = _value;
        result._value -= value._value;
        return result;
    }

    UInt32 &UInt32::operator=(const uint32_t &value) {
        _value = value;
        return *this;
    }

    bool UInt32::operator==(const uint32_t &value) const {
        return _value == value;
    }

    bool UInt32::operator!=(const uint32_t &value) const {
        return !operator==(value);
    }

    bool UInt32::operator>(const uint32_t &value) const {
        return _value > value;
    }

    bool UInt32::operator>=(const uint32_t &value) const {
        return _value >= value;
    }

    bool UInt32::operator<(const uint32_t &value) const {
        return _value < value;
    }

    bool UInt32::operator<=(const uint32_t &value) const {
        return _value <= value;
    }

    UInt32 UInt32::operator+=(const uint32_t &value) {
        _value += value;
        return *this;
    }

    UInt32 UInt32::operator-=(const uint32_t &value) {
        _value -= value;
        return *this;
    }

    UInt32 UInt32::operator+(const uint32_t &value) {
        UInt32 result = _value;
        result._value += value;
        return result;
    }

    UInt32 UInt32::operator-(const uint32_t &value) {
        UInt32 result = _value;
        result._value -= value;
        return result;
    }

    uint32_t UInt32::operator++()  // ++i
    {
        return ++_value;
    }

    uint32_t UInt32::operator++(int)   // i++
    {
        return _value++;
    }

    uint32_t UInt32::operator--()  // --i
    {
        return --_value;
    }

    uint32_t UInt32::operator--(int)   // i--
    {
        return _value--;
    }

    void UInt32::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt32(_value, bigEndian);
    }

    void UInt32::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt32(bigEndian);
    }

    const String UInt32::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRIu32, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    bool UInt32::parse(const String &str, UInt32 &value, bool decimal) {
        uint len = 0;
        if (sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRIu32 "%n" : "%" PRIx32 "%n",
                   &value._value, &len) == 1 && str.length() == len) {
            return true;
        }
        return false;
    }

    bool UInt32::parse(const String &str, uint32_t &value, bool decimal) {
        UInt32 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const Int64 Int64::MaxValue = Int64(INT64_MAX);
    const Int64 Int64::MinValue = Int64(INT64_MIN);

    Int64::Int64() : _value(0) {
    }

    Int64::Int64(
            const Int64 &value) : _value(value._value) {
    }

    Int64::Int64(
            const int64_t &value) : _value(value) {
    }

    Int64::~Int64() {
    }

    Int64::operator int64_t() const {
        return _value;
    }

    Int64 &Int64::operator=(const Int64 &value) {
        _value = value._value;
        return *this;
    }

    bool Int64::operator==(const Int64 &value) const {
        return _value == value._value;
    }

    bool Int64::operator!=(const Int64 &value) const {
        return !operator==(value);
    }

    bool Int64::operator>(const Int64 &value) const {
        return _value > value._value;
    }

    bool Int64::operator>=(const Int64 &value) const {
        return _value >= value._value;
    }

    bool Int64::operator<(const Int64 &value) const {
        return _value < value._value;
    }

    bool Int64::operator<=(const Int64 &value) const {
        return _value <= value._value;
    }

    Int64 Int64::operator+=(const Int64 &value) {
        _value += value._value;
        return *this;
    }

    Int64 Int64::operator-=(const Int64 &value) {
        _value -= value._value;
        return *this;
    }

    Int64 Int64::operator+(const Int64 &value) {
        Int64 result = _value;
        result._value += value._value;
        return result;
    }

    Int64 Int64::operator-(const Int64 &value) {
        Int64 result = _value;
        result._value -= value._value;
        return result;
    }

    Int64 &Int64::operator=(const int64_t &value) {
        _value = value;
        return *this;
    }

    bool Int64::operator==(const int64_t &value) const {
        return _value == value;
    }

    bool Int64::operator!=(const int64_t &value) const {
        return !operator==(value);
    }

    bool Int64::operator>(const int64_t &value) const {
        return _value > value;
    }

    bool Int64::operator>=(const int64_t &value) const {
        return _value >= value;
    }

    bool Int64::operator<(const int64_t &value) const {
        return _value < value;
    }

    bool Int64::operator<=(const int64_t &value) const {
        return _value <= value;
    }

    Int64 Int64::operator+=(const int64_t &value) {
        _value += value;
        return *this;
    }

    Int64 Int64::operator-=(const int64_t &value) {
        _value -= value;
        return *this;
    }

    Int64 Int64::operator+(const int64_t &value) {
        Int64 result = _value;
        result._value += value;
        return result;
    }

    Int64 Int64::operator-(const int64_t &value) {
        Int64 result = _value;
        result._value -= value;
        return result;
    }

    int64_t Int64::operator++()  // ++i
    {
        return ++_value;
    }

    int64_t Int64::operator++(int)   // i++
    {
        return _value++;
    }

    int64_t Int64::operator--()  // --i
    {
        return --_value;
    }

    int64_t Int64::operator--(int)   // i--
    {
        return _value--;
    }

    void Int64::write(Stream *stream, bool bigEndian) const {
        stream->writeInt64(_value, bigEndian);
    }

    void Int64::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt64(bigEndian);
    }

    const String Int64::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRId64, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int Int64::compareTo(Int64 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool Int64::parse(const String &str, Int64 &value, bool decimal) {
        uint len = 0;
        if (sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRId64 "%n" : "%" PRIx64 "%n",
                   &value._value, &len) == 1 && str.length() == len) {
            return true;
        }
        return false;
    }

    bool Int64::parse(const String &str, int64_t &value, bool decimal) {
        Int64 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const UInt64 UInt64::MaxValue = UInt64(UINT64_MAX);
    const UInt64 UInt64::MinValue = UInt64(0);

    UInt64::UInt64() : _value(0) {
    }

    UInt64::UInt64(
            const UInt64 &value) : _value(value._value) {
    }

    UInt64::UInt64(
            const uint64_t &value) : _value(value) {
    }

    UInt64::~UInt64() {
    }

    UInt64::operator uint64_t() const {
        return _value;
    }

    UInt64 &UInt64::operator=(const UInt64 &value) {
        _value = value._value;
        return *this;
    }

    bool UInt64::operator==(const UInt64 &value) const {
        return _value == value._value;
    }

    bool UInt64::operator!=(const UInt64 &value) const {
        return !operator==(value);
    }

    bool UInt64::operator>(const UInt64 &value) const {
        return _value > value._value;
    }

    bool UInt64::operator>=(const UInt64 &value) const {
        return _value >= value._value;
    }

    bool UInt64::operator<(const UInt64 &value) const {
        return _value < value._value;
    }

    bool UInt64::operator<=(const UInt64 &value) const {
        return _value <= value._value;
    }

    UInt64 UInt64::operator+=(const UInt64 &value) {
        _value += value._value;
        return *this;
    }

    UInt64 UInt64::operator-=(const UInt64 &value) {
        _value -= value._value;
        return *this;
    }

    UInt64 UInt64::operator+(const UInt64 &value) {
        UInt64 result = _value;
        result._value += value._value;
        return result;
    }

    UInt64 UInt64::operator-(const UInt64 &value) {
        UInt64 result = _value;
        result._value -= value._value;
        return result;
    }

    UInt64 &UInt64::operator=(const uint64_t &value) {
        _value = value;
        return *this;
    }

    bool UInt64::operator==(const uint64_t &value) const {
        return _value == value;
    }

    bool UInt64::operator!=(const uint64_t &value) const {
        return !operator==(value);
    }

    bool UInt64::operator>(const uint64_t &value) const {
        return _value > value;
    }

    bool UInt64::operator>=(const uint64_t &value) const {
        return _value >= value;
    }

    bool UInt64::operator<(const uint64_t &value) const {
        return _value < value;
    }

    bool UInt64::operator<=(const uint64_t &value) const {
        return _value <= value;
    }

    UInt64 UInt64::operator+=(const uint64_t &value) {
        _value += value;
        return *this;
    }

    UInt64 UInt64::operator-=(const uint64_t &value) {
        _value -= value;
        return *this;
    }

    UInt64 UInt64::operator+(const uint64_t &value) {
        UInt64 result = _value;
        result._value += value;
        return result;
    }

    UInt64 UInt64::operator-(const uint64_t &value) {
        UInt64 result = _value;
        result._value -= value;
        return result;
    }

    uint64_t UInt64::operator++()  // ++i
    {
        return ++_value;
    }

    uint64_t UInt64::operator++(int)   // i++
    {
        return _value++;
    }

    uint64_t UInt64::operator--()  // --i
    {
        return --_value;
    }

    uint64_t UInt64::operator--(int)   // i--
    {
        return _value--;
    }

    void UInt64::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt64(_value, bigEndian);
    }

    void UInt64::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt64(bigEndian);
    }

    const String UInt64::toString(const String &format) const {
        if (format.isNullOrEmpty()) {
            char temp[255];
            sprintf(temp, "%" PRIu64, _value);
            return temp;
        } else {
            char temp[255];
            sprintf(temp, format, _value);
            return temp;
        }
    }

    int UInt64::compareTo(UInt64 value) const {
        // Need to use compare because subtraction will wrap
        // to positive for very large neg numbers, etc.
        if (_value < value) return -1;
        if (_value > value) return 1;
        return 0;
    }

    bool UInt64::parse(const String &str, UInt64 &value, bool decimal) {
        uint len = 0;
        if (sscanf(str.c_str(), decimal && !Int32::isHexInteger(str) ? "%" PRIu64 "%n" : "%" PRIx64 "%n",
                   &value._value, &len) == 1 && str.length() == len) {
            return true;
        }
        return false;
    }

    bool UInt64::parse(const String &str, uint64_t &value, bool decimal) {
        UInt64 temp;
        if (parse(str, temp, decimal)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    const Float Float::MaxValue = Float((float) 3.40282346638528859e+38);
    const Float Float::MinValue = Float((float) -3.40282346638528859e+38);
    const Float Float::Epsilon = Float((float) 1.4e-45);
#ifdef WIN32
    const Float Float::NegativeInfinity = Float((float) HUGE_VALF);
    const Float Float::PositiveInfinity = Float((float) -HUGE_VALF);
#else
    const Float Float::NegativeInfinity = Float(HUGE_VAL);
    const Float Float::PositiveInfinity = Float(-HUGE_VAL);
#endif
    const Float Float::NaN = Float(NAN);

    Float::Float() : _value(0.0f) {
    }

    Float::Float(
            const Float &value) : _value(value._value) {
    }

    Float::Float(
            const float &value) : _value(value) {
    }

    Float::~Float() {
    }

    Float::operator float() const {
        return _value;
    }

    Float &Float::operator=(const Float &value) {
        _value = value._value;
        return *this;
    }

    bool Float::operator==(const Float &value) const {
        if (isNaN() && value.isNaN())
            return true;
        return _value == value._value;
    }

    bool Float::operator!=(const Float &value) const {
        return !operator==(value);
    }

    bool Float::operator>(const Float &value) const {
        return _value > value._value;
    }

    bool Float::operator>=(const Float &value) const {
        return _value >= value._value;
    }

    bool Float::operator<(const Float &value) const {
        return _value < value._value;
    }

    bool Float::operator<=(const Float &value) const {
        return _value <= value._value;
    }

    Float Float::operator+=(const Float &value) {
        _value += value._value;
        return *this;
    }

    Float Float::operator-=(const Float &value) {
        _value -= value._value;
        return *this;
    }

    Float Float::operator+(const Float &value) {
        Float result = _value;
        result._value += value._value;
        return result;
    }

    Float Float::operator-(const Float &value) {
        Float result = _value;
        result._value -= value._value;
        return result;
    }

    Float &Float::operator=(const float &value) {
        _value = value;
        return *this;
    }

    bool Float::operator==(const float &value) const {
        if (isNaN() && Float::isNaN(value))
            return true;
        return _value == value;
    }

    bool Float::operator!=(const float &value) const {
        return !operator==(value);
    }

    bool Float::operator>(const float &value) const {
        return _value > value;
    }

    bool Float::operator>=(const float &value) const {
        return _value >= value;
    }

    bool Float::operator<(const float &value) const {
        return _value < value;
    }

    bool Float::operator<=(const float &value) const {
        return _value <= value;
    }

    Float Float::operator+=(const float &value) {
        _value += value;
        return *this;
    }

    Float Float::operator-=(const float &value) {
        _value -= value;
        return *this;
    }

    Float Float::operator+(const float &value) {
        Float result = _value;
        result._value += value;
        return result;
    }

    Float Float::operator-(const float &value) {
        Float result = _value;
        result._value -= value;
        return result;
    }

    float Float::operator++()  // ++i
    {
        return ++_value;
    }

    float Float::operator++(int)   // i++
    {
        return _value++;
    }

    float Float::operator--()  // --i
    {
        return --_value;
    }

    float Float::operator--(int)   // i--
    {
        return _value--;
    }

    void Float::write(Stream *stream, bool bigEndian) const {
        stream->writeDouble(_value, bigEndian);
    }

    void Float::read(Stream *stream, bool bigEndian) {
        _value = stream->readFloat(bigEndian);
    }

    const String Float::toString(const String &format) const {
        char temp[255];
        if (format.isNullOrEmpty()) {
            sprintf(temp, "%g", _value);
        } else {
            StringArray texts;
            StringArray::parse(format, texts, '.');
            const String &integerStr = texts.count() > 0 ? texts[0] : String::Empty;
            int integerSize = 0;
            for (size_t i = 0; i < integerStr.length(); i++) {
                if (integerStr[i] == '0')
                    integerSize++;
            }

            const String &pointStr = texts.count() > 1 ? texts[1] : String::Empty;
            int pointSize = 0;
            for (size_t i = 0; i < pointStr.length(); i++) {
                if (pointStr[i] == '0')
                    pointSize++;
            }

            if (pointSize >= 0 && pointSize <= 7) {
                sprintf(temp, "%0*.*f", integerSize + pointSize + (pointSize > 0 ? 1 : 0), pointSize, _value);
            } else {
                sprintf(temp, "%g", _value);
            }
        }
        return temp;
    }

    const String Float::toString(int pointSize) const {
        char temp[255];
        if (pointSize >= 0 && pointSize <= 7) {
            float value = _value;
            float intValue = Math::abs(_value * Math::pow(10.0f, (float) pointSize));
            if (intValue < 1.0f)
                value = 0.0f;

            sprintf(temp, "%.*f", pointSize, value);
        } else {
            sprintf(temp, "%g", _value);
        }
        return temp;
    }

    bool Float::isNaN() const {
        return Float::isNaN(*this);
    }

    bool Float::isFinite(const Float &value) {
        return isfinite(value._value) == 0;
    }

    bool Float::parse(const String &str, Float &value) {
        uint len = 0;
        // fixed bug, removed 'f' or 'F'
        String text = String::trim(str, 'f', 'F');
        if (sscanf(text.c_str(), "%f%n", &value._value, &len) == 1 && text.length() == len) {
            return true;
        }
        return false;
    }

    bool Float::parse(const String &str, float &value) {
        Float temp;
        if (parse(str, temp)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    bool Float::isNaN(const Float &value) {
        return isnan(value._value);
    }

    bool Float::isFinite() const {
        return Float::isFinite(*this);
    }

    const Double Double::MaxValue = Double(1.7976931348623157E+308);
    const Double Double::MinValue = Double(-1.7976931348623157E+308);
    const Double Double::Epsilon = Double(4.9406564584124654E-324);
    const Double Double::NegativeInfinity = Double(HUGE_VAL);
    const Double Double::PositiveInfinity = Double(-HUGE_VAL);
    const Double Double::NaN = Double(NAN);
    const Double Double::Infinity = Double(INFINITY);

    Double::Double() : _value(NAN) {
    }

    Double::Double(
            const Double &value) : _value(value._value) {
    }

    Double::Double(
            const double &value) : _value(value) {
    }

    Double::~Double() {
    }

    Double::operator double() const {
        return _value;
    }

    Double &Double::operator=(const Double &value) {
        _value = value._value;
        return *this;
    }

    bool Double::operator==(const Double &value) const {
        if (isNaN() && value.isNaN())
            return true;
        return _value == value._value;
    }

    bool Double::operator!=(const Double &value) const {
        return !operator==(value);
    }

    bool Double::operator>(const Double &value) const {
        return _value > value._value;
    }

    bool Double::operator>=(const Double &value) const {
        return _value >= value._value;
    }

    bool Double::operator<(const Double &value) const {
        return _value < value._value;
    }

    bool Double::operator<=(const Double &value) const {
        return _value <= value._value;
    }

    Double Double::operator+=(const Double &value) {
        _value += value._value;
        return *this;
    }

    Double Double::operator-=(const Double &value) {
        _value -= value._value;
        return *this;
    }

    Double Double::operator+(const Double &value) {
        Double result = _value;
        result._value += value._value;
        return result;
    }

    Double Double::operator-(const Double &value) {
        Double result = _value;
        result._value -= value._value;
        return result;
    }

    Double &Double::operator=(const double &value) {
        _value = value;
        return *this;
    }

    bool Double::operator==(const double &value) const {
        if (isNaN() && Double::isNaN(value))
            return true;
        return _value == value;
    }

    bool Double::operator!=(const double &value) const {
        return !operator==(value);
    }

    bool Double::operator>(const double &value) const {
        return _value > value;
    }

    bool Double::operator>=(const double &value) const {
        return _value >= value;
    }

    bool Double::operator<(const double &value) const {
        return _value < value;
    }

    bool Double::operator<=(const double &value) const {
        return _value <= value;
    }

    Double Double::operator+=(const double &value) {
        _value += value;
        return *this;
    }

    Double Double::operator-=(const double &value) {
        _value -= value;
        return *this;
    }

    Double Double::operator+(const double &value) {
        Double result = _value;
        result._value += value;
        return result;
    }

    Double Double::operator-(const double &value) {
        Double result = _value;
        result._value -= value;
        return result;
    }

    double Double::operator++()  // ++i
    {
        return ++_value;
    }

    double Double::operator++(int)   // i++
    {
        return _value++;
    }

    double Double::operator--()  // --i
    {
        return --_value;
    }

    double Double::operator--(int)   // i--
    {
        return _value--;
    }

    void Double::write(Stream *stream, bool bigEndian) const {
        stream->writeDouble(_value, bigEndian);
    }

    void Double::read(Stream *stream, bool bigEndian) {
        _value = stream->readDouble(bigEndian);
    }

    const String Double::toString(const String &format) const {
        char temp[255];
        if (format.isNullOrEmpty()) {
            sprintf(temp, "%lg", _value);
        } else {
            StringArray texts;
            StringArray::parse(format, texts, '.');
            const String &integerStr = texts.count() > 0 ? texts[0] : String::Empty;
            int integerSize = 0;
            for (size_t i = 0; i < integerStr.length(); i++) {
                if (integerStr[i] == '0')
                    integerSize++;
            }

            const String &pointStr = texts.count() > 1 ? texts[1] : String::Empty;
            int pointSize = 0;
            for (size_t i = 0; i < pointStr.length(); i++) {
                if (pointStr[i] == '0')
                    pointSize++;
            }

            if (pointSize >= 0 && pointSize <= 15) {
                sprintf(temp, "%0*.*lf", integerSize + pointSize + (pointSize > 0 ? 1 : 0), pointSize, _value);
            } else {
                sprintf(temp, "%lg", _value);
            }
        }
        return temp;
    }

    const String Double::toString(int pointSize) const {
        char temp[255];
        if (pointSize >= 0 && pointSize <= 15) {
            double value = _value;
            double intValue = Math::abs(_value * Math::pow(10.0, (double) pointSize));
            if (intValue < 1.0)
                value = 0.0;

            sprintf(temp, "%.*lf", pointSize, value);
        } else {
            sprintf(temp, "%lg", _value);
        }
        return temp;
    }

    bool Double::isNaN() const {
        return Double::isNaN(*this);
    }

    bool Double::isFinite() const {
        return Double::isFinite(*this);
    }

    bool Double::parse(const String &str, Double &value) {
        uint len = 0;
        if (sscanf(str.c_str(), "%lf%n", &value._value, &len) == 1 && str.length() == len) {
            return true;
        }
        return false;
    }

    bool Double::parse(const String &str, double &value) {
        Double temp;
        if (parse(str, temp)) {
            value = temp._value;
            return true;
        }
        return false;
    }

    bool Double::isNaN(const Double &value) {
        return isnan(value._value);
    }

    bool Double::isFinite(const Double &value) {
        return isfinite(value._value) == 0;
    }

    const PositionCoord PositionCoord::Empty = PositionCoord();

    PositionCoord::PositionCoord(double
                                 longitude, double
                                 latitude) {
        this->longitude = longitude;
        this->latitude = latitude;
    }

    PositionCoord::PositionCoord(
            const PositionCoord &coord) {
        this->longitude = coord.longitude;
        this->latitude = coord.latitude;
    }

    PositionCoord::PositionCoord(
            const String &str) {
        PositionCoord coord;
        if (PositionCoord::parse(str, coord)) {
            this->operator=(coord);
        }
    }

    PositionCoord::PositionCoord(
            const char *str) : PositionCoord(String(str)) {
    }

    bool PositionCoord::isEmpty() const {
        return Double::isNaN(longitude) && Double::isNaN(latitude);
    }

    void PositionCoord::empty() {
        longitude = Double::NaN;
        latitude = Double::NaN;
    }

    const String PositionCoord::toString() const {
        if (isEmpty())
            return String::Empty;
        else
            return String::convert("%s,%s",
                                   Double(longitude).toString().c_str(),
                                   Double(latitude).toString().c_str());
    }

    bool PositionCoord::parse(const String &str, PositionCoord &value) {
        StringArray texts;
        Convert::splitStr(str, ',', texts);
        if (texts.count() != 2)
            Convert::splitStr(str, ';', texts);
        if (texts.count() != 2)
            Convert::splitStr(str, '|', texts);
        if (texts.count() == 2) {
            String longitudeStr = texts[0].trim();
            String latitudeStr = texts[1].trim();

            return Double::parse(longitudeStr, value.longitude) &&
                   Double::parse(latitudeStr, value.latitude);
        }
        return false;
    }

    void PositionCoord::operator=(const PositionCoord &value) {
        longitude = value.longitude;
        latitude = value.latitude;
    }

    bool PositionCoord::operator==(const PositionCoord &value) const {
        return longitude == value.longitude &&
               latitude == value.latitude;
    }

    bool PositionCoord::operator!=(const PositionCoord &value) const {
        return !operator==(value);
    }

    void PositionCoord::write(Stream *stream, bool bigEndian) const {
        Double(longitude).write(stream, bigEndian);
        Double(latitude).write(stream, bigEndian);
    }

    void PositionCoord::read(Stream *stream, bool bigEndian) {
        Double longitude, latitude;
        longitude.read(stream, bigEndian);
        latitude.read(stream, bigEndian);

        this->latitude = latitude;
        this->longitude = longitude;
    }
}
