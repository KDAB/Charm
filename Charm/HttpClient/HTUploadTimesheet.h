#ifndef HTUPLOADTIMESHEET_H
#define HTUPLOADTIMESHEET_H 1

#include "HTClient.h"

class HTUploadTimesheet : public HTClient
{
	QByteArray m_payload;

protected:

	enum States {
		sUploadTimesheet = HTClient::sBase + 0
	};

public:

	HTUploadTimesheet();
	virtual ~HTUploadTimesheet();

	const QByteArray & payload() const
	    { return(m_payload); }
	void setPayload(const QByteArray &payload);

public slots:

	virtual bool execute(int state, QNetworkAccessManager *manager);
	virtual bool handle(QNetworkReply *reply);
};

#endif
