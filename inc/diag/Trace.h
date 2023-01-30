//
//  Trace.h
//  common
//
//  Created by baowei on 2016/8/3.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Trace_h
#define Trace_h

#include "data/String.h"
#include "diag/TraceListener.h"
#include <assert.h>

namespace Diag {
    class Trace {
    public:
        static void writeFormat(const char *format, ...);

        static void writeFormatLine(const char *format, ...);

        static void write(const String &message, const String &category = String::Empty, bool showTime = true);

        static void writeLine(const String &message, const String &category = String::Empty, bool showTime = true);

        static void addTraceListener(TraceListener *tl);

        static void removeTraceListener(TraceListener *tl);

        static void removeTraceListeners();

        static void enableConsoleOutput();

        static void disableConsoleOutput();

        static void enableFlushConsoleOutput();

        static void disableFlushConsoleOutput();

        static const String getLenBytesStr(long length);

        static void enableDebugOutput(bool debug = true);

        static void enableColorShown(bool colorShown);

        static void debug(const String &message);

        static void error(const String &message);

        static void warn(const String &message);

        static void info(const String &message);

        static void verbose(const String &message);

        static void verb(const String &message);

        static void fatal(const String &message);

    private:
        static void
        writeInner(const char *message, bool newLine = false, const char *category = nullptr, bool showTime = true);

    public:
        static const char *Information;        // "Information"
        static const char *Info;            // "Info"
        static const char *Error;            // "Error"
        static const char *Warning;            // "Warning"
        static const char *Verbose;            // "Verbose"
        static const char *Debug;            // "Debug"
        static const char *Fatal;            // "Fatal"

        static const int MaxMessageLength = 1024 * 2;    // 2 K

    private:
        friend class Debug;

        static bool _enableLog;
        static TraceListeners _traceListeners;
        static bool _enableConsoleOutput;
        static bool _enableFlushConsoleOutput;
        static bool _enableDebug;
        static bool _colorShown;
    };

    class Debug {
    public:
        static void Assert(bool condition);

        static void writeFormat(const char *format, ...);

        static void writeFormatLine(const char *format, ...);

        static void write(const String &message, const String &category = Trace::Debug, bool showTime = true);

        static void writeLine(const String &message, const String &category = Trace::Debug, bool showTime = true);

    private:
        static void writeInner(const char *message, bool newLine = false, const char *category = Trace::Debug,
                               bool showTime = true);
    };
}

#endif // Trace_h
