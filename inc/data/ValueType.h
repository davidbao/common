//
//  ValueType.h
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef ValueType_h
#define ValueType_h

#include "data/Vector.h"
#include "data/PrimitiveInterface.h"
#include "data/String.h"

namespace Common {
    class Stream;
    class NumberFormatInfo;

    enum NumberStyles : uint32_t {
        // Bit flag indicating that leading whitespace is allowed. Character values
        // 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, and 0x0020 are considered to be
        // whitespace.

        NSNone = 0x00000000,

        NSAllowLeadingWhite = 0x00000001,

        NSAllowTrailingWhite = 0x00000002, //Bitflag indicating trailing whitespace is allowed.

        NSAllowLeadingSign = 0x00000004, //Can the number start with a sign char.
        //Specified by NumberFormatInfo.PositiveSign and NumberFormatInfo.NegativeSign

        NSAllowTrailingSign = 0x00000008, //Allow the number to end with a sign char

        NSAllowParentheses = 0x00000010, //Allow the number to be enclosed in parens

        NSAllowDecimalPoint = 0x00000020, //Allow a decimal point

        NSAllowThousands = 0x00000040, //Allow thousands separators (more properly, allow group separators)

        NSAllowExponent = 0x00000080, //Allow an exponent

        NSAllowCurrencySymbol = 0x00000100, //Allow a currency symbol.

        NSAllowHexSpecifier = 0x00000200, //Allow specifying hexadecimal.
        //Common uses.  These represent some of the most common combinations of these flags.


        NSInteger = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowLeadingSign,

        NSHexNumber = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowHexSpecifier,

        NSNumber = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowLeadingSign | NSAllowTrailingSign |
                   NSAllowDecimalPoint | NSAllowThousands,

        NSFloat = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowLeadingSign |
                  NSAllowDecimalPoint | NSAllowExponent,

        NSCurrency = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowLeadingSign | NSAllowTrailingSign |
                     NSAllowParentheses | NSAllowDecimalPoint | NSAllowThousands | NSAllowCurrencySymbol,

        NSAny = NSAllowLeadingWhite | NSAllowTrailingWhite | NSAllowLeadingSign | NSAllowTrailingSign |
                NSAllowParentheses | NSAllowDecimalPoint | NSAllowThousands | NSAllowCurrencySymbol | NSAllowExponent,

    };

    struct Boolean : public IEquatable<Boolean>, public IEquatable<Boolean, bool>, public IEvaluation<Boolean> {
    public:
        Boolean(const bool &value = false);

        Boolean(const Boolean &value);

        ~Boolean() override;

        bool equals(const Boolean &other) const override;

        bool equals(const bool &other) const override;

        void evaluates(const Boolean &other) override;

        operator bool() const;

        Boolean &operator=(const Boolean &other);

        Boolean &operator=(const bool &value);

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString() const;

    public:
        static bool parse(const String &str, Boolean &value);

        static bool parse(const String &str, bool &value);

    public:
        static const Boolean TrueValue;
        static const Boolean FalseValue;

    private:
        bool _value;
    };

    struct BaseValueType {
    public:
        BaseValueType();

        virtual ~BaseValueType();

    protected:
        template<typename type>
        static String toValueString(const type &value, const String &format, const IFormatProvider<NumberFormatInfo>* provider);

        static bool parseInt64(const String &str, int64_t &value, NumberStyles style = NSInteger);

        static bool parseUInt64(const String &str, uint64_t &value, NumberStyles style = NSInteger);

        static bool
        parseDouble(const String &str, double &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

    private:
        template<typename type>
        static String toValueString(const type &value, const String &format, const NumberFormatInfo *info);

        static String addThousandSeparator(const String &str, const String& decimalSeparator, const String &groupSeparator);
    };

