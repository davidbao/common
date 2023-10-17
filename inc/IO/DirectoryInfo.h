//
//  DirectoryInfo.h
//  common
//
//  Created by baowei on 2023/1/19.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef DirectoryInfo_h
#define DirectoryInfo_h

#include "data/String.h"
#include "data/StringArray.h"
#include "IO/Directory.h"

using namespace Data;

namespace IO {
    class DirectoryInfo {
    public:
        explicit DirectoryInfo(const String &path);

        DirectoryInfo(const DirectoryInfo &other);

        bool exists() const;

        String name() const;

        String fullName() const;

        DirectoryInfo parent() const;

        DirectoryInfo root() const;

        DateTime modifiedTime() const;

        DateTime creationTime() const;

        DateTime lastAccessTime() const;

        bool createDirectory() const;

        bool createSubDirectory(const String &path) const;

        bool deleteDirectory(bool recursive = true) const;

        StringArray getFiles(const String &searchPattern = "*.*",
                      SearchOption searchOption = SearchOption::AllDirectories) const;

        StringArray getDirectories(const String &searchPattern = "*",
                            SearchOption searchOption = SearchOption::AllDirectories) const;

    private:
        String _path;
    };
}

#endif // DirectoryInfo_h
