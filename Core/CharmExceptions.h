#ifndef CHARMEXCEPTIONS_H
#define CHARMEXCEPTIONS_H

#include <QString>

class CharmException
{
public:
    explicit CharmException( const QString& message );
    QString what() const throw();
private:
    QString m_message;
};

class ParseError : public CharmException {
public:
    explicit ParseError( const QString& text );
};

class XmlSerializationException : public CharmException
{
public:
    explicit XmlSerializationException( const QString& message );
};

class UnsupportedDatabaseVersionException : public CharmException
{
public:
    explicit UnsupportedDatabaseVersionException( const QString& message );
};

class InvalidTaskListException : public CharmException
{
public:
    explicit InvalidTaskListException( const QString& message );
};

class TransactionException : public CharmException {
public:
    explicit TransactionException( const QString& text = QString() );
};

#endif
