#ifndef INSTRUCTIONDESCRIPTION_H
#define INSTRUCTIONDESCRIPTION_H

#include "data/Vector.h"
#include "exception/Exception.h"
#include "system/Convert.h"
#include "../IContextProperty.h"
#include "InstructionContext.h"

using namespace Common;

namespace Drivers
{
	class InstructionDescription
	{
	public:
        InstructionDescription(const String& name, InstructionContext* context = NULL, bool autoDeleteContext = true);
        ~InstructionDescription();

        const String& name() const;

        InstructionContext* context() const;
        void setContext(InstructionContext* context);
        
        bool allowExecution() const;

	private:
		String _name;
		InstructionContext* _context;
		bool _autoDeleteContext;
	};
    typedef Vector<InstructionDescription> InstructionDescriptions;
}

#endif // INSTRUCTIONDESCRIPTION_H
