//
//  DateTimeFormat.cpp
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright © 2022 com. All rights reserved.
//

#include "data/DateTimeFormat.h"
#include "system/Math.h"

namespace Common {
    const TimeSpan DateTimeFormat::NullOffset = TimeSpan::MinValue;

    const char DateTimeFormat::allStandardFormats[] = {
            'd', 'D', 'f', 'F', 'g', 'G',
            'm', 'M', 'o', 'O', 'r', 'R',
            's', 't', 'T', 'u', 'U', 'y', 'Y',
    };

    const String DateTimeFormat::RoundtripFormat = "yyyy'-'MM'-'dd'T'HH':'mm':'ss.fffffffK";
    const String DateTimeFormat::RoundtripDateTimeUnfixed = "yyyy'-'MM'-'ddTHH':'mm':'ss zzz";

    const DateTimeFormatInfo DateTimeFormat::InvariantFormatInfo = DateTimeFormatInfo::currentInfo();
    const String *DateTimeFormat::InvariantAbbreviatedMonthNames = DateTimeFormatInfo::currentInfo().abbreviatedMonthNames;
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

    void DateTimeFormat::HebrewFormatDigits(String &outputBuffer, int digits) {
        outputBuffer.append(HebrewNumber.ToString(digits));
    }

    int DateTimeFormat::ParseRepeatPattern(const String &format, int pos, char patternChar) {
        int len = format.length();
        int index = pos + 1;
        while ((index < len) && (format[index] == patternChar)) {
            index++;
        }
        return (index - pos);
    }

    String DateTimeFormat::FormatDayOfWeek(int dayOfWeek, int repeat, const DateTimeFormatInfo &dtfi) {
        assert(dayOfWeek >= 0 && dayOfWeek <= 6);
        if (repeat == 3) {
            return (dtfi.getAbbreviatedDayName((DayOfWeek) dayOfWeek));
        }
        // Call dtfi.GetDayName() here, instead of accessing DayNames property, because we don't
        // want a clone of DayNames, which will hurt perf.
        return (dtfi.getDayName((DayOfWeek) dayOfWeek));
    }

    String DateTimeFormat::FormatMonth(int month, int repeatCount, const DateTimeFormatInfo &dtfi) {
        assert(month >= 1 && month <= 12);
        if (repeatCount == 3) {
            return (dtfi.getAbbreviatedMonthName(month));
        }
        // Call GetMonthName() here, instead of accessing MonthNames property, because we don't
        // want a clone of MonthNames, which will hurt perf.
        return (dtfi.getMonthName(month));
    }

    //
    //  FormatHebrewMonthName
    //
    //  Action: Return the Hebrew month name for the specified DateTime.
    //  Returns: The month name string for the specified DateTime.
    //  Arguments:
    //        time   the time to format
    //        month  The month is the value of HebrewCalendar.GetMonth(time).
    //        repeat Return abbreviated month name if repeat=3, or full month name if repeat=4
    //        dtfi    The DateTimeFormatInfo which uses the Hebrew calendars as its calendar.
    //  Exceptions: None.
    //

    /* Note:
        If DTFI is using Hebrew calendar, GetMonthName()/GetAbbreviatedMonthName() will return month names like this:
        1   Hebrew 1st Month
        2   Hebrew 2nd Month
        ..  ...
        6   Hebrew 6th Month
        7   Hebrew 6th Month II (used only in a leap year)
        8   Hebrew 7th Month
        9   Hebrew 8th Month
        10  Hebrew 9th Month
        11  Hebrew 10th Month
        12  Hebrew 11th Month
        13  Hebrew 12th Month

        Therefore, if we are in a regular year, we have to increment the month name if moth is greater or eqaul to 7.
    */
    String
    DateTimeFormat::FormatHebrewMonthName(const DateTime& time, int month, int repeatCount, const DateTimeFormatInfo &dtfi) {
        assert(repeatCount != 3 || repeatCount != 4);
        if (dtfi.Calendar.IsLeapYear(dtfi.Calendar.GetYear(time))) {
            // This month is in a leap year
            return (dtfi.internalGetMonthName(month, MonthNameStyles.LeapYear, (repeatCount == 3)));
        }
        // This is in a regular year.
        if (month >= 7) {
            month++;
        }
        if (repeatCount == 3) {
            return (dtfi.getAbbreviatedMonthName(month));
        }
        return (dtfi.getMonthName(month));
    }

