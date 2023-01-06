//
//  MappingStream.cpp
//  common
//
//  Created by baowei on 2015/9/20.
//  Copyright © 2015 com. All rights reserved.
//

#include "IO/MappingStream.h"
#include "IO/File.h"
#include "system/Math.h"
#include "exception/Exception.h"
#if WIN32
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

using namespace Diag;
using namespace System;

namespace IO {
    MappingStream::View::View(ViewMapping mapping, FileAccess access, off_t offset, size_t size) {
        this->_mapping = mapping;
        this->offset = offset;
        this->size = size;
        this->_access = access;
#if WIN32
        _accessor = nullptr;
#else
        _accessor = MAP_FAILED;
#endif
    }

    MappingStream::View::~View() {
        close();
    }

    ssize_t MappingStream::View::write(off_t position, const uint8_t *array, off_t offset, size_t count) {
        updateViewAccessor();

        if (isValid()) {
            memcpy((uint8_t *) _accessor + position, array + offset, count);

            return count;
        }

        return 0;
    }

    ssize_t MappingStream::View::read(off_t position, uint8_t *array, off_t offset, size_t count) {
        updateViewAccessor();

        if (isValid()) {
            memcpy(array + offset, (uint8_t *) _accessor + position, count);
            return count;
        }

        return 0;
    }

    void MappingStream::View::close() {
        if (isValid()) {
            if (canWrite())
                flush();
#if WIN32
            UnmapViewOfFile(_accessor);
            _accessor = nullptr;
#else
            if ((munmap(_accessor, (size_t) size)) == -1) {
                char *mesg = strerror(errno);
                Debug::writeFormatLine("munmap wrong!, reason: %s", mesg);
            }
            _accessor = MAP_FAILED;
#endif
        }
    }

    bool MappingStream::View::flush() {
        if (isValid()) {
#if WIN32
#else
            if (canWrite()) {
                if ((msync(_accessor, (size_t) size, MS_SYNC)) == -1) {
                    char *mesg = strerror(errno);
                    Debug::writeFormatLine("msync wrong!, reason: %s", mesg);
                    return false;
                }
            }
#endif
            return true;
        }
        return false;
    }

    bool MappingStream::View::isValid() const {
#if WIN32
        return _accessor != nullptr;
#else
        return _accessor != MAP_FAILED;
#endif
    }

