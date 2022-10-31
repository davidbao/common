//
//  Directory.cpp
//  common
//
//  Created by baowei on 2016/12/18.
//  Copyright © 2016 com. All rights reserved.
//

#include "IO/Directory.h"
#include "exception/Exception.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "data/TimeZone.h"
#include "system/Application.h"
#include <sys/stat.h>
#include <cstdlib>

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
#include <dirent.h>
#include <pwd.h>
#include <sys/time.h>
#else
#include <sys/vfs.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/time.h>
#endif

namespace Common {
    bool Directory::createDirectoryInner(const char *path) {
        if (path == nullptr)
            return false;

#if WIN32
        BOOL result = CreateDirectory(path, nullptr);
        if (!result) {
            Debug::writeFormatLine("Create directory failed, path: %s, reason: %d", path, GetLastError());
        }
        return result != 0 ? true : false;
        //return _mkdir(path) == 0 ? true : false;
#else
        return mkdir(path, 0777) == 0;
#endif
    }

    bool Directory::createDirectory(const String &path) {
        if (path.isNullOrEmpty())
            return false;

        String str = path;
#ifdef WIN32
        if (str.find('/') >= 0) {
            str = str.replace(str, "/", "\\");
        }
#endif
        if (path[path.length() - 1] != Path::DirectorySeparatorChar) {
            str.append(Path::DirectorySeparatorChar);
        }
        if (exists(str)) {
            Debug::writeLine("The creating directory is exists!");
            return false;
        } else {
            String temp;
            ssize_t pos = 0;
            while ((pos = str.find(Path::DirectorySeparatorChar, pos)) != -1) {
                if (pos > 0) {
                    temp = str.substr(0, pos);
                    if (!exists(temp)) {
                        if (!createDirectoryInner(temp.c_str()))
                            return false;
                    }
                }
                pos += sizeof(Path::DirectorySeparatorChar);
            }
            return true;
        }
    }

    int Directory::remove_directory(const char *path) {
#ifdef WIN32
        WIN32_FIND_DATA ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
        // Find the first file in the directory.
        String fileStr = Path::combine(path, "*.*");
        hFind = FindFirstFile(fileStr.c_str(), &ffd);
        if (INVALID_HANDLE_VALUE != hFind) {
            // List all the files in the directory with some info about them.
            do {
                if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    String temp = ffd.cFileName;
                    if (temp != "." && temp != "..") {
                        remove_directory(Path::combine(path, ffd.cFileName));
                    }
                } else {
                    DeleteFile(Path::combine(path, ffd.cFileName));
                }
            } while (FindNextFile(hFind, &ffd) != 0);
            FindClose(hFind);
        }
        int result = RemoveDirectory(path) ? 0 : 1;
        return result;
#else
        DIR *d = opendir(path);
        size_t path_len = strlen(path);
        int r = -1;

        if (d) {
            struct dirent *p;

            r = 0;

            while (!r && (p = readdir(d))) {
                int r2 = -1;
                char *buf;
                size_t len;

                /* Skip the names "." and ".." as we don't want to recurse on them. */
                if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                    continue;
                }

                len = path_len + strlen(p->d_name) + 2;
                buf = (char *) malloc(len);

                if (buf) {
                    struct stat statbuf;

                    snprintf(buf, len, "%s/%s", path, p->d_name);

                    if (!stat(buf, &statbuf)) {
                        if (S_ISDIR(statbuf.st_mode)) {
                            r2 = remove_directory(buf);
                        } else {
                            r2 = unlink(buf);
                        }
                    }

                    free(buf);
                }

                r = r2;
            }

            closedir(d);
        }

        if (!r) {
            r = rmdir(path);
        }

        return r;
