//
//  Environment.h
//  common
//
//  Created by baowei on 2020/7/28.
//  Copyright © 2020 com. All rights reserved.
//

#ifndef Environment_h
#define Environment_h

#include "data/String.h"
#include "data/StringMap.h"

namespace System {
    class Environment {
    public:
        static void getVariables(StringMap &variables);

        static bool getVariable(const String &name, String &value);

        static bool setVariable(const String &name, const String &value, bool overwrite = true);

        static bool removeVariable(const String &name);

        static String getCurrentDirectory();

        static bool setCurrentDirectory(const String &path);

    private:
        static char **const AllEnvironments;
    };
}


#endif /* Environment_h */
