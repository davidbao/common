//
//  Metrics.cpp
//  common
//
//  Created by baowei on 2020/4/28.
//  Copyright © 2020 com. All rights reserved.
//

#include "IO/Metrics.h"
#include "data/ValueType.h"
#include "diag/Trace.h"
#include "thread/Process.h"
#include "system/Environment.h"
#include "thread/ThreadPool.h"
#include "system/Application.h"
#include <cassert>
#include <cinttypes>

#if MAC_OS

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/processor_info.h>
#include <mach/mach_host.h>
#include <unistd.h>
#include <malloc/malloc.h>

#elif WIN_OS

#include <Windows.h>
#include <strsafe.h>

#elif LINUX_OS
#include <sys/vfs.h>
#include <unistd.h>
#include <limits.h>
#include <malloc.h>
#include <dirent.h>
#include <sys/stat.h>
#elif __APPLE__
#include <sys/param.h>
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#include <unistd.h>
#include <limits.h>

#endif

#ifdef WIN_OS

//Time conversion
static __int64 file_time_2_utc(const FILETIME *ftime) {
    LARGE_INTEGER li;

    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

#endif

namespace IO {
    bool DiskStat::isDiskFull(const String &path, int maxBytes) {
        uint64_t freeSize = getFree(path);
        return freeSize < maxBytes;
    }

    uint64_t DiskStat::getTotal(const String &path) {
#if WIN32
        ULARGE_INTEGER available, total, free;
        if (GetDiskFreeSpaceEx(path.c_str(), &available, &total, &free)) {
            return total.QuadPart;
        }
#else
        struct statfs ds{};
        if (statfs(path.c_str(), &ds) >= 0) {
            return ds.f_bsize * ds.f_blocks;
        }
#endif
        return 0;
    }

    uint64_t DiskStat::getFree(const String &path) {
#if WIN32
        ULARGE_INTEGER available, total, free;
        if (GetDiskFreeSpaceEx(path.c_str(), &available, &total, &free)) {
            return free.QuadPart;
        }
#else
        struct statfs ds{};
        if (statfs(path.c_str(), &ds) >= 0) {
            return ds.f_bfree * ds.f_bsize;
        }
#endif
        return 0;
    }

    uint64_t DiskStat::getAvailable(const String &path) {
#if WIN32
        ULARGE_INTEGER available, total, free;
        if (GetDiskFreeSpaceEx(path.c_str(), &available, &total, &free)) {
            return available.QuadPart;
        }
#else
        struct statfs ds{};
        if (statfs(path.c_str(), &ds) >= 0) {
            return ds.f_bavail * ds.f_bsize;
        }
#endif
        return 0;
    }

    uint64_t DiskStat::getUsed(const String &path) {
        return getTotal(path) - getAvailable(path);
    }

#ifdef WIN_OS

    LARGE_INTEGER __GetDirectorySize(const char *szDir) {
        WIN32_FIND_DATA ffd;
        LARGE_INTEGER diretorySize;
        diretorySize.QuadPart = 0;

        HANDLE hFind = INVALID_HANDLE_VALUE;
        DWORD dwError = 0;

        char szDirAll[MAX_PATH];
        StringCchCopy(szDirAll, MAX_PATH, szDir);
        StringCchCat(szDirAll, MAX_PATH, TEXT("\\*"));

        hFind = FindFirstFile(szDirAll, &ffd);
        if (INVALID_HANDLE_VALUE == hFind) {
            return diretorySize;
        }

        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp((char *) ffd.cFileName, ".") == 0 || strcmp((char *) ffd.cFileName, "..") == 0) {
                    continue;
                }

                TCHAR szSubDir[MAX_PATH];
                StringCchCopy(szSubDir, MAX_PATH, szDir);
                StringCchCat(szSubDir, MAX_PATH, TEXT("\\"));
                StringCchCat(szSubDir, MAX_PATH, ffd.cFileName);

                LARGE_INTEGER subDirSize = __GetDirectorySize(szSubDir);
                diretorySize.QuadPart += subDirSize.QuadPart;
            } else {
                diretorySize.QuadPart += ffd.nFileSizeLow + ffd.nFileSizeHigh;
            }
        } while (FindNextFile(hFind, &ffd) != 0);

        FindClose(hFind);

        return diretorySize;
    }