#endif
    }

    bool Directory::deleteDirectory(const String &path) {
        if (path.isNullOrEmpty())
            return false;

#if WIN32
        bool result = remove_directory(path) == 0 ? true : false;
        if (!result) {
            Debug::writeFormatLine("remove directory failed with error: %d", GetLastError());
        }
        return result;
#else
        bool result = remove_directory(path) == 0 ? true : false;
        if (!result) {
            Debug::writeFormatLine("remove directory failed with error: %s", strerror(errno));
        }
        return result;
#endif
    }

    bool Directory::exists(const String &path) {
        if (path.isNullOrEmpty())
            return false;

#if WIN32
        if (PathFileExists(path) == TRUE) {
            if (strlen(path) == 2 && path[1] == ':')    // dirve, fixed bug.
            {
                return true;
            }

            WIN32_FIND_DATA wfd;
            bool rValue = false;
            HANDLE hFind = FindFirstFile(path, &wfd);
            if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                rValue = true;
            } else {
                Debug::writeFormatLine("The directory is not exists, path: '%s', handle: %x, attribute: %d, reason: %d",
                                       path.c_str(), hFind, wfd.dwFileAttributes, GetLastError());
            }
            FindClose(hFind);
            return rValue;
        }
#else
        struct stat s;
        int err = stat(path, &s);
        if (-1 == err) {
            if (ENOENT == errno) {
                /* does not exist */
            } else {
            }
        } else {
            if (S_ISDIR(s.st_mode)) {
                /* it's a dir */
                return true;
            } else {
                /* exists but is no dir */
            }
        }
#endif
        return false;
    }

    bool Directory::getFiles(const String &path, const String &searchPattern, SearchOption searchOption,
                             StringArray &files) {
        String filter = !searchPattern.isNullOrEmpty() ? searchPattern : "*.*";
#if WIN32
        if (exists(path)) {
            StringArray directories;
            directories.add(path);
            if (searchOption == SearchOption::AllDirectories) {
                Directory::getDirectories(path, "*", searchOption, directories);
            }
            for (size_t i = 0; i < directories.count(); i++) {
                const String &dir = directories[i];

                WIN32_FIND_DATA ffd;
                HANDLE hFind = INVALID_HANDLE_VALUE;
                // Find the first file in the directory.
                String fileStr = Path::combine(dir, filter.c_str());
                hFind = FindFirstFile(fileStr.c_str(), &ffd);
                if (INVALID_HANDLE_VALUE != hFind) {
                    // List all the files in the directory with some info about them.
                    do {
                        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                            files.add(Path::combine(dir, ffd.cFileName));
                        }
                    } while (FindNextFile(hFind, &ffd) != 0);
                    FindClose(hFind);
                }
            }
            return true;
        }
#else
        //const char* path = dirname((char*)_logPath.c_str());
        if (exists(path)) {
            DIR *dir;
            struct dirent *ent;
            struct stat st;

            dir = opendir(path);
            while ((ent = readdir(dir)) != nullptr) {
                const String file_name = ent->d_name;
                const String full_file_name = (String) path + "/" + file_name;

                if (file_name[0] == '.')
                    continue;

                if (stat(full_file_name.c_str(), &st) == -1)
                    continue;

                const bool is_directory = (st.st_mode & S_IFDIR) != 0;

                if (is_directory) {
                    if (searchOption == SearchOption::AllDirectories) {
                        getFiles(full_file_name, filter, searchOption, files);
                    }
                } else {
                    int index;
                    if (filter == "*.*" || filter == "*")
                        files.add(full_file_name);
                    else if ((index = filter.find("*")) >= 0) {
                        String str = String::replace(filter, "*", String::Empty);
                        if (full_file_name.find(str) > 0)
                            files.add(full_file_name);
                    } else {
                        if (full_file_name.find(filter) > 0)
                            files.add(full_file_name);
                    }
                }
            }
            closedir(dir);
            return true;
        }
#endif
        return false;
    }

    bool Directory::getDirectories(const String &path, const String &searchPattern, SearchOption searchOption,
                                   StringArray &directories) {
        String filter = !searchPattern.isNullOrEmpty() ? searchPattern : "*.*";
#if WIN32
        if (exists(path)) {
            WIN32_FIND_DATA ffd;
            HANDLE hFind = INVALID_HANDLE_VALUE;
            // Find the first file in the directory.
            String fileStr = Path::combine(path, filter.c_str());
            hFind = FindFirstFile(fileStr.c_str(), &ffd);
            if (INVALID_HANDLE_VALUE != hFind) {
                // List all the files in the directory with some info about them.
                do {
                    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        String pathStr = ffd.cFileName;
                        if (pathStr != "." && pathStr != "..") {
                            directories.add(Path::combine(path, pathStr));

                            if (searchOption == SearchOption::AllDirectories) {
                                getDirectories(Path::combine(path, ffd.cFileName), filter, searchOption, directories);
                            }
                        }
                    }
                } while (FindNextFile(hFind, &ffd) != 0);
                FindClose(hFind);
            }
            return true;
        }
