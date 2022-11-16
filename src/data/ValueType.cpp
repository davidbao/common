//
//  ValueType.cpp
//  common
//
//  Created by baowei on 2015/10/27.
//  Copyright © 2015 com. All rights reserved.
//

#include <cctype>
#include <cinttypes>
#include <clocale>
#include <cfloat>
#include "data/ValueType.h"
#include "data/WString.h"
#include "data/StringArray.h"
#include "IO/Stream.h"
#include "system/Math.h"

// Limits of integer and floating-point types
// https://en.cppreference.com/w/cpp/types/climits

namespace Common {
    const Boolean Boolean::TrueValue = true;
    const Boolean Boolean::FalseValue = false;

    Boolean::Boolean(const bool &value) : _value(value) {
    }

    Boolean::Boolean(const Boolean &value) : _value(value._value) {
    }

    Boolean::~Boolean() {
    }

    bool Boolean::equals(const Boolean &other) const {
        return _value == other._value;
    }

    bool Boolean::equals(const bool &other) const {
        return _value == other;
    }

    void Boolean::evaluates(const Boolean &other) {
        _value = other._value;
    }

    Boolean::operator bool() const {
        return _value;
    }

    Boolean &Boolean::operator=(const Boolean &other) {
        evaluates(other);
        return *this;
    }

    Boolean &Boolean::operator=(const bool &value) {
        evaluates(value);
        return *this;
    }

    void Boolean::write(Stream *stream) const {
        stream->writeBoolean(_value);
    }

    void Boolean::read(Stream *stream) {
        _value = stream->readBoolean();
    }

