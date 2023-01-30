//
//  ConfigFile.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright © 2015 com. All rights reserved.
//

#ifndef ConfigFile_h
#define ConfigFile_h

#include "data/String.h"
#include "IO/Path.h"
#include "IO/File.h"
#include "IO/Zip.h"
#include "database/DbClient.h"

using namespace Database;

namespace Config {
    class ConfigSource {
    public:
        enum FileType {
            XmlFile = 0,
            JsonFile = 1,
            AutoFile = 2
        };

        String rootPath;
        Zip *zip;
        FileType fileType;

        ConfigSource(FileType type = FileType::XmlFile);

        ConfigSource(Zip *zip, FileType type = FileType::XmlFile);

        ConfigSource(const String &pathName, FileType type = FileType::XmlFile);

        ConfigSource(const ConfigSource &cs);

        virtual ~ConfigSource();

        bool isZip() const;

        virtual void empty();

        virtual bool isEmpty() const;

        void operator=(const ConfigSource &value);

        bool operator==(const ConfigSource &value) const;

        bool operator!=(const ConfigSource &value) const;

        bool isXmlFile() const;

        bool isJsonFile() const;

        bool isAutoFile() const;

    protected:
        ConfigSource(Zip *zip, const String &rootPath, FileType type = FileType::XmlFile);
    };

    class ConfigFile : public ConfigSource {
    public:
        String fileName;
        String text;
        DbClient *dbClient;
        Uuid id;

        ConfigFile(FileType type = FileType::XmlFile);

        ConfigFile(Zip *zip, FileType type = FileType::XmlFile);

        ConfigFile(Zip *zip, const String &fileName, FileType type = FileType::XmlFile);

        ConfigFile(const String &pathName, const String &fileName, FileType type = FileType::XmlFile);

        ConfigFile(const String &fullFileName, FileType type = FileType::XmlFile);

        ConfigFile(const String &text, uint32_t length, FileType type = FileType::XmlFile);

        ConfigFile(DbClient *dbClient, const Uuid &id);

        ConfigFile(const ConfigFile &cf);

        ~ConfigFile() override;

        void operator=(const ConfigSource &value);

        void operator=(const ConfigFile &value);

        bool operator==(const ConfigFile &value) const;

        bool operator!=(const ConfigFile &value) const;

        void empty() override;

        bool isEmpty() const override;

        bool isText() const;

        bool isFile() const;

        bool isDatabase() const;

        bool deleteFile() const;

        bool isFileExist() const;

    private:
        ConfigFile(Zip *zip, const String &rootPath, const String &fileName, FileType type = FileType::XmlFile);
    };
}

#endif    // ConfigFile_h
