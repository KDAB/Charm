/*
  GetUserInfoJob.h

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

#ifndef GETUSERINFOJOB_H
#define GETUSERINFOJOB_H

#include "HttpJob.h"
#include <QUrl>
#include <QDebug>
#include <QVariant>
#include <QVariantMap>

class GetUserInfoJob : public HttpJob
{
    Q_OBJECT
public:

    explicit GetUserInfoJob(QObject* parent=nullptr, const QString &schema = " ");
    ~GetUserInfoJob();

    QByteArray userInfo() const;

    QUrl downloadUrl() const;
    void setDownloadUrl(const QUrl& url);
    QString schema() const;
    void setSchema(const QString &schema);

public slots:

    bool execute(int state, QNetworkAccessManager *manager) override;
    bool handle(QNetworkReply *reply) override;

protected:

    enum State {
        GetProjectCodes = HttpJob::Base
    };

private:
    QByteArray m_userInfo;
    QUrl m_downloadUrl;
    QString m_schema;
};

#endif // GETUSERINFOJOB_H
