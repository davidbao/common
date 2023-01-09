//
//  TcpServer.cpp
//  common
//
//  Created by baowei on 2023/1/6.
//  Copyright © 2023 com. All rights reserved.
//

#include "IO/Path.h"
#include "system/Environment.h"

using namespace IO;
using namespace System;

bool testChangeExtension() {
    {
        String goodFileName = "C:\\mydir\\myfile.com.extension";
        if (Path::changeExtension(goodFileName, ".old") != "C:\\mydir\\myfile.com.old") {
            return false;
        }
    }

    {
        String goodFileName = "C:\\mydir\\myfile.com.extension";
        if (Path::changeExtension(goodFileName, "") != "C:\\mydir\\myfile.com") {
            return false;
        }
    }

    {
        String badFileName = "C:\\mydir\\";
        if (Path::changeExtension(badFileName, ".old") != "C:\\mydir\\.old") {
            return false;
        }
    }

    {
        String goodFileName = "/usr/local/mydir/myfile.com.extension";
        if (Path::changeExtension(goodFileName, ".old") != "/usr/local/mydir/myfile.com.old") {
            return false;
        }
    }

    {
        String goodFileName = "/usr/local/mydir/myfile.com.extension";
        if (Path::changeExtension(goodFileName, "") != "/usr/local/mydir/myfile.com") {
            return false;
        }
    }

    {
        String badFileName = "/usr/local/mydir/";
        if (Path::changeExtension(badFileName, ".old") != "/usr/local/mydir/.old") {
            return false;
        }
    }

    return true;
}

