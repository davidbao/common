#ifndef DYNAMICLOADER_H
#define DYNAMICLOADER_H

#include "data/ValueType.h"

namespace Common
{
    class DynamicLoader
    {
    public:
        DynamicLoader(const char* libName);
        DynamicLoader(const String& libName);
        ~DynamicLoader();
        
        void* getSymbol(const char* symbol);
        void* getSymbol(const String& symbol);
        
        bool hasError() const;
        String error() const;

	private:
		void retrieveError();
        
    private:
        void* _handle;
        String _error;
    };
}

#endif	// DYNAMICLOADER_H