#endif

    uint64_t DiskStat::getPathUsed(const String &path) {
#if MAC_OS
        char buf[1024];
        char line[256];
        FILE *f = popen(String::format("du -sk %s", path.c_str()), "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            uint64_t size = 0;
            sscanf(line, "%" PRId64 "\t%s", &size, buf);
            pclose(f);
            return size * 1024;
        }
        pclose(f);
        return 0;
#elif LINUX_OS
        char buf[1024];
        char line[256];
        FILE *f = popen(String::format("du -s --block-size=1 %s", path.c_str()), "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            uint64_t size = 0;
            sscanf(line, "%" PRId64 "\t%s", &size, buf);
            pclose(f);
            return size;
        }
        pclose(f);
        return 0;
#elif WIN_OS
        LARGE_INTEGER diretorySize = __GetDirectorySize(path.c_str());
        return diretorySize.QuadPart;
#else
        return 0;
#endif
    }

    int CPUStat::numCPUs() {
#if MAC_OS
        int numCPUs;
        int mib[2U] = {CTL_HW, HW_NCPU};
        size_t sizeOfNumCPUs = sizeof(numCPUs);
        int status = sysctl(mib, 2U, &numCPUs, &sizeOfNumCPUs, nullptr, 0U);
        if (status)
            numCPUs = 1;
        return numCPUs;
#elif LINUX_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen("cat /proc/cpuinfo |grep processor|wc -l", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int numCPUs;
            if(Int32::parse(lineStr, numCPUs))
            {
                pclose(f);
                return numCPUs;
            }
        }
        pclose(f);
        return 1;
#elif WIN_OS
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
#else
        return 1;
#endif
    }

    double CPUStat::processUsage() {
#if MAC_OS || LINUX_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen(String::format("ps -wweo args,pcpu |grep %s", Application::instance()->name().c_str()), "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim('\t', '\n');
            ssize_t index = lineStr.findLastOf(' ');
            if (index > 0) {
                String str = lineStr.substr(index, lineStr.length() - index).trim();
                double usage;
                if (Double::parse(str, usage)) {
                    pclose(f);
                    return usage;
                }
            }
        }
        pclose(f);
        return 0.0;
#elif __arm_linux__
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        String file = String::format("top -n 1 |grep %d | awk '{printf \"%%s\", $7}'",
                                     Process::getCurrentProcessId());
        FILE *f = popen(file, "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line);
            int pos = lineStr.find('%');
            if(pos > 0)
            {
                lineStr = lineStr.substr(0, pos);
                double usage;
                if(Double::parse(lineStr, usage))
                {
                    pclose(f);
                    return usage;
                }
            }
        }
        pclose(f);
        return 0.0;
#elif WIN_OS
        return 0.0;
#else
        return 0.0;
#endif
    }

    double CPUStat::usage() {
#if MAC_OS || LINUX_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen("ps -A -o %cpu | awk '{cpu += $1} END {print cpu}'", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            double usage;
            if (Double::parse(lineStr, usage)) {
                pclose(f);
                return usage;
            }
        }
        pclose(f);
        return 0.0;
#elif __arm_linux__
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen("top -n 1 |grep idle | awk '{printf \"%s\", $8}'", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line);
            int pos = lineStr.find('%');
            if(pos > 0)
            {
                lineStr = lineStr.substr(0, pos);
                double usage;
                if(Double::parse(lineStr, usage))
                {
                    pclose(f);
                    return 100.0 - usage;
                }
            }
        }
        pclose(f);
        return 0.0;
#elif WIN_OS
        //cpu quantity
        static int processor_count_ = -1;
        //Last time
        static __int64 last_time_ = 0;
        static __int64 last_system_time_ = 0;

        FILETIME now;
        FILETIME creation_time;
        FILETIME exit_time;
        FILETIME kernel_time;
        FILETIME user_time;
        __int64 system_time;
        __int64 time;
        __int64 system_time_delta;
        __int64 time_delta;
        if (processor_count_ == -1) {
            processor_count_ = numCPUs();
        }

        GetSystemTimeAsFileTime(&now);
        if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time)) {
            return -1;
        }
        system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count_;
        time = file_time_2_utc(&now);

        if ((last_system_time_ == 0) || (last_time_ == 0)) {
            last_system_time_ = system_time;
            last_time_ = time;
            return -1;
        }

        system_time_delta = system_time - last_system_time_;
        time_delta = time - last_time_;

        if (time_delta == 0)
            return -1;

        int cpu = (int) ((system_time_delta * 100 + time_delta / 2) / time_delta);
        last_system_time_ = system_time;
        last_time_ = time;
        return cpu;
