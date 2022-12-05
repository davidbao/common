#ifndef ZIP_H
#define ZIP_H

#include "data/ByteArray.h"
#include "data/ValueType.h"

namespace Xml {
    class XmlTextReader;
}

namespace Common
{
    class StringArray;
    class JsonTextReader;
    class ZipInner;
    class ZipFileInner;
    
    class Zip;
    class ZipFile
    {
    public:
        ~ZipFile();
        
    private:
        ZipFile(ZipFileInner* handle);
        
        void* context() const;
        
    private:
        friend Zip;
        friend Xml::XmlTextReader;
        
        ZipFileInner* _handle;
    };
    
	class Zip
	{
	public:
		Zip(const String& fileName);
        Zip(const ByteArray& buffer);
		~Zip();
        
        bool isValid() const;
        
        ZipFile* open(const String& fileName) const;
        void close(ZipFile* file);
        
        bool read(const String& fileName, ByteArray& buffer);
        bool read(const String& fileName, String& text);
        
        const String& fileName() const;
        
        bool exists(const String& fileName) const;
        
    private:
        int64_t getPosition(const String& fileName) const;
        
    public:
		static bool extract(const String& zipfile, const String& path);
        
        static bool compress(const String& path, const String& zipfile);
        static bool compressFile(const String& fileName, const String& zipfile);
        static bool compressFile(const String& cpath, const StringArray& filenames, const String& zipfile);
        static bool compressFile(const StringArray& filenames, const String& zipfile);
        
    private:
        static int zipRead(void * context, char * buffer, int len);
        static int zipClose(void * context);

    private:
        friend Xml::XmlTextReader;
        friend JsonTextReader;
        
        ZipInner* _zip;
        int _error;
        String _fileName;
	};
}

#endif	// ZIP_H
