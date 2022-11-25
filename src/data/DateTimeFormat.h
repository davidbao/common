//
//  DateTimeFormat.h
//  common
//
//  Created by baowei on 2022/11/21.
//  Copyright © 2022 com. All rights reserved.
//

#ifndef DateTimeFormat_h
#define DateTimeFormat_h

#include "data/String.h"
#include "data/TimeSpan.h"
#include "data/DateTime.h"
#include "data/Culture.h"

namespace Common {
    /*
         Customized format patterns:
         P.S. Format in the table below is the number format used to display the pattern.

         Patterns   Format      Description                           Example
         =========  ==========  ===================================== ========
            "h"     "0"         hour (12-hour clock)w/o leading zero  3
            "hh"    "00"        hour (12-hour clock)with leading zero 03
            "hh*"   "00"        hour (12-hour clock)with leading zero 03

            "H"     "0"         hour (24-hour clock)w/o leading zero  8
            "HH"    "00"        hour (24-hour clock)with leading zero 08
            "HH*"   "00"        hour (24-hour clock)                  08

            "m"     "0"         minute w/o leading zero
            "mm"    "00"        minute with leading zero
            "mm*"   "00"        minute with leading zero

            "s"     "0"         second w/o leading zero
            "ss"    "00"        second with leading zero
            "ss*"   "00"        second with leading zero

            "f"     "0"         second fraction (1 digit)
            "ff"    "00"        second fraction (2 digit)
            "fff"   "000"       second fraction (3 digit)
            "ffff"  "0000"      second fraction (4 digit)
            "fffff" "00000"         second fraction (5 digit)
            "ffffff"    "000000"    second fraction (6 digit)
            "fffffff"   "0000000"   second fraction (7 digit)

            "F"     "0"         second fraction (up to 1 digit)
            "FF"    "00"        second fraction (up to 2 digit)
            "FFF"   "000"       second fraction (up to 3 digit)
            "FFFF"  "0000"      second fraction (up to 4 digit)
            "FFFFF" "00000"         second fraction (up to 5 digit)
            "FFFFFF"    "000000"    second fraction (up to 6 digit)
            "FFFFFFF"   "0000000"   second fraction (up to 7 digit)

            "t"                 first character of AM/PM designator   A
            "tt"                AM/PM designator                      AM
            "tt*"               AM/PM designator                      PM

            "d"     "0"         day w/o leading zero                  1
            "dd"    "00"        day with leading zero                 01
            "ddd"               short weekday name (abbreviation)     Mon
            "dddd"              full weekday name                     Monday
            "dddd*"             full weekday name                     Monday


            "M"     "0"         month w/o leading zero                2
            "MM"    "00"        month with leading zero               02
            "MMM"               short month name (abbreviation)       Feb
            "MMMM"              full month name                       Febuary
            "MMMM*"             full month name                       Febuary

            "y"     "0"         two digit year (year % 100) w/o leading zero           0
            "yy"    "00"        two digit year (year % 100) with leading zero          00
            "yyy"   "D3"        year                                  2000
            "yyyy"  "D4"        year                                  2000
            "yyyyy" "D5"        year                                  2000
            ...

            "z"     "+0;-0"     timezone offset w/o leading zero      -8
            "zz"    "+00;-00"   timezone offset with leading zero     -08
            "zzz"      "+00;-00" for hour offset, "00" for minute offset  full timezone offset   -07:30
            "zzz*"  "+00;-00" for hour offset, "00" for minute offset   full timezone offset   -08:00

            "K"    -Local       "zzz", e.g. -08:00
                   -Utc         "'Z'", representing UTC
                   -Unspecified ""
                   -DateTimeOffset      "zzzzz" e.g -07:30:15

            "g*"                the current era name                  A.D.

            ":"                 time separator                        : -- DEPRECATED - Insert separator directly into pattern (eg: "H.mm.ss")
            "/"                 date separator                        /-- DEPRECATED - Insert separator directly into pattern (eg: "M-dd-yyyy")
            "'"                 quoted string                         'ABC' will insert ABC into the formatted string.
            '"'                 quoted string                         "ABC" will insert ABC into the formatted string.
            "%"                 used to quote a single pattern characters      E.g.The format character "%y" is to print two digit year.
            "\"                 escaped character                     E.g. '\d' insert the character 'd' into the format string.
            other characters    insert the character into the format string.

        Pre-defined format characters:
            (U) to indicate Universal time is used.
            (G) to indicate Gregorian calendar is used.

            Format              Description                             Real format                             Example
            =========           =================================       ======================                  =======================
            "d"                 short date                              culture-specific                        10/31/1999
            "D"                 long data                               culture-specific                        Sunday, October 31, 1999
            "f"                 full date (long date + short time)      culture-specific                        Sunday, October 31, 1999 2:00 AM
            "F"                 full date (long date + long time)       culture-specific                        Sunday, October 31, 1999 2:00:00 AM
            "g"                 general date (short date + short time)  culture-specific                        10/31/1999 2:00 AM
            "G"                 general date (short date + long time)   culture-specific                        10/31/1999 2:00:00 AM
            "m"/"M"             Month/Day date                          culture-specific                        October 31
    (G)     "o"/"O"             Round Trip XML                          "yyyy-MM-ddTHH:mm:ss.fffffffK"          1999-10-31 02:00:00.0000000Z
    (G)     "r"/"R"             RFC 1123 date,                          "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'"   Sun, 31 Oct 1999 10:00:00 GMT
    (G)     "s"                 Sortable format, based on ISO 8601.     "yyyy-MM-dd'T'HH:mm:ss"                 1999-10-31T02:00:00
                                                                        ('T' for local time)
            "t"                 short time                              culture-specific                        2:00 AM
            "T"                 long time                               culture-specific                        2:00:00 AM
    (G)     "u"                 Universal time with sortable format,    "yyyy'-'MM'-'dd HH':'mm':'ss'Z'"        1999-10-31 10:00:00Z
                                based on ISO 8601.
    (U)     "U"                 Universal time with full                culture-specific                        Sunday, October 31, 1999 10:00:00 AM
                                (long date + long time) format
                                "y"/"Y"             Year/Month day                          culture-specific                        October, 1999

    */

