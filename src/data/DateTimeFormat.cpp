//
//  DateTimeFormat.cpp
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright © 2022 com. All rights reserved.
//

#include "DateTimeFormat.h"
#include "data/StringArray.h"
#include "data/TimeZone.h"
#include "system/Math.h"

using namespace System;

namespace Data {
    const TimeSpan DateTimeFormat::NullOffset = TimeSpan::MinValue;

    const char DateTimeFormat::allStandardFormats[allStandardFormatsCount] = {
            'd', 'D', 'f', 'F', 'g', 'G',
            'm', 'M', 'o', 'O', 'r', 'R',
            's', 't', 'T', 'u', 'U', 'y', 'Y',
    };

    const String DateTimeFormat::RoundtripFormat = "yyyy'-'MM'-'dd'T'HH':'mm':'ss.fffffffK";
    const String DateTimeFormat::RoundtripDateTimeUnfixed = "yyyy'-'MM'-'ddTHH':'mm':'ss zzz";

    const DateTimeFormatInfo DateTimeFormat::InvariantFormatInfo = DateTimeFormatInfo::currentInfo();
    const String *DateTimeFormat::InvariantAbbreviatedMonthNames = DateTimeFormatInfo::currentInfo().abbreviatedMonthGenitiveNames;
    const String *DateTimeFormat::InvariantAbbreviatedDayNames = DateTimeFormatInfo::currentInfo().abbreviatedDayNames;
    const String DateTimeFormat::Gmt = "GMT";

