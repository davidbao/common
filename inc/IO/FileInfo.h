//
//  FileInfo.h
//  common
//
//  Created by baowei on 2015/7/21.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef FileInfo_h
#define FileInfo_h

#include <cstdint>
#include "data/String.h"
#include "IO/File.h"
#include "data/DateTime.h"

namespace IO {
    class FileInfo {
    public:
        enum FileAttributes {
            Unknown = 0x0,
            Read = 0x0100,
            Write = 0x0080,
            Execute = 0x0040,
            Regular = 0x8000,
        };

        FileInfo(const String &name);

        FileAttributes attributes() const;

        bool isReadOnly() const;

        bool isWritable() const;

        int64_t size() const;

        bool exists() const;

        DateTime modifiedTime() const;

    private:
        void stat();

    private:
        String _name;

        FileInfo::FileAttributes _attributes;
        int64_t _size;
        DateTime _modifiedTime;
    };
}

#endif // FileInfo_h
