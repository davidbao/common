#ifndef XMLATTRIBUTE_H
#define XMLATTRIBUTE_H

#include "data/ValueType.h"
#include "data/Vector.h"

namespace Common
{
    class XmlNode;
    class XmlAttributeInner;
	class XmlAttribute
	{
	public:
		XmlAttribute();
		~XmlAttribute();

        bool isValid() const;

		const String name() const;
		const String value() const;
		bool setValue(const char* value);
        bool setValue(const String& value);
        
        void copyFrom(const XmlAttribute* value);

	private:
		void setXmlAttributeInner(const XmlNode* node, const void* attr);

	private:
		friend class XmlNode;

		XmlNode* _node;
		XmlAttributeInner* _attr;
	};
	//typedef Vector<XmlAttribute> XmlAttributes;
	class XmlAttributes : public CopyVector<XmlAttribute>
	{
	public:
        XmlAttributes(bool autoDelete = true, uint capacity = Vector<XmlAttribute>::DefaultCapacity);

        XmlAttribute* at(const String& name) const;
        XmlAttribute* operator[](const String& name) const;
        const String value(const String& name) const;

        XmlAttribute* at(size_t pos) const override;
	};
}

#endif	// XMLATTRIBUTE_H
