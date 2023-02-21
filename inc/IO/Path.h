//
//  Path.h
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Path_h
#define Path_h

#include "data/String.h"
#include "data/Array.h"
#include "data/ByteArray.h"
#include "data/StringArray.h"

using namespace Data;

namespace IO {
    class Path {
    public:
        static String changeExtension(const String &path, const String &extension);

        static String combine(const String &path1, const String &path2);

        static String combine(const String &path1, const String &path2, const String &path3);

        static String combine(const String &path1, const String &path2, const String &path3, const String &path4);

        static String combine(const StringArray &paths);

        static bool endsInDirectorySeparator(const String &path);

        static String getAppPath();

        static String getDirectoryName(const String &path);

        static String getDocumentPath(const String &subDocPath = String::Empty);

        static String getExtension(const String &path);

        static String getFileName(const String &path);

        static String getFileNameWithoutExtension(const String &path);

        static String getFullPath(const String &path);

        static String getFullPath(const String &path, const String &basePath);

        static String getHomePath();

        static const Array<char, 41> &getInvalidFileNameChars();

        static const Array<char, 36> &getInvalidPathChars();

        static String getPathRoot(const String &path);

        static String getRandomFileName();

        static String getTempFileName(const String &prefix = String::Empty);

        static String getTempPath();

        static bool hasExtension(const String &path);

        static bool isPathRooted(const String &path);

    private:
        static bool hasIllegalCharacters(const String &path, bool checkAdditional);

        static String combineNoChecks(const String &path1, const String &path2);

        static bool checkInvalidPathChars(const String &path, bool checkAdditional = false);

        static String toBase32StringSuitableForDirName(const ByteArray &buff);

        static bool isDirectorySeparatorChar(char ch);

    public:
#if WIN32
        static const char DirectorySeparatorChar = '\\';
#else
        static const char DirectorySeparatorChar = '/';
#endif

    private:
        static const char AltDirectorySeparatorChar = '/';
        static const int MaxDirectoryPath = 0xf8;
        static const int MaxDirectoryLength = 0xff;
        static const int MaxLongPath = 0x7d00;
        static const int MaxPath = 4096;
        static const char PathSeparator = ';';
        static const char VolumeSeparatorChar = ':';

        static const char Prefix[];
        static const char InvalidFileNameChars[];
        static const char InvalidPathChars[];
        static const char s_Base32Char[];
        static const char TrimEndChars[];

    };
}

#endif // Path_h
