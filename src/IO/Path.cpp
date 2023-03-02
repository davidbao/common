//
//  Path.cpp
//  common
//
//  Created by baowei on 2015/7/14.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "IO/Path.h"
#include "exception/Exception.h"
#include "diag/Trace.h"
#include "system/Random.h"
#include "system/Environment.h"
#include "system/Application.h"

#ifdef WIN32

#include <Windows.h>
#include <memory.h>
#include <io.h>
#include <direct.h>
#include <Shlwapi.h>
#include <shlobj_core.h>

#elif __APPLE__

#include <unistd.h>

#elif __arm_linux__

#include <libgen.h>
#include <pwd.h>
#include <unistd.h>

#else

#include <unistd.h>
#include <dirent.h>
#include <cstdlib>

#endif

using namespace Diag;
using namespace System;

namespace IO {
    const char Path::InvalidFileNameChars[41] = {'\"', '<', '>', '|', '\0', (char) 1, (char) 2, (char) 3, (char) 4,
                                                 (char) 5, (char) 6, (char) 7, (char) 8, (char) 9, (char) 10, (char) 11,
                                                 (char) 12, (char) 13, (char) 14, (char) 15, (char) 16, (char) 17,
                                                 (char) 18, (char) 19, (char) 20, (char) 21, (char) 22, (char) 23,
                                                 (char) 24, (char) 25, (char) 26, (char) 27, (char) 28, (char) 29,
                                                 (char) 30, (char) 31, ':', '*', '?', '\\', '/'};
    const char Path::InvalidPathChars[36] = {
            '\"', '<', '>', '|', '\0',
            (char) 1, (char) 2, (char) 3, (char) 4, (char) 5, (char) 6, (char) 7, (char) 8, (char) 9, (char) 10,
            (char) 11, (char) 12, (char) 13, (char) 14, (char) 15, (char) 16, (char) 17, (char) 18, (char) 19,
            (char) 20,
            (char) 21, (char) 22, (char) 23, (char) 24, (char) 25, (char) 26, (char) 27, (char) 28, (char) 29,
            (char) 30,
            (char) 31
    };
    const char Path::s_Base32Char[32] = {
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
            'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5'
    };
    const char Path::TrimEndChars[8] = {'\t', '\n', '\v', '\f', '\r', ' ', '\x0085', '\x00a0'};

    String Path::changeExtension(const String &path, const String &extension) {
        if (!path.isNullOrEmpty()) {
            if (!checkInvalidPathChars(path)) {
                return String::Empty;
            }

            String s = path;
            for (size_t i = path.length(); --i >= 0;) {
                char ch = path[i];
                if (ch == '.') {
                    s = path.substr(0, i);
                    break;
                }
                if (isDirectorySeparatorChar(ch)) break;
            }
            if (!extension.isNullOrEmpty()) {
                if (extension.length() == 0 || extension[0] != '.') {
                    s = s + ".";
                }
                s = s + extension;
            }
            return s;
        }
        return String::Empty;
    }

    String Path::combine(const String &path1, const String &path2) {
        if (!checkInvalidPathChars(path1, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path2, false)) {
            return String::Empty;
        }

        return combineNoChecks(path1, path2);
    }

    String Path::combine(const String &path1, const String &path2, const String &path3) {
        if (!checkInvalidPathChars(path1, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path2, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path3, false)) {
            return String::Empty;
        }

        return combineNoChecks(combineNoChecks(path1, path2), path3);
    }

    String Path::combine(const String &path1, const String &path2, const String &path3, const String &path4) {
        if (!checkInvalidPathChars(path1, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path2, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path3, false)) {
            return String::Empty;
        }
        if (!checkInvalidPathChars(path4, false)) {
            return String::Empty;
        }

        return combineNoChecks(combineNoChecks(combineNoChecks(path1, path2), path3), path4);
    }

