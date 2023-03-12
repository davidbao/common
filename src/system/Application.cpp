//
//  Application.cpp
//  common
//
//  Created by baowei on 2015/4/18.
//  Copyright (c) 2015 com. All rights reserved.
//

#include <cassert>
#include <clocale>
#include "system/Application.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "thread/TickTimeout.h"
#include "thread/Timer.h"
#include "diag/FileTraceListener.h"
#include "diag/MemoryTraceListener.h"

#if WIN32

#include <Windows.h>
#include <memory.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <Shlwapi.h>
#include <Shlobj.h>

#elif __APPLE__

#include <sys/param.h>
#include <unistd.h>
#include <mach-o/dyld.h>

#else

#include <sys/vfs.h>
#include <libgen.h>
#include <climits>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#ifdef __arm_linux__
#include <linux/reboot.h>
#include <sys/reboot.h>
#endif // __arm_linux__
#endif

namespace System {
    Application *Application::_instance = nullptr;

    Application::Application(int argc, const char *argv[],
                             const String &rootPath,
                             const TraceListenerContexts &contexts,
                             const String &singleAppName) :
            _exitCode(0), _loop(true), _withQuit(false) {
#if !defined(WIN32) && !defined(PHONE_OS) && !defined(__EMSCRIPTEN__)
        signal(SIGPIPE, SIG_IGN);
//        sigset_t set;
//        sigemptyset(&set);
//        sigaddset(&set, SIGPIPE);
//        sigprocmask(SIG_BLOCK, &set, NULL);

        signal(SIGCHLD, SIG_IGN);

        signal(SIGABRT, onTerm);
        signal(SIGINT, onTerm);
        signal(SIGTERM, onTerm);
#endif

        if (!singleAppName.isNullOrEmpty()) {
            if (ProcessMutex::exists(singleAppName)) {
                ::exit(0);
            }
        }

        setlocale(LC_ALL, ".UTF-8");

        _instance = this;
        _startTime = TickTimeout::getCurrentTickCount();
        _rootPath = rootPath.isNullOrEmpty() ? Path::getAppPath() : rootPath;
        _fullFileName = Application::startupPath();

        if (argc > 0 && argv != nullptr) {
            for (int i = 0; i < argc; i++) {
                _argv.add(argv[i]);
            }
        }

        parseArguments(argc, argv);

        initLog(contexts);
    }

    Application::Application(const String &rootPath,
                             const TraceListenerContexts &contexts,
                             const String &singleAppName) :
            Application(0, nullptr, rootPath, contexts, singleAppName) {
    }

    Application::~Application() {
        _instance = nullptr;

        if (OS::isRebooting())
            Trace::writeLine("start to reboot.", Trace::Info);
        else if (OS::isPowerOffing())
            Trace::writeLine("start to power off.", Trace::Info);

        unInitLog();

        if (OS::isRebooting())
            OS::reboot();
        else if (OS::isPowerOffing())
            OS::powerOff();
    }

    Application *Application::instance() {
        if (_instance == nullptr) {
            _instance = new Application();
        }
        return _instance;
    }

    void Application::onTerm(int signum) {
        Debug::writeFormatLine("received %d signal.", signum);
        Application::instance()->exit();
    }

    const String &Application::rootPath() const {
        return _rootPath;
    }

    const Culture &Application::culture() const {
        return _culture;
    }

    void Application::setCulture(const Culture &culture) {
        if (culture != _culture) {
            _culture = culture;

            String locale = String::format("%s.UTF-8", culture.name().c_str());
            const char *result = setlocale(LC_ALL, locale);
            if (result != nullptr) {
                Trace::info(String::format("The current language is '%s'", culture.name().c_str()));
            }
        }
    }

    String Application::logPath() const {
        for (size_t i = 0; i < _traceListeners.count(); i++) {
            auto *listener = dynamic_cast<FileTraceListener *>(_traceListeners[i]);
            if (listener != nullptr) {
                return listener->context().path;
            }
        }
        return String::Empty;
    }

    String Application::logFileFilter() const {
        for (size_t i = 0; i < _traceListeners.count(); i++) {
            auto *listener = dynamic_cast<FileTraceListener *>(_traceListeners[i]);
            if (listener != nullptr) {
                return String::convert("*%s", listener->context().extName.c_str());
            }
        }
        return String::Empty;
    }