#else
        return 0.0;
#endif
    }

    int ThreadStat::live() {
#if MAC_OS
        pid_t pid = Process::getCurrentProcessId();   //-- this is the process id you need info for
        task_t port;
        task_for_pid(mach_task_self(), pid, &port);

        task_info_data_t tinfo;
        mach_msg_type_number_t task_info_count;

        task_info_count = TASK_INFO_MAX;
        kern_return_t kr = task_info(port, TASK_BASIC_INFO, (task_info_t) tinfo, &task_info_count);
        if (kr != KERN_SUCCESS) {
            return 0;
        }

//        task_basic_info_t basic_info;
        thread_array_t thread_list;
        mach_msg_type_number_t thread_count;

//        thread_info_data_t thinfo;
//        mach_msg_type_number_t thread_info_count;

//        thread_basic_info_t basic_info_th;
//        uint32_t stat_thread = 0; // Mach threads

//        basic_info = (task_basic_info_t) tinfo;

        // get threads in the task
        kr = task_threads(port, &thread_list, &thread_count);
        if (kr != KERN_SUCCESS) {
            return 0;
        }
        return (int) thread_count;
#elif LINUX_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE* f = popen(String::format("ps hH p %d | wc -l", Process::getCurrentProcessId()), "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int thread_count;
            if(Int32::parse(lineStr, thread_count))
            {
                pclose(f);
                return thread_count;
            }
        }
        pclose(f);
        return 0;
#elif __arm_linux__
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        String file = String::format("cat /proc/%d/status | grep Threads | awk '{printf \"%%d\", $2}'", Process::getCurrentProcessId());
        FILE* f = popen(file, "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int thread_count;
            if(Int32::parse(lineStr, thread_count))
            {
                pclose(f);
                return thread_count;
            }
        }
        pclose(f);
        return 0;
#else
        return 0;
