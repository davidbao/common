//
//  Environment.cpp
//  common
//
//  Created by baowei on 2020/7/28.
//  Copyright © 2020 com. All rights reserved.
//

#define _DCRTIMP
#include "system/Environment.h"
//#include "system/Regex.h"
#include <cstdlib>

#ifndef WIN32
extern char **environ;
#else
extern char** _environ;
#endif

namespace System {
#ifndef WIN32
    char **const Environment::AllEnvironments = environ;
#else
    char** const Environment::AllEnvironments = _environ;
#endif

    void Environment::getVariables(StringMap &variables) {
//        static const Regex regex("^(\\S+)=(\\S+)$");

        for (int i = 0; AllEnvironments[i] != nullptr; i++) {
            const char *env = AllEnvironments[i];
            StringArray texts;
            StringArray::parse(env, texts, '=');
            if (texts.count() == 2) {
                String key = texts[0].trim();
                String value = texts[1].trim();
                variables.add(key, value);
            }
//            StringMap groups;
//            if (regex.match(env, groups)) {
//                variables.addRange(groups);
//            }
        }
    }

    bool Environment::getVariable(const String &name, String &value) {
        const char *result = getenv(name);
        if (result != nullptr) {
            value = result;
            return true;
        }
        return false;
    }

    bool Environment::setVariable(const String &name, const String &value, bool overwrite) {
        if (!overwrite) {
            String v;
            if (getVariable(name, v))
                return false;
        }

#ifdef WIN32
        String str = String::format("%s=%s", name.c_str(), value.c_str());
        if (putenv(str) == 0)
        {
            return true;
        }
        return false;
#else
        if (setenv(name, value, overwrite ? 1 : 0) == 0) {
            return true;
        }
        return false;
#endif
    }

    bool Environment::removeVariable(const String &name) {
#ifdef WIN32
        return false;
#else
        return unsetenv(name) == 0;
#endif
    }
}