    String Path::combine(const StringArray &paths) {
        if (paths.count() == 0) {
            return String::Empty;
        }

        size_t finalSize = 0;
        size_t firstComponent = 0;

        // We have two passes, the first calculates how large a buffer to allocate and does some precondition
        // checks on the paths passed in.  The second actually does the combination.

        for (size_t i = 0; i < paths.count(); i++) {
            const String &path = paths[i];
            if (path.length() == 0) {
                continue;
            }

            if (!checkInvalidPathChars(path)) {
                return String::Empty;
            }

            if (Path::isPathRooted(path)) {
                firstComponent = i;
                finalSize = path.length();
            } else {
                finalSize += path.length();
            }

            char ch = paths[i][path.length() - 1];
            if (!isDirectorySeparatorChar(ch))
                finalSize++;
        }

        String finalPath;
        for (size_t i = firstComponent; i < paths.count(); i++) {
            const String &path = paths[i];
            if (path.length() == 0) {
                continue;
            }

            if (finalPath.length() == 0) {
                finalPath.append(path);
            } else {
                char ch = finalPath[finalPath.length() - 1];
                if (!isDirectorySeparatorChar(ch)) {
                    finalPath.append(DirectorySeparatorChar);
                }

                finalPath.append(path);
            }
        }

        return finalPath;
    }

    bool Path::endsInDirectorySeparator(const String &path) {
        if (path.isNullOrEmpty()) {
            return false;
        }

        return path[path.length() - 1] == DirectorySeparatorChar;
    }

    String Path::getAppPath() {
        return Path::getDirectoryName(Application::startupPath());
    }

    String Path::getDirectoryName(const String &path) {
        if (!checkInvalidPathChars(path)) {
            return String::Empty;
        }

        ssize_t length = path.findLastOf(DirectorySeparatorChar);
        if (length < 0) {
#ifdef WIN32
            length = path.findLastOf('/');
            if (length < 0)
                return path;
            else
                return path.substr(0, length);
#else
            return path;
#endif
        } else if (length == 0) {
            return DirectorySeparatorChar;
        } else {
            if (endsInDirectorySeparator(path)) {
                return getDirectoryName(path.substr(0, length));
            } else {
                return path.substr(0, length);
            }
        }
    }

    String Path::getDocumentPath(const String &subDocPath) {
        String docPath = Path::getHomePath();
        assert(!docPath.isNullOrEmpty());
        if (subDocPath.isNullOrEmpty()) {
            docPath = Path::getAppPath();
        } else {
            String tempPath = subDocPath;
#ifdef WIN32
            if (subDocPath.find('/') >= 0) {
                tempPath = subDocPath.replace(subDocPath, "/", "\\");
            }
#endif
            docPath = Path::combine(docPath, tempPath);
        }
        return docPath;
    }

    String Path::getExtension(const String &path) {
        if (!checkInvalidPathChars(path)) {
            return String::Empty;
        }

        String fileName = getFileName(path);
        if (fileName.isNullOrEmpty()) {
            fileName = path;
        }
        ssize_t length = fileName.findLastOf('.');
        if (length < 0) {
            return String::Empty;
        }
        return fileName.substr(length, fileName.length() - 1);
    }

    String Path::getFileName(const String &path) {
        if (!path.isNullOrEmpty()) {
            checkInvalidPathChars(path, false);
            int length = (int) path.length();
            int position = length;
            while (--position >= 0) {
                char ch = path[position];
                if (isDirectorySeparatorChar(ch)) {
                    return path.substr((off_t) position + 1, (size_t) ((length - position) - 1));
                }
            }
            return path;
        }
        return String::Empty;
    }

    String Path::getFileNameWithoutExtension(const String &path) {
        String pathStr = getFileName(path);
        if (pathStr.isNullOrEmpty()) {
            return String::Empty;
        }
        ssize_t length = pathStr.findLastOf('.');
        if (length < 0) {
            return pathStr;
        }
        return pathStr.substr(0, length);
    }

    String Path::getFullPath(const String &path) {
        return getFullPath(path, Environment::getCurrentDirectory());
    }

    String Path::getFullPath(const String &path, const String &basePath) {
        if (!checkInvalidPathChars(path)) {
            return String::Empty;
        }
        if (isPathRooted(path)) {
            return path;
        }

        return combine(basePath, path);
    }

