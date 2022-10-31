#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H

#include "data/ValueType.h"
#include "XmlNode.h"

namespace Common
{
    class XmlDocumentInner;
	class XmlDocument
	{
	public:
        XmlDocument(const String& fileName = String::Empty);
		~XmlDocument();

		bool load(const String& fileName);
		bool save(const String& fileName);

		bool documentElement(XmlNode& node);
        
        bool newDocument(const String& encoding = "utf-8");

        bool isValid() const;
        
        bool createNode(const String& name, XmlNode& node);
        
        bool setRootNode(const XmlNode& rootNode);

	private:
		XmlDocumentInner* _doc;
        
        XmlNode _rootNode;
	};
}

#endif	// XMLDOCUMENT_H
