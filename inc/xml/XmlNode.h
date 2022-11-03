#ifndef XMLNODE_H
#define XMLNODE_H

#include "data/DateTime.h"
#include "data/TimeSpan.h"
#include "data/Version.h"
#include "data/Point.h"
#include "data/Size.h"
#include "data/Rectangle.h"
#include "XmlAttribute.h"

namespace Common
{
    enum XmlNodeType
    {
        NodeNone = 0,
        Element = 1,
        Attribute = 2,
        Text = 3,
        CDATA = 4,
        EntityRef = 5,
        EntityDeclaration = 6,
        ProcessingInstruction = 7,
        Comments = 8,
        Document = 9,
        DocumentType = 10,
        DocumentFragment = 11,
        Notation = 12,
        Whitespace = 13,
        SignificantWhitespace = 14,
        EndElement = 15,
        EndEntity = 16,
        mlDeclaration = 17
    };
    
    class XmlDocument;
    class XmlNodeInner;
	class XmlNode
	{
	public:
        XmlNode();
        XmlNode(const String& name, const String& content = String::Empty);
		~XmlNode();
        
        bool isValid() const;
        const XmlAttributes& attributes() const;
        const String name() const;
        XmlNodeType nodeType() const;
        
        const String content() const;
        void setContent(const String& content);

		void selectSingleNode(const char* xpath, XmlNode& node);

        bool subNodes(PList<XmlNode>& nodes) const;
        bool hasSubNodes() const;
        
        bool containsNode(const String& name) const;
        
        bool appendNode(XmlNode& node);
        
        bool getSubNode(const String& name, XmlNode& node) const;
        bool getSubNode(const String& name, const String& value, XmlNode& node) const;
        bool getOrNewSubNode(const String& name, XmlNode& node);
        
        bool appendAttribute(const String& name, const String& value);
        bool appendAttribute(const String& name, const char* value);
        bool appendAttribute(const String& name, const String& value, XmlAttribute& attr);
        bool appendAttribute(const String& name, const bool& value);
        bool appendAttribute(const String& name, const char& value);
        bool appendAttribute(const String& name, const uint8_t& value);
        bool appendAttribute(const String& name, const short& value);
        bool appendAttribute(const String& name, const ushort& value);
        bool appendAttribute(const String& name, const int& value);
        bool appendAttribute(const String& name, const uint& value);
        bool appendAttribute(const String& name, const int64_t& value);
        bool appendAttribute(const String& name, const uint64_t& value);
        bool appendAttribute(const String& name, const float& value);
        bool appendAttribute(const String& name, const double& value);
        bool appendAttribute(const String& name, const DateTime& value);
        bool appendAttribute(const String& name, const TimeSpan& value);
        bool appendAttribute(const String& name, const Version& value);
        bool appendAttribute(const String& name, const Point& value);
        bool appendAttribute(const String& name, const PointF& value);
        bool appendAttribute(const String& name, const Size& value);
        bool appendAttribute(const String& name, const SizeF& value);
        bool appendAttribute(const String& name, const Rectangle& value);
        bool appendAttribute(const String& name, const RectangleF& value);
        template <class T>
        bool appendAttribute(const String& name, const T& value)
        {
            return appendAttribute(name, value.toString());
        }
        
        bool updateAttribute(const String& name, const String& value);
        bool updateAttribute(const String& name, const char*& value);
        bool updateAttribute(const String& name, const bool& value);
        bool updateAttribute(const String& name, const char& value);
        bool updateAttribute(const String& name, const uint8_t& value);
        bool updateAttribute(const String& name, const short& value);
        bool updateAttribute(const String& name, const ushort& value);
        bool updateAttribute(const String& name, const int& value);
        bool updateAttribute(const String& name, const uint& value);
        bool updateAttribute(const String& name, const int64_t& value);
        bool updateAttribute(const String& name, const uint64_t& value);
        bool updateAttribute(const String& name, const float& value);
        bool updateAttribute(const String& name, const double& value);
        bool updateAttribute(const String& name, const DateTime& value);
        bool updateAttribute(const String& name, const TimeSpan& value);
        bool updateAttribute(const String& name, const Version& value);
        bool updateAttribute(const String& name, const Point& value);
        bool updateAttribute(const String& name, const PointF& value);
        bool updateAttribute(const String& name, const Size& value);
        bool updateAttribute(const String& name, const SizeF& value);
        bool updateAttribute(const String& name, const Rectangle& value);
        bool updateAttribute(const String& name, const RectangleF& value);
        template <class T>
        bool updateAttribute(const String& name, const T& value)
        {
            return updateAttribute(name, value.toString());
        }
        
        XmlDocument* document() const;
        
        void copyFrom(const XmlNode* value);
        
        const String getAttribute(const String& name) const;
        bool getAttribute(const String& name, String& value) const;
        bool getAttribute(const String& name, bool& value) const;
        bool getAttribute(const String& name, char& value) const;
        bool getAttribute(const String& name, uint8_t& value) const;
        bool getAttribute(const String& name, short& value) const;
        bool getAttribute(const String& name, ushort& value) const;
        bool getAttribute(const String& name, int& value) const;
        bool getAttribute(const String& name, uint& value) const;
        bool getAttribute(const String& name, int64_t& value) const;
        bool getAttribute(const String& name, uint64_t& value) const;
        bool getAttribute(const String& name, float& value) const;
        bool getAttribute(const String& name, double& value) const;
        bool getAttribute(const String& name, DateTime& value) const;
        bool getAttribute(const String& name, TimeSpan& value) const;
        bool getAttribute(const String& name, Version& value) const;
        bool getAttribute(const String& name, Point& value) const;
        bool getAttribute(const String& name, PointF& value) const;
        bool getAttribute(const String& name, Size& value) const;
        bool getAttribute(const String& name, SizeF& value) const;
        bool getAttribute(const String& name, Rectangle& value) const;
        bool getAttribute(const String& name, RectangleF& value) const;
        template <class T>
        bool getAttribute(const String& name, T& value) const
        {
            return T::parse(getAttribute(name), value);
        }
        
        String toString() const;

	private:
        XmlNode(XmlDocument* doc, const void* node);
        
		void setXmlNodeInner(const void* node);
        
        void* nodePtr() const;
        void setNodePtr(void* node);

	private:
		friend class XmlDocument;
        friend class XmlAttribute;
        
        XmlDocument* _doc;

		XmlNodeInner* _node;

		XmlAttributes _attributes;
	};
    
    typedef PList<XmlNode> XmlNodes;
}

#endif	// XMLNODE_H
