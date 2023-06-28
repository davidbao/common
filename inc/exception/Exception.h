#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "data/String.h"

using namespace Data;

namespace System {
    class Exception : public std::exception {
    public:
        explicit Exception(const String &message, const Exception *innerException = nullptr) noexcept;

        Exception(const Exception &e) noexcept;

        ~Exception() noexcept override;

        const char *what() const noexcept override;

        const String &message() const noexcept;

        const Exception *innerException() const noexcept;

        void showExceptionInfo() const noexcept;

    protected:
        virtual void showMessage() const = 0;

        static void print_stacktrace();

    private:
        static void showExceptionInfo(const Exception *exception) noexcept;

    private:
        String _message;
        const Exception *_innerException;
    };

    class MessageException : public Exception {
    public:
        explicit MessageException(const String &message, const Exception *innerException = nullptr) noexcept;

        MessageException(const MessageException &e) noexcept;

        ~MessageException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class BindingException : public Exception {
    public:
        explicit BindingException(const String &message, const Exception *innerException = nullptr) noexcept;

        BindingException(const BindingException &e) noexcept;

        ~BindingException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class OverflowException : public Exception {
    public:
        explicit OverflowException(const String &message, const Exception *innerException = nullptr) noexcept;

        OverflowException(const OverflowException &e) noexcept;

        ~OverflowException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class StreamException : public Exception {
    public:
        explicit StreamException(const String &message, const Exception *innerException = nullptr) noexcept;

        StreamException(const StreamException &e) noexcept;

        ~StreamException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class IOException : public Exception {
    public:
        explicit IOException(const String &message, const Exception *innerException = nullptr) noexcept;

        IOException(const IOException &e) noexcept;

        ~IOException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class ArgumentException : public Exception {
    public:
        explicit ArgumentException(const String &message, const Exception *innerException = nullptr) noexcept;

        ArgumentException(const String &message, const String &paramName,
                          const Exception *innerException = nullptr) noexcept;

        ArgumentException(const ArgumentException &e) noexcept;

        ~ArgumentException() noexcept override;

        const String &paramName() const noexcept;

    protected:
        void showMessage() const noexcept override;

    private:
        String _paramName;
    };

    class ArgumentNullException : public ArgumentException {
    public:
        explicit ArgumentNullException(const String &paramName) noexcept;

        ArgumentNullException(const ArgumentNullException &e) noexcept;

        ~ArgumentNullException() noexcept override;

    protected:
        void showMessage() const noexcept override;

    private:
        static const String ArgumentNull_Generic;
    };

    class ArgumentOutOfRangeException : public ArgumentException {
    public:
        explicit ArgumentOutOfRangeException(const String &message = String::Empty,
                                             const Exception *innerException = nullptr) noexcept;

        ArgumentOutOfRangeException(const String &message, const String &paramName,
                                    const Exception *innerException = nullptr) noexcept;

        ArgumentOutOfRangeException(const ArgumentOutOfRangeException &e) noexcept;

        ~ArgumentOutOfRangeException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class TimeoutException : public Exception {
    public :
        explicit TimeoutException(const String &message, const Exception *innerException = nullptr) noexcept;

        TimeoutException(const TimeoutException &e) noexcept;

        ~TimeoutException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class NotImplementedException : public Exception {
    public :
        explicit NotImplementedException(const String &message, const Exception *innerException = nullptr) noexcept;

        NotImplementedException(const NotImplementedException &e) noexcept;

        ~NotImplementedException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class NotSupportedException : public Exception {
    public :
        explicit NotSupportedException(const String &message, const Exception *innerException = nullptr) noexcept;

        NotSupportedException(const NotSupportedException &e) noexcept;

        ~NotSupportedException() noexcept override;

    protected:
        void showMessage() const noexcept override;
    };

    class FileNotFoundException : public Exception {
    public :
        explicit FileNotFoundException(const String &fileName, const Exception *innerException = nullptr) noexcept;

        FileNotFoundException(const FileNotFoundException &e) noexcept;

        ~FileNotFoundException() noexcept override;

        const String &fileName() const noexcept;

    protected:
        void showMessage() const noexcept override;
    };

    class PathNotFoundException : public Exception {
    public :
        explicit PathNotFoundException(const String &path, const Exception *innerException = nullptr) noexcept;

        PathNotFoundException(const PathNotFoundException &e) noexcept;

        ~PathNotFoundException() noexcept override;

        const String &path() const noexcept;

    protected:
        void showMessage() const noexcept override;
    };
}
#endif // EXCEPTION_H
