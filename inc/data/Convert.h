//
//  Convert.h
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Convert_h
#define Convert_h

#include "data/StringArray.h"
#include "data/String.h"
#include "data/ValueType.h"
#include "data/PList.h"

namespace Data {
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

        static String convertStr(float value, const String &format = String::Empty);

        static String convertStr(double value, const String &format = String::Empty);

        static bool parseStr(const String &text, bool &value);

        static bool parseStr(const String &text, int8_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, uint8_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, int16_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, uint16_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, int32_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, uint32_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, int64_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool parseStr(const String &text, uint64_t &value, NumberStyles style = NumberStyles::NSInteger);

        static bool
        parseStr(const String &text, float &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        static bool
        parseStr(const String &text, double &value, NumberStyles style = (NumberStyles) (NSFloat | NSAllowThousands));

        static void
        splitStr(const String &str, char splitSymbol, StringArray &texts, char incSymbol = '\0');

        static void
        splitStr(const String &str, StringArray &texts, char splitSymbol = ';', char incSymbol = '\0');

        static void splitStr(const String &str, StringArray &texts, const String &splitSymbol = ";");

        static void
        splitItems(const String &str, StringArray &texts, char splitSymbol = ';', char escape = '\\',
                   char startRange = '{', char endRange = '}');

        class KeyPair {
        public:
            String name;
            String value;

            explicit KeyPair(const String &name = String::Empty, const String &value = String::Empty) {
                this->name = name;
                this->value = value;
            }
        };

        typedef PList<KeyPair> KeyPairs;

        static bool
        splitItems(const String &str, KeyPairs &pairs, char splitSymbol = ';', char escape = '\\',
                   char startRange = '{', char endRange = '}');
    };
}

#endif // Convert_h
