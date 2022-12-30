#include "thread/TickTimeout.h"

#if WIN32
#include <Windows.h>
#else

#include <time.h>
#include <sys/time.h>

#endif

#include "data/TimeSpan.h"
#include "system/Math.h"
#include "diag/Trace.h"

#if defined(__APPLE__)

#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#endif

namespace Threading {
    uint32_t TickTimeout::getCurrentTickCount() {
#if WIN32
        return GetTickCount();
#else
        return mono_msec_ticks();
        //struct timespec ts;
        //clock_gettime(CLOCK_MONOTONIC, &ts);
        //return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
    }

#if !WIN32
#define MTICKS_PER_SEC 10000000
#define MTICKS_PER_MICROSEC 10000

    /* Returns the number of 100ns ticks from unspecified time: this should be monotonic */
    int64_t TickTimeout::mono_100ns_ticks() {
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
        struct timespec tspec;
        static struct timespec tspec_freq = {0, 0};
        static int can_use_clock = 0;
        if (!tspec_freq.tv_nsec) {
            can_use_clock = clock_getres (CLOCK_MONOTONIC, &tspec_freq) == 0;
//                printf ("resolution: %lu.%lu\n", tspec_freq.tv_sec, tspec_freq.tv_nsec);
        }
        if (can_use_clock) {
            if (clock_gettime (CLOCK_MONOTONIC, &tspec) == 0) {
//                    printf ("time: %lu.%lu\n", tspec.tv_sec, tspec.tv_nsec);
                return ((int64_t)tspec.tv_sec * MTICKS_PER_SEC + tspec.tv_nsec / 100);
            }
        }
        return 0;
#else
        struct timeval tv;
        if (gettimeofday (&tv, nullptr) == 0)
            return ((int64_t)tv.tv_sec * 1000000 + tv.tv_usec) * 10;
        return 0;
#endif
    }

    int64_t TickTimeout::get_boot_time() {
#ifdef __APPLE__
        int mib[2];
        size_t size;
//            time_t now;
        struct timeval boottime;

//            (void)time(&now);

        mib[0] = CTL_KERN;
        mib[1] = KERN_BOOTTIME;

        size = sizeof(boottime);

        if (sysctl(mib, 2, &boottime, &size, nullptr, 0) != -1)
            return (int64_t) ((int64_t) boottime.tv_sec * MTICKS_PER_SEC +
                              (int64_t) boottime.tv_usec * MTICKS_PER_MICROSEC);

//            if (sysctl(mib, 2, &boottime, &size, nullptr, 0) != -1)
//                return (int64_t)((now - boottime.tv_sec) * MTICKS_PER_SEC);
#else
        FILE *uptime = fopen ("/proc/uptime", "r");
        if (uptime) {
            double upt;
            if (fscanf (uptime, "%lf", &upt) == 1) {
                int64_t now = mono_100ns_ticks ();
                fclose (uptime);
                return now - (int64_t)(upt * MTICKS_PER_SEC);
            }
            fclose (uptime);
        }
#endif
        /* a made up uptime of 300 seconds */
        return (int64_t) 300 * MTICKS_PER_SEC;
    }

    /* Returns the number of milliseconds from boot time: this should be monotonic */
    uint32_t TickTimeout::mono_msec_ticks() {
        static int64_t boot_time = 0;
        int64_t now;
        if (!boot_time)
            boot_time = get_boot_time();
        now = mono_100ns_ticks();
//			printf ("now: %llu (boot: %llu) ticks: %llu\n", (int64_t)now, (int64_t)boot_time, (int64_t)(now - boot_time));
        return (uint32_t) ((now - boot_time) / 10000);
    }

#endif

    bool TickTimeout::isTimeout(uint32_t start, uint32_t end, uint32_t now) {
        if (end == start) return true;
        if (end > start) {
            return now > end || now < start;
        }
        return now > end && now < start;
    }

    bool TickTimeout::isTimeout(uint32_t start, uint32_t end) {
        return isTimeout(start, end, getCurrentTickCount());
    }

    bool TickTimeout::isTimeout(uint32_t start, const TimeSpan &timeout) {
        return isTimeout(start, getDeadTickCount(start, (uint32_t) timeout.totalMilliseconds()));
    }

    uint32_t TickTimeout::getDeadTickCount(uint32_t timeout) {
        return getDeadTickCount(getCurrentTickCount(), timeout);
    }

    uint32_t TickTimeout::getDeadTickCount(uint32_t start, uint32_t timeout) {
        return getNextTickCount(start, timeout);
    }

    uint32_t TickTimeout::elapsed(uint32_t start, uint32_t end) {
        return (end >= start) ? end - start : MaxTickCount - end + start;
    }

    uint32_t TickTimeout::elapsed(uint32_t start) {
        return elapsed(start, getCurrentTickCount());
    }

    uint32_t TickTimeout::getPrevTickCount(uint32_t start, uint32_t elapsed) {
        uint32_t result;
        uint32_t curTick = start == MaxTickCount ? getCurrentTickCount() : start;
        if (curTick >= elapsed) {
            result = curTick - elapsed;
        } else {
            result = MaxTickCount - (elapsed - curTick);
        }
        return result;
    }

    uint32_t TickTimeout::getNextTickCount(uint32_t start, uint32_t elapsed) {
        uint32_t result;
        uint32_t curTick = start == MaxTickCount ? getCurrentTickCount() : start;
        if ((uint64_t) (curTick + elapsed) <= MaxTickCount) {
            result = curTick + elapsed;
        } else {
            result = elapsed - (MaxTickCount - curTick);
        }
        return result;
    }

    bool TickTimeout::sdelay(uint32_t sec, delay_callback condition, void *parameter, uint32_t sleepms) {
        return msdelay(sec * 1000, condition, parameter, sleepms);
    }

    bool TickTimeout::delay(const TimeSpan &timeout, delay_callback condition, void *parameter, uint32_t sleepms) {
        return msdelay((uint32_t) timeout.totalMilliseconds(), condition, parameter, sleepms);
    }

    bool TickTimeout::msdelay(uint32_t msec, delay_callback condition, void *parameter, uint32_t sleepms) {
        uint32_t startTime = getCurrentTickCount();
        uint32_t deadTime = getDeadTickCount(startTime, msec);

        do {
            if (condition != nullptr && condition(parameter)) {
                return true;
            }

            if (isTimeout(startTime, deadTime, getCurrentTickCount())) {
                return false;
            }

            Thread::msleep(sleepms);
        } while (true);
    }

    bool TickTimeout::delayCallback(void *parameter) {
        DelayExecution *callback = static_cast<DelayExecution *>(parameter);
        assert(callback);
        return callback->execute();
    }
}
