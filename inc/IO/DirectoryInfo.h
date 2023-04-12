//
//  DirectoryInfo.h
//  common
//
//  Created by baowei on 2023/1/19.
//  Copyright (c) 2023 com. All rights reserved.
//

#ifndef DirectoryInfo_h
#define DirectoryInfo_h

#include "data/StringArray.h"
#include "data/String.h"

using namespace Data;

namespace IO {
    class DirectoryInfo {
    public:
        DirectoryInfo(const String &path);

        bool exists() const;

        String name() const;

        DirectoryInfo parent() const;

        DirectoryInfo root() const;

        bool createDirectory() const;

        bool createSubDirectory(const String &path) const;

        bool deleteDirectory() const;
    };
}

#endif // DirectoryInfo_h
