#include "data/Version.h"
#include "data/Convert.h"
#include "data/StringArray.h"
#include "IO/Stream.h"

namespace Common
{
    const Version Version::Empty = Version();
    const Version Version::Verson1_0 = Version(1, 0);
    Version::Version(const char* version) : Version()
	{
		if (version != nullptr)
		{
			parse(version, *this);
		}
	}
	Version::Version(const String& version) : Version()
	{
		if (!version.isNullOrEmpty())
		{
			parse(version, *this);
		}
	}
	Version::Version(int major, int minor, int build, int revision)
	{
		_major = major;
		_minor = minor;
		_build = build;
		_revision = revision;
	}
    Version::Version(const Version& version) : Version()
    {
        this->operator=(version);
    }

	Version::~Version()
	{
	}

	bool Version::parse(const String& str, Version& version)
	{
        return parseInner<Int32>(str, version);
	}
    
    bool Version::parseByte(const String& str, Version& version)
    {
        return parseInner<Byte>(str, version);
    }
    
    template <class T>
    bool Version::parseInner(const String& str, Version& version)
    {
        StringArray components;
        Convert::splitStr(str, '.', components);
        size_t length = components.count();
        if ((length < 2) || (length > 4))
        {
            return false;
        }
        
        String str1 = components[0];
        String str2 = components[1];
        T v1, v2;
        if (!T::parse(str1, v1))
        {
            return false;
        }
        if (!T::parse(str2, v2))
        {
            return false;
        }
        
        length -= 2;
        if (length > 0)
        {
            T v3, v4;
            String str3 = components[2];
            if (!T::parse(str3, v3))
            {
                return false;
            }
            length--;
            if (length > 0)
            {
                String str4 = components[3];
                if (!T::parse(str4, v4))
                {
                    return false;
                }
                version._major = v1;
                version._minor = v2;
                version._build = v3;
                version._revision = v4;
            }
            else
            {
                version._major = v1;
                version._minor = v2;
                version._build = v3;
            }
        }
        else
        {
            version._major = v1;
            version._minor = v2;
        }
        return true;
    }

	String Version::toString() const
	{
		if (_build == -1)
		{
			return toString(2);
		}
		if (_revision == -1)
		{
			return toString(3);
		}
		return toString(4);

	}
    bool Version::isEmpty() const
    {
        return this->operator==(Empty);
    }

	String Version::toString(int fieldCount) const
	{
		switch (fieldCount)
		{
		case 0:
            return String::Empty;

		case 1:
			return Convert::convertStr(_major).c_str();

		case 2:
			return String::convert("%d.%d", _major, _minor);

		case 3:
			return String::convert("%d.%d.%d", _major, _minor, _build);

		case 4:
			return String::convert("%d.%d.%d.%d", _major, _minor, _build, _revision);

		default:
			return String::Empty;
		}
	}

    void Version::operator=(const Version& version)
    {
        this->_major = version._major;
        this->_minor = version._minor;
        this->_build = version._build;
        this->_revision = version._revision;
    }
	bool Version::operator==(const Version& version) const
	{
		return (_major == version._major &&
			_minor == version._minor &&
			_build == version._build &&
			_revision == version._revision);
	}
	bool Version::operator!=(const Version& version) const
	{
		return !operator==(version);
	}
	bool Version::operator>=(const Version& version) const
	{
		return version.compareTo(*this) <= 0;
	}
	bool Version::operator>(const Version& version) const
	{
		return version.compareTo(*this) < 0;
	}
	bool Version::operator<=(const Version& version) const
	{
		return compareTo(version) <= 0;
	}
	bool Version::operator<(const Version& version) const
	{
		return compareTo(version) < 0;
	}

	int Version::compareTo(const Version& version) const
	{
		if (_major != version._major)
		{
			if (_major > version._major)
			{
				return 1;
			}
			return -1;
		}
		if (_minor != version._minor)
		{
			if (_minor > version._minor)
			{
				return 1;
			}
			return -1;
		}
		if (_build != version._build)
		{
			if (_build > version._build)
			{
				return 1;
			}
			return -1;
		}
        if (_revision != version._revision)
        {
            if (_revision > version._revision)
            {
                return 1;
            }
            return -1;
        }
        return 0;
//		if (_revision == version._revision)
//		{
//			return 0;
//		}
//		if (_revision > version._revision)
//		{
//			return 1;
//		}
//		return -1;
	}

