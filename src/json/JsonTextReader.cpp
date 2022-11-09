#include "json/JsonTextReader.h"
#include "exception/Exception.h"
#include "data/Convert.h"
#include "IO/Path.h"
#include "data/ValueType.h"
#include "diag/Trace.h"

namespace Common
{
    JsonTextReader::JsonTextReader() : _iterator(-1)
    {
    }
    JsonTextReader::JsonTextReader(const String& fileName) : JsonTextReader()
	{
        FileStream fs(fileName, FileMode::FileOpen, FileAccess::FileRead);
        if(fs.isOpen())
        {
            _configFile.rootPath = Path::getDirectoryName(fileName);
            _configFile.fileName = Path::getFileName(fileName);
            
            String str;
            fs.readToEnd(str);
            JsonNode::parse(str, _currentNode);
        }

#ifdef DEBUG
        if(!isValid())
            Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
	}
//    JsonTextReader::JsonTextReader(const String& text, uint length)
//    {
//        _reader = new JsonTextReaderInner();
//
//        clear();
//        /*
//         * this initialize the library and check potential ABI mismatches
//         * between the version it was compiled for and the actual shared
//         * library used.
//         */
//        LIBXML_TEST_VERSION
//
//        _reader->reader = xmlReaderForMemory(text.c_str(), (int)length, nullptr, nullptr, 0);
//
//        _zipFile = nullptr;
//    }
    JsonTextReader::JsonTextReader(Zip* zip, const String& fileName) : JsonTextReader()
    {
        if(zip == nullptr)
            throw ArgumentException("zip");

#ifdef WIN32
        // fixbug: '/' must be used instead of '\' in windows.
        _configFile.fileName = String::replace(fileName, "\\", "/");
#else
        _configFile.fileName = fileName;
#endif

        String str;
        if(zip->read(_configFile.fileName, str))
            JsonNode::parse(str, _currentNode);
            
#ifdef DEBUG
            if(!isValid())
                Debug::writeFormatLine("Failed to construct JsonTextReader! file name: %s", fileName.c_str());
#endif
    }
//    JsonTextReader::JsonTextReader(const String& zipFileName, const String& fileName)
//    {
//        _reader = new JsonTextReaderInner();
//
//        clear();
//        /*
//         * this initialize the library and check potential ABI mismatches
//         * between the version it was compiled for and the actual shared
//         * library used.
//         */
//        LIBXML_TEST_VERSION
//
//        Zip* zip = new Zip(zipFileName);
//        if(zip->isValid())
//        {
//            _deleteZip = true;
//            _configFile.zip = zip;
//#ifdef WIN32
//            // fixbug: '/' must be used instead of '\' in windows.
//            _configFile.fileName = String::replace(fileName, "\\", "/");
//#else
//            _configFile.fileName = fileName;
//#endif
//            _zipFile = zip->open(_configFile.fileName);
//            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
//        }
//        else
//        {
//            delete zip;
//        }
//    }

	JsonTextReader::~JsonTextReader()
	{
        close();
        
//        if(_configFile.isZip() && _zipFile != nullptr)
//        {
//            _configFile.zip->close(_zipFile);
//            if(_deleteZip)
//            {
//                delete _configFile.zip;
//            }
//            _configFile.zip = nullptr;
//            _zipFile = nullptr;
//        }
	}

	bool JsonTextReader::isValid() const
	{
        return !_currentNode.isEmpty();
	}
    
    bool JsonTextReader::close()
    {
        if (isValid())
        {
            return true;
        }
        return false;
    }

    bool JsonTextReader::read(const String& name, JsonTextReader& reader) const
    {
        if (isValid())
        {
            JsonNode* currentNode = this->currentNode();
            JsonNode node;
            if(currentNode->atByName(name, node))
            {
                reader._currentNode = node;

                return true;
            }
        }
        return false;
    }
    bool JsonTextReader::read(JsonTextReader& reader)
    {
        if (isValid())
        {
            JsonNode* currentNode = this->currentNode();
            JsonNode node;
            if(currentNode->at(++_iterator, node))
            {
                reader._currentNode = node;
                
                return true;
            }
            else
            {
                _iterator = -1;
            }
        }
        return false;
    }
    
    JsonNode::Type JsonTextReader::nodeType() const
    {
        return this->currentNode()->type();
    }

	String JsonTextReader::name() const
	{
        return currentNode()->name();
	}
    
    JsonNode* JsonTextReader::currentNode() const
    {
        return (JsonNode*)&_currentNode;
    }

	const String JsonTextReader::getAttribute(const String& name)
	{
        if (isValid())
        {
            String value;
            if(currentNode()->getAttribute(name, value))
                return value;
        }
		return String::Empty;
	}
    bool JsonTextReader::getAttribute(const String& name, bool& value)
    {
        return Boolean::parse(getAttribute(name), value);
    }
    bool JsonTextReader::getAttribute(const String& name, char& value, char minValue, char maxValue)
    {
        return Char::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, uint8_t& value, uint8_t minValue, uint8_t maxValue)
    {
        return Byte::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, short& value, short minValue, short maxValue)
    {
        return Int16::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, ushort& value, ushort minValue, ushort maxValue)
    {
        return UInt16::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, int& value, int minValue, int maxValue)
    {
        return Int32::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, uint& value, uint minValue, uint maxValue)
    {
        return UInt32::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, int64_t& value, int64_t minValue, int64_t maxValue)
    {
        return Int64::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, uint64_t& value, uint64_t minValue, uint64_t maxValue)
    {
        return UInt64::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, float& value, float minValue, float maxValue)
    {
        return Float::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, double& value, double minValue, double maxValue)
    {
        return Double::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, String& value)
    {
        value = getAttribute(name);
        return true;
    }
    bool JsonTextReader::getAttribute(const String& name, DateTime& value, DateTime minValue, DateTime maxValue)
    {
        return DateTime::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool JsonTextReader::getAttribute(const String& name, TimeSpan& value, const TimeSpan& minValue, const TimeSpan& maxValue)
    {
        TimeSpan ts;
        if(TimeSpan::parse(getAttribute(name), ts) &&
           ts >= minValue && ts <= maxValue)
        {
            value = ts;
            return true;
        }
        return false;
    }
    bool JsonTextReader::getAttribute(const String& name, Version& value)
    {
        return Version::parse(getAttribute(name), value);
    }
    bool JsonTextReader::getAttribute(const String& name, IPAddress& value)
    {
        return IPAddress::parse(getAttribute(name), value);
    }
    bool JsonTextReader::getAttribute(const String& name, MacAddress& value)
    {
        return MacAddress::parse(getAttribute(name), value);
    }
    bool JsonTextReader::getAttribute(const String& name, Uuid& value)
    {
        return Uuid::parse(getAttribute(name), value);
    }
    
    const ConfigFile& JsonTextReader::configFile() const
    {
        return _configFile;
    }
}
