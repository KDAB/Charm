#ifndef INSTALLATION_H
#define INSTALLATION_H

#include <QString>

class Installation
{
public:
	Installation() : m_id(), m_userId()
	{}

	bool isValid() const
	{
		return m_id != 0;
	}

	int id() const
	{
		return m_id;
	}

	void setId( int newid )
	{
		m_id = newid;
	}

	int userId() const
	{
		return m_userId;
	}

	void setUserId( int userId )
	{
		m_userId = userId;
	}

	QString name() const
	{
		return m_name;
	}

	void setName( const QString& newname )
	{
		m_name = newname;
	}

private:
	int m_id;
	int m_userId;
	QString m_name;
};

#endif
