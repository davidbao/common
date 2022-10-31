//
//  File.cpp
//  common
//
//  Created by baowei on 2015/9/26.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "IO/File.h"
#include "IO/Directory.h"
#include "data/TimeZone.h"
#include "IO/Path.h"
#include "IO/FileStream.h"

#if WIN32
#include <Windows.h>
#include <memory.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include "Shlwapi.h"
#elif __APPLE__
#include <sys/param.h>
#include <sys/mount.h>
#include <libgen.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#elif __ANDROID__
#include <malloc.h>
#else
#include <sys/vfs.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif

#ifndef S_ISDIR
#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)	/* directory */
#endif

namespace Common {
    bool File::exists(const String &path) {
        if (path.isNullOrEmpty())
            return false;

#if WIN32
        if (PathFileExists(path) == TRUE)
        {
            WIN32_FIND_DATA  wfd;
            bool rValue = false;
            HANDLE hFind = FindFirstFile(path, &wfd);
            if ((hFind != INVALID_HANDLE_VALUE) && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                rValue = true;
            }
            FindClose(hFind);
            return rValue;
        }
#else
        String str = path;
        if (str.find("file://") >= 0)
            str.replace("file://", String::Empty);

        struct stat s;
        int err = stat(str.c_str(), &s);
        if (-1 == err) {
            if (ENOENT == errno) {
                /* does not exist */
            } else {
            }
        } else {
            if (S_ISDIR(s.st_mode)) {
                /* it's a dir */
            } else {
                /* exists but is no dir */
                return true;
            }
        }
#endif
        return false;
    }

    bool File::deleteFile(const String &path) {
        if (path.isNullOrEmpty())
            return false;

        if (!exists(path))
            return false;

        return remove(path) == 0 ? true : false;
    }

    bool File::move(const String &sourceFileName, const String &destFileName) {
        if (sourceFileName.isNullOrEmpty())
            return false;
        if (destFileName.isNullOrEmpty())
            return false;

        if (exists(sourceFileName)) {
            if (exists(destFileName)) {
                deleteFile(destFileName);
            }
#if WIN32
            return MoveFile(sourceFileName, destFileName) ? true : false;
#else
            return move_file(sourceFileName, destFileName);
#endif
        }
        return false;
    }

    bool File::copy(const String &sourceFileName, const String &destFileName, bool overwrite) {
        if (sourceFileName.isNullOrEmpty())
            return false;
        if (destFileName.isNullOrEmpty())
            return false;

        if (exists(sourceFileName)) {
#if WIN32
            return CopyFile(sourceFileName, destFileName, overwrite ? FALSE : TRUE) ? true : false;
#else
            return copy_file(sourceFileName, destFileName, overwrite ? false : true);
#endif
        }
        return false;
    }

#if !WIN32

    bool File::chmod(const String &fileName, int mode) {
        // chmod
#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif
#ifndef S_IEXEC
#define S_IEXEC S_IXUSR
#endif
        if (File::exists(fileName)) {
            int result = ::chmod(fileName.c_str(), mode);
            if (result != 0) {
                Debug::writeFormatLine("Can not chmod the file, file: %s", fileName.c_str());
                return false;
            }
            return true;
        } else {
            Debug::writeFormatLine("Can not find the file, file: %s", fileName.c_str());
            return false;
        }
        return false;
    }

    bool File::write_file(int src_fd, int dest_fd, struct stat *st_src) {
        int remain, n;
        char *buf, *wbuf;
        int buf_size = st_src->st_blksize;

        buf_size = buf_size < 8192 ? 8192 : (buf_size > 65536 ? 65536 : buf_size);
        buf = (char *) malloc(buf_size);

        for (;;) {
            remain = (int) read(src_fd, buf, buf_size);
            if (remain < 0) {
                if (errno == EINTR)
                    continue;

                free(buf);
                return false;
            }
            if (remain == 0) {
                break;
            }

            wbuf = buf;
            while (remain > 0) {
                if ((n = (int) write(dest_fd, wbuf, remain)) < 0) {
                    if (errno == EINTR)
                        continue;

                    free(buf);
                    return false;
                }

                remain -= n;
                wbuf += n;
            }
        }

        free(buf);
        return true;
    }

