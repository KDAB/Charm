/*
  UploadTimesheetJob.h

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

#ifndef UPLOADTIMESHEETJOB_H
#define UPLOADTIMESHEETJOB_H

#include "HttpJob.h"

#include <QUrl>

class UploadTimesheetJob : public HttpJob
{
    Q_OBJECT
public:

    explicit UploadTimesheetJob(QObject* parent=nullptr);
    ~UploadTimesheetJob();

    QByteArray payload() const;
    void setPayload(const QByteArray &payload);
    QString fileName() const;
    void setFileName(const QString &fileName);
    QUrl uploadUrl() const;
    void setUploadUrl(const QUrl& url);

public slots:

    bool execute(int state, QNetworkAccessManager *manager) override;
    bool handle(QNetworkReply *reply) override;

protected:

    enum State {
        UploadTimesheet = HttpJob::Base
    };

private:
    QByteArray m_payload;
    QString m_fileName;
    QUrl m_uploadUrl;
};

#endif
