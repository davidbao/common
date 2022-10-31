//
//  Convert.h
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Convert_h
#define Convert_h

#include <stdio.h>
#include "data/StringArray.h"
#include "data/ValueType.h"

namespace Common {
    class Convert {
    public:
        static String convertStr(bool value);

        static String convertStr(int8_t value);

        static String convertStr(uint8_t value);

        static String convertStr(int16_t value);

        static String convertStr(uint16_t value);

        static String convertStr(int32_t value);

        static String convertStr(uint32_t value);

        static String convertStr(int64_t value);

        static String convertStr(uint64_t value);

        static String convertStr(float value, int pointSize = -1);

        static String convertStr(double value, int pointSize = -1);

        static bool parseStr(const String &text, bool &value);

        static bool parseStr(const String &text, int8_t &value, bool decimal = true);

        static bool parseStr(const String &text, uint8_t &value, bool decimal = true);

        static bool parseStr(const String &text, int16_t &value, bool decimal = true);

        static bool parseStr(const String &text, uint16_t &value, bool decimal = true);

        static bool parseStr(const String &text, int32_t &value, bool decimal = true);

        static bool parseStr(const String &text, uint32_t &value, bool decimal = true);

        static bool parseStr(const String &text, int64_t &value, bool decimal = true);

        static bool parseStr(const String &text, uint64_t &value, bool decimal = true);

        static bool parseStr(const String &text, float &value);

        static bool parseStr(const String &text, double &value);

        static void
        splitStr(const String &str, const char splitSymbol, StringArray &texts, const char incSymbol = '\0');

        static void
        splitStr(const String &str, StringArray &texts, const char splitSymbol = ';', const char incSymbol = '\0');

        static void splitStr(const String &str, StringArray &texts, const String &splitSymbol = ";");

        static void
        splitItems(const String &str, StringArray &texts, const char splitSymbol = ';', const char escape = '\\',
                   const char startRange = '{', const char endRange = '}');

        class KeyPair {
        public:
            String name;
            String value;

            KeyPair(const String &name = String::Empty, const String &value = String::Empty) {
                this->name = name;
                this->value = value;
            }
        };

        typedef Vector<KeyPair> KeyPairs;

        static bool
        splitItems(const String &str, KeyPairs &pairs, const char splitSymbol = ';', const char escape = '\\',
                   const char startRange = '{', const char endRange = '}');
    };
}

#endif // Convert_h
