//
//  Configuration.cpp
//  common
//
//  Created by baowei on 15/3/31.
//  Copyright (c) 2015 com. All rights reserved.
//

#include "configuration/Configuration.h"
#include "exception/Exception.h"
#include "IO/File.h"
#include "diag/Trace.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "IO/Directory.h"
#include "crypto/Md5Provider.h"

#ifdef DEBUG

#include "system/Application.h"

#endif

using namespace Diag;
using namespace Crypto;
using namespace System;

namespace Config {
    Configuration::Configuration(const ConfigFile &file) {
        if (file.isEmpty()) {
            throw new ArgumentNullException("file");
        }
        _file = file;
    }

    Configuration::~Configuration() {
    }

    void Configuration::detectFileType(const String &fullFileName) {
        if (_file.isAutoFile()) {
            FileStream fs(fullFileName, FileMode::FileOpen, FileAccess::FileRead);
            if (fs.isOpen()) {
                String text;
                if (fs.readText(text, 32)) {
                    if (text.find("<?xml") >= 0)
                        _file.fileType = ConfigFile::XmlFile;
                    else if (text[0] == '{')
                        _file.fileType = ConfigFile::JsonFile;
                }
                fs.close();
            }
        }
    }

    bool Configuration::load(void *sender) {
        try {
            if (_file.isEmpty()) {
                Debug::writeLine("The configuration file is empty!");
                return false;
            }

            if (_file.isZip()) {
                if (_file.isXmlFile()) {
                    XmlTextReader reader(_file.zip, _file.fileName);
                    return loadInner(reader, sender);
                } else if (_file.isJsonFile()) {
                    JsonTextReader reader(_file.zip, _file.fileName);
                    return loadInner(reader, sender);
                } else {
                    return false;
                }
            } else if (_file.isFile()) {
                const String fullFileName = this->fullFileName();
                if (File::exists(fullFileName)) {
                    detectFileType(fullFileName);

                    if (_file.isXmlFile()) {
                        XmlTextReader reader(fullFileName);
                        return loadInner(reader, sender);
                    } else if (_file.isJsonFile()) {
                        JsonTextReader reader(fullFileName);
                        return loadInner(reader, sender);
                    } else {
                        return false;
                    }
                } else {
                    Debug::writeFormatLine("The configuration file is not exist! file name: %s", fullFileName.c_str());
                }
            } else if (_file.isText()) {
                if (_file.isXmlFile()) {
                    XmlTextReader reader(_file.text, _file.text.length());
                    return loadInner(reader, sender);
                } else if (_file.isJsonFile()) {

                } else {
                    return false;
                }
            } else if (_file.isDatabase()) {
                return loadInner(_file.dbClient, sender);
            }
            Debug::writeLine("The configuration file is unknown!");
        } catch (...) {
        }
        return false;
    }

    bool Configuration::loadInner(XmlTextReader &reader, void *sender) {
        if (reader.isValid()) {
            reader.moveToContent();
            if (sender != nullptr) {
                return load(reader, sender);
            } else {
                return load(reader);
            }
        }
        Debug::writeLine("The configuration file is not valid!");
        return false;
    }

    bool Configuration::loadInner(JsonTextReader &reader, void *sender) {
        if (reader.isValid()) {
            if (sender != nullptr) {
                return load(reader, sender);
            } else {
                return load(reader);
            }
        }
        Debug::writeLine("The configuration file is not valid!");
        return false;
    }

    bool Configuration::loadInner(DbClient *dbClient, void *sender) {
        if (dbClient != nullptr) {
            if (sender != nullptr) {
                return load(dbClient, sender);
            } else {
                return load(dbClient);
            }
        }
        Debug::writeLine("The configuration database is not valid!");
        return false;
    }

