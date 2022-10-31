#ifndef FILEINFO_H
#define FILEINFO_H

#include <stdint.h>
#include "data/ValueType.h"
#include "IO/File.h"
#include "data/DateTime.h"

namespace Common
{
	class FileInfo
	{
	public:
		enum FileAttributes
		{
			Unkown = 0x0,
			Read = 0x0100,
			Write = 0x0080,
			Execute = 0x0040,
			Regular = 0x8000,
		};

        FileInfo();
        FileInfo(const String& name);
        FileInfo(const char* name);

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

#endif // FILEINFO_H
