//
//  TimeSpanFormat.h
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright (c) 2022 com. All rights reserved.
//

#ifndef TimeSpanFormat_h
#define TimeSpanFormat_h

#include "data/String.h"
#include "data/TimeSpan.h"
#include "data/DateTime.h"
#include "data/Culture.h"

namespace Data {
    class FormatLiterals {
    public:
        String AppCompatLiteral;
        int dd;
        int hh;
        int mm;
        int ss;
        int ff;
        String literals[6];

    public:
        FormatLiterals();

        FormatLiterals(const FormatLiterals &fl);

        const String &Start() const;

        String DayHourSep() const;

        String HourMinuteSep() const;

        String MinuteSecondSep() const;

        String SecondFractionSep() const;

        String End() const;

        void Init(const String &format, bool useInvariantFieldLengths);

    public:
        static FormatLiterals InitInvariant(bool isNegative);

    public:
        static const int MaxSecondsFractionDigits = 7;

        static const String FixedNumberFormats[MaxSecondsFractionDigits];
    };

    struct TimeSpanFormat {
    public:
        enum Pattern {
            PatternNone = 0,
            Minimum = 1,
            Full = 2,
        };

        static String formatStandard(const TimeSpan &value, bool isInvariant, const String &format, Pattern pattern);

        static String formatCustomized(const TimeSpan &value, const String &format, const DateTimeFormatInfo *dtfi);

        static String intToString(int64_t n, int digits);

        static int parseRepeatPattern(const String &format, int pos, char patternChar);

        static void formatDigits(String &outputBuffer, int value, int len);

        static void formatDigits(String &outputBuffer, int value, int len, bool overrideLengthLimit);

        static int parseNextChar(const String &format, int pos);

        static int parseQuoteString(const String &format, int pos, String &result);
    };

    enum TimeSpanStyles {
        TimeStyleNone = 0x00000000,
        AssumeNegative = 0x00000001,
    };

    class TimeSpanParse {
    public:
        // ---- SECTION:  static methods called by System.TimeSpan ---------*
        //
        //  [Try]Parse, [Try]ParseExact, and [Try]ParseExactMultiple
        //
        //  Actions: Main methods called from TimeSpan.Parse
        static TimeSpan Parse(const String &input, const IFormatProvider<DateTimeFormatInfo> *formatProvider);

        static bool
        TryParse(const String &input, const IFormatProvider<DateTimeFormatInfo> *formatProvider, TimeSpan &result);

        static TimeSpan
        ParseExact(const String &input, const String &format, const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                   TimeSpanStyles styles);

        static bool
        TryParseExact(const String &input, const String &format,
                      const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                      TimeSpanStyles styles, TimeSpan &result);

        TimeSpan ParseExactMultiple(const String &input, const StringArray &formats,
                                    const IFormatProvider<DateTimeFormatInfo> *formatProvider, TimeSpanStyles styles);

        static bool TryParseExactMultiple(const String &input, const StringArray &formats,
                                          const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                          TimeSpanStyles styles, TimeSpan &result);

    private:
        // ---- SECTION:  members for support ---------*
        static bool ValidateStyles(TimeSpanStyles style, const String &parameterName);

        static const int unlimitedDigits = -1;
        static const int maxFractionDigits = 7;

        static const int maxDays = 10675199;
        static const int maxHours = 23;
        static const int maxMinutes = 59;
        static const int maxSeconds = 59;
        static const int maxFraction = 9999999;

        static const int64_t MaxMilliSeconds;
        static const int64_t MinMilliSeconds;

        enum TimeSpanThrowStyle {
            ThrowStyleNone = 0,
            ThrowStyleAll = 1,
        };

        enum ParseFailureKind {
            ParseFailureKindNone = 0,
            ArgumentNull = 1,
            Format = 2,
            FormatWithParameter = 3,
            Overflow = 4,
        };

        enum TimeSpanStandardStyles {     // Standard Format Styles
            StandardStyleNone = 0x00000000,
            Invariant = 0x00000001, //Allow Invariant Culture
            Localized = 0x00000002, //Allow Localized Culture
            RequireFull = 0x00000004, //Require the input to be in DHMSF format
            StandardStyleAny = Invariant | Localized,
        };

