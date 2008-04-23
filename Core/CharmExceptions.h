#ifndef CHARMEXCEPTIONS_H
#define CHARMEXCEPTIONS_H

#include <exception>

class CharmException : public std::exception
{
public:
	CharmException( const char* message = 0 )
        : m_message( message )
    {}
    const char* what() const throw() {
        return m_message;
    }
private:
    const char* m_message;
};

class XmlSerializationException : public CharmException
{
public:
	XmlSerializationException( const char* message = 0 )
        : CharmException( message )
    {}
};

class UnsupportedDatabaseVersionException : public CharmException
{
public:
	UnsupportedDatabaseVersionException( const char* message = 0 )
        : CharmException( message )
    {}
};

#endif