    bool Application::parseLogFileName(const String &logFileName, DateTime &date) const {
        for (size_t i = 0; i < _traceListeners.count(); i++) {
            auto *listener = dynamic_cast<FileTraceListener *>(_traceListeners[i]);
            if (listener != nullptr) {
                return listener->parseLogFileName(logFileName, date);
            }
        }
        return false;
    }

    Delegates *Application::logUpdatedDelegates() {
        for (size_t i = 0; i < _traceListeners.count(); i++) {
            auto *listener = dynamic_cast<MemoryTraceListener *>(_traceListeners[i]);
            if (listener != nullptr) {
                return listener->updatedDelegates();
            }
        }
        return nullptr;
    }

    void Application::getAllMessages(StringArray &messages) {
        for (size_t i = 0; i < _traceListeners.count(); i++) {
            auto *listener = dynamic_cast<MemoryTraceListener *>(_traceListeners[i]);
            if (listener != nullptr) {
                return listener->getAllMessages(messages);
            }
        }
    }

    void Application::initLog(const TraceListenerContexts &contexts) {
        if (contexts.count() > 0) {
            for (size_t i = 0; i < contexts.count(); i++) {
                const TraceListenerContext *context = contexts[i];
                TraceListener *listener = TraceListener::create(context);
                _traceListeners.add(listener);
                Trace::addTraceListener(listener);
            }

            Trace::enableConsoleOutput();
        }
    }

    void Application::unInitLog() {
        if (_traceListeners.count() > 0) {
            Trace::removeTraceListeners();
            Trace::disableConsoleOutput();

            _traceListeners.clear();
        }
    }

    String Application::fullFileName() const {
        return _fullFileName;
    }

    String Application::fileName() const {
        return Path::getFileName(fullFileName());
    }

    String Application::name() const {
        String fName = fileName();
        ssize_t pos = fName.find('.');
        if (pos > 0)
            return fName.substr(0, pos);
        else
            return fName;
    }

    void Application::runLoop() {
        runLoop(loopProc);
    }

    void Application::runLoop(loop_callback callback) {
        if (callback != nullptr)
            callback();
    }

    void Application::loopProc() {
#ifdef __arm_linux__
        static const int time = 100;
#else
        static const int time = 1;
#endif

        Application *app = Application::instance();
        assert(app);
        while (app->_loop) {
            Thread::msleep(time);
        }
    }

    void Application::exit(int code) {
        Trace::writeFormatLine("The application is exiting.", Trace::Info);
        _exitCode = code;
        _loop = false;

        auto timerProc = []() {
            Trace::info("Force exiting!");
            OS::reboot();
        };
        static auto *forceExitTimer = new Timer("forceExitTimer",
                                                TimeSpan::fromSeconds(30), TimeSpan::fromSeconds(30), timerProc);
        forceExitTimer->start();
    }

    void Application::exit() {
        exit(0);
    }

    int Application::exitCode() const {
        return _exitCode;
    }

    bool Application::isExiting() const {
        return !_loop;
    }

#ifndef WIN32

    void Application::daemonize() {
        pid_t pid;

        /*
         * Become a session leader to lose controlling TTY.
         */
        if ((pid = fork()) < 0) {
            perror("fork");
            ::exit(1);
        } else if (pid != 0) /* parent */
            ::exit(0);
        setsid();

        /*
         * Change the current working directory to the root.
         */
        if (chdir("/") < 0) {
            perror("chdir");
            ::exit(1);
        }

        /*
         * Attach file descriptors 0, 1, and 2 to /dev/null.
         */
        close(0);
        open("/dev/null", O_RDWR);
        dup2(0, 1);
        dup2(0, 2);
    }

#endif

    TimeSpan Application::elapsedTime() const {
        uint32_t start = _startTime;
        uint32_t end = TickTimeout::getCurrentTickCount();
        uint32_t elapsed = TickTimeout::elapsed(start, end);
        return TimeSpan::fromMilliseconds(elapsed);
    }

    DateTime Application::startTime() const {
        return DateTime::now() - elapsedTime();
    }

    const StringArray &Application::argv() const {
        return _argv;
    }

