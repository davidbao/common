#include "system/DynamicLoader.h"
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

namespace System {
    DynamicLoader::DynamicLoader(const char *libName) : _handle(nullptr) {
        if (libName == nullptr || strlen(libName) == 0)
            throw ArgumentException("libName");

#if WIN32
        _handle = LoadLibrary(libName);
        if(_handle == nullptr)
        {
            retrieveError();
        }
        else
        {
            _error.empty();
        }
#elif HAVE_DLOPEN
        _handle = dlopen(libName, RTLD_LAZY);
        if (_handle == nullptr) {
            retrieveError();
        } else {
            _error.empty();
            // reset errors
            dlerror();
        }
#endif
    }

    DynamicLoader::DynamicLoader(const String &libName) : DynamicLoader(libName.c_str()) {
    }

    DynamicLoader::~DynamicLoader() {
        if (_handle != nullptr) {
#if WIN32
            FreeLibrary((HINSTANCE)_handle);
#elif HAVE_DLOPEN
            dlclose(_handle);
#endif
            _handle = nullptr;
        }
    }

    void *DynamicLoader::getSymbol(const char *symbol) {
        if (symbol == nullptr || strlen(symbol) == 0)
            throw ArgumentException("symbol");

        if (_handle != nullptr) {
            _error.empty();

#if WIN32
            FARPROC result = GetProcAddress((HINSTANCE)_handle, symbol);
            if(result == nullptr)
            {
                retrieveError();
            }
            return (void*)result;
#elif HAVE_DLOPEN
            void *result = dlsym(_handle, symbol);
            if (result == nullptr) {
                retrieveError();
            }
            return result;
#endif
        }
        return nullptr;
    }

    void *DynamicLoader::getSymbol(const String &symbol) {
        return getSymbol(symbol.c_str());
    }

    bool DynamicLoader::hasError() const {
        return !_error.isNullOrEmpty();
    }

    String DynamicLoader::error() const {
        return _error;
    }

    void DynamicLoader::retrieveError() {
#if WIN32
        // Retrieve the system error message for the last-error code

        LPVOID lpMsgBuf;
        DWORD dw = GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, nullptr);

        _error = (LPTSTR)lpMsgBuf;
        LocalFree(lpMsgBuf);
#elif HAVE_DLOPEN
        _error = dlerror();
#endif
    }
}
