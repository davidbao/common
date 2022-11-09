//
//  WString.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef WString_h
#define WString_h

#include "data/Vector.h"
#include "data/PrimitiveInterface.h"
#include <string>

using namespace std;

namespace Common {
    class Stream;

    class ByteArray;

    class String;

    class WString
            : public IEquatable<WString>,
              public IEvaluation<WString>,
              public IComparable<WString>,
              public IIndexable<wchar_t, wchar_t>,
              public Iterator<wchar_t> {
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

        static const WString NewLine;
        static const WString Empty;
        static const WString NA;     // Not applicable

        // Constructor & destructor
        WString(uint capacity = 256);

        WString(const WString &value);

        WString(const wstring &value);

//        WString(const String &value);

        WString(const wchar_t *value, size_t count = 0);

        WString(wchar_t ch, size_t count = 1);

        ~WString() override;

        // Element access
        wchar_t &at(size_t pos) override;

        wchar_t at(size_t pos) const override;

        bool set(size_t pos, const wchar_t &value) override;

        const wchar_t *c_str() const;

        wchar_t front() const;

        wchar_t &front();

        wchar_t back() const;

        wchar_t &back();

        // Capacity
        const WString &toString() const;

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
        WString toLower() const;

        WString toUpper() const;

        WString
        trim(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
             const wchar_t trimChar4 = '\0');

        WString trimStart(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
                          const wchar_t trimChar4 = '\0');

        WString trimEnd(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
                        const wchar_t trimChar4 = '\0');

        bool contains(const WString &str) const;

        bool contains(const wchar_t ch) const;

        void append(const wchar_t ch);

        void append(const wchar_t *str, size_t count);

        void append(const WString &str);

        void append(const WString &str, off_t offset, size_t count);

        void appendLine(const wchar_t ch);

        void appendLine(const wchar_t *str, size_t count);

        void appendLine(const WString &str = WString::Empty);

        void appendLine(const WString &str, off_t offset, size_t count);

        WString replace(const WString &src, const WString &dst);

        WString substr(off_t offset, size_t count) const;

        WString substr(off_t offset) const;

        WString insert(off_t offset, const WString &str);

        WString insert(off_t offset, const wchar_t ch);

        bool removeAt(size_t pos);

        bool removeRange(size_t pos, size_t count);

        WString trim(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
                     const wchar_t trimChar4 = '\0') const;

        WString trimStart(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
                          const wchar_t trimChar4 = '\0') const;

        WString trimEnd(const wchar_t trimChar1 = ' ', const wchar_t trimChar2 = '\0', const wchar_t trimChar3 = '\0',
                        const wchar_t trimChar4 = '\0') const;

        void empty();

        bool equals(const WString &other) const override;

        void evaluates(const WString &other) override;

        int compareTo(const WString &other) const override;

        int compareTo(const WString &other, bool ignoreCase) const;

        // Find
        ssize_t find(const WString &str, off_t offset = 0) const;

        ssize_t find(const wchar_t ch, off_t offset = 0) const;

        ssize_t findLastOf(const WString &str) const;

        ssize_t findLastOf(const wchar_t ch) const;

        // Operator.
        operator const wchar_t *() const;

        operator const String() const;

        WString operator+=(const WString &value);

        WString operator+=(const wstring &value);

        WString operator+=(const wchar_t *value);

        WString operator+(const WString &value) const;

        WString operator+(const wstring &value) const;

        WString operator+(const wchar_t *value) const;

        WString &operator=(const WString &value);

        WString &operator=(const wstring &value);

        WString &operator=(const wchar_t *value);

        WString &operator=(const String &value);

        bool operator==(const WString &value) const;

        bool operator==(const wchar_t *value) const;

        bool operator==(const wstring &value) const;

        bool operator!=(const WString &value) const;

        bool operator!=(const wchar_t *value) const;

        bool operator!=(const wstring &value) const;

        bool operator>(const WString &value) const;

        bool operator>(const wstring &value) const;

        bool operator>(const wchar_t *value) const;

        bool operator>=(const WString &value) const;

        bool operator>=(const wstring &value) const;

        bool operator>=(const wchar_t *value) const;

        bool operator<(const WString &value) const;

        bool operator<(const wstring &value) const;

        bool operator<(const wchar_t *value) const;

        bool operator<=(const WString &value) const;

        bool operator<=(const wstring &value) const;

        bool operator<=(const wchar_t *value) const;

        // Stream
        void write(Stream *stream, StreamLength streamLength = StreamLength1) const;

        void read(Stream *stream, StreamLength streamLength = StreamLength1);

        void writeFixedLengthStr(Stream *stream, size_t length) const;

        void readFixedLengthStr(Stream *stream, size_t length);

        // Encode
        WString GBKtoUTF8() const;

        WString UTF8toGBK() const;

        bool isUTF8() const;

        // Base64
        WString toBase64() const;

        WString fromBase64() const;

    public:
        static bool equals(const WString &value1, const WString &value2, bool ignoreCase = false);

        static int compare(const WString &value1, const WString &value2, bool ignoreCase = false);

        static WString GBKtoUTF8(const WString &value);

        static WString UTF8toGBK(const WString &value);

        static WString toBase64(const WString &value);

        static WString toBase64(const uint8_t *inArray, off_t offset, size_t length,
                                Base64FormattingOptions options = Base64FormattingOptions::Base64None);

        static bool toBase64(const uint8_t *inArray, off_t offset, size_t length, WString &str,
                             Base64FormattingOptions options = Base64FormattingOptions::Base64None);

        static WString fromBase64(const WString &value);

        static bool fromBase64(const WString &value, ByteArray &array);

        static WString convert(const wchar_t *format, ...);

        static WString format(const wchar_t *format, ...);

        static bool isNullOrEmpty(const WString &value);

        static WString replace(const WString &str, const WString &src, const WString &dst);

        static WString substr(const WString &str, off_t offset, size_t count);

        static WString
        trim(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2 = '\0',
             const wchar_t trimChar3 = '\0',
             const wchar_t trimChar4 = '\0');

        static WString
        trimStart(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2 = '\0',
                  const wchar_t trimChar3 = '\0',
                  const wchar_t trimChar4 = '\0');

        static WString
        trimEnd(const WString &str, const wchar_t trimChar1, const wchar_t trimChar2 = '\0',
                const wchar_t trimChar3 = '\0',
                const wchar_t trimChar4 = '\0');

        static bool isUTF8(const WString &str);

        static WString encoding(const wchar_t *fromCode, const wchar_t *toCode, const wchar_t *str);

        static WString convert(const String &str);

        static String convert(const WString &str);

    private:
        void setString(const wchar_t *value);

        void setString(const wchar_t *value, size_t count);

        void setString(const wstring &value);

        void setString(wchar_t value);

        void addString(const wchar_t *value);

        void addString(const wchar_t *value, size_t count);

        void addString(const wstring &value);

        void addString(wchar_t value);

        const wchar_t *getString() const;

    private:
        enum TrimType {
            TrimHead = 0,
            TrimTail = 1,
            TrimBoth = 2
        };

        static WString
        trimInner(const WString &str, const Vector<wchar_t> &trimChars, TrimType trimType);

    private:
        Vector<wchar_t> _buffer;

        static const int MaxFormatStrLength = 65535;
    };
}

#endif /* WString_h */
