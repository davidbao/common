//
//  Zip.h
//  common
//
//  Created by baowei on 2016/3/28.
//  Copyright © 2016 com. All rights reserved.
//

#ifndef Zip_h
#define Zip_h

#include "data/ByteArray.h"
#include "data/String.h"
#include "data/StringArray.h"

namespace Xml {
    class XmlTextReader;
}

namespace Json {
    class JsonTextReader;
}

namespace IO {
    class ZipInner;

    class ZipFileInner;

    class Zip;

    class ZipFile {
    public:
        ~ZipFile();

    private:
        explicit ZipFile(ZipFileInner *handle);

        void *context() const;

    private:
        friend Zip;
        friend Xml::XmlTextReader;

        ZipFileInner *_handle;
    };

    class Zip {
    public:
        explicit Zip(const String &fileName);

        explicit Zip(const ByteArray &buffer);

        ~Zip();

        bool isValid() const;

        void close();

        ZipFile *openFile(const String &fileName) const;

        void closeFile(ZipFile *file);

        bool read(const String &fileName, ByteArray &buffer);

        bool read(const String &fileName, String &text);

        const String &fileName() const;

        bool exists(const String &fileName) const;

    private:
        int64_t getPosition(const String &fileName) const;

    public:
        static bool extract(const String &zipfile, const String &path, const String &password = String::Empty);

        static bool compress(const String &path, const String &zipfile, const String &password = String::Empty);

        static bool compressFile(const String &fileName, const String &zipfile, const String &password = String::Empty);

        static bool compressFile(const String &path, const StringArray &fileNames, const String &zipfile, const String &password = String::Empty);

        static bool compressFile(const StringArray &fileNames, const String &zipfile, const String &password = String::Empty);

    private:
        static int zipRead(void *context, char *buffer, int len);

    private:
        friend Xml::XmlTextReader;
        friend Json::JsonTextReader;

        ZipInner *_zip;
        int _error;
        String _fileName;
    };
}

#endif  // Zip_h