        // TimeSpan Token Types
        enum TTT {
            TTTNone = 0,    // None of the TimeSpanToken fields are set
            End = 1,    // '\0'
            Num = 2,    // Number
            Sep = 3,    // literal
            NumOverflow = 4,    // Number that overflowed
        };

        struct TimeSpanToken {
            TTT ttt;
            int num;           // Store the number that we are parsing (if any)
            int zeroes;        // Store the number of leading zeroes (if any)
            String sep;        // Store the literal that we are parsing (if any)

            TimeSpanToken(int number = 0);

            TimeSpanToken(int leadingZeroes, int number);

            bool IsInvalidNumber(int maxValue, int maxPrecision) const;
        };

        static const TimeSpanToken zero;

        //
        //  TimeSpanTokenizer
        //
        //  Actions: TimeSpanTokenizer.GetNextToken() returns the next token in the input string.
        //
        struct TimeSpanTokenizer {
            int m_pos;
            String m_value;

            void Init(const String &input);

            void Init(const String &input, int startPosition);

            // used by the parsing routines that operate on standard-formats
            TimeSpanToken GetNextToken();

            bool EOL() const;

            // BackOne, NextChar, CurrentChar - used by ParseExact (ParseByFormat) to operate
            // on custom-formats where exact character-by-character control is allowed
            void BackOne();

            char NextChar();

            char CurrentChar() const;
        };

        // This will store the result of the parsing.  And it will eventually be used to construct a TimeSpan instance.
        struct TimeSpanResult {
            TimeSpan parsedTimeSpan;
            TimeSpanThrowStyle throwStyle;

            ParseFailureKind m_failure;
            string m_failureMessageID;
            string m_failureArgumentName;

            void Init(TimeSpanThrowStyle canThrow);

            void SetFailure(ParseFailureKind failure, const String &failureMessageID);

            void
            SetFailure(ParseFailureKind failure, const String &failureMessageID, const String &failureArgumentName);

            String GetExceptionString();
        };

        // This stores intermediary parsing state for the standard formats
        struct TimeSpanRawInfo {
            static const int MaxTokens = 11;
            static const int MaxLiteralTokens = 6;
            static const int MaxNumericTokens = 5;

            TTT lastSeenTTT;
            int tokenCount;
            int SepCount;
            int NumCount;
            String literals[MaxLiteralTokens];
            TimeSpanToken numbers[MaxNumericTokens];  // raw numbers

            FormatLiterals m_posLoc;
            FormatLiterals m_negLoc;
            bool m_posLocInit;
            bool m_negLocInit;
            String m_fullPosPattern;
            String m_fullNegPattern;

            void Init(const DateTimeFormatInfo *dtfi);

            bool ProcessToken(TimeSpanToken &tok, TimeSpanResult &result);

            bool AddSep(const String &sep, TimeSpanResult &result);

            bool AddNum(const TimeSpanToken &num, TimeSpanResult &result);

            FormatLiterals PositiveInvariant();

            FormatLiterals NegativeInvariant();

            FormatLiterals PositiveLocalized();

            FormatLiterals NegativeLocalized();

            bool FullAppCompatMatch(const FormatLiterals &pattern);

            bool PartialAppCompatMatch(const FormatLiterals &pattern);

            // DHMSF (all values matched)
            bool FullMatch(const FormatLiterals &pattern);

            // D (no hours, minutes, seconds, or fractions)
            bool FullDMatch(const FormatLiterals &pattern);

            // HM (no days, seconds, or fractions)
            bool FullHMMatch(const FormatLiterals &pattern);

            // DHM (no seconds or fraction)
            bool FullDHMMatch(const FormatLiterals &pattern);

            // HMS (no days or fraction)
            bool FullHMSMatch(const FormatLiterals &pattern);

            // DHMS (no fraction)
            bool FullDHMSMatch(const FormatLiterals &pattern);

            // HMSF (no days)
            bool FullHMSFMatch(const FormatLiterals &pattern);
        };

        struct StringParser {
            String str;
            char ch;
            int pos;
            int len;

            void NextChar();

            char NextNonDigit();

            bool TryParse(const String &input, TimeSpanResult &result);

            bool ParseInt(int max, int &i, TimeSpanResult &result);

            bool ParseTime(int64_t &time, TimeSpanResult &result);

