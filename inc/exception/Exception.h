#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "data/String.h"

using namespace Data;

namespace System {
    class Exception : public std::exception {
    public:
        explicit Exception(const String &message, const Exception *innerException = nullptr);

        Exception(const Exception &e);

        ~Exception() override;

        const char* what() const noexcept override;

        const String &message() const;

        const Exception *innerException() const;

        void showExceptionInfo() const;

    protected:
        virtual void showMessage() const = 0;

        static void print_stacktrace();

    private:
        static void showExceptionInfo(const Exception *exception);

    private:
        String _message;
        const Exception *_innerException;
    };

    class MessageException : public Exception {
    public:
        explicit MessageException(const String &message, const Exception *innerException = nullptr);

        MessageException(const MessageException &e);

        ~MessageException() override;

    protected:
        void showMessage() const override;
    };

    class BindingException : public Exception {
    public:
        explicit BindingException(const String &message, const Exception *innerException = nullptr);

        BindingException(const BindingException &e);

        ~BindingException() override;

    protected:
        void showMessage() const override;
    };

    class OverflowException : public Exception {
    public:
        explicit OverflowException(const String &message, const Exception *innerException = nullptr);

        OverflowException(const OverflowException &e);

        ~OverflowException() override;

    protected:
        void showMessage() const override;
    };

    class StreamException : public Exception {
    public:
        explicit StreamException(const String &message, const Exception *innerException = nullptr);

        StreamException(const StreamException &e);

        ~StreamException() override;

    protected:
        void showMessage() const override;
    };

    class IOException : public Exception {
    public:
        explicit IOException(const String &message, const Exception *innerException = nullptr);

        IOException(const IOException &e);

        ~IOException() override;

    protected:
        void showMessage() const override;
    };

    class ArgumentException : public Exception {
    public:
        explicit ArgumentException(const String &message, const Exception *innerException = nullptr);

        ArgumentException(const String &message, const String &paramName, const Exception *innerException = nullptr);

        ArgumentException(const ArgumentException &e);

        ~ArgumentException() override;

        const String &paramName() const;

    protected:
        void showMessage() const override;

    private:
        String _paramName;
    };

    class ArgumentNullException : public ArgumentException {
    public:
        explicit ArgumentNullException(const String &paramName);

        ArgumentNullException(const ArgumentNullException &e);

        ~ArgumentNullException() override;

    protected:
        void showMessage() const override;

    private:
        static const String ArgumentNull_Generic;
    };

    class ArgumentOutOfRangeException : public ArgumentException {
    public:
        explicit ArgumentOutOfRangeException(const String &message = String::Empty, const Exception *innerException = nullptr);

        ArgumentOutOfRangeException(const String &message, const String &paramName, const Exception *innerException = nullptr);

        ArgumentOutOfRangeException(const ArgumentOutOfRangeException &e);

        ~ArgumentOutOfRangeException() override;

    protected:
        void showMessage() const override;
    };

    class TimeoutException : public Exception {
    public :
        explicit TimeoutException(const String &message, const Exception *innerException = nullptr);

        TimeoutException(const TimeoutException &e);

        ~TimeoutException() override;

    protected:
        void showMessage() const override;
    };

    class NotImplementedException : public Exception {
    public :
        explicit NotImplementedException(const String &message, const Exception *innerException = nullptr);

        NotImplementedException(const NotImplementedException &e);

        ~NotImplementedException() override;

    protected:
        void showMessage() const override;
    };

    class NotSupportedException : public Exception {
    public :
        explicit NotSupportedException(const String &message, const Exception *innerException = nullptr);

        NotSupportedException(const NotSupportedException &e);

        ~NotSupportedException() override;

    protected:
        void showMessage() const override;
    };

    class FileNotFoundException : public Exception {
    public :
        explicit FileNotFoundException(const String &fileName, const Exception *innerException = nullptr);

        FileNotFoundException(const FileNotFoundException &e);

        ~FileNotFoundException() override;

        const String &fileName() const;

    protected:
        void showMessage() const override;
    };

    class PathNotFoundException : public Exception {
    public :
        explicit PathNotFoundException(const String &path, const Exception *innerException = nullptr);

        PathNotFoundException(const PathNotFoundException &e);

        ~PathNotFoundException() override;

        const String &path() const;

    protected:
        void showMessage() const override;
    };
}
#endif // EXCEPTION_H
