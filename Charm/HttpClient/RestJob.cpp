/*
  RestJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Pál Tóth <pal.toth@kdab.com>

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

#include "RestJob.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

RestJob::RestJob(QObject *parent)
    : HttpJob(parent)
{
}

RestJob::~RestJob()
{
}

QByteArray RestJob::resultData() const
{
    return m_resultData;
}

void RestJob::executeRequest(QNetworkAccessManager *manager)
{
    QNetworkRequest request(m_url);

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &RestJob::handleResult);

    if (reply->error() != QNetworkReply::NoError)
        setErrorFromReplyAndEmitFinishedOrRestart(reply);
}

void RestJob::handleResult()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setErrorFromReplyAndEmitFinishedOrRestart(reply);
        return;
    }

    m_resultData = reply->readAll();
    emitFinishedOrRestart();
}

QUrl RestJob::url() const
{
    return m_url;
}

void RestJob::setUrl(const QUrl &url)
{
    m_url = url;
}