    const String DateTimeFormat::fixedNumberFormats[7]{
            "0",
            "00",
            "000",
            "0000",
            "00000",
            "000000",
            "0000000",
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // Format the positive integer value to a string and perfix with assigned
    // length of leading zero.
    //
    // Parameters:
    //  value: The value to format
    //  len: The maximum length for leading zero.
    //  If the digits of the value is greater than len, no leading zero is added.
    //
    // Notes:
    //  The function can format to Int32.MaxValue.
    //
    ////////////////////////////////////////////////////////////////////////////
    void DateTimeFormat::FormatDigits(String &outputBuffer, int value, int len) {
        assert(value >= 0);
        FormatDigits(outputBuffer, value, len, false);
    }

    void DateTimeFormat::FormatDigits(String &outputBuffer, int value, int len, bool overrideLengthLimit) {
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

    int DateTimeFormat::ParseRepeatPattern(const String &format, int pos, char patternChar) {
        int len = (int) format.length();
        int index = pos + 1;
        while ((index < len) && (format[index] == patternChar)) {
            index++;
        }
        return (index - pos);
    }

    String DateTimeFormat::FormatDayOfWeek(int dayOfWeek, int repeat, const DateTimeFormatInfo *dtfi) {
        assert(dayOfWeek >= 0 && dayOfWeek <= 6);
        if (repeat == 3) {
            return (dtfi->getAbbreviatedDayName((DayOfWeek) dayOfWeek));
        }
        // Call dtfi->GetDayName() here, instead of accessing DayNames property, because we don't
        // want a clone of DayNames, which will hurt perf.
        return (dtfi->getDayName((DayOfWeek) dayOfWeek));
    }

    String DateTimeFormat::FormatMonth(int month, int repeatCount, const DateTimeFormatInfo *dtfi) {
        assert(month >= 1 && month <= 12);
        if (repeatCount == 3) {
            return (dtfi->getAbbreviatedMonthName(month));
        }
        // Call GetMonthName() here, instead of accessing MonthNames property, because we don't
        // want a clone of MonthNames, which will hurt perf.
        return (dtfi->getMonthName(month));
    }

    //
    // The pos should point to a quote character. This method will
    // get the string encloed by the quote character.
    //
    int DateTimeFormat::ParseQuoteString(const String &format, int pos, String &result) {
        //
        // NOTE : pos will be the index of the quote character in the 'format' string.
        //
        int formatLen = (int) format.length();
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
    int DateTimeFormat::ParseNextChar(const String &format, int pos) {
        if (pos >= (int) format.length() - 1) {
            return (-1);
        }
        return ((int) format[pos + 1]);
    }

    //
    //  IsUseGenitiveForm
    //
    //  Actions: Check the format to see if we should use genitive month in the formatting.
    //      Starting at the position (index) in the (format) string, look back and look ahead to
    //      see if there is "d" or "dd".  In the case like "d MMMM" or "MMMM dd", we can use
    //      genitive form.  Genitive form is not used if there is more than two "d".
    //  Arguments:
    //      format      The format string to be scanned.
    //      index       Where we should start the scanning.  This is generally where "M" starts.
    //      tokenLen    The len of the current pattern character.  This indicates how many "M" that we have.
    //      patternToMatch  The pattern that we want to search. This generally uses "d"
    //
    bool DateTimeFormat::IsUseGenitiveForm(const String &format, int index, int tokenLen, char patternToMatch) {
        int i;
        int repeat = 0;
        //
        // Look back to see if we can find "d" or "ddd"
        //

        // Find first "d".
        for (i = index - 1; i >= 0 && format[i] != patternToMatch; i--) {  /*Do nothing here */ };

        if (i >= 0) {
            // Find a "d", so look back to see how many "d" that we can find.
            while (--i >= 0 && format[i] == patternToMatch) {
                repeat++;
            }
            //
            // repeat == 0 means that we have one (patternToMatch)
            // repeat == 1 means that we have two (patternToMatch)
            //
            if (repeat <= 1) {
                return (true);
            }
            // Note that we can't just stop here.  We may find "ddd" while looking back, and we have to look
            // ahead to see if there is "d" or "dd".
        }

        //
        // If we can't find "d" or "dd" by looking back, try look ahead.
        //

        // Find first "d"
        for (i = index + tokenLen;
             i < (int) format.length() && format[i] != patternToMatch; i++) { /* Do nothing here */ };

        if (i < (int) format.length()) {
            repeat = 0;
            // Find a "d", so continue the walk to see how may "d" that we can find.
            while (++i < (int) format.length() && format[i] == patternToMatch) {
                repeat++;
            }
            //
            // repeat == 0 means that we have one (patternToMatch)
            // repeat == 1 means that we have two (patternToMatch)
            //
            if (repeat <= 1) {
                return (true);
            }
        }
        return (false);
    }

    //
    //  FormatCustomized
    //
    //  Actions: Format the DateTime instance using the specified format.
    //
    String
    DateTimeFormat::FormatCustomized(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi,
                                     const TimeSpan &offset) {
        String result;

        // This is a flag to indicate if we are formatting hour/minute/second only.
        bool bTimeOnly = true;

        int i = 0;
        int tokenLen, hour12;

        while (i < (int) format.length()) {
            char ch = format[i];
            int nextChar;
            switch (ch) {
                case 'g':
                    break;
                case 'h':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    hour12 = dateTime.hour() % 12;
                    if (hour12 == 0) {
                        hour12 = 12;
                    }
                    FormatDigits(result, hour12, tokenLen);
                    break;
                case 'H':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    FormatDigits(result, dateTime.hour(), tokenLen);
                    break;
                case 'm':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    FormatDigits(result, dateTime.minute(), tokenLen);
                    break;
                case 's':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    FormatDigits(result, dateTime.second(), tokenLen);
                    break;
                case 'f':
                case 'F':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen <= MaxSecondsFractionDigits) {
                        int64_t fraction = (dateTime.ticks() % DateTime::TicksPerSecond);
                        fraction = fraction / (int64_t) Math::pow(10.0f, 7 - tokenLen);
                        if (ch == 'f') {
                            result.append(Int64(fraction).toString(fixedNumberFormats[tokenLen - 1]));
                        } else {
                            int effectiveDigits = tokenLen;
                            while (effectiveDigits > 0) {
                                if (fraction % 10 == 0) {
                                    fraction = fraction / 10;
                                    effectiveDigits--;
                                } else {
                                    break;
                                }
                            }
                            if (effectiveDigits > 0) {
                                result.append(Int64(fraction).toString(fixedNumberFormats[effectiveDigits - 1]));
                            } else {
                                // No fraction to emit, so see if we should remove decimal also.
                                if (result.length() > 0 && result[result.length() - 1] == '.') {
                                    result.removeRange(result.length() - 1, 1);
                                }
                            }
                        }
                    } else {
                    }
                    break;
                case 't':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen == 1) {
                        if (dateTime.hour() < 12) {
                            if (dtfi->aMDesignator.length() >= 1) {
                                result.append(dtfi->aMDesignator[0]);
                            }
                        } else {
                            if (dtfi->pMDesignator.length() >= 1) {
                                result.append(dtfi->pMDesignator[0]);
                            }
                        }

                    } else {
                        result.append((dateTime.hour() < 12 ? dtfi->aMDesignator : dtfi->pMDesignator));
                    }
                    break;
                case 'd':
                    //
                    // tokenLen == 1 : Day of month as digits with no leading zero.
                    // tokenLen == 2 : Day of month as digits with leading zero for single-digit months.
                    // tokenLen == 3 : Day of week as a three-leter abbreviation.
                    // tokenLen >= 4 : Day of week as its full name.
                    //
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen <= 2) {
                        int day = dateTime.day();
                        FormatDigits(result, day, tokenLen);
                    } else {
                        int dayOfWeek = dateTime.dayOfWeek();
                        result.append(FormatDayOfWeek(dayOfWeek, tokenLen, dtfi));
                    }
                    bTimeOnly = false;
                    break;
                case 'M': {
                    //
                    // tokenLen == 1 : Month as digits with no leading zero.
                    // tokenLen == 2 : Month as digits with leading zero for single-digit months.
                    // tokenLen == 3 : Month as a three-letter abbreviation.
                    // tokenLen >= 4 : Month as its full name.
                    //
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    int month = dateTime.month();
                    if (tokenLen <= 2) {
                        FormatDigits(result, month, tokenLen);
                    } else {
                        result.append(FormatMonth(month, tokenLen, dtfi));
                    }
                    bTimeOnly = false;
                }
                    break;
                case 'y': {
                    // Notes about OS behavior:
                    // y: Always print (year % 100). No leading zero.
                    // yy: Always print (year % 100) with leading zero.
                    // yyy/yyyy/yyyyy/... : Print year value.  No leading zero.

                    int year = dateTime.year();
                    tokenLen = ParseRepeatPattern(format, i, ch);

                    /*if (dtfi->HasForceTwoDigitYears) {
                        FormatDigits(result, year, tokenLen <= 2 ? tokenLen : 2);
                    } else*/ {
                        if (tokenLen <= 2) {
                            FormatDigits(result, year % 100, tokenLen);
                        } else {
                            String fmtPattern = String::format("D%d", tokenLen);
                            result.append(Int32(year).toString(fmtPattern));
                        }
                    }
                    bTimeOnly = false;
                }
                    break;
                case 'z':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    FormatCustomizedTimeZone(dateTime, offset, format, tokenLen, bTimeOnly, result);
                    break;
                case 'K':
                    tokenLen = 1;
                    FormatCustomizedRoundripTimeZone(dateTime, offset, result);
                    break;
                case ':':
                    result.append(dtfi->timeSeparator);
                    tokenLen = 1;
                    break;
                case '/':
                    result.append(dtfi->dateSeparator);
                    tokenLen = 1;
                    break;
                case '\'':
                case '\"': {
                    String enquotedString;
                    tokenLen = ParseQuoteString(format, i, enquotedString);
                    result.append(enquotedString);
                }
                    break;
                case '%':
                    // Optional format character.
                    // For example, format string "%d" will print day of month
                    // without leading zero.  Most of the cases, "%" can be ignored.
                    nextChar = ParseNextChar(format, i);
                    // nextChar will be -1 if we already reach the end of the format string.
                    // Besides, we will not allow "%%" appear in the pattern.
                    if (nextChar >= 0 && nextChar != (int) '%') {
                        result.append(FormatCustomized(dateTime, Char(nextChar).toString(), dtfi, offset));
                        tokenLen = 2;
                    } else {
                        //
                        // This means that '%' is at the end of the format string or
                        // "%%" appears in the format string.
                        //
                    }
                    break;
                case '\\':
                    // Escaped character.  Can be used to insert character into the format string.
                    // For exmple, "\d" will insert the character 'd' into the string.
                    //
                    // NOTENOTE : we can remove this format character if we enforce the enforced quote
                    // character rule.
                    // That is, we ask everyone to use single quote or double quote to insert characters,
                    // then we can remove this character.
                    //
                    nextChar = ParseNextChar(format, i);
                    if (nextChar >= 0) {
                        result.append(((char) nextChar));
                        tokenLen = 2;
                    } else {
                        //
                        // This means that '\' is at the end of the formatting string.
                        //
                    }
                    break;
                default:
                    // NOTENOTE : we can remove this rule if we enforce the enforced quote
                    // character rule.
                    // That is, if we ask everyone to use single quote or double quote to insert characters,
                    // then we can remove this default block.
                    result.append(ch);
                    tokenLen = 1;
                    break;
            }
            i += tokenLen;
        }
        return result;
    }

