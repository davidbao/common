#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <regex>
#include "data/ValueType.h"
#include "data/StringArray.h"
#include "data/Dictionary.h"

#ifndef REG_NOERROR
#define REG_NOERROR 0
#endif

namespace Common
{
    class Regex
    {
    public:
        Regex(const String& pattern);
        ~Regex();
        
        bool match(const String& input) const;
        bool match(const String& input, StringArray& groups) const;
        bool match(const String& input, StringMap& groups) const;
        
    private:
        regex* _regex;
    };

//    class Regex2
//    {
//    public:
//        Regex2(const String& pattern);
//        ~Regex2();
//        
//        bool match(const String& input) const;
//        bool match(const String& input, StringArray& groups) const;
//        bool match(const String& input, StringMap& groups) const;
//        
//    private:
//        regex_t _regex;
//        bool _created;
//        String _pattern;
//    };
    
    class SystemRegex
    {
    public:
        static bool isPhoneNumber(const String& str);
    };
}
#endif // REGEX_H
