#include "HTClient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>
#include <QTimer>

#include "HTConfig.h"

HTClient::HTClient()
    : m_network_manager(0)
    , m_username()
    , m_password()
    , m_current_state(sReady)
{
	restoreConfiguration();
	reset();
}

HTClient::~HTClient()
{
	m_current_state = 0;
	delete m_network_manager;
	m_network_manager = 0;
}

void
HTClient::setUsername(const QString &value)
{
	m_username = value;
	saveConfiguration();
}

void
HTClient::setPassword(const QString &value)
{
	m_password = value;
	saveConfiguration();
}

void
HTClient::setDomain(const QString &value)
{
	m_domain = value;
	saveConfiguration();
}

void
HTClient::abort()
{
	qDebug("HTClient::abort: Aborted!");
	emit(failure());
	reset();
}

void
HTClient::finish()
{
	qDebug("HTClient::finish: Success!");
	emit(success());
	reset();
}

void
HTClient::next()
{
	/* finish if next state is not found */
	if (!execute(++m_current_state, m_network_manager)) {
		finish();
		return;
	}

	qDebug("HTClient::next: Switched state!");
}

bool
HTClient::execute(int state, QNetworkAccessManager *manager)
{
	switch (state) {

	case sInit:
	case sPortal:
	{
		QNetworkRequest request(QUrl(QString(HT_PORTAL_URL).arg(domain())));

		QNetworkReply *reply = manager->get(request);

		if (reply->error() != QNetworkReply::NoError)
			QTimer::singleShot(0, this, SLOT(abort()));
	} return true;

	case sLogin:
	{
		QUrl data;
		data.addQueryItem("j_username", m_username);
		data.addQueryItem("j_password", m_password);

		QNetworkRequest request(QUrl(QString(HT_LOGIN_URL).arg(domain())));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		request.setHeader(QNetworkRequest::ContentLengthHeader, data.encodedQuery().size());

		QNetworkReply *reply = manager->post(request, data.encodedQuery());

		if (reply->error() != QNetworkReply::NoError)
			QTimer::singleShot(0, this, SLOT(abort()));
	} return true;

	default: break;
	}

	return false;
}

bool
HTClient::handle(QNetworkReply *reply)
{
	/* check for failure */
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("HTClient::handle: Oh error! %s", reply->errorString().toUtf8().constData());
		QTimer::singleShot(0, this, SLOT(abort()));
		return false;
	}

	switch (m_current_state) {

	case sInit:
	case sPortal:
	{
		qDebug("Portal succesfull!");
		QTimer::singleShot(0, this, SLOT(next()));
	} return true;

	case sLogin:
	{
		if (reply->header(QNetworkRequest::LocationHeader).isNull()) {
			qDebug("Login failed!");
			QTimer::singleShot(0, this, SLOT(abort()));
		} else {
			qDebug("Login was succesfull!");
			QTimer::singleShot(0, this, SLOT(next()));
		}
	} return true;

	default: break;
	}

	return false;
}

void
HTClient::restoreConfiguration()
{
    QSettings settings;
    settings.beginGroup("htclient");
    m_username = settings.value("username").toString();
    m_password = QByteArray::fromBase64(settings.value("password").toByteArray());
    m_domain = settings.value("domain").toString();
}

void
HTClient::saveConfiguration()
{
    QSettings settings;
    settings.beginGroup("htclient");
    settings.setValue("username", m_username);
    settings.setValue("password", m_password.toUtf8().toBase64());
    settings.setValue("domain", m_domain);
}

void
HTClient::reset()
{
	m_current_state = sReady;
	delete m_network_manager;
	m_network_manager = new QNetworkAccessManager(this);
	connect(m_network_manager, SIGNAL(finished(QNetworkReply *)), SLOT(handle(QNetworkReply *)));
}

#include "HTClient.moc"