#endif
    }

    int ThreadStat::peak() {
        static int maxCount = 0;
        int count = live();
        if (count > maxCount)
            maxCount = count;
        return maxCount;
    }

    int ThreadStat::daemon() {
        return ThreadPool::threadCount();
    }

    int64_t MemoryStat::systemMax() {
#if defined(_WIN32) && (defined(__CYGWIN__) || defined(__CYGWIN32__))
        /* Cygwin under Windows. ------------------------------------ */
        /* New 64-bit MEMORYSTATUSEX isn't available.  Use old 32.bit */
        MEMORYSTATUS status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatus( &status );
        return (size_t)status.dwTotalPhys;

#elif defined(_WIN32)
        /* Windows. ------------------------------------------------- */
        /* Use new 64-bit MEMORYSTATUSEX, not old 32-bit MEMORYSTATUS */
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return (size_t) status.ullTotalPhys;

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
        /* UNIX variants. ------------------------------------------- */
        /* Prefer sysctl() over sysconf() except sysctl() HW_REALMEM and HW_PHYSMEM */

#if defined(CTL_HW) && (defined(HW_MEMSIZE) || defined(HW_PHYSMEM64))
        int mib[2];
        mib[0] = CTL_HW;
#if defined(HW_MEMSIZE)
        mib[1] = HW_MEMSIZE;            /* OSX. --------------------- */
#elif defined(HW_PHYSMEM64)
        mib[1] = HW_PHYSMEM64;          /* NetBSD, OpenBSD. --------- */
#endif
        int64_t size = 0;               /* 64-bit */
        size_t len = sizeof(size);
        if (sysctl(mib, 2, &size, &len, nullptr, 0) == 0)
            return (int64_t) size;
        return 0L;            /* Failed? */

#elif defined(_SC_AIX_REALMEM)
        /* AIX. ----------------------------------------------------- */
        return (size_t)sysconf( _SC_AIX_REALMEM ) * (size_t)1024L;

#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
        /* FreeBSD, Linux, OpenBSD, and Solaris. -------------------- */
        return (size_t)sysconf( _SC_PHYS_PAGES ) *
            (size_t)sysconf( _SC_PAGESIZE );

#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGE_SIZE)
        /* Legacy. -------------------------------------------------- */
        return (size_t)sysconf( _SC_PHYS_PAGES ) *
            (size_t)sysconf( _SC_PAGE_SIZE );

#elif defined(CTL_HW) && (defined(HW_PHYSMEM) || defined(HW_REALMEM))
        /* DragonFly BSD, FreeBSD, NetBSD, OpenBSD, and OSX. -------- */
        int mib[2];
        mib[0] = CTL_HW;
#if defined(HW_REALMEM)
        mib[1] = HW_REALMEM;        /* FreeBSD. ----------------- */
#elif defined(HW_PYSMEM)
        mib[1] = HW_PHYSMEM;        /* Others. ------------------ */
#endif
        unsigned int size = 0;        /* 32-bit */
        size_t len = sizeof( size );
        if ( sysctl( mib, 2, &size, &len, nullptr, 0 ) == 0 )
            return (size_t)size;
        return 0L;            /* Failed? */
#endif /* sysctl and sysconf variants */

#else
        return 0L;            /* Unknown OS. */
#endif
    }

    int64_t MemoryStat::systemUsed() {
#if MAC_OS
        mach_port_t host_port;
        mach_msg_type_number_t host_size;
        vm_size_t pagesize;

        host_port = mach_host_self();
        host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
        host_page_size(host_port, &pagesize);

        vm_statistics_data_t vm_stat;

        if (host_statistics(host_port, HOST_VM_INFO, (host_info_t) &vm_stat, &host_size) != KERN_SUCCESS)
            Trace::error("Failed to fetch vm statistics");

        /* Stats in bytes */
        int64_t mem_used = (vm_stat.active_count +
                            vm_stat.inactive_count +
                            vm_stat.wire_count) * (int64_t) pagesize;
//        int64_t mem_free = vm_stat.free_count * (int64_t)pagesize;
//        int64_t mem_total = mem_used + mem_free;
        return mem_used;
#elif LINUX_OS || __arm_linux__
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen("free -b |grep Mem: | awk '{printf \"%s\", $3}'", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int64_t used;
            if(Int64::parse(lineStr, used))
            {
                pclose(f);
                return used;
            }
        }
        pclose(f);
        return 0;
#else
        return 0;
#endif
    }

#ifdef LINUX_OS
    // return bytes.
    int64_t MemoryStat::getProcStatusValue(const char* name)
    {
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        String file = String::format("cat /proc/%d/status | grep %s | awk '{printf \"%%s\", $2}'",
                                     Process::getCurrentProcessId(),
                                     name);
        FILE *f = popen(file, "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int64_t value;
            if(Int64::parse(lineStr, value))
            {
                pclose(f);
                return value * 1024;
            }
        }
        pclose(f);
        return 0;
    }
    // return bytes.
    int64_t MemoryStat::getProcStatmValue(ProcStatm statm)
    {
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        String file = String::format("cat /proc/%d/statm | awk '{printf \"%%s\", $%d}'",
                                     Process::getCurrentProcessId(),
                                     statm + 1);
        FILE *f = popen(file, "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = String(line).trim(' ', '\t', '\n');
            int64_t value;
            if(Int64::parse(lineStr, value))
            {
                pclose(f);
                return value * 4096;
            }
        }
        pclose(f);
        return 0;
    }
