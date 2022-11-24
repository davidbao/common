//
//  Culture.h
//  common
//
//  Created by baowei on 2015/8/29.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Culture_h
#define Culture_h

#include "data/ValueType.h"

namespace Common {
    enum DigitShapes {
        DigitShapesContext = 0x0000,   // The shape depends on the previous text in the same output.
        DigitShapesNone = 0x0001,   // Gives full Unicode compatibility.
        DigitShapesNativeNational = 0x0002,   // National shapes determined by LOCALE_SNATIVEDIGITS
    };

    //
    // Property             Default Description
    // PositiveSign           '+'   Character used to indicate positive values.
    // NegativeSign           '-'   Character used to indicate negative values.
    // NumberDecimalSeparator '.'   The character used as the decimal separator.
    // NumberGroupSeparator   ','   The character used to separate groups of
    //                              digits to the left of the decimal point.
    // NumberDecimalDigits    2     The default number of decimal places.
    // NumberGroupSizes       3     The number of digits in each group to the
    //                              left of the decimal point.
    // NaNSymbol             "NaN"  The string used to represent NaN values.
    // PositiveInfinitySymbol"Infinity" The string used to represent positive
    //                              infinities.
    // NegativeInfinitySymbol"-Infinity" The string used to represent negative
    //                              infinities.
    //
    //
    //
    // Property                  Default  Description
    // CurrencyDecimalSeparator  '.'      The character used as the decimal
    //                                    separator.
    // CurrencyGroupSeparator    ','      The character used to separate groups
    //                                    of digits to the left of the decimal
    //                                    point.
    // CurrencyDecimalDigits     2        The default number of decimal places.
    // CurrencyGroupSizes        3        The number of digits in each group to
    //                                    the left of the decimal point.
    // CurrencyPositivePattern   0        The format of positive values.
    // CurrencyNegativePattern   0        The format of negative values.
    // CurrencySymbol            "$"      String used as local monetary symbol.
    //
    class NumberFormatInfo : public IFormatProvider<NumberFormatInfo> {
    public:
        int numberGroupSizes[5] = {3};
        int currencyGroupSizes[5] = {3};
        int percentGroupSizes[5] = {3};
        String positiveSign = "+";
        String negativeSign = "-";
        String numberDecimalSeparator = ".";
        String numberGroupSeparator = ",";
        String currencyGroupSeparator = ",";
        String currencyDecimalSeparator = ".";
        String currencySymbol = "¤";  // U+00a4 is the symbol for International Monetary Fund.
        String ansiCurrencySymbol;
        String nanSymbol = "NaN";
        String positiveInfinitySymbol = "Infinity";
        String negativeInfinitySymbol = "-Infinity";
        String percentDecimalSeparator = ".";
        String percentGroupSeparator = ",";
        String percentSymbol = "%";
        String perMilleSymbol = "‰";
        String nativeDigits[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        int numberDecimalDigits = 2;
        int currencyDecimalDigits = 2;
        int currencyPositivePattern = 0;
        int currencyNegativePattern = 1;
        int numberNegativePattern = 1;
        int percentPositivePattern = 0;
        int percentNegativePattern = 0;
        int percentDecimalDigits = 2;
        DigitShapes digitSubstitution = DigitShapesNone;

        NumberFormatInfo();

        ~NumberFormatInfo() override;

        const NumberFormatInfo *getFormat(const char *typeName) const override;

    public:
        static const NumberFormatInfo *getInstance(const IFormatProvider<NumberFormatInfo> *provider);

        static const NumberFormatInfo &currentInfo();
    };

    enum CalendarWeekRule {
        FirstDay = 0,           // Week 1 begins on the first day of the year
        FirstFullWeek = 1,      // Week 1 begins on first FirstDayOfWeek not before the first day of the year
        FirstFourDayWeek = 2    // Week 1 begins on first FirstDayOfWeek such that FirstDayOfWeek+3 is not before the first day of the year
    };

    enum DayOfWeek {
        Sunday = 0,
        Monday = 1,
        Tuesday = 2,
        Wednesday = 3,
        Thursday = 4,
        Friday = 5,
        Saturday = 6,
    };

