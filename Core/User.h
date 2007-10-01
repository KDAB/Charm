#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User()
        : m_id( 0 )
    {}

    bool isValid() const {
        return m_id != 0;
    }

    const QString& name() const {
        return m_name;
    }

    void setName( const QString& newname ) {
        m_name = newname;
    }

    int id() const {
        return m_id;
    }

    void setId( int newid ) {
        m_id = newid;
    }

private:
    QString m_name;
    int m_id;
};

#endif
