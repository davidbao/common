//
//  File.h
//  common
//
//  Created by baowei on 2015/9/26.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef File_h
#define File_h

#ifndef WIN32

#include <cstdio>
#include <sys/stat.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cerrno>
#include <unistd.h>

#endif

#include "data/String.h"
#include "data/DateTime.h"
#include "data/StringArray.h"

namespace IO {
    class File {
    public:
        static void appendBytes(const String &path, const ByteArray &content);

        static void appendLines(const String &path, const StringArray &contents);

        static void appendText(const String &path, const String &content);

        static bool copy(const String &sourceFileName, const String &destFileName, bool overwrite = true);

        static bool deleteFile(const String &path);

        static bool exists(const String &path);

        static bool getCreationTime(const String &path, DateTime &time);

        static bool getLastAccessTime(const String &path, DateTime &time);

        static bool getLength(const String &path, int64_t &fileLength);

        static int64_t getLength(const String &path);

        static bool getModifyTime(const String &path, DateTime &time);

        static bool move(const String &sourceFileName, const String &destFileName);

        static ByteArray readAllBytes(const String &path);

        static StringArray readAllLines(const String &path);

        static String readAllText(const String &path);

        static bool rename(const String &oldFile, const String &newFile);

        static bool setCreationTime(const String &path, const DateTime &time);

        static bool setLastAccessTime(const String &path, const DateTime &time);

        static bool setModifyTime(const String &path, const DateTime &time);

#ifndef WIN32

        static bool chmod(const String &fileName, int mode = 0x777);

#endif

    private:
#ifndef WIN32

        static bool write_file(int src_fd, int dest_fd, struct stat *st_src);

        /**
         * CopyFile:
         * @name: a pointer to a NULL-terminated unicode String, that names
         * the file to be copied.
         * @dest_name: a pointer to a NULL-terminated unicode String, that is the
         * new name for the file.
         * @fail_if_exists: if TRUE and dest_name exists, the copy will fail.
         *
         * Copies file @name to @dest_name
         *
         * Return value: %TRUE on success, %FALSE otherwise.
         */
        static bool copy_file(const char *name, const char *dest_name, bool fail_if_exists);

        /**
         * MoveFile:
         * @name: a pointer to a NULL-terminated unicode String, that names
         * the file to be moved.
         * @dest_name: a pointer to a NULL-terminated unicode String, that is the
         * new name for the file.
         *
         * Renames file @name to @dest_name.
         * MoveFile sets ERROR_ALREADY_EXISTS if the destination exists, except
         * when it is the same file as the source.  In that case it silently succeeds.
         *
         * Return value: %TRUE on success, %FALSE otherwise.
         */
        static bool move_file(const char *name, const char *dest_name);

#endif
    };
}

#endif // File_h
