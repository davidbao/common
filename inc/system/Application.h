#ifndef APPLICATION_H
#define APPLICATION_H

#include <assert.h>

#ifndef WIN32

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#endif

#include "diag/TraceListener.h"
#include "diag/Trace.h"
#include "thread/Mutex.h"
#include "data/String.h"
#include "data/StringMap.h"
#include "diag/ProcessMutex.h"
#include "thread/Thread.h"
#include "data/Culture.h"
#include "data/DateTime.h"
#include "Delegate.h"

using namespace Data;
using namespace Diag;

namespace System {
    class Application {
    public:
        typedef void (*loop_callback)();

        typedef StringMap Arguments;

        Application(int argc, const char *argv[],
                    const String &rootPath = String::Empty,
                    const TraceListenerContexts &contexts = TraceListenerContexts::Empty,
                    const String &singleAppName = String::Empty);

        Application(const String &rootPath = String::Empty,
                    const TraceListenerContexts &contexts = TraceListenerContexts::Empty,
                    const String &singleAppName = String::Empty);

        virtual ~Application();

        const String &rootPath() const;

        const Culture &culture() const;

        void setCulture(const Culture &culture);

        String fullFileName() const;

        String fileName() const;

        virtual String name() const;

        String logPath() const;

        String logFileFilter() const;

        bool parseLogFileName(const String &logFileName, DateTime &date) const;

        Delegates *logUpdatedDelegates();   // for MemoryTraceListener
        void getAllMessages(StringArray &messages);

        virtual void runLoop();

        virtual void runLoop(loop_callback callback);

        virtual void exit(int code);

        void exit();

        int exitCode() const;

        TimeSpan elapsedTime() const;

        DateTime startTime() const;

        const StringArray &argv() const;

        bool isExiting() const;

        const Arguments &arguments() const;

        void addArgument(const String &key, const String &value);

        bool withQuit() const;

    public:
#ifndef WIN32

        static void daemonize();

#endif

        static Application *instance();

        static String startupPath();

    protected:
        void initLog(const TraceListenerContexts &contexts);

        void unInitLog();

        void parseArguments(int argc, const char *argv[]);

    private:
        static void onTerm(int signum);

        static void loopProc();

    protected:
        String _rootPath;
        String _fullFileName;

        TraceListeners _traceListeners;
        Culture _culture;

        uint32_t _startTime;

    private:
        int _exitCode;
        bool _loop;

        StringArray _argv;

        Arguments _arguments;

        bool _withQuit;

    protected:
        static Application *_instance;
    };

    class OS {
    public:
        enum Actions {
            None,
            Reboot,
            PowerOff
        };

    public:
        static Actions action();

        static void postReboot();

        static void postPowerOff();

        static bool isRebooting();

        static bool isPowerOffing();

        static void reboot();

        static void powerOff();

    private:
        static Actions _action;
    };
}

#endif // APPLICATION_H