bool testCombine() {
#ifdef WIN32
    {
        String path1 = "C:\\mydir";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "C:\\mydir\\myfile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "C:\\mydir\\";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "C:\\mydir\\myfile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "mydir";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "mydir\\myfile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "C:\\mydir\\";
        String path2 = "mydir2";
        String path3 = "myfile.com.extension";
        if (Path::combine(path1, path2, path3) != "C:\\mydir\\mydir2\\myfile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "C:\\mydir\\";
        String path2 = "mydir2";
        String path3 = "mydir3";
        String path4 = "myfile.com.extension";
        if (Path::combine(path1, path2, path3, path4) != "C:\\mydir\\mydir2\\mydir3\\myfile.com.extension") {
            return false;
        }
    }

    {
        StringArray paths = {"C:\\mydir\\", "mydir2", "mydir3", "myfile.com.extension"};
        if (Path::combine(paths) != "C:\\mydir\\mydir2\\mydir3\\myfile.com.extension") {
            return false;
        }
    }
#else
    {
        String path1 = "/usr/local/mydir";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "/usr/local/mydir/myfile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "/usr/local/mydir/";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "/usr/local/mydir/myfile.com.extension") {
            return false;
        }
    }
    {
        String path1 = "mydir";
        String path2 = "myfile.com.extension";
        if (Path::combine(path1, path2) != "mydir/myfile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "/usr/local/mydir";
        String path2 = "mydir2";
        String path3 = "myfile.com.extension";
        if (Path::combine(path1, path2, path3) != "/usr/local/mydir/mydir2/myfile.com.extension") {
            return false;
        }
    }

    {
        String path1 = "/usr/local/mydir";
        String path2 = "mydir2";
        String path3 = "mydir3";
        String path4 = "myfile.com.extension";
        if (Path::combine(path1, path2, path3, path4) != "/usr/local/mydir/mydir2/mydir3/myfile.com.extension") {
            return false;
        }
    }

    {
        StringArray paths = {"/usr/local/mydir", "mydir2", "mydir3", "myfile.com.extension"};
        if (Path::combine(paths) != "/usr/local/mydir/mydir2/mydir3/myfile.com.extension") {
            return false;
        }
    }
#endif

    return true;
}

bool testEndsInDirectorySeparator() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\";
        if (!Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "C:\\mydir";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/";
        if (!Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }
    {
        String path = "/usr/local/mydir";
        if (Path::endsInDirectorySeparator(path)) {
            return false;
        }
    }

#endif

    return true;
}

bool testGetDirectoryName() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\mydir2\\";
        if (Path::getDirectoryName(path) != "C:\\mydir") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\mydir2";
        if (Path::getDirectoryName(path) != "C:\\mydir") {
            return false;
        }
    }

    {
        String path = "C:\\mydir";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:\\";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "C:";
        if (Path::getDirectoryName(path) != "C:") {
            return false;
        }
    }

    {
        String path = "abc";
        if (Path::getDirectoryName(path) != "abc") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/";
        if (Path::getDirectoryName(path) != "/usr/local") {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir";
        if (Path::getDirectoryName(path) != "/usr/local") {
            return false;
        }
    }

    {
        String path = "/usr";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "/usr/";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "/";
        if (Path::getDirectoryName(path) != "/") {
            return false;
        }
    }

    {
        String path = "abc";
        if (Path::getDirectoryName(path) != "abc") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetExtension() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\myfile.com.extension";
        if (Path::getExtension(path) != ".extension") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\mydir\\";
        if (Path::getExtension(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/myfile.com.extension";
        if (Path::getExtension(path) != ".extension") {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir/";
        if (Path::getExtension(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetFileName() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\myfile.com.extension";
        if (Path::getFileName(path) != "myfile.com.extension") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\mydir\\";
        if (Path::getFileName(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/myfile.com.extension";
        if (Path::getFileName(path) != "myfile.com.extension") {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir/";
        if (Path::getFileName(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetFileNameWithoutExtension() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\myfile.com.extension";
        if (Path::getFileNameWithoutExtension(path) != "myfile.com") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\mydir\\";
        if (Path::getFileNameWithoutExtension(path) != "") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/myfile.com.extension";
        if (Path::getFileNameWithoutExtension(path) != "myfile.com") {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir/";
        if (Path::getFileNameWithoutExtension(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetFullPath() {
    {
        String path = "mydir";
        String fullPath = Path::getFullPath(path);
        String current = Environment::getCurrentDirectory();
        if (fullPath != Path::combine(current, path)) {
            return false;
        }
    }

    {
        String path = "mydir";
        String basePath = "/usr/local/mydir";
        String fullPath = Path::getFullPath(path, basePath);
        if (fullPath != Path::combine(basePath, path)) {
            return false;
        }
    }

    return true;
}

bool testChars() {
    {
        const Array<char, 41> &chars = Path::getInvalidFileNameChars();
        if (!(chars[0] == '\"' && chars[chars.count() - 1] == '/')) {
            return false;
        }
    }

    {
        const Array<char, 36> &chars = Path::getInvalidPathChars();
        if (!(chars[0] == '\"' && chars[chars.count() - 1] == 31)) {
            return false;
        }
    }

    return true;
}

bool testGetPathRoot() {
#ifdef WIN32
    {
        String path = "\\mydir\\";
        if (Path::getPathRoot(path) != "") {
            return false;
        }
    }

    {
        String path = "myfile.ext";
        if (Path::getPathRoot(path) != "") {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\myfile.ext";
        if (Path::getPathRoot(path) != "C:\\") {
            return false;
        }
    }

    {
        String path = "\\\\myPC\\mydir\\myfile.ext";
        if (Path::getPathRoot(path) != "\\\\myPC\\") {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir";
        if(Path::getPathRoot(path) != "/") {
            return false;
        }
    }

    {
        String path = "myfile.ext";
        if(Path::getPathRoot(path) != "") {
            return false;
        }
    }
#endif

    return true;
}

bool testGetRandomFileName() {
    {
        String fileName = Path::getRandomFileName();
        if (fileName.length() != 12) {
            return false;
        }

        ssize_t pos = fileName.find('.');
        if (pos != 8) {
            return false;
        }
    }

    return true;
}

bool testGetTempFileName() {
    {
        String tempName = Path::getTempFileName();
        if (tempName.isNullOrEmpty()) {
            return false;
        }
    }
    return true;
}

bool testGetTempPath() {
    {
        String tempPath = Path::getTempPath();
        if (tempPath.isNullOrEmpty()) {
            return false;
        }
    }
    return true;
}

bool testHasExtension() {
#ifdef WIN32
    {
        String path = "C:\\mydir\\myfile.ext";
        if (!Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\myfile.ext\\";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "C:\\mydir\\myfile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "mydir\\myfile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir/myfile.ext";
        if (!Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir/myfile.ext/";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "/usr/local/mydir/myfile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }

    {
        String path = "mydir/myfile";
        if (Path::hasExtension(path)) {
            return false;
        }
    }
#endif

    return true;
}

bool testIsPathRooted() {
#ifdef WIN32
    {
        String path = "C:\\mydir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "\\myPC\\mydir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "mydir";
        if (Path::isPathRooted(path)) {
            return false;
        }
    }
#else
    {
        String path = "/usr/local/mydir";
        if (!Path::isPathRooted(path)) {
            return false;
        }
    }

    {
        String path = "mydir";
        if (Path::isPathRooted(path)) {
            return false;
        }
    }
#endif

    return true;
}

int main() {
    if (!testChangeExtension()) {
        return 1;
    }
    if (!testCombine()) {
        return 2;
    }
    if (!testEndsInDirectorySeparator()) {
        return 3;
    }
    if (!testGetDirectoryName()) {
        return 4;
    }
    if (!testGetExtension()) {
        return 5;
    }
    if (!testGetFileName()) {
        return 6;
    }
    if (!testGetFileNameWithoutExtension()) {
        return 7;
    }
    if (!testGetFullPath()) {
        return 8;
    }
    if (!testChars()) {
        return 9;
    }
    if (!testGetPathRoot()) {
        return 10;
    }
    if (!testGetRandomFileName()) {
        return 11;
    }
    if (!testGetTempFileName()) {
        return 12;
    }
    if (!testGetTempPath()) {
        return 13;
    }
    if (!testHasExtension()) {
        return 14;
    }
    if (!testIsPathRooted()) {
        return 15;
    }

    return 0;
}