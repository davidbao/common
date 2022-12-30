//
//  Trace.cpp
//  common
//
//  Created by baowei on 2016/8/3.
//  Copyright © 2016 com. All rights reserved.
//

#include "diag/FileTraceListener.h"

#if WIN32

#include <Windows.h>

#elif __ANDROID__
#include <android/log.h>
#else
#endif

#include <stdarg.h>
#include "data/DateTime.h"
#include "diag/Trace.h"

namespace Diag {
    const char *Trace::Information = "INFO";
    const char *Trace::Info = "INFO";
    const char *Trace::Error = "ERROR";
    const char *Trace::Warning = "WARN";
    const char *Trace::Verbose = "VERB";
    const char *Trace::Debug = "DEBUG";
    const char *Trace::Fatal = "FATAL";

    bool Trace::_enableLog = false;
    bool Trace::_enableConsoleOutput = true;
    bool Trace::_enableFlushConsoleOutput = false;
    bool Trace::_enableDebug = false;
    bool Trace::_colorShown = true;
    TraceListeners Trace::_traceListeners = TraceListeners(false, 10);

    void Trace::writeFormat(const char *format, ...) {
        char *message = new char[MaxMessageLength];
        memset(message, 0, MaxMessageLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);

        writeInner(message, false);
        delete[] message;
    }

    void Trace::writeFormatLine(const char *format, ...) {
        char *message = new char[MaxMessageLength];
        memset(message, 0, MaxMessageLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);

        writeInner(message, true);
        delete[] message;
    }

    void Trace::write(const String &message, const String &category, bool showTime) {
        writeInner(message.c_str(), false, category.c_str(), showTime);
    }

    void Trace::writeLine(const String &message, const String &category, bool showTime) {
        writeInner(message.c_str(), true, category.c_str(), showTime);
    }

    void Trace::writeInner(const char *message, bool newLine, const char *category, bool showTime) {
        if (message == nullptr || strlen(message) == 0)
            return;

        String str;
        if (showTime) {
            str.append(DateTime::now().toString("yyyy-MM-dd HH:mm:ss.fff"));
            str.append(" ");
        }

        String categoryStr = (category != nullptr && strlen(category) > 0) ? category : Trace::Info;
        static const size_t MaxCategoryLen = 6;
        size_t spaceLen = categoryStr.length() < MaxCategoryLen ? MaxCategoryLen - categoryStr.length() : 1;
        str.append(String::format("[%s]%*s", categoryStr.c_str(), spaceLen, ""));
        str.append(message);
        if (newLine) {
            str.append(String::NewLine);
        }

        const char *dstr = str.c_str();
#if WIN32
        String encodingStr = str;
        OutputDebugString(encodingStr);
        if (_enableConsoleOutput) {
            printf("%s", encodingStr.c_str());
            if (_enableFlushConsoleOutput)
                fflush(stdout);
        }
#elif __ANDROID__
        int prio = ANDROID_LOG_INFO;
        if (category != nullptr)
        {
            if(String::equals(Trace::Error, category))
            {
                prio = ANDROID_LOG_ERROR;
            }
            else if(String::equals(Trace::Information, category) ||
                    String::equals(Trace::Info, category))
            {
                prio = ANDROID_LOG_INFO;
            }
            else if(String::equals(Trace::Verbose, category))
            {
                prio = ANDROID_LOG_VERBOSE;
            }
            else if(String::equals(Trace::Debug, category))
            {
                prio = ANDROID_LOG_DEBUG;
            }
            else if(String::equals(Trace::Warning, category))
            {
                prio = ANDROID_LOG_WARN;
            }
            else if(String::equals(Trace::Fatal, category))
            {
                prio = ANDROID_LOG_FATAL;
            }
        }
        __android_log_print(prio, nullptr, "%s", message);
#elif __APPLE__
        printf("%s", dstr);
#elif __EMSCRIPTEN__
        printf("%s", dstr);
#else
        if(_colorShown)
        {
#define COLOR_NONE			"\033[0m"
#define FONT_COLOR_RED		"\033[0;31m"
#define FONT_COLOR_YELLOW	"\033[0;33m"
#define FONT_COLOR_GREEN	"\033[0;32m"
#define FONT_COLOR_BLUE		"\033[0;34m"
            if(String::equals(Trace::Error, category) ||
                String::equals(Trace::Fatal, category))
            {
                // red
                printf(FONT_COLOR_RED"%s" COLOR_NONE, dstr);
            }
            else if(String::equals(Trace::Warning, category))
            {
                // yellow
                printf(FONT_COLOR_YELLOW"%s" COLOR_NONE, dstr);
            }
            else if(String::equals(Trace::Information, category) ||
                String::equals(Trace::Info, category))
            {
                // green
                printf(FONT_COLOR_GREEN"%s" COLOR_NONE, dstr);
            }
            else if(String::equals(Trace::Debug, category))
            {
                // blue
                printf(FONT_COLOR_BLUE"%s" COLOR_NONE, dstr);
            }
            else
            {
                printf("%s", dstr);
            }
        }
        else
        {
            printf("%s", dstr);
        }
#endif
        // log to listeners.
        if (_enableLog) {
            for (uint32_t i = 0; i < _traceListeners.count(); i++) {
                _traceListeners[i]->write(str, category);
            }
        }
    }

