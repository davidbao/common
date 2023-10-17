//
//  DirectoryInfo.cpp
//  common
//
//  Created by baowei on 2023/1/19.
//  Copyright (c) 2023 com. All rights reserved.
//

#include "IO/DirectoryInfo.h"
#include "IO/Path.h"

namespace IO {
    DirectoryInfo::DirectoryInfo(const String &path) : _path(path) {
    }

    DirectoryInfo::DirectoryInfo(const DirectoryInfo &other) : _path(other._path) {
    }

    bool DirectoryInfo::exists() const {
        return Directory::exists(_path);
    }

    String DirectoryInfo::name() const {
        return Path::getFileName(_path);
    }

    String DirectoryInfo::fullName() const {
        return _path;
    }

    DirectoryInfo DirectoryInfo::parent() const {
        return DirectoryInfo(Path::getDirectoryName(_path));
    }

    DirectoryInfo DirectoryInfo::root() const {
        return DirectoryInfo(Path::getPathRoot(_path));
    }

    DateTime DirectoryInfo::modifiedTime() const {
        DateTime time;
        Directory::getModifyTime(_path, time);
        return time;
    }

    DateTime DirectoryInfo::creationTime() const {
        DateTime time;
        Directory::getCreationTime(_path, time);
        return time;
    }

    DateTime DirectoryInfo::lastAccessTime() const {
        DateTime time;
        Directory::getLastAccessTime(_path, time);
        return time;
    }

    bool DirectoryInfo::createDirectory() const {
        return Directory::createDirectory(_path);
    }

    bool DirectoryInfo::createSubDirectory(const String &path) const {
        return Directory::createDirectory(Path::combine(_path, path));
    }

    bool DirectoryInfo::deleteDirectory(bool recursive) const {
        if (recursive) {
            return Directory::deleteDirectory(_path);
        } else {
            return false;
        }
    }

    StringArray DirectoryInfo::getFiles(const String &searchPattern, SearchOption searchOption) const {
        StringArray files;
        if (Directory::getFiles(_path, searchPattern, searchOption, files)) {
            return files;
        }
        return StringArray::Empty;
    }

    StringArray DirectoryInfo::getDirectories(const String &searchPattern, SearchOption searchOption) const {
        StringArray directories;
        if (Directory::getDirectories(_path, searchPattern, searchOption, directories)) {
            return directories;
        }
        return StringArray::Empty;
    }
}
