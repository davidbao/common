//
//  TimeSpanFormat.cpp
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright © 2022 com. All rights reserved.
//

#include "TimeSpanFormat.h"
#include "data/StringArray.h"
#include "system/Math.h"

namespace Common {
    const String FormatLiterals::FixedNumberFormats[MaxSecondsFractionDigits] = {
            "0",
            "00",
            "000",
            "0000",
            "00000",
            "000000",
            "0000000",
    };

    FormatLiterals::FormatLiterals() = default;

    FormatLiterals::FormatLiterals(const FormatLiterals &fl) {
        AppCompatLiteral = fl.AppCompatLiteral;
        dd = fl.dd;
        hh = fl.hh;
        mm = fl.mm;
        ss = fl.ss;
        ff = fl.ff;
        for (int i = 0; i < 6; ++i) {
            literals[i] = fl.literals[i];
        }
    }

    const String &FormatLiterals::Start() const {
        return literals[0];
    }

    String FormatLiterals::DayHourSep() const {
        return literals[1];
    }

    String FormatLiterals::HourMinuteSep() const {
        return literals[2];
    }

    String FormatLiterals::MinuteSecondSep() const {
        return literals[3];
    }

    String FormatLiterals::SecondFractionSep() const {
        return literals[4];
    }

    String FormatLiterals::End() const {
        return literals[5];
    }

    /* factory method for static invariant FormatLiterals */
    FormatLiterals FormatLiterals::InitInvariant(bool isNegative) {
        FormatLiterals result;
        result.literals[0] = isNegative ? "-" : String::Empty;
        result.literals[1] = ".";
        result.literals[2] = ":";
        result.literals[3] = ":";
        result.literals[4] = ".";
        result.literals[5] = String::Empty;
        result.AppCompatLiteral = ":."; // MinuteSecondSep+SecondFractionSep;
        result.dd = 2;
        result.hh = 2;
        result.mm = 2;
        result.ss = 2;
        result.ff = MaxSecondsFractionDigits;
        return result;
    }

    // For the "v1" TimeSpan localized patterns, the data is simply literal field separators with
    // the constants guaranteed to include DHMSF ordered greatest to least significant.
    // Once the data becomes more complex than this we will need to write a proper tokenizer for
    // parsing and formatting
    void FormatLiterals::Init(const String &format, bool useInvariantFieldLengths) {
        for (int i = 0; i < 6; i++)
            literals[i] = String::Empty;
        dd = 0;
        hh = 0;
        mm = 0;
        ss = 0;
        ff = 0;

        String sb;
        bool inQuote = false;
        char quote = '\'';
        int field = 0;

        for (int i = 0; i < format.length(); i++) {
            switch (format[i]) {
                case '\'':
                case '\"':
                    if (inQuote && (quote == format[i])) {
                        /* we were in a quote and found a matching exit quote, so we are outside a quote now */
                        assert(field >= 0 && field <= 5);
                        if (field >= 0 && field <= 5) {
                            literals[field] = sb;
                            sb.empty();
                            inQuote = false;
                        } else {
                            return; // how did we get here?
                        }
                    } else if (!inQuote) {
                        /* we are at the start of a new quote block */
                        quote = format[i];
                        inQuote = true;
                    } else {
                        /* we were in a quote and saw the other type of quote character, so we are still in a quote */
                    }
                    break;
                case '%':
                    sb.append(format[i]);
                    break;
                case '\\':
                    if (!inQuote) {
                        i++; /* skip next character that is escaped by this backslash or percent sign */
                        break;
                    }
                    sb.append(format[i]);
                    break;
                case 'd':
                    if (!inQuote) {
                        assert((field == 0 && sb.length() == 0) || field == 1);
                        field = 1; // DayHourSep
                        dd++;
                    }
                    break;
                case 'h':
                    if (!inQuote) {
                        assert((field == 1 && sb.length() == 0) || field == 2);
                        field = 2; // HourMinuteSep
                        hh++;
                    }
                    break;
                case 'm':
                    if (!inQuote) {
                        assert((field == 2 && sb.length() == 0) || field == 3);
                        field = 3; // MinuteSecondSep
                        mm++;
                    }
                    break;
                case 's':
                    if (!inQuote) {
                        assert((field == 3 && sb.length() == 0) || field == 4);
                        field = 4; // SecondFractionSep
                        ss++;
                    }
                    break;
                case 'f':
                case 'F':
                    if (!inQuote) {
                        assert((field == 4 && sb.length() == 0) || field == 5);
                        field = 5; // End
                        ff++;
                    }
                    break;
                default:
                    sb.append(format[i]);
                    break;
            }
        }

        assert(field == 5);
        AppCompatLiteral = MinuteSecondSep() + SecondFractionSep();

        assert(0 < dd && dd < 3);
        assert(0 < hh && hh < 3);
        assert(0 < mm && mm < 3);
        assert(0 < ss && ss < 3);
        assert(0 < ff && ff < 8);

        if (useInvariantFieldLengths) {
            dd = 2;
            hh = 2;
            mm = 2;
            ss = 2;
            ff = MaxSecondsFractionDigits;
        } else {
            if (dd < 1 || dd > 2)
                dd = 2;   // The DTFI property has a problem. let's try to make the best of the situation.
            if (hh < 1 || hh > 2) hh = 2;
            if (mm < 1 || mm > 2) mm = 2;
            if (ss < 1 || ss > 2) ss = 2;
            if (ff < 1 || ff > 7) ff = 7;
        }
    }

    String
    TimeSpanFormat::formatStandard(const TimeSpan &value, bool isInvariant, const String &format, Pattern pattern) {
        String sb;
        int day = (int) (value._ticks / TimeSpan::TicksPerDay);
        int64_t time = value._ticks % TimeSpan::TicksPerDay;

        if (value._ticks < 0) {
            day = -day;
            time = -time;
        }
        int hours = (int) (time / TimeSpan::TicksPerHour % 24);
        int minutes = (int) (time / TimeSpan::TicksPerMinute % 60);
        int seconds = (int) (time / TimeSpan::TicksPerSecond % 60);
        int fraction = (int) (time % TimeSpan::TicksPerSecond);

        FormatLiterals literal;
        if (isInvariant) {
            if (value._ticks < 0) {
                static FormatLiterals negativeInvariantFormatLiterals = FormatLiterals::InitInvariant(true);
                literal = negativeInvariantFormatLiterals;
            } else {
                static FormatLiterals positiveInvariantFormatLiterals = FormatLiterals::InitInvariant(false);
                literal = positiveInvariantFormatLiterals;
            }
        } else {
            literal.Init(format, pattern == Pattern::Full);
        }
        if (fraction != 0) { // truncate the partial second to the specified length
            fraction = (int) ((int64_t) fraction /
                              (int64_t) Math::pow(10.0f, FormatLiterals::MaxSecondsFractionDigits - literal.ff));
        }

        // Pattern.Full: [-]dd.hh:mm:ss.fffffff
        // Pattern.Minimum: [-][d.]hh:mm:ss[.fffffff]

        sb.append(literal.Start());                             // [-]
        if (pattern == Pattern::Full || day != 0) {                 //
            sb.append(Int32(day).toString());               // [dd]
            sb.append(literal.DayHourSep());                    // [.]
        }
        sb.append(intToString(hours, literal.hh));      // hh
        sb.append(literal.HourMinuteSep());                     // :
        sb.append(intToString(minutes, literal.mm));    // mm
        sb.append(literal.MinuteSecondSep());                   // :
        sb.append(intToString(seconds, literal.ss));    // ss
        if (!isInvariant && pattern == Pattern::Minimum) {
            int effectiveDigits = literal.ff;
            while (effectiveDigits > 0) {
                if (fraction % 10 == 0) {
                    fraction = fraction / 10;
                    effectiveDigits--;
                } else {
                    break;
                }
            }
            if (effectiveDigits > 0) {
                sb.append(literal.SecondFractionSep());           // [.FFFFFFF]
                sb.append((Int32(fraction)).toString(FormatLiterals::FixedNumberFormats[effectiveDigits - 1]));
            }
        } else if (pattern == Pattern::Full || fraction != 0) {
            sb.append(literal.SecondFractionSep());           // [.]
            sb.append(intToString(fraction, literal.ff));   // [fffffff]
        }                                                   //
        sb.append(literal.End());                             //

        return sb;
    }