    void Trace::addTraceListener(TraceListener *tl) {
        if (tl != nullptr) {
            _enableLog = true;
            _traceListeners.add(tl);
        }
    }

    void Trace::removeTraceListener(TraceListener *tl) {
        if (tl != nullptr) {
            _traceListeners.remove(tl, false);
            _enableLog = _traceListeners.count() > 0;
        }
    }

    void Trace::removeTraceListeners() {
        _enableLog = false;
        _traceListeners.clear();
    }

    void Trace::enableConsoleOutput() {
        _enableConsoleOutput = true;
    }

    void Trace::disableConsoleOutput() {
        _enableConsoleOutput = false;
    }

    void Trace::enableFlushConsoleOutput() {
        _enableFlushConsoleOutput = true;
    }

    void Trace::disableFlushConsoleOutput() {
        _enableFlushConsoleOutput = false;
    }

    const String Trace::getLenBytesStr(long length) {
        String str;
        if (length < 1024)
            str = String::convert("%ld", length);
        else if (length >= 1024 && length < 1024 * 1024)
            str.append(String::convert("%ld K", length / 1024));
        else if (length >= 1024 * 1024)
            str.append(String::convert("%ld M", length / 1024 / 1024));
        return str;
    }

    void Trace::enableDebugOutput(bool debug) {
        _enableDebug = debug;
    }

    void Trace::enableColorShown(bool colorShown) {
        _colorShown = colorShown;
    }

    void Trace::debug(const String &message) {
        if (_enableDebug)
            Trace::writeLine(message, Trace::Debug);
    }

    void Trace::error(const String &message) {
        Trace::writeLine(message, Trace::Error);
    }

    void Trace::warn(const String &message) {
        Trace::writeLine(message, Trace::Warning);
    }

    void Trace::info(const String &message) {
        Trace::writeLine(message, Trace::Info);
    }

    void Trace::verbose(const String &message) {
        Trace::writeLine(message, Trace::Verbose);
    }

    void Trace::verb(const String &message) {
        verbose(message);
    }

    void Trace::fatal(const String &message) {
        Trace::writeLine(message, Trace::Fatal);
    }

    void Debug::Assert(bool condition) {
#ifdef DEBUG
        assert(condition);
#endif
    }

    void Debug::writeFormat(const char *format, ...) {
#ifdef DEBUG
        char *message = new char[Trace::MaxMessageLength];
        memset(message, 0, Trace::MaxMessageLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);

        writeInner(message, false);
        delete[] message;
#endif
    }

    void Debug::writeFormatLine(const char *format, ...) {
#ifdef DEBUG
        char *message = new char[Trace::MaxMessageLength];
        memset(message, 0, Trace::MaxMessageLength);
        va_list ap;
                va_start(ap, format);
        vsprintf(message, format, ap);
                va_end(ap);

        writeInner(message, true);
        delete[] message;
#endif
    }

    void Debug::write(const String &message, const String &category, bool showTime) {
#ifdef DEBUG
        writeInner(message.c_str(), false, category.c_str(), showTime);
#endif
    }

    void Debug::writeLine(const String &message, const String &category, bool showTime) {
#ifdef DEBUG
        writeInner(message.c_str(), true, category.c_str(), showTime);
#endif
    }

    void Debug::writeInner(const char *message, bool newLine, const char *category, bool showTime) {
#ifdef DEBUG
#ifdef __ANDROID__
        Trace::writeInner(message, newLine, category != NULL ? category : Trace::Debug, showTime);
#else
        Trace::writeInner(message, newLine, category, showTime);
#endif    // __ANDROID__
#endif    // DEBUG
    }
}