    bool Configuration::save(void *sender) {
        if (_file.isEmpty())
            return false;

        if (_file.isZip()) {
            // todo: save file in zip.
        } else if (_file.isDatabase()) {
            if (_file.dbClient != nullptr) {
                if (sender != nullptr) {
                    return save(_file.dbClient, sender);
                } else {
                    return save(_file.dbClient);
                }
            }
        } else {
            // save a temp file at first.
            if (!Directory::exists(_file.rootPath)) {
                Directory::createDirectory(_file.rootPath);
            }
            String tempFileName = this->fullFileName(_file.rootPath, (String) "~" + _file.fileName);

            if (_file.isXmlFile()) {
                XmlTextWriter writer(tempFileName);
                if (writer.isValid()) {
                    writer.writeStartDocument();

                    if (sender != nullptr) {
                        if (!save(writer, sender))
                            return false;
                    } else {
                        if (!save(writer))
                            return false;
                    }

                    writer.writeEndDocument();
                }
                writer.close();
            } else if (_file.isJsonFile()) {
                JsonTextWriter writer(tempFileName);
                if (writer.isValid()) {
                    writer.enableIndent(false);
                    writer.writeStartDocument();

                    if (sender != nullptr) {
                        if (!save(writer, sender))
                            return false;
                    } else {
                        if (!save(writer))
                            return false;
                    }

                    writer.writeEndDocument();
                }
                writer.close();
            } else {
                return false;
            }

            // copy temp file to real file
            if (File::exists(tempFileName)) {
                String fullFileName = this->fullFileName(_file.rootPath, _file.fileName);
                return File::move(tempFileName, fullFileName);
            }
        }
        return false;
    }

    bool Configuration::savePart(void *sender) {
        if (_file.isEmpty())
            return false;

        if (_file.isZip()) {
            // todo: save file in zip.
        } else if (_file.isDatabase()) {
            if (_file.dbClient != nullptr) {
                if (sender != nullptr) {
                    return savePart(_file.dbClient, sender);
                } else {
                    return savePart(_file.dbClient);
                }
            }
        } else {
            // save a temp file at first.
            if (!Directory::exists(_file.rootPath)) {
                Directory::createDirectory(_file.rootPath);
            }

            XmlDocument doc;
            String fullFileName = this->fullFileName(_file.rootPath, _file.fileName);
            String tempFileName = this->fullFileName(_file.rootPath, (String) "~" + _file.fileName);
            if (File::exists(fullFileName)) {
                File::copy(fullFileName, tempFileName);
                doc.load(tempFileName);
            }
            if (!doc.isValid())
                doc.newDocument();

            if (doc.isValid()) {
                if (sender != nullptr) {
                    if (!savePart(doc, sender))
                        return false;
                } else {
                    if (!savePart(doc))
                        return false;
                }
                doc.save(tempFileName);
            }

            // copy temp file to real file
            if (File::exists(tempFileName)) {
                return File::move(tempFileName, fullFileName);
            }
        }
        return false;
    }

    bool Configuration::savePart(action_savePart action, void *sender) {
        if (_file.isEmpty())
            return false;

        if (_file.isZip()) {
            // todo: save file in zip.
        } else if (_file.isDatabase()) {
        } else {
            // save a temp file at first.
            if (!Directory::exists(_file.rootPath)) {
                Directory::createDirectory(_file.rootPath);
            }

            XmlDocument doc;
            String fullFileName = this->fullFileName(_file.rootPath, _file.fileName);
            String tempFileName = this->fullFileName(_file.rootPath, (String) "~" + _file.fileName);
            if (File::exists(fullFileName)) {
                File::copy(fullFileName, tempFileName);
                doc.load(tempFileName);
            }
            if (!doc.isValid())
                doc.newDocument();

            if (doc.isValid()) {
                if (action != nullptr) {
                    if (!action(doc, sender))
                        return false;
                }
                doc.save(tempFileName);
            }

            // copy temp file to real file
            if (File::exists(tempFileName)) {
                return File::move(tempFileName, fullFileName);
            }
        }
        return false;
    }

