//
//  Zip.cpp
//  common
//
//  Created by baowei on 2016/3/28.
//  Copyright © 2016 com. All rights reserved.
//

#include "IO/Zip.h"
#include "exception/Exception.h"
#include "IO/Directory.h"
#include "IO/File.h"
#include "IO/Path.h"
#include <fcntl.h>
#include <zip.h>

#ifdef WIN32

#include <Windows.h>
#include <memory.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>

#elif __APPLE__

#include <unistd.h>

#else

#include <unistd.h>
#include <dirent.h>

#endif

namespace IO {
    class ZipInner {
    public:
        struct zip *zip;

        ZipInner() : zip(nullptr) {
        }
    };

    class ZipFileInner {
    public:
        zip_file *file;

        explicit ZipFileInner(zip_file *zf) : file(zf) {
        }
    };

    ZipFile::ZipFile(ZipFileInner *handle) : _handle(handle) {
    }

    ZipFile::~ZipFile() {
        delete _handle;
        _handle = nullptr;
    }

    void *ZipFile::context() const {
        return _handle->file;
    }

    Zip::Zip(const String &fileName) {
        _zip = new ZipInner();
        _error = 0;

        //Open the ZIP archive
        int err = 0;
        zip_t *zip = zip_open(fileName.c_str(), 0, &err);
        if (zip != nullptr) {
            struct zip_stat sb{};
            if (zip_stat_index(zip, 0, 0, &sb) == 0) {
                _zip->zip = zip;
                _fileName = fileName;
            } else {
                zip_close(zip);
            }
        }
    }

    Zip::Zip(const ByteArray &buffer) {
        _zip = new ZipInner();
        _error = 0;

        //Open the ZIP from buffer
        zip_error_t err;
        zip_error_init(&err);
        zip_source_t *zs = zip_source_buffer_create(buffer.data(), buffer.count(), 0, &err);
        if (zs != nullptr) {
            zip_t *zip = zip_open_from_source(zs, 0, &err);
            if (zip != nullptr) {
                struct zip_stat sb{};
                if (zip_stat_index(zip, 0, 0, &sb) == 0) {
                    _zip->zip = zip;
                } else {
                    zip_close(zip);
                }
            }
        }
    }

    Zip::~Zip() {
        //And close the archive
        close();

        delete _zip;
    }

    const String &Zip::fileName() const {
        return _fileName;
    }

    bool Zip::isValid() const {
        return _zip->zip != nullptr;
    }

    void Zip::close() {
        if (isValid()) {
            zip_close(_zip->zip);
            _zip->zip = nullptr;
        }
    }

    ZipFile *Zip::openFile(const String &fileName) const {
        if (isValid()) {
            int64_t pos = getPosition(fileName);
            if (pos >= 0) {
                zip_file *zf = zip_fopen_index(_zip->zip, pos, 0);
                if (zf != nullptr) {
                    return new ZipFile(new ZipFileInner(zf));
                }
            }
        }
        return nullptr;
    }

    void Zip::closeFile(ZipFile *file) {
        if (isValid()) {
            if (file != nullptr) {
                zip_fclose(file->_handle->file);
                delete file;
            }
        }
    }

    bool Zip::read(const String &fileName, ByteArray &buffer) {
        ZipFile *zf = openFile(fileName);
        if (zf) {
            const int count = 1024;
            uint8_t temp[count];
            memset(temp, 0, sizeof(temp));
            zip_int64_t len;
            do {
                len = zip_fread(zf->_handle->file, temp, count);
                if (len > 0) {
                    buffer.addRange(temp, (uint32_t) len);
                }
            } while (len > 0);

            closeFile(zf);
            return true;
        }
        return false;
    }

    bool Zip::read(const String &fileName, String &text) {
        ZipFile *zf = openFile(fileName);
        if (zf) {
            const int count = 1024;
            char temp[count + 1];
            memset(temp, 0, sizeof(temp));
            zip_int64_t len;
            do {
                len = zip_fread(zf->_handle->file, temp, count);
                if (len > 0) {
                    text.append(temp, (uint32_t) len);
                }
            } while (len > 0);

            closeFile(zf);
            return true;
        }
        return false;
    }

    bool Zip::exists(const String &fileName) const {
        return getPosition(fileName) >= 0;
    }

