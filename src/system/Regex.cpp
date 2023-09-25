//
//  Regex.cpp
//  common
//
//  Created by baowei on 2015/11/6.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "system/Regex.h"
#include "diag/Trace.h"

using namespace Diag;

namespace System {
    Regex::Regex(const String &pattern, RegexOptions options) : _regex(nullptr) {
        if (!pattern.isNullOrEmpty()) {
            auto convertOption = [](RegexOptions options) {
                static const int Count = 10;
                static const RegexOptions allOptions[Count] = {
                        ECMAScript, icase, nosubs, optimize, collate,
                        basic, extended, awk, grep, egrep
                };
                static const regex_constants::syntax_option_type allTypes[Count] = {
                        regex::ECMAScript, regex::icase, regex::nosubs, regex::optimize, regex::collate,
                        regex::basic, regex::extended, regex::awk, regex::grep, regex::egrep
                };
                regex_constants::syntax_option_type flag = regex::ECMAScript;
                for (int i = 0; i < Count; ++i) {
                    if (options & allOptions[i]) {
                        flag |= allTypes[i];
                    }
                }
                return flag;
            };

            try {
                _regex = new regex(pattern, convertOption(options));
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                _error = e.what();
                Debug::writeFormatLine("regex error, pattern: %s, code: %d", pattern.c_str(), e.code());
            }
        }
    }

    Regex::~Regex() {
        if (_regex != nullptr) {
            delete _regex;
            _regex = nullptr;
        }
    }

    bool Regex::isValid() const {
        return _regex != nullptr;
    }

    const String &Regex::error() const {
        return _error;
    }

    bool Regex::isMatch(const String &input) {
        if (isValid() && !input.isNullOrEmpty()) {
            cmatch m;
            try {
                return regex_search(input.c_str(), m, *_regex, regex_constants::match_not_null);
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                _error = e.what();
                Debug::writeFormatLine("regex error, input: %s, code: %d", input.c_str(), e.code());
            }
        }
        return false;
    }

    bool Regex::match(const String &input, StringArray &groups) {
        if (isValid() && !input.isNullOrEmpty()) {
            try {
                std::string str = input.c_str();
                smatch sm;
                if (regex_search(str, sm, *_regex)) {
                    if (sm.size() == 1) {
                        groups.add(sm[0].str());
                    } else {
                        for (size_t i = 1; i < sm.size(); ++i) {
                            groups.add(sm[i].str());
                        }
                    }
                }
                return true;
            } catch (std::regex_error &e) {
                // Syntax error in the regular expression
                _error = e.what();
                Debug::writeFormatLine("regex error, input: %s, code: %d", input.c_str(), e.code());
            }
        }
        return false;
    }

    bool SystemRegex::isPhoneNumber(const String &str) {
        static Regex r("^1[34578][0-9]{9}$");
        return r.isMatch(str);
    }

    bool SystemRegex::isIdCardNo(const String &str) {
        static Regex r(R"(^\d{17}(\d|X|x)$)");
        return r.isMatch(str);
    }

    bool SystemRegex::isMail(const String &str) {
        static Regex r(R"(^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$)");
        return r.isMatch(str);
    }
}
