//
//  FileStream.cpp
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright (c) 2017 com. All rights reserved.
//

#if WIN32
#include <Windows.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#else

#include <termios.h>

#endif

#include "IO/FileStream.h"
#include "IO/File.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "diag/Trace.h"
#include "exception/Exception.h"
#include "system/Math.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif
#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif

using namespace Diag;
using namespace System;

namespace IO {
    FileStream::FileStream(const String &filename, FileMode mode, FileAccess access) : _fd(InvalidHandle),
                                                                                       _access(access) {
        if (filename.isNullOrEmpty()) {
            throw ArgumentNullException("filename");
        }
        if (mode == FileOpen) {
            if (!File::exists(filename)) {
                throw FileNotFoundException(filename);
            }
        } else if (mode == FileCreateNew) {
            if (File::exists(filename)) {
                throw IOException(String::format("throw a file found exception, file name: %s", filename.c_str()));
            }
        } else if (mode == FileTruncate) {
            if (!File::exists(filename)) {
                throw FileNotFoundException(filename);
            }
        }

        String path = Path::getDirectoryName(filename);
        if (!Directory::exists(path)) {
            throw PathNotFoundException(filename);
        }

        open(filename, openFlag(filename, mode, access), openMode(access));
        if (isOpen()) {
            if (mode == FileMode::FileCreate) {
                setLength(0);
            }
        }
    }

    FileStream::~FileStream() {
        FileStream::close();
    }

    void FileStream::open(const char *filename, int openFlag, int mode) {
        _fileName = filename;
        _fd = ::open(filename, openFlag, mode);
        if (_fd == InvalidHandle) {
            Debug::writeFormatLine("can't open file: name: %s, error: %s", filename, strerror(errno));
        }
    }

    void FileStream::close() {
        if (_fd != InvalidHandle) {
            ::close(_fd);
        }
        _fd = InvalidHandle;
    }

    bool FileStream::isOpen() const {
        return _fd != InvalidHandle;
    }

    int FileStream::openFlag(const char *filename, FileMode mode, FileAccess access) {
        int result;
        switch (access) {
            case FileAccess::FileRead:
                result = O_RDONLY;
                break;
            case FileAccess::FileWrite:
                result = O_WRONLY;
                break;
            case FileAccess::FileReadWrite:
                result = O_RDWR;
                break;
            default:
                result = O_RDONLY;
                break;
        }

        switch (mode) {
            case FileCreateNew:
                return O_CREAT | O_BINARY;
            case FileCreate:
                return O_CREAT | result | O_BINARY;
            case FileOpen:
                return result | O_BINARY;
            case FileOpenOrCreate:
                return File::exists(filename) ? result | O_BINARY : O_CREAT | O_TRUNC | result | O_BINARY;
            case FileTruncate:
                return O_CREAT | O_TRUNC | O_RDWR | O_BINARY;
            case FileAppend:
                return O_CREAT | O_APPEND | result | O_BINARY;
            case FileOpenWithoutException:
                return result | O_BINARY;
            default:
                return O_RDONLY | O_BINARY;
        }
    }

    int FileStream::openMode(FileAccess access) {
        switch (access) {
            case FileRead:
                return S_IREAD;
            case FileWrite:
                return S_IREAD | S_IWRITE;
            case FileReadWrite:
                return S_IREAD | S_IWRITE;
            default:
                return S_IREAD;
        }
    }

    off_t FileStream::position() const {
#if WIN32
        return isOpen() ? tell(_fd) : -1;
#else
        return isOpen() ? lseek(_fd, 0, SeekCurrent) : -1;
#endif
    }

    size_t FileStream::length() const {
#if WIN32
        return isOpen() ? filelength(_fd) : 0;
#else
        if (isOpen()) {
            struct stat fst{};
            fstat(_fd, &fst);
            return fst.st_size;
        }
        return -1;
#endif
    }

    void FileStream::setLength(size_t length) {
        if (isOpen()) {
            off_t pos = position();
#ifdef WIN32
            auto file = (HANDLE)_get_osfhandle(_fd);
            SetEndOfFile(file);
#else
            ftruncate(_fd, (off_t) length);
#endif
            if (pos != length) {
                if (pos < (off_t) length) {
                    seek(pos, SeekBegin);
                } else {
                    seek(0, SeekEnd);
                }
            }
        }
    }

    off_t FileStream::seek(off_t offset, SeekOrigin origin) {
        if (isOpen()) {
            return ::lseek(_fd, offset, origin);
        }
        return -1;
    }

    bool FileStream::canWrite() const {
        return (_access & FileAccess::FileWrite) == FileAccess::FileWrite;
    }

    bool FileStream::canRead() const {
        return (_access & FileAccess::FileRead) == FileAccess::FileRead;
    }

    ssize_t FileStream::write(const uint8_t *array, off_t offset, size_t count) {
        if (isOpen()) {
            return ::write(_fd, array + offset, (uint32_t) count);
        }
        return 0;
    }

    ssize_t FileStream::read(uint8_t *array, off_t offset, size_t count) {
        if (isOpen()) {
            return ::read(_fd, array + offset, (uint32_t) count);
        }
        return 0;
    }

    void FileStream::flush() {
        if (isOpen()) {
#if WIN32
            _commit(_fd);
#else
            tcflush(_fd, TCIOFLUSH);
#endif
        }
    }

    int FileStream::fd() const {
        return _fd;
    }

    const String &FileStream::fileName() const {
        return _fileName;
    }
}
