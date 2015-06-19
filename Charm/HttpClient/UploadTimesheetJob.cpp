/*
  UploadTimesheetJob.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Guillermo A. Amaral <gamaral@kdab.com>

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

#include "UploadTimesheetJob.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp> // Required for Qt 4
#include <QSettings>

UploadTimesheetJob::UploadTimesheetJob(QObject* parent)
    : HttpJob(parent), m_fileName("payload")
{
    QSettings s;
    s.beginGroup(QLatin1String("httpconfig"));
    setUploadUrl(s.value(QLatin1String("timesheetUploadUrl")).toUrl());
}

UploadTimesheetJob::~UploadTimesheetJob()
{
}

QByteArray UploadTimesheetJob::payload() const
{
    return m_payload;
}

void UploadTimesheetJob::setPayload(const QByteArray &_payload)
{
    m_payload = _payload;
}

QString UploadTimesheetJob::fileName() const
{
    return m_fileName;
}

void UploadTimesheetJob::setFileName(const QString &_fileName)
{
    m_fileName = _fileName;
}

QUrl UploadTimesheetJob::uploadUrl() const
{
    return m_uploadUrl;
}

void UploadTimesheetJob::setUploadUrl(const QUrl& url)
{
    m_uploadUrl = url;
}

bool UploadTimesheetJob::execute(int state, QNetworkAccessManager *manager)
{
    if (state != UploadTimesheet)
        return HttpJob::execute(state, manager);

    QByteArray data;
    QByteArray uploadName;

    /* validate filename */
    if (!m_fileName.contains(QRegExp("^WeeklyTimeSheet-\\d\\d\\d\\d-\\d\\d$"))) {
        qDebug("Invalid filename encountered, using default (\"payload\").");
        uploadName = "payload";
    }
    else uploadName = m_fileName.toUtf8();

    /* username */
    data += "--KDAB\r\n"
            "Content-Disposition: form-data; name=\"user\"\r\n\r\n";
    data += username().toUtf8();
    data += "\r\n";

    /* payload */
    data += "--KDAB\r\n"
            "Content-Disposition: form-data; name=\"" + uploadName + "\"; filename=\"" +
            uploadName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
    data += m_payload;
    data += "\r\n";

    /* eot */
    data += "--KDAB--\r\n";

    QNetworkRequest request(m_uploadUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=KDAB");
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());

    QNetworkReply *reply = manager->post(request, data);

    if (reply->error() != QNetworkReply::NoError)
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
     return true;
}

bool UploadTimesheetJob::handle(QNetworkReply *reply)
{
    /* check for failure */
    if (reply->error() != QNetworkReply::NoError) {
        setErrorAndEmitFinished(SomethingWentWrong, reply->errorString());
        return false;
    }

    if (state() != UploadTimesheet)
        return HttpJob::handle(reply);

    const QByteArray answer = reply->readAll();

    if (answer.contains("SuccessResultMessage")) {
        delayedNext();
    } else {
        const QString errorMessage = extractErrorMessageFromReply(answer);
        setErrorAndEmitFinished(SomethingWentWrong, !errorMessage.isEmpty()
                                ? errorMessage
                                : tr("An error occurred, could not extract details"));
    }
    return true;
}

#include "moc_UploadTimesheetJob.cpp"
