/*
  GetProjectCodesJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#include "GetProjectCodesJob.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

GetProjectCodesJob::GetProjectCodesJob(QObject* parent)
    : HttpJob(parent)
    , m_verbose( true )
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    setDownloadUrl(s.value(QLatin1String("projectCodeDownloadUrl")).toUrl());
}

GetProjectCodesJob::~GetProjectCodesJob()
{
}

QByteArray GetProjectCodesJob::payload() const
{
    return m_payload;
}

bool GetProjectCodesJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != GetProjectCodes)
        return HttpJob::execute(state, manager);
    QNetworkRequest request(m_downloadUrl);

    QNetworkReply *reply = manager->get(request);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    return true;
}

bool GetProjectCodesJob::handle(QNetworkReply *reply)
{
    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    if (state() != GetProjectCodes)
        return HttpJob::handle(reply);

    m_payload = reply->readAll();
    delayedNext();
    return true;
}

QUrl GetProjectCodesJob::downloadUrl() const
{
    return m_downloadUrl;
}

void GetProjectCodesJob::setDownloadUrl(const QUrl& url)
{
    m_downloadUrl = url;
}

void GetProjectCodesJob::setVerbose( bool verbose )
{
    m_verbose = verbose;
}

bool GetProjectCodesJob::isVerbose() const
{
    return m_verbose;
}

#include "moc_GetProjectCodesJob.cpp"