    //
    // The pos should point to a quote character. This method will
    // get the string encloed by the quote character.
    //
    int DateTimeFormat::ParseQuoteString(const String &format, int pos, String &result) {
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
    int DateTimeFormat::ParseNextChar(const String &format, int pos) {
        if (pos >= format.length() - 1) {
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
             i < format.length() && format[i] != patternToMatch; i++) { /* Do nothing here */ };

        if (i < format.length()) {
            repeat = 0;
            // Find a "d", so continue the walk to see how may "d" that we can find.
            while (++i < format.length() && format[i] == patternToMatch) {
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
    DateTimeFormat::FormatCustomized(const DateTime &dateTime, const String &format, const DateTimeFormatInfo &dtfi,
                                     const TimeSpan &offset) {
        Calendar cal = dtfi.Calendar;
        String result;
        // This is a flag to indicate if we are format the dates using Hebrew calendar.

        bool isHebrewCalendar = (cal.ID == Calendar.CAL_HEBREW);
        bool isJapaneseCalendar = (cal.ID == Calendar.CAL_JAPAN);

        // This is a flag to indicate if we are formating hour/minute/second only.
        bool bTimeOnly = true;

        int i = 0;
        int tokenLen, hour12;

        while (i < format.length()) {
            char ch = format[i];
            int nextChar;
            switch (ch) {
                case 'g':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    result.append(dtfi.getEraName(cal.GetEra(dateTime)));
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
                        long fraction = (dateTime.ticks() % Calendar.TicksPerSecond);
                        fraction = fraction / (long) Math::pow(10, 7 - tokenLen);
                        if (ch == 'f') {
                            result.append(((int) fraction).toString(fixedNumberFormats[tokenLen - 1],
                                                                    CultureInfo.InvariantCulture));
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
                                result.append(((int) fraction).ToString(fixedNumberFormats[effectiveDigits - 1],
                                                                        CultureInfo.InvariantCulture));
                            } else {
                                // No fraction to emit, so see if we should remove decimal also.
                                if (result.Length > 0 && result[result.Length - 1] == '.') {
                                    result.Remove(result.Length - 1, 1);
                                }
                            }
                        }
                    } else {
                        throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
                    }
                    break;
                case 't':
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    if (tokenLen == 1) {
                        if (dateTime.hour() < 12) {
                            if (dtfi.aMDesignator.length() >= 1) {
                                result.append(dtfi.aMDesignator[0]);
                            }
                        } else {
                            if (dtfi.pMDesignator.length() >= 1) {
                                result.append(dtfi.pMDesignator[0]);
                            }
                        }

                    } else {
                        result.append((dateTime.hour() < 12 ? dtfi.aMDesignator : dtfi.pMDesignator));
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
                        int day = cal.GetDayOfMonth(dateTime);
                        if (isHebrewCalendar) {
                            // For Hebrew calendar, we need to convert numbers to Hebrew text for yyyy, MM, and dd values.
                            HebrewFormatDigits(result, day);
                        } else {
                            FormatDigits(result, day, tokenLen);
                        }
                    } else {
                        int dayOfWeek = (int) cal.GetDayOfWeek(dateTime);
                        result.append(FormatDayOfWeek(dayOfWeek, tokenLen, dtfi));
                    }
                    bTimeOnly = false;
                    break;
                case 'M':
                    //
                    // tokenLen == 1 : Month as digits with no leading zero.
                    // tokenLen == 2 : Month as digits with leading zero for single-digit months.
                    // tokenLen == 3 : Month as a three-letter abbreviation.
                    // tokenLen >= 4 : Month as its full name.
                    //
                    tokenLen = ParseRepeatPattern(format, i, ch);
                    int month = cal.GetMonth(dateTime);
                    if (tokenLen <= 2) {
                        if (isHebrewCalendar) {
                            // For Hebrew calendar, we need to convert numbers to Hebrew text for yyyy, MM, and dd values.
                            HebrewFormatDigits(result, month);
                        } else {
                            FormatDigits(result, month, tokenLen);
                        }
                    } else {
                        if (isHebrewCalendar) {
                            result.append(FormatHebrewMonthName(dateTime, month, tokenLen, dtfi));
                        } else {
                            if ((dtfi.FormatFlags & DateTimeFormatFlags.UseGenitiveMonth) != 0 && tokenLen >= 4) {
                                result.append(
                                        dtfi.internalGetMonthName(
                                                month,
                                                IsUseGenitiveForm(format, i, tokenLen, 'd')
                                                ? MonthNameStyles.Genitive : MonthNameStyles.Regular,
                                                false));
                            } else {
                                result.append(FormatMonth(month, tokenLen, dtfi));
                            }
                        }
                    }
                    bTimeOnly = false;
                    break;
                case 'y':
                    // Notes about OS behavior:
                    // y: Always print (year % 100). No leading zero.
                    // yy: Always print (year % 100) with leading zero.
                    // yyy/yyyy/yyyyy/... : Print year value.  No leading zero.

                    int year = cal.GetYear(dateTime);
                    tokenLen = ParseRepeatPattern(format, i, ch);

                    if (isJapaneseCalendar &&
                        !AppContextSwitches.FormatJapaneseFirstYearAsANumber &&
                        year == 1 &&
                        ((i + tokenLen < format.length() &&
                          format[i + tokenLen] == DateTimeFormatInfoScanner.CJKYearSuff[0]) ||
                         (i + tokenLen < format.length() - 1 && format[i + tokenLen] == '\'' &&
                          format[i + tokenLen + 1] == DateTimeFormatInfoScanner.CJKYearSuff[0]))) {
                        // We are formatting a Japanese date with year equals 1 and the year number is followed by the year sign \u5e74
                        // In Japanese dates, the first year in the era is not formatted as a number 1 instead it is formatted as \u5143 which means
                        // first or beginning of the era.
                        result.append(DateTimeFormatInfo.JapaneseEraStart[0]);
                    } else if (dtfi.HasForceTwoDigitYears) {
                        FormatDigits(result, year, tokenLen <= 2 ? tokenLen : 2);
                    } else if (cal.ID == Calendar.CAL_HEBREW) {
                        HebrewFormatDigits(result, year);
                    } else {
                        if (tokenLen <= 2) {
                            FormatDigits(result, year % 100, tokenLen);
                        } else {
                            String fmtPattern = "D" + tokenLen;
                            result.append(year.ToString(fmtPattern, CultureInfo.InvariantCulture));
                        }
                    }
                    bTimeOnly = false;
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
                    result.append(dtfi.TimeSeparator);
                    tokenLen = 1;
                    break;
                case '/':
                    result.append(dtfi.DateSeparator);
                    tokenLen = 1;
                    break;
                case '\'':
                case '\"':
                    String enquotedString = new String();
                    tokenLen = ParseQuoteString(format, i, enquotedString);
                    result.append(enquotedString);
                    break;
                case '%':
                    // Optional format character.
                    // For example, format string "%d" will print day of month
                    // without leading zero.  Most of the cases, "%" can be ignored.
                    nextChar = ParseNextChar(format, i);
                    // nextChar will be -1 if we already reach the end of the format string.
                    // Besides, we will not allow "%%" appear in the pattern.
                    if (nextChar >= 0 && nextChar != (int) '%') {
                        result.append(FormatCustomized(dateTime, ((char) nextChar).ToString(), dtfi, offset));
                        tokenLen = 2;
                    } else {
                        //
                        // This means that '%' is at the end of the format string or
                        // "%%" appears in the format string.
                        //
                        throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
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
                        throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
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
        return StringCache.GetStringAndRelease(result);
    }

    // output the 'z' famliy of formats, which output a the offset from UTC, e.g. "-07:30"
    void DateTimeFormat::FormatCustomizedTimeZone(const DateTime &dateTime, const TimeSpan &offset, const String &format,
                                             int tokenLen,
                                             bool timeOnly,
                                             String &result) {
        // See if the instance already has an offset
        Boolean dateTimeFormat = (offset == NullOffset);
        if (dateTimeFormat) {
            // No offset. The instance is a DateTime and the output should be the local time zone

            if (timeOnly && dateTime.ticks() < Calendar.TicksPerDay) {
                // For time only format and a time only input, the time offset on 0001/01/01 is less
                // accurate than the system's current offset because of daylight saving time.
                offset = TimeZoneInfo.GetLocalUtcOffset(DateTime.Now, TimeZoneInfoOptions.NoThrowOnInvalidTime);
            } else if (dateTime.Kind == DateTimeKind.Utc) {
#if FEATURE_CORECLR
                offset = TimeSpan.Zero;
#else // FEATURE_CORECLR
                // This code path points to a bug in user code. It would make sense to return a 0 offset in this case.
                // However, because it was only possible to detect this in Whidbey, there is user code that takes a
                // dependency on being serialize a UTC DateTime using the 'z' format, and it will work almost all the
                // time if it is offset by an incorrect conversion to local time when parsed. Therefore, we need to
                // explicitly emit the local time offset, which we can do by removing the UTC flag.
                InvalidFormatForUtc(format, dateTime);
                dateTime = DateTime.SpecifyKind(dateTime, DateTimeKind.Local);
                offset = TimeZoneInfo.GetLocalUtcOffset(dateTime, TimeZoneInfoOptions.NoThrowOnInvalidTime);
#endif // FEATURE_CORECLR
            } else {
                offset = TimeZoneInfo.GetLocalUtcOffset(dateTime, TimeZoneInfoOptions.NoThrowOnInvalidTime);
            }
        }
        if (offset >= TimeSpan.Zero) {
            result.append('+');
        } else {
            result.append('-');
            // get a positive offset, so that you don't need a separate code path for the negative numbers.
            offset = offset.Negate();
        }

        if (tokenLen <= 1) {
            // 'z' format e.g "-7"
            result.appendFormat(CultureInfo.InvariantCulture, "{0:0}", offset.Hours);
        } else {
            // 'zz' or longer format e.g "-07"
            result.appendFormat(CultureInfo.InvariantCulture, "{0:00}", offset.Hours);
            if (tokenLen >= 3) {
                // 'zzz*' or longer format e.g "-07:30"
                result.appendFormat(CultureInfo.InvariantCulture, ":{0:00}", offset.Minutes);
            }
        }
    }

// output the 'K' format, which is for round-tripping the data
    void
    DateTimeFormat::FormatCustomizedRoundripTimeZone(const DateTime &dateTime, const TimeSpan &offset, String &result) {

        // The objective of this format is to round trip the data in the type
        // For DateTime it should round-trip the Kind value and preserve the time zone.
        // DateTimeOffset instance, it should do so by using the time zone.

        if (offset == NullOffset) {
            // source is a date time, so behavior depends on the kind.
            switch (dateTime.Kind) {
                case DateTimeKind.Local:
                    // This should output the local offset, e.g. "-07:30"
                    offset = TimeZoneInfo.GetLocalUtcOffset(dateTime, TimeZoneInfoOptions.NoThrowOnInvalidTime);
                    // fall through to shared time zone output code
                    break;
                case DateTimeKind.Utc:
                    // The 'Z' constant is a marker for a UTC date
                    result.append("Z");
                    return;
                default:
                    // If the kind is unspecified, we output nothing here
                    return;
            }
        }
        if (offset >= TimeSpan.Zero) {
            result.append('+');
        } else {
            result.append('-');
            // get a positive offset, so that you don't need a separate code path for the negative numbers.
            offset = offset.Negate();
        }

        result.appendFormat(CultureInfo.InvariantCulture, "{0:00}:{1:00}", offset.Hours, offset.Minutes);
    }


    String DateTimeFormat::GetRealFormat(const String &format, const DateTimeFormatInfo &dtfi) {
        String realFormat = null;

        switch (format[0]) {
            case 'd':       // Short Date
                realFormat = dtfi.ShortDatePattern;
                break;
            case 'D':       // Long Date
                realFormat = dtfi.LongDatePattern;
                break;
            case 'f':       // Full (long date + short time)
                realFormat = dtfi.LongDatePattern + " " + dtfi.ShortTimePattern;
                break;
            case 'F':       // Full (long date + long time)
                realFormat = dtfi.FullDateTimePattern;
                break;
            case 'g':       // General (short date + short time)
                realFormat = dtfi.GeneralShortTimePattern;
                break;
            case 'G':       // General (short date + long time)
                realFormat = dtfi.GeneralLongTimePattern;
                break;
            case 'm':
            case 'M':       // Month/Day Date
                realFormat = dtfi.MonthDayPattern;
                break;
            case 'o':
            case 'O':
                realFormat = RoundtripFormat;
                break;
            case 'r':
            case 'R':       // RFC 1123 Standard
                realFormat = dtfi.RFC1123Pattern;
                break;
            case 's':       // Sortable without Time Zone Info
                realFormat = dtfi.SortableDateTimePattern;
                break;
            case 't':       // Short Time
                realFormat = dtfi.ShortTimePattern;
                break;
            case 'T':       // Long Time
                realFormat = dtfi.LongTimePattern;
                break;
            case 'u':       // Universal with Sortable format
                realFormat = dtfi.UniversalSortableDateTimePattern;
                break;
            case 'U':       // Universal with Full (long date + long time) format
                realFormat = dtfi.FullDateTimePattern;
                break;
            case 'y':
            case 'Y':       // Year/Month Date
                realFormat = dtfi.YearMonthPattern;
                break;
            default:
                throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
        }
        return (realFormat);
    }


// Expand a pre-defined format string (like "D" for long date) to the real format that
// we are going to use in the date time parsing.
// This method also convert the dateTime if necessary (e.g. when the format is in Universal time),
// and change dtfi if necessary (e.g. when the format should use invariant culture).
//
    String
    DateTimeFormat::ExpandPredefinedFormat(const String &format, const DateTime &dateTime,
                                           const DateTimeFormatInfo &dtfi,
                                           const TimeSpan &offset) {
        switch (format[0]) {
            case 's':       // Sortable without Time Zone Info
                dtfi = DateTimeFormatInfo.InvariantInfo;
                break;
            case 'u':       // Universal time in sortable format.
                if (offset != NullOffset) {
                    // Convert to UTC invariants mean this will be in range
                    dateTime = dateTime - offset;
                } else if (dateTime.Kind == DateTimeKind.Local) {

                    InvalidFormatForLocal(format, dateTime);
                }
                dtfi = DateTimeFormatInfo.InvariantInfo;
                break;
            case 'U':       // Universal time in culture dependent format.
                if (offset != NullOffset) {
                    // This format is not supported by DateTimeOffset
                    throw new FormatException(Environment.GetResourceString("Format_InvalidString"));
                }
                // Universal time is always in Greogrian calendar.
                //
                // Change the Calendar to be Gregorian Calendar.
                //
                dtfi = (DateTimeFormatInfo)
                        dtfi.Clone();
                if (dtfi.Calendar.GetType() != typeof(GregorianCalendar)) {
            dtfi.Calendar = GregorianCalendar.GetDefaultInstance();
        }
                dateTime = dateTime.ToUniversalTime();
                break;
        }
        format = GetRealFormat(format, dtfi);
        return (format);
    }

    String DateTimeFormat::Format(DateTime dateTime, const String &format, const DateTimeFormatInfo &dtfi) {
        return Format(dateTime, format, dtfi, NullOffset);
    }


    String
    DateTimeFormat::Format(const DateTime &dateTime, const String &format, const DateTimeFormatInfo &dtfi,
                           const TimeSpan &offset) {
        Contract.Requires(dtfi != null);
        if (format == null || format.length() == 0) {
            Boolean timeOnlySpecialCase = false;
            if (dateTime.ticks() < Calendar.TicksPerDay) {
                // If the time is less than 1 day, consider it as time of day.
                // Just print out the short time format.
                //
                // This is a workaround for VB, since they use ticks less then one day to be
                // time of day.  In cultures which use calendar other than Gregorian calendar, these
                // alternative calendar may not support ticks less than a day.
                // For example, Japanese calendar only supports date after 1868/9/8.
                // This will pose a problem when people in VB get the time of day, and use it
                // to call ToString(), which will use the general format (short date + long time).
                // Since Japanese calendar does not support Gregorian year 0001, an exception will be
                // thrown when we try to get the Japanese year for Gregorian year 0001.
                // Therefore, the workaround allows them to call ToString() for time of day from a DateTime by
                // formatting as ISO 8601 format.
                switch (dtfi.Calendar.ID) {
                    case Calendar.CAL_JAPAN:
                    case Calendar.CAL_TAIWAN:
                    case Calendar.CAL_HIJRI:
                    case Calendar.CAL_HEBREW:
                    case Calendar.CAL_JULIAN:
                    case Calendar.CAL_UMALQURA:
                    case Calendar.CAL_PERSIAN:
                        timeOnlySpecialCase = true;
                        dtfi = DateTimeFormatInfo.InvariantInfo;
                        break;
                }
            }
            if (offset == NullOffset) {
                // Default DateTime.ToString case.
                if (timeOnlySpecialCase) {
                    format = "s";
                } else {
                    format = "G";
                }
            } else {
                // Default DateTimeOffset.ToString case.
                if (timeOnlySpecialCase) {
                    format = RoundtripDateTimeUnfixed;
                } else {
                    format = dtfi.DateTimeOffsetPattern;
                }

            }

        }

        if (format.length() == 1) {
            switch (format[0]) {
                case 'O':
                case 'o':
                    return StringCache.GetStringAndRelease(FastFormatRoundtrip(dateTime, offset));
                case 'R':
                case 'r':
                    return StringCache.GetStringAndRelease(FastFormatRfc1123(dateTime, offset, dtfi));
            }

            format = ExpandPredefinedFormat(format, ref
            dateTime, ref
            dtfi, ref
            offset);
        }

        return FormatCustomized(dateTime, format, dtfi, offset);
    }

    String
    DateTimeFormat::FastFormatRfc1123(const DateTime &dateTime, const TimeSpan &offset,
                                      const DateTimeFormatInfo &dtfi) {
        // ddd, dd MMM yyyy HH:mm:ss GMT
        const int Rfc1123FormatLength = 29;
        String result = StringCache.Acquire(Rfc1123FormatLength);

        if (offset != NullOffset) {
            // Convert to UTC invariants
            dateTime = dateTime - offset;
        }

        int year, month, day;
        dateTime.GetDatePart(out
        year, out
        month, out
        day);

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
        const int roundTripFormatLength = 28;
        String result = StringCache.Acquire(roundTripFormatLength);

        int year, month, day;
        dateTime.GetDatePart(out
        year, out
        month, out
        day);

        appendNumber(result, year, 4);
        result.append('-');
        appendNumber(result, month, 2);
        result.append('-');
        appendNumber(result, day, 2);
        result.append('T');
        appendHHmmssTimeOfDay(result, dateTime);
        result.append('.');

        long fraction = dateTime.ticks() % TimeSpan::TicksPerSecond;
        appendNumber(result, fraction, 7);

        FormatCustomizedRoundripTimeZone(dateTime, offset, result);

        return result;
    }

    void DateTimeFormat::appendHHmmssTimeOfDay(String &result, const DateTime &dateTime) {
        // HH:mm:ss
        appendNumber(result, dateTime.Hour, 2);
        result.append(':');
        appendNumber(result, dateTime.Minute, 2);
        result.append(':');
        appendNumber(result, dateTime.Second, 2);
    }

    void DateTimeFormat::appendNumber(String &builder, long val, int digits) {
        for (int i = 0; i < digits; i++) {
            builder.append('0');
        }

        int index = 1;
        while (val > 0 && index <= digits) {
            builder[builder.Length - index] = (char) ('0' + (val % 10));
            val = val / 10;
            index++;
        }

        BCLDebug.Assert(val == 0, "DateTimeFormat.appendNumber(): digits less than size of val");
    }

    void DateTimeFormat::GetAllDateTimes(const DateTime &dateTime, char format, const DateTimeFormatInfo &dtfi,
                                         StringArray &result) {
        Contract.Requires(dtfi != null);
        String[]
        allFormats = null;
        String[]
        results = null;

        switch (format) {
            case 'd':
            case 'D':
            case 'f':
            case 'F':
            case 'g':
            case 'G':
            case 'm':
            case 'M':
            case 't':
            case 'T':
            case 'y':
            case 'Y':
                allFormats = dtfi.getAllDateTimePatterns(format);
                results = new String[allFormats.Length];
                for (int i = 0; i < allFormats.Length; i++) {
                    results[i] = Format(dateTime, allFormats[i], dtfi);
                }
                break;
            case 'U':
                DateTime universalTime = dateTime.toUtcTime();
                allFormats = dtfi.getAllDateTimePatterns(format);
                results = new String[allFormats.Length];
                for (int i = 0; i < allFormats.Length; i++) {
                    results[i] = Format(universalTime, allFormats[i], dtfi);
                }
                break;
                //
                // The following ones are special cases because these patterns are read-only in
                // DateTimeFormatInfo.
                //
            case 'r':
            case 'R':
            case 'o':
            case 'O':
            case 's':
            case 'u':
                results = new String[]{Format(dateTime, new String(new char[]{format}), dtfi)};
                break;
            default:
                throw new FormatException(Environment.GetResourceString("Format_InvalidString"));

        }
        return (results);
    }

    void DateTimeFormat::GetAllDateTimes(DateTime dateTime, const DateTimeFormatInfo &dtfi, StringArray &result) {
        List <String> results = new List<String>(DEFAULT_ALL_DATETIMES_SIZE);

        for (
                int i = 0;
                i < allStandardFormats.
                        Length;
                i++) {
            String[]
            strings = GetAllDateTimes(dateTime, allStandardFormats[i], dtfi);
            for (
                    int j = 0;
                    j < strings.
                            Length;
                    j++) {
                results.
                        Add(strings[j]);
            }
        }
        String[]
        value = new String[results.Count];
        results.CopyTo(0, value, 0, results.Count);
        return (value);
    }

}