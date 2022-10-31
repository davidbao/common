#include "system/DynamicLoader.h"
#include "system/Convert.h"
#include "exception/Exception.h"

#ifdef __EMSCRIPTEN__
#undef HAVE_DLOPEN
#else
#define HAVE_DLOPEN 1
#endif

#if WIN32
#include <Windows.h>
#elif HAVE_DLOPEN
#include <dlfcn.h>
#endif

namespace Common
{
    DynamicLoader::DynamicLoader(const char* libName) : _handle(nullptr)
    {
        if (libName == NULL || strlen(libName) == 0)
            throw ArgumentException("libName");
        
#if WIN32
		_handle = LoadLibrary(libName);
		if(_handle == NULL)
		{
			retrieveError();
		}
		else
		{
            _error.empty();
		}
#elif HAVE_DLOPEN
        _handle = dlopen(libName, RTLD_LAZY);
        if(_handle == NULL)
        {
			retrieveError();
        }
        else
        {
            _error.empty();
            // reset errors
            dlerror();
        }
#endif
	}
    DynamicLoader::DynamicLoader(const String& libName) : DynamicLoader(libName.c_str())
    {
    }
    DynamicLoader::~DynamicLoader()
    {
        if(_handle != NULL)
        {
#if WIN32
			FreeLibrary((HINSTANCE)_handle);
#elif HAVE_DLOPEN
            dlclose(_handle);
#endif
            _handle = NULL;
        }
    }
    
    void* DynamicLoader::getSymbol(const char* symbol)
    {
        if (symbol == NULL || strlen(symbol) == 0)
            throw ArgumentException("symbol");
        
        if(_handle != NULL)
        {
            _error.empty();
            
#if WIN32
			FARPROC result = GetProcAddress((HINSTANCE)_handle, symbol);
            if(result == NULL)
            {
                retrieveError();
            }
            return (void*)result;
#elif HAVE_DLOPEN
            void* result = dlsym(_handle, symbol);
            if(result == NULL)
            {
                retrieveError();
            }
            return result;
#endif
        }
        return NULL;
    }
    void* DynamicLoader::getSymbol(const String& symbol)
    {
        return getSymbol(symbol.c_str());
    }
    
    bool DynamicLoader::hasError() const
    {
        return !_error.isNullOrEmpty();
    }
    String DynamicLoader::error() const
    {
        return _error;
    }

	void DynamicLoader::retrieveError()
	{
#if WIN32
		// Retrieve the system error message for the last-error code

		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		_error = (LPTSTR)lpMsgBuf;
		LocalFree(lpMsgBuf);
#elif HAVE_DLOPEN
		_error = dlerror();
#endif
	}
}
