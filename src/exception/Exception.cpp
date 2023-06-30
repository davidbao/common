#if !__ANDROID__ && !WIN32 && !__EMSCRIPTEN__ && !MSYS

#include <execinfo.h>

#endif

#include "exception/Exception.h"
#include "diag/Trace.h"

using namespace Diag;

namespace System {
    Exception::Exception(const String &message, const Exception *innerException) noexcept {
        _message = message;
        _innerException = innerException;
    }

    Exception::Exception(const Exception &e) noexcept {
        _message = e._message;
        _innerException = e._innerException;
    }

    Exception& Exception::operator=(const Exception& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    Exception::~Exception() noexcept {
        _innerException = nullptr;
    }

    const char *Exception::what() const noexcept {
        return _message.c_str();
    }

    const String &Exception::message() const noexcept {
        return _message;
    }

    const Exception *Exception::innerException() const noexcept {
        return _innerException;
    }

    void Exception::showExceptionInfo() const noexcept {
        showExceptionInfo(this);
    }

    void Exception::showExceptionInfo(const Exception *exception) noexcept {
        if (exception != nullptr) {
            if (exception->_innerException != nullptr) {
                showExceptionInfo(exception->_innerException);
            } else {
                exception->showMessage();
            }
        }
    }

    void Exception::print_stacktrace() {
//#if !__ANDROID__ && !WIN32
//        int size = 16;
//        void * array[16];
//        int stack_num = backtrace(array, size);
//        char ** stacktrace = backtrace_symbols(array, stack_num);
//        for (int i = 0; i < stack_num; ++i)
//        {
//            Trace::writeLine(stacktrace[i], Trace::Error);
//        }
//        free(stacktrace);
//#endif
    }

    MessageException::MessageException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
        
    }

