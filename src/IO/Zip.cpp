#include "IO/Zip.h"
#include "exception/Exception.h"
#include "data/Convert.h"
#include "diag/Trace.h"
#include "IO/Directory.h"
#include "IO/File.h"
#include "IO/Path.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <zip.h>

#if WIN32
#include <Windows.h>
#include <memory.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
#elif __APPLE__

#include <sys/param.h>
#include <sys/mount.h>
#include <libgen.h>
#include <dirent.h>
#include <unistd.h>

#else
#include <sys/vfs.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#endif

using namespace Diag;

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

        ZipFileInner(zip_file *zf) : file(zf) {
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
        _zip->zip = zip_open(fileName.c_str(), 0, &err);
        if (isValid())
            _fileName = fileName;
        else {
            Debug::writeFormatLine("Can not open zip file'%s', error: %d", fileName.c_str(), err);
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
            _zip->zip = zip_open_from_source(zs, 0, &err);
            if (isValid()) {
            } else {
                Debug::writeFormatLine("Can not open zip buffer, zip_err: %d, sys_err: %d, err_str: '%s'",
                                       err.zip_err, err.sys_err, err.str != nullptr ? err.str : "");
            }

            zip_source_free(zs);
        }
    }

    Zip::~Zip() {
        //And close the archive
        if (isValid()) {
            zip_close(_zip->zip);
            _zip->zip = nullptr;
        }

        delete _zip;
    }

    const String &Zip::fileName() const {
        return _fileName;
    }

    bool Zip::isValid() const {
        return _zip->zip != nullptr;
    }

    ZipFile *Zip::open(const String &fileName) const {
        if (isValid()) {
//            zip_file* zf = zip_fopen(_zip->zip, fileName, 0);
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

    void Zip::close(ZipFile *file) {
        if (file != nullptr) {
            zip_fclose(file->_handle->file);
            delete file;
        }
    }

    bool Zip::read(const String &fileName, ByteArray &buffer) {
        ZipFile *zf = open(fileName);
        if (zf) {
            const int count = 1024;
            uint8_t temp[count];
            memset(temp, 0, sizeof(temp));
            zip_int64_t len = 0;
            do {
                len = zip_fread(zf->_handle->file, temp, count);
                if (len > 0) {
                    buffer.addRange(temp, (uint32_t) len);
                }
            } while (len > 0);

            close(zf);
            return true;
        }
        return false;
    }

    bool Zip::read(const String &fileName, String &text) {
        ZipFile *zf = open(fileName);
        if (zf) {
            const int count = 1024;
            char temp[count + 1];
            memset(temp, 0, sizeof(temp));
            zip_int64_t len = 0;
            do {
                len = zip_fread(zf->_handle->file, temp, count);
                if (len > 0) {
                    text.append(temp, (uint32_t) len);
                }
            } while (len > 0);

            close(zf);
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
            pos = zip_name_locate(_zip->zip, temp, 0);
            if (pos < 0) {
                zip_error_t *error = zip_get_error(_zip->zip);
                if (error != nullptr) {
                    if (error->zip_err == ZIP_ER_NOENT) {
                        for (zip_int64_t i = 0; i < zip_get_num_entries(_zip->zip, 0); i++) {
                            struct zip_stat sb;
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

    bool Zip::extract(const String &zipfile, const String &path) {
        if (!Directory::exists(path)) {
            Directory::createDirectory(path);
        }

        //Open the ZIP archive
        int err = 0;
        struct zip *za = zip_open(zipfile, 0, &err);
        if (za == nullptr)
            return false;

        for (zip_int64_t i = 0; i < zip_get_num_entries(za, 0); i++) {
            struct zip_stat sb;
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
                struct zip_file *zf = zip_fopen_index(za, i, 0);
                if (!zf)
                    continue;

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

    bool Zip::compress(const String &path, const String &zipfile) {
        if (!Directory::exists(path))
            return false;

        StringArray files;
        if (!Directory::getFiles(path, "*", SearchOption::AllDirectories, files))
            return false;

        return compressFile(path, files, zipfile);
    }

    bool Zip::compressFile(const String &fileName, const String &zipfile) {
        StringArray files(fileName, nullptr);
        String path = Path::getDirectoryName(fileName);
        return compressFile(path, files, zipfile);
    }

    bool Zip::compressFile(const String &cpath, const StringArray &filenames, const String &zipfile) {
        //Create the ZIP archive
        if (File::exists(zipfile))
            File::deleteFile(zipfile);

        int err = 0;
        struct zip *za = zip_open(zipfile.c_str(), ZIP_CREATE, &err);
        if (za == nullptr)
            return false;

        for (uint32_t i = 0; i < filenames.count(); i++) {
            const String &fileName = filenames[i];
            if (File::exists(fileName)) {
                zip_source *zs = zip_source_file(za, fileName.c_str(), 0, 0);
                if (zs != nullptr) {
                    String cfilename = String::replace(fileName, cpath, String::Empty);
                    if (cfilename.length() > 0 &&
                        cfilename[0] == Path::DirectorySeparatorChar) {
                        cfilename = cfilename.substr(1, cfilename.length());
                    }
#ifdef WIN32
                    cfilename = cfilename.replace("\\", "/");
                    cfilename = String::GBKtoUTF8(cfilename);
#endif
                    if (zip_add(za, cfilename.c_str(), zs) < 0) {
                        zip_close(za);
                        return false;
                    }
                }
            }
        }

        //And close the archive
        zip_close(za);
#ifdef __EMSCRIPTEN__
        int result = ::chmod(zipfile.c_str(), 777);
        if(result != 0)
        {
            Debug::writeFormatLine("Can not chmod the file, file: %s", zipfile.c_str());
        }
#endif
        return true;
    }

    bool Zip::compressFile(const StringArray &filenames, const String &zipfile) {
        //Create the ZIP archive
        if (File::exists(zipfile))
            File::deleteFile(zipfile);

        int err = 0;
        struct zip *za = zip_open(zipfile.c_str(), ZIP_CREATE, &err);
        if (za == nullptr)
            return false;

        for (uint32_t i = 0; i < filenames.count(); i++) {
            const String &fileName = filenames[i];
            if (File::exists(fileName)) {
                zip_source *zs = zip_source_file(za, fileName.c_str(), 0, 0);
                if (zs != nullptr) {
                    String cfilename = Path::getFileName(fileName);
#ifdef WIN32
                    cfilename = cfilename.replace("\\", "/");
                    cfilename = String::GBKtoUTF8(cfilename);
#endif
                    if (zip_add(za, cfilename.c_str(), zs) < 0) {
                        zip_close(za);
                        return false;
                    }
                }
            }
        }

        //And close the archive
        zip_close(za);
#ifdef __EMSCRIPTEN__
        int result = ::chmod(zipfile.c_str(), 777);
        if(result != 0)
        {
            Debug::writeFormatLine("Can not chmod the file, file: %s", zipfile.c_str());
        }
#endif
        return true;
    }

    int Zip::zipRead(void *context, char *buffer, int len) {
        zip_file *zf = static_cast<zip_file *>(context);
        if (zf != nullptr) {
            return (int) zip_fread(zf, buffer, len);
        }
        return 0;
    }

    int Zip::zipClose(void *context) {
        zip_file *zf = static_cast<zip_file *>(context);
        if (zf != nullptr) {
            zip_fclose(zf);
        }
        return 0;
    }
}
