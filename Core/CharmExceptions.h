#ifndef CHARMEXCEPTIONS_H
#define CHARMEXCEPTIONS_H

#include <exception>

class XmlSerializationException : public std::exception
{
public:
    XmlSerializationException( const char* message = 0 )
        : m_message( message )
    {}
    const char* what() const throw() {
        return m_message;
    }
private:
    const char* m_message;
};

#endif
