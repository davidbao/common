#include "xml/XmlDocument.h"
#include "exception/Exception.h"
#include "IO/File.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace Common
{
    class XmlDocumentInner
    {
    public:
        xmlDocPtr doc;
        
        XmlDocumentInner(xmlDocPtr doc = nullptr)
        {
            this->doc = doc;
        }
    };
    
	XmlDocument::XmlDocument(const String& fileName)
	{
        _doc = new XmlDocumentInner();
        
        if(!fileName.isNullOrEmpty())
        {
            load(fileName);
        }
        else
        {
            newDocument();
        }
	}

	XmlDocument::~XmlDocument()
	{
        if(isValid())
        {
            xmlFreeDoc(_doc->doc);
            _doc->doc = nullptr;
        }
        delete _doc;
	}
    
    bool XmlDocument::isValid() const
    {
        return _doc->doc != nullptr;
    }
    
	bool XmlDocument::load(const String& fileName)
	{
        if(File::exists(fileName))
        {
            if(isValid())
            {
                xmlFreeDoc(_doc->doc);
                _doc->doc = nullptr;
            }
            _doc->doc = xmlParseFile(fileName);
        }
        return _doc->doc != nullptr;
	}

	bool XmlDocument::save(const String& fileName)
	{
        if (isValid())
        {
            //Save the document back out to disk.
            xmlKeepBlanksDefault(0);//libxml2 global variable .
            xmlIndentTreeOutput = 1;// indent .with \n
            int result = xmlSaveFormatFileEnc(fileName, _doc->doc, nullptr, 1);
#ifdef __arm_linux__
            sync();    // save all of files.
#endif
            return result > 0;
        }
        return false;
    }

	bool XmlDocument::documentElement(XmlNode& node)
	{
		if (isValid())
		{
			node.setXmlNodeInner(_doc->doc->children);
            node._doc = this;
            return true;
		}
		return false;
	}
    
    bool XmlDocument::newDocument(const String& encoding)
    {
        if(isValid())
        {
            xmlFreeDoc(_doc->doc);
            _doc->doc = nullptr;
        }
        
        _doc->doc = xmlNewDoc(BAD_CAST"1.0");
        _doc->doc->encoding = new xmlChar[encoding.length()+1];
        strcpy((char*)_doc->doc->encoding, encoding);
        
        return isValid();
    }
    
    bool XmlDocument::createNode(const String& name, XmlNode& node)
    {
        if(isValid())
        {
            xmlNodePtr nodePtr = xmlNewNode(nullptr, (const xmlChar *)name.c_str());
            node.setNodePtr(nodePtr);
            node._doc = this;
            return nodePtr != nullptr;
        }
        return false;
    }
    
    bool XmlDocument::setRootNode(const XmlNode& rootNode)
    {
        if(isValid())
        {
            return xmlDocSetRootElement(_doc->doc, (xmlNodePtr)rootNode.nodePtr()) != nullptr;
        }
        return false;
    }
}