    String
    TimeSpanFormat::formatCustomized(const TimeSpan &value, const String &format, const DateTimeFormatInfo *dtfi) {
        int day = (int) (value._ticks / TimeSpan::TicksPerDay);
        int64_t time = value._ticks % TimeSpan::TicksPerDay;

        if (value._ticks < 0) {
            day = -day;
            time = -time;
        }
        int hours = (int) (time / TimeSpan::TicksPerHour % 24);
        int minutes = (int) (time / TimeSpan::TicksPerMinute % 60);
        int seconds = (int) (time / TimeSpan::TicksPerSecond % 60);
        int fraction = (int) (time % TimeSpan::TicksPerSecond);

        int64_t tmp = 0;
        int i = 0;
        int tokenLen;
        String result;

        while (i < format.length()) {
            char ch = format[i];
            int nextChar;
            switch (ch) {
                case 'h':
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > 2)
                        return String::Empty;
                    formatDigits(result, hours, tokenLen);
                    break;
                case 'm':
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > 2)
                        return String::Empty;
                    formatDigits(result, minutes, tokenLen);
                    break;
                case 's':
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > 2)
                        return String::Empty;
                    formatDigits(result, seconds, tokenLen);
                    break;
                case 'f':
                    //
                    // The fraction of a second in single-digit precision. The remaining digits are truncated.
                    //
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > FormatLiterals::MaxSecondsFractionDigits)
                        return String::Empty;

                    tmp = (int64_t) fraction;
                    tmp /= (int64_t) Math::pow(10.0f, FormatLiterals::MaxSecondsFractionDigits - tokenLen);
                    result.append((Int64(tmp)).toString(FormatLiterals::FixedNumberFormats[tokenLen - 1]));
                    break;
                case 'F': {
                    //
                    // Displays the most significant digit of the seconds fraction. Nothing is displayed if the digit is zero.
                    //
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > FormatLiterals::MaxSecondsFractionDigits)
                        return String::Empty;

                    tmp = (int64_t) fraction;
                    tmp /= (int64_t) Math::pow(10.0f, FormatLiterals::MaxSecondsFractionDigits - tokenLen);
                    int effectiveDigits = tokenLen;
                    while (effectiveDigits > 0) {
                        if (tmp % 10 == 0) {
                            tmp = tmp / 10;
                            effectiveDigits--;
                        } else {
                            break;
                        }
                    }
                    if (effectiveDigits > 0) {
                        result.append((Int64(tmp)).toString(FormatLiterals::FixedNumberFormats[effectiveDigits - 1]));
                    }
                }
                    break;
                case 'd':
                    //
                    // tokenLen == 1 : Day as digits with no leading zero.
                    // tokenLen == 2+: Day as digits with leading zero for single-digit days.
                    //
                    tokenLen = parseRepeatPattern(format, i, ch);
                    if (tokenLen > 8)
                        return String::Empty;
                    formatDigits(result, day, tokenLen, true);
                    break;
                case '\'':
                case '\"': {
                    String enquotedString;
                    tokenLen = parseQuoteString(format, i, enquotedString);
                    result.append(enquotedString);
                }
                    break;
                case '%': {
                    // Optional format character.
                    // For example, format string "%d" will print day
                    // Most of the cases, "%" can be ignored.
                    nextChar = parseNextChar(format, i);
                    // nextChar will be -1 if we already reach the end of the format string.
                    // Besides, we will not allow "%%" appear in the pattern.
                    if (nextChar >= 0 && nextChar != (int) '%') {
                        result.append(formatCustomized(value, (Char(nextChar)).toString(), dtfi));
                        tokenLen = 2;
                    } else {
                        //
                        // This means that '%' is at the end of the format string or
                        // "%%" appears in the format string.
                        //
                        return String::Empty;
                    }
                }
                    break;
                case '\\':
                    // Escaped character.  Can be used to insert character into the format string.
                    // For example, "\d" will insert the character 'd' into the string.
                    //
                    nextChar = parseNextChar(format, i);
                    if (nextChar >= 0) {
                        result.append(((char) nextChar));
                        tokenLen = 2;
                    } else {
                        //
                        // This means that '\' is at the end of the formatting string.
                        //
                        return String::Empty;
                    }
                    break;
                default:
                    return String::Empty;
            }
            i += tokenLen;
        }
        return result;
    }

    String TimeSpanFormat::intToString(int64_t n, int digits) {
        char result[64];
        sprintf(result, "%0*"
        PRId64, digits, n);
        return result;
    }

    int TimeSpanFormat::parseRepeatPattern(const String &format, int pos, char patternChar) {
        int len = format.length();
        int index = pos + 1;
        while ((index < len) && (format[index] == patternChar)) {
            index++;
        }
        return (index - pos);
    }

    void TimeSpanFormat::formatDigits(String &outputBuffer, int value, int len) {
        assert(value >= 0);
        formatDigits(outputBuffer, value, len, false);
    }

    void TimeSpanFormat::formatDigits(String &outputBuffer, int value, int len, bool overrideLengthLimit) {
        assert(value >= 0);

        // Limit the use of this function to be two-digits, so that we have the same behavior
        // as RTM bits.
        if (!overrideLengthLimit && len > 2) {
            len = 2;
        }

        char buffer[16];
        char *p = buffer + 16;
        int n = value;
        do {
            *--p = (char) (n % 10 + '0');
            n /= 10;
        } while ((n != 0) && (p > buffer));

        int digits = (int) (buffer + 16 - p);

        //If the repeat count is greater than 0, we're trying
        //to emulate the "00" format, so we have to prepend
        //a zero if the string only has one character.
        while ((digits < len) && (p > buffer)) {
            *--p = '0';
            digits++;
        }
        outputBuffer.append(String(p, digits));
    }

    int TimeSpanFormat::parseQuoteString(const String &format, int pos, String &result) {
        //
        // NOTE : pos will be the index of the quote character in the 'format' string.
        //
        int formatLen = format.length();
        int beginPos = pos;
        char quoteChar = format[pos++]; // Get the character used to quote the following string.

        bool foundQuote = false;
        while (pos < formatLen) {
            char ch = format[pos++];
            if (ch == quoteChar) {
                foundQuote = true;
                break;
            } else if (ch == '\\') {
                // The following are used to support escaped character.
                // Escaped character is also supported in the quoted string.
                // Therefore, someone can use a format like "'minute:' mm\"" to display:
                //  minute: 45"
                // because the second double quote is escaped.
                if (pos < formatLen) {
                    result.append(format[pos++]);
                } else {
                    //
                    // This means that '\' is at the end of the formatting string.
                    //
                    return -1;
                }
            } else {
                result.append(ch);
            }
        }

        if (!foundQuote) {
            // Here we can't find the matching quote.
            return -1;
        }

        //
        // Return the character count including the begin/end quote characters and enclosed string.
        //
        return (pos - beginPos);
    }

    //
    // Get the next character at the index of 'pos' in the 'format' string.
    // Return value of -1 means 'pos' is already at the end of the 'format' string.
    // Otherwise, return value is the int value of the next character.
    //
    int TimeSpanFormat::parseNextChar(const String &format, int pos) {
        if (pos >= format.length() - 1) {
            return (-1);
        }
        return ((int) format[pos + 1]);
    }

    const int64_t TimeSpanParse::MaxMilliSeconds = TimeSpan::MaxMilliSeconds;
    const int64_t TimeSpanParse::MinMilliSeconds = TimeSpan::MinMilliSeconds;
    const TimeSpanParse::TimeSpanToken TimeSpanParse::zero(0);

    bool TimeSpanParse::ValidateStyles(TimeSpanStyles style, const String &parameterName) {
        if (style != TimeSpanStyles::TimeStyleNone && style != TimeSpanStyles::AssumeNegative)
            return false;
        return true;
    }

    TimeSpanParse::TimeSpanToken::TimeSpanToken(int number) {
        ttt = TTT::Num;
        num = number;
        zeroes = 0;
    }

    TimeSpanParse::TimeSpanToken::TimeSpanToken(int leadingZeroes, int number) {
        ttt = TTT::Num;
        num = number;
        zeroes = leadingZeroes;
    }

    bool TimeSpanParse::TimeSpanToken::IsInvalidNumber(int maxValue, int maxPrecision) const {
        assert(ttt == TTT::Num);
        assert(num > -1);
        assert(maxValue > 0);
        assert(maxPrecision == maxFractionDigits || maxPrecision == unlimitedDigits);

        if (num > maxValue)
            return true;
        if (maxPrecision == unlimitedDigits)
            return false; // all validation past this point applies only to fields with precision limits
        if (zeroes > maxPrecision)
            return true;
        if (num == 0 || zeroes == 0)
            return false;

        // num > 0 && zeroes > 0 && num <= maxValue && zeroes <= maxPrecision
        return (num >= (maxValue / (int64_t) Math::pow(10.0f, zeroes - 1)));
    }

    void TimeSpanParse::TimeSpanTokenizer::Init(const String &input) {
        Init(input, 0);
    }

    void TimeSpanParse::TimeSpanTokenizer::Init(const String &input, int startPosition) {
        m_pos = startPosition;
        m_value = input;
    }

    // used by the parsing routines that operate on standard-formats
    TimeSpanParse::TimeSpanToken TimeSpanParse::TimeSpanTokenizer::GetNextToken() {
        assert(m_pos > -1);

        TimeSpanToken tok;
        char ch = CurrentChar();

        if (ch == (char) 0) {
            tok.ttt = TTT::End;
            return tok;
        }

        if (ch >= '0' && ch <= '9') {
            tok.ttt = TTT::Num;
            tok.num = 0;
            tok.zeroes = 0;
            do {
                if ((tok.num & 0xF0000000) != 0) {
                    tok.ttt = TTT::NumOverflow;
                    return tok;
                }
                tok.num = tok.num * 10 + ch - '0';
                if (tok.num == 0) tok.zeroes++;
                if (tok.num < 0) {
                    tok.ttt = TTT::NumOverflow;
                    return tok;
                }
                ch = NextChar();
            } while (ch >= '0' && ch <= '9');
            return tok;
        } else {
            tok.ttt = TTT::Sep;
            int startIndex = m_pos;
            int length = 0;

            while (ch != (char) 0 && (ch < '0' || '9' < ch)) {
                ch = NextChar();
                length++;
            }
            tok.sep = m_value.substr(startIndex, length);
            return tok;
        }
    }

    bool TimeSpanParse::TimeSpanTokenizer::EOL() const {
        return m_pos >= (m_value.length() - 1);
    }

    // BackOne, NextChar, CurrentChar - used by ParseExact (ParseByFormat) to operate
    // on custom-formats where exact character-by-character control is allowed
    void TimeSpanParse::TimeSpanTokenizer::BackOne() {
        if (m_pos > 0) --m_pos;
    }

    char TimeSpanParse::TimeSpanTokenizer::NextChar() {
        m_pos++;
        return CurrentChar();
    }

    char TimeSpanParse::TimeSpanTokenizer::CurrentChar() const {
        if (m_pos > -1 && m_pos < m_value.length()) {
            return m_value[m_pos];
        } else {
            return (char) 0;
        }
    }

    void TimeSpanParse::TimeSpanRawInfo::Init(const DateTimeFormatInfo *dtfi) {
        assert(dtfi != nullptr);

        lastSeenTTT = TTT::TTTNone;
        tokenCount = 0;
        SepCount = 0;
        NumCount = 0;

        m_fullPosPattern = dtfi->fullTimeSpanPositivePattern();
        m_fullNegPattern = dtfi->fullTimeSpanNegativePattern();
        m_posLocInit = false;
        m_negLocInit = false;
    }

    bool TimeSpanParse::TimeSpanRawInfo::ProcessToken(TimeSpanToken &tok, TimeSpanResult &result) {
        if (tok.ttt == TTT::NumOverflow) {
            result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
            return false;
        }
        if (tok.ttt != TTT::Sep && tok.ttt != TTT::Num) {
            // Some unknown token or a repeat token type in the input
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        switch (tok.ttt) {
            case TTT::Sep:
                if (!AddSep(tok.sep, result)) return false;
                break;
            case TTT::Num:
                if (tokenCount == 0) {
                    if (!AddSep(String::Empty, result)) return false;
                }
                if (!AddNum(tok, result)) return false;
                break;
            default:
                break;
        }

        lastSeenTTT = tok.ttt;
        assert(tokenCount == (SepCount + NumCount));
        return true;
    }

    bool TimeSpanParse::TimeSpanRawInfo::AddSep(const String &sep, TimeSpanResult &result) {
        if (SepCount >= MaxLiteralTokens || tokenCount >= MaxTokens) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        literals[SepCount++] = sep;
        tokenCount++;
        return true;
    }

    bool TimeSpanParse::TimeSpanRawInfo::AddNum(const TimeSpanToken &num, TimeSpanResult &result) {
        if (NumCount >= MaxNumericTokens || tokenCount >= MaxTokens) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        numbers[NumCount++] = num;
        tokenCount++;
        return true;
    }

    FormatLiterals TimeSpanParse::TimeSpanRawInfo::PositiveInvariant() {
        static FormatLiterals positiveInvariantFormatLiterals = FormatLiterals::InitInvariant(false);
        return positiveInvariantFormatLiterals;
    }


    FormatLiterals TimeSpanParse::TimeSpanRawInfo::NegativeInvariant() {
        static FormatLiterals negativeInvariantFormatLiterals = FormatLiterals::InitInvariant(true);
        return negativeInvariantFormatLiterals;
    }


    FormatLiterals TimeSpanParse::TimeSpanRawInfo::PositiveLocalized() {
        if (!m_posLocInit) {
            m_posLoc.Init(m_fullPosPattern, false);
            m_posLocInit = true;
        }
        return m_posLoc;
    }


    FormatLiterals TimeSpanParse::TimeSpanRawInfo::NegativeLocalized() {
        if (!m_negLocInit) {
            m_negLoc.Init(m_fullNegPattern, false);
            m_negLocInit = true;
        }
        return m_negLoc;
    }

    //<
    bool TimeSpanParse::TimeSpanRawInfo::FullAppCompatMatch(const FormatLiterals &pattern) {
        return SepCount == 5
               && NumCount == 4
               && pattern.Start() == literals[0]
               && pattern.DayHourSep() == literals[1]
               && pattern.HourMinuteSep() == literals[2]
               && pattern.AppCompatLiteral == literals[3]
               && pattern.End() == literals[4];
    }

    //<
    bool TimeSpanParse::TimeSpanRawInfo::PartialAppCompatMatch(const FormatLiterals &pattern) {
        return SepCount == 4
               && NumCount == 3
               && pattern.Start() == literals[0]
               && pattern.HourMinuteSep() == literals[1]
               && pattern.AppCompatLiteral == literals[2]
               && pattern.End() == literals[3];
    }

    // DHMSF (all values matched)
    bool TimeSpanParse::TimeSpanRawInfo::FullMatch(const FormatLiterals &pattern) {
        return SepCount == MaxLiteralTokens
               && NumCount == MaxNumericTokens
               && pattern.Start() == literals[0]
               && pattern.DayHourSep() == literals[1]
               && pattern.HourMinuteSep() == literals[2]
               && pattern.MinuteSecondSep() == literals[3]
               && pattern.SecondFractionSep() == literals[4]
               && pattern.End() == literals[5];
    }

    // D (no hours, minutes, seconds, or fractions)
    bool TimeSpanParse::TimeSpanRawInfo::FullDMatch(const FormatLiterals &pattern) {
        return SepCount == 2
               && NumCount == 1
               && pattern.Start() == literals[0]
               && pattern.End() == literals[1];
    }

    // HM (no days, seconds, or fractions)
    bool TimeSpanParse::TimeSpanRawInfo::FullHMMatch(const FormatLiterals &pattern) {
        return SepCount == 3
               && NumCount == 2
               && pattern.Start() == literals[0]
               && pattern.HourMinuteSep() == literals[1]
               && pattern.End() == literals[2];
    }

    // DHM (no seconds or fraction)
    bool TimeSpanParse::TimeSpanRawInfo::FullDHMMatch(const FormatLiterals &pattern) {
        return SepCount == 4
               && NumCount == 3
               && pattern.Start() == literals[0]
               && pattern.DayHourSep() == literals[1]
               && pattern.HourMinuteSep() == literals[2]
               && pattern.End() == literals[3];

    }

    // HMS (no days or fraction)
    bool TimeSpanParse::TimeSpanRawInfo::FullHMSMatch(const FormatLiterals &pattern) {
        return SepCount == 4
               && NumCount == 3
               && pattern.Start() == literals[0]
               && pattern.HourMinuteSep() == literals[1]
               && pattern.MinuteSecondSep() == literals[2]
               && pattern.End() == literals[3];
    }

    // DHMS (no fraction)
    bool TimeSpanParse::TimeSpanRawInfo::FullDHMSMatch(const FormatLiterals &pattern) {
        return SepCount == 5
               && NumCount == 4
               && pattern.Start() == literals[0]
               && pattern.DayHourSep() == literals[1]
               && pattern.HourMinuteSep() == literals[2]
               && pattern.MinuteSecondSep() == literals[3]
               && pattern.End() == literals[4];
    }

    // HMSF (no days)
    bool TimeSpanParse::TimeSpanRawInfo::FullHMSFMatch(const FormatLiterals &pattern) {
        return SepCount == 5
               && NumCount == 4
               && pattern.Start() == literals[0]
               && pattern.HourMinuteSep() == literals[1]
               && pattern.MinuteSecondSep() == literals[2]
               && pattern.SecondFractionSep() == literals[3]
               && pattern.End() == literals[4];
    }

    void TimeSpanParse::TimeSpanResult::Init(TimeSpanThrowStyle canThrow) {
        parsedTimeSpan = TimeSpan();
        throwStyle = canThrow;
    }

    void TimeSpanParse::TimeSpanResult::SetFailure(ParseFailureKind failure, const String &failureMessageID) {
        SetFailure(failure, failureMessageID, String::Empty);
    }

    void TimeSpanParse::TimeSpanResult::SetFailure(ParseFailureKind failure, const String &failureMessageID,
                                                   const String &failureArgumentName) {
        m_failure = failure;
        m_failureMessageID = failureMessageID;
        m_failureArgumentName = failureArgumentName;
    }

    String TimeSpanParse::TimeSpanResult::GetExceptionString() {
        switch (m_failure) {
            case ParseFailureKind::ArgumentNull:
                return String::format("%s %s", m_failureArgumentName.c_str(), m_failureMessageID.c_str());
            case ParseFailureKind::FormatWithParameter:
                return String::format("%s %s", m_failureArgumentName.c_str(), m_failureMessageID.c_str());
            case ParseFailureKind::Format:
                return m_failureMessageID;
            case ParseFailureKind::Overflow:
                return m_failureMessageID;
            default:
                return "Format_InvalidString";
        }
    }

    void TimeSpanParse::StringParser::NextChar() {
        if (pos < len) pos++;
        ch = pos < len ? str[pos] : (char) 0;
    }

    char TimeSpanParse::StringParser::NextNonDigit() {
        int i = pos;
        while (i < len) {
            char ch = str[i];
            if (ch < '0' || ch > '9') return ch;
            i++;
        }
        return (char) 0;
    }

    bool TimeSpanParse::StringParser::TryParse(const String &input, TimeSpanResult &result) {
        result.parsedTimeSpan = TimeSpan::Zero;

        if (input.isNullOrEmpty()) {
            return false;
        }
        str = input;
        len = input.length();
        pos = -1;
        NextChar();
        SkipBlanks();
        bool negative = false;
        if (ch == '-') {
            negative = true;
            NextChar();
        }
        int64_t time;
        if (NextNonDigit() == ':') {
            if (!ParseTime(time, result)) {
                return false;
            };
        } else {
            int days;
            if (!ParseInt((int) (0x7FFFFFFFFFFFFFFFL / TimeSpan::TicksPerDay), days, result)) {
                return false;
            }
            time = days * TimeSpan::TicksPerDay;
            if (ch == '.') {
                NextChar();
                int64_t remainingTime;
                if (!ParseTime(remainingTime, result)) {
                    return false;
                };
                time += remainingTime;
            }
        }
        if (negative) {
            time = -time;
            // Allow -0 as well
            if (time > 0) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
        } else {
            if (time < 0) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
        }
        SkipBlanks();
        if (pos < len) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        result.parsedTimeSpan._ticks = time;
        return true;
    }

    bool TimeSpanParse::StringParser::ParseInt(int max, int &i, TimeSpanResult &result) {
        i = 0;
        int p = pos;
        while (ch >= '0' && ch <= '9') {
            if ((i & 0xF0000000) != 0) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
            i = i * 10 + ch - '0';
            if (i < 0) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
            NextChar();
        }
        if (p == pos) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        if (i > max) {
            result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
            return false;
        }
        return true;
    }

    bool TimeSpanParse::StringParser::ParseTime(int64_t &time, TimeSpanResult &result) {
        time = 0;
        int unit;
        if (!ParseInt(23, unit, result)) {
            return false;
        }
        time = unit * TimeSpan::TicksPerHour;
        if (ch != ':') {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        NextChar();
        if (!ParseInt(59, unit, result)) {
            return false;
        }
        time += unit * TimeSpan::TicksPerMinute;
        if (ch == ':') {
            NextChar();
            // allow seconds with the leading zero
            if (ch != '.') {
                if (!ParseInt(59, unit, result)) {
                    return false;
                }
                time += unit * TimeSpan::TicksPerSecond;
            }
            if (ch == '.') {
                NextChar();
                int f = (int) TimeSpan::TicksPerSecond;
                while (f > 1 && ch >= '0' && ch <= '9') {
                    f /= 10;
                    time += (ch - '0') * f;
                    NextChar();
                }
            }
        }
        return true;
    }

    void TimeSpanParse::StringParser::SkipBlanks() {
        while (ch == ' ' || ch == '\t') NextChar();
    }

    bool TimeSpanParse::TryTimeToTicks(bool positive, const TimeSpanToken &days, const TimeSpanToken &hours,
                                       const TimeSpanToken &minutes,
                                       const TimeSpanToken &seconds, const TimeSpanToken &fraction, int64_t &result) {
        if (days.IsInvalidNumber(maxDays, unlimitedDigits)
            || hours.IsInvalidNumber(maxHours, unlimitedDigits)
            || minutes.IsInvalidNumber(maxMinutes, unlimitedDigits)
            || seconds.IsInvalidNumber(maxSeconds, unlimitedDigits)
            || fraction.IsInvalidNumber(maxFraction, maxFractionDigits)) {
            result = 0;
            return false;
        }

        Int64 ticks =
                ((Int64) days.num * 3600 * 24 + (Int64) hours.num * 3600 + (Int64) minutes.num * 60 + seconds.num) *
                1000;
        if (ticks > MaxMilliSeconds || ticks < MinMilliSeconds) {
            result = 0;
            return false;
        }

        // Normalize the fraction component
        //
        // string representation => (zeroes,num) => resultant fraction ticks
        // ---------------------    ------------    ------------------------
        // ".9999999"            => (0,9999999)  => 9,999,999 ticks (same as constant maxFraction)
        // ".1"                  => (0,1)        => 1,000,000 ticks
        // ".01"                 => (1,1)        =>   100,000 ticks
        // ".001"                => (2,1)        =>    10,000 ticks
        int64_t f = fraction.num;
        if (f != 0) {
            int64_t lowerLimit = TimeSpan::TicksPerTenthSecond;
            if (fraction.zeroes > 0) {
                int64_t divisor = (int64_t) Math::pow(10.0f, fraction.zeroes);
                lowerLimit = lowerLimit / divisor;
            }
            while (f < lowerLimit) {
                f *= 10;
            }
        }
        result = ((int64_t) ticks * TimeSpan::TicksPerMillisecond) + f;
        if (positive && result < 0) {
            result = 0;
            return false;
        }
        return true;
    }


    // ---- SECTION:  static methods called by System.TimeSpan ---------*
    //
    //  [Try]Parse, [Try]ParseExact, and [Try]ParseExactMultiple
    //
    //  Actions: Main methods called from TimeSpan::Parse
    TimeSpan TimeSpanParse::Parse(const String &input, const IFormatProvider<DateTimeFormatInfo> *formatProvider) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleAll);

        if (TryParseTimeSpan(input, TimeSpanStandardStyles::StandardStyleNone, formatProvider, parseResult)) {
            return parseResult.parsedTimeSpan;
        } else {
            return TimeSpan::Zero;
        }
    }

    bool
    TimeSpanParse::TryParse(const String &input, const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                            TimeSpan &result) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleNone);

        if (TryParseTimeSpan(input, TimeSpanStandardStyles::StandardStyleAny, formatProvider, parseResult)) {
            result = parseResult.parsedTimeSpan;
            return true;
        } else {
            result = TimeSpan();
            return false;
        }
    }

    TimeSpan
    TimeSpanParse::ParseExact(const String &input, const String &format,
                              const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                              TimeSpanStyles styles) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleAll);

        if (TryParseExactTimeSpan(input, format, formatProvider, styles, parseResult)) {
            return parseResult.parsedTimeSpan;
        } else {
            return TimeSpan::Zero;
        }
    }

    bool
    TimeSpanParse::TryParseExact(const String &input, const String &format,
                                 const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                 TimeSpanStyles styles, TimeSpan &result) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleNone);

        if (TryParseExactTimeSpan(input, format, formatProvider, styles, parseResult)) {
            result = parseResult.parsedTimeSpan;
            return true;
        } else {
            result = TimeSpan();
            return false;
        }
    }

    TimeSpan TimeSpanParse::ParseExactMultiple(const String &input, const StringArray &formats,
                                               const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                               TimeSpanStyles styles) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleAll);

        if (TryParseExactMultipleTimeSpan(input, formats, formatProvider, styles, parseResult)) {
            return parseResult.parsedTimeSpan;
        } else {
            return TimeSpan::Zero;
        }
    }

    bool TimeSpanParse::TryParseExactMultiple(const String &input, const StringArray &formats,
                                              const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                              TimeSpanStyles styles, TimeSpan &result) {
        TimeSpanResult parseResult;
        parseResult.Init(TimeSpanThrowStyle::ThrowStyleNone);

        if (TryParseExactMultipleTimeSpan(input, formats, formatProvider, styles, parseResult)) {
            result = parseResult.parsedTimeSpan;
            return true;
        } else {
            result = TimeSpan();
            return false;
        }
    }

    // ---- SECTION:  static methods that do the actual work ---------*
    //
    //  TryParseTimeSpan
    //
    //  Actions: Common Parse method called by both Parse and TryParse
    //
    bool TimeSpanParse::TryParseTimeSpan(const String &input, TimeSpanStandardStyles style,
                                         const IFormatProvider<DateTimeFormatInfo> *formatProvider, TimeSpanResult &
    result) {
        if (input.isNullOrEmpty()) {
            return false;
        }

        String inputStr = input.trim();
        if (inputStr == String::Empty) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        TimeSpanTokenizer tokenizer;
        tokenizer.Init(inputStr);

        TimeSpanRawInfo raw;
        raw.Init(DateTimeFormatInfo::getInstance(formatProvider));

        TimeSpanToken tok = tokenizer.GetNextToken();

        /* The following loop will break out when we reach the end of the str or
         * when we can determine that the input is invalid. */
        while (tok.ttt != TTT::End) {
            if (!raw.ProcessToken(tok, result)) {
                result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
                return false;
            }
            tok = tokenizer.GetNextToken();
        }
        if (!tokenizer.EOL()) {
            // embedded nulls in the input string
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        if (!ProcessTerminalState(raw, style, result)) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
        return true;
    }

    //
    //  ProcessTerminalState
    //
    //  Actions: Validate the terminal state of a standard format parse.
    //           Sets result.parsedTimeSpan on success.
    //
    // Calculates the resultant TimeSpan from the TimeSpanRawInfo
    //
    // try => +InvariantPattern, -InvariantPattern, +LocalizedPattern, -LocalizedPattern
    // 1) Verify Start matches
    // 2) Verify End matches
    // 3) 1 number  => d
    //    2 numbers => h:m
    //    3 numbers => h:m:s | d.h:m | h:m:.f
    //    4 numbers => h:m:s.f | d.h:m:s | d.h:m:.f
    //    5 numbers => d.h:m:s.f
    bool
    TimeSpanParse::ProcessTerminalState(TimeSpanRawInfo &raw, TimeSpanStandardStyles style,
                                        TimeSpanResult &result) {
        if (raw.lastSeenTTT == TTT::Num) {
            TimeSpanToken tok;
            tok.ttt = TTT::Sep;
            tok.sep = String::Empty;
            if (!raw.ProcessToken(tok, result)) {
                result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
                return false;
            }
        }

        switch (raw.NumCount) {
            case 1:
                return ProcessTerminal_D(raw, style, result);
            case 2:
                return ProcessTerminal_HM(raw, style, result);
            case 3:
                return ProcessTerminal_HM_S_D(raw, style, result);
            case 4:
                return ProcessTerminal_HMS_F_D(raw, style, result);
            case 5:
                return ProcessTerminal_DHMSF(raw, style, result);
            default:
                result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
                return false;
        }
    }

    //
    //  ProcessTerminal_DHMSF
    //
    //  Actions: Validate the 5-number "Days.Hours:Minutes:Seconds.Fraction" terminal case.
    //           Sets result.parsedTimeSpan on success.
    //
    bool
    TimeSpanParse::ProcessTerminal_DHMSF(TimeSpanRawInfo &raw, TimeSpanStandardStyles style,
                                         TimeSpanResult &result) {
        if (raw.SepCount != 6 || raw.NumCount != 5) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        bool inv = ((style & TimeSpanStandardStyles::Invariant) != 0);
        bool loc = ((style & TimeSpanStandardStyles::Localized) != 0);

        bool positive = false;
        bool match = false;

        if (inv) {
            if (raw.FullMatch(raw.PositiveInvariant())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullMatch(raw.NegativeInvariant())) {
                match = true;
                positive = false;
            }
        }
        if (loc) {
            if (!match && raw.FullMatch(raw.PositiveLocalized())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullMatch(raw.NegativeLocalized())) {
                match = true;
                positive = false;
            }
        }
        int64_t ticks;
        if (match) {
            if (!TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], raw.numbers[3],
                                raw.numbers[4], ticks)) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
            if (!positive) {
                ticks = -ticks;
                if (ticks > 0) {
                    result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                    return false;
                }
            }
            result.parsedTimeSpan._ticks = ticks;
            return true;
        }

        result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
        return false;
    }

    //
    //  ProcessTerminal_HMS_F_D
    //
    //  Actions: Validate the ambiguous 4-number "Hours:Minutes:Seconds.Fraction", "Days.Hours:Minutes:Seconds", or "Days.Hours:Minutes:.Fraction" terminal case.
    //           Sets result.parsedTimeSpan on success.
    //
    bool
    TimeSpanParse::ProcessTerminal_HMS_F_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style,
                                           TimeSpanResult &result) {
        if (raw.SepCount != 5 || raw.NumCount != 4 || (style & TimeSpanStandardStyles::RequireFull) != 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        bool inv = ((style & TimeSpanStandardStyles::Invariant) != 0);
        bool loc = ((style & TimeSpanStandardStyles::Localized) != 0);

        int64_t ticks = 0;
        bool positive = false;
        bool match = false;
        bool overflow = false;

        if (inv) {
            if (raw.FullHMSFMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2],
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMSMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], raw.numbers[3],
                                       zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullAppCompatMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero,
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullHMSFMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2],
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMSMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], raw.numbers[3],
                                       zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullAppCompatMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero,
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
        }
        if (loc) {
            if (!match && raw.FullHMSFMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2],
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMSMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], raw.numbers[3],
                                       zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullAppCompatMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero,
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullHMSFMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2],
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMSMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], raw.numbers[3],
                                       zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullAppCompatMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero,
                                       raw.numbers[3], ticks);
                overflow = overflow || !match;
            }
        }

        if (match) {
            if (!positive) {
                ticks = -ticks;
                if (ticks > 0) {
                    result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                    return false;
                }
            }
            result.parsedTimeSpan._ticks = ticks;
            return true;
        }

        if (overflow) {
            // we found at least one literal pattern match but the numbers just didn't fit
            result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
            return false;
        } else {
            // we couldn't find a thing
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
    }

    //
    //  ProcessTerminal_HM_S_D
    //
    //  Actions: Validate the ambiguous 3-number "Hours:Minutes:Seconds", "Days.Hours:Minutes", or "Hours:Minutes:.Fraction" terminal case
    //
    bool
    TimeSpanParse::ProcessTerminal_HM_S_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style,
                                          TimeSpanResult &result) {
        if (raw.SepCount != 4 || raw.NumCount != 3 || (style & TimeSpanStandardStyles::RequireFull) != 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        bool inv = ((style & TimeSpanStandardStyles::Invariant) != 0);
        bool loc = ((style & TimeSpanStandardStyles::Localized) != 0);

        bool positive = false;
        bool match = false;
        bool overflow = false;

        int64_t ticks = 0;

        if (inv) {
            if (raw.FullHMSMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.PartialAppCompatMatch(raw.PositiveInvariant())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], zero, raw.numbers[2], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullHMSMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.PartialAppCompatMatch(raw.NegativeInvariant())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], zero, raw.numbers[2], ticks);
                overflow = overflow || !match;
            }
        }
        if (loc) {
            if (!match && raw.FullHMSMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.PartialAppCompatMatch(raw.PositiveLocalized())) {
                positive = true;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], zero, raw.numbers[2], ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullHMSMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.FullDHMMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, raw.numbers[0], raw.numbers[1], raw.numbers[2], zero, zero, ticks);
                overflow = overflow || !match;
            }
            if (!match && raw.PartialAppCompatMatch(raw.NegativeLocalized())) {
                positive = false;
                match = TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], zero, raw.numbers[2], ticks);
                overflow = overflow || !match;
            }
        }

        if (match) {
            if (!positive) {
                ticks = -ticks;
                if (ticks > 0) {
                    result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                    return false;
                }
            }
            result.parsedTimeSpan._ticks = ticks;
            return true;
        }

        if (overflow) {
            // we found at least one literal pattern match but the numbers just didn't fit
            result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
            return false;
        } else {
            // we couldn't find a thing
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }
    }

    //
    //  ProcessTerminal_HM
    //
    //  Actions: Validate the 2-number "Hours:Minutes" terminal case
    //
    bool
    TimeSpanParse::ProcessTerminal_HM(TimeSpanRawInfo &raw, TimeSpanStandardStyles style,
                                      TimeSpanResult &result) {
        if (raw.SepCount != 3 || raw.NumCount != 2 || (style & TimeSpanStandardStyles::RequireFull) != 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        bool inv = ((style & TimeSpanStandardStyles::Invariant) != 0);
        bool loc = ((style & TimeSpanStandardStyles::Localized) != 0);

        bool positive = false;
        bool match = false;

        if (inv) {
            if (raw.FullHMMatch(raw.PositiveInvariant())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullHMMatch(raw.NegativeInvariant())) {
                match = true;
                positive = false;
            }
        }
        if (loc) {
            if (!match && raw.FullHMMatch(raw.PositiveLocalized())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullHMMatch(raw.NegativeLocalized())) {
                match = true;
                positive = false;
            }
        }

        int64_t ticks = 0;
        if (match) {
            if (!TryTimeToTicks(positive, zero, raw.numbers[0], raw.numbers[1], zero, zero, ticks)) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
            if (!positive) {
                ticks = -ticks;
                if (ticks > 0) {
                    result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                    return false;
                }
            }
            result.parsedTimeSpan._ticks = ticks;
            return true;
        }

        result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
        return false;
    }


    //
    //  ProcessTerminal_D
    //
    //  Actions: Validate the 1-number "Days" terminal case
    //
    bool
    TimeSpanParse::ProcessTerminal_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result) {
        if (raw.SepCount != 2 || raw.NumCount != 1 || (style & TimeSpanStandardStyles::RequireFull) != 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        bool inv = ((style & TimeSpanStandardStyles::Invariant) != 0);
        bool loc = ((style & TimeSpanStandardStyles::Localized) != 0);

        bool positive = false;
        bool match = false;

        if (inv) {
            if (raw.FullDMatch(raw.PositiveInvariant())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullDMatch(raw.NegativeInvariant())) {
                match = true;
                positive = false;
            }
        }
        if (loc) {
            if (!match && raw.FullDMatch(raw.PositiveLocalized())) {
                match = true;
                positive = true;
            }
            if (!match && raw.FullDMatch(raw.NegativeLocalized())) {
                match = true;
                positive = false;
            }
        }

        int64_t ticks = 0;
        if (match) {
            if (!TryTimeToTicks(positive, raw.numbers[0], zero, zero, zero, zero, ticks)) {
                result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                return false;
            }
            if (!positive) {
                ticks = -ticks;
                if (ticks > 0) {
                    result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
                    return false;
                }
            }
            result.parsedTimeSpan._ticks = ticks;
            return true;
        }

        result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
        return false;
    }

    //
    //  TryParseExactTimeSpan
    //
    //  Actions: Common ParseExact method called by both ParseExact and TryParseExact
    //
    bool TimeSpanParse::TryParseExactTimeSpan(const String &input, const String &format,
                                              const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                              TimeSpanStyles styles, TimeSpanResult &result) {
        if (input.isNullOrEmpty()) {
            result.SetFailure(ParseFailureKind::ArgumentNull, "ArgumentNull_String", "input");
            return false;
        }
        if (format.isNullOrEmpty()) {
            result.SetFailure(ParseFailureKind::ArgumentNull, "ArgumentNull_String", "format");
            return false;
        }
        if (format.length() == 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadFormatSpecifier");
            return false;
        }

        if (format.length() == 1) {
            TimeSpanStandardStyles style = TimeSpanStandardStyles::StandardStyleNone;

            if (format[0] == 'c' || format[0] == 't' || format[0] == 'T') {
                // fast path for legacy style TimeSpan formats.
                return TryParseTimeSpanConstant(input, result);
            } else if (format[0] == 'g') {
                style = TimeSpanStandardStyles::Localized;
            } else if (format[0] == 'G') {
                style = (TimeSpanStandardStyles) (TimeSpanStandardStyles::Localized |
                                                  TimeSpanStandardStyles::RequireFull);
            } else {
                result.SetFailure(ParseFailureKind::Format, "Format_BadFormatSpecifier");
                return false;
            }
            return TryParseTimeSpan(input, style, formatProvider, result);
        }

        return TryParseByFormat(input, format, styles, result);
    }

    //
    //  TryParseByFormat
    //
    //  Actions: Parse the TimeSpan instance using the specified format.  Used by TryParseExactTimeSpan::
    //
    bool
    TimeSpanParse::TryParseByFormat(const String &input, const String &format, TimeSpanStyles styles,
                                    TimeSpanResult &result) {
        assert(!input.isNullOrEmpty());
        assert(!format.isNullOrEmpty());

        bool seenDD = false;      // already processed days?
        bool seenHH = false;      // already processed hours?
        bool seenMM = false;      // already processed minutes?
        bool seenSS = false;      // already processed seconds?
        bool seenFF = false;      // already processed fraction?
        int dd = 0;               // parsed days
        int hh = 0;               // parsed hours
        int mm = 0;               // parsed minutes
        int ss = 0;               // parsed seconds
        int leadingZeroes = 0;    // number of leading zeroes in the parsed fraction
        int ff = 0;               // parsed fraction
        int i = 0;                // format string position
        int tokenLen = 0;         // length of current format token, used to update index 'i'

        TimeSpanTokenizer tokenizer;
        tokenizer.Init(input, -1);

        while (i < format.length()) {
            char ch = format[i];
            int nextFormatChar;
            switch (ch) {
                case 'h':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen > 2 || seenHH || !ParseExactDigits(tokenizer, tokenLen, hh)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    seenHH = true;
                    break;
                case 'm':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen > 2 || seenMM || !ParseExactDigits(tokenizer, tokenLen, mm)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    seenMM = true;
                    break;
                case 's':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen > 2 || seenSS || !ParseExactDigits(tokenizer, tokenLen, ss)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    seenSS = true;
                    break;
                case 'f':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen > FormatLiterals::MaxSecondsFractionDigits || seenFF ||
                        !ParseExactDigits(tokenizer, tokenLen, tokenLen, leadingZeroes, ff)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    seenFF = true;
                    break;
                case 'F':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen > FormatLiterals::MaxSecondsFractionDigits || seenFF) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    ParseExactDigits(tokenizer, tokenLen, tokenLen, leadingZeroes, ff);
                    seenFF = true;
                    break;
                case 'd': {
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    int tmp = 0;
                    if (tokenLen > 8 || seenDD ||
                        !ParseExactDigits(tokenizer, (tokenLen < 2) ? 1 : tokenLen, (tokenLen < 2) ? 8 : tokenLen,
                                          tmp, dd)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    seenDD = true;
                }
                    break;
                case '\'':
                case '\"': {
                    String enquotedString;
                    if (!TryParseQuoteString(format, i, enquotedString, tokenLen)) {
                        result.SetFailure(ParseFailureKind::FormatWithParameter, "Format_BadQuote", ch);
                        return false;
                    }
                    if (!ParseExactLiteral(tokenizer, enquotedString)) {
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                }
                    break;
                case '%':
                    // Optional format character.
                    // For example, format string "%d" will print day
                    // Most of the cases, "%" can be ignored.
                    nextFormatChar = ParseNextChar(format, i);
                    // nextFormatChar will be -1 if we already reach the end of the format string.
                    // Besides, we will not allow "%%" appear in the pattern.
                    if (nextFormatChar >= 0 && nextFormatChar != (int) '%') {
                        tokenLen = 1; // skip the '%' and process the format character
                        break;
                    } else {
                        // This means that '%' is at the end of the format string or
                        // "%%" appears in the format string.
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                case '\\':
                    // Escaped character.  Can be used to insert character into the format string.
                    // For example, "\d" will insert the character 'd' into the string.
                    //
                    nextFormatChar = ParseNextChar(format, i);
                    if (nextFormatChar >= 0 && tokenizer.NextChar() == (char) nextFormatChar) {
                        tokenLen = 2;
                    } else {
                        // This means that '\' is at the end of the format string or the literal match failed.
                        result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                        return false;
                    }
                    break;
                default:
                    result.SetFailure(ParseFailureKind::Format, "Format_InvalidString");
                    return false;
            }
            i += tokenLen;
        }


        if (!tokenizer.EOL()) {
            // the custom format didn't consume the entire input
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        int64_t ticks = 0;
        bool positive = (styles & TimeSpanStyles::AssumeNegative) == 0;
        if (TryTimeToTicks(positive, TimeSpanToken(dd),
                           TimeSpanToken(hh),
                           TimeSpanToken(mm),
                           TimeSpanToken(ss),
                           TimeSpanToken(leadingZeroes, ff),
                           ticks)) {
            if (!positive) ticks = -ticks;
            result.parsedTimeSpan._ticks = ticks;
            return true;
        } else {
            result.SetFailure(ParseFailureKind::Overflow, "Overflow_TimeSpanElementTooLarge");
            return false;

        }
    }

    bool TimeSpanParse::ParseExactDigits(TimeSpanTokenizer &tokenizer, int minDigitLength, int &result) {
        result = 0;
        int zeroes = 0;
        int maxDigitLength = (minDigitLength == 1) ? 2 : minDigitLength;
        return ParseExactDigits(tokenizer, minDigitLength, maxDigitLength, zeroes, result);
    }

    bool TimeSpanParse::ParseExactDigits(TimeSpanTokenizer &tokenizer, int minDigitLength, int maxDigitLength,
                                         int &zeroes,
                                         int &result) {
        result = 0;
        zeroes = 0;

        int tokenLength = 0;
        while (tokenLength < maxDigitLength) {
            char ch = tokenizer.NextChar();
            if (ch < '0' || ch > '9') {
                tokenizer.BackOne();
                break;
            }
            result = result * 10 + (ch - '0');
            if (result == 0) zeroes++;
            tokenLength++;
        }
        return (tokenLength >= minDigitLength);
    }

    bool TimeSpanParse::ParseExactLiteral(TimeSpanTokenizer &tokenizer, const String &enquotedString) {
        for (int i = 0; i < enquotedString.length(); i++) {
            if (enquotedString[i] != tokenizer.NextChar())
                return false;
        }
        return true;
    }

    //
    // TryParseTimeSpanConstant
    //
    // Actions: Parses the "c" (constant) format.  This code is 100% identical to the non-globalized v1.0-v3.5 TimeSpan::Parse() routine
    //          and exists for performance/appcompat with legacy callers who cannot move onto the globalized Parse overloads.
    //
    bool TimeSpanParse::TryParseTimeSpanConstant(const String &input, TimeSpanResult &result) {
        return (StringParser().TryParse(input, result));
    }

    //
    //  TryParseExactMultipleTimeSpan
    //
    //  Actions: Common ParseExactMultiple method called by both ParseExactMultiple and TryParseExactMultiple
    //
    bool TimeSpanParse::TryParseExactMultipleTimeSpan(const String &input, const StringArray &formats,
                                                      const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                                      TimeSpanStyles styles, TimeSpanResult &result) {
        if (input.isNullOrEmpty()) {
            result.SetFailure(ParseFailureKind::ArgumentNull, "ArgumentNull_String", "input");
            return false;
        }

        if (input.length() == 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
            return false;
        }

        if (formats.count() == 0) {
            result.SetFailure(ParseFailureKind::Format, "Format_BadFormatSpecifier");
            return false;
        }

        //
        // Do a loop through the provided formats and see if we can parse succesfully in
        // one of the formats.
        //
        for (int i = 0; i < formats.count(); i++) {
            if (formats[i].length() == 0) {
                result.SetFailure(ParseFailureKind::Format, "Format_BadFormatSpecifier");
                return false;
            }

            // Create a new non-throwing result each time to ensure the runs are independent.
            TimeSpanResult innerResult;
            innerResult.Init(TimeSpanThrowStyle::ThrowStyleNone);

            if (TryParseExactTimeSpan(input, formats[i], formatProvider, styles, innerResult)) {
                result.parsedTimeSpan = innerResult.parsedTimeSpan;
                return true;
            }
        }

        result.SetFailure(ParseFailureKind::Format, "Format_BadTimeSpan");
        return (false);
    }

    int TimeSpanParse::ParseRepeatPattern(const String &format, int pos, char patternChar) {
        int len = format.length();
        int index = pos + 1;
        while ((index < len) && (format[index] == patternChar)) {
            index++;
        }
        return (index - pos);
    }

    bool TimeSpanParse::TryParseQuoteString(const String &format, int pos, String &result, int &returnValue) {
        //
        // NOTE : pos will be the index of the quote character in the 'format' string.
        //
        returnValue = 0;
        int formatLen = format.length();
        int beginPos = pos;
        char quoteChar = format[pos++]; // Get the character used to quote the following string.

        bool foundQuote = false;
        while (pos < formatLen) {
            char ch = format[pos++];
            if (ch == quoteChar) {
                foundQuote = true;
                break;
            } else if (ch == '\\') {
                // The following are used to support escaped character.
                // Escaped character is also supported in the quoted string.
                // Therefore, someone can use a format like "'minute:' mm\"" to display:
                //  minute: 45"
                // because the second double quote is escaped.
                if (pos < formatLen) {
                    result.append(format[pos++]);
                } else {
                    //
                    // This means that '\' is at the end of the formatting string.
                    //
                    return false;
                }
            } else {
                result.append(ch);
            }
        }

        if (!foundQuote) {
            // Here we can't find the matching quote.
            return false;
        }

        //
        // Return the character count including the begin/end quote characters and enclosed string.
        //
        returnValue = (pos - beginPos);
        return true;
    }

    //
    // Get the next character at the index of 'pos' in the 'format' string.
    // Return value of -1 means 'pos' is already at the end of the 'format' string.
    // Otherwise, return value is the int value of the next character.
    //
    int TimeSpanParse::ParseNextChar(const String &format, int pos) {
        if (pos >= format.length() - 1) {
            return (-1);
        }
        return ((int) format[pos + 1]);
    }
}