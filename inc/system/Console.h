//
//  Console.h
//  common
//
//  Created by baowei on 27/12/2016.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef common_Console_h
#define common_Console_h

#include "data/ValueType.h"
#include "data/StringArray.h"
#include "thread/ProcessMutex.h"
#include "data/Culture.h"
#include "data/DateTime.h"
#include "Delegate.h"
#ifdef WIN32
#include "getopt.h"
#else
#include <getopt.h>
#endif

namespace Common
{
    class Console
    {
    public:
        static const String readLine(int bufferCount = 1024);
        
        static int getopt_long(int, char * const *, const char *, const struct option *, int *);
        static int getopt_long_only(int, char * const *, const char *, const struct option *, int *);
        static int getopt(int, char * const [], const char *);
    };
}

#endif /* common_Console_h */
