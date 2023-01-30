#if !__ANDROID__ && !WIN32 && !__EMSCRIPTEN__ && !MSYS

#include <execinfo.h>

#endif

#include "exception/Exception.h"
#include "diag/Trace.h"

using namespace Diag;

namespace System {
    Exception::Exception(const String &message, const Exception *innerException) {
        _message = message;
        _innerException = innerException;
    }

    Exception::Exception(const Exception &e) {
        _message = e._message;
        _innerException = e._innerException;
    }

    Exception::~Exception() {
        _innerException = nullptr;
    }

    const char *Exception::what() const noexcept {
        return _message.c_str();
    }

    const String &Exception::message() const {
        return _message;
    }

    const Exception *Exception::innerException() const {
        return _innerException;
    }

    void Exception::showExceptionInfo() const {
        showExceptionInfo(this);
    }

    void Exception::showExceptionInfo(const Exception *exception) {
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

    MessageException::MessageException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                           innerException) {
        showExceptionInfo();
    }

    MessageException::MessageException(const MessageException &e) = default;

    MessageException::~MessageException() = default;

    void MessageException::showMessage() const {
        Trace::writeFormatLine("throw a message exception, message: %s", message().c_str());
        print_stacktrace();
    }

    BindingException::BindingException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                           innerException) {
        showExceptionInfo();
    }

    BindingException::BindingException(const BindingException &e) = default;

    BindingException::~BindingException() = default;

    void BindingException::showMessage() const {
        Trace::writeFormatLine("throw a binding exception, message: %s", message().c_str());
        print_stacktrace();
    }

    OverflowException::OverflowException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                             innerException) {
        showExceptionInfo();
    }

    OverflowException::OverflowException(const OverflowException &e) = default;

    OverflowException::~OverflowException() = default;

    void OverflowException::showMessage() const {
        Trace::writeFormatLine("throw an overflow exception, message: %s", message().c_str());
        print_stacktrace();
    }

    StreamException::StreamException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                         innerException) {
        showExceptionInfo();
    }

    StreamException::StreamException(const StreamException &e) = default;

    StreamException::~StreamException() = default;

    void StreamException::showMessage() const {
        Trace::writeFormatLine("throw a stream exception, message: %s", message().c_str());
        print_stacktrace();
    }

    IOException::IOException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                 innerException) {
        showExceptionInfo();
    }

    IOException::IOException(const IOException &e) = default;

    IOException::~IOException() = default;

    void IOException::showMessage() const {
        Trace::writeFormatLine("throw an IO exception, message: %s", message().c_str());
        print_stacktrace();
    }

    ArgumentException::ArgumentException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                             innerException) {
        showExceptionInfo();
    }

    ArgumentException::ArgumentException(const String &message, const String &paramName,
                                         const Exception *innerException) : Exception(message, innerException) {
        _paramName = paramName;
        showExceptionInfo();
    }

    ArgumentException::ArgumentException(const ArgumentException &e) : Exception(e) {
        _paramName = e._paramName;
    }

    ArgumentException::~ArgumentException() = default;

    const String &ArgumentException::paramName() const {
        return _paramName;
    }

    void ArgumentException::showMessage() const {
        Trace::writeFormatLine("throw an argument exception, message: %s, paramName: %s",
                               message().c_str(), _paramName.c_str());
        print_stacktrace();
    }

    const String ArgumentNullException::ArgumentNull_Generic = "Value cannot be null.";

    ArgumentNullException::ArgumentNullException(const String &paramName) : ArgumentException(ArgumentNull_Generic,
                                                                                              paramName) {
        showExceptionInfo();
    }

    ArgumentNullException::ArgumentNullException(const ArgumentNullException &e) = default;

    ArgumentNullException::~ArgumentNullException() = default;

    void ArgumentNullException::showMessage() const {
        Trace::writeFormatLine("throw an argument null exception, message: %s, paramName: %s",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &message, const Exception *innerException)
            : ArgumentException(message, innerException) {
        showExceptionInfo();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &message, const String &paramName,
                                                             const Exception *innerException) : ArgumentException(
            message, paramName, innerException) {
        showExceptionInfo();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const ArgumentOutOfRangeException &e) = default;

    ArgumentOutOfRangeException::~ArgumentOutOfRangeException() = default;

    void ArgumentOutOfRangeException::showMessage() const {
        Trace::writeFormatLine("throw an argument out of range exception, message: %s, paramName: %s",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    TimeoutException::TimeoutException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                           innerException) {
        showExceptionInfo();
    }

    TimeoutException::TimeoutException(const TimeoutException &e) = default;

    TimeoutException::~TimeoutException() = default;

    void TimeoutException::showMessage() const {
        Trace::writeFormatLine("throw a timeout exception, message: %s", message().c_str());
        print_stacktrace();
    }

    NotImplementedException::NotImplementedException(const String &message, const Exception *innerException)
            : Exception(message, innerException) {
        showExceptionInfo();
    }

    NotImplementedException::NotImplementedException(const NotImplementedException &e) = default;

    NotImplementedException::~NotImplementedException() = default;

    void NotImplementedException::showMessage() const {
        Trace::writeFormatLine("throw a not implemented exception, message: %s", message().c_str());
        print_stacktrace();
    }

    NotSupportedException::NotSupportedException(const String &message, const Exception *innerException) : Exception(
            message, innerException) {
        showExceptionInfo();
    }

    NotSupportedException::NotSupportedException(const NotSupportedException &e) = default;

    NotSupportedException::~NotSupportedException() = default;

    void NotSupportedException::showMessage() const {
        Trace::writeFormatLine("throw a not supported exception, message: %s", message().c_str());
        print_stacktrace();
    }

    FileNotFoundException::FileNotFoundException(const String &fileName, const Exception *innerException) : Exception(
            fileName, innerException) {
        showExceptionInfo();
    }

    FileNotFoundException::~FileNotFoundException() = default;

    FileNotFoundException::FileNotFoundException(const FileNotFoundException &e) = default;

    const String &FileNotFoundException::fileName() const {
        return message();
    }

    void FileNotFoundException::showMessage() const {
        Trace::writeFormatLine("throw a file not found exception, file name: %s", fileName().c_str());
        print_stacktrace();
    }

    PathNotFoundException::PathNotFoundException(const String &path, const Exception *innerException) : Exception(
            path, innerException) {
        showExceptionInfo();
    }

    PathNotFoundException::~PathNotFoundException() = default;

    PathNotFoundException::PathNotFoundException(const PathNotFoundException &e) = default;

    const String &PathNotFoundException::path() const {
        return message();
    }

    void PathNotFoundException::showMessage() const {
        Trace::writeFormatLine("throw a path not found exception, file name: %s", path().c_str());
        print_stacktrace();
    }
}