    void MappingStream::View::updateViewAccessor() {
        if (!isValid()) {
#if WIN32
            DWORD access = _access == FileAccess::FileReadWrite ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
            if((_accessor = MapViewOfFile(_mapping, access, 0, (DWORD)offset, (SIZE_T)size)) == nullptr)
            {
                LPVOID lpMsgBuf;
                DWORD dw = GetLastError();

                FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    dw,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&lpMsgBuf,
                    0, nullptr);

                Debug::writeFormatLine("mmap wrong!, reason: %s", (LPTSTR)lpMsgBuf);
                LocalFree(lpMsgBuf);
            }
#else
#if !defined(MAP_NOCACHE)
#define MAP_NOCACHE 0
#endif
            int access = _access == FileAccess::FileReadWrite ? PROT_READ | PROT_WRITE : PROT_READ;
            if ((_accessor = mmap(nullptr, (size_t) size, access, MAP_SHARED | MAP_NOCACHE, _mapping, offset)) ==
                MAP_FAILED) {
                int errnum = errno;
                char *mesg = strerror(errnum);
                Debug::writeFormatLine("mmap wrong!, reason: %s", mesg);
            }
#endif
        }
    }

    bool MappingStream::View::canWrite() const {
        return (_access & FileAccess::FileWrite) == FileAccess::FileWrite;
    }

    bool MappingStream::View::canRead() const {
        return (_access & FileAccess::FileRead) == FileAccess::FileRead;
    }

    MappingStream::MappingStream(const String &fileName, size_t fileSize) {
        if (fileName.isNullOrEmpty()) {
            throw new ArgumentNullException("fileName");
        }
        if (fileSize <= 0) {
            throw ArgumentOutOfRangeException("fileSize", "fileSize must be greater than or equal to zero.");
        }
        if (fileSize > (int64_t) 2 * 1024 * 1024 * 1024)    // 2G
        {
            // 2G is the largest.
            throw ArgumentOutOfRangeException("fileSize", "fileSize must be less than 2G.");
        }

        if (!File::exists(fileName)) {
            FileStream fs(fileName, FileMode::FileCreate, FileAccess::FileReadWrite);
            fs.setLength(fileSize);
        }
        FileAccess access = FileAccess::FileReadWrite;

        ViewMapping mapping;
#if WIN32
        _file = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
        _mapFile = CreateFileMapping(_file, nullptr, PAGE_READWRITE, 0, (DWORD)fileSize, nullptr);
        mapping = _mapFile;
#else
        _mapFile = new FileStream(fileName, FileMode::FileOpen, access);
        mapping = _mapFile->_fd;
#endif

        _fileSize = fileSize;
        _access = access;
        _position = 0;

        Views views;
        size_t offset = 0, size = 0;
        size_t vsize = viewSize();
        for (size_t i = 0; i < fileSize; i += vsize) {
            offset = i;
            size = Math::min(vsize, (fileSize - i));
            _views.add(new View(mapping, access, (off_t)offset, size));
        }
    }

    MappingStream::MappingStream(const String &fileName) {
        if (fileName.isNullOrEmpty()) {
            throw new ArgumentNullException("fileName");
        }
        if (!File::exists(fileName)) {
            throw new FileNotFoundException("fileName");
        }
        FileAccess access = FileAccess::FileRead;
        size_t fileSize = 0;

        ViewMapping mapping;
#if WIN32
        _file = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
        _mapFile = CreateFileMapping(_file, nullptr, PAGE_READONLY, 0, (DWORD)fileSize, nullptr);
        mapping = _mapFile;
        LARGE_INTEGER li;
        GetFileSizeEx(_file, &li);
        fileSize = li.QuadPart;
#else
        _mapFile = new FileStream(fileName, FileMode::FileOpen, access);
        mapping = _mapFile->_fd;
        fileSize = _mapFile->length();
#endif

        _fileSize = fileSize;
        _access = access;
        _position = 0;

        Views views;
        size_t offset = 0, size = 0;
        size_t vsize = viewSize();
        for (size_t i = 0; i < fileSize; i += vsize) {
            offset = i;
            size = Math::min(vsize, (fileSize - i));
            _views.add(new View(mapping, access, (off_t)offset, size));
        }
    }

    MappingStream::~MappingStream() {
        close();
    }

    ssize_t MappingStream::write(const uint8_t *array, off_t offset, size_t count) {
        if (array == nullptr) {
            throw ArgumentNullException("array cannot be nullptr.");
        }
        if (offset < 0) {
            throw ArgumentOutOfRangeException("offset", "MappingStream::read, Non-negative number required.");
        }
        if (count < 0) {
            throw ArgumentOutOfRangeException("count", "MappingStream::read, Non-negative number required.");
        }

        if (!isOpen()) {
            throw StreamException("Cannot access a closed file.");
        }
        if (!canWrite()) {
            throw NotSupportedException("Stream does not support writing.");
        }

        size_t size = 0;
        size_t length = 0;
        size_t pos = position();
        for (size_t i = 0; i < _views.count(); i++) {
            View *view = _views[i];
            size += view->size;
            if ((off_t) size > position()) {
                length = Math::min((int64_t) (count - length), (int64_t) (view->size - (position() - view->offset)));
                view->write(position() - view->offset, array, offset, length);
                offset += (off_t)length;
                pos += length;
                seek((off_t)pos, SeekOrigin::SeekBegin);
                if (offset >= (off_t) count)
                    break;
            }
        }
        return length;
    }

    ssize_t MappingStream::read(uint8_t *array, off_t offset, size_t count) {
        if (array == nullptr) {
            throw ArgumentNullException("array");
        }
        if (offset < 0) {
            throw ArgumentOutOfRangeException("offset", "MappingStream::read, Non-negative number required.");
        }
        if (count < 0) {
            throw ArgumentOutOfRangeException("count", "MappingStream::read, Non-negative number required.");
        }

        if (!isOpen()) {
            throw StreamException("Cannot access a closed file.");
        }
        if (!canRead()) {
            throw NotSupportedException("Stream does not support reading.");
        }

        size_t size = 0;
        ssize_t length = 0;
        off_t pos = position();
        for (size_t i = 0; i < _views.count(); i++) {
            View *view = _views[i];
            size += view->size;
            if ((off_t) size > position()) {
                length += view->read(position() - view->offset, array, offset + (off_t)length, count - length);
                pos += (off_t)length;
                seek(pos, SeekOrigin::SeekBegin);
                if (length >= (ssize_t)count)
                    break;
            }
        }
        return length;
    }

    off_t MappingStream::position() const {
        if (isOpen()) {
            return _position;
        }
        return 0;
    }

    size_t MappingStream::length() const {
        if (isOpen()) {
            return _fileSize;
        }
        return 0;
    }

    bool MappingStream::seek(off_t offset, SeekOrigin origin) {
        if ((origin < SeekOrigin::SeekBegin) || (origin > SeekOrigin::SeekEnd)) {
            throw ArgumentException("Invalid seek origin.");
        }
        if (!isOpen()) {
            throw StreamException("Cannot access a closed file.");
        }
        if (!canSeek()) {
            throw NotSupportedException("Stream does not support seeking.");
        }

        switch (origin) {
            case SeekOrigin::SeekBegin:
                break;
            case SeekOrigin::SeekCurrent:
                offset += position();
                break;
            case SeekOrigin::SeekEnd:
                offset = (off_t)length() - offset;
                break;
            default:
                break;
        }

        _position = offset;
        return true;
    }

    bool MappingStream::canWrite() const {
        return (_access & FileAccess::FileWrite) == FileAccess::FileWrite;
    }

    bool MappingStream::canRead() const {
        return (_access & FileAccess::FileRead) == FileAccess::FileRead;
    }

    bool MappingStream::canSeek() const {
        return true;
    }

    size_t MappingStream::viewSize() const {
        if (canWrite()) {
            return WriteViewSize;
        }
        return ReadViewSize;
    }

    bool MappingStream::isOpen() const {
        return _mapFile != nullptr;
    }

    void MappingStream::close() {
        if (isOpen()) {
            for (size_t i = 0; i < _views.count(); i++) {
                View *view = _views[i];
                view->close();
            }
            _views.clear();

#if WIN32
            CloseHandle(_file);
            _file = nullptr;
            CloseHandle(_mapFile);
#else
            delete _mapFile;
#endif
            _mapFile = nullptr;
        }
    }

    void MappingStream::flush() {
        if (canWrite()) {
            for (uint32_t i = 0; i < _views.count(); i++) {
                View *view = _views[i];
                view->flush();
            }
        }
    }
}
