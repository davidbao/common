#ifndef ICONTEXTPROPERTY_H
#define ICONTEXTPROPERTY_H

#include "Context.h"

namespace Drivers
{
	class IContextProperty
	{
	public:
		virtual ~IContextProperty(){}
		virtual Context* context() const = 0;
	};
}

#endif // ICONTEXTPROPERTY_H
