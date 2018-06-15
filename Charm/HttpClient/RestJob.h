/*
  RestJob.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#ifndef RESTJOB_H
#define RESTJOB_H

#include "HttpJob.h"
#include <QUrl>
#include <QVariant>
#include <QVariantMap>

class RestJob : public HttpJob
{
    Q_OBJECT
public:

    explicit RestJob(QObject *parent = nullptr);
    ~RestJob() override;

    QByteArray resultData() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

public Q_SLOTS:

    void executeRequest(QNetworkAccessManager *manager) override;
    void handleResult();

private:
    QByteArray m_resultData;
    QUrl m_url;
};

#endif // RESTJOB_H