    template<typename type>
    struct ValueType
            : public BaseValueType, public IEvaluation<ValueType<type>>,
              public IEquatable<ValueType<type>>, public IEquatable<ValueType<type>, type>,
              public IComparable<ValueType<type>>, public IComparable<type> {
    public:
        using IComparable<ValueType<type>>::operator>;
        using IComparable<ValueType<type>>::operator>=;
        using IComparable<ValueType<type>>::operator<;
        using IComparable<ValueType<type>>::operator<=;
        using IComparable<type>::operator>;
        using IComparable<type>::operator>=;
        using IComparable<type>::operator<;
        using IComparable<type>::operator<=;

        explicit ValueType(const type &value) : _value(value) {
        }

        ValueType(const ValueType &value) : _value(value._value) {
        }

        ~ValueType() override {
        }

        inline bool equals(const ValueType &other) const override {
            return _value == other._value;
        }

        inline bool equals(const type &other) const override {
            return _value == other;
        }

        inline void evaluates(const ValueType &other) override {
            _value = other._value;
        }

        inline int compareTo(const ValueType &other) const override {
            // Need to use compare because subtraction will wrap
            // to positive for very large neg numbers, etc.
            if (_value < other._value) return -1;
            if (_value > other._value) return 1;
            return 0;
        }

        inline int compareTo(const type &other) const override {
            // Need to use compare because subtraction will wrap
            // to positive for very large neg numbers, etc.
            if (_value < other) return -1;
            if (_value > other) return 1;
            return 0;
        }

        inline operator type() const {
            return _value;
        }

        friend bool operator==(const type &a, ValueType &b) noexcept {
            return b.equals(a);
        }

        friend bool operator==(type &a, ValueType &b) noexcept {
            return b.equals(a);
        }

        friend bool operator==(const type &a, const ValueType &b) noexcept {
            return b.equals(a);
        }

        friend bool operator==(type &a, const ValueType &b) noexcept {
            return b.equals(a);
        }

        inline ValueType &operator+=(const ValueType &value) {
            _value += value._value;
            return *this;
        }

        inline ValueType &operator-=(const ValueType &value) {
            _value -= value._value;
            return *this;
        }

        inline ValueType operator+(const ValueType &value) {
            ValueType result = _value;
            result._value += value._value;
            return result;
        }

        inline ValueType operator-(const ValueType &value) {
            ValueType result = _value;
            result._value -= value._value;
            return result;
        }

        inline ValueType operator+=(const type &value) {
            _value += value;
            return *this;
        }

        inline ValueType operator-=(const type &value) {
            _value -= value;
            return *this;
        }

        inline ValueType operator+(const type &value) {
            ValueType result(_value);
            result._value += value;
            return result;
        }

        inline ValueType operator-(const type &value) {
            ValueType result(_value);
            result._value -= value;
            return result;
        }

        // ++i
        inline type operator++() {
            return ++_value;
        }

        // i++
        inline type operator++(int) {
            return _value++;
        }

        // --i
        inline type operator--() {
            return --_value;
        }

        // i--
        inline type operator--(int) {
            return _value--;
        }

    protected:
        static String toString(const type &value, const String &format = String::Empty) {
            return toValueString(value, format);
        }

        template<typename name>
        static bool parseNumber(const String &str, type &value, NumberStyles style) {
            name temp;
            if (parseValue(str, temp, style)) {
                value = temp._value;
                return true;
            }
            return false;
        }

    protected:
        type _value;
    };

    struct Char : public ValueType<char> {
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
            //     Indicates that the character is a title-case letter. Signified by the Unicode
            //     designation "Lt" (letter, title-case). The value is 2.
            TitlecaseLetter = 2,
            //
            // Summary:
            //     Indicates that the character is a modifier letter, which is freestanding spacing
            //     character that indicates modifications of a preceding letter. Signified by the
            //     Unicode designation "Lm" (letter, modifier). The value is 3.
            ModifierLetter = 3,
            //
            // Summary:
            //     Indicates that the character is a letter that is not an uppercase letter, a lowercase
            //     letter, a title-case letter, or a modifier letter. Signified by the Unicode designation
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

        Char(const char &value = 0);

        Char(const Char &value);

        ~Char() override;

        Char &operator=(const Char &value);

        Char &operator=(const char &value);

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

        Char toLower() const;

        Char toUpper() const;

        bool isDigit() const;

        bool isLetter() const;

        bool isWhiteSpace() const;

        bool isUpper() const;

        bool isLower() const;

        bool isLetterOrDigit() const;

        bool isSymbol() const;

        bool isAscii() const;

    public:
        static bool parse(const String &str, Char &value);

        static bool parse(const String &str, char &value);