    class DateTimeFormatInfo : public IFormatProvider<DateTimeFormatInfo> {
    public:
        String abbreviatedDayNames[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        String abbreviatedMonthGenitiveNames[13] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
                                                    "Oct", "Nov", "Dec", String::Empty};
        String abbreviatedMonthNames[13] = {"January", "February", "March", "April", "May", "June", "July", "August",
                                            "September", "October", "November", "December", String::Empty};
        String aMDesignator = "AM";
        CalendarWeekRule calendarWeekRule = FirstDay;
        String dateSeparator = "-";
        String dayNames[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        DayOfWeek firstDayOfWeek = Sunday;
        String fullDateTimePattern = "yyyy-MM-dd HH:mm:ss";
        String longDatePattern = "yyyy-MM-dd";
        String longTimePattern = "HH:mm:ss";
        String monthDayPattern = "MM-dd";
        String monthGenitiveNames[13] = {"January", "February", "March", "April", "May", "June", "July", "August",
                                         "September", "October", "November", "December", String::Empty};
        String monthNames[13] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
                                 "October", "November", "December", String::Empty};
        String pMDesignator = "PM";
        String rFC1123Pattern = "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'";
        String shortDatePattern = "yyyy-MM-dd";
        String shortestDayNames[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
        String shortTimePattern = "HH:mm";
        String sortableDateTimePattern = "yyyy'-'MM'-'dd'T'HH':'mm':'ss";
        String timeSeparator = ":";
        String universalSortableDateTimePattern = "yyyy'-'MM'-'dd HH':'mm':'ss'Z'";
        String yearMonthPattern = "yyyy-MM";

        DateTimeFormatInfo();

        ~DateTimeFormatInfo() override;

        const DateTimeFormatInfo *getFormat(const char *typeName) const override;

        String getAbbreviatedDayName(DayOfWeek dayofweek) const;

        String getDayName(DayOfWeek dayofweek) const;

        String getAbbreviatedMonthName(int month) const;

        String getMonthName(int month) const;

        String fullTimeSpanNegativePattern() const;

        String fullTimeSpanPositivePattern() const;

    public:
        static const DateTimeFormatInfo *getInstance(const IFormatProvider<DateTimeFormatInfo> *provider);

        static const DateTimeFormatInfo &currentInfo();
    };

    class Culture : public IEquatable<Culture>, public IEvaluation<Culture>, public IFormatProvider<NumberFormatInfo> {
    public:
        class Data : public IEquatable<Data>, public IEvaluation<Data> {
        public:
            uint32_t lcid;
            String name;
            String region;
            String language;
            uint32_t ansiPageCode;
            uint32_t oemPageCode;

            explicit Data(uint32_t lcid = 0x0009, const String &name = "en", const String &region = "English",
                          const String &language = "English", uint32_t ansiPageCode = 1252, uint32_t oemPageCode = 437);

            Data(const Data &data);

            ~Data() override;

            bool equals(const Data &other) const override;

            void evaluates(const Data &other) override;

            Data &operator=(const Data &value);
        };

        Culture();

        Culture(const String &name);

        Culture(uint32_t lcid);

        Culture(const Culture &culture);

        ~Culture() override;

        bool equals(const Culture &other) const override;

        void evaluates(const Culture &other) override;

        const NumberFormatInfo *getFormat(const char *typeName) const override;

        const String &name() const;

        uint32_t lcid() const;

        const String &region() const;

        const String &language() const;

        uint32_t ansiPageCode() const;

        uint32_t oemPageCode() const;

        const NumberFormatInfo &numberFormatInfo() const;

        const DateTimeFormatInfo &dateTimeFormatInfo() const;

        Culture &operator=(const Culture &value);

        bool isChinese() const;

    public:
        static void updateNumberFormatInfo(NumberFormatInfo *info);

    private:
        void updateNumberFormatInfo(const String &cultureName);

    public:
        static const Culture &currentCulture();

    private:
        Data _data;

        NumberFormatInfo _numberFormat;

        DateTimeFormatInfo _dateTimeFormat;

    private:
        static Culture _current;

        static const int AllCulturesCount = 273;
        static const Culture::Data *AllCultures[AllCulturesCount];
    };
}

#endif // Culture_h
