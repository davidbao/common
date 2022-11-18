#include "IO/Path.h"
#include "data/Convert.h"
#include "exception/Exception.h"
#include "diag/Trace.h"
#if WIN32
#include <Windows.h>
#include <memory.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#include <Shlwapi.h>
//#pragma comment (lib, "Shlwapi.lib")
#elif __APPLE__
#include <sys/param.h>
#include <sys/mount.h>
#include <libgen.h>
#include <dirent.h>
#else
#include <sys/vfs.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#endif

namespace Common
{
	bool Path::isPathRooted(const char* path)
	{
#if WIN32
		return PathIsRelative(path) ? false : true;
#else
        if (path != NULL)
        {
            checkInvalidPathChars(path);
            
            size_t length = strlen(path);
            if ((length >= 1 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar))
                || (length >= 2 && path[1] == VolumeSeparatorChar))
            {
                return true;
            }
        }
        return false;
#endif
	}
	bool Path::isPathRooted(const String& path)
	{
		return isPathRooted(path.c_str());
	}

	String Path::combine(const String& path1, const String& path2)
	{
        checkInvalidPathChars(path1.c_str(), false);
        checkInvalidPathChars(path2.c_str(), false);
        
        return combineNoChecks(path1.c_str(), path2.c_str());
	}

	String Path::getFileName(const char* path)
	{
		if (path != NULL)
		{
			checkInvalidPathChars(path, false);
			int length = (int)strlen(path);
			int num2 = (int)length;
			while (--num2 >= 0)
			{
				char ch = path[num2];
				if (((ch == DirectorySeparatorChar) || (ch == AltDirectorySeparatorChar)) || (ch == VolumeSeparatorChar))
				{
					return ((String)path).substr(num2 + 1, (length - num2) - 1);
				}
			}
            return path;
		}
        return String::Empty;
	}
	String Path::getFileName(const String& path)
	{
		return getFileName(path.c_str());
	}

	String Path::getFileNameWithoutExtension(const char* path)
	{
		String pathStr = getFileName(path);
		if (pathStr.isNullOrEmpty())
		{
            return String::Empty;
		}
		int length = pathStr.findLastOf('.');
        if (length < 0)
		{
			return pathStr;
		}
		return pathStr.substr(0, length);
	}
	String Path::getFileNameWithoutExtension(const String& path)
	{
		return getFileNameWithoutExtension(path.c_str());
	}

	String Path::getDirectoryName(const char* path)
	{
		if (path != NULL)
		{
			checkInvalidPathChars(path, false);
            
            String pathStr = path;
            int length = pathStr.findLastOf(DirectorySeparatorChar);
            if (length < 0)
            {
#if WIN32
				length = (int)pathStr.findLastOf('/');
				if (length < 0)
					return pathStr;
				else
					return pathStr.substr(0, length);
#else
				return pathStr;
#endif
            }
            else if(length == 0)
            {
                return DirectorySeparatorChar;
            }
            else
            {
                return pathStr.substr(0, length);
            }
//#if WIN32
//			char buffer[MAX_PATH];
//			strcpy(buffer, path);
//			PathRemoveFileSpec(buffer);
//			return buffer;
//#endif
		}
        return String::Empty;
	}
	String Path::getDirectoryName(const String& path)
	{
		return getDirectoryName(path.c_str());
	}
	String Path::getExtension(const char* path)
	{
		if (path == NULL)
            return String::Empty;

		String pathStr = getFileName(path);
		if (pathStr.isNullOrEmpty())
		{
			pathStr = path;
		}
		int length = pathStr.findLastOf('.');
		if (length < 0)
		{
			return pathStr;
		}
		return pathStr.substr(length, pathStr.length() - 1);
	}
	String Path::getExtension(const String& path)
	{
		return getExtension(path.c_str());
	}

	bool Path::hasIllegalCharacters(const char* path, bool checkAdditional)
	{
		if (path == NULL)
			return false;

		size_t length = strlen(path);
		for (uint32_t i = 0; i < length; i++)
		{
			uint8_t num2 = path[i];
			if (((num2 == 0x22) || (num2 == 60)) || (((num2 == 0x3e) || (num2 == 0x7c)) || (num2 < (uint8_t)0x20)))
			{
				return true;
			}
			if (checkAdditional && ((num2 == 0x3f) || (num2 == 0x2a)))
			{
				return true;
			}
		}
		return false;
	}
	void Path::checkInvalidPathChars(const char* path, bool checkAdditional)
	{
		if (path == NULL)
		{
            Trace::error("path is null.");
		}
		if (hasIllegalCharacters(path, checkAdditional))
		{
			Trace::error(String::format("Illegal characters in path'%s'.", path));
		}
	}
	String Path::combineNoChecks(const char* path1, const char* path2)
	{
		if (path2 == NULL || strlen(path2) == 0)
		{
			return path1;
		}
		if (path1 == NULL || strlen(path1) == 0)
		{
			return path2;
		}
		if (isPathRooted(path2))
		{
			return path2;
		}

		char ch = path1[strlen(path1) - 1];
		if (((ch != DirectorySeparatorChar) && (ch != AltDirectorySeparatorChar)) && (ch != VolumeSeparatorChar))
		{
			return String::convert("%s%c%s", path1, DirectorySeparatorChar, path2);
		}
		return String::convert("%s%s", path1, path2);
	}
}