    // output the 'z' famliy of formats, which output a the offset from UTC, e.g. "-07:30"
    void
    DateTimeFormat::FormatCustomizedTimeZone(const DateTime &dateTime, const TimeSpan &offset, const String &format,
                                             int tokenLen,
                                             bool timeOnly,
                                             String &result) {
        TimeSpan time = offset;
        // See if the instance already has an offset
        bool dateTimeFormat = (time == NullOffset);
        if (dateTimeFormat) {
            // No offset. The instance is a DateTime and the output should be the local time zone

            if (timeOnly && dateTime.ticks() < DateTime::TicksPerDay) {
                // For time only format and a time only input, the time offset on 0001/01/01 is less
                // accurate than the system's current offset because of daylight saving time.
                time = TimeZone::getUtcOffset(DateTime::now());
            } else if (dateTime.kind() == DateTime::Kind::Utc) {
                time = TimeSpan::Zero;
            } else {
                time = TimeZone::getUtcOffset(dateTime);
            }
        }
        if (time >= TimeSpan::Zero) {
            result.append('+');
        } else {
            result.append('-');
            // get a positive offset, so that you don't need a separate code path for the negative numbers.
            time = time.negate();
        }

        if (tokenLen <= 1) {
            // 'z' format e.g "-7"
            result.append(Int32(time.hours()).toString("d1"));
        } else {
            // 'zz' or longer format e.g "-07"
            result.append(Int32(time.hours()).toString("d2"));
            if (tokenLen >= 3) {
                // 'zzz*' or longer format e.g "-07:30"
                result.append(Int32(time.minutes()).toString("d2"));
            }
        }
    }

// output the 'K' format, which is for round-tripping the data
    void
    DateTimeFormat::FormatCustomizedRoundripTimeZone(const DateTime &dateTime, const TimeSpan &offset, String &result) {

        // The objective of this format is to round trip the data in the type
        // For DateTime it should round-trip the Kind value and preserve the time zone.
        // DateTimeOffset instance, it should do so by using the time zone.
        TimeSpan time = offset;
        if (offset == NullOffset) {
            // source is a date time, so behavior depends on the kind.
            switch (dateTime.kind()) {
                case DateTime::Kind::Local:
                    // This should output the local offset, e.g. "-07:30"
                    time = TimeZone::getUtcOffset(dateTime);
                    // fall through to shared time zone output code
                    break;
                case DateTime::Kind::Utc:
                    // The 'Z' constant is a marker for a UTC date
                    result.append("Z");
                    return;
                default:
                    // If the kind is unspecified, we output nothing here
                    return;
            }
        }
        if (time >= TimeSpan::Zero) {
            result.append('+');
        } else {
            result.append('-');
            // get a positive offset, so that you don't need a separate code path for the negative numbers.
            time = time.negate();
        }

        result.appendFormat("%s:%s",
                            Int32(time.hours()).toString("d2").c_str(),
                            Int32(time.minutes()).toString("d2").c_str());
    }


