#if !__ANDROID__ && !WIN32 && !__EMSCRIPTEN__ && !MSYS

#include <execinfo.h>

#endif

#include "exception/Exception.h"
#include "diag/Trace.h"

namespace Common {
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

    void Exception::showExceptionInfo() {
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

    MessageException::MessageException(const MessageException &e) : Exception(e) {
    }

    MessageException::~MessageException() {
    }

    void MessageException::showMessage() const {
        Trace::writeFormatLine("throw a message exception, message: %s", message().c_str());
        print_stacktrace();
    }

    BindingException::BindingException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                           innerException) {
        showExceptionInfo();
    }

    BindingException::BindingException(const BindingException &e) : Exception(e) {
    }

    BindingException::~BindingException() {
    }

    void BindingException::showMessage() const {
        Trace::writeFormatLine("throw a binding exception, message: %s", message().c_str());
        print_stacktrace();
    }

    OverflowException::OverflowException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                             innerException) {
        showExceptionInfo();
    }

    OverflowException::OverflowException(const OverflowException &e) : Exception(e) {
    }

    OverflowException::~OverflowException() {
    }

    void OverflowException::showMessage() const {
        Trace::writeFormatLine("throw an overflow exception, message: %s", message().c_str());
        print_stacktrace();
    }

    StreamException::StreamException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                         innerException) {
        showExceptionInfo();
    }

    StreamException::StreamException(const StreamException &e) : Exception(e) {
    }

    StreamException::~StreamException() {
    }

    void StreamException::showMessage() const {
        Trace::writeFormatLine("throw a stream exception, message: %s", message().c_str());
        print_stacktrace();
    }

    IOException::IOException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                 innerException) {
        showExceptionInfo();
    }

    IOException::IOException(const IOException &e) : Exception(e) {
    }

    IOException::~IOException() {
    }

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

    ArgumentException::~ArgumentException() {
    }

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

    ArgumentNullException::ArgumentNullException(const ArgumentNullException &e) : ArgumentException(e) {
    }

    ArgumentNullException::~ArgumentNullException() {
    }

    void ArgumentNullException::showMessage() const {
        Trace::writeFormatLine("throw an argument null exception, message: %s, paramName: %s",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &paramName, const String &message,
                                                             const Exception *innerException) : ArgumentException(
            paramName, message, innerException) {
        showExceptionInfo();
    }

    ArgumentOutOfRangeException::ArgumentOutOfRangeException(const ArgumentOutOfRangeException &e) : ArgumentException(
            e) {
    }

    ArgumentOutOfRangeException::~ArgumentOutOfRangeException() {
    }

    void ArgumentOutOfRangeException::showMessage() const {
        Trace::writeFormatLine("throw an argument out of range exception, message: %s, paramName: %s",
                               message().c_str(), paramName().c_str());
        print_stacktrace();
    }

    TimeoutException::TimeoutException(const String &message, const Exception *innerException) : Exception(message,
                                                                                                           innerException) {
        showExceptionInfo();
    }

    TimeoutException::TimeoutException(const TimeoutException &e) : Exception(e) {
    }

    TimeoutException::~TimeoutException() {
    }

    void TimeoutException::showMessage() const {
        Trace::writeFormatLine("throw a timeout exception, message: %s", message().c_str());
        print_stacktrace();
    }

    NotImplementedException::NotImplementedException(const String &message, const Exception *innerException)
            : Exception(message, innerException) {
        showExceptionInfo();
    }

    NotImplementedException::NotImplementedException(const NotImplementedException &e) : Exception(e) {
    }

    NotImplementedException::~NotImplementedException() {
    }

    void NotImplementedException::showMessage() const {
        Trace::writeFormatLine("throw a not implemented exception, message: %s", message().c_str());
        print_stacktrace();
    }

    NotSupportedException::NotSupportedException(const String &message, const Exception *innerException) : Exception(
            message, innerException) {
        showExceptionInfo();
    }

    NotSupportedException::NotSupportedException(const NotSupportedException &e) : Exception(e) {
    }

    NotSupportedException::~NotSupportedException() {
    }

    void NotSupportedException::showMessage() const {
        Trace::writeFormatLine("throw a not supported exception, message: %s", message().c_str());
        print_stacktrace();
    }

    FileNotFoundException::FileNotFoundException(const String &fileName, const Exception *innerException) : Exception(
            fileName, innerException) {
        showExceptionInfo();
    }

    FileNotFoundException::~FileNotFoundException() {
    }

    FileNotFoundException::FileNotFoundException(const FileNotFoundException &e) : Exception(e) {
    }

    const String &FileNotFoundException::fileName() const {
        return message();
    }

    void FileNotFoundException::showMessage() const {
        Trace::writeFormatLine("throw a file not found exception, file name: %s", fileName().c_str());
        print_stacktrace();
    }
}
