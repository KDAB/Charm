#ifndef HTCLIENT_H
#define HTCLIENT_H 1

#include <QtCore/QObject>

#include <QtCore/QMap>

class QNetworkAccessManager;
class QNetworkReply;

class HTClient : public QObject
{
	Q_OBJECT;

	QNetworkAccessManager *m_network_manager;

	QString m_username;
	QString m_password;
	QString m_domain;

	int m_current_state;

public:

	HTClient();
	virtual ~HTClient();

	const QString & username() const
	    { return(m_username); }
	void setUsername(const QString &value);

	const QString & password() const
	    { return(m_password); }
	void setPassword(const QString &value);

	const QString & domain() const
	    { return(m_domain); }
	void setDomain(const QString &value);

	int state() const
	    { return(m_current_state); }

public slots:

	virtual void abort();
	virtual void finish();
	virtual void next();
	virtual bool execute(int state, QNetworkAccessManager *manager);
	virtual bool handle(QNetworkReply *reply);

protected:

	enum States {
		sReady  = 0,
		sInit   = 1,
		sLogin  = 2,
		sPortal = 3,
		sBase   = 4
	};

	void restoreConfiguration();
	void saveConfiguration();

	virtual void reset();

signals:

	void success();
	void failure();

};

#endif