    String DateTimeFormat::GetRealFormat(const String &format, const DateTimeFormatInfo *dtfi) {
        String realFormat;

        switch (format[0]) {
            case 'd':       // Short Date
                realFormat = dtfi->shortDatePattern;
                break;
            case 'D':       // Long Date
                realFormat = dtfi->longDatePattern;
                break;
            case 'f':       // Full (int64_t date + short time)
                realFormat = dtfi->longDatePattern + " " + dtfi->shortTimePattern;
                break;
            case 'F':       // Full (int64_t date + int64_t time)
                realFormat = dtfi->fullDateTimePattern;
                break;
            case 'g':       // General (short date + short time)
                realFormat = dtfi->generalShortTimePattern();
                break;
            case 'G':       // General (short date + int64_t time)
                realFormat = dtfi->generalLongTimePattern();
                break;
            case 'm':
            case 'M':       // Month/Day Date
                realFormat = dtfi->monthDayPattern;
                break;
            case 'o':
            case 'O':
                realFormat = RoundtripFormat;
                break;
            case 'r':
            case 'R':       // RFC 1123 Standard
                realFormat = dtfi->rFC1123Pattern;
                break;
            case 's':       // Sortable without Time Zone Info
                realFormat = dtfi->sortableDateTimePattern;
                break;
            case 't':       // Short Time
                realFormat = dtfi->shortTimePattern;
                break;
            case 'T':       // Long Time
                realFormat = dtfi->longTimePattern;
                break;
            case 'u':       // Universal with Sortable format
                realFormat = dtfi->universalSortableDateTimePattern;
                break;
            case 'U':       // Universal with Full (int64_t date + int64_t time) format
                realFormat = dtfi->fullDateTimePattern;
                break;
            case 'y':
            case 'Y':       // Year/Month Date
                realFormat = dtfi->yearMonthPattern;
                break;
            default:
                break;
        }
        return realFormat;
    }


