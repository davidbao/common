//
//  ValueType.cpp
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#include <cctype>
#include <cstdarg>
#include <cinttypes>
#include "iconv.h"
#include "data/String.h"
#include "data/ByteArray.h"
#include "data/WString.h"
#include "IO/Stream.h"
#include "system/Math.h"

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

    String::String(size_t capacity) : _buffer(capacity) {
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

    String::~String() = default;

    void String::setString(const String &value) {
        _buffer.clear();
        addString(value.c_str());
    }

    void String::setString(const char *value, size_t count) {
        _buffer.clear();
        addString(value, count);
    }

    void String::addString(const char *value, size_t count) {
        if (value != nullptr) {
            if(count == 0) {
                count = strlen(value);
            }
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

    int String::compare(const String &value1, const string &value2, bool ignoreCase) {
        return String::compare(value1, value2.c_str(), ignoreCase);
    }

    int String::compare(const String &value1, const char *value2, bool ignoreCase) {
        return String::compare(value1, String(value2), ignoreCase);
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
        writeFixedLengthStr(stream, len);
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

    void String::writeFixedLengthStr(Stream *stream, size_t length) const {
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

    String::operator WString() const {
        return WString::convert(*this);
    }

    bool String::equals(const String &other) const {
        return String::equals(*this, other);
    }

    bool String::equals(const char* other) const {
        return String::equals(*this, other);
    }

    bool String::equals(const string &other) const {
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

    int String::compareTo(const string &other) const {
        return String::compare(*this, other);
    }

    int String::compareTo(const string &other, bool ignoreCase) const {
        return String::compare(*this, other, ignoreCase);
    }

    int String::compareTo(const char *other) const {
        return String::compare(*this, other);
    }

    int String::compareTo(const char *other, bool ignoreCase) const {
        return String::compare(*this, other, ignoreCase);
    }

    String
    String::trim(const String &str, char trimChar1, char trimChar2, char trimChar3,
                 char trimChar4) {
        Vector<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (char i : temp) {
            if (i != '\0') {
                trimChars.add(i);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimBoth);
    }

    String String::trimStart(const String &str, char trimChar1, char trimChar2, char trimChar3,
                             char trimChar4) {
        Vector<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (char i : temp) {
            if (i != '\0') {
                trimChars.add(i);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimHead);
    }

    String
    String::trimEnd(const String &str, char trimChar1, char trimChar2, char trimChar3,
                    char trimChar4) {
        Vector<char> trimChars;
        const char temp[] = {trimChar1, trimChar2, trimChar3, trimChar4};
        for (char i : temp) {
            if (i != '\0') {
                trimChars.add(i);
            }
        }
        return trimInner(str, trimChars, TrimType::TrimTail);
    }

    String
    String::trimInner(const String &str, const Vector<char> &trimChars, TrimType trimType) {
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
        if (cd != (iconv_t) -1) {
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
        size_t len = (inputLength) / 3 * 4;          // the base length - we want integer division here.
        len += ((inputLength % 3) != 0) ? 4 : 0;         // at most 4 more chars for the remainder

        if (len == 0)
            return 0;

        if (insertLineBreaks) {
            size_t newLines = len / base64LineBreakPosition;
            if ((len % base64LineBreakPosition) == 0) {
                --newLines;
            }
            len += newLines * 2;              // the number of line break chars we'll add, "\r\n"
        }

        return len;
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
            default:
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
            return {(const char *) array.data(), array.count()};
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
            auto *decodedBytes = new uint8_t[resultLength];

            // Convert Base64 chars into bytes:
            size_t actualResultLength = fromBase64_Decode(inputPtr, inputLength, decodedBytes, resultLength);
            array.addRange(decodedBytes, resultLength);
            delete[] decodedBytes;

            return actualResultLength > 0;
        }
        // Note that actualResultLength can differ from resultLength if the caller is modifying the array
        // as it is being converted. Silently ignore the failure.
        // Consider throwing exception in a non in-place release.

        // We are done:
        return false;
    }

    /// <summary>
    /// Compute the number of bytes encoded in the specified Base 64 char array:
    /// Walk the entire input counting white spaces and padding chars, then compute result length
    /// based on 3 bytes per 4 chars.
    /// </summary>
    size_t String::fromBase64_ComputeResultLength(const char *inputPtr, size_t inputLength) {
        const auto intEq = (size_t) '=';
        const auto intSpace = (size_t) ' ';

        assert(0 <= inputLength);

        const char *inputEndPtr = inputPtr + inputLength;
        size_t usefulInputLength = inputLength;
        int padding = 0;

        while (inputPtr < inputEndPtr) {
            auto c = (size_t) (*inputPtr);
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

        const auto intA = (size_t) 'A';
        const auto inta = (size_t) 'a';
        const auto int0 = (size_t) '0';
        const auto intEq = (size_t) '=';
        const auto intPlus = (size_t) '+';
        const auto intSlash = (size_t) '/';
        const auto intSpace = (size_t) ' ';
        const auto intTab = (size_t) '\t';
        const auto intNLn = (size_t) '\n';
        const auto intCRt = (size_t) '\r';
        const auto intAtoZ = (size_t) ('Z' - 'A');  // = ('z' - 'a')
        const auto int0to9 = (size_t) ('9' - '0');

        const char *inputPtr = startInputPtr;
        uint8_t *destPtr = startDestPtr;

        // Pointers to the end of input and output:
        const char *endInputPtr = inputPtr + inputLength;
        uint8_t *endDestPtr = destPtr + destLength;

        // Current char code/value:
        size_t currCode;

        // This 4-uint8_t integer will contain the 4 codes of the current 4-char group.
        // Each char codes for 6 bits = 24 bits.
        // The remaining uint8_t will be FF, we use it as a marker when 4 chars have been processed.
        size_t currBlockCodes = 0x000000FFu;

        while (true) {
            // break when done:
            if (inputPtr >= endInputPtr)
                goto _AllInputConsumed;

            // Get current char:
            currCode = (size_t) (*inputPtr);
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
            // Otherwise, we would be rejecting input such as "abc= =":
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
        const char *first = ::strstr(substring, str.c_str());
        if (nullptr == first) return -1;
        return (first - current);
    }

    ssize_t String::find(char ch, off_t offset) const {
        char buffer[2] = {ch, '\0'};
        return find(buffer, offset);
    }

    ssize_t String::findLastOf(const String &str) const {
        if (isNullOrEmpty())
            return -1;
        if (str.isNullOrEmpty())
            return -1;

        return (ssize_t)((string) getString()).find_last_of(str.c_str());
    }

    ssize_t String::findLastOf(char ch) const {
        char buffer[2] = {ch, '\0'};
        return findLastOf(buffer);
    }

    bool String::contains(const String &str) const {
        return find(str) != -1;
    }

    bool String::contains(char ch) const {
        char buffer[2] = {ch, '\0'};
        return contains(buffer);
    }

    void String::append(char ch) {
        addString(ch);
    }

    void String::append(const char *str, size_t count) {
        addString(str, count);
    }

    void String::append(const String &str) {
        addString(str.c_str());
    }

    void String::append(const String &str, off_t offset, size_t count) {
        const char *buffer = str._buffer.data();
        append(buffer + offset, count);
    }

    void String::appendLine(char ch) {
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
        while ((pos = strBig.find(src.c_str(), pos)) != string::npos) {
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

    String String::insert(off_t offset, char ch) {
        char buffer[2] = {ch, '\0'};
        return insert(offset, buffer);
    }

    bool String::removeAt(size_t pos) {
        return _buffer.removeAt(pos);
    }

    bool String::removeRange(size_t pos, size_t count) {
        return _buffer.removeRange(pos, count);
    }

    String String::trim(char trimChar1, char trimChar2, char trimChar3,
                        char trimChar4) const {
        return String::trim(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimStart(char trimChar1, char trimChar2, char trimChar3,
                             char trimChar4) const {
        return String::trimStart(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::trimEnd(char trimChar1, char trimChar2, char trimChar3,
                           char trimChar4) const {
        return String::trimEnd(*this, trimChar1, trimChar2, trimChar3, trimChar4);
    }

    String String::convert(const char *format, ...) {
        char *message = new char[MaxFormatStrLength];
        memset(message, 0, MaxFormatStrLength);
        va_list ap;
        va_start(ap, format);
        vsnprintf(message, MaxFormatStrLength, format, ap);
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
        vsnprintf(message, MaxFormatStrLength, format, ap);
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
                    WChar::parse(str, value);
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
        return {_buffer.data()};
    }

    Iterator<char>::const_iterator String::end() const {
        return {_buffer.data() + length()};
    }

    Iterator<char>::iterator String::begin() {
        return {_buffer.data()};
    }

    Iterator<char>::iterator String::end() {
        return {_buffer.data() + length()};
    }

    Iterator<char>::const_reverse_iterator String::rbegin() const {
        return {_buffer.data() + length() - 1};
    }

    Iterator<char>::const_reverse_iterator String::rend() const {
        return {_buffer.data() - 1};
    }

    Iterator<char>::reverse_iterator String::rbegin() {
        return {_buffer.data() + length() - 1};
    }

    Iterator<char>::reverse_iterator String::rend() {
        return {_buffer.data() - 1};
    }
}
