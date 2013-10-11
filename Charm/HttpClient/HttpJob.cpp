#include "HttpJob.h"
#include "keychain.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QSettings>
#include <QSslError>
#include <QInputDialog>
#include <QLineEdit>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QUrlQuery>
#endif

static void setLastAuthenticationFailed(bool failed)
{
    QSettings settings;
    settings.beginGroup("httpconfig");
    settings.setValue(QLatin1String("lastAuthenticationFailed"), failed);
}

static bool lastAuthenticationFailed()
{
    QSettings settings;
    settings.beginGroup("httpconfig");
    return settings.value(QLatin1String("lastAuthenticationFailed"), false).toBool();
}

bool HttpJob::credentialsAvailable()
{
    QSettings settings;
    settings.beginGroup("httpconfig");
    return !settings.value(QLatin1String("username")).toString().isEmpty()
        && settings.value(QLatin1String("portalUrl")).toUrl().isValid()
        && settings.value(QLatin1String("loginUrl")).toUrl().isValid();
}

HttpJob::HttpJob(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_username()
    , m_password()
    , m_currentState(Ready)
    , m_errorCode(NoError)
    , m_dialog()
    , m_lastAuthenticationFailed(true)
{
    connect(m_networkManager, SIGNAL(finished(QNetworkReply *)), SLOT(handle(QNetworkReply *)));
    QSettings settings;
    settings.beginGroup("httpconfig");
    setUsername(settings.value(QLatin1String("username")).toString());
    setPortalUrl(settings.value(QLatin1String("portalUrl")).toUrl());
    setLoginUrl(settings.value(QLatin1String("loginUrl")).toUrl());
    m_lastAuthenticationFailed = settings.value("lastAuthenticationFailed", false).toBool();
}

HttpJob::~HttpJob()
{
}

QString HttpJob::username() const
{
    return m_username;
}

void HttpJob::setUsername(const QString &value)
{
    m_username = value;
}

QString HttpJob::password() const
{
    return m_password;
}

void HttpJob::setPassword(const QString &value)
{
    m_password = value;
}

QUrl HttpJob::portalUrl() const
{
    return m_portalUrl;
}

void HttpJob::setPortalUrl(const QUrl &value)
{
    m_portalUrl = value;
}

QUrl HttpJob::loginUrl() const
{
    return m_loginUrl;
}

void HttpJob::setLoginUrl(const QUrl &value)
{
    m_loginUrl = value;
}

int HttpJob::state() const
{
    return m_currentState;
}

int HttpJob::error() const
{
    return m_errorCode;
}

QString HttpJob::errorString() const
{
    return m_errorString;
}

void HttpJob::start()
{
    QMetaObject::invokeMethod(this, "doStart", Qt::QueuedConnection);
}

using namespace QKeychain;

void HttpJob::doStart()
{
    if (m_username.isEmpty() || m_loginUrl.isEmpty() || m_portalUrl.isEmpty()) {
        setErrorAndEmitFinished(NotConfigured, tr("Timesheet upload and task list download not configured. Download and import the task list manually to configure them."));
        return;
    }

    ReadPasswordJob* readJob = new ReadPasswordJob(QLatin1String("Charm"), this);
    connect(readJob, SIGNAL(finished(QKeychain::Job*)), this, SLOT(passwordRead(QKeychain::Job*)));
    readJob->setKey(QLatin1String("lotsofcake"));
    readJob->start();
}

void HttpJob::passwordRead(QKeychain::Job* j) {
    ReadPasswordJob* job = qobject_cast<ReadPasswordJob*>(j);
    Q_ASSERT(job);

    const bool readError = job->error() != QKeychain::NoError && job->error() != QKeychain::EntryNotFound;

    const QString oldpass = job->error() ? QString() : job->textData();

    const bool authenticationFailed = lastAuthenticationFailed();

    QString newpass;

    if (!oldpass.isEmpty() && !authenticationFailed) {
        newpass = oldpass;
    } else {
        bool ok;
        QPointer<QObject> that( this ); //guard against destruction while dialog is open
        newpass = QInputDialog::getText(m_parentWidget, tr("Password"), tr("Please enter your lotsofcake password"), QLineEdit::Password, oldpass, &ok);
        if (!that || !ok) {
            setErrorAndEmitFinished(Canceled, tr("Canceled"));
            return;
        }
    }

    m_password = newpass;

    if (oldpass != newpass && !readError) {
        WritePasswordJob* writeJob = new WritePasswordJob(QLatin1String("Charm"), this);
        connect(writeJob, SIGNAL(finished(QKeychain::Job*)), this, SLOT(passwordWritten()));
        writeJob->setKey(QLatin1String("lotsofcake"));
        writeJob->setTextData(newpass);
        writeJob->start();
    } else
        passwordWritten();


}

void HttpJob::passwordWritten()
{
    m_dialog = new QProgressDialog(m_parentWidget);
    m_dialog->setWindowTitle(dialogTitle());
    m_dialog->setLabelText(tr("Wait..."));
    m_dialog->show();

    delayedNext();

}

void HttpJob::cancel()
{
    QMetaObject::invokeMethod(this, "doCancel", Qt::QueuedConnection);
}

void HttpJob::doCancel()
{
    setErrorAndEmitFinished(Canceled, tr("Canceled"));
}

void HttpJob::next()
{
    /* finish if next state is not found */
    if (!execute(++m_currentState, m_networkManager)) {
        emitFinished();
        return;
    }
}

void HttpJob::delayedNext()
{
    QMetaObject::invokeMethod(this, "next", Qt::QueuedConnection);
}

bool HttpJob::execute(int state, QNetworkAccessManager *manager)
{
    switch (state) {

    case Init:
    case Portal:
    {
        QNetworkRequest request(m_portalUrl);

        QNetworkReply *reply = manager->get(request);

        if (reply->error() != QNetworkReply::NoError)
            setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    } return true;

    case Login:
    {
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        QUrl data;
        data.addQueryItem("j_username", m_username);
        data.addQueryItem("j_password", m_password);
        QByteArray encodedQueryPlusPlus = data.encodedQuery().replace('+', "%2b").replace(' ', "+");
#else
        QUrlQuery urlQuery;
        urlQuery.addQueryItem("j_username", m_username);
        urlQuery.addQueryItem("j_password", m_password);
        QByteArray encodedQueryPlusPlus = urlQuery.query(QUrl::FullyEncoded).toUtf8().replace('+', "%2b");
#endif

        QNetworkRequest request(m_loginUrl);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        request.setHeader(QNetworkRequest::ContentLengthHeader, encodedQueryPlusPlus.size());

        QNetworkReply *reply = manager->post(request, encodedQueryPlusPlus);

        if (reply->error() != QNetworkReply::NoError)
            setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());

    } return true;

    default: break;
    }

    return false;
}

bool HttpJob::handle(QNetworkReply *reply)
{
    // check for failure
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    switch (m_currentState) {
    case Init:
    case Portal:
    {
        delayedNext();
        return true;
    }
    case Login:
    {
        if (reply->header(QNetworkRequest::LocationHeader).isNull()) {
            setErrorAndEmitFinished(AuthenticationFailed, tr("Login failed. Wrong username or password."));
        } else {
            delayedNext();
        }
        return true;
    }
    default:
        break;
    }

    return false;
}

void HttpJob::emitFinished()
{
    if (m_errorCode == AuthenticationFailed)
        setLastAuthenticationFailed(true);
    else if (m_errorCode == NoError)
        setLastAuthenticationFailed(false);
    m_networkManager->disconnect(this);
    delete m_dialog;
    m_dialog = 0;
    emit finished(this);
    deleteLater();
}

void HttpJob::setErrorAndEmitFinished(int code, const QString& errorString)
{
    m_errorCode = code;
    m_errorString = errorString;
    emitFinished();
}

QWidget* HttpJob::parentWidget() const
{
    return m_parentWidget;
}

void HttpJob::setParentWidget(QWidget* pw)
{
    m_parentWidget = pw;
}

#include "HttpJob.moc"
