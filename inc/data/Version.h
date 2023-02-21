//
//  Version.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Version_h
#define Version_h

#include "data/String.h"
#include "data/Vector.h"

#undef major
#undef minor

namespace IO {
    class Stream;
}
using namespace IO;

namespace Data {
    struct Version : public IEquatable<Version>, public IEvaluation<Version>, public IComparable<Version> {
    public:
        Version(int major = 0, int minor = 0, int build = -1, int revision = -1);

        Version(const char *version);

        Version(const String &version);

        Version(const Version &version);

        ~Version() override;

        bool equals(const Version &other) const override;

        void evaluates(const Version &other) override;

        int compareTo(const Version &other) const override;

        bool isEmpty() const;

        String toString() const;

        int major() const;

        int minor() const;

        int build() const;

        int revision() const;

        Version &operator=(const Version &version);

        Version &operator=(const String &version);

        operator String() const;

        void writeInt32(Stream *stream, bool bigEndian = true) const;

        void readInt32(Stream *stream, bool bigEndian = true);

        void writeInt16(Stream *stream, bool bigEndian = true) const;

        void readInt16(Stream *stream, bool bigEndian = true);

        void writeByte(Stream *stream) const;

        void readByte(Stream *stream);

        void writeBCDInt32(Stream *stream) const;

        void readBCDInt32(Stream *stream);

        void writeBCDInt16(Stream *stream) const;

        void readBCDInt16(Stream *stream);

        void writeBCDByte(Stream *stream) const;

        void readBCDByte(Stream *stream);

        void write(Stream *stream, bool bigEndian = true) const;

        void read(Stream *stream, bool bigEndian = true);

    public:
        static bool parse(const String &str, Version &version);

        static bool parseByte(const String &str, Version &version);

        static Version increase(const Version &version);

    public:
        static const Version Empty;
        static const Version Version1_0;

    private:
        String toString(int fieldCount) const;

    private:
        template<class T>
        static bool parseInner(const String &str, Version &version);

    private:
        int _major;
        int _minor;
        int _build;
        int _revision;
    };

    typedef Vector<Version> Versions;
}

#endif // Version_h