    //
    // Flags used to indicate different styles of month names.
    // This is an internal flag used by internalGetMonthName().
    // Use flag here in case that we need to provide a combination of these styles
    // (such as month name of a leap year in genitive form.  Not likely for now,
    // but would like to keep the option open).
    //
    enum MonthNameStyles {
        Regular     = 0x00000000,
        Genitive    = 0x00000001,
        LeapYear    = 0x00000002,
    };

    //
    // Flags used to indicate special rule used in parsing/formatting
    // for a specific DateTimeFormatInfo instance.
    // This is an internal flag.
    //
    // This flag is different from MonthNameStyles because this flag
    // can be expanded to accommodate parsing behaviors like CJK month names
    // or alternative month names, etc.
    enum DateTimeFormatFlags {
        None                    = 0x00000000,
        UseGenitiveMonth        = 0x00000001,
        UseLeapYearMonth        = 0x00000002,
        UseSpacesInMonthNames   = 0x00000004, // Has spaces or non-breaking space in the month names.
        UseHebrewRule           = 0x00000008,   // Format/Parse using the Hebrew calendar rule.
        UseSpacesInDayNames     = 0x00000010,   // Has spaces or non-breaking space in the day names.
        UseDigitPrefixInTokens  = 0x00000020,   // Has token starting with numbers.

        NotInitialized          = -1,
    };

    class StringArray;

    class DateTimeFormat {
    public:
        static const int MaxSecondsFractionDigits = 7;
        static const TimeSpan NullOffset;

        static const int allStandardFormatsCount = 19;
        static const char allStandardFormats[allStandardFormatsCount];

        static const String RoundtripFormat;
        static const String RoundtripDateTimeUnfixed;

        static const int DEFAULT_ALL_DATETIMES_SIZE = 132;

        static const DateTimeFormatInfo InvariantFormatInfo;
        static const String *InvariantAbbreviatedMonthNames;
        static const String *InvariantAbbreviatedDayNames;
        static const String Gmt;

        static const String fixedNumberFormats[7];

        static void FormatDigits(String &outputBuffer, int value, int len);

        static void FormatDigits(String &outputBuffer, int value, int len, bool overrideLengthLimit);

        static int ParseRepeatPattern(const String &format, int pos, char patternChar);

        static String FormatDayOfWeek(int dayOfWeek, int repeat, const DateTimeFormatInfo *dtfi);

        static String FormatMonth(int month, int repeatCount, const DateTimeFormatInfo *dtfi);

        static int ParseQuoteString(const String &format, int pos, String &result);

        static int ParseNextChar(const String &format, int pos);

        static bool IsUseGenitiveForm(const String &format, int index, int tokenLen, char patternToMatch);

        static String FormatCustomized(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi,
                                       const TimeSpan &offset);

        static void
        FormatCustomizedTimeZone(const DateTime &dateTime, const TimeSpan &offset, const String &format, int tokenLen,
                                 bool timeOnly,
                                 String &result);

        // output the 'K' format, which is for round-tripping the data
        static void FormatCustomizedRoundripTimeZone(const DateTime &dateTime, const TimeSpan &offset, String &result);

        static String GetRealFormat(const String &format, const DateTimeFormatInfo *dtfi);

        static String
        ExpandPredefinedFormat(const String &format, const DateTime &dateTime, const DateTimeFormatInfo *dtfi,
                               const TimeSpan &offset);

        static String Format(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi);

        static String
        Format(const DateTime &dateTime, const String &format, const DateTimeFormatInfo *dtfi, const TimeSpan &offset);

        static String FastFormatRoundtrip(const DateTime &dateTime, const TimeSpan &offset);

        static void appendHHmmssTimeOfDay(String &result, const DateTime &dateTime);

        static void appendNumber(String &builder, int64_t val, int digits);

        static String FastFormatRfc1123(DateTime dateTime, const TimeSpan &offset, const DateTimeFormatInfo *dtfi);
    };
}

#endif // DateTimeFormat_h