            void SkipBlanks();
        };

        static bool TryTimeToTicks(bool positive, const TimeSpanToken &days, const TimeSpanToken &hours,
                                   const TimeSpanToken &minutes,
                                   const TimeSpanToken &seconds, const TimeSpanToken &fraction, int64_t &result);

        // ---- SECTION:  static methods that do the actual work ---------*
        //
        //  TryParseTimeSpan
        //
        //  Actions: Common Parse method called by both Parse and TryParse
        //
        static bool TryParseTimeSpan(const String &input, TimeSpanStandardStyles style,
                                     const IFormatProvider<DateTimeFormatInfo> *formatProvider, TimeSpanResult &
        result);


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
        static bool
        ProcessTerminalState(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);

        //
        //  ProcessTerminal_DHMSF
        //
        //  Actions: Validate the 5-number "Days.Hours:Minutes:Seconds.Fraction" terminal case.
        //           Sets result.parsedTimeSpan on success.
        //
        static bool
        ProcessTerminal_DHMSF(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);

        //
        //  ProcessTerminal_HMS_F_D
        //
        //  Actions: Validate the ambiguous 4-number "Hours:Minutes:Seconds.Fraction", "Days.Hours:Minutes:Seconds", or "Days.Hours:Minutes:.Fraction" terminal case.
        //           Sets result.parsedTimeSpan on success.
        //
        static bool
        ProcessTerminal_HMS_F_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);

        //
        //  ProcessTerminal_HM_S_D
        //
        //  Actions: Validate the ambiguous 3-number "Hours:Minutes:Seconds", "Days.Hours:Minutes", or "Hours:Minutes:.Fraction" terminal case
        //
        static bool
        ProcessTerminal_HM_S_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);

        //
        //  ProcessTerminal_HM
        //
        //  Actions: Validate the 2-number "Hours:Minutes" terminal case
        //
        static bool
        ProcessTerminal_HM(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);


        //
        //  ProcessTerminal_D
        //
        //  Actions: Validate the 1-number "Days" terminal case
        //
        static bool
        ProcessTerminal_D(TimeSpanRawInfo &raw, TimeSpanStandardStyles style, TimeSpanResult &result);

        //
        //  TryParseExactTimeSpan
        //
        //  Actions: Common ParseExact method called by both ParseExact and TryParseExact
        //
        static bool TryParseExactTimeSpan(const String &input, const String &format,
                                          const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                          TimeSpanStyles styles, TimeSpanResult &result);

        //
        //  TryParseByFormat
        //
        //  Actions: Parse the TimeSpan instance using the specified format.  Used by TryParseExactTimeSpan.
        //
        static bool
        TryParseByFormat(const String &input, const String &format, TimeSpanStyles styles, TimeSpanResult &result);

        static bool ParseExactDigits(TimeSpanTokenizer &tokenizer, int minDigitLength, int &result);

        static bool
        ParseExactDigits(TimeSpanTokenizer &tokenizer, int minDigitLength, int maxDigitLength, int &zeroes,
                         int &result);

        static bool ParseExactLiteral(TimeSpanTokenizer &tokenizer, const String &enquotedString);

        //
        // TryParseTimeSpanConstant
        //
        // Actions: Parses the "c" (constant) format.  This code is 100% identical to the non-globalized v1.0-v3.5 TimeSpan.Parse() routine
        //          and exists for performance/appcompat with legacy callers who cannot move onto the globalized Parse overloads.
        //
        static bool TryParseTimeSpanConstant(const String &input, TimeSpanResult &result);

        //
        //  TryParseExactMultipleTimeSpan
        //
        //  Actions: Common ParseExactMultiple method called by both ParseExactMultiple and TryParseExactMultiple
        //
        static
        bool TryParseExactMultipleTimeSpan(const String &input, const StringArray &formats,
                                           const IFormatProvider<DateTimeFormatInfo> *formatProvider,
                                           TimeSpanStyles styles, TimeSpanResult &result);

        static int ParseRepeatPattern(const String &format, int pos, char patternChar);

        static bool TryParseQuoteString(const String &format, int pos, String &result, int &returnValue);

        static int ParseNextChar(const String &format, int pos);
    };
}

#endif // TimeSpanFormat_h
