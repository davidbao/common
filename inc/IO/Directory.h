//
//  Directory.h
//  common
//
//  Created by baowei on 2016/12/18.
//  Copyright (c) 2016 com. All rights reserved.
//

#ifndef Directory_h
#define Directory_h

#include "data/StringArray.h"
#include "data/String.h"

namespace IO {
    enum SearchOption {
        TopDirectoryOnly,
        AllDirectories
    };

    class Directory {
    public:
        static bool createDirectory(const String &path);

        static bool deleteDirectory(const String &path);

        static bool exists(const String &path);

        static bool
        getFiles(const String &path, const String &searchPattern, SearchOption searchOption, StringArray &files);

        static bool getDirectories(const String &path, const String &searchPattern, SearchOption searchOption,
                                   StringArray &directories);

        static bool copy(const String &sourcePath, const String &destPath, const StringArray *excludedItems = nullptr);

        static bool rename(const String &oldPath, const String &newPath);

        static String getCurrentDirectory();

        static bool setCurrentDirectory(const String &path);

        static bool getModifyTime(const String &path, DateTime &time);

        static bool setModifyTime(const String &path, const DateTime &time);

        static bool getCreationTime(const String &path, DateTime &time);

        static bool setCreationTime(const String &path, const DateTime &time);

        static bool getLastAccessTime(const String &path, DateTime &time);

        static bool setLastAccessTime(const String &path, const DateTime &time);

    private:
        static bool createDirectoryInner(const char *path);

        static int remove_directory(const char *path);
    };
}

#endif // Directory_h
