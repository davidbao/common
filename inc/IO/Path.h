#ifndef PATH_H
#define PATH_H

#include "data/ValueType.h"

namespace Common
{
	class Path
	{
	public:
		static bool isPathRooted(const char* path);
		static bool isPathRooted(const String& path);
		static String combine(const String& path1, const String& path2);
		static String getFileName(const char* path);
		static String getFileName(const String& path);
		static String getFileNameWithoutExtension(const char* path);
		static String getFileNameWithoutExtension(const String& path);
		static String getDirectoryName(const char* path);
		static String getDirectoryName(const String& path);
		static String getExtension(const char* path);
		static String getExtension(const String& path);

	private:
		static bool hasIllegalCharacters(const char* path, bool checkAdditional);
		static String combineNoChecks(const char* path1, const char* path2);
		static void checkInvalidPathChars(const char* path, bool checkAdditional = false);

	public:
#if WIN32
		static const char DirectorySeparatorChar = '\\';
#else
		static const char DirectorySeparatorChar = '/';
#endif

	private:
		static const char AltDirectorySeparatorChar = '/';
		const char* InvalidFileNameChars = new char[41] {
				'"', '<', '>', '|', '\0', '\x0001', '\x0002', '\x0003', '\x0004', '\x0005', '\x0006', '\a', '\b', '\t', '\n', '\v',
				'\f', '\r', '\x000e', '\x000f', '\x0010', '\x0011', '\x0012', '\x0013', '\x0014', '\x0015', '\x0016', '\x0017', '\x0018', '\x0019', '\x001a', '\x001b',
				'\x001c', '\x001d', '\x001e', '\x001f', ':', '*', '?', '\\', '/'
		};
		static const  int MAX_DIRECTORY_PATH = 0xf8;
		//static const  int MAX_PATH = 260;
		static const  int MaxDirectoryLength = 0xff;
		static const  int MaxLongPath = 0x7d00;
		static const  int MaxPath = 260;
		static const  char PathSeparator = ';';
		const char* Prefix = "\\\?\\";
		const char* RealInvalidPathChars = new char[37] {
			'"', '<', '>', '|', '\0', '\x0001', '\x0002', '\x0003', '\x0004', '\x0005', '\x0006', '\a', '\b', '\t', '\n', '\v',
				'\f', '\r', '\x000e', '\x000f', '\x0010', '\x0011', '\x0012', '\x0013', '\x0014', '\x0015', '\x0016', '\x0017', '\x0018', '\x0019', '\x001a', '\x001b',
				'\x001c', '\x001d', '\x001e', '\x001f'
		};
		const char* s_Base32Char = new char[32] {
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
				'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5'
		};
		const char* TrimEndChars = new char[8] { '\t', '\n', '\v', '\f', '\r', ' ', '\x0085', '\x00a0' };
		static const  char VolumeSeparatorChar = ':';

	};
}

#endif // PATH_H