    bool Configuration::getMD5Str(String &str) const {
        if (_file.isEmpty()) {
            Debug::writeLine("The configuration file is empty!");
            return false;
        }

        if (_file.isZip()) {
            ByteArray output;
            Md5Provider::computeFileHash(_file.zip->fileName(), output);
            str = output.toString("%02X", String::Empty);
            return true;
        } else if (_file.isFile()) {
            const String fullFileName = this->fullFileName();
            if (File::exists(fullFileName)) {
                ByteArray output;
                Md5Provider::computeFileHash(fullFileName, output);
                str = output.toString("%02X", String::Empty);
                return true;
            } else {
                Debug::writeFormatLine("The configuration file is not exist! file name: %s", fullFileName.c_str());
            }
        } else if (_file.isText()) {
            ByteArray output;
            Md5Provider::computeHash(_file.text, output);
            str = output.toString("%02X", String::Empty);
            return true;
        }

        Debug::writeLine("The configuration file is unknown!");
        return false;
    }

    bool Configuration::load(XmlTextReader &reader) {
        return false;
    }

    bool Configuration::load(XmlTextReader &reader, void *sender) {
        return false;
    }

    bool Configuration::load(DbClient *dbClient) {
        return false;
    }

    bool Configuration::load(DbClient *dbClient, void *sender) {
        return false;
    }

    bool Configuration::save(XmlTextWriter &writer) {
        return false;
    }

    bool Configuration::save(XmlTextWriter &writer, void *sender) {
        return false;
    }

    bool Configuration::savePart(XmlDocument &doc, void *sender) {
        return false;
    }

    bool Configuration::savePart(XmlDocument &doc) {
        return false;
    }

    bool Configuration::save(DbClient *dbClient) {
        return false;
    }

    bool Configuration::save(DbClient *dbClient, void *sender) {
        return false;
    }

    bool Configuration::savePart(DbClient *dbClient, void *sender) {
        return false;
    }

    bool Configuration::savePart(DbClient *dbClient) {
        return false;
    }

    bool Configuration::load(JsonTextReader &reader) {
        return false;
    }

    bool Configuration::load(JsonTextReader &reader, void *sender) {
        return false;
    }

    bool Configuration::save(JsonTextWriter &writer) {
        return false;
    }

    bool Configuration::save(JsonTextWriter &writer, void *sender) {
        return false;
    }

    bool Configuration::exists() const {
        if (_file.isEmpty())
            return false;

        if (_file.isZip()) {
            return _file.zip->exists(_file.fileName);
        } else if (_file.isFile()) {
            const String fullFileName = this->fullFileName();
            if (File::exists(fullFileName)) {
                return true;
            }
        } else if (_file.isText()) {
            return true;
        } else if (_file.isDatabase()) {
            return _file.dbClient != nullptr;
        }
        return false;
    }

    bool Configuration::remove() {
        if (_file.isEmpty())
            return false;

        if (_file.isFile()) {
            const String fullFileName = this->fullFileName();
            if (File::exists(fullFileName)) {
                return File::deleteFile(fullFileName);
            }
        }
        return false;
    }

    const String Configuration::fullFileName() const {
        if (_file.isFile()) {
            return fullFileName(_file.rootPath, _file.fileName);
        }
        return String::Empty;
    }

    const String Configuration::fullFileName(const String &path, const String &fileName) {
#ifdef WIN32
        String fullFileName = Path::combine(path, String::UTF8toGBK(fileName));
#else
        String fullFileName = Path::combine(path, fileName);
#endif
        return fullFileName;
    }

#ifdef DEBUG

    void Configuration::createSqlFile(const String &fileName, const String &sql) const {
        String sqlPath = Path::combine(Application::instance()->rootPath(), "sql");
        if (!Directory::exists(sqlPath))
            Directory::createDirectory(sqlPath);
        String fullFileName = Path::combine(sqlPath, fileName);
        FileStream fs(fullFileName, FileMode::FileCreate, FileAccess::FileReadWrite);
        fs.writeText(sql);
        fs.close();
    }

#endif
}