        static Char toLower(const Char &c);

        static Char toLower(char c);

        static Char toUpper(const Char &c);

        static Char toUpper(char c);

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
    };

    struct WChar : public ValueType<wchar_t> {
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
            //     Indicates that the character is a title-case letter. Signified by the Unicode
            //     designation "Lt" (letter, title-case). The value is 2.
            TitlecaseLetter = 2,
            //
            // Summary:
            //     Indicates that the character is a modifier letter, which is freestanding spacing
            //     character that indicates modifications of a preceding letter. Signified by the
            //     Unicode designation "Lm" (letter, modifier). The value is 3.
            ModifierLetter = 3,
            //
            // Summary:
            //     Indicates that the character is a letter that is not an uppercase letter, a lowercase
            //     letter, a title-case letter, or a modifier letter. Signified by the Unicode designation
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

        WChar(const wchar_t &value = 0);

        WChar(const WChar &value);

        ~WChar() override;

        WChar &operator=(const WChar &value);

        WChar &operator=(const wchar_t &value);

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

        WChar toLower() const;

        WChar toUpper() const;

        bool isDigit() const;

        bool isLetter() const;

        bool isWhiteSpace() const;

        bool isUpper() const;

        bool isLower() const;

        bool isLetterOrDigit() const;

        bool isSymbol() const;

        bool isAscii() const;

        bool isHanzi() const;

    public:
        static bool parse(const String &str, WChar &value);

        static bool parse(const String &str, wchar_t &value);

        static WChar toLower(const WChar &c);

        static WChar toLower(wchar_t c);

        static WChar toUpper(const WChar &c);

        static WChar toUpper(wchar_t c);

        static uint8_t toHex(wchar_t value);

        static wchar_t toChar(uint8_t value);

        static bool isDigit(wchar_t c);

        static bool isLetter(wchar_t c);

        static bool isWhiteSpace(wchar_t c);

        static bool isUpper(wchar_t c);

        static bool isLower(wchar_t c);

        static bool isLetterOrDigit(wchar_t c);

        static bool isSymbol(wchar_t c);

        static bool isAscii(wchar_t ch);

        static bool isHanzi(wchar_t c);

    private:
        static bool isWhiteSpaceLatin1(wchar_t c);

        // Return true for all characters below or equal U+00ff, which is ASCII + Latin-1 Supplement.
        static bool isLatin1(wchar_t ch);

        // Return true for all characters below or equal U+007f, which is ASCII.
        static bool checkSymbol(UnicodeCategory uc);

        static UnicodeCategory getLatin1UnicodeCategory(wchar_t ch);

        static bool checkLetterOrDigit(UnicodeCategory uc);

    public:
        static const WChar MinValue;
        static const WChar MaxValue;
        static const WChar NewLine;

    private:
        static const uint8_t categoryForLatin1[256];
    };

    struct Int8 : public ValueType<int8_t> {
    public:
        Int8(const int8_t &value = 0);

        Int8(const Int8 &value);

        ~Int8() override;

        Int8 &operator=(const Int8 &value);

        Int8 &operator=(const int8_t &value);

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, Int8 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, int8_t &value, NumberStyles style = NSInteger);

    public:
        static const Int8 MinValue;
        static const Int8 MaxValue;
    };

    struct UInt8 : public ValueType<uint8_t> {
    public:
        UInt8(const uint8_t &value = 0);

        UInt8(const UInt8 &value);

        ~UInt8() override;

        UInt8 &operator=(const UInt8 &value);

        UInt8 &operator=(const uint8_t &value);

        void write(Stream *stream) const;

        void read(Stream *stream);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, UInt8 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, uint8_t &value, NumberStyles style = NSInteger);

