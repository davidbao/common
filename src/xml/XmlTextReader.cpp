//
//  XmlTextReader.cpp
//  common
//
//  Created by baowei on 2017/2/1.
//  Copyright © 2017 com. All rights reserved.
//

#include "xml/XmlTextReader.h"
#include "exception/Exception.h"
#include "data/Convert.h"
#include "IO/Path.h"
#include "data/ValueType.h"
#include "diag/Trace.h"
#include "libxml/xmlreader.h"

namespace Common
{
    class XmlTextReaderInner
    {
    public:
        xmlTextReaderPtr reader;
        
        XmlTextReaderInner() : reader(nullptr)
        {
        }
    };
    
	XmlTextReader::XmlTextReader(const String& fileName)
	{
        _reader = new XmlTextReaderInner();
        
        clear();
		/*
		* this initialize the library and check potential ABI mismatches
		* between the version it was compiled for and the actual shared
		* library used.
		*/
		LIBXML_TEST_VERSION

        _reader->reader = xmlReaderForFile(fileName, nullptr, 0);
        
        if(_reader->reader != nullptr)
        {
            _configFile.rootPath = Path::getDirectoryName(fileName);
            _configFile.fileName = Path::getFileName(fileName);
        }
		else
		{
			Debug::writeFormatLine("Failed to construct XmlTextReader! file name: %s", fileName.c_str());
		}
        _zipFile = nullptr;
	}
    XmlTextReader::XmlTextReader(const String& text, size_t length)
    {
        _reader = new XmlTextReaderInner();
        
        clear();
        /*
         * this initialize the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        
        if(length == 0 || length > text.length())
            length = text.length();
        _reader->reader = xmlReaderForMemory(text.c_str(), (int)length, nullptr, nullptr, 0);

        _zipFile = nullptr;
    }
    XmlTextReader::XmlTextReader(Zip* zip, const String& fileName)
    {
        if(zip == nullptr)
            throw ArgumentException("zip");
        
        _reader = new XmlTextReaderInner();
        
        clear();
        /*
         * this initialize the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        
        _deleteZip = false;
        _configFile.zip = zip;
#ifdef WIN32
        // fixbug: '/' must be used instead of '\' in windows.
        _configFile.fileName = String::replace(fileName, "\\", "/");
#else
        _configFile.fileName = fileName;
#endif
        _zipFile = zip->open(_configFile.fileName);
        if(_zipFile != nullptr)
            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
    }
    XmlTextReader::XmlTextReader(const String& zipFileName, const String& fileName)
    {
        _reader = new XmlTextReaderInner();
        
        clear();
        /*
         * this initialize the library and check potential ABI mismatches
         * between the version it was compiled for and the actual shared
         * library used.
         */
        LIBXML_TEST_VERSION
        
        Zip* zip = new Zip(zipFileName);
        if(zip->isValid())
        {
            _deleteZip = true;
            _configFile.zip = zip;
#ifdef WIN32
            // fixbug: '/' must be used instead of '\' in windows.
            _configFile.fileName = String::replace(fileName, "\\", "/");
#else
            _configFile.fileName = fileName;
#endif
            _zipFile = zip->open(_configFile.fileName);
            _reader->reader = xmlReaderForIO(Zip::zipRead, nullptr, _zipFile->context(), nullptr, nullptr, 0);
        }
        else
        {
            delete zip;
        }
    }

	XmlTextReader::~XmlTextReader()
	{
		if (isValid())
		{
			xmlFreeTextReader(_reader->reader);
		}
        
        if(_configFile.isZip() && _zipFile != nullptr)
        {
            _configFile.zip->close(_zipFile);
            if(_deleteZip)
            {
                delete _configFile.zip;
            }
            _configFile.zip = nullptr;
            _zipFile = nullptr;
        }
        
        delete _reader;
	}

	bool XmlTextReader::isValid() const
	{
		return _reader->reader != nullptr && xmlTextReaderIsValid(_reader->reader) != XmlFailed;
	}

	void XmlTextReader::clear()
	{
		_name = String::Empty;
		_localName = String::Empty;
		_namespaceUri = String::Empty;
		_value = String::Empty;
		_nodeType = NodeNone;
	}

	bool XmlTextReader::read()
	{
		if (isValid())
		{
			clear();
			return xmlTextReaderRead(_reader->reader) == XmlSuccess;
			//int ret = xmlTextReaderRead(_reader->reader);
			//while (ret == 1)
			//{
			//	const xmlChar *name, *value;

			//	name = xmlTextReaderConstName(_reader->reader);
			//	value = xmlTextReaderConstValue(_reader->reader);
			//	ret = xmlTextReaderRead(_reader->reader);
			//}
			///*
			//* Once the document has been fully parsed check the validation results
			//*/
			//if (xmlTextReaderIsValid(_reader->reader) != 1) {
			//	fprintf(stderr, "Document %s does not validate\n", fileName);
			//}
			//xmlFreeTextReader(_reader->reader);
			//if (ret != 0) {
			//	fprintf(stderr, "%s : failed to parse\n", fileName);
			//}
		}
		return false;
	}

	XmlNodeType XmlTextReader::nodeType()
	{
		if (_nodeType == NodeNone)
		{
			if (isValid())
			{
				_nodeType = (XmlNodeType)xmlTextReaderNodeType(_reader->reader);
			}
		}
		return _nodeType;
	}

