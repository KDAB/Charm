/*
  HttpJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include <QUrlQuery>

HttpJob::HttpJob(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::authenticationRequired,
            this, &HttpJob::authenticationRequired);
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
    if (m_username.isEmpty()) {
        setErrorAndEmitFinishedOrRestart(NotConfigured,
                                         tr("lotsofcake login data not configured. Download and import the task list manually to configure them."));
        return;
    }

    auto readJob = new ReadPasswordJob(QStringLiteral("Charm"), this);
    connect(readJob, &Job::finished, this, &HttpJob::passwordRead);
    readJob->setKey(QStringLiteral("lotsofcake"));
    readJob->start();
}

void HttpJob::passwordRead(QKeychain::Job *j)
{
    ReadPasswordJob *job = qobject_cast<ReadPasswordJob *>(j);
    Q_ASSERT(job);

    m_passwordReadError = job->error() != QKeychain::NoError
                          && job->error() != QKeychain::EntryNotFound;

    const QString oldpass = job->error() ? QString() : job->textData();

    if (oldpass.isEmpty() || m_lastAuthenticationFailed) {
        emit passwordRequested(oldpass.isEmpty() ? HttpJob::NoPasswordFound : HttpJob::PasswordIncorrect);
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
        connect(writeJob, &Job::finished, this, &HttpJob::passwordWritten);
        writeJob->setKey(QStringLiteral("lotsofcake"));
        writeJob->setTextData(m_password);
        writeJob->start();
    } else {
        passwordWritten();
    }
}

void HttpJob::passwordRequestCanceled()
{
    setErrorAndEmitFinishedOrRestart(Canceled, tr("Canceled"));
}

void HttpJob::passwordWritten()
{
    emit transferStarted();
    executeRequest(m_networkManager);
}

void HttpJob::cancel()
{
    QMetaObject::invokeMethod(this, "doCancel", Qt::QueuedConnection);
}

void HttpJob::doCancel()
{
    setErrorAndEmitFinishedOrRestart(Canceled, tr("Canceled"));
}

void HttpJob::authenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{
    if (!m_authenticationDoneAlready) {
        authenticator->setUser(m_username);
        authenticator->setPassword(m_password);
        m_authenticationDoneAlready = true;
    }
}

void HttpJob::emitFinishedOrRestart()
{
    if (m_errorCode == AuthenticationFailed) {
        m_authenticationDoneAlready = false;
        m_lastAuthenticationFailed = true;
        m_errorCode = NoError;
        m_errorString.clear();
        start();
        return;
    }
    m_networkManager->disconnect(this);
    emit finished(this);
    deleteLater();
}

void HttpJob::setErrorAndEmitFinishedOrRestart(int code, const QString &errorString)
{
    m_errorCode = code;
    m_errorString = errorString;
    emitFinishedOrRestart();
}

void HttpJob::setErrorFromReplyAndEmitFinishedOrRestart(QNetworkReply *reply)
{
    switch (reply->error()) {
    case QNetworkReply::HostNotFoundError:
        setErrorAndEmitFinishedOrRestart(HostNotFound, reply->errorString());
        break;
    case QNetworkReply::AuthenticationRequiredError:
        setErrorAndEmitFinishedOrRestart(AuthenticationFailed, reply->errorString());
        break;
    default:
        setErrorAndEmitFinishedOrRestart(SomethingWentWrong, reply->errorString());
        break;
    }
}
