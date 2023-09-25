//
//  Regex.h
//  common
//
//  Created by baowei on 2015/11/6.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Regex_h
#define Regex_h

#include <regex>
#include "data/String.h"
#include "data/StringArray.h"

using namespace Data;

namespace System {
    enum RegexOptions {
        ECMAScript = 0x0000,
        icase = 0x0001,
        nosubs = 0x0002,
        optimize = 0x0004,
        collate = 0x0008,
        basic = 0x0010,
        extended = 0x0020,
        awk = 0x0040,
        grep = 0x0080,
        egrep = 0x0100,
    };

    class Regex {
    public:
        explicit Regex(const String &pattern, RegexOptions options = ECMAScript);

        ~Regex();

        bool isValid() const;

        const String &error() const;

        bool isMatch(const String &input);

        bool match(const String &input, StringArray &groups);

    private:
        regex *_regex;

        String _error;
    };

    class SystemRegex {
    public:
        static bool isPhoneNumber(const String &str);

        static bool isIdCardNo(const String &str);

        static bool isMail(const String &str);
    };
}
#endif // Regex_h
