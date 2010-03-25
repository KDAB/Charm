#ifndef CHARMEXCEPTIONS_H
#define CHARMEXCEPTIONS_H

#include <QString>

class CharmException
{
public:
    CharmException( const QString& message )
        : m_message( message )
    {}
    QString what() const throw() {
        return m_message;
    }
private:
    QString m_message;
};

class ParseError : public CharmException {
public:
    explicit ParseError( const QString& text = QString() )
        : CharmException( text )
    {}
};

class XmlSerializationException : public CharmException
{
public:
    explicit XmlSerializationException( const QString& message )
        : CharmException( message )
    {}
};

class UnsupportedDatabaseVersionException : public CharmException
{
public:
    explicit UnsupportedDatabaseVersionException( const QString& message )
        : CharmException( message )
    {}
};

class InvalidTaskListException : public CharmException
{
public:
    explicit InvalidTaskListException( const QString& message )
        : CharmException( message )
    {}
};

#endif
