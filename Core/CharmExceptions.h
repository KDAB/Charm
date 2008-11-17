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

class XmlSerializationException : public CharmException
{
public:
	XmlSerializationException( const QString& message )
        : CharmException( message )
    {}
};

class UnsupportedDatabaseVersionException : public CharmException
{
public:
	UnsupportedDatabaseVersionException( const QString& message )
        : CharmException( message )
    {}
};

class InvalidTaskListException : public CharmException
{
public:
    InvalidTaskListException( const QString& message )
        : CharmException( message )
    {}
};

#endif