#endif

    int64_t MemoryStat::max() {
#if MAC_OS
        return 0;
#elif LINUX_OS
        return getProcStatusValue("VmPeak") * 1024;
#else
        return 0;
#endif
    }

    int64_t MemoryStat::maxHeap() {
        return systemMax();
    }

    int64_t MemoryStat::maxNonHeap() {
        return 0;
    }

    int64_t MemoryStat::used() {
#if MAC_OS
        task_vm_info_data_t vmInfo;
        mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
        kern_return_t result = task_info(mach_task_self(), TASK_VM_INFO, (task_info_t) &vmInfo, &count);
        if (result != KERN_SUCCESS)
            return 0;
        return static_cast<int64_t>(vmInfo.phys_footprint);
#elif LINUX_OS
        return getProcStatmValue(ProcStatm::Resident) - getProcStatmValue(ProcStatm::Shared);
#else
        return 0;
#endif
    }

    int64_t MemoryStat::usedHeap() {
#if MAC_OS
        struct mstats ms = mstats();
        return (int64_t) ms.bytes_used;
#elif LINUX_OS
        struct mallinfo mi = mallinfo();
        return mi.uordblks;
#else
        return 0;
#endif
    }

    int64_t MemoryStat::usedNonHeap() {
        int64_t value = used() - usedHeap();
        return value >= 0 ? value : 0;
    }

    int64_t MemoryStat::committed() {
        return 0;
    }

    int64_t MemoryStat::committedHeap() {
#if MAC_OS
        struct mstats ms = mstats();
        return (int64_t) ms.bytes_total;
#elif LINUX_OS
        struct mallinfo mi = mallinfo();
        return mi.arena;
#else
        return 0;
#endif
    }

    int64_t MemoryStat::committedNonHeap() {
        return 0;
    }

    void MemoryStat::logUsed() {
        int64_t used = MemoryStat::used();
        String str = Double(((double) used) / 1024.0 / 1024.0).toString("0.0");
        Trace::verb(String::format("Current used memory, %s M", str.c_str()));
    }

    String UserStat::country() {
        return String::Empty;
    }

    String UserStat::countryFormat() {
        return String::Empty;
    }

    String UserStat::dir() {
        return Application::startupPath();
    }

    String UserStat::name() {
        String name;
        Environment::getVariable("USER", name);
#if LINUX_OS
        return name.isNullOrEmpty() ? "root" : name;
#else
        return name;
#endif
    }

    String UserStat::language() {
        return String::Empty;
    }

    String UserStat::timezone() {
#if MAC_OS
        String result;
        char str[PATH_MAX];
        ssize_t count = readlink("/etc/localtime", str, PATH_MAX);
        if (count > 0 && count < PATH_MAX) {
            str[count] = '\0';
            result = str;
            result = result.replace("/var/db/timezone/zoneinfo/", String::Empty);
        }
        return result;
#elif LINUX_OS
        String result;
        char str[PATH_MAX];
        ssize_t count = readlink("/etc/localtime", str, PATH_MAX);
        if (count > 0 && count < PATH_MAX)
        {
            str[count] = '\0';
            result = str;
            result = result.replace("/usr/share/zoneinfo/", String::Empty);
        }
        return result;
#elif WIN_OS
        return "Not supported";
#else
        return "Not supported";
#endif
    }

    String UserStat::home() {
        String name;
        Environment::getVariable("HOME", name);
        return name;
    }

    String OSStat::arch() {
#if OS_X64
        return "x86_64";
#else
        return "x86";
#endif
    }

    String OSStat::name() {
#if MAC_OS
        return "Mac OS X";
#elif LINUX_OS
        return "Linux";
#elif WIN_OS
        return "Windows";
#elif BROWSER_OS
        return "WebAssembly";
#else
        return "Not supported";
#endif
    }

    String OSStat::version() {
#if MAC_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE *f = popen("sw_vers | grep ProductVersion: | awk '{printf \"%s\", $2}'", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String version = String(line).trim(' ', '\t', '\n');
            return version;
        }
        pclose(f);
        return String::Empty;
#elif LINUX_OS
        char buf[1024];
        unsigned buflen = 0;
        char line[256];
        FILE* f = popen("cat /proc/version", "r");
        while (fgets(line, sizeof(line), f) != nullptr) {
            int l = snprintf(buf + buflen, sizeof(buf) - buflen, "%s", line);
            buflen += l;
            assert(buflen < sizeof(buf));
            String lineStr = line;
            if(lineStr.find("Linux version ") >= 0)
            {
                pclose(f);
                lineStr = lineStr.replace("Linux version ", String::Empty);
                int end = lineStr.find(' ');
                if(end > 0)
                {
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
}
