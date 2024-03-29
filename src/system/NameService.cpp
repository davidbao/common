//
//  NameService.cpp
//  common
//
//  Created by baowei on 2018/9/21.
//  Copyright (c) 2018 com. All rights reserved.
//

#include "system/NameService.h"
#include "system/Regex.h"

namespace System {
    INameContainer::INameContainer() = default;

    INameContainer::~INameContainer() = default;

    String NameService::createNameByType(const INameContainer *container, const String &typeName) {
        String result;
        int index = 1;
        do {
            result = typeName + ((Int32) index).toString();
            index++;
        } while (container->containName(result));

        return result;
    }

    String NameService::createNameByName(const INameContainer *container, const String &name) {
        String result;
        int index = 1;
        do {
            result = createNameInner(name, index);
            index++;
        } while (container->containName(result));

        return result;
    }

    bool NameService::validate(const INameContainer *container, const String &name) {
        return container->containName(name);
    }

    bool NameService::validate(const String &name) {
        if (name.isNullOrEmpty())
            return false;

        char ch = name[0];
        if (!(Char::isLetter(ch) || ch == '_'))
            return false;

        for (char i : name) {
            if (!Char::isLetterOrDigit(i) && i != '_')
                return false;
        }

        static const char *Keywords[] = {
                "class", "struct", "const", "for", "while", "if", "else", "throw", "size_t", "off_t",
                "bool", "string", "byte", "char", "uchar", "short", "ushort", "int", "uint", "long", "ulong",
                "int8_t", "uint8_t", "int16_t", "uint16_t", "int32_t", "uint32_t", "int64_t", "uint64_t",
                nullptr
        };

        int index = 0;
        const char *keyword;
        while ((keyword = Keywords[index]) != nullptr) {
            if (name == keyword)
                return false;
        }

        static const char *ClassNames[] = {
                "String", "DateTime", "Int32", "Int16", "Int8", "UInt32", "UInt16", "UInt8",
                "Int64", "UInt64", "Boolean", "Byte", "TimeSpan", "Version",
                nullptr
        };

        const char *className;
        while ((className = ClassNames[index]) != nullptr) {
            if (name == className)
                return false;
        }

        return true;
    }

    String NameService::createNameInner(const String &name, int index) {
        String result;
        static Regex strIndexRegex("([^0-9]*)([0-9]+)");
        StringArray texts;
        if (strIndexRegex.match(name, texts) && texts.count() == 2) {
            String prefix = texts[0];
            String indexStr = texts[1];
            Int64 indexValue = 1;
            Int64::parse(indexStr, indexValue);
            result = String::convert("%s%d", prefix.c_str(), (int) indexValue + index);
        } else {
            result = String::convert("%s%d", name.c_str(), index);
        }
        return result;
    }
}