	void Version::writeInt32(Stream* stream, bool bigEndian) const
	{
		stream->writeInt32(_major, bigEndian);
		stream->writeInt32(_minor, bigEndian);
		stream->writeInt32(_build, bigEndian);
		stream->writeInt32(_revision, bigEndian);
	}
	void Version::readInt32(Stream* stream, bool bigEndian)
	{
		_major = stream->readInt32(bigEndian);
		_minor = stream->readInt32(bigEndian);
		_build = stream->readInt32(bigEndian);
		_revision = stream->readInt32(bigEndian);
	}
	void Version::writeInt16(Stream* stream, bool bigEndian) const
	{
		stream->writeInt16(_major, bigEndian);
		stream->writeInt16(_minor, bigEndian);
		stream->writeInt16(_build, bigEndian);
		stream->writeInt16(_revision, bigEndian);
	}
	void Version::readInt16(Stream* stream, bool bigEndian)
	{
		_major = stream->readInt16(bigEndian);
		_minor = stream->readInt16(bigEndian);
		_build = stream->readInt16(bigEndian);
		_revision = stream->readInt16(bigEndian);
        
        if(_minor == Int16::MaxValue)
            _minor = -1;
        if(_build == Int16::MaxValue)
            _build = -1;
        if(_revision == Int16::MaxValue)
            _revision = -1;
	}
	void Version::writeBCDInt16(Stream* stream) const
	{
		stream->writeBCDInt16(_major);
		stream->writeBCDInt16(_minor);
		stream->writeBCDInt16(_build);
		stream->writeBCDInt16(_revision);
	}
	void Version::readBCDInt16(Stream* stream)
	{
		_major = stream->readBCDInt16();
		_minor = stream->readBCDInt16();
		_build = stream->readBCDInt16();
		_revision = stream->readBCDInt16();
        
        if(_minor == Int16::MaxValue)
            _minor = -1;
        if(_build == Int16::MaxValue)
            _build = -1;
        if(_revision == Int16::MaxValue)
            _revision = -1;
	}
	void Version::writeBCDByte(Stream* stream) const
	{
		stream->writeBCDByte(_major);
		stream->writeBCDByte(_minor);
		stream->writeBCDByte(_build);
		stream->writeBCDByte(_revision);
	}
	void Version::readBCDByte(Stream* stream)
	{
		_major = stream->readBCDByte();
		_minor = stream->readBCDByte();
		_build = stream->readBCDByte();
		_revision = stream->readBCDByte();
        
        if(_minor == Byte::MaxValue)
            _minor = -1;
        if(_build == Byte::MaxValue)
            _build = -1;
        if(_revision == Byte::MaxValue)
            _revision = -1;
	}
    void Version::writeByte(Stream* stream) const
    {
        stream->writeByte(_major);
        stream->writeByte(_minor);
        stream->writeByte(_build);
        stream->writeByte(_revision);
    }
    void Version::readByte(Stream* stream)
    {
        _major = stream->readByte();
        _minor = stream->readByte();
        _build = stream->readByte();
        _revision = stream->readByte();
        
        if(_minor == Byte::MaxValue)
            _minor = -1;
        if(_build == Byte::MaxValue)
            _build = -1;
        if(_revision == Byte::MaxValue)
            _revision = -1;
    }
    void Version::write(Stream* stream) const
    {
        writeInt32(stream);
    }
    void Version::read(Stream* stream)
    {
        readInt32(stream);
    }
    
    const Version Version::increase(const Version& version)
    {
        static const int MaxVersion = 255;
        if(version._minor < 0)
            return Version(version._major + 1);
        else if(version._build < 0)
        {
            if(version._minor < MaxVersion)
                return Version(version._major, version._minor + 1, version._build, version._revision);
            else if(version._major < MaxVersion)
                return Version(version._major + 1, version._minor, version._build, version._revision);
            else
                return Version(version._major, version._minor + 1, version._build, version._revision);
        }
        else if(version._revision < 0)
        {
            if(version._build < MaxVersion)
                return Version(version._major, version._minor, version._build + 1, version._revision);
            else if(version._minor < MaxVersion)
                return Version(version._major, version._minor + 1, version._build, version._revision);
            else if(version._major < MaxVersion)
                return Version(version._major + 1, version._minor, version._build, version._revision);
            else
                return Version(version._major, version._minor, version._build + 1, version._revision);
        }
        else
        {
            if(version._revision < MaxVersion)
                return Version(version._major, version._minor, version._build, version._revision + 1);
            else if(version._build < MaxVersion)
                return Version(version._major, version._minor, version._build + 1, version._revision);
            else if(version._minor < MaxVersion)
                return Version(version._major, version._minor + 1, version._build, version._revision);
            else if(version._major < MaxVersion)
                return Version(version._major + 1, version._minor, version._build, version._revision);
            else
                return Version(version._major, version._minor, version._build, version._revision + 1);
        }
    }
}
