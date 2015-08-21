/*
  CheckForUpdatesJob.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Michel Boyer de la Giroday <michel.giroday@kdab.com>

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

#ifndef CHECKFORUPDATESJOB
#define CHECKFORUPDATESJOB

#include <QObject>
#include <QUrl>

class QByteArray;
class QNetworkReply;
class QUrl;

namespace Charm {
    bool versionLessThan( const QString& lhs, const QString& rhs );
}

class CheckForUpdatesJob : public QObject
{
    Q_OBJECT
public:

    struct JobData {
        QUrl link;
        QString releaseInformationLink;
        QString releaseVersion;
        QString errorString;
        int error = 0; // QNetworkReply::NetworkError or xml parsing error ( 999 )
        bool verbose = false; // display error message or not ( default == not )
    };

    explicit CheckForUpdatesJob( QObject* parent=nullptr );
    ~CheckForUpdatesJob();

    void start();
    void setUrl( const QUrl& url );
    void setVerbose( bool verbose );

signals:
    void finished( CheckForUpdatesJob::JobData data );

private slots:
    void jobFinished( QNetworkReply* reply );

private:
    void parseXmlData( const QByteArray& data );

    QUrl m_url;
    JobData m_jobData;
};

#endif // CHECKFORUPDATESJOB