#else
        //const char* path = dirname((char*)_logPath.c_str());
        if (exists(path)) {
            DIR *dir;
            struct dirent *ent;
            struct stat st;

            dir = opendir(path);
            while ((ent = readdir(dir)) != nullptr) {
                const String file_name = ent->d_name;
                const String full_file_name = (String) path + "/" + file_name;

                if (file_name[0] == '.')
                    continue;

                if (stat(full_file_name.c_str(), &st) == -1)
                    continue;

                const bool is_directory = (st.st_mode & S_IFDIR) != 0;

                if (is_directory) {
                    directories.add(full_file_name);

                    if (searchOption == SearchOption::AllDirectories) {
                        getDirectories(full_file_name, filter, searchOption, directories);
                    }
                }
            }
            closedir(dir);
            return true;
        }
#endif
        return false;

    }

    bool Directory::copy(const String &sourcePath, const String &destPath, const StringArray *excludedItems) {
        if (sourcePath.isNullOrEmpty())
            return false;
        if (destPath.isNullOrEmpty())
            return false;

        if (exists(sourcePath)) {
            const String filter = "*";
            if (!exists(destPath)) {
                createDirectory(destPath);
            }

#if WIN32
            WIN32_FIND_DATA ffd;
            HANDLE hFind = INVALID_HANDLE_VALUE;
            // Find the first file in the directory.
            String fileStr = Path::combine(sourcePath, filter.c_str());
            hFind = FindFirstFile(fileStr.c_str(), &ffd);
            if (INVALID_HANDLE_VALUE != hFind) {
                // List all the files in the directory with some info about them.
                do {
                    if (excludedItems != nullptr &&
                        excludedItems->contains(ffd.cFileName)) {
                        continue;
                    }

                    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        String path = ffd.cFileName;
                        if (path != "." && path != "..") {
                            String sPathName = Path::combine(sourcePath, ffd.cFileName);
                            String dPathName = Path::combine(destPath, ffd.cFileName);
                            Directory::copy(sPathName.c_str(), dPathName.c_str(), excludedItems);
                        }
                    } else {
                        String sFileName = Path::combine(sourcePath, ffd.cFileName);
                        String dFileName = Path::combine(destPath, ffd.cFileName);
                        File::copy(sFileName.c_str(), dFileName.c_str());
                    }
                } while (FindNextFile(hFind, &ffd) != 0);
                FindClose(hFind);
            }
#else
            DIR *dir;
            struct dirent *ent;
            struct stat st;

            dir = opendir(sourcePath);
            while ((ent = readdir(dir)) != nullptr) {
                const String file_name = ent->d_name;
                const String full_file_name = (String) sourcePath + "/" + file_name;

                if (file_name[0] == '.')
                    continue;

                if (stat(full_file_name.c_str(), &st) == -1)
                    continue;

                if (excludedItems != nullptr &&
                    excludedItems->contains(file_name)) {
                    continue;
                }

                const bool is_directory = (st.st_mode & S_IFDIR) != 0;
                if (is_directory) {
                    String sPathName = Path::combine(sourcePath, file_name);
                    String dPathName = Path::combine(destPath, file_name);
                    if (Directory::exists(dPathName))
                        Directory::deleteDirectory(dPathName);
                    Directory::createDirectory(dPathName);
                    Directory::copy(sPathName.c_str(), dPathName.c_str(), excludedItems);
                } else {
                    String sFileName = Path::combine(sourcePath, file_name);
                    String dFileName = Path::combine(destPath, file_name);
                    File::copy(sFileName.c_str(), dFileName.c_str());
                }
            }
            closedir(dir);

#ifdef __arm_linux__
            sync();    // save all of files.
#endif
#endif
            return true;
        }
        return false;
    }

    String Directory::getAppPath() {
        return Path::getDirectoryName(Application::startupPath());
    }

    String Directory::getHomePath() {
#ifdef WIN32
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, 0, path))) {
            return path;
        }
        const char *homeDir = getenv("HOMEPATH");
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#elif __EMSCRIPTEN__
        return Path::combine(getAppPath(), "home");
