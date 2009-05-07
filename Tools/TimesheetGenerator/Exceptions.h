#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

namespace TimesheetGenerator {

    class Exception : public std::exception
    {
    public:
        explicit Exception( const QString& text = QString() )
            : mWhat( text )
        {}

        ~Exception() throw()
        {}

        const char* what() const throw() {
            return qPrintable( mWhat );
        }
    private:
        QString mWhat;
    };

    class UsageException : public Exception
    {
    public:
        explicit UsageException( const QString& text = QString() )
            : Exception( text )
        {}
    };

}

#endif
