#include "HTUploadTimesheet.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

#include "HTConfig.h"

HTUploadTimesheet::HTUploadTimesheet()
    : HTClient()
{
}

HTUploadTimesheet::~HTUploadTimesheet()
{
}

void
HTUploadTimesheet::setPayload(const QByteArray &_payload)
{
	m_payload = _payload;
}

bool
HTUploadTimesheet::execute(int state, QNetworkAccessManager *manager)
{
	switch (state) {

	case sUploadTimesheet:
	{
		QByteArray data;

		/* username */
		data += "--KDAB\r\n"
			"Content-Disposition: form-data; name=\"user\"\r\n\r\n";
		data += username().toUtf8();
		data += "\r\n";

		/* payload */
		data += "--KDAB\r\n"
			"Content-Disposition: form-data; name=\"filename\"; filename=\"payload\"\r\n"
			"Content-Type: application/octet-stream\r\n\r\n";
		data += m_payload;
		data += "\r\n";

		/* eot */
		data += "--KDAB--\r\n";

		QNetworkRequest request(QUrl(QString(HT_TIMESHEETUPLOAD_URL).arg(domain())));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=KDAB");
		request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());

		QNetworkReply *reply = manager->post(request, data);

		if (reply->error() != QNetworkReply::NoError)
			QTimer::singleShot(0, this, SLOT(abort()));
	} return true;

	default: break;

	}

	return HTClient::execute(state, manager);
}

bool
HTUploadTimesheet::handle(QNetworkReply *reply)
{
	/* check for failure */
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("HTClient::handle: Oh error! %s", reply->errorString().toUtf8().constData());
		QTimer::singleShot(0, this, SLOT(abort()));
		return false;
	}

	switch (state()) {

	case sUploadTimesheet:
	{
		const QByteArray answer = reply->readAll();

		if (answer.contains("SuccessResultMessage")) {
			qDebug("Timesheet uploaded successfully!");
			QTimer::singleShot(0, this, SLOT(next()));
		} else {
			qDebug("Timesheet upload failed!");
			QTimer::singleShot(0, this, SLOT(abort()));
		}
	} return true;

	default: break;
	}

	return HTClient::handle(reply);
}
