//
//  ConfigFile.cpp
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#include "configuration/ConfigFile.h"

namespace Common
{
    ConfigSource::ConfigSource(FileType type) : ConfigSource(nullptr, String::Empty, type)
    {
    }
    ConfigSource::ConfigSource(Zip* zip, FileType type) : ConfigSource(zip, String::Empty, type)
    {
    }
    ConfigSource::ConfigSource(const String& pathName, FileType type) : ConfigSource(nullptr, pathName, type)
    {
    }
    ConfigSource::ConfigSource(const ConfigSource& cs) : zip(nullptr)
    {
        this->operator=(cs);
    }
    ConfigSource::ConfigSource(Zip* zip, const String& rootPath, FileType type)
    {
        this->zip = zip;
        this->rootPath = rootPath;
        this->fileType = type;
    }
    ConfigSource::~ConfigSource()
    {
        this->zip = nullptr;
    }
    
    bool ConfigSource::isZip() const
    {
        return this->zip != nullptr;
    }

    void ConfigSource::empty()
    {
        this->zip = nullptr;
        this->rootPath.empty();
    }
    bool ConfigSource::isEmpty() const
    {
        return this->zip == nullptr && this->rootPath.isNullOrEmpty();
    }
    
    void ConfigSource::operator=(const ConfigSource& value)
    {
        this->rootPath = value.rootPath;
        this->zip = value.zip;
        this->fileType = value.fileType;
    }
    bool ConfigSource::operator==(const ConfigSource& value) const
    {
        return this->rootPath == value.rootPath &&
                this->zip == value.zip &&
                this->fileType == value.fileType;
    }
    bool ConfigSource::operator!=(const ConfigSource& value) const
    {
        return !operator==(value);
    }
    bool ConfigSource::isXmlFile() const
    {
        return fileType == FileType::XmlFile;
    }
    bool ConfigSource::isJsonFile() const
    {
        return fileType == FileType::JsonFile;
    }
    bool ConfigSource::isAutoFile() const
    {
        return fileType == FileType::AutoFile;
    }
    
    ConfigFile::ConfigFile(FileType type) : ConfigFile(nullptr, String::Empty, String::Empty, type)
    {
    }
    ConfigFile::ConfigFile(Zip* z, FileType t) : ConfigFile(z, String::Empty, String::Empty, t)
    {
    }
    ConfigFile::ConfigFile(Zip* z, const String& fileName, FileType t) : ConfigFile(z, String::Empty, fileName, t)
    {
    }
    ConfigFile::ConfigFile(const String& pathName, const String& fileName, FileType type) : ConfigFile(nullptr, pathName, fileName, type)
    {
    }
    ConfigFile::ConfigFile(const String& fullFileName, FileType type) : ConfigFile(nullptr, Path::getDirectoryName(fullFileName), Path::getFileName(fullFileName), type)
    {
    }
    ConfigFile::ConfigFile(const String& text, uint32_t length, FileType type) : ConfigFile(type)
    {
        this->text = text.substr(0, length);
    }
    ConfigFile::ConfigFile(DbClient* dbClient, const Uuid& id) : ConfigFile()
    {
        this->dbClient = dbClient;
        this->id = id;
    }
    ConfigFile::ConfigFile(Zip* z, const String& rpath, const String& fileName, FileType t) : ConfigSource(z, rpath, t)
    {
        this->fileName = fileName;
        this->dbClient = nullptr;
    }
    ConfigFile::ConfigFile(const ConfigFile& cf)
    {
        this->operator=(cf);
    }
    ConfigFile::~ConfigFile()
    {
    }
    
    void ConfigFile::operator=(const ConfigSource& value)
    {
        this->rootPath = value.rootPath;
        this->zip = value.zip;
    }
    void ConfigFile::operator=(const ConfigFile& value)
    {
        this->rootPath = value.rootPath;
        this->zip = value.zip;
        this->fileType = value.fileType;
        this->fileName = value.fileName;
        this->text = value.text;
        this->dbClient = value.dbClient;
        this->id = value.id;
    }
    bool ConfigFile::operator==(const ConfigFile& value) const
    {
        return this->rootPath == value.rootPath &&
                this->zip == value.zip &&
                this->fileType == value.fileType &&
                this->fileName == value.fileName &&
                this->text == value.text &&
                this->dbClient == value.dbClient &&
                this->id == value.id;
    }
    bool ConfigFile::operator!=(const ConfigFile& value) const
    {
        return !operator==(value);
    }
    
    void ConfigFile::empty()
    {
        this->zip = nullptr;
        this->rootPath.empty();
        this->fileName.empty();
        this->text.empty();
        this->dbClient = nullptr;
        this->id.empty();
    }
    bool ConfigFile::isEmpty() const
    {
        return this->zip == nullptr && this->rootPath.isNullOrEmpty() &&
                this->fileName.isNullOrEmpty() && this->text.isNullOrEmpty() &&
                this->dbClient == nullptr && this->id.isEmpty();
    }
    
    bool ConfigFile::isText() const
    {
        return !this->text.isNullOrEmpty();
    }
    bool ConfigFile::isFile() const
    {
        return !this->rootPath.isNullOrEmpty();
    }
    bool ConfigFile::isDatabase() const
    {
        return dbClient != nullptr;
    }
    
    bool ConfigFile::deleteFile() const
    {
        if(isFile())
        {
            String fullFileName = Path::combine(rootPath, fileName);
            if(File::exists(fullFileName))
            {
                return File::deleteFile(fullFileName);
            }
        }
        return false;
    }
    
    bool ConfigFile::isFileExist() const
    {
        if(isFile())
        {
            String fullFileName = Path::combine(rootPath, fileName);
            if(File::exists(fullFileName))
            {
                return true;
            }
        }
        return false;
    }
}
