#ifndef INSTRUCTIONDESCRIPTION_H
#define INSTRUCTIONDESCRIPTION_H

#include "data/PList.h"
#include "exception/Exception.h"
#include "data/Convert.h"
#include "../IContextProperty.h"
#include "InstructionContext.h"

using namespace Data;

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
    typedef PList<InstructionDescription> InstructionDescriptions;
}

#endif // INSTRUCTIONDESCRIPTION_H