#elif __arm_linux__
        const char *homeDir = getenv("HOME");
        if (!homeDir) {
            struct passwd *pwd = getpwuid(getuid());
            if (pwd)
                homeDir = pwd->pw_dir;
        }
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#else
        const char *homeDir = getenv("HOME");
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#endif
    }

    String Directory::getDocumentPath(const String &subDocPath) {
        String docPath = Directory::getHomePath();
        assert(!docPath.isNullOrEmpty());
        if (subDocPath.isNullOrEmpty()) {
            docPath = Directory::getAppPath();
        } else {
            String tempPath = subDocPath;
#ifdef WIN32
            if (subDocPath.find('/') >= 0) {
                tempPath = subDocPath.replace(subDocPath, "/", "\\");
            }
#endif
            docPath = Path::combine(docPath, tempPath);
        }
        return docPath;
    }

    String Directory::getTempPath() {
#ifdef WIN32
        char path[MAX_PATH];
        memset(path, 0, sizeof(path));
        if (GetTempPath(sizeof(path), path) > 0)
            return (String) path;
        return String::Empty;
#elif __EMSCRIPTEN__
        return Path::combine(getAppPath(), "tmp");
#else
        return "/tmp";
#endif
    }

    String Directory::getCurrentDirectory() {
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

    bool Directory::setCurrentDirectory(const String &path) {
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

    bool Directory::rename(const String &oldPath, const String &newPath) {
        if (oldPath != newPath) {
            if (exists(oldPath)) {
                if (exists(newPath))
                    deleteDirectory(newPath);
                return ::rename(oldPath, newPath) == 0;
            }
        }
        return false;
    }

    bool Directory::getModifyTime(const String &path, DateTime &time) {
        if (Directory::exists(path)) {

            struct stat s;
            int err = stat(path.c_str(), &s);
            if (-1 == err) {
                if (ENOENT == errno) {
                    /* does not exist */
                } else {
                }
            } else {
                time = DateTime::fromLocalTime(s.st_mtime);
                return true;
            }
        }
        return false;
    }

    bool Directory::setModifyTime(const String &path, const DateTime &time) {
        if (Directory::exists(path)) {

#ifdef WIN32
            HANDLE handle = CreateFile(path,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                       FILE_FLAG_BACKUP_SEMANTICS,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if (GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
                SYSTEMTIME stime;
                DateTime utime = TimeZone::Local.toUtcTime(time);
                stime.wYear = utime.year();
                stime.wMonth = utime.month();
                stime.wDay = utime.day();
                stime.wHour = utime.hour();
                stime.wMinute = utime.minute();
                stime.wSecond = utime.second();
                stime.wMilliseconds = utime.millisecond();
                SystemTimeToFileTime(&stime, &lastWriteTime);
                result = SetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime);
            }
            CloseHandle(handle);
            return result;
#elif __EMSCRIPTEN__ || __ANDROID__
            DateTime lastAccessTime;
            if(!getLastAccessTime(path, lastAccessTime))
            {
                lastAccessTime = time;
            }

            struct utimbuf timebuf = {0};
            time_t actime = DateTime::toLocalTime(lastAccessTime);
            time_t modtime = DateTime::toLocalTime(time);
            timebuf.actime = actime;
            timebuf.modtime = modtime;
            return utime(path, &timebuf) == 0;
#else
            DateTime lastAccessTime;
            if (!getLastAccessTime(path, lastAccessTime)) {
                lastAccessTime = time;
            }

            uint64_t atime = (uint64_t) lastAccessTime.total1970Milliseconds();
            uint64_t mtime = (uint64_t) time.total1970Milliseconds();
            struct timeval tv[2] = {0};
            // times[0] specifies the new access time, and times[1] specifies the new modification time.
            tv[0].tv_sec = atime / 1000;
            tv[0].tv_usec = 1000 * (atime % 1000);
            tv[1].tv_sec = mtime / 1000;
            tv[1].tv_usec = 1000 * (mtime % 1000);
            return lutimes(path.c_str(), tv) == 0;
#endif
        }
        return false;
    }

    bool Directory::getCreationTime(const String &path, DateTime &time) {
        if (Directory::exists(path)) {
            struct stat s;
            int err = stat(path.c_str(), &s);
            if (-1 == err) {
                if (ENOENT == errno) {
                    /* does not exist */
                } else {
                }
            } else {
                time = DateTime::fromLocalTime(s.st_ctime);
                return true;
            }
        }
        return false;
    }

    bool Directory::setCreationTime(const String &path, const DateTime &time) {
        if (Directory::exists(path)) {

#ifdef WIN32
            HANDLE handle = CreateFile(path,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                       FILE_FLAG_BACKUP_SEMANTICS,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if (GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
                SYSTEMTIME stime;
                DateTime utime = TimeZone::Local.toUtcTime(time);
                stime.wYear = utime.year();
                stime.wMonth = utime.month();
                stime.wDay = utime.day();
                stime.wHour = utime.hour();
                stime.wMinute = utime.minute();
                stime.wSecond = utime.second();
                stime.wMilliseconds = utime.millisecond();
                SystemTimeToFileTime(&stime, &creationTime);
                result = SetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime);
            }
            CloseHandle(handle);
            return result;
#endif
        }
        return false;
    }

    bool Directory::getLastAccessTime(const String &path, DateTime &time) {
        if (Directory::exists(path)) {

            struct stat s;
            int err = stat(path.c_str(), &s);
            if (-1 == err) {
                if (ENOENT == errno) {
                    /* does not exist */
                } else {
                }
            } else {
                time = DateTime::fromLocalTime(s.st_atime);
                return true;
            }
        }
        return false;
    }

    bool Directory::setLastAccessTime(const String &path, const DateTime &time) {
        if (Directory::exists(path)) {

#ifdef WIN32
            HANDLE handle = CreateFile(path,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                       FILE_FLAG_BACKUP_SEMANTICS,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if (GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
                SYSTEMTIME stime;
                DateTime utime = TimeZone::Local.toUtcTime(time);
                stime.wYear = utime.year();
                stime.wMonth = utime.month();
                stime.wDay = utime.day();
                stime.wHour = utime.hour();
                stime.wMinute = utime.minute();
                stime.wSecond = utime.second();
                stime.wMilliseconds = utime.millisecond();
                SystemTimeToFileTime(&stime, &lastAccessTime);
                result = SetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime);
            }
            CloseHandle(handle);
            return result;
#elif __EMSCRIPTEN__ || __ANDROID__
            DateTime modifyTime;
            if(!getModifyTime(path, modifyTime))
            {
                modifyTime = time;
            }

            struct utimbuf timebuf = {0};
            time_t actime = DateTime::toLocalTime(time);
            time_t modtime = DateTime::toLocalTime(modifyTime);
            timebuf.actime = actime;
            timebuf.modtime = modtime;
            return utime(path, &timebuf) == 0;
#else
            DateTime modifyTime;
            if (!getModifyTime(path, modifyTime)) {
                modifyTime = time;
            }

            uint64_t atime = (uint64_t) time.total1970Milliseconds();
            uint64_t mtime = (uint64_t) modifyTime.total1970Milliseconds();
            struct timeval tv[2] = {0};
            // times[0] specifies the new access time, and times[1] specifies the new modification time.
            tv[0].tv_sec = atime / 1000;
            tv[0].tv_usec = 1000 * (atime % 1000);
            tv[1].tv_sec = mtime / 1000;
            tv[1].tv_usec = 1000 * (mtime % 1000);
            return lutimes(path.c_str(), tv) == 0;
#endif
        }
        return false;
    }
}