    String Boolean::toString() const {
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

    BaseValueType::BaseValueType() {
    }

    BaseValueType::~BaseValueType() {
    }

    template<typename type>
    String BaseValueType::toValueString(const type &value, const String &format) {
        // https://learn.microsoft.com/en-us/dotnet/standard/base-types/standard-numeric-format-strings
        String fmt = !format.isNullOrEmpty() ? format : "G";
        const char specifier = Char::toLower(fmt[0]);
        int size = 0;
        if (Char::isLetter(specifier) && fmt.length() > 1) {
            String str = fmt.substr(1, fmt.length() - 1);
            Int32::parse(str, size);
        }
        static const char *defLocale = "en_US";
        char result[255];
        memset(result, 0, sizeof(result));
        if (specifier == 'c') {
            // Result: A currency value.
            // Supported by: All numeric types.
            // Precision specifier: Number of decimal digits.
            int pointSize = size > 0 ? size : 2;
            String localeStr;
            struct lconv *lc = localeconv();
            if (lc->currency_symbol[0] == '\0') {
                // The default locale has no currency symbol.
                localeStr = setlocale(LC_MONETARY, nullptr);
                setlocale(LC_MONETARY, defLocale);
                lc = localeconv();
            }
            String fmtStr;
            static const char *quantities = "%.*lf";
            const double v = value >= 0 ? (double) value : -(double) value;
            const bool hasSymbol = lc->currency_symbol[0] != '\0';
            const char *space, *symbol, *sign;
            char sign_pos, cs_precedes;
            if (hasSymbol) {
                space = lc->n_sep_by_space ? " " : "";
                symbol = lc->currency_symbol;
                sign = value >= 0 ? lc->positive_sign : lc->negative_sign;
                sign_pos = hasSymbol ? (value >= 0 ? lc->p_sign_posn : lc->n_sign_posn) : 127;
                cs_precedes = value >= 0 ? lc->p_cs_precedes : lc->n_cs_precedes;
            } else {
                space = "";
                symbol = "$";
                sign = value >= 0 ? "" : "-";
                sign_pos = 127;
                cs_precedes = 1;
            }
            if (sign_pos == 0) {
                // Currency symbol and quantity surrounded by parentheses.
                fmtStr.append('(');
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
                fmtStr.append(')');
            } else if (sign_pos == 1) {
                // Sign before the quantity and currency symbol.
                fmtStr.append(sign);
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
            } else if (sign_pos == 2) {
                // Sign after the quantity and currency symbol.
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
                fmtStr.append(sign);
            } else if (sign_pos == 3) {
                // Sign right before currency symbol.
                fmtStr.append(sign);
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
            } else if (sign_pos == 4) {
                // Sign right after currency symbol.
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
                fmtStr.append(sign);
            } else {
                // Sign before the quantity and currency symbol.
                fmtStr.append(sign);
                if (cs_precedes) {
                    // If this value is 1, the currency symbol should precede
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                    fmtStr.append(quantities);
                } else {
                    fmtStr.append(quantities);
                    fmtStr.append(symbol);
                    fmtStr.append(space);
                }
            }
            sprintf(result, fmtStr.c_str(), pointSize, v);
            if (!localeStr.isNullOrEmpty()) {
                setlocale(LC_MONETARY, localeStr.c_str());
            }
        } else if (specifier == 'd') {
            // Result: Integer digits with optional negative sign.
            // Supported by: Integral types only.
            // Precision specifier: Minimum number of digits.
            int integerSize = size + (value < 0 ? 1 : 0);
            if (((long double) value) <= (long double) Int64::MaxValue) {
                sprintf(result, "%0*" PRId64, integerSize, (int64_t) value);
            } else {
                sprintf(result, "%0*" PRIu64, integerSize, (uint64_t) value);
            }
        } else if (specifier == 'e') {
            // Result: Exponential notation.
            // Supported by: All numeric types.
            // Precision specifier: Number of decimal digits.
            int integerSize = size + (size > 0 ? 1 : 0);
            int pointSize = size > 0 ? size : 6;
            sprintf(result, fmt[0] == 'e' ? "%0*.*e" : "%0*.*E", integerSize, pointSize, (double) value);
        } else if (specifier == 'f') {
            // Result: Integral and decimal digits with optional negative sign.
            // Supported by: All numeric types.
            // Precision specifier: Number of decimal digits.
            int integerSize = size + (size > 0 ? 1 : 0);
            int pointSize = size > 0 ? size : 2;
            sprintf(result, "%0*.*lf", integerSize, pointSize, (double) value);
        } else if (specifier == 'g') {
            // Result: The more compact of either fixed-point or scientific notation.
            // Supported by: All numeric types.
            // Precision specifier: Number of significant digits.
            int pointSize = size;
            if (pointSize > 0) {
                sprintf(result, fmt[0] == 'g' ? "%.*lg" : "%.*lG", pointSize, (double) value);
            } else {
                sprintf(result, fmt[0] == 'g' ? "%lg" : "%lG", (double) value);
            }
        } else if (specifier == 'n') {
            // Result: Integral and decimal digits, group separators, and a decimal separator with optional negative sign.
            // Supported by: All numeric types.
            // Precision specifier: Desired number of decimal places.
            int integerSize = 0;
            int pointSize = size > 0 ? size : 2;
            sprintf(result, "%.*lf", pointSize, (double) value);
            addThousandSeparator(result, result);
        } else if (specifier == 'p') {
            // Result: Number multiplied by 100 and displayed with a percent symbol.
            // Supported by: All numeric types.
            // Precision specifier: Desired number of decimal places.
            int pointSize = size > 0 ? size : 2;
            sprintf(result, "%.*lf %%", pointSize, ((double) value) * 100.0);
        } else if (specifier == 'x') {
            // Result: A hexadecimal string.
            // Supported by: Integral types only.
            // Precision specifier: Number of digits in the result string.
            int integerSize = size;
            uint64_t v;
            switch (sizeof(value)) {
                case 1:
                    v = (uint8_t) value;
                    break;
                case 2:
                    v = (uint16_t) value;
                    break;
                case 4:
                    v = (uint32_t) value;
                    break;
                case 8:
                    v = (uint64_t) value;
                    break;
                default:
                    v = (uint32_t) value;
                    break;
            }
            sprintf(result, fmt[0] == 'x' ? "%0*" PRIx64 : "%0*" PRIX64, integerSize, v);
        } else {
            StringArray texts;
            StringArray::parse(format, texts, '.');
            const String &integerStr = texts.count() > 0 ? texts[0] : String::Empty;
            int integerSize = 0;
            for (char i: integerStr) {
                if (i == '0')
                    integerSize++;
            }

            const String &pointStr = texts.count() > 1 ? texts[1] : String::Empty;
            int pointSize = 0;
            for (char i: pointStr) {
                if (i == '0')
                    pointSize++;
            }

            if (pointSize >= 0 && pointSize <= 15) {
                sprintf(result, "%0*.*lf", integerSize + pointSize + (pointSize > 0 ? 1 : 0), pointSize,
                        (double) value);
            } else {
                sprintf(result, "%lg", (double) value);
            }
        }
//        printf("(%s: %s)\n", format.c_str(), result);
        return result;
    }

    void BaseValueType::addThousandSeparator(const char *str, char *result) {
        String localeStr;
        struct lconv *lc = localeconv();
        static const char *defLocale = "en_US";
        if (!(lc->thousands_sep != nullptr && lc->thousands_sep[0] != '\0')) {
            // The default locale has no thousands' separator.
            localeStr = setlocale(LC_NUMERIC, nullptr);
            setlocale(LC_NUMERIC, defLocale);
            lc = localeconv();
        }
        String decimalPoint = lc->decimal_point[0] != '\0' ? lc->decimal_point : ".";
        String thousandsSep = lc->thousands_sep[0] != '\0' ? lc->thousands_sep : ",";

        String temp = str;
        ssize_t index = temp.findLastOf(decimalPoint);
        if (index <= 0) {
            index = (ssize_t) temp.length();
        }
        while (index > 0) {
            index -= 3;
            if (index > 0) {
                temp.insert(index, thousandsSep);
            }
        }
        memcpy(result, temp.c_str(), temp.length());

        if (!localeStr.isNullOrEmpty()) {
            setlocale(LC_NUMERIC, localeStr.c_str());
        }
    }

    bool BaseValueType::parseInt64(const String &str, int64_t &value, NumberStyles style) {
        if ((style & NumberStyles::NSAllowHexSpecifier) != 0) {
            uint len = 0;
            if (sscanf(str.c_str(), "%" PRIx64 "%n", &value, &len) == 1 && str.length() == len) {
                return true;
            }
        } else {
            uint len = 0;
            if (sscanf(str.c_str(), "%" PRId64 "%n", &value, &len) == 1 && str.length() == len) {
                return true;
            }
        }
        return false;
    }

    bool BaseValueType::parseUInt64(const String &str, uint64_t &value, NumberStyles style) {
        if ((style & NumberStyles::NSAllowHexSpecifier) != 0) {
            uint len = 0;
            if (sscanf(str.c_str(), "%" PRIx64 "%n", &value, &len) == 1 && str.length() == len) {
                return true;
            }
        } else {
            uint len = 0;
            if (sscanf(str.c_str(), "%" PRIu64 "%n", &value, &len) == 1 && str.length() == len) {
                return true;
            }
        }
        return false;
    }

    bool BaseValueType::parseDouble(const String &str, double &value, NumberStyles style) {
        uint len = 0;
        if (sscanf(str.c_str(), "%lf%n", &value, &len) == 1 && str.length() == len) {
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

    Char::Char(const char &value) : ValueType<char>(value) {
    }

    Char::Char(const Char &value) : ValueType<char>(value) {
    }

    Char::~Char() {
    }

    Char &Char::operator=(const Char &value) {
        _value = value._value;
        return *this;
    }

    Char &Char::operator=(const char &value) {
        _value = value;
        return *this;
    }

    void Char::write(Stream *stream) const {
        stream->writeByte(_value);
    }

    void Char::read(Stream *stream) {
        _value = (char) stream->readByte();
    }

    String Char::toString(const String &format) const {
        return toValueString(_value, format);
    }

    Char Char::toLower() const {
        return Char::toLower(_value);
    }

    Char Char::toUpper() const {
        return Char::toUpper(_value);
    }

    bool Char::isDigit() const {
        return Char::isDigit(_value);
    }

    bool Char::isLetter() const {
        return Char::isLetter(_value);
    }

    bool Char::isWhiteSpace() const {
        return Char::isWhiteSpace(_value);
    }

    bool Char::isUpper() const {
        return Char::isUpper(_value);
    }

    bool Char::isLower() const {
        return Char::isLower(_value);
    }

    bool Char::isLetterOrDigit() const {
        return Char::isLetterOrDigit(_value);
    }

    bool Char::isSymbol() const {
        return Char::isSymbol(_value);
    }

    bool Char::isAscii() const {
        return Char::isAscii(_value);
    }

    bool Char::parse(const String &str, Char &value) {
        return parse(str, value._value);
    }

    bool Char::parse(const String &str, char &value) {
        value = '\0';
        if (str.length() != 1) {
            return false;
        }
        value = str[0];
        return true;
    }

    Char Char::toLower(const Char &c) {
        return (char) ::tolower(c);
    }

    Char Char::toLower(char c) {
        return (char) ::tolower(c);
    }

    Char Char::toUpper(const Char &c) {
        return (char) ::toupper(c);
    }

    Char Char::toUpper(char c) {
        return (char) ::toupper(c);
    }

    char Char::toChar(uint8_t value) {
        char result;
        if (value <= 0x09)
            result = (char) (value + 0x30);
        else if ((value >= 0x0A) && (value <= 0x0F))
            result = (char) (value + 0x37);
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
        return ((uint32_t) ch <= 0xFF);
    }

    // Return true for all characters below or equal U+007f, which is ASCII.
    bool Char::isAscii(char ch) {
        return ((uint32_t) ch <= 0x7F);
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
        // U+000c = <control> FORM FEED
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
     **Arguments: c -- the character to be checked.
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
     **Arguments: c -- the character to be checked.
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
        return (UnicodeCategory) (categoryForLatin1[(uint8_t) ch]);
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
    const WChar WChar::NewLine = '\n';
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

    WChar::WChar(const wchar_t &value) : ValueType<wchar_t>(value) {
    }

    WChar::WChar(const WChar &value) : ValueType<wchar_t>(value) {
    }

    WChar::~WChar() {
    }

    WChar &WChar::operator=(const WChar &value) {
        _value = value._value;
        return *this;
    }

    WChar &WChar::operator=(const wchar_t &value) {
        _value = value;
        return *this;
    }

    void WChar::write(Stream *stream) const {
        stream->writeUInt16(_value);
    }

    void WChar::read(Stream *stream) {
        _value = stream->readUInt16();
    }

    String WChar::toString(const String &format) const {
        return toValueString(_value, format);
    }

    WChar WChar::toLower() const {
        return WChar::toLower(_value);
    }

    WChar WChar::toUpper() const {
        return WChar::toUpper(_value);
    }

    bool WChar::isDigit() const {
        return WChar::isDigit(_value);
    }

    bool WChar::isLetter() const {
        return WChar::isLetter(_value);
    }

    bool WChar::isWhiteSpace() const {
        return WChar::isWhiteSpace(_value);
    }

    bool WChar::isUpper() const {
        return WChar::isUpper(_value);
    }

    bool WChar::isLower() const {
        return WChar::isLower(_value);
    }

    bool WChar::isLetterOrDigit() const {
        return WChar::isLetterOrDigit(_value);
    }

    bool WChar::isSymbol() const {
        return WChar::isSymbol(_value);
    }

    bool WChar::isAscii() const {
        return WChar::isAscii(_value);
    }

    bool WChar::isHanzi() const {
        return WChar::isHanzi(_value);
    }

    bool WChar::parse(const String &str, WChar &value) {
        return parse(str, value._value);
    }

    bool WChar::parse(const String &str, wchar_t &value) {
        value = L'\0';
        WString temp = str;
        if (temp.length() != 1) {
            return false;
        }
        value = temp[0];
        return true;
    }

    WChar WChar::toLower(const WChar &c) {
        return (wchar_t) ::tolower(c);
    }

    WChar WChar::toLower(wchar_t c) {
        return (wchar_t) ::tolower(c);
    }

    WChar WChar::toUpper(const WChar &c) {
        return (wchar_t) ::toupper(c);
    }

    WChar WChar::toUpper(wchar_t c) {
        return (wchar_t) ::toupper(c);
    }

    wchar_t WChar::toChar(uint8_t value) {
        wchar_t result;
        if (value <= 9)
            result = value + 0x30;
        else if ((value >= 10) && (value <= 15))
            result = value + 0x37;
        else
            result = (wchar_t) 0xff;
        return result;
    }

    uint8_t WChar::toHex(wchar_t value) {
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
    bool WChar::isLatin1(wchar_t ch) {
        return ((uint32_t) ch <= 0xFF);
    }

    // Return true for all characters below or equal U+007f, which is ASCII.
    bool WChar::isAscii(wchar_t ch) {
        return ((uint32_t) ch <= 0x7F);
    }
    /*=================================ISDIGIT======================================
     **A wrapper for WChar.  Returns a boolean indicating whether    **
     **character c is considered to be a digit.                                    **
     ==============================================================================*/
    // Determines whether a character is a digit.
    bool WChar::isDigit(wchar_t c) {
        if (isLatin1(c)) {
            return (c >= '0' && c <= '9');
        }
        return false;
    }
    /*=================================ISLETTER=====================================
     **A wrapper for WChar.  Returns a boolean indicating whether    **
     **character c is considered to be a letter.                                   **
     ==============================================================================*/
    // Determines whether a character is a letter.
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
        // There are characters which belong to UnicodeCategory::Control but are considered as white spaces.
        // We use code point comparisons for these characters here as a temporary fix.

        // U+0009 = <control> HORIZONTAL TAB
        // U+000a = <control> LINE FEED
        // U+000b = <control> VERTICAL TAB
        // U+000c = <control> FORM FEED
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
     **character c is considered to be a whitespace character.                     **
     ==============================================================================*/
    // Determines whether a character is whitespace.
    bool WChar::isWhiteSpace(wchar_t c) {
        if (isLatin1(c)) {
            return (isWhiteSpaceLatin1(c));
        }
        return false;
    }
    /*===================================IsUpper====================================
     **Arguments: c -- the character to be checked.
     **Returns:  True if c is an uppercase character.
     ==============================================================================*/
    // Determines whether a character is upper-case.
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
     **Arguments: c -- the character to be checked.
     **Returns:  True if c is an lowercase character.
     ==============================================================================*/
    // Determines whether a character is lower-case.
    bool WChar::isLower(wchar_t c) {
        if (isLatin1(c)) {
            if (isAscii(c)) {
                return (c >= 'a' && c <= 'z');
            }
            return false;
        }
        return false;
    }

    // Determines whether a character is a letter or a digit.
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

    Int8::Int8(const int8_t &value) : ValueType<int8_t>(value) {
    }

    Int8::Int8(const Int8 &value) : ValueType<int8_t>(value) {
    }

    Int8::~Int8() {
    }

    Int8 &Int8::operator=(const Int8 &value) {
        _value = value._value;
        return *this;
    }

    Int8 &Int8::operator=(const int8_t &value) {
        _value = _value;
        return *this;
    }

    void Int8::write(Stream *stream) const {
        stream->writeInt8(_value);
    }

    void Int8::read(Stream *stream) {
        _value = stream->readInt8();
    }

    String Int8::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Int8::parse(const String &str, Int8 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Int8::parse(const String &str, int8_t &value, NumberStyles style) {
        int64_t v;
        if (parseInt64(str, v, style) &&
            (v >= Int8::MinValue && v <= Int8::MaxValue)) {
            value = (int8_t) v;
            return true;
        }
        return false;
    }

    const UInt8 UInt8::MaxValue = UInt8(UINT8_MAX);
    const UInt8 UInt8::MinValue = UInt8(0);

    UInt8::UInt8(const uint8_t &value) : ValueType<uint8_t>(value) {
    }

    UInt8::UInt8(const UInt8 &value) : ValueType<uint8_t>(value) {
    }

    UInt8::~UInt8() {
    }

    UInt8 &UInt8::operator=(const UInt8 &value) {
        _value = value._value;
        return *this;
    }

    UInt8 &UInt8::operator=(const uint8_t &value) {
        _value = value;
        return *this;
    }

    void UInt8::write(Stream *stream) const {
        stream->writeUInt8(_value);
    }

    void UInt8::read(Stream *stream) {
        _value = stream->readUInt8();
    }

    String UInt8::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool UInt8::parse(const String &str, UInt8 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool UInt8::parse(const String &str, uint8_t &value, NumberStyles style) {
        uint64_t v;
        if (parseUInt64(str, v, style) &&
            (v >= UInt8::MinValue && v <= UInt8::MaxValue)) {
            value = v;
            return true;
        }
        return false;
    }

    const Int16 Int16::MaxValue = Int16(INT16_MAX);
    const Int16 Int16::MinValue = Int16(INT16_MIN);

    Int16::Int16(const int16_t &value) : ValueType<int16_t>(value) {
    }

    Int16::Int16(const Int16 &value) : ValueType<int16_t>(value) {
    }

    Int16::~Int16() {
    }

    Int16 &Int16::operator=(const Int16 &value) {
        _value = value._value;
        return *this;
    }

    Int16 &Int16::operator=(const int16_t &value) {
        _value = value;
        return *this;
    }

    void Int16::write(Stream *stream, bool bigEndian) const {
        stream->writeInt16(_value, bigEndian);
    }

    void Int16::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt16(bigEndian);
    }

    String Int16::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Int16::parse(const String &str, Int16 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Int16::parse(const String &str, int16_t &value, NumberStyles style) {
        int64_t v;
        if (parseInt64(str, v, style) &&
            (v >= Int16::MinValue && v <= Int16::MaxValue)) {
            value = (int16_t) v;
            return true;
        }
        return false;
    }

    const UInt16 UInt16::MaxValue = UInt16(UINT16_MAX);
    const UInt16 UInt16::MinValue = UInt16(0);

    UInt16::UInt16(const uint16_t &value) : ValueType<uint16_t>(value) {
    }

    UInt16::UInt16(const UInt16 &value) : ValueType<uint16_t>(value) {
    }

    UInt16::~UInt16() {
    }

    UInt16 &UInt16::operator=(const UInt16 &value) {
        _value = value._value;
        return *this;
    }

    UInt16 &UInt16::operator=(const uint16_t &value) {
        _value = value;
        return *this;
    }

    void UInt16::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt16(_value, bigEndian);
    }

    void UInt16::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt16(bigEndian);
    }

    String UInt16::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool UInt16::parse(const String &str, UInt16 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool UInt16::parse(const String &str, uint16_t &value, NumberStyles style) {
        uint64_t v;
        if (parseUInt64(str, v, style) &&
            (v >= UInt16::MinValue && v <= UInt16::MaxValue)) {
            value = (uint16_t) v;
            return true;
        }
        return false;
    }

    const Int32 Int32::MaxValue = Int32(INT32_MAX);
    const Int32 Int32::MinValue = Int32(INT32_MIN);

    Int32::Int32(const int32_t &value) : ValueType<int32_t>(value) {
    }

    Int32::Int32(const Int32 &value) : ValueType<int32_t>(value) {
    }

    Int32::~Int32() {
    }

    Int32 &Int32::operator=(const Int32 &value) {
        _value = value._value;
        return *this;
    }

    Int32 &Int32::operator=(const int32_t &value) {
        _value = value;
        return *this;
    }

    void Int32::write(Stream *stream, bool bigEndian) const {
        stream->writeInt32(_value, bigEndian);
    }

    void Int32::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt32(bigEndian);
    }

    String Int32::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Int32::parse(const String &str, Int32 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Int32::parse(const String &str, int32_t &value, NumberStyles style) {
        int64_t v;
        if (parseInt64(str, v, style) &&
            (v >= Int32::MinValue && v <= Int32::MaxValue)) {
            value = (int32_t) v;
            return true;
        }
        return false;
    }

    const UInt32 UInt32::MaxValue = UInt32(UINT32_MAX);
    const UInt32 UInt32::MinValue = UInt32(0);

    UInt32::UInt32(const uint32_t &value) : ValueType<uint32_t>(value) {
    }

    UInt32::UInt32(const UInt32 &value) : ValueType<uint32_t>(value) {
    }

    UInt32::~UInt32() {
    }

    UInt32 &UInt32::operator=(const UInt32 &value) {
        _value = value._value;
        return *this;
    }

    UInt32 &UInt32::operator=(const uint32_t &value) {
        _value = value;
        return *this;
    }

    void UInt32::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt32(_value, bigEndian);
    }

    void UInt32::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt32(bigEndian);
    }

    String UInt32::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool UInt32::parse(const String &str, UInt32 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool UInt32::parse(const String &str, uint32_t &value, NumberStyles style) {
        uint64_t v;
        if (parseUInt64(str, v, style) &&
            (v >= UInt32::MinValue && v <= UInt32::MaxValue)) {
            value = (uint32_t) v;
            return true;
        }
        return false;
    }

    const Int64 Int64::MaxValue = Int64(INT64_MAX);
    const Int64 Int64::MinValue = Int64(INT64_MIN);

    Int64::Int64(const int64_t &value) : ValueType<int64_t>(value) {
    }

    Int64::Int64(const Int64 &value) : ValueType<int64_t>(value) {
    }

    Int64::~Int64() {
    }

    Int64 &Int64::operator=(const Int64 &value) {
        _value = value._value;
        return *this;
    }

    Int64 &Int64::operator=(const int64_t &value) {
        _value = value;
        return *this;
    }

    void Int64::write(Stream *stream, bool bigEndian) const {
        stream->writeInt64(_value, bigEndian);
    }

    void Int64::read(Stream *stream, bool bigEndian) {
        _value = stream->readInt64(bigEndian);
    }

    String Int64::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Int64::parse(const String &str, Int64 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Int64::parse(const String &str, int64_t &value, NumberStyles style) {
        int64_t v;
        if (parseInt64(str, v, style) &&
            (v >= Int64::MinValue && v <= Int64::MaxValue)) {
            value = v;
            return true;
        }
        return false;
    }

    const UInt64 UInt64::MaxValue = UInt64(UINT64_MAX);
    const UInt64 UInt64::MinValue = UInt64(0);

    UInt64::UInt64(const uint64_t &value) : ValueType<uint64_t>(value) {
    }

    UInt64::UInt64(const UInt64 &value) : ValueType<uint64_t>(value) {
    }

    UInt64::~UInt64() {
    }

    UInt64 &UInt64::operator=(const UInt64 &value) {
        _value = value._value;
        return *this;
    }

    UInt64 &UInt64::operator=(const uint64_t &value) {
        _value = value;
        return *this;
    }

    void UInt64::write(Stream *stream, bool bigEndian) const {
        stream->writeUInt64(_value, bigEndian);
    }

    void UInt64::read(Stream *stream, bool bigEndian) {
        _value = stream->readUInt64(bigEndian);
    }

    String UInt64::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool UInt64::parse(const String &str, UInt64 &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool UInt64::parse(const String &str, uint64_t &value, NumberStyles style) {
        uint64_t v;
        if (parseUInt64(str, v, style) &&
            (v >= UInt64::MinValue && v <= UInt64::MaxValue)) {
            value = v;
            return true;
        }
        return false;
    }

    const Float Float::MaxValue = Float(FLT_MAX);
    const Float Float::MinValue = Float(-FLT_MAX);
    const Float Float::Epsilon = Float(FLT_EPSILON);
    const Float Float::NegativeInfinity = Float(-HUGE_VALF);
    const Float Float::PositiveInfinity = Float(HUGE_VALF);
    const Float Float::NaN = Float(NAN);

    Float::Float(const float &value) : ValueType<float>(value) {
    }

    Float::Float(const Float &value) : ValueType<float>(value) {
    }

    Float::~Float() {
    }

    Float &Float::operator=(const Float &value) {
        _value = value._value;
        return *this;
    }

    Float &Float::operator=(const float &value) {
        _value = value;
        return *this;
    }

    void Float::write(Stream *stream, bool bigEndian) const {
        stream->writeFloat(_value, bigEndian);
    }

    void Float::read(Stream *stream, bool bigEndian) {
        _value = stream->readFloat(bigEndian);
    }

    String Float::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Float::parse(const String &str, Float &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Float::parse(const String &str, float &value, NumberStyles style) {
        double v;
        if (parseDouble(str, v, style) &&
            (v >= Float::MinValue && v <= Float::MaxValue)) {
            value = (float) v;
            return true;
        }
        return false;
    }

    bool Float::isNaN() const {
        return Float::isNaN(_value);
    }

    bool Float::isFinite() const {
        return Float::isFinite(_value);
    }

    bool Float::isInfinity() const {
        return Float::isInfinity(_value);
    }

    bool Float::isNegative() const {
        return Float::isNegative(_value);
    }

    bool Float::isNegativeInfinity() const {
        return Float::isNegativeInfinity(_value);
    }

    bool Float::isPositiveInfinity() const {
        return Float::isPositiveInfinity(_value);
    }

    bool Float::isNormal() const {
        return Float::isNormal(_value);
    }

    bool Float::isSubnormal() const {
        return Float::isSubnormal(_value);
    }

    bool Float::isNaN(const Float &value) {
        return ::isnan(value._value);
    }

    bool Float::isFinite(const Float &value) {
        return ::isfinite(value._value);
    }

    bool Float::isInfinity(const Float &value) {
        return ::isinf(value._value);
    }

    bool Float::isNegative(const Float &value) {
        return signbit(value._value);
    }

    bool Float::isNegativeInfinity(const Float &value) {
        return *(uint32_t *) (&value._value) == (uint32_t) 0xFF800000;
    }

    bool Float::isPositiveInfinity(const Float &value) {
        return *(int32_t *) (&value._value) == (int32_t) 0x7F800000;
    }

    bool Float::isNormal(const Float &value) {
        return ::isnormal(value._value);
    }

    bool Float::isSubnormal(const Float &value) {
        return ::fpclassify(value._value) == FP_SUBNORMAL;
    }

    const Double Double::MaxValue = Double(DBL_MAX);
    const Double Double::MinValue = Double(-DBL_MAX);
    const Double Double::Epsilon = Double(DBL_EPSILON);
    const Double Double::NegativeInfinity = Double(-HUGE_VAL);
    const Double Double::PositiveInfinity = Double(HUGE_VAL);
    const Double Double::NaN = Double(NAN);

    Double::Double(const double &value) : ValueType<double>(value) {
    }

    Double::Double(const Double &value) : ValueType<double>(value) {
    }

    Double::~Double() {
    }

    Double &Double::operator=(const Double &value) {
        _value = value._value;
        return *this;
    }

    Double &Double::operator=(const double &value) {
        _value = value;
        return *this;
    }

    void Double::write(Stream *stream, bool bigEndian) const {
        stream->writeDouble(_value, bigEndian);
    }

    void Double::read(Stream *stream, bool bigEndian) {
        _value = stream->readDouble(bigEndian);
    }

    String Double::toString(const String &format) const {
        return toValueString(_value, format);
    }

    bool Double::parse(const String &str, Double &value, NumberStyles style) {
        return parse(str, value._value, style);
    }

    bool Double::parse(const String &str, double &value, NumberStyles style) {
        double v;
        if (parseDouble(str, v, style) &&
            (v >= Double::MinValue && v <= Double::MaxValue)) {
            value = v;
            return true;
        }
        return false;
    }

    bool Double::isNaN() const {
        return Double::isNaN(_value);
    }

    bool Double::isFinite() const {
        return Double::isFinite(_value);
    }

    bool Double::isInfinity() const {
        return Double::isInfinity(_value);
    }

    bool Double::isNegative() const {
        return Double::isNegative(_value);
    }

    bool Double::isNegativeInfinity() const {
        return Double::isNegativeInfinity(_value);
    }

    bool Double::isPositiveInfinity() const {
        return Double::isPositiveInfinity(_value);
    }

    bool Double::isNormal() const {
        return Double::isNormal(_value);
    }

    bool Double::isSubnormal() const {
        return Double::isSubnormal(_value);
    }

    bool Double::isNaN(const Double &value) {
        return ::isnan(value._value);
    }

    bool Double::isFinite(const Double &value) {
        return ::isfinite(value._value);
    }

    bool Double::isInfinity(const Double &value) {
        return ::isinf(value._value);
    }

    bool Double::isNegative(const Double &value) {
        return signbit(value._value);
    }

    bool Double::isNegativeInfinity(const Double &value) {
        return value == NegativeInfinity;
    }

    bool Double::isPositiveInfinity(const Double &value) {
        return value == PositiveInfinity;
    }

    bool Double::isNormal(const Double &value) {
        return ::isnormal(value._value);
    }

    bool Double::isSubnormal(const Double &value) {
        return ::fpclassify(value._value) == FP_SUBNORMAL;
    }
}
