#ifndef INSTALLATION_H
#define INSTALLATION_H

#include <QString>

class Installation
{
public:
    Installation() : m_id( 0 ) {}

    bool isValid() const {
        return m_id != 0;
    }

    int id() const {
        return m_id;
    }

    void setId( int newid ) {
        m_id = newid;
    }

    QString name() const {
        return m_name;
    }

    void setName( const QString& newname ) {
        m_name = newname;
    }

private:
    int m_id;
    QString m_name;
};

#endif
