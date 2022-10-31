//
//  Convert.cpp
//  common
//
//  Created by baowei on 2016/9/4.
//  Copyright © 2016 com. All rights reserved.
//

#include <string.h>
#include <assert.h>
#include "data/ByteArray.h"
#include "system/Convert.h"

namespace Common {
    String Convert::convertStr(bool value) {
        return Boolean(value).toString();
    }

    String Convert::convertStr(int8_t value) {
        return Int8(value).toString();
    }

    String Convert::convertStr(uint8_t value) {
        return UInt8(value).toString();
    }

    String Convert::convertStr(int16_t value) {
        return Int16(value).toString();
    }

    String Convert::convertStr(uint16_t value) {
        return UInt16(value).toString();
    }

    String Convert::convertStr(int32_t value) {
        return Int32(value).toString();
    }

    String Convert::convertStr(uint32_t value) {
        return UInt32(value).toString();
    }

    String Convert::convertStr(int64_t value) {
        return Int64(value).toString();
    }

    String Convert::convertStr(uint64_t value) {
        return UInt64(value).toString();
    }

    String Convert::convertStr(float value, int pointSize) {
        return Float(value).toString(pointSize);
    }

    String Convert::convertStr(double value, int pointSize) {
        return Double(value).toString(pointSize);
    }

    bool Convert::parseStr(const String &text, bool &value) {
        return Boolean::parse(text, value);
    }

    bool Convert::parseStr(const String &text, int8_t &value, bool decimal) {
        return Int8::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, uint8_t &value, bool decimal) {
        return UInt8::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, int16_t &value, bool decimal) {
        return Int16::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, uint16_t &value, bool decimal) {
        return UInt16::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, int32_t &value, bool decimal) {
        return Int32::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, uint32_t &value, bool decimal) {
        return UInt32::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, int64_t &value, bool decimal) {
        return Int64::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, uint64_t &value, bool decimal) {
        return UInt64::parse(text, value, decimal);
    }

    bool Convert::parseStr(const String &text, float &value) {
        return Float::parse(text, value);
    }

    bool Convert::parseStr(const String &text, double &value) {
        return Double::parse(text, value);
    }

    void Convert::splitStr(const String &str, const char splitSymbol, StringArray &texts, const char incSymbol) {
        int size;
        String splitStr;
        String text = str;

        while (!text.isNullOrEmpty()) {
            size = (int) text.find(splitSymbol);
            if (size < 0) {
                splitStr = text;
                texts.add(text);
                return;
            }
            splitStr = text.substr(0, size);
            if (incSymbol != '\0') {
                int incSize = (int) splitStr.find(incSymbol);
                if (incSize >= 0) {
                    String splitStr2 = splitStr.substr(incSize + 1, splitStr.length() - incSize);
                    if (splitStr2.find(incSymbol) < 0) {
                        String lastStr = text.substr(size, text.length() - size);
                        int size2 = (int) lastStr.find(incSymbol);
                        if (size2 >= 0) {
                            splitStr = text.substr(0, size + size2 + 1);     // included splitSymbol
                            text = text.substr(size2 + 1, text.length() - size2);
                        }
                    }
                }
            }
            texts.add(splitStr);
            if (splitStr != text) {
                text = text.substr(size + 1, text.length() - size);
            } else {
                break;
            }
        }
    }

    void Convert::splitStr(const String &str, StringArray &texts, const char splitSymbol, const char incSymbol) {
        splitStr(str, splitSymbol, texts, incSymbol);
    }

    void Convert::splitStr(const String &str, StringArray &texts, const String &splitSymbol) {
        ssize_t size;
        String splitStr;
        String text = str;

        while (!text.isNullOrEmpty()) {
            size = text.find(splitSymbol);
            if (size < 0) {
                splitStr = text;
                texts.add(text);
                return;
            }
            splitStr = text.substr(0, size);
            texts.add(splitStr);
            if (splitStr != text) {
                size_t len = splitSymbol.length();
                text = text.substr(size + len, text.length() - size - len);
            }
        }
    }

    void Convert::splitItems(const String &str, StringArray &texts, const char splitSymbol, const char escape,
                             const char startRange, const char endRange) {
        off_t offset = 0;
        int range = 0;
        char prevCh = '\0';
        for (uint i = 0; i < str.length(); i++) {
            char ch = str[i];
            if (ch == splitSymbol && prevCh != escape) {
                if (range == 0) {
                    texts.add(str.substr(offset, i - offset));
                    offset = i + 1;     // skip ;
                }
            } else if (ch == startRange && prevCh != escape) {
                range++;
            } else if (ch == endRange && prevCh != escape) {
                range--;
            }
            prevCh = ch;
        }

        // process the last item.
        texts.add(str.substr(offset, str.length() - offset));
    }

    bool Convert::splitItems(const String &str, KeyPairs &pairs, const char splitSymbol, const char escape,
                             const char startRange, const char endRange) {
        if (str.isNullOrEmpty())
            return false;

        StringArray texts;
        Convert::splitItems(str, texts, splitSymbol, escape, startRange, endRange);
        if (texts.count() > 0) {
            for (uint i = 0; i < texts.count(); i++) {
                StringArray values;
                Convert::splitItems(texts[i], values, ':');
                if (values.count() == 2) {
                    String name = String::trim(values[0], ' ', startRange, endRange);
                    String value = String::trim(values[1], ' ', startRange, endRange);

                    if (!name.isNullOrEmpty()) {
                        KeyPair *kp = new KeyPair(name, value);
                        pairs.add(kp);
                    }
                }
            }

            return pairs.count() > 0;
        }
        return false;
    }
}
