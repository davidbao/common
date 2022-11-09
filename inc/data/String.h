//
//  String.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef String_h
#define String_h

#include "data/Vector.h"
#include "data/PrimitiveInterface.h"
#include <string>

using namespace std;

namespace Common {
    class Stream;

    class ByteArray;

    class WString;

    class String
            : public IEquatable<String>,
              public IEvaluation<String>,
              public IComparable<String>,
              public IIndexable<char, char>,
              public Iterator<char> {
    public:
        enum Base64FormattingOptions {
            Base64None = 0,
            InsertLineBreaks = 1
        };
        enum StreamLength {
            StreamLength1 = 1,
            StreamLength2 = 2,
            StreamLength4 = 4
        };

        static const String NewLine;
        static const String Empty;
        static const String NA;     // Not applicable

        // Constructor & destructor
        String(uint capacity = 256);

        String(const String &value);

        String(const string &value);

        String(const char *value, size_t count = 0);

        String(char ch, size_t count = 1);

        ~String() override;

        // Element access
        char &at(size_t pos) override;

        char at(size_t pos) const override;

        bool set(size_t pos, const char &value) override;

        const char *c_str() const;

        char front() const;

        char &front();

        char back() const;

        char &back();

        // Capacity
        const String &toString() const;

        bool isNullOrEmpty() const;

        size_t length() const;

        // Iterators
        const_iterator begin() const override;

        const_iterator end() const override;

        iterator begin() override;

        iterator end() override;

        const_reverse_iterator rbegin() const override;

        const_reverse_iterator rend() const override;

        reverse_iterator rbegin() override;

        reverse_iterator rend() override;

        // Operations
        String toLower() const;

        String toUpper() const;

        String
        trim(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
             const char trimChar4 = '\0');

        String trimStart(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
                         const char trimChar4 = '\0');

        String trimEnd(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
                       const char trimChar4 = '\0');

        bool contains(const String &str) const;

        bool contains(const char ch) const;

        void append(const char ch);

        void append(const char *str, size_t count);

        void append(const String &str);

        void append(const String &str, off_t offset, size_t count);

        void appendLine(const char ch);

        void appendLine(const char *str, size_t count);

        void appendLine(const String &str = String::Empty);

        void appendLine(const String &str, off_t offset, size_t count);

        String replace(const String &src, const String &dst);

        String substr(off_t offset, size_t count) const;

        String substr(off_t offset) const;

        String insert(off_t offset, const String &str);

        String insert(off_t offset, const char ch);

        bool removeAt(size_t pos);

        bool removeRange(size_t pos, size_t count);

        String trim(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
                    const char trimChar4 = '\0') const;

        String trimStart(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
                         const char trimChar4 = '\0') const;

        String trimEnd(const char trimChar1 = ' ', const char trimChar2 = '\0', const char trimChar3 = '\0',
                       const char trimChar4 = '\0') const;

        void empty();

        bool equals(const String &other) const override;

        void evaluates(const String &other) override;

        int compareTo(const String &other) const override;

        int compareTo(const String &other, bool ignoreCase) const;

        // Find
        ssize_t find(const String &str, off_t offset = 0) const;

        ssize_t find(const char ch, off_t offset = 0) const;

        ssize_t findLastOf(const String &str) const;

        ssize_t findLastOf(const char ch) const;

        // Operator.
        operator const char *() const;

        operator const WString() const;

        String operator+=(const String &value);

        String operator+=(const string &value);

        String operator+=(const char *value);

        String operator+(const String &value) const;

        String operator+(const string &value) const;

        String operator+(const char *value) const;

        String &operator=(const String &value);

        String &operator=(const string &value);

        String &operator=(const char *value);

        bool operator==(const String &value) const;

        bool operator==(const char *value) const;

        bool operator==(const string &value) const;

        bool operator!=(const String &value) const;

        bool operator!=(const char *value) const;

        bool operator!=(const string &value) const;

        bool operator>(const String &value) const;

        bool operator>(const string &value) const;

        bool operator>(const char *value) const;

        bool operator>=(const String &value) const;

        bool operator>=(const string &value) const;

        bool operator>=(const char *value) const;

        bool operator<(const String &value) const;

        bool operator<(const string &value) const;

        bool operator<(const char *value) const;

        bool operator<=(const String &value) const;

        bool operator<=(const string &value) const;

        bool operator<=(const char *value) const;

        // Stream
        void write(Stream *stream, StreamLength streamLength = StreamLength1) const;

        void read(Stream *stream, StreamLength streamLength = StreamLength1);

        void writeFixedLengthStr(Stream *stream, size_t length) const;

        void readFixedLengthStr(Stream *stream, size_t length);

        // Encode
        String GBKtoUTF8() const;

        String UTF8toGBK() const;

        bool isUTF8() const;

        // Base64
        String toBase64() const;

        String fromBase64() const;

    public:
        static bool equals(const String &value1, const String &value2, bool ignoreCase = false);

        static int compare(const String &value1, const String &value2, bool ignoreCase = false);

        static String GBKtoUTF8(const String &value);

        static String UTF8toGBK(const String &value);

        static String toBase64(const String &value);

        static String toBase64(const uint8_t *inArray, off_t offset, size_t length,
                               Base64FormattingOptions options = Base64FormattingOptions::Base64None);

        static bool toBase64(const uint8_t *inArray, off_t offset, size_t length, String &str,
                             Base64FormattingOptions options = Base64FormattingOptions::Base64None);

        static String fromBase64(const String &value);

        static bool fromBase64(const String &value, ByteArray &array);

        static bool fromBase64(const char *inputPtr, size_t inputLength, ByteArray &array);

        static String convert(const char *format, ...);

        static String format(const char *format, ...);

        static bool isNullOrEmpty(const String &value);

        static String replace(const String &str, const String &src, const String &dst);

        static String substr(const String &str, off_t offset, size_t count);

        static String
        trim(const String &str, const char trimChar1, const char trimChar2 = '\0', const char trimChar3 = '\0',
             const char trimChar4 = '\0');

        static String
        trimStart(const String &str, const char trimChar1, const char trimChar2 = '\0', const char trimChar3 = '\0',
                  const char trimChar4 = '\0');

        static String
        trimEnd(const String &str, const char trimChar1, const char trimChar2 = '\0', const char trimChar3 = '\0',
                const char trimChar4 = '\0');

        static bool isUTF8(const String &str);

        static String encoding(const char *fromCode, const char *toCode, const char *str);

        static String unicode2String(const String &unicode);

    private:
        void setString(const char *value);

        void setString(const char *value, size_t count);

        void setString(const string &value);

        void setString(char value);

        void addString(const char *value);

        void addString(const char *value, size_t count);

        void addString(const string &value);

        void addString(char value);

        const char *getString() const;

        static size_t
        convertToBase64Array(char *outChars, const uint8_t *inData, off_t offset, size_t length, bool insertLineBreaks);

        static size_t toBase64_CalculateAndValidateOutputLength(size_t inputLength, bool insertLineBreaks);

        static size_t fromBase64_ComputeResultLength(const char *inputPtr, size_t inputLength);

        static size_t
        fromBase64_Decode(const char *startInputPtr, size_t inputLength, uint8_t *startDestPtr, size_t destLength);

    private:
        static bool encoding(const char *fromCode, const char *toCode, const char *str, char *&buffer, size_t &length);

        enum TrimType {
            TrimHead = 0,
            TrimTail = 1,
            TrimBoth = 2
        };

        static String
        trimInner(const String &str, const Vector<char> &trimChars, TrimType trimType);

    private:
        Vector<char> _buffer;

        static const char base64Table[65];
        static const int base64LineBreakPosition = 76;
        static const int MaxFormatStrLength = 65535;
    };
}

#endif // String_h