	const String& XmlTextReader::localName()
	{
		if (_localName.isNullOrEmpty())
		{
			if (isValid())
			{
				const xmlChar* result = xmlTextReaderConstLocalName(_reader->reader);
#if WIN32
				_localName = result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
                _localName = result != nullptr ? (char*)result : String::Empty;
#endif
			}
		}
		return _localName;
	}

	const String& XmlTextReader::namespaceUri()
	{
		if (_namespaceUri.isNullOrEmpty())
		{
			if (isValid())
			{
				const xmlChar* result = xmlTextReaderConstNamespaceUri(_reader->reader);
#if WIN32
                _namespaceUri = result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
                _namespaceUri = result != nullptr ? (char*)result : String::Empty;
#endif
			}
		}
		return _namespaceUri;
	}

	const String& XmlTextReader::name()
	{
		if (_name.isNullOrEmpty())
		{
			if (isValid())
			{
				const xmlChar* result = xmlTextReaderConstName(_reader->reader);
#if WIN32
                _name = result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
                _name = result != nullptr ? (char*)result : String::Empty;
#endif
			}
		}
		return _name;
	}

	const String& XmlTextReader::value()
	{
		if (_value.isNullOrEmpty())
		{
			if (isValid())
			{
				const xmlChar* result = xmlTextReaderConstValue(_reader->reader);
#if WIN32
                _value = result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
#else
                _value = result != nullptr ? (char*)result : String::Empty;
#endif
			}
		}
		return _value;
	}
    bool XmlTextReader::isEmptyElement() const
    {
        if (isValid())
        {
            return xmlTextReaderIsEmptyElement(_reader->reader) == 1;
        }
        return false;
    }

	XmlNodeType XmlTextReader::moveToContent()
	{
		if (isValid())
		{
			XmlNodeType type;

		LNext:
			type = nodeType();
			switch (type)
			{
			case Element:
			case Text:
			case CDATA:
			case EntityRef:
			case EntityDeclaration:
			case EndElement:
			case EndEntity:
				break;

			case Attribute:
				moveToElement();
				break;

			default:
				if (read())
				{
					goto LNext;
				}
				break;
			}
			return type;
		}
		return XmlNodeType::NodeNone;
	}

	bool XmlTextReader::moveToElement()
	{
		if (isValid())
		{
			clear();
			return xmlTextReaderMoveToElement(_reader->reader) == XmlSuccess;
		}
		return false;
	}

	const String XmlTextReader::getAttribute(const String& name) const
	{
        String value;
        getAttribute(name, value);
        return value;
	}
    bool XmlTextReader::getAttribute(const String& name, String& value) const
    {
        if (isValid())
        {
            xmlChar* result = xmlTextReaderGetAttribute(_reader->reader, (const xmlChar*)name.c_str());
            if (result == nullptr)
                return false;
         
            String attribute = (char*)result;
//#if WIN32
//            String attribute = result != nullptr ? String::UTF8toGBK((char*)result) : String::Empty;
//#else
//            String attribute = result != nullptr ? (char*)result : String::Empty;
//#endif
            xmlFree(result);
            
            if (attribute.find("&lt;") >= 0)
                attribute = attribute.replace("&lt;", "<");
            if (attribute.find("&gt;") >= 0)
                attribute = attribute.replace("&gt;", ">");
            if (attribute.find("&amp;") >= 0)
                attribute = attribute.replace("&amp;", "&");
            if (attribute.find("&apos;") >= 0)
                attribute = attribute.replace("&apos;", "'");
            if (attribute.find("&quot;") >= 0)
                attribute = attribute.replace("&quot;", "\"");
            
            value = attribute;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, bool& value) const
    {
        return Boolean::parse(getAttribute(name), value);
    }
    bool XmlTextReader::getAttribute(const String& name, char& value, char minValue, char maxValue) const
    {
        char temp;
        if(Char::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, uint8_t& value, uint8_t minValue, uint8_t maxValue) const
    {
        uint8_t temp;
        if(Byte::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, short& value, short minValue, short maxValue) const
    {
        short temp;
        if(Int16::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, ushort& value, ushort minValue, ushort maxValue) const
    {
        ushort temp;
        if(UInt16::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, int& value, int minValue, int maxValue) const
    {
        int temp;
        if(Int32::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, uint& value, uint minValue, uint maxValue) const
    {
        uint temp;
        if(UInt32::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, int64_t& value, int64_t minValue, int64_t maxValue) const
    {
        int64_t temp;
        if(Int64::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, uint64_t& value, uint64_t minValue, uint64_t maxValue) const
    {
        uint64_t temp;
        if(UInt64::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, float& value, float minValue, float maxValue) const
    {
        float temp;
        if(Float::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, double& value, double minValue, double maxValue) const
    {
        double temp;
        if(Double::parse(getAttribute(name), temp) && (temp >= minValue && temp <= maxValue))
        {
            value = temp;
            return true;
        }
        return false;
    }
    bool XmlTextReader::getAttribute(const String& name, DateTime& value, DateTime minValue, DateTime maxValue) const
    {
        return DateTime::parse(getAttribute(name), value) && (value >= minValue && value <= maxValue);
    }
    bool XmlTextReader::getAttribute(const String& name, TimeSpan& value, const TimeSpan& minValue, const TimeSpan& maxValue) const
    {
        TimeSpan temp;
        if(TimeSpan::parse(getAttribute(name), temp) && temp >= minValue && temp <= maxValue)
        {
            value = temp;
            return true;
        }
        return false;
    }

    const ConfigFile& XmlTextReader::configFile() const
    {
        return _configFile;
    }
}