    void Application::parseArguments(int argc, const char *argv[]) {
        StringArray groups;
        for (int i = 1; i < argc; i++)  // skip application name.
        {
            String str = String(argv[i]);
            if (str == "-q" || str == "--quit") {
                _withQuit = true;
            } else if (str == "-d" || str == "--daemonize") {
#ifndef WIN32
                daemonize();
#endif
            } else {
//                if(str.length() == 2 && str[0] == '-' && Char::isLower(str[1]))
//                {
//                    _arguments.add(str[1], "true");
//                }
                ssize_t start = str.find("--");
                if (start >= 0)  // has a long argument.
                {
                    str = String::replace(str, "--", String::Empty);
                    ssize_t end = str.find('=');
                    if (end > 0) {
                        String key = str.substr(0, end);
                        String value = str.substr(end + 1, str.length() - end);
                        _arguments.add(key, value);
                    } else {
                        _arguments.add(str, String::Empty);
                    }
                } else {
                    start = str.find("-");
                    if (start >= 0)  // has a short argument.
                    {
                        str = String::replace(str, "-", String::Empty);
                        ssize_t end = str.find('=');
                        if (end > 0) {
                            String key = str.substr(0, end);
                            String value = str.substr(end + 1, str.length() - end);
                            _arguments.add(key, value);
                        } else {
                            _arguments.add(str, "true");
                        }
                    }
                }
            }
        }
    }

    const Application::Arguments &Application::arguments() const {
        return _arguments;
    }

    bool Application::withQuit() const {
        return _withQuit;
    }

    String Application::startupPath() {
        String filePath;
#if WIN32
        // We do MAX_PATH + 2 here, and request with MAX_PATH + 1, so we can handle all paths
        // up to, and including MAX_PATH size perfectly fine with String termination, as well
        // as easily detect if the file path is indeed larger than MAX_PATH, in which case we
        // need to use the heap instead. This is a work-around, since contrary to what the
        // MSDN documentation states, GetModuleFileName sometimes doesn't set the
        // ERROR_INSUFFICIENT_BUFFER error number, and we thus cannot rely on this value if
        // GetModuleFileName(0, buffer, MAX_PATH) == MAX_PATH.
        // GetModuleFileName(0, buffer, MAX_PATH + 1) == MAX_PATH just means we hit the normal
        // file path limit, and we handle it normally, if the result is MAX_PATH + 1, we use
        // heap (even if the result _might_ be exactly MAX_PATH + 1, but that's ok).
        char buffer[MAX_PATH + 2];
        memset(buffer, 0, sizeof(buffer));
        DWORD v = GetModuleFileName(0, buffer, MAX_PATH + 1);

        if (v == 0) {
            filePath = "";
        } else if (v <= MAX_PATH) {
            filePath = String(buffer);
        } else {
            // MAX_PATH sized buffer wasn't large enough to contain the full path, use heap
            char *b = 0;
            int i = 1;
            DWORD size;
            do {
                ++i;
                size = MAX_PATH * i;
                b = reinterpret_cast<char *>(realloc(b, (size + 1) * sizeof(char)));
                if (b)
                    v = GetModuleFileName(nullptr, b, size);
            } while (b && v == size);

            if (b)
                *(b + size) = 0;
            filePath = String(b);
            free(b);
        }
#elif __APPLE__
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0)
            filePath = String(path);
#elif __EMSCRIPTEN__
        return Directory::getCurrentDirectory();
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count > 0 && count < PATH_MAX) {
            result[count] = '\0';
            filePath = String(result);
        }
#endif    // WIN32

        return filePath;
    }

    OS::Actions OS::_action = OS::None;

    OS::Actions OS::action() {
        return _action;
    }

    void OS::postReboot() {
        _action = Reboot;
        Application::instance()->exit(-1);
    }

    void OS::postPowerOff() {
        _action = PowerOff;
        Application::instance()->exit(-1);
    }

    bool OS::isRebooting() {
        return _action == Reboot;
    }

    bool OS::isPowerOffing() {
        return _action == PowerOff;
    }

    void OS::reboot() {
#ifdef __arm_linux__
        sync();    // save all of files.
//        Thread::msleep(3000);
        
        if(::reboot(LINUX_REBOOT_CMD_RESTART) != 0)
        {
            Trace::writeFormatLine("reboot failed! reason: %s", strerror(errno));
            return;
        }
#else
        // todo: reboot current os.
#endif
    }

    void OS::powerOff() {
#ifdef __arm_linux__
        sync();    // save all of files.
//        Thread::msleep(3000);
        
        if(::reboot(LINUX_REBOOT_CMD_POWER_OFF) != 0)
        {
            Trace::writeFormatLine("poweroff failed! reason: %s", strerror(errno));
            return;
        }
#else
        // todo: poweroff current os.
#endif
    }
}