    // Expand a pre-defined format string (like "D" for int64_t date) to the real format that
    // we are going to use in the date time parsing.
    // This method also convert the dateTime if necessary (e.g. when the format is in Universal time),
    // and change dtfi if necessary (e.g. when the format should use invariant culture).
    //
    String
    DateTimeFormat::ExpandPredefinedFormat(const String &format, const DateTime &dateTime,
                                           const DateTimeFormatInfo *dtfi,
                                           const TimeSpan &offset) {
//        switch (format[0]) {
//            case 's':       // Sortable without Time Zone Info
//                dtfi = DateTimeFormatInfo.InvariantInfo;
//                break;
//            case 'u':       // Universal time in sortable format.
//                if (offset != NullOffset) {
//                    // Convert to UTC invariants mean this will be in range
//                    dateTime = dateTime - offset;
//                } else if (dateTime.Kind == DateTimeKind.Local) {
//
//                    InvalidFormatForLocal(format, dateTime);
//                }
//                dtfi = DateTimeFormatInfo.InvariantInfo;
//                break;
//            case 'U':       // Universal time in culture dependent format.
//                if (offset != NullOffset) {
//                    // This format is not supported by DateTimeOffset
//                    throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
//                }
//                // Universal time is always in Greogrian calendar.
//                //
//                // Change the Calendar to be Gregorian Calendar.
//                //
//                dtfi = (DateTimeFormatInfo)
//                        dtfi->Clone();
//                if (dtfi->Calendar.GetType() != typeof(GregorianCalendar)) {
//            dtfi->Calendar = GregorianCalendar.GetDefaultInstance();
//        }
//                dateTime = dateTime.ToUniversalTime();
//                break;
//        }
        return GetRealFormat(format, dtfi);
    }

    String DateTimeFormat::Format(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi) {
        return Format(dateTime, format, dtfi, NullOffset);
    }


