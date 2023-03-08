//
//  Environment.h
//  common
//
//  Created by baowei on 2020/7/28.
//  Copyright (c) 2020 com. All rights reserved.
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

        static String userName();

        static String version();

        // Gets the number of milliseconds elapsed since the system started.
        static uint64_t getTickCount();

    private:
#ifndef WIN32

        /* Returns the number of 100ns ticks from unspecified time: this should be monotonic */
        static uint64_t get_100ns_ticks();

        static uint64_t get_boot_time();

        /* Returns the number of milliseconds from boot time: this should be monotonic */
        static uint64_t get_msec_ticks();

#endif
    };
}

#endif // Environment_h
