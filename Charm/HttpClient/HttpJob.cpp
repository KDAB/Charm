/*
  HttpJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "HttpJob.h"
#include "CharmCMake.h"

#include <qt5keychain/keychain.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QSettings>
#include <QXmlStreamReader>
#include <QXmlStreamEntityResolver>
#include <QUrlQuery>

static void setLastAuthenticationFailed(bool failed)
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("httpconfig"));
    settings.setValue(QStringLiteral("lastAuthenticationFailed"), failed);
}

bool HttpJob::lastAuthenticationFailed()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("httpconfig"));
    return settings.value(QStringLiteral("lastAuthenticationFailed"), false).toBool();
}

bool HttpJob::credentialsAvailable()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("httpconfig"));
    return !settings.value(QStringLiteral("username")).toString().isEmpty()
        && settings.value(QStringLiteral("portalUrl")).toUrl().isValid()
        && settings.value(QStringLiteral("loginUrl")).toUrl().isValid();
}

QString HttpJob::extractErrorMessageFromReply(const QByteArray& xml)
{
    class XmlStreamEntityResolver : public QXmlStreamEntityResolver {
    public:
        XmlStreamEntityResolver() : QXmlStreamEntityResolver() {}
        virtual QString	resolveUndeclaredEntity(const QString &name) {
            Q_UNUSED(name);
            return QLatin1String(" "); // replace undeclared entities with a whitespace just to not abort parsing
        }
    };
    XmlStreamEntityResolver resolver;
    QXmlStreamReader reader(xml);
    reader.setEntityResolver(&resolver);
    if (reader.hasError()) {
        return QString(QLatin1String("Error parsing response: %1")).arg(reader.errorString());
    }
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError()) {
            return QString(QLatin1String("Error parsing response: %1")).arg(reader.errorString());
        }
        if (reader.isStartElement() && reader.name() == QLatin1String("div")
                && reader.attributes().value(QLatin1String("class")) == QLatin1String("ErrorResultMessage"))
        {
            return reader.readElementText();
        }
    }

    return QString();
}

HttpJob::HttpJob(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(handle(QNetworkReply*)));
    connect(m_networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    QSettings settings;
    settings.beginGroup(QStringLiteral("httpconfig"));
    setUsername(settings.value(QStringLiteral("username")).toString());
    setPortalUrl(settings.value(QStringLiteral("portalUrl")).toUrl());
    setLoginUrl(settings.value(QStringLiteral("loginUrl")).toUrl());
    m_lastAuthenticationFailed = settings.value(QStringLiteral("lastAuthenticationFailed"), false).toBool();
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

    auto readJob = new ReadPasswordJob(QStringLiteral("Charm"), this);
    connect(readJob, SIGNAL(finished(QKeychain::Job*)), this, SLOT(passwordRead(QKeychain::Job*)));
    readJob->setKey(QStringLiteral("lotsofcake"));
    readJob->start();
}

void HttpJob::passwordRead(QKeychain::Job* j) {
    ReadPasswordJob* job = qobject_cast<ReadPasswordJob*>(j);
    Q_ASSERT(job);

    m_passwordReadError = job->error() != QKeychain::NoError && job->error() != QKeychain::EntryNotFound;

    const QString oldpass = job->error() ? QString() : job->textData();

    const bool authenticationFailed = lastAuthenticationFailed();

    if (oldpass.isEmpty() || authenticationFailed) {
        emit passwordRequested();
        return;
    } else {
        provideRequestedPassword(oldpass);
    }
}

void HttpJob::provideRequestedPassword(const QString &password)
{
    const QString oldpass = m_password;
    m_password = password;

    if (oldpass != m_password && !m_passwordReadError) {
        auto writeJob = new WritePasswordJob(QStringLiteral("Charm"), this);
        connect(writeJob, SIGNAL(finished(QKeychain::Job*)), this, SLOT(passwordWritten()));
        writeJob->setKey(QStringLiteral("lotsofcake"));
        writeJob->setTextData(m_password);
        writeJob->start();
    } else {
        passwordWritten();
    }
}

void HttpJob::passwordRequestCanceled()
{
    setErrorAndEmitFinished(Canceled, tr("Canceled"));
}

void HttpJob::passwordWritten()
{
    emit transferStarted();
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
    /* go to the next state */
    ++m_currentState;

    /* skip login if authenticationRequired() was called meanwhile */
    if (m_authenticationDoneAlready && m_currentState == Login)
        ++m_currentState;

    /* finish if next state is not found */
    if (!execute(m_currentState, m_networkManager)) {
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
            setErrorFromReplyAndEmitFinished(reply);
    } return true;

    case Login:
    {
        QUrlQuery urlQuery;
        urlQuery.addQueryItem(QStringLiteral("j_username"), m_username);
        urlQuery.addQueryItem(QStringLiteral("j_password"), m_password);
        QByteArray encodedQueryPlusPlus = urlQuery.query(QUrl::FullyEncoded).toUtf8().replace('+', "%2b");

        QNetworkRequest request(m_loginUrl);

        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/x-www-form-urlencoded"));
        request.setHeader(QNetworkRequest::ContentLengthHeader, encodedQueryPlusPlus.size());

        QNetworkReply *reply = manager->post(request, encodedQueryPlusPlus);

        if (reply->error() != QNetworkReply::NoError)
            setErrorFromReplyAndEmitFinished(reply);

    } return true;

    default: break;
    }

    return false;
}

bool HttpJob::handle(QNetworkReply *reply)
{
    // check for failure
    if (reply->error() != QNetworkReply::NoError) {
        setErrorFromReplyAndEmitFinished(reply);
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

void HttpJob::authenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    if (!m_authenticationDoneAlready) {
        authenticator->setUser(m_username);
        authenticator->setPassword(m_password);
        m_authenticationDoneAlready = true;
    }
}

void HttpJob::emitFinished()
{
    if (m_errorCode == AuthenticationFailed)
        setLastAuthenticationFailed(true);
    else if (m_errorCode == NoError)
        setLastAuthenticationFailed(false);
    m_networkManager->disconnect(this);
    emit finished(this);
    deleteLater();
}

void HttpJob::setErrorAndEmitFinished(int code, const QString& errorString)
{
    m_errorCode = code;
    m_errorString = errorString;
    emitFinished();
}

void HttpJob::setErrorFromReplyAndEmitFinished(QNetworkReply *reply)
{
    m_authenticationDoneAlready = false;
    switch (reply->error()) {
        case QNetworkReply::HostNotFoundError:
            setErrorAndEmitFinished(HostNotFound, reply->errorString());
            break;
        case QNetworkReply::AuthenticationRequiredError:
            setErrorAndEmitFinished(AuthenticationFailed, reply->errorString());
            break;
        default:
            setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
            break;
    }
}

#include "moc_HttpJob.cpp"
