//
//  Environment.cpp
//  common
//
//  Created by baowei on 2020/7/28.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "diag/Trace.h"
#include "system/Environment.h"
#include <cstdlib>

#ifdef WIN32

#include <Windows.h>

_DCRTIMP char **_environ;
#define environ _environ

#else

#include <unistd.h>
#include <ctime>
#include <sys/time.h>

#endif // WIN32

#ifdef __APPLE__

#include <sys/sysctl.h>
#include <mach/mach_time.h>

extern char **environ;

#endif // __APPLE__

using namespace Diag;

namespace System {
    void Environment::getVariables(StringMap &variables) {
//        static const Regex regex("^(\\S+)=(\\S+)$");

        for (int i = 0; environ[i] != nullptr; i++) {
            const char *env = environ[i];
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
        if (putenv(str) == 0) {
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

    String Environment::userName() {
        String name;
#ifdef WIN_OS
        Environment::getVariable("USERNAME", name);
#elif __linux__
        Environment::getVariable("USER", name);
        if (name.isNullOrEmpty()) {
            name = "root";
        }
#else
        Environment::getVariable("USER", name);
#endif
        return name;
    }

    String Environment::version() {
#if MAC_OS
        char buf[1024];
        unsigned len = 0;
        char line[256];
        FILE *f = popen("sw_vers | grep ProductVersion: | awk '{printf \"%s\", $2}'", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + len, sizeof(buf) - len, "%s", line);
            len += l;
            assert(len < sizeof(buf));
            String version = String(line).trim(' ', '\t', '\n');
            return version;
        }
        pclose(f);
        return String::Empty;
#elif __linux__
        char buf[1024];
        unsigned len = 0;
        char line[256];
        FILE *f = popen("cat /proc/version", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + len, sizeof(buf) - len, "%s", line);
            len += l;
            assert(len < sizeof(buf));
            String lineStr = line;
            if (lineStr.find("Linux version ") >= 0) {
                pclose(f);
                lineStr = lineStr.replace("Linux version ", String::Empty);
                ssize_t end = lineStr.find(' ');
                if (end > 0) {
                    lineStr = lineStr.substr(0, end);
                    lineStr = lineStr.trim(' ', '\t', '\n');
                    return lineStr;
                }
            }
        }
        pclose(f);
        return String::Empty;
#elif WIN_OS
        return "Not supported";
#else
        return "Not supported";
#endif
    }

    uint64_t Environment::getTickCount() {
#ifdef WIN32
#if (_WIN32_WINNT >= 0x0600) // Windows Vista
        return GetTickCount64();
#else
        return GetTickCount();
#endif
#else
        return get_msec_ticks();
#endif
    }

#ifndef WIN32
#define MTICKS_PER_SEC 10000000
#define MTICKS_PER_MICROSEC 10000

    /* Returns the number of 100ns ticks from unspecified time: this should be monotonic */
    uint64_t Environment::get_100ns_ticks() {
#ifdef __APPLE__
        /* http://developer.apple.com/library/mac/#qa/qa1398/_index.html */
        static mach_timebase_info_data_t timebase;
        uint64_t now = mach_absolute_time();
        if (timebase.denom == 0) {
            mach_timebase_info(&timebase);
            timebase.denom *= 100; /* we return 100ns ticks */
        }
        return now * timebase.numer / timebase.denom;
#elif __linux__
        struct timespec tspec{};
        static struct timespec tspec_freq = {0, 0};
        static int can_use_clock = 0;
        if (!tspec_freq.tv_nsec) {
            can_use_clock = clock_getres(CLOCK_MONOTONIC, &tspec_freq) == 0;
//                printf ("resolution: %lu.%lu\n", tspec_freq.tv_sec, tspec_freq.tv_nsec);
        }
        if (can_use_clock) {
            if (clock_gettime(CLOCK_MONOTONIC, &tspec) == 0) {
//                    printf ("time: %lu.%lu\n", tspec.tv_sec, tspec.tv_nsec);
                return ((uint64_t) tspec.tv_sec * MTICKS_PER_SEC + tspec.tv_nsec / 100);
            }
        }
        return 0;
#else
        struct timeval tv;
        if (gettimeofday(&tv, nullptr) == 0)
            return ((uint64_t) tv.tv_sec * 1000000 + tv.tv_usec) * 10;
        return 0;
#endif
    }

    uint64_t Environment::get_boot_time() {
#ifdef __APPLE__
        int mib[2];
        size_t size;
        struct timeval boottime{};

        mib[0] = CTL_KERN;
        mib[1] = KERN_BOOTTIME;

        size = sizeof(boottime);

        if (sysctl(mib, 2, &boottime, &size, nullptr, 0) != -1)
            return (uint64_t) ((uint64_t) boottime.tv_sec * MTICKS_PER_SEC +
                               (uint64_t) boottime.tv_usec * MTICKS_PER_MICROSEC);
#else
        FILE *uptime = fopen("/proc/uptime", "r");
        if (uptime) {
            double upt;
            if (fscanf(uptime, "%lf", &upt) == 1) {
                uint64_t now = get_100ns_ticks();
                fclose(uptime);
                return now - (int64_t) (upt * MTICKS_PER_SEC);
            }
            fclose(uptime);
        }
#endif
        /* a made up uptime of 300 seconds */
        return (uint64_t) 300 * MTICKS_PER_SEC;
    }

    /* Returns the number of milliseconds from boot time: this should be monotonic */
    uint64_t Environment::get_msec_ticks() {
        static uint64_t boot_time = 0;
        uint64_t now;
        if (!boot_time)
            boot_time = get_boot_time();
        now = get_100ns_ticks();
//			printf ("now: %llu (boot: %llu) ticks: %llu\n", (int64_t)now, (int64_t)boot_time, (int64_t)(now - boot_time));
        return (uint64_t) ((now - boot_time) / 10000);
    }

#endif
}
