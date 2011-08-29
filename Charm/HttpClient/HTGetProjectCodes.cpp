#include "HTGetProjectCodes.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#include "HTConfig.h"

HTGetProjectCodes::HTGetProjectCodes()
    : HTClient()
{
}

HTGetProjectCodes::~HTGetProjectCodes()
{
}

bool
HTGetProjectCodes::execute(int state, QNetworkAccessManager *manager)
{
	switch (state) {

	case sGetProjectCodes:
	{
		QNetworkRequest request(QUrl(QString(HT_PROJECTCODES_URL).arg(domain())));

		QNetworkReply *reply = manager->get(request);

		if (reply->error() != QNetworkReply::NoError)
			QTimer::singleShot(0, this, SLOT(abort()));
	} return true;

	default: break;

	}

	return HTClient::execute(state, manager);
}

bool
HTGetProjectCodes::handle(QNetworkReply *reply)
{
	/* check for failure */
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("HTClient::handle: Oh error! %s", reply->errorString().toUtf8().constData());
		QTimer::singleShot(0, this, SLOT(abort()));
		return false;
	}

	switch (state()) {

	case sGetProjectCodes:
	{
		m_payload = reply->readAll();
		qDebug("Project codes downloaded successfully!");
		QTimer::singleShot(0, this, SLOT(next()));
	} return true;

	default: break;
	}

	return HTClient::handle(reply);
}
