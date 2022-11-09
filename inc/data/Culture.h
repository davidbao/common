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
    class NumberFormat : public IEquatable<NumberFormat>, public IEvaluation<NumberFormat>, public IComparable<NumberFormat> {
    public:
        String currencySymbol;
        bool p_cs_precedes;
        bool n_cs_precedes;
        bool p_sep_by_space;
        bool n_sep_by_space;
        int p_sign_posn;
        int n_sign_posn;

//        String decimal_point;
//        String thousands_sep;
//        String grouping;
//        String positive_sign;
//        String negative_sign;
//
//        String mon_decimal_point;
//        String mon_thousands_sep;
//        String mon_grouping;
//
//        char frac_digits;

        NumberFormat();

        ~NumberFormat() override;

        bool equals(const NumberFormat &other) const override;

        void evaluates(const NumberFormat &other) override;

        int compareTo(const NumberFormat &other) const override;
    };

    class Culture {
    public:
        class Data {
        public:
            uint lcid;
            String name;
            String region;
            String language;
            uint ansiPageCode;
            uint oemPageCode;

            Data(uint lcid = 0x0009, const String &name = "en", const String &region = "English",
                 const String &language = "English", uint ansiPageCode = 1252, uint oemPageCode = 437);

            Data(const Data &data);

            ~Data();

            Data &operator=(const Data &value);

            bool operator==(const Data &value) const;

            bool operator!=(const Data &value) const;
        };

        Culture();

        Culture(const String &name);

        Culture(uint lcid);

        Culture(const Culture &culture);

        ~Culture();

        const String &name() const;

        uint lcid() const;

        const String &region() const;

        const String &language() const;

        uint ansiPageCode() const;

        uint oemPageCode() const;

        const NumberFormat &numberFormat() const;

        Culture &operator=(const Culture &value);

        bool operator==(const Culture &value) const;

        bool operator!=(const Culture &value) const;

        bool isChinese() const;

    public:
        static const Culture &currentCulture();

    private:
        Data _data;

        NumberFormat _numberFormat;

    private:
        static Culture _current;

        static const int AllCulturesCount = 273;
        static const Culture::Data *AllCultures[AllCulturesCount];
    };
}

#endif // Culture_h
