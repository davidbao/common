#include "system/Regex.h"
#include "diag/Trace.h"

using namespace Diag;

namespace System {
    Regex::Regex(const String &pattern) {
        _regex = nullptr;

        try {
            _regex = new regex(pattern);
        } catch (std::regex_error &e) {
            // Syntax error in the regular expression
            Debug::writeFormatLine("regex error, pattern: %s, code: %d", pattern.c_str(), e.code());
        }
    }

    Regex::~Regex() {
        if (_regex != nullptr) {
            delete _regex;
            _regex = nullptr;
        }
    }

    bool Regex::match(const String &input) const {
        if (_regex != nullptr && !input.isNullOrEmpty()) {
            cmatch m;
            try {
                return regex_search(input.c_str(), m, *_regex, regex_constants::match_not_null);
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                Debug::writeFormatLine("regex error, input: %s, code: %d", input.c_str(), e.code());
            }
        }
        return false;
    }

    bool Regex::match(const String &input, StringArray &groups) const {
        if (_regex != nullptr && !input.isNullOrEmpty()) {
            groups.clear();
            try {
                std::string str = input.c_str();
                smatch sm;
                if (regex_search(str, sm, *_regex)) {
                    int count = (int) sm.size();
                    for (int i = 1; i < count; i++) {
                        groups.add(sm[i].str());
                    }
                }
                return true;
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                Debug::writeFormatLine("regex error, input: %s, code: %d", input.c_str(), e.code());
            }
        }
        return false;
    }

    bool Regex::match(const String &input, StringMap &groups) const {
#ifdef __GNUC__
#if (__GNUC__ >= 3)
#define REGEX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define REGEX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#endif
#endif

#ifdef __linux__
#if (REGEX_GCC_VERSION >= 40900)
#define HAVE_REGEX_ITERATOR
#endif
#else
#define HAVE_REGEX_ITERATOR
#endif

#ifdef HAVE_REGEX_ITERATOR
        if (_regex != nullptr && !input.isNullOrEmpty()) {
            groups.clear();
            try {
                std::string str = input.c_str();
                auto words_begin = std::sregex_iterator(str.begin(), str.end(), *_regex);
                auto words_end = std::sregex_iterator();
                for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                    std::smatch sm = *i;
//                    assert(sm.size() == 3);
                    groups.add(sm[1].str(), sm[2].str());
                }
                return groups.count() > 0;
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                Debug::writeFormatLine("regex error, input: %s, code: %d", input.c_str(), e.code());
            }
        }
#endif
        return false;
    }

//    Regex2::Regex2(const String& pattern) : _created(false)
//    {
//        int result = regcomp(&_regex, pattern, REG_EXTENDED);
//        if (result != 0)
//        {
//            char buffer[100];
//            regerror(result, &_regex, buffer, sizeof(buffer));
//            Trace::error(String::format("%s: pattern '%s'.", buffer, pattern.c_str()));
//        }
//        else
//        {
//            _created = true;
//            _pattern = pattern;
//        }
//    }
//    Regex2::~Regex2()
//    {
//        if(_created)
//        {
//            regfree(&_regex);
//        }
//    }
//    
//    bool Regex2::match(const String& input) const
//    {
//        StringArray groups;
//        return match(input, groups);
//    }
//    
//    bool Regex2::match(const String& input, StringArray& groups) const
//    {
//        if(_created && !input.isNullOrEmpty())
//        {
//            static const int maxMathCount = 10;
//            size_t nmatch = maxMathCount;
//            regmatch_t pmatch[maxMathCount];
//            memset(&pmatch, 0, sizeof(pmatch));
//            int result = regexec(&_regex, input, nmatch, pmatch, 0);
//            if (result != 0)
//            {
//                char buffer[100];
//                regerror(result, &_regex, buffer, sizeof(buffer));
//                Trace::error(String::format("Failed to match '%s' with '%s', returning %s.", input.c_str(), _pattern.c_str(), buffer));
//                return false;
//            }
//            else
//            {
//                const char* string = input.c_str();
//                for (size_t i=1; i<nmatch; i++)
//                {
//                    regmatch_t match = pmatch[i];
//                    if(match.rm_so == -1)
//                    {
//                        break;
//                    }
//                    else
//                    {
//                        String str = input.substr(match.rm_so, match.rm_eo - match.rm_so + 1);
//                        groups.add(str);
//                    }
//                }
//                return true;
//            }
//        }
//        return false;
//    }
//    bool Regex2::match(const String& input, StringMap& groups) const
//    {
//        return false;
//    }

    bool SystemRegex::isPhoneNumber(const String &str) {
        static Regex r("^1[34578][0-9]{9}$");
        return r.match(str);
    }
}