    /**
     * CopyFile:
     * @name: a pointer to a nullptr-terminated unicode String, that names
     * the file to be copied.
     * @dest_name: a pointer to a nullptr-terminated unicode String, that is the
     * new name for the file.
     * @fail_if_exists: if TRUE and dest_name exists, the copy will fail.
     *
     * Copies file @name to @dest_name
     *
     * Return value: %TRUE on success, %FALSE otherwise.
     */
    bool File::copy_file(const char *name, const char *dest_name, bool fail_if_exists) {
        int src_fd, dest_fd;
        struct stat st, dest_st;
        struct utimbuf dest_time;
        bool ret = true;
        int ret_utime;

        if (name == nullptr) {
            return (false);
        }

        if (dest_name == nullptr) {
            return (false);
        }

        src_fd = open(name, O_RDONLY, 0);
        if (src_fd < 0) {
            return (false);
        }

        if (fstat(src_fd, &st) < 0) {
            return (false);
        }

        /* Before trying to open/create the dest, we need to report a 'file busy'
         * error if src and dest are actually the same file. We do the check here to take
         * advantage of the IOMAP capability */
        if (!stat(dest_name, &dest_st) && st.st_dev == dest_st.st_dev &&
            st.st_ino == dest_st.st_ino) {
            return (false);
        }

        if (fail_if_exists) {
            dest_fd = open(dest_name, O_WRONLY | O_CREAT | O_EXCL, st.st_mode);
        } else {
            /* FIXME: it kinda sucks that this code path potentially scans
             * the directory twice due to the weird SetLastError()
             * behavior. */
            dest_fd = open(dest_name, O_WRONLY | O_TRUNC, st.st_mode);
            if (dest_fd < 0) {
                /* The file does not exist, try creating it */
                dest_fd = open(dest_name, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
            } else {
                /* Apparently this error is set if we
                 * overwrite the dest file
                 */
            }
        }
        if (dest_fd < 0) {
            close(src_fd);

            return (false);
        }

        if (!write_file(src_fd, dest_fd, &st))
            ret = false;

        close(src_fd);
        fsync(dest_fd);
        close(dest_fd);

        dest_time.modtime = st.st_mtime;
        dest_time.actime = st.st_atime;
        ret_utime = utime(dest_name, &dest_time);
        if (ret_utime == -1)
            Debug::writeFormatLine("%s: file [%s] utime failed: %s", __func__, dest_name, strerror(errno));

#ifdef __arm_linux__
        sync();    // save all of files.
#endif
        return ret;
    }

    bool File::move_file(const char *name, const char *dest_name) {
        int result, errno_copy;
        struct stat stat_src, stat_dest;
        bool ret = false;

        if (name == nullptr) {
            return (false);
        }

        if (dest_name == nullptr) {
            return (false);
        }

        /*
         * In C# land we check for the existence of src, but not for dest.
         * We check it here and return the failure if dest exists and is not
         * the same file as src.
         */
        if (stat(name, &stat_src) < 0) {
            if (errno != ENOENT || lstat(name, &stat_src) < 0) {
                return false;
            }
        }

        if (!stat(dest_name, &stat_dest)) {
            if (stat_dest.st_dev != stat_src.st_dev ||
                stat_dest.st_ino != stat_src.st_ino) {
                return false;
            }
        }

        result = ::rename(name, dest_name);
        errno_copy = errno;

        if (result == -1) {
            switch (errno_copy) {
                case EEXIST:
                    break;

                case EXDEV:
                    /* Ignore here, it is dealt with below */
                    break;

                default:
                    break;
            }
        }

        if (result != 0 && errno_copy == EXDEV) {
            if (S_ISDIR (stat_src.st_mode)) {
                return false;
            }
            /* Try a copy to the new location, and delete the source */
            if (copy(name, dest_name, true) == false) {
                /* CopyFile will set the error */
                return (false);
            }

            return (deleteFile(name));
        }

        if (result == 0) {
            ret = true;
        }
#ifdef __arm_linux__
        sync();    // save all of files.
#endif
        return (ret);
    }

#endif

    bool File::rename(const String &oldFile, const String &newFile) {
        if (oldFile != newFile) {
            if (exists(oldFile)) {
                if (exists(newFile))
                    deleteFile(newFile);
                return ::rename(oldFile, newFile) == 0;
            }
        }
        return false;
    }

    const String File::getTempFileName(const String &prefix) {
        String pfx = !prefix.isNullOrEmpty() ? prefix.c_str() : "common";
        String path = Directory::getTempPath();

        String fileName = String::format("%s-%s", pfx.c_str(), Uuid::generate().toString().c_str());
        return Path::combine(path, fileName);

//#ifdef WIN32
//        char szTempFileName[MAX_PATH];
//        memset(szTempFileName, 0, sizeof(szTempFileName));
//        if(GetTempFileName(path, pfx, 0, szTempFileName) > 0)
//            return (String)szTempFileName;
//        return String::Empty;
//#else
////        String pfxTemplate = String::format("%s-XXXXXX", pfx.c_str());
////        char* str = new char[strlen(pfxTemplate) + 1];
////        memset(str, 0, strlen(pfxTemplate) + 1);
////        strcpy(str, pfxTemplate);
////        mkstemp(str);
////        String fileName = str;
////        delete[] str;
////        return Path::combine(path, fileName);
//
////        char* str = tempnam(path, pfx);
////        String result = str;
////        free(str);
//        String fileName = String::format("%s-%s", pfx.c_str(), Uuid::generate().toString().c_str());
//        return Path::combine(path, fileName);
//#endif
    }

    bool File::getSize(const String &path, int64_t &fileSize) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

            struct stat s;
            int err = stat(str.c_str(), &s);
            if (-1 == err) {
                if (ENOENT == errno) {
                    /* does not exist */
                } else {
                }
            } else {
                fileSize = s.st_size;
                return true;
            }
        }
        return false;
    }

    bool File::getModifyTime(const String &path, DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

            struct stat s;
            int err = stat(str.c_str(), &s);
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

    bool File::setModifyTime(const String &path, const DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

#ifdef WIN32
            HANDLE handle = CreateFileA(str,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                        0,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if(GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
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
            return utime(str, &timebuf) == 0;
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
            return lutimes(str.c_str(), tv) == 0;
#endif
        }
        return false;
    }

    bool File::getCreationTime(const String &path, DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

            struct stat s;
            int err = stat(str.c_str(), &s);
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

    bool File::setCreationTime(const String &path, const DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

#ifdef WIN32
            HANDLE handle = CreateFileA(str,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                        0,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if(GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
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

    bool File::getLastAccessTime(const String &path, DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

            struct stat s;
            int err = stat(str.c_str(), &s);
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

    bool File::setLastAccessTime(const String &path, const DateTime &time) {
        if (File::exists(path)) {
            String str = path;
            if (str.find("file://") >= 0)
                str.replace("file://", String::Empty);

#ifdef WIN32
            HANDLE handle = CreateFileA(str,
                                        FILE_GENERIC_READ | FILE_WRITE_ATTRIBUTES,
                                        FILE_SHARE_READ,
                                        nullptr,
                                        OPEN_EXISTING,
                                        0,
                                        nullptr);
            if (handle == INVALID_HANDLE_VALUE)
                return false;
            bool result = false;
            FILETIME creationTime, lastAccessTime, lastWriteTime;
            if(GetFileTime(handle, &creationTime, &lastAccessTime, &lastWriteTime)) {
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
            return utime(str, &timebuf) == 0;
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
            return lutimes(str.c_str(), tv) == 0;
#endif
        }
        return false;
    }

    String File::readAllText(const String &path) {
        if (File::exists(path)) {
            FileStream fs(path, FileMode::FileOpen, FileAccess::FileRead);
            String text;
            if (fs.readToEnd(text))
                return text;
        }
        return String::Empty;
    }

    ByteArray File::readAllBytes(const String &path) {
        if (File::exists(path)) {
            FileStream fs(path, FileMode::FileOpen, FileAccess::FileRead);
            ByteArray array;
            if (fs.readToEnd(array))
                return array;
        }
        return ByteArray::Empty;
    }
}
