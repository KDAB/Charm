#include <CharmExceptions.h>

CharmException::CharmException( const QString& message )
    : m_message( message )
{}

QString CharmException::what() const throw() {
    return m_message;
}

ParseError::ParseError( const QString& text )
    : CharmException( text )
{}

XmlSerializationException:: XmlSerializationException( const QString& message )
    : CharmException( message )
{}

UnsupportedDatabaseVersionException::UnsupportedDatabaseVersionException( const QString& message )
    : CharmException( message )
{}

InvalidTaskListException::InvalidTaskListException( const QString& message )
    : CharmException( message )
{}

TransactionException::TransactionException( const QString& text )
    : CharmException( text )
{}

AlreadyRunningException::AlreadyRunningException()
    : CharmException( QString() )
{}
