#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "data/ValueType.h"

namespace Common
{
	class Exception
	{
	public:
        Exception(const String& message, const Exception* innerException = nullptr);
        Exception(const Exception& e);
        virtual ~Exception();

        const String& message() const;

        const Exception* innerException() const;
        void showExceptionInfo();
        
    protected:
        virtual void showMessage() const = 0;
        
        static void print_stacktrace();

    private:
        static void showExceptionInfo(const Exception* exception);
        
	private:
		String _message;
		const Exception* _innerException;
	};

    class MessageException : public Exception
    {
    public:
        MessageException(const String& message, const Exception* innerException = nullptr);
        MessageException(const MessageException& e);
        ~MessageException() override;
        
    protected:
        void showMessage() const override;
    };
    
    class BindingException : public Exception
    {
    public:
        BindingException(const String& message, const Exception* innerException = nullptr);
        BindingException(const BindingException& e);
        ~BindingException() override;
        
    protected:
        void showMessage() const override;
    };
    
    class OverflowException : public Exception
    {
    public:
        OverflowException(const String& message, const Exception* innerException = nullptr);
        OverflowException(const OverflowException& e);
        ~OverflowException() override;
        
    protected:
        void showMessage() const override;
    };
    
    class StreamException : public Exception
    {
    public:
        StreamException(const String& message, const Exception* innerException = nullptr);
        StreamException(const StreamException& e);
        ~StreamException() override;
        
    protected:
        void showMessage() const override;
    };
    
    class IOException : public Exception
    {
    public:
        IOException(const String& message, const Exception* innerException = nullptr);
        IOException(const IOException& e);
        ~IOException() override;
        
    protected:
        void showMessage() const override;
    };

	class ArgumentException : public Exception
	{
	public:
        ArgumentException(const String& message, const Exception* innerException = nullptr);
        ArgumentException(const String& message, const String& paramName, const Exception* innerException = nullptr);
        ArgumentException(const ArgumentException& e);
        ~ArgumentException() override;

        const String& paramName() const;
        
    protected:
        void showMessage() const override;

	private:
		String _paramName;
	};

	class ArgumentNullException : public ArgumentException
	{
	public:
        ArgumentNullException(const String& paramName);
        ArgumentNullException(const ArgumentNullException& e);
        ~ArgumentNullException() override;
        
    protected:
        void showMessage() const override;

	private:
		static const String ArgumentNull_Generic;
	};

	class ArgumentOutOfRangeException : public ArgumentException
	{
	public:
        ArgumentOutOfRangeException(const String& paramName, const String& message = String::Empty, const Exception* innerException = nullptr);
        ArgumentOutOfRangeException(const ArgumentOutOfRangeException& e);
        ~ArgumentOutOfRangeException() override;
        
    protected:
        void showMessage() const override;
	};

	class TimeoutException : public Exception
	{
	public :
        TimeoutException(const String& message, const Exception* innerException = nullptr);
        TimeoutException(const TimeoutException& e);
        ~TimeoutException() override;
        
    protected:
        void showMessage() const override;
	};

	class NotImplementedException : public Exception
	{
	public :
        NotImplementedException(const String& message, const Exception* innerException = nullptr);
        NotImplementedException(const NotImplementedException& e);
        ~NotImplementedException() override;
        
    protected:
        void showMessage() const override;
	};

	class NotSupportedException : public Exception
	{
	public :
        NotSupportedException(const String& message, const Exception* innerException = nullptr);
        NotSupportedException(const NotSupportedException& e);
        ~NotSupportedException() override;
        
    protected:
        void showMessage() const override;
	};

    class FileNotFoundException : public Exception
    {
    public :
        FileNotFoundException(const String& fileName, const Exception* innerException = nullptr);
        FileNotFoundException(const FileNotFoundException& e);
        ~FileNotFoundException() override;
        const String& fileName() const;
        
    protected:
        void showMessage() const override;
    };
}
#endif // EXCEPTION_H
