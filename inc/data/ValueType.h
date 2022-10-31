//
//  ValueType.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef ValueType_h
#define ValueType_h

#include "data/Array.h"
#include "data/Vector.h"
#include "data/PrimitiveInterface.h"

#include <string>

using namespace std;

namespace Common {
    class Stream;

    class ByteArray;

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

        reverse_const_iterator rbegin() const override;

        reverse_const_iterator rend() const override;

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

        void writeFixedLengthStr(Stream *stream, size_t length);

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

//        static bool parse(const String &str, String &value);

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
        trimInner(const String &str, const Array<char> &trimChars, TrimType trimType);

    private:
        Array<char> _buffer;

        static const char base64Table[65];
        static const int base64LineBreakPosition = 76;
        static const int MaxFormatStrLength = 1024 * 100;    // 100 K
    };

    class WString : public IIndexable<wchar_t, wchar_t> {
    public:
        static const WString NewLine;
        static const WString Empty;
        static const WString NA;

        WString(uint capacity = 256);

        WString(const WString &value);

        WString(const wstring &value);

        WString(const String &value);

        WString(const wchar_t *value, size_t count = 0);

        WString(wchar_t ch, size_t count = 1);

        ~WString() override;

        bool isNullOrEmpty() const;

        size_t length() const;

        void empty();

        operator const wchar_t *() const;

        operator const String() const;

        const wchar_t *c_str() const;

        WString operator+=(const WString &value);

        WString operator+=(const wstring &value);

        WString operator+=(const wchar_t *value);

        WString operator+(const WString &value) const;

        WString operator+(const wstring &value) const;

        WString operator+(const wchar_t *value) const;

        WString &operator=(const WString &value);

        WString &operator=(const wstring &value);

        WString &operator=(const wchar_t *value);

        bool operator==(const WString &value) const;

        bool operator!=(const WString &value) const;

        bool operator==(const wchar_t *value) const;

        bool operator!=(const wchar_t *value) const;

        bool operator==(const wstring &value) const;

        bool operator!=(const wstring &value) const;

        bool operator>(const WString &value) const;

        bool operator>=(const WString &value) const;

        bool operator<(const WString &value) const;

        bool operator<=(const WString &value) const;

        wchar_t &at(size_t pos) override;

        wchar_t at(size_t pos) const override;

        bool set(size_t pos, const wchar_t &value) override;

        WString toLower() const;

        WString toUpper() const;

        WString trim(const wchar_t symbol1 = ' ', const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
                     const wchar_t symbol4 = '\0') const;

        WString trimStart(const wchar_t symbol1 = ' ', const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
                          const wchar_t symbol4 = '\0') const;

        WString trimEnd(const wchar_t symbol1 = ' ', const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
                        const wchar_t symbol4 = '\0') const;

        WString GBKtoUTF8() const;

        WString UTF8toGBK() const;

        WString toBase64() const;

        WString fromBase64() const;

        ssize_t find(const wchar_t *substring, off_t offset = 0) const;

        ssize_t find(const wchar_t ch) const;

        ssize_t findLastOf(const wchar_t *substring) const;

        ssize_t findLastOf(const wchar_t ch) const;

        bool contains(const wchar_t *substring) const;

        bool contains(const wchar_t ch) const;

        void append(const wchar_t ch);

        void append(const wchar_t *str, size_t count);

        void append(const WString &str);

        void appendLine(const wchar_t ch);

        void appendLine(const wchar_t *str, size_t count);

        void appendLine(const WString &str = WString::Empty);

        WString replace(const WString &src, const WString &dst);

        WString substr(off_t offset, size_t count) const;

        WString substr(off_t offset) const;

        WString insert(off_t offset, const WString &str);

        WString insert(off_t offset, const wchar_t ch);

    public:
        static bool equals(const WString &value1, const WString &value2, bool ignoreCase = false);

        static int compare(const WString &value1, const WString &value2, bool ignoreCase = false);

        static bool isNullOrEmpty(const WString &value);

        static WString replace(const WString &str, const WString &src, const WString &dst);

        static WString substr(const WString &str, off_t offset, size_t count);

        static WString
        trim(const WString &str, const wchar_t symbol1, const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
             const wchar_t symbol4 = '\0');

        static WString
        trimStart(const WString &str, const wchar_t symbol1, const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
                  const wchar_t symbol4 = '\0');

        static WString
        trimEnd(const WString &str, const wchar_t symbol1, const wchar_t symbol2 = '\0', const wchar_t symbol3 = '\0',
                const wchar_t symbol4 = '\0');

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
        Array<wchar_t> _buffer;
    };

    struct Boolean {
    public:
        Boolean();

        Boolean(const Boolean &value);

        Boolean(const bool &value);

        ~Boolean();

        operator bool() const;

        Boolean &operator=(const Boolean &value);

        bool operator==(const Boolean &value) const;

        bool operator!=(const Boolean &value) const;

        void write(Stream *stream) const;

        void read(Stream *stream);

        const String toString() const;

    public:
        static bool parse(const String &str, Boolean &value);

        static bool parse(const String &str, bool &value);

    public:
        static const Boolean TrueValue;
        static const Boolean FalseValue;

    private:
        bool _value;
    };

    struct Char {
    public:
        enum UnicodeCategory : uint8_t {
            //
            // Summary:
            //     Indicates that the character is an uppercase letter. Signified by the Unicode
            //     designation "Lu" (letter, uppercase). The value is 0.
            UppercaseLetter = 0,
            //
            // Summary:
            //     Indicates that the character is a lowercase letter. Signified by the Unicode
            //     designation "Ll" (letter, lowercase). The value is 1.
            LowercaseLetter = 1,
            //
            // Summary:
            //     Indicates that the character is a titlecase letter. Signified by the Unicode
            //     designation "Lt" (letter, titlecase). The value is 2.
            TitlecaseLetter = 2,
            //
            // Summary:
            //     Indicates that the character is a modifier letter, which is free-standing spacing
            //     character that indicates modifications of a preceding letter. Signified by the
            //     Unicode designation "Lm" (letter, modifier). The value is 3.
            ModifierLetter = 3,
            //
            // Summary:
            //     Indicates that the character is a letter that is not an uppercase letter, a lowercase
            //     letter, a titlecase letter, or a modifier letter. Signified by the Unicode designation
            //     "Lo" (letter, other). The value is 4.
            OtherLetter = 4,
            //
            // Summary:
            //     Indicates that the character is a nonspacing character, which indicates modifications
            //     of a base character. Signified by the Unicode designation "Mn" (mark, nonspacing).
            //     The value is 5.
            NonSpacingMark = 5,
            //
            // Summary:
            //     Indicates that the character is a spacing character, which indicates modifications
            //     of a base character and affects the width of the glyph for that base character.
            //     Signified by the Unicode designation "Mc" (mark, spacing combining). The value
            //     is 6.
            SpacingCombiningMark = 6,
            //
            // Summary:
            //     Indicates that the character is an enclosing mark, which is a nonspacing combining
            //     character that surrounds all previous characters up to and including a base character.
            //     Signified by the Unicode designation "Me" (mark, enclosing). The value is 7.
            EnclosingMark = 7,
            //
            // Summary:
            //     Indicates that the character is a decimal digit, that is, in the range 0 through
            //     9. Signified by the Unicode designation "Nd" (number, decimal digit). The value
            //     is 8.
            DecimalDigitNumber = 8,
            //
            // Summary:
            //     Indicates that the character is a number represented by a letter, instead of
            //     a decimal digit, for example, the Roman numeral for five, which is "V". The indicator
            //     is signified by the Unicode designation "Nl" (number, letter). The value is 9.
            LetterNumber = 9,
            //
            // Summary:
            //     Indicates that the character is a number that is neither a decimal digit nor
            //     a letter number, for example, the fraction 1/2. The indicator is signified by
            //     the Unicode designation "No" (number, other). The value is 10.
            OtherNumber = 10,
            //
            // Summary:
            //     Indicates that the character is a space character, which has no glyph but is
            //     not a control or format character. Signified by the Unicode designation "Zs"
            //     (separator, space). The value is 11.
            SpaceSeparator = 11,
            //
            // Summary:
            //     Indicates that the character is used to separate lines of text. Signified by
            //     the Unicode designation "Zl" (separator, line). The value is 12.
            LineSeparator = 12,
            //
            // Summary:
            //     Indicates that the character is used to separate paragraphs. Signified by the
            //     Unicode designation "Zp" (separator, paragraph). The value is 13.
            ParagraphSeparator = 13,
            //
            // Summary:
            //     Indicates that the character is a control code, with a Unicode value of U+007F
            //     or in the range U+0000 through U+001F or U+0080 through U+009F. Signified by
            //     the Unicode designation "Cc" (other, control). The value is 14.
            Control = 14,
            //
            // Summary:
            //     Indicates that the character is a format character, which is not normally rendered
            //     but affects the layout of text or the operation of text processes. Signified
            //     by the Unicode designation "Cf" (other, format). The value is 15.
            Format = 15,
            //
            // Summary:
            //     Indicates that the character is a high surrogate or a low surrogate. Surrogate
            //     code values are in the range U+D800 through U+DFFF. Signified by the Unicode
            //     designation "Cs" (other, surrogate). The value is 16.
            Surrogate = 16,
            //
            // Summary:
            //     Indicates that the character is a private-use character, with a Unicode value
            //     in the range U+E000 through U+F8FF. Signified by the Unicode designation "Co"
            //     (other, private use). The value is 17.
            PrivateUse = 17,
            //
            // Summary:
            //     Indicates that the character is a connector punctuation, which connects two characters.
            //     Signified by the Unicode designation "Pc" (punctuation, connector). The value
            //     is 18.
            ConnectorPunctuation = 18,
            //
            // Summary:
            //     Indicates that the character is a dash or a hyphen. Signified by the Unicode
            //     designation "Pd" (punctuation, dash). The value is 19.
            DashPunctuation = 19,
            //
            // Summary:
            //     Indicates that the character is the opening character of one of the paired punctuation
            //     marks, such as parentheses, square brackets, and braces. Signified by the Unicode
            //     designation "Ps" (punctuation, open). The value is 20.
            OpenPunctuation = 20,
            //
            // Summary:
            //     Indicates that the character is the closing character of one of the paired punctuation
            //     marks, such as parentheses, square brackets, and braces. Signified by the Unicode
            //     designation "Pe" (punctuation, close). The value is 21.
            ClosePunctuation = 21,
            //
            // Summary:
            //     Indicates that the character is an opening or initial quotation mark. Signified
            //     by the Unicode designation "Pi" (punctuation, initial quote). The value is 22.
            InitialQuotePunctuation = 22,
            //
            // Summary:
            //     Indicates that the character is a closing or final quotation mark. Signified
            //     by the Unicode designation "Pf" (punctuation, final quote). The value is 23.
            FinalQuotePunctuation = 23,
            //
            // Summary:
            //     Indicates that the character is a punctuation that is not a connector punctuation,
            //     a dash punctuation, an open punctuation, a close punctuation, an initial quote
            //     punctuation, or a final quote punctuation. Signified by the Unicode designation
            //     "Po" (punctuation, other). The value is 24.
            OtherPunctuation = 24,
            //
            // Summary:
            //     Indicates that the character is a mathematical symbol, such as "+" or "= ". Signified
            //     by the Unicode designation "Sm" (symbol, math). The value is 25.
            MathSymbol = 25,
            //
            // Summary:
            //     Indicates that the character is a currency symbol. Signified by the Unicode designation
            //     "Sc" (symbol, currency). The value is 26.
            CurrencySymbol = 26,
            //
            // Summary:
            //     Indicates that the character is a modifier symbol, which indicates modifications
            //     of surrounding characters. For example, the fraction slash indicates that the
            //     number to the left is the numerator and the number to the right is the denominator.
            //     The indicator is signified by the Unicode designation "Sk" (symbol, modifier).
            //     The value is 27.
            ModifierSymbol = 27,
            //
            // Summary:
            //     Indicates that the character is a symbol that is not a mathematical symbol, a
            //     currency symbol or a modifier symbol. Signified by the Unicode designation "So"
            //     (symbol, other). The value is 28.
            OtherSymbol = 28,
            //
            // Summary:
            //     Indicates that the character is not assigned to any Unicode category. Signified
            //     by the Unicode designation "Cn" (other, not assigned). The value is 29.
            OtherNotAssigned = 29
        };

        Char();

        Char(const Char &value);

        Char(const char &value);

        ~Char();

        operator char() const;

        Char &operator=(const Char &value);

        bool operator==(const Char &value) const;

        bool operator!=(const Char &value) const;

        bool operator>(const Char &value) const;

        bool operator>=(const Char &value) const;

        bool operator<(const Char &value) const;

        bool operator<=(const Char &value) const;

        Char operator+=(const Char &value);

        Char operator-=(const Char &value);

        Char operator+(const Char &value);

        Char operator-(const Char &value);

        Char &operator=(const char &value);

        bool operator==(const char &value) const;

        bool operator!=(const char &value) const;

        bool operator>(const char &value) const;

        bool operator>=(const char &value) const;

        bool operator<(const char &value) const;

        bool operator<=(const char &value) const;

        Char operator+=(const char &value);

        Char operator-=(const char &value);

        Char operator+(const char &value);

        Char operator-(const char &value);

        char operator++();       // ++i
        char operator++(int);    // i++
        char operator--();       // --i
        char operator--(int);    // i--

        void write(Stream *stream) const;

        void read(Stream *stream);

        const String toString(const String &format = String::Empty) const;

        int compareTo(Char value) const;

        const Char toLower() const;

        const Char toUpper() const;

    public:
        static bool parse(const String &str, Char &value, bool decimal = true);

        static bool parse(const String &str, char &value, bool decimal = true);

        static bool parse(const String &str, int8_t &value, bool decimal = true);

        static uint8_t toHex(char value);

        static char toChar(uint8_t value);

        static bool isDigit(char c);

        static bool isLetter(char c);

        static bool isWhiteSpace(char c);

        static bool isUpper(char c);

        static bool isLower(char c);

        static bool isLetterOrDigit(char c);

        static bool isSymbol(char c);

        static bool isAscii(char ch);

    private:
        static bool isWhiteSpaceLatin1(char c);

        // Return true for all characters below or equal U+00ff, which is ASCII + Latin-1 Supplement.
        static bool isLatin1(char ch);

        // Return true for all characters below or equal U+007f, which is ASCII.
        static bool checkSymbol(UnicodeCategory uc);

        static UnicodeCategory getLatin1UnicodeCategory(char ch);

        static bool checkLetterOrDigit(UnicodeCategory uc);

    public:
        static const Char MinValue;
        static const Char MaxValue;

        static const Char NewLine;

    private:
        static const uint8_t categoryForLatin1[256];

    private:
        char _value;
    };

    struct WChar {
    public:
        enum UnicodeCategory : uint8_t {
            //
            // Summary:
            //     Indicates that the character is an uppercase letter. Signified by the Unicode
            //     designation "Lu" (letter, uppercase). The value is 0.
            UppercaseLetter = 0,
            //
            // Summary:
            //     Indicates that the character is a lowercase letter. Signified by the Unicode
            //     designation "Ll" (letter, lowercase). The value is 1.
            LowercaseLetter = 1,
            //
            // Summary:
            //     Indicates that the character is a titlecase letter. Signified by the Unicode
            //     designation "Lt" (letter, titlecase). The value is 2.
            TitlecaseLetter = 2,
            //
            // Summary:
            //     Indicates that the character is a modifier letter, which is free-standing spacing
            //     character that indicates modifications of a preceding letter. Signified by the
            //     Unicode designation "Lm" (letter, modifier). The value is 3.
            ModifierLetter = 3,
            //
            // Summary:
            //     Indicates that the character is a letter that is not an uppercase letter, a lowercase
            //     letter, a titlecase letter, or a modifier letter. Signified by the Unicode designation
            //     "Lo" (letter, other). The value is 4.
            OtherLetter = 4,
            //
            // Summary:
            //     Indicates that the character is a nonspacing character, which indicates modifications
            //     of a base character. Signified by the Unicode designation "Mn" (mark, nonspacing).
            //     The value is 5.
            NonSpacingMark = 5,
            //
            // Summary:
            //     Indicates that the character is a spacing character, which indicates modifications
            //     of a base character and affects the width of the glyph for that base character.
            //     Signified by the Unicode designation "Mc" (mark, spacing combining). The value
            //     is 6.
            SpacingCombiningMark = 6,
            //
            // Summary:
            //     Indicates that the character is an enclosing mark, which is a nonspacing combining
            //     character that surrounds all previous characters up to and including a base character.
            //     Signified by the Unicode designation "Me" (mark, enclosing). The value is 7.
            EnclosingMark = 7,
            //
            // Summary:
            //     Indicates that the character is a decimal digit, that is, in the range 0 through
            //     9. Signified by the Unicode designation "Nd" (number, decimal digit). The value
            //     is 8.
            DecimalDigitNumber = 8,
            //
            // Summary:
            //     Indicates that the character is a number represented by a letter, instead of
            //     a decimal digit, for example, the Roman numeral for five, which is "V". The indicator
            //     is signified by the Unicode designation "Nl" (number, letter). The value is 9.
            LetterNumber = 9,
            //
            // Summary:
            //     Indicates that the character is a number that is neither a decimal digit nor
            //     a letter number, for example, the fraction 1/2. The indicator is signified by
            //     the Unicode designation "No" (number, other). The value is 10.
            OtherNumber = 10,
            //
            // Summary:
            //     Indicates that the character is a space character, which has no glyph but is
            //     not a control or format character. Signified by the Unicode designation "Zs"
            //     (separator, space). The value is 11.
            SpaceSeparator = 11,
            //
            // Summary:
            //     Indicates that the character is used to separate lines of text. Signified by
            //     the Unicode designation "Zl" (separator, line). The value is 12.
            LineSeparator = 12,
            //
            // Summary:
            //     Indicates that the character is used to separate paragraphs. Signified by the
            //     Unicode designation "Zp" (separator, paragraph). The value is 13.
            ParagraphSeparator = 13,
            //
            // Summary:
            //     Indicates that the character is a control code, with a Unicode value of U+007F
            //     or in the range U+0000 through U+001F or U+0080 through U+009F. Signified by
            //     the Unicode designation "Cc" (other, control). The value is 14.
            Control = 14,
            //
            // Summary:
            //     Indicates that the character is a format character, which is not normally rendered
            //     but affects the layout of text or the operation of text processes. Signified
            //     by the Unicode designation "Cf" (other, format). The value is 15.
            Format = 15,
            //
            // Summary:
            //     Indicates that the character is a high surrogate or a low surrogate. Surrogate
            //     code values are in the range U+D800 through U+DFFF. Signified by the Unicode
            //     designation "Cs" (other, surrogate). The value is 16.
            Surrogate = 16,
            //
            // Summary:
            //     Indicates that the character is a private-use character, with a Unicode value
            //     in the range U+E000 through U+F8FF. Signified by the Unicode designation "Co"
            //     (other, private use). The value is 17.
            PrivateUse = 17,
            //
            // Summary:
            //     Indicates that the character is a connector punctuation, which connects two characters.
            //     Signified by the Unicode designation "Pc" (punctuation, connector). The value
            //     is 18.
            ConnectorPunctuation = 18,
            //
            // Summary:
            //     Indicates that the character is a dash or a hyphen. Signified by the Unicode
            //     designation "Pd" (punctuation, dash). The value is 19.
            DashPunctuation = 19,
            //
            // Summary:
            //     Indicates that the character is the opening character of one of the paired punctuation
            //     marks, such as parentheses, square brackets, and braces. Signified by the Unicode
            //     designation "Ps" (punctuation, open). The value is 20.
            OpenPunctuation = 20,
            //
            // Summary:
            //     Indicates that the character is the closing character of one of the paired punctuation
            //     marks, such as parentheses, square brackets, and braces. Signified by the Unicode
            //     designation "Pe" (punctuation, close). The value is 21.
            ClosePunctuation = 21,
            //
            // Summary:
            //     Indicates that the character is an opening or initial quotation mark. Signified
            //     by the Unicode designation "Pi" (punctuation, initial quote). The value is 22.
            InitialQuotePunctuation = 22,
            //
            // Summary:
            //     Indicates that the character is a closing or final quotation mark. Signified
            //     by the Unicode designation "Pf" (punctuation, final quote). The value is 23.
            FinalQuotePunctuation = 23,
            //
            // Summary:
            //     Indicates that the character is a punctuation that is not a connector punctuation,
            //     a dash punctuation, an open punctuation, a close punctuation, an initial quote
            //     punctuation, or a final quote punctuation. Signified by the Unicode designation
            //     "Po" (punctuation, other). The value is 24.
            OtherPunctuation = 24,
            //
            // Summary:
            //     Indicates that the character is a mathematical symbol, such as "+" or "= ". Signified
            //     by the Unicode designation "Sm" (symbol, math). The value is 25.
            MathSymbol = 25,
            //
            // Summary:
            //     Indicates that the character is a currency symbol. Signified by the Unicode designation
            //     "Sc" (symbol, currency). The value is 26.
            CurrencySymbol = 26,
            //
            // Summary:
            //     Indicates that the character is a modifier symbol, which indicates modifications
            //     of surrounding characters. For example, the fraction slash indicates that the
            //     number to the left is the numerator and the number to the right is the denominator.
            //     The indicator is signified by the Unicode designation "Sk" (symbol, modifier).
            //     The value is 27.
            ModifierSymbol = 27,
            //
            // Summary:
            //     Indicates that the character is a symbol that is not a mathematical symbol, a
            //     currency symbol or a modifier symbol. Signified by the Unicode designation "So"
            //     (symbol, other). The value is 28.
            OtherSymbol = 28,
            //
            // Summary:
            //     Indicates that the character is not assigned to any Unicode category. Signified
            //     by the Unicode designation "Cn" (other, not assigned). The value is 29.
            OtherNotAssigned = 29
        };

        WChar();

        WChar(const WChar &value);

        WChar(const wchar_t &value);

        ~WChar();

        operator wchar_t() const;

        WChar &operator=(const WChar &value);

        bool operator==(const WChar &value) const;

        bool operator!=(const WChar &value) const;

        bool operator>(const WChar &value) const;

        bool operator>=(const WChar &value) const;

        bool operator<(const WChar &value) const;

        bool operator<=(const WChar &value) const;

        WChar operator+=(const WChar &value);

        WChar operator-=(const WChar &value);

        WChar operator+(const WChar &value);

        WChar operator-(const WChar &value);

        WChar &operator=(const wchar_t &value);

        bool operator==(const wchar_t &value) const;

        bool operator!=(const wchar_t &value) const;

        bool operator>(const wchar_t &value) const;

        bool operator>=(const wchar_t &value) const;

        bool operator<(const wchar_t &value) const;

        bool operator<=(const wchar_t &value) const;

        WChar operator+=(const wchar_t &value);

        WChar operator-=(const wchar_t &value);

        WChar operator+(const wchar_t &value);

        WChar operator-(const wchar_t &value);

        wchar_t operator++();       // ++i
        wchar_t operator++(int);    // i++
        wchar_t operator--();       // --i
        wchar_t operator--(int);    // i--

        void write(Stream *stream) const;

        void read(Stream *stream);

        const String toString(const String &format = String::Empty) const;

        int compareTo(WChar value) const;

    public:
        static bool parse(const String &str, WChar &value, bool decimal = true);

        static bool parse(const String &str, wchar_t &value, bool decimal = true);

        static bool isDigit(wchar_t c);

        static bool isLetter(wchar_t c);

        static bool isWhiteSpace(wchar_t c);

        static bool isUpper(wchar_t c);

        static bool isLower(wchar_t c);

        static bool isLetterOrDigit(wchar_t c);

        static bool isSymbol(wchar_t c);

        static bool isHanzi(wchar_t c);

    private:
        static bool isWhiteSpaceLatin1(wchar_t c);

        // Return true for all wchar_tacters below or equal U+00ff, which is ASCII + Latin-1 Supplement.
        static bool isLatin1(wchar_t ch);

        // Return true for all wchar_tacters below or equal U+007f, which is ASCII.
        static bool isAscii(wchar_t ch);

        static bool checkSymbol(UnicodeCategory uc);

        static UnicodeCategory getLatin1UnicodeCategory(wchar_t ch);

        static bool checkLetterOrDigit(UnicodeCategory uc);

    public:
        static const WChar MinValue;
        static const WChar MaxValue;

    private:
        static const uint8_t categoryForLatin1[256];

    private:
        wchar_t _value;
    };

    struct Int8 {
    public:
        Int8();

        Int8(const Int8 &value);

        Int8(const int8_t &value);

        ~Int8();

        operator int8_t() const;

        Int8 &operator=(const Int8 &value);

        bool operator==(const Int8 &value) const;

        bool operator!=(const Int8 &value) const;

        bool operator>(const Int8 &value) const;

        bool operator>=(const Int8 &value) const;

        bool operator<(const Int8 &value) const;

        bool operator<=(const Int8 &value) const;

        Int8 operator+=(const Int8 &value);

        Int8 operator-=(const Int8 &value);

        Int8 operator+(const Int8 &value);

        Int8 operator-(const Int8 &value);

        Int8 &operator=(const int8_t &value);

        bool operator==(const int8_t &value) const;

        bool operator!=(const int8_t &value) const;

        bool operator>(const int8_t &value) const;

        bool operator>=(const int8_t &value) const;

        bool operator<(const int8_t &value) const;

        bool operator<=(const int8_t &value) const;

        Int8 operator+=(const int8_t &value);

        Int8 operator-=(const int8_t &value);

        Int8 operator+(const int8_t &value);

        Int8 operator-(const int8_t &value);

        int8_t operator++();       // ++i
        int8_t operator++(int);    // i++
        int8_t operator--();       // --i
        int8_t operator--(int);    // i--

        void write(Stream *stream) const;

        void read(Stream *stream);

        const String toString(const String &format = String::Empty) const;

        int compareTo(Int8 value) const;

    public:
        static bool parse(const String &str, Int8 &value, bool decimal = true);

        static bool parse(const String &str, int8_t &value, bool decimal = true);

    public:
        static const Int8 MinValue;
        static const Int8 MaxValue;

    private:
        int8_t _value;
    };

    struct UInt8 {
    public:
        UInt8();

        UInt8(const UInt8 &value);

        UInt8(const uint8_t &value);

        ~UInt8();

        operator uint8_t() const;

        UInt8 &operator=(const UInt8 &value);

        bool operator==(const UInt8 &value) const;

        bool operator!=(const UInt8 &value) const;

        bool operator>(const UInt8 &value) const;

        bool operator>=(const UInt8 &value) const;

        bool operator<(const UInt8 &value) const;

        bool operator<=(const UInt8 &value) const;

        UInt8 operator+=(const UInt8 &value);

        UInt8 operator-=(const UInt8 &value);

        UInt8 operator+(const UInt8 &value);

        UInt8 operator-(const UInt8 &value);

        UInt8 &operator=(const uint8_t &value);

        bool operator==(const uint8_t &value) const;

        bool operator!=(const uint8_t &value) const;

        bool operator>(const uint8_t &value) const;

        bool operator>=(const uint8_t &value) const;

        bool operator<(const uint8_t &value) const;

        bool operator<=(const uint8_t &value) const;

        UInt8 operator+=(const uint8_t &value);

        UInt8 operator-=(const uint8_t &value);

        UInt8 operator+(const uint8_t &value);

        UInt8 operator-(const uint8_t &value);

        uint8_t operator++();       // ++i
        uint8_t operator++(int);    // i++
        uint8_t operator--();       // --i
        uint8_t operator--(int);    // i--

        void write(Stream *stream) const;

        void read(Stream *stream);

        const String toString(const String &format = String::Empty) const;

        int compareTo(UInt8 value) const;

    public:
        static bool parse(const String &str, UInt8 &value, bool decimal = true);

        static bool parse(const String &str, uint8_t &value, bool decimal = true);

    public:
        static const UInt8 MinValue;
        static const UInt8 MaxValue;

    private:
        uint8_t _value;
    };

    typedef UInt8 Byte;

    struct Int16 {
    public:
        Int16();

        Int16(const Int16 &value);

        Int16(const int16_t &value);

        ~Int16();

        operator int16_t() const;

        Int16 &operator=(const Int16 &value);

        bool operator==(const Int16 &value) const;

        bool operator!=(const Int16 &value) const;

        bool operator>(const Int16 &value) const;

        bool operator>=(const Int16 &value) const;

        bool operator<(const Int16 &value) const;

        bool operator<=(const Int16 &value) const;

        Int16 operator+=(const Int16 &value);

        Int16 operator-=(const Int16 &value);

        Int16 operator+(const Int16 &value);

        Int16 operator-(const Int16 &value);

        Int16 &operator=(const int16_t &value);

        bool operator==(const int16_t &value) const;

        bool operator!=(const int16_t &value) const;

        bool operator>(const int16_t &value) const;

        bool operator>=(const int16_t &value) const;

        bool operator<(const int16_t &value) const;

        bool operator<=(const int16_t &value) const;

        Int16 operator+=(const int16_t &value);

        Int16 operator-=(const int16_t &value);

        Int16 operator+(const int16_t &value);

        Int16 operator-(const int16_t &value);

        int16_t operator++();       // ++i
        int16_t operator++(int);    // i++
        int16_t operator--();       // --i
        int16_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(Int16 value) const;

    public:
        static bool parse(const String &str, Int16 &value, bool decimal = true);

        static bool parse(const String &str, int16_t &value, bool decimal = true);

    public:
        static const Int16 MinValue;
        static const Int16 MaxValue;

    private:
        int16_t _value;
    };

    typedef Int16 Short;

    struct UInt16 {
    public:
        UInt16();

        UInt16(const UInt16 &value);

        UInt16(const uint16_t &value);

        ~UInt16();

        operator uint16_t() const;

        UInt16 &operator=(const UInt16 &value);

        bool operator==(const UInt16 &value) const;

        bool operator!=(const UInt16 &value) const;

        bool operator>(const UInt16 &value) const;

        bool operator>=(const UInt16 &value) const;

        bool operator<(const UInt16 &value) const;

        bool operator<=(const UInt16 &value) const;

        UInt16 operator+=(const UInt16 &value);

        UInt16 operator-=(const UInt16 &value);

        UInt16 operator+(const UInt16 &value);

        UInt16 operator-(const UInt16 &value);

        UInt16 &operator=(const uint16_t &value);

        bool operator==(const uint16_t &value) const;

        bool operator!=(const uint16_t &value) const;

        bool operator>(const uint16_t &value) const;

        bool operator>=(const uint16_t &value) const;

        bool operator<(const uint16_t &value) const;

        bool operator<=(const uint16_t &value) const;

        UInt16 operator+=(const uint16_t &value);

        UInt16 operator-=(const uint16_t &value);

        UInt16 operator+(const uint16_t &value);

        UInt16 operator-(const uint16_t &value);

        uint16_t operator++();       // ++i
        uint16_t operator++(int);    // i++
        uint16_t operator--();       // --i
        uint16_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(UInt16 value) const;

    public:
        static bool parse(const String &str, UInt16 &value, bool decimal = true);

        static bool parse(const String &str, uint16_t &value, bool decimal = true);

    public:
        static const UInt16 MinValue;
        static const UInt16 MaxValue;

    private:
        uint16_t _value;
    };

    typedef UInt16 UShort;

    struct Int32 {
    public:
        Int32();

        Int32(const Int32 &value);

        Int32(const int32_t &value);

        ~Int32();

        operator int32_t() const;

        Int32 &operator=(const Int32 &value);

        bool operator==(const Int32 &value) const;

        bool operator!=(const Int32 &value) const;

        bool operator>(const Int32 &value) const;

        bool operator>=(const Int32 &value) const;

        bool operator<(const Int32 &value) const;

        bool operator<=(const Int32 &value) const;

        Int32 operator+=(const Int32 &value);

        Int32 operator-=(const Int32 &value);

        Int32 operator+(const Int32 &value);

        Int32 operator-(const Int32 &value);

        Int32 &operator=(const int32_t &value);

        bool operator==(const int32_t &value) const;

        bool operator!=(const int32_t &value) const;

        bool operator>(const int32_t &value) const;

        bool operator>=(const int32_t &value) const;

        bool operator<(const int32_t &value) const;

        bool operator<=(const int32_t &value) const;

        Int32 operator+=(const int32_t &value);

        Int32 operator-=(const int32_t &value);

        Int32 operator+(const int32_t &value);

        Int32 operator-(const int32_t &value);

        int32_t operator++();       // ++i
        int32_t operator++(int);    // i++
        int32_t operator--();       // --i
        int32_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(Int32 value) const;

    public:
        static bool parse(const String &str, Int32 &value, bool decimal = true);

        static bool parse(const String &str, int32_t &value, bool decimal = true);

        static bool isHexInteger(const String &str);

    public:
        static const Int32 MinValue;
        static const Int32 MaxValue;

    private:
        int32_t _value;
    };

    struct UInt32 {
    public:
        UInt32();

        UInt32(const UInt32 &value);

        UInt32(const uint32_t &value);

        ~UInt32();

        operator uint32_t() const;

        UInt32 &operator=(const UInt32 &value);

        bool operator==(const UInt32 &value) const;

        bool operator!=(const UInt32 &value) const;

        bool operator>(const UInt32 &value) const;

        bool operator>=(const UInt32 &value) const;

        bool operator<(const UInt32 &value) const;

        bool operator<=(const UInt32 &value) const;

        UInt32 operator+=(const UInt32 &value);

        UInt32 operator-=(const UInt32 &value);

        UInt32 operator+(const UInt32 &value);

        UInt32 operator-(const UInt32 &value);

        UInt32 &operator=(const uint32_t &value);

        bool operator==(const uint32_t &value) const;

        bool operator!=(const uint32_t &value) const;

        bool operator>(const uint32_t &value) const;

        bool operator>=(const uint32_t &value) const;

        bool operator<(const uint32_t &value) const;

        bool operator<=(const uint32_t &value) const;

        UInt32 operator+=(const uint32_t &value);

        UInt32 operator-=(const uint32_t &value);

        UInt32 operator+(const uint32_t &value);

        UInt32 operator-(const uint32_t &value);

        uint32_t operator++();       // ++i
        uint32_t operator++(int);    // i++
        uint32_t operator--();       // --i
        uint32_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(UInt32 value) const;

    public:
        static bool parse(const String &str, UInt32 &value, bool decimal = true);

        static bool parse(const String &str, uint32_t &value, bool decimal = true);

    public:
        static const UInt32 MinValue;
        static const UInt32 MaxValue;

    private:
        uint32_t _value;
    };

    struct Int64 {
    public:
        Int64();

        Int64(const Int64 &value);

        Int64(const int64_t &value);

        ~Int64();

        operator int64_t() const;

        Int64 &operator=(const Int64 &value);

        bool operator==(const Int64 &value) const;

        bool operator!=(const Int64 &value) const;

        bool operator>(const Int64 &value) const;

        bool operator>=(const Int64 &value) const;

        bool operator<(const Int64 &value) const;

        bool operator<=(const Int64 &value) const;

        Int64 operator+=(const Int64 &value);

        Int64 operator-=(const Int64 &value);

        Int64 operator+(const Int64 &value);

        Int64 operator-(const Int64 &value);

        Int64 &operator=(const int64_t &value);

        bool operator==(const int64_t &value) const;

        bool operator!=(const int64_t &value) const;

        bool operator>(const int64_t &value) const;

        bool operator>=(const int64_t &value) const;

        bool operator<(const int64_t &value) const;

        bool operator<=(const int64_t &value) const;

        Int64 operator+=(const int64_t &value);

        Int64 operator-=(const int64_t &value);

        Int64 operator+(const int64_t &value);

        Int64 operator-(const int64_t &value);

        int64_t operator++();       // ++i
        int64_t operator++(int);    // i++
        int64_t operator--();       // --i
        int64_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(Int64 value) const;

    public:
        static bool parse(const String &str, Int64 &value, bool decimal = true);

        static bool parse(const String &str, int64_t &value, bool decimal = true);

    public:
        static const Int64 MinValue;
        static const Int64 MaxValue;

    private:
        int64_t _value;
    };

    struct UInt64 {
    public:
        UInt64();

        UInt64(const UInt64 &value);

        UInt64(const uint64_t &value);

        ~UInt64();

        operator uint64_t() const;

        UInt64 &operator=(const UInt64 &value);

        bool operator==(const UInt64 &value) const;

        bool operator!=(const UInt64 &value) const;

        bool operator>(const UInt64 &value) const;

        bool operator>=(const UInt64 &value) const;

        bool operator<(const UInt64 &value) const;

        bool operator<=(const UInt64 &value) const;

        UInt64 operator+=(const UInt64 &value);

        UInt64 operator-=(const UInt64 &value);

        UInt64 operator+(const UInt64 &value);

        UInt64 operator-(const UInt64 &value);

        UInt64 &operator=(const uint64_t &value);

        bool operator==(const uint64_t &value) const;

        bool operator!=(const uint64_t &value) const;

        bool operator>(const uint64_t &value) const;

        bool operator>=(const uint64_t &value) const;

        bool operator<(const uint64_t &value) const;

        bool operator<=(const uint64_t &value) const;

        UInt64 operator+=(const uint64_t &value);

        UInt64 operator-=(const uint64_t &value);

        UInt64 operator+(const uint64_t &value);

        UInt64 operator-(const uint64_t &value);

        uint64_t operator++();       // ++i
        uint64_t operator++(int);    // i++
        uint64_t operator--();       // --i
        uint64_t operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        int compareTo(UInt64 value) const;

    public:
        static bool parse(const String &str, UInt64 &value, bool decimal = true);

        static bool parse(const String &str, uint64_t &value, bool decimal = true);

    public:
        static const UInt64 MinValue;
        static const UInt64 MaxValue;

    private:
        uint64_t _value;
    };

    struct Float {
    public:
        Float();

        Float(const Float &value);

        Float(const float &value);

        ~Float();

        operator float() const;

        Float &operator=(const Float &value);

        bool operator==(const Float &value) const;

        bool operator!=(const Float &value) const;

        bool operator>(const Float &value) const;

        bool operator>=(const Float &value) const;

        bool operator<(const Float &value) const;

        bool operator<=(const Float &value) const;

        Float operator+=(const Float &value);

        Float operator-=(const Float &value);

        Float operator+(const Float &value);

        Float operator-(const Float &value);

        Float &operator=(const float &value);

        bool operator==(const float &value) const;

        bool operator!=(const float &value) const;

        bool operator>(const float &value) const;

        bool operator>=(const float &value) const;

        bool operator<(const float &value) const;

        bool operator<=(const float &value) const;

        Float operator+=(const float &value);

        Float operator-=(const float &value);

        Float operator+(const float &value);

        Float operator-(const float &value);

        float operator++();       // ++i
        float operator++(int);    // i++
        float operator--();       // --i
        float operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        const String toString(int pointSize) const;

        bool isNaN() const;

        bool isFinite() const;

        int compareTo(Float value) const;

    public:
        static bool parse(const String &str, Float &value);

        static bool parse(const String &str, float &value);

        static bool isNaN(const Float &value);

        static bool isFinite(const Float &value);

    public:
        static const Float MinValue;
        static const Float MaxValue;
        static const Float Epsilon;
        static const Float NegativeInfinity;
        static const Float PositiveInfinity;
        static const Float NaN;

    private:
        float _value;
    };

    struct Double {
    public:
        Double();

        Double(const Double &value);

        Double(const double &value);

        ~Double();

        operator double() const;

        Double &operator=(const Double &value);

        bool operator==(const Double &value) const;

        bool operator!=(const Double &value) const;

        bool operator>(const Double &value) const;

        bool operator>=(const Double &value) const;

        bool operator<(const Double &value) const;

        bool operator<=(const Double &value) const;

        Double operator+=(const Double &value);

        Double operator-=(const Double &value);

        Double operator+(const Double &value);

        Double operator-(const Double &value);

        Double &operator=(const double &value);

        bool operator==(const double &value) const;

        bool operator!=(const double &value) const;

        bool operator>(const double &value) const;

        bool operator>=(const double &value) const;

        bool operator<(const double &value) const;

        bool operator<=(const double &value) const;

        Double operator+=(const double &value);

        Double operator-=(const double &value);

        Double operator+(const double &value);

        Double operator-(const double &value);

        double operator++();       // ++i
        double operator++(int);    // i++
        double operator--();       // --i
        double operator--(int);    // i--

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        const String toString(const String &format = String::Empty) const;

        const String toString(int pointSize) const;

        bool isNaN() const;

        bool isFinite() const;

        int compareTo(Double value) const;

    public:
        static bool parse(const String &str, Double &value);

        static bool parse(const String &str, double &value);

        static bool isNaN(const Double &value);

        static bool isFinite(const Double &value);

    public:
        static const Double MinValue;
        static const Double MaxValue;
        static const Double Epsilon;
        static const Double NegativeInfinity;
        static const Double PositiveInfinity;
        static const Double NaN;
        static const Double Infinity;

    private:
        double _value;
    };

    struct PositionCoord {
    public:
        double latitude;
        double longitude;

        PositionCoord(double longitude = Double::NaN, double latitude = Double::NaN);

        PositionCoord(const PositionCoord &coord);

        PositionCoord(const String &str);

        PositionCoord(const char *str);

        bool isEmpty() const;

        void empty();

        const String toString() const;

        static bool parse(const String &str, PositionCoord &value);

        void operator=(const PositionCoord &value);

        bool operator==(const PositionCoord &value) const;

        bool operator!=(const PositionCoord &value) const;

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

    public:
        static const PositionCoord Empty;
    };

    typedef Array<PositionCoord> LocationCoords;
}

#endif /* ValueType_h */