    String
    DateTimeFormat::Format(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi,
                           const TimeSpan &offset) {
        String fmt = format;
        if (fmt.isNullOrEmpty()) {
            Boolean timeOnlySpecialCase = false;
            if (dateTime.ticks() < DateTime::TicksPerDay) {
                // If the time is less than 1 day, consider it as time of day.
                // Just print  the short time fmt.
                //
                // This is a workaround for VB, since they use ticks less then one day to be
                // time of day.  In cultures which use calendar other than Gregorian calendar, these
                // alternative calendar may not support ticks less than a day.
                // For example, Japanese calendar only supports date after 1868/9/8.
                // This will pose a problem when people in VB get the time of day, and use it
                // to call ToString(), which will use the general fmt (short date + int64_t time).
                // Since Japanese calendar does not support Gregorian year 0001, an exception will be
                // thrown when we try to get the Japanese year for Gregorian year 0001.
                // Therefore, the workaround allows them to call ToString() for time of day from a DateTime by
                // fmtting as ISO 8601 fmt.
//                switch (dtfi->Calendar.ID) {
//                    case Calendar.CAL_JAPAN:
//                    case Calendar.CAL_TAIWAN:
//                    case Calendar.CAL_HIJRI:
//                    case Calendar.CAL_HEBREW:
//                    case Calendar.CAL_JULIAN:
//                    case Calendar.CAL_UMALQURA:
//                    case Calendar.CAL_PERSIAN:
//                        timeOnlySpecialCase = true;
//                        dtfi = DateTimeFormatInfo.InvariantInfo;
//                        break;
//                }
            }
            if (offset == NullOffset) {
                // Default DateTime.ToString case.
                if (timeOnlySpecialCase) {
                    fmt = "s";
                } else {
                    fmt = "G";
                }
            } else {
                // Default DateTimeOffset.ToString case.
                if (timeOnlySpecialCase) {
                    fmt = RoundtripDateTimeUnfixed;
                } else {
                    fmt = dtfi->dateTimeOffsetPattern();
                }

            }

        }

        if (fmt.length() == 1) {
            switch (fmt[0]) {
                case 'O':
                case 'o':
                    return FastFormatRoundtrip(dateTime, offset);
                case 'R':
                case 'r':
                    return FastFormatRfc1123(dateTime, offset, dtfi);
            }

            fmt = ExpandPredefinedFormat(fmt,
                                         dateTime,
                                         dtfi,
                                         offset);
        }

        return FormatCustomized(dateTime, fmt, dtfi, offset);
    }

    String
    DateTimeFormat::FastFormatRfc1123(DateTime dateTime, const TimeSpan &offset, const DateTimeFormatInfo *dtfi) {
        // ddd, dd MMM yyyy HH:mm:ss GMT
        String result;

        if (offset != NullOffset) {
            // Convert to UTC invariants
            dateTime = dateTime - offset;
        }

        int year = dateTime.year();
        int month = dateTime.month();
        int day = dateTime.day();

        result.append(InvariantAbbreviatedDayNames[(int) dateTime.dayOfWeek()]);
        result.append(',');
        result.append(' ');
        appendNumber(result, day, 2);
        result.append(' ');
        result.append(InvariantAbbreviatedMonthNames[month - 1]);
        result.append(' ');
        appendNumber(result, year, 4);
        result.append(' ');
        appendHHmmssTimeOfDay(result, dateTime);
        result.append(' ');
        result.append(Gmt);

        return result;
    }

    String DateTimeFormat::FastFormatRoundtrip(const DateTime &dateTime, const TimeSpan &offset) {
        // yyyy-MM-ddTHH:mm:ss.fffffffK
        String result;

        int year = dateTime.year();
        int month = dateTime.month();
        int day = dateTime.day();

        appendNumber(result, year, 4);
        result.append('-');
        appendNumber(result, month, 2);
        result.append('-');
        appendNumber(result, day, 2);
        result.append('T');
        appendHHmmssTimeOfDay(result, dateTime);
        result.append('.');

        int64_t fraction = dateTime.ticks() % TimeSpan::TicksPerSecond;
        appendNumber(result, fraction, 7);

        FormatCustomizedRoundripTimeZone(dateTime, offset, result);

        return result;
    }

    void DateTimeFormat::appendHHmmssTimeOfDay(String &result, const DateTime &dateTime) {
        // HH:mm:ss
        appendNumber(result, dateTime.hour(), 2);
        result.append(':');
        appendNumber(result, dateTime.minute(), 2);
        result.append(':');
        appendNumber(result, dateTime.second(), 2);
    }

    void DateTimeFormat::appendNumber(String &builder, int64_t val, int digits) {
        for (int i = 0; i < digits; i++) {
            builder.append('0');
        }

        int index = 1;
        while (val > 0 && index <= digits) {
            builder[builder.length() - index] = (char) ('0' + (val % 10));
            val = val / 10;
            index++;
        }

        assert(val == 0);
    }
}