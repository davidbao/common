//
//  Configuration.h
//  common
//
//  Created by baowei on 2015/7/20.
//  Copyright (c) 2015 com. All rights reserved.
//

#ifndef Configuration_h
#define Configuration_h

#include "xml/XmlTextReader.h"
#include "xml/XmlTextWriter.h"
#include "xml/XmlDocument.h"
#include "json/JsonTextWriter.h"
#include "json/JsonTextReader.h"
#include "configuration/ConfigFile.h"

using namespace Xml;

namespace Common
{
    typedef bool (*action_savePart)(XmlDocument&, void*);
    class Configuration
    {
    public:
        Configuration(const ConfigFile& file);
        virtual ~Configuration();
        
        bool load(void* sender = nullptr);
        bool save(void* sender = nullptr);
        bool savePart(void* sender = nullptr);
        bool savePart(action_savePart action, void* sender);
        
        bool getMD5Str(String& str) const;
        
        bool exists() const;
        bool remove();
        
    protected:
        virtual bool load(XmlTextReader& reader);
        virtual bool load(XmlTextReader& reader, void* sender);
        
        virtual bool load(DbClient* dbClient);
        virtual bool load(DbClient* dbClient, void* sender);
        
        virtual bool save(XmlTextWriter& writer);
        virtual bool save(XmlTextWriter& writer, void* sender);
        
        virtual bool savePart(XmlDocument& doc);
        virtual bool savePart(XmlDocument& doc, void* sender);
        
        virtual bool save(DbClient* dbClient);
        virtual bool save(DbClient* dbClient, void* sender);
        
        virtual bool savePart(DbClient* dbClient);
        virtual bool savePart(DbClient* dbClient, void* sender);
        
        virtual bool load(JsonTextReader& reader);
        virtual bool load(JsonTextReader& reader, void* sender);
        
        virtual bool save(JsonTextWriter& writer);
        virtual bool save(JsonTextWriter& writer, void* sender);
        
#ifdef DEBUG
        void createSqlFile(const String& fileName, const String& sql) const;
#endif
        
    private:
        bool loadInner(XmlTextReader& reader, void* sender);
        bool loadInner(JsonTextReader& reader, void* sender);
        bool loadInner(DbClient* dbClient, void* sender);
        
        const String fullFileName() const;
        
        void detectFileType(const String& fullFileName);
        
    private:
        static const String fullFileName(const String& path, const String& fileName);

    protected:
        ConfigFile _file;
    };
}

#endif	// Configuration_h