    int64_t Zip::getPosition(const String &fileName) const {
        zip_int64_t pos = -1;
        if (isValid()) {
#ifdef WIN32
            // fixbug: '/' must be used instead of '\' in windows.
            const String temp = String::replace(fileName, "\\", "/");
#else
            const String temp = fileName;
#endif
//            const char *tempName = zip_get_name(_zip->zip, 0, ZIP_FL_ENC_RAW);
            pos = zip_name_locate(_zip->zip, temp, ZIP_FL_ENC_UTF_8);
            if (pos < 0) {
                zip_error_t *error = zip_get_error(_zip->zip);
                if (error != nullptr) {
                    if (error->zip_err == ZIP_ER_NOENT) {
                        for (zip_int64_t i = 0; i < zip_get_num_entries(_zip->zip, 0); i++) {
                            struct zip_stat sb{};
                            if (zip_stat_index(_zip->zip, i, 0, &sb) == 0) {
                                String name;
                                const char *tempName = zip_get_name(_zip->zip, i, ZIP_FL_ENC_RAW);
                                if (!String::isUTF8(tempName)) {
                                    name = String::encoding("gbk", "UTF-8", tempName);
                                } else {
                                    name = sb.name;
                                }

                                if (String::equals(name, temp)) {
                                    pos = i;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        return pos;
    }

    bool Zip::extract(const String &zipfile, const String &path, const String &password) {
        if (!Directory::exists(path)) {
            Directory::createDirectory(path);
        }

        //Open the ZIP archive
        int err = 0;
        struct zip *za = zip_open(zipfile, 0, &err);
        if (za == nullptr)
            return false;

        for (zip_int64_t i = 0; i < zip_get_num_entries(za, 0); i++) {
            struct zip_stat sb{};
            if (zip_stat_index(za, i, 0, &sb) == 0) {
                String name;
                const char *tempName = zip_get_name(za, i, ZIP_FL_ENC_RAW);
                if (!String::isUTF8(tempName)) {
                    name = String::encoding("gbk", "UTF-8", tempName);
                } else {
                    name = sb.name;
                }

                String fullPathName;
                fullPathName = Path::getDirectoryName(Path::combine(path, name));
                if (!Directory::exists(fullPathName)) {
                    Directory::createDirectory(fullPathName);
                }

                zip_int64_t len;
                String fullFileName = Path::combine(path, name);
                struct zip_file *zf;
                if (password.isNullOrEmpty()) {
                    zf = zip_fopen_index(za, i, 0);
                    if (!zf)
                        continue;
                } else {
                    zf = zip_fopen_index_encrypted(za, i, 0, password);
                    if (!zf)
                        continue;
                }

                int fd = ::open(fullFileName.c_str(), O_RDWR | O_TRUNC | O_CREAT, 0644);
                if (fd < 0)
                    continue;

                char buf[65535];
                zip_uint64_t sum = 0;
                while (sum != sb.size) {
                    len = zip_fread(zf, buf, sizeof(buf));
                    if (len < 0)
                        continue;

                    write(fd, buf, len);
                    sum += len;
                }
                ::close(fd);
                zip_fclose(zf);
            } else {
                return false;
            }
        }

        //And close the archive
        zip_close(za);

#ifdef __arm_linux__
        sync();    // save all of files.
#endif
        return true;
    }

    bool Zip::compress(const String &path, const String &zipfile, const String &password) {
        if (!Directory::exists(path))
            return false;

        StringArray files;
        if (!Directory::getFiles(path, "*", SearchOption::AllDirectories, files))
            return false;

        return compressFile(path, files, zipfile, password);
    }

    bool Zip::compressFile(const String &fileName, const String &zipfile, const String &password) {
        StringArray files(fileName, nullptr);
        String path = Path::getDirectoryName(fileName);
        return compressFile(path, files, zipfile, password);
    }

    bool Zip::compressFile(const String &path, const StringArray &fileNames, const String &zipfile,
                           const String &password) {
        //Create the ZIP archive
        if (File::exists(zipfile))
            File::deleteFile(zipfile);

        int err = 0;
        struct zip *za = zip_open(zipfile.c_str(), ZIP_CREATE, &err);
        if (za == nullptr)
            return false;

        for (size_t i = 0; i < fileNames.count(); i++) {
            const String &fileName = fileNames[i];
            if (File::exists(fileName)) {
                zip_source *zs = zip_source_file(za, fileName.c_str(), 0, 0);
                if (zs != nullptr) {
                    String cFileName = path.isNullOrEmpty() ?
                                       Path::getFileName(fileName) :
                                       String::replace(fileName, path, String::Empty);
                    if (cFileName.length() > 0 &&
                        cFileName[0] == Path::DirectorySeparatorChar) {
                        cFileName = cFileName.substr(1, cFileName.length());
                    }
#ifdef WIN32
                    cFileName = cFileName.replace("\\", "/");
                    cFileName = String::GBKtoUTF8(cFileName);
#endif
                    zip_int64_t index = zip_file_add(za, cFileName.c_str(), zs, 0);
                    if (index < 0) {
                        zip_close(za);
                        return false;
                    }

                    if (!password.isNullOrEmpty()) {
                        zip_file_set_encryption(za, index, ZIP_EM_AES_256, password);
                    }
                }
            }
        }

        //And close the archive
        zip_close(za);
#ifdef __EMSCRIPTEN__
        ::chmod(zipfile.c_str(), 777);
#endif
        return true;
    }

    bool Zip::compressFile(const StringArray &fileNames, const String &zipfile, const String &password) {
        return compressFile(String::Empty, fileNames, zipfile, password);
    }

    int Zip::zipRead(void *context, char *buffer, int len) {
        auto *zf = static_cast<zip_file *>(context);
        if (zf != nullptr) {
            return (int) zip_fread(zf, buffer, len);
        }
        return 0;
    }
}