    MessageException::MessageException(const MessageException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    MessageException& MessageException::operator=(const MessageException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    MessageException::~MessageException() noexcept = default;

    void MessageException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a message exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    BindingException::BindingException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
    }

    BindingException::BindingException(const BindingException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    BindingException& BindingException::operator=(const BindingException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    BindingException::~BindingException() noexcept = default;

    void BindingException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a binding exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    OverflowException::OverflowException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
    }

    OverflowException::OverflowException(const OverflowException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    OverflowException& OverflowException::operator=(const OverflowException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    OverflowException::~OverflowException() noexcept = default;

    void OverflowException::showMessage() const noexcept {
        Trace::writeFormatLine("throw an overflow exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    StreamException::StreamException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
    }

    StreamException::StreamException(const StreamException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    StreamException& StreamException::operator=(const StreamException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    StreamException::~StreamException() noexcept = default;

    void StreamException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a stream exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    IOException::IOException(const String &message, const Exception *innerException) noexcept: Exception(
            message, innerException) {
    }

    IOException::IOException(const IOException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    IOException& IOException::operator=(const IOException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    IOException::~IOException() noexcept = default;

    void IOException::showMessage() const noexcept {
        Trace::writeFormatLine("throw an IO exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    ArgumentException::ArgumentException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
        
    }

    ArgumentException::ArgumentException(const String &message, const String &paramName,
                                         const Exception *innerException) noexcept: Exception(message, innerException) {
        _paramName = paramName;
        
    }

    ArgumentException::ArgumentException(const ArgumentException &e) noexcept: Exception(e) {
        _paramName = e._paramName;
    }

    ArgumentException& ArgumentException::operator=(const ArgumentException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
            _paramName = e._paramName;
        }
        return *this;
    }

    ArgumentException::~ArgumentException() noexcept = default;

    const String &ArgumentException::paramName() const noexcept {
        return _paramName;
    }

    void ArgumentException::showMessage() const noexcept {
        Trace::writeFormatLine("throw an argument exception, message: '%s', paramName: '%s'",
                               message().c_str(), _paramName.c_str());
        print_stacktrace();
    }

    const String ArgumentNullException::ArgumentNull_Generic = "Value cannot be null.";

    ArgumentNullException::ArgumentNullException(const String &paramName) noexcept: ArgumentException(
            ArgumentNull_Generic,
            paramName) {
    }

    ArgumentNullException::ArgumentNullException(const ArgumentNullException &e) noexcept : ArgumentException(e) {
        _message = e._message;
        _innerException = e._innerException;
        _paramName = e._paramName;
    }

    ArgumentNullException& ArgumentNullException::operator=(const ArgumentNullException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
            _paramName = e._paramName;
        }
        return *this;
    }

    ArgumentNullException::~ArgumentNullException() noexcept = default;

    void ArgumentNullException::showMessage() const noexcept {
        Trace::writeFormatLine("throw an argument null exception, message: '%s', paramName: '%s'",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &message,
                                                             const Exception *innerException) noexcept
            : ArgumentException(message, innerException) {
        
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &message, const String &paramName,
                                                             const Exception *innerException) noexcept :
                                                             ArgumentException(message, paramName, innerException) {
        
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const ArgumentOutOfRangeException &e) noexcept :
        ArgumentException(e) {
        _message = e._message;
        _innerException = e._innerException;
        _paramName = e._paramName;
    }

    ArgumentOutOfRangeException& ArgumentOutOfRangeException::operator=(const ArgumentOutOfRangeException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
            _paramName = e._paramName;
        }
        return *this;
    }

    ArgumentOutOfRangeException::~ArgumentOutOfRangeException() noexcept = default;

    void ArgumentOutOfRangeException::showMessage() const noexcept {
        Trace::writeFormatLine("throw an argument out of range exception, message: '%s', paramName: '%s'",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    TimeoutException::TimeoutException(const String &message, const Exception *innerException) noexcept: Exception(
            message,
            innerException) {
    }

    TimeoutException::TimeoutException(const TimeoutException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    TimeoutException& TimeoutException::operator=(const TimeoutException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    TimeoutException::~TimeoutException() noexcept = default;

    void TimeoutException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a timeout exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    NotImplementedException::NotImplementedException(const String &message, const Exception *innerException) noexcept
            : Exception(message, innerException) {
        
    }

    NotImplementedException::NotImplementedException(const NotImplementedException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    NotImplementedException& NotImplementedException::operator=(const NotImplementedException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    NotImplementedException::~NotImplementedException() noexcept = default;

    void NotImplementedException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a not implemented exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    NotSupportedException::NotSupportedException(const String &message, const Exception *innerException) noexcept
            : Exception(
            message, innerException) {
    }

    NotSupportedException::NotSupportedException(const NotSupportedException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    NotSupportedException& NotSupportedException::operator=(const NotSupportedException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    NotSupportedException::~NotSupportedException() noexcept = default;

    void NotSupportedException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a not supported exception, message: '%s'", message().c_str());
        print_stacktrace();
    }

    FileNotFoundException::FileNotFoundException(const String &fileName, const Exception *innerException) noexcept
            : Exception(
            fileName, innerException) {
    }

    FileNotFoundException::FileNotFoundException(const FileNotFoundException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    FileNotFoundException& FileNotFoundException::operator=(const FileNotFoundException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    FileNotFoundException::~FileNotFoundException() noexcept = default;

    const String &FileNotFoundException::fileName() const noexcept {
        return message();
    }

    void FileNotFoundException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a file not found exception, file name: '%s'", fileName().c_str());
        print_stacktrace();
    }

    PathNotFoundException::PathNotFoundException(const String &path, const Exception *innerException) noexcept
            : Exception(path, innerException) {
    }

    PathNotFoundException& PathNotFoundException::operator=(const PathNotFoundException& e) noexcept {
        if (&e != this) {
            _message = e._message;
            _innerException = e._innerException;
        }
        return *this;
    }

    PathNotFoundException::~PathNotFoundException() noexcept = default;

    PathNotFoundException::PathNotFoundException(const PathNotFoundException &e) noexcept : Exception(e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    const String &PathNotFoundException::path() const noexcept {
        return message();
    }

    void PathNotFoundException::showMessage() const noexcept {
        Trace::writeFormatLine("throw a path not found exception, file name: '%s'", path().c_str());
        print_stacktrace();
    }
}
