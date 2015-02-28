/*
  GetUserInfoJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "GetUserInfoJob.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

GetUserInfoJob::GetUserInfoJob(QObject* parent, const QString &schema)
    : HttpJob(parent),
      m_schema(schema)
{
    QSettings s;
    s.beginGroup(m_schema);
    setDownloadUrl(s.value(QLatin1String("userInfoDownloadUrl")).toUrl());
}

GetUserInfoJob::~GetUserInfoJob()
{
}

QByteArray GetUserInfoJob::userInfo() const
{
    return m_userInfo;
}

QString GetUserInfoJob::schema() const {
    return m_schema;
}

void GetUserInfoJob::setSchema(const QString &schema) {
    m_schema = schema;
}



bool GetUserInfoJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != GetProjectCodes)
        return HttpJob::execute(state, manager);
    QNetworkRequest request(m_downloadUrl);

    QNetworkReply *reply = manager->get(request);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
    return true;
}

bool GetUserInfoJob::handle(QNetworkReply *reply)
{

    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    m_userInfo = reply->readAll();

    delayedNext();
    return true;
}

QUrl GetUserInfoJob::downloadUrl() const
{
    return m_downloadUrl;
}

void GetUserInfoJob::setDownloadUrl(const QUrl& url)
{
    m_downloadUrl = url;
}
