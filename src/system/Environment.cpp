//
//  Environment.cpp
//  common
//
//  Created by baowei on 2020/7/28.
//  Copyright © 2020 com. All rights reserved.
//

#define _DCRTIMP
#include "diag/Trace.h"
#include "system/Environment.h"
#include <cstdlib>

#ifdef WIN32
#include <Windows.h>
extern char** _environ;
#else
#include <unistd.h>
extern char **environ;
#endif

using namespace Diag;

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

    String Environment::getCurrentDirectory() {
#ifdef WIN32
        char path[MAX_PATH];
        memset(path, 0, sizeof(path));
        if (GetCurrentDirectory(sizeof(path), path) > 0)
            return (String) path;
#else
        char path[PATH_MAX];
        memset(path, 0, sizeof(path));
        if (getcwd(path, sizeof(path)) != nullptr)
            return (String) path;
#endif
        return String::Empty;
    }

    bool Environment::setCurrentDirectory(const String &path) {
#ifdef WIN32
        return SetCurrentDirectory(path.c_str()) == TRUE;
#else
        bool result = chdir(path.c_str()) == 0;
#ifdef DEBUG
        if (!result) {
            Debug::writeFormatLine("chdir failed with error: %s", strerror(errno));
        }
#endif
        return result;
#endif
    }
}
