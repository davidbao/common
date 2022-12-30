//
//  Console.cpp
//  common
//
//  Created by baowei on 27/12/2016.
//  Copyright © 2016 com. All rights reserved.
//

#include "system/Console.h"

namespace System {
    const String Console::readLine(int bufferCount) {
        String result;
        char *buffer = new char[bufferCount];
        if (fgets(buffer, bufferCount, stdin) != NULL) {
            result = String(buffer).trim('\r', '\n');
        }
        delete[] buffer;
        return result;
    }

    int Console::getopt_long(int nargc, char *const *nargv, const char *options, const struct option *long_options,
                             int *idx) {
        return ::getopt_long(nargc, nargv, options, long_options, idx);
    }

    int Console::getopt_long_only(int nargc, char *const *nargv, const char *options, const struct option *long_options,
                                  int *idx) {
        return ::getopt_long_only(nargc, nargv, options, long_options, idx);
    }

    int Console::getopt(int nargc, char *const *nargv, const char *options) {
        return ::getopt(nargc, nargv, options);
    }
}
