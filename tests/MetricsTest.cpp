//
//  MetricsTest.cpp
//  common
//
//  Created by baowei on 2023/1/23.
//  Copyright © 2023 com. All rights reserved.
//

#include "IO/Metrics.h"
#include "IO/Path.h"

using namespace IO;

static const uint64_t OneMB = 1 * 1024 * 1024;

bool testDiskStat() {
    String path;
#ifdef WIN32
    path = "C:\\";
#else
    path = "/";
#endif
    if (DiskStat::isDiskFull(path)) {
        return false;
    }

    if (DiskStat::getTotal(path) < 100 * OneMB) {
        printf("failed DiskStat::getTotal\n");
        return false;
    }

    if (DiskStat::getFree(path) < 10 * OneMB) {
        printf("failed DiskStat::getFree\n");
        return false;
    }

    if (DiskStat::getAvailable(path) < 10 * OneMB) {
        printf("failed DiskStat::getAvailable\n");
        return false;
    }

    if (DiskStat::getUsed(path) < 10 * OneMB) {
        printf("failed DiskStat::getUsed\n");
        return false;
    }

#ifndef __EMSCRIPTEN__
    if (DiskStat::getPathUsed(Path::getAppPath()) == 0) {
        printf("failed DiskStat::getPathUsed\n");
        return false;
    }
#endif

    return true;
}

bool testCPUStat() {
    {
        if (CPUStat::numCPUs() < 1) {
            return false;
        }

        double processUsage = CPUStat::processUsage();
        if (!(processUsage >= 0.0 && processUsage <= 100.0)) {
            return false;
        }

        double usage = CPUStat::usage();
        if (!(usage >= 0.0 && usage <= 400.0)) {
            return false;
        }
    }

    return true;
}

bool testThreadStat() {
    {
#ifndef __EMSCRIPTEN__
        if (ThreadStat::live() <= 0) {
            return false;
        }

        if (ThreadStat::peak() <= 0) {
            return false;
        }

        if (ThreadStat::daemon() != 0) {
            return false;
        }
#endif
    }

    return true;
}

bool testMemoryStat() {
    {
#ifndef __EMSCRIPTEN__
        if (MemoryStat::systemMax() <= 0) {
            return false;
        }

        if (MemoryStat::systemUsed() <= 0) {
            return false;
        }
#endif

#ifdef LINUX_OS
        if (MemoryStat::max() <= 0) {
            return false;
        }
#endif

#ifndef __EMSCRIPTEN__
        if (MemoryStat::maxHeap() <= 0) {
            return false;
        }

        if (MemoryStat::maxNonHeap() != 0) {
            return false;
        }

        if (MemoryStat::used() <= 0) {
            return false;
        }

        if (MemoryStat::usedHeap() <= 0) {
            return false;
        }

        if (MemoryStat::usedNonHeap() <= 0) {
            return false;
        }

        if (MemoryStat::committed() != 0) {
            return false;
        }

        if (MemoryStat::committedHeap() <= 0) {
            return false;
        }

        if (MemoryStat::committedNonHeap() != 0) {
            return false;
        }

        MemoryStat::logUsed();
#endif
    }

    return true;
}

bool testUserStat() {
    {
        if (!UserStat::country().isNullOrEmpty()) {
            return false;
        }

        if (!UserStat::countryFormat().isNullOrEmpty()) {
            return false;
        }

        if (UserStat::dir().isNullOrEmpty()) {
            return false;
        }

        if (!UserStat::countryFormat().isNullOrEmpty()) {
            return false;
        }

        if (UserStat::name().isNullOrEmpty()) {
            return false;
        }

        if (!UserStat::language().isNullOrEmpty()) {
            return false;
        }

        if (UserStat::timezone().isNullOrEmpty()) {
            return false;
        }

        if (UserStat::home().isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

bool testOSStat() {
    {
        if (OSStat::arch().isNullOrEmpty()) {
            return false;
        }

        if (OSStat::name().isNullOrEmpty()) {
            return false;
        }

        if (OSStat::version().isNullOrEmpty()) {
            return false;
        }
    }

    return true;
}

int main() {
    if (!testDiskStat()) {
        return 1;
    }

    if (!testCPUStat()) {
        return 2;
    }

    if (!testThreadStat()) {
        return 3;
    }

    if (!testMemoryStat()) {
        return 4;
    }

    if (!testUserStat()) {
        return 5;
    }

    if (!testOSStat()) {
        return 6;
    }

    return 0;
}