    String Path::getHomePath() {
#ifdef WIN32
        char path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PROFILE, nullptr, 0, path))) {
            return path;
        }
        const char *homeDir = getenv("HOMEPATH");
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#elif __EMSCRIPTEN__
        return Path::combine(getAppPath(), "home");
#elif __arm_linux__
        const char *homeDir = getenv("HOME");
        if (!homeDir) {
            struct passwd *pwd = getpwuid(getuid());
            if (pwd)
                homeDir = pwd->pw_dir;
        }
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#else
        const char *homeDir = getenv("HOME");
        return homeDir != nullptr ? (String) homeDir : String::Empty;
#endif
    }

    const Array<char, 41> &Path::getInvalidFileNameChars() {
        static const Array<char, 41> chars(InvalidFileNameChars, 41);
        return chars;
    }

    const Array<char, 36> &Path::getInvalidPathChars() {
        static const Array<char, 36> chars(InvalidPathChars, 36);
        return chars;
    }

    String Path::getPathRoot(const String &path) {
        if (path.isNullOrEmpty()) {
            return String::Empty;
        }

#ifdef WIN32
        if (path.length() >= 2) {
            if (path[1] == VolumeSeparatorChar) {
                String result = path.substr(0, 2);
                result.append(DirectorySeparatorChar);
                return result;
            } else if (path[0] == DirectorySeparatorChar && path[1] == DirectorySeparatorChar) {
                // UNC path.
                ssize_t pos = path.find(DirectorySeparatorChar, 2);
                if (pos > 0) {
                    String result = path.substr(0, pos);
                    result.append(DirectorySeparatorChar);
                    return result;
                }
            }
        }
#else
        if (path[0] == DirectorySeparatorChar) {
            return {DirectorySeparatorChar};
        }
#endif
        return String::Empty;
    }

    String Path::getRandomFileName() {
        // 5 bytes == 40 bits == 40/5 == 8 chars in our encoding
        // This gives us exactly 8 chars. We want to avoid the 8.3 short name issue
        ByteArray key;
        Random::getRandValues(10, key);
        String rnsStr = toBase32StringSuitableForDirName(key);
        rnsStr[8] = '.';
        return rnsStr.substr(0, 12);
    }

    String Path::getTempFileName(const String &prefix) {
        String path = getTempPath();
#ifdef WIN32
        char tmp[MAX_PATH];
        memset(tmp, 0, sizeof(tmp));
        UINT result = GetTempFileName(path,
                                      !prefix.isNullOrEmpty() ? prefix : nullptr,
                                      0, tmp);
        if (result != 0) {
            return tmp;
        }
#else
        String name = Path::combine(path, String::format("%s%s", prefix.c_str(), "XXXXXX"));

        char tmp[PATH_MAX];
        memset(tmp, 0, sizeof(tmp));
        strncpy(tmp, name, PATH_MAX - 1);
        int fd = mkstemp(tmp);
        if (fd != -1) {
            close(fd);
            return tmp;
        }
#endif
        return String::Empty;
    }

    String Path::getTempPath() {
#ifdef WIN32
        char path[MAX_PATH];
        memset(path, 0, sizeof(path));
        if (GetTempPath(sizeof(path), path) > 0)
            return (String) path;
        return String::Empty;
#else
        //        On POSIX systems, the path may be the one specified in the environment variables TMPDIR, TMP, TEMP, TEMPDIR,
        //        and, if none of them are specified, the path "/tmp" is returned.
        String temp;
        if (Environment::getVariable("TMPDIR", temp)) {
            return temp;
        }
        if (Environment::getVariable("TMP", temp)) {
            return temp;
        }
        if (Environment::getVariable("TEMP", temp)) {
            return temp;
        }
        if (Environment::getVariable("TEMPDIR", temp)) {
            return temp;
        }
        return "/tmp";
#endif
    }

    bool Path::hasExtension(const String &path) {
        if (!path.isNullOrEmpty()) {
            if (!checkInvalidPathChars(path)) {
                return false;
            }

            for (size_t i = path.length(); --i >= 0;) {
                char ch = path[i];
                if (ch == '.') {
                    if (i != path.length() - 1)
                        return true;
                    else
                        return false;
                }
                if (isDirectorySeparatorChar(ch)) break;
            }
        }
        return false;
    }

    bool Path::isPathRooted(const String &path) {
        if (!checkInvalidPathChars(path)) {
            return false;
        }

#if WIN32
        return PathIsRelative(path) ? false : true;
#else
        size_t length = path.length();
        if (length >= 1 && path[0] == DirectorySeparatorChar) {
            return true;
        }
        return false;
#endif
    }

    bool Path::hasIllegalCharacters(const String &path, bool checkAdditional) {
        if (path.isNullOrEmpty())
            return false;

        for (char ch: path) {
            for (char InvalidPathChar: InvalidPathChars) {
                if (ch == InvalidPathChar) {
                    return true;
                }
            }
            if (checkAdditional && ((ch == '*') || (ch == '?'))) {
                return true;
            }
        }
        return false;
    }

    bool Path::checkInvalidPathChars(const String &path, bool checkAdditional) {
        if (path.isNullOrEmpty()) {
            return false;
        }
        if (hasIllegalCharacters(path, checkAdditional)) {
            Trace::error(String::format("Illegal characters in path'%s'.", path.c_str()));
            return false;
        }
        return true;
    }

    String Path::combineNoChecks(const String &path1, const String &path2) {
        if (path2.isNullOrEmpty()) {
            return path1;
        }
        if (path1.isNullOrEmpty()) {
            return path2;
        }
        if (isPathRooted(path2)) {
            return path2;
        }

        char ch = path1[path1.length() - 1];
        if (!isDirectorySeparatorChar(ch)) {
            return String::format("%s%c%s", path1.c_str(), DirectorySeparatorChar, path2.c_str());
        }
        return String::format("%s%s", path1.c_str(), path2.c_str());
    }

    String Path::toBase32StringSuitableForDirName(const ByteArray &buff) {
        // This routine is optimised to be used with buffs of length 20
        assert((buff.count() % 5) == 0);

        String sb;
        uint8_t b0, b1, b2, b3, b4;
        size_t l, i;

        l = buff.count();
        i = 0;

        // Create l chars using the last 5 bits of each byte.
        // Consume 3 MSB bits 5 bytes at a time.

        do {
            b0 = (i < l) ? buff[i++] : (uint8_t) 0;
            b1 = (i < l) ? buff[i++] : (uint8_t) 0;
            b2 = (i < l) ? buff[i++] : (uint8_t) 0;
            b3 = (i < l) ? buff[i++] : (uint8_t) 0;
            b4 = (i < l) ? buff[i++] : (uint8_t) 0;

            // Consume the 5 Least significant bits of each byte
            sb.append(s_Base32Char[b0 & 0x1F]);
            sb.append(s_Base32Char[b1 & 0x1F]);
            sb.append(s_Base32Char[b2 & 0x1F]);
            sb.append(s_Base32Char[b3 & 0x1F]);
            sb.append(s_Base32Char[b4 & 0x1F]);

            // Consume 3 MSB of b0, b1, MSB bits 6, 7 of b3, b4
            sb.append(s_Base32Char[(
                    ((b0 & 0xE0) >> 5) |
                    ((b3 & 0x60) >> 2))]);

            sb.append(s_Base32Char[(
                    ((b1 & 0xE0) >> 5) |
                    ((b4 & 0x60) >> 2))]);

            // Consume 3 MSB bits of b2, 1 MSB bit of b3, b4

            b2 >>= 5;

            assert((b2 & 0xF8) == 0);

            if ((b3 & 0x80) != 0)
                b2 |= 0x08;
            if ((b4 & 0x80) != 0)
                b2 |= 0x10;

            sb.append(s_Base32Char[b2]);

        } while (i < l);

        return sb;
    }

    bool Path::isDirectorySeparatorChar(char ch) {
        return ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar;
    }
}
