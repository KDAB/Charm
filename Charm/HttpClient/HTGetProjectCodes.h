#ifndef HTGETPROJECTCODES_H
#define HTGETPROJECTCODES_H 1

#include "HTClient.h"

class HTGetProjectCodes : public HTClient
{
	QByteArray m_payload;

protected:

	enum States {
		sGetProjectCodes = HTClient::sBase + 0
	};

public:

	HTGetProjectCodes();
	virtual ~HTGetProjectCodes();

	const QByteArray & payload() const
	    { return(m_payload); }

public slots:

	virtual bool execute(int state, QNetworkAccessManager *manager);
	virtual bool handle(QNetworkReply *reply);
};

#endif
