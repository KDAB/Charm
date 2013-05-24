#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <QString>

class TimesheetProcessorException : public std::exception
{
public:
    explicit TimesheetProcessorException(const QString& text = QString::null) :
        mWhat(text)
    {
    }

    ~TimesheetProcessorException() throw()
    {
    }

    const char* what() const throw()
    {
        return qPrintable(mWhat);
    }

private:
    QString mWhat;
};

class UsageException : public TimesheetProcessorException
{
public:
    explicit UsageException(const QString& text = QString::null) :
        TimesheetProcessorException(text)
    {
    }
};

#endif
