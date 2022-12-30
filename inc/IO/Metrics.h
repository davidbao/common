#ifndef METRICS_H
#define METRICS_H

#include "data/ValueType.h"

#if WIN32
#undef min
#undef max
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#endif // WIN32

using namespace Data;

namespace IO {
    class DiskStat {
    public:
        // 10 M;
        static bool isDiskFull(const String &path, int maxBytes = 10 * 1024 * 1024);

        static uint64_t getTotal(const String &path);

        static uint64_t getFree(const String &path);

        static uint64_t getAvailable(const String &path);

        static uint64_t getUsed(const String &path);

        static uint64_t getPathUsed(const String &path);
    };

    class CPUStat {
    public:
        static int numCPUs();

        static double processUsage();

        static double usage();
    };

    class ThreadStat {
    public:
        static int live();

        static int peak();

        static int daemon();
    };

    class MemoryStat {
    public:
        static int64_t systemMax();

        static int64_t systemUsed();

        static int64_t max();

        static int64_t maxHeap();

        static int64_t maxNonHeap();

        static int64_t used();

        static int64_t usedHeap();

        static int64_t usedNonHeap();

        static int64_t committed();

        static int64_t committedHeap();

        static int64_t committedNonHeap();

        static void logUsed();

    private:
        enum ProcStatm {
            Size = 0,
            Resident,
            Shared,
            Trs,
            Lrs,
            Drs,
            dt,
        };

    private:
#ifdef LINUX_OS
        static int64_t getProcStatusValue(const char* name);
        static int64_t getProcStatmValue(ProcStatm statm);
#endif
    };

    class UserStat {
    public:
        static String country();

        static String countryFormat();

        static String dir();

        static String name();

        static String language();

        static String timezone();

        static String home();
    };

    class OSStat {
    public:
        static String arch();

        static String name();

        static String version();
    };
}

#endif // METRICS_H