    public:
        static const UInt8 MinValue;
        static const UInt8 MaxValue;
    };

    typedef UInt8 Byte;

    struct Int16 : public ValueType<int16_t> {
    public:
        Int16(const int16_t &value = 0);

        Int16(const Int16 &value);

        ~Int16() override;

        Int16 &operator=(const Int16 &value);

        Int16 &operator=(const int16_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, Int16 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, int16_t &value, NumberStyles style = NSInteger);

    public:
        static const Int16 MinValue;
        static const Int16 MaxValue;
    };

    typedef Int16 Short;

    struct UInt16 : public ValueType<uint16_t> {
    public:
        UInt16(const uint16_t &value = 0);

        UInt16(const UInt16 &value);

        ~UInt16() override;

        UInt16 &operator=(const UInt16 &value);

        UInt16 &operator=(const uint16_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, UInt16 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, uint16_t &value, NumberStyles style = NSInteger);

    public:
        static const UInt16 MinValue;
        static const UInt16 MaxValue;
    };

    typedef UInt16 UShort;

    struct Int32 : public ValueType<int32_t> {
    public:
        Int32(const int32_t &value = 0);

        Int32(const Int32 &value);

        ~Int32() override;

        Int32 &operator=(const Int32 &value);

        Int32 &operator=(const int32_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, Int32 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, int32_t &value, NumberStyles style = NSInteger);

    public:
        static const Int32 MinValue;
        static const Int32 MaxValue;
    };

    struct UInt32 : public ValueType<uint32_t> {
    public:
        UInt32(const uint32_t &value = 0);

        UInt32(const UInt32 &value);

        ~UInt32() override;

        UInt32 &operator=(const UInt32 &value);

        UInt32 &operator=(const uint32_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, UInt32 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, uint32_t &value, NumberStyles style = NSInteger);

    public:
        static const UInt32 MinValue;
        static const UInt32 MaxValue;
    };

    struct Int64 : public ValueType<int64_t> {
    public:
        Int64(const int64_t &value = 0);

        Int64(const Int64 &value);

        ~Int64() override;

        Int64 &operator=(const Int64 &value);

        Int64 &operator=(const int64_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, Int64 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, int64_t &value, NumberStyles style = NSInteger);

    public:
        static const Int64 MinValue;
        static const Int64 MaxValue;
    };

    struct UInt64 : public ValueType<uint64_t> {
    public:
        UInt64(const uint64_t &value = 0);

        UInt64(const UInt64 &value);

        ~UInt64() override;

        UInt64 &operator=(const UInt64 &value);

        UInt64 &operator=(const uint64_t &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

    public:
        static bool parse(const String &str, UInt64 &value, NumberStyles style = NSInteger);

        static bool parse(const String &str, uint64_t &value, NumberStyles style = NSInteger);

    public:
        static const UInt64 MinValue;
        static const UInt64 MaxValue;
    };

    struct Float : public ValueType<float> {
    public:
        Float(const float &value = 0.0f);

        Float(const Float &value);

        ~Float() override;

        Float &operator=(const Float &value);

        Float &operator=(const float &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

        // Determines if the given floating point number value is nan.
        // value	-	floating point value
        // true if value is nan, false otherwise
        bool isNaN() const;

        // Determines if the given floating point number value is finite.
        // value	-	floating point value
        // true if value is finite, false otherwise
        bool isFinite() const;

        // Determines if the given floating point number value is infinity.
        // value	-	floating point value
        // true if value is infinity, false otherwise
        bool isInfinity() const;

        // Determines if the given floating point number value is negative.
        // value	-	floating point value
        // true if value is negative, false otherwise
        bool isNegative() const;

        // Determines if the given floating point number value is negative infinity.
        // value	-	floating point value
        // true if arg is negative infinity, false otherwise
        bool isNegativeInfinity() const;

        // Determines if the given floating point number value is positive infinity.
        // value	-	floating point value
        // true if value is positive infinity, false otherwise
        bool isPositiveInfinity() const;

        // Determines if the given floating point number value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
        // value	-	floating point value
        // true if value is normal, false otherwise
        bool isNormal() const;

        // Determines if the given floating point number value is subnormal.
        // value	-	floating point value
        // true if value is subnormal, false otherwise
        bool isSubnormal() const;

    public:
        static bool
        parse(const String &str, Float &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        static bool
        parse(const String &str, float &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        // Determines if the given floating point number value is nan.
        // value	-	floating point value
        // true if value is nan, false otherwise
        static bool isNaN(const Float &value);

        // Determines if the given floating point number value is finite.
        // value	-	floating point value
        // true if value is finite, false otherwise
        static bool isFinite(const Float &value);

        // Determines if the given floating point number value is infinity.
        // value	-	floating point value
        // true if value is infinity, false otherwise
        static bool isInfinity(const Float &value);

        // Determines if the given floating point number value is negative.
        // value	-	floating point value
        // true if value is negative, false otherwise
        static bool isNegative(const Float &value);

        // Determines if the given floating point number value is negative infinity.
        // value	-	floating point value
        // true if arg is negative infinity, false otherwise
        static bool isNegativeInfinity(const Float &value);

        // Determines if the given floating point number value is positive infinity.
        // value	-	floating point value
        // true if value is positive infinity, false otherwise
        static bool isPositiveInfinity(const Float &value);

        // Determines if the given floating point number value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
        // value	-	floating point value
        // true if value is normal, false otherwise
        static bool isNormal(const Float &value);

        // Determines if the given floating point number value is subnormal.
        // value	-	floating point value
        // true if value is subnormal, false otherwise
        static bool isSubnormal(const Float &value);

    public:
        static const Float MinValue;
        static const Float MaxValue;
        static const Float Epsilon;
        static const Float NegativeInfinity;
        static const Float PositiveInfinity;
        static const Float NaN;
    };

    struct Double : public ValueType<double> {
    public:
        Double(const double &value = 0.0);

        Double(const Double &value);

        ~Double() override;

        Double &operator=(const Double &value);

        Double &operator=(const double &value);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

        String toString(const String &format = String::Empty, const IFormatProvider<NumberFormatInfo>* provider = nullptr) const;

        // Determines if the given floating point number value is nan.
        // value	-	floating point value
        // true if value is nan, false otherwise
        bool isNaN() const;

        // Determines if the given floating point number value is finite.
        // value	-	floating point value
        // true if value is finite, false otherwise
        bool isFinite() const;

        // Determines if the given floating point number value is infinity.
        // value	-	floating point value
        // true if value is infinity, false otherwise
        bool isInfinity() const;

        // Determines if the given floating point number value is negative.
        // value	-	floating point value
        // true if value is negative, false otherwise
        bool isNegative() const;

        // Determines if the given floating point number value is negative infinity.
        // value	-	floating point value
        // true if arg is negative infinity, false otherwise
        bool isNegativeInfinity() const;

        // Determines if the given floating point number value is positive infinity.
        // value	-	floating point value
        // true if value is positive infinity, false otherwise
        bool isPositiveInfinity() const;

        // Determines if the given floating point number value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
        // value	-	floating point value
        // true if value is normal, false otherwise
        bool isNormal() const;

        // Determines if the given floating point number value is subnormal.
        // value	-	floating point value
        // true if value is subnormal, false otherwise
        bool isSubnormal() const;

    public:
        static bool
        parse(const String &str, Double &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        static bool
        parse(const String &str, double &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        // Determines if the given floating point number value is nan.
        // value	-	floating point value
        // true if value is nan, false otherwise
        static bool isNaN(const Double &value);

        // Determines if the given floating point number value is finite.
        // value	-	floating point value
        // true if value is finite, false otherwise
        static bool isFinite(const Double &value);

        // Determines if the given floating point number value is infinity.
        // value	-	floating point value
        // true if value is infinity, false otherwise
        static bool isInfinity(const Double &value);

        // Determines if the given floating point number value is negative.
        // value	-	floating point value
        // true if value is negative, false otherwise
        static bool isNegative(const Double &value);

        // Determines if the given floating point number value is negative infinity.
        // value	-	floating point value
        // true if arg is negative infinity, false otherwise
        static bool isNegativeInfinity(const Double &value);

        // Determines if the given floating point number value is positive infinity.
        // value	-	floating point value
        // true if value is positive infinity, false otherwise
        static bool isPositiveInfinity(const Double &value);

        // Determines if the given floating point number value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
        // value	-	floating point value
        // true if value is normal, false otherwise
        static bool isNormal(const Double &value);

        // Determines if the given floating point number value is subnormal.
        // value	-	floating point value
        // true if value is subnormal, false otherwise
        static bool isSubnormal(const Double &value);

    public:
        static const Double MinValue;
        static const Double MaxValue;
        static const Double Epsilon;
        static const Double NegativeInfinity;
        static const Double PositiveInfinity;
        static const Double NaN;
    };
}

#endif /* ValueType_h */
