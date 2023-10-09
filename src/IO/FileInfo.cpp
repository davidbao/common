//
//  FileInfo.cpp
//  common
//
//  Created by baowei on 2015/7/21.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "IO/File.h"
#include "IO/FileInfo.h"

#if WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#elif __APPLE__

#include <sys/stat.h>

#else

#include <sys/vfs.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#endif

namespace IO {
    FileInfo::FileInfo(const String &name) {
        _name = name;
        _attributes = FileAttributes::Unknown;
        _size = 0;

        stat();
    }

    FileInfo::FileAttributes FileInfo::attributes() const {
        return _attributes;
    }

    bool FileInfo::isReadOnly() const {
        FileAttributes attr = attributes();
        return ((attr & Read) != 0) && ((attr & Write) == 0);
    }

    bool FileInfo::isWritable() const {
        FileAttributes attr = attributes();
        return ((attr & Write) != 0);
    }

    off_t FileInfo::size() const {
        return _size;
    }

    bool FileInfo::exists() const {
        return File::exists(_name);
    }

    DateTime FileInfo::modifiedTime() const {
        return _modifiedTime;
    }

    void FileInfo::stat() {
        if (exists()) {
            struct stat buffer{};
            int result = ::stat(_name.c_str(), &buffer);
            if (result == 0) {
                _attributes = (FileInfo::FileAttributes) buffer.st_mode;
                _size = buffer.st_size;

                _modifiedTime = DateTime::fromLocalTime(buffer.st_mtime);
            }
        }
    }
}
