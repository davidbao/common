#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
#include "data/ValueType.h"

namespace Common
{
    class Stream;
	struct Version
	{
	public:
		Version(const char* version);
		Version(const String& version);
        Version(const Version& version);
		Version(int major = 0, int minor = 0, int build = -1, int revision = -1);
		~Version();

		String toString() const;
        bool isEmpty() const;

        void operator=(const Version& version);
		bool operator==(const Version& version) const;
		bool operator!=(const Version& version) const;
		bool operator>=(const Version& version) const;
		bool operator>(const Version& version) const;
		bool operator<=(const Version& version) const;
		bool operator<(const Version& version) const;

		void writeInt32(Stream* stream, bool bigEndian = true) const;
		void readInt32(Stream* stream, bool bigEndian = true);
		void writeInt16(Stream* stream, bool bigEndian = true) const;
		void readInt16(Stream* stream, bool bigEndian = true);
		void writeBCDInt16(Stream* stream) const;
		void readBCDInt16(Stream* stream);
		void writeBCDByte(Stream* stream) const;
		void readBCDByte(Stream* stream);
        void writeByte(Stream* stream) const;
        void readByte(Stream* stream);
        void write(Stream* stream) const;
        void read(Stream* stream);
        
    public:
        static bool parse(const String& str, Version& version);
        static bool parseByte(const String& str, Version& version);
        static const Version increase(const Version& version);
        
    public:
        static const Version Empty;
        static const Version Verson1_0;

	private:
		String toString(int fieldCount) const;

		int compareTo(const Version& version) const;
        
    private:
        template <class T>
        static bool parseInner(const String& str, Version& version);

	private:
		int _major;
		int _minor;
		int _build;
		int _revision;
	};
    typedef Array<Version> Versions;
}

#endif // VERSION_H

