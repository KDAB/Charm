/*
  CheckForUpdatesJob.cpp

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

#include "CheckForUpdatesJob.h"

#include <QBuffer>
#include <QByteArray>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>

bool Charm::versionLessThan( const QString& lhs, const QString& rhs )
{
    const QStringList lhsSplit = lhs.split( QLatin1Char('.') );
    const QStringList rhsSplit = rhs.split( QLatin1Char('.') );
    for ( int i = 0; i < lhsSplit.count() && i < rhsSplit.count(); ++i ) {
        const int diff = rhsSplit[i].toInt() - lhsSplit[i].toInt();
        if ( diff != 0 ) {
            return diff > 0;
        }
    }

    for ( int i = lhsSplit.size(); i < rhsSplit.size(); ++i ) {
        if ( rhsSplit[i].toInt() > 0 ) {
            return true;
        }
    }
    return false;
}

CheckForUpdatesJob::CheckForUpdatesJob( QObject* parent )
    : QObject( parent )
{

}

CheckForUpdatesJob::~CheckForUpdatesJob()
{

}

void CheckForUpdatesJob::start()
{
    Q_ASSERT( !m_url.toString().isEmpty() );
    QNetworkAccessManager * manager = new QNetworkAccessManager( this );
    connect(manager, SIGNAL(finished(QNetworkReply*)), SLOT(jobFinished(QNetworkReply*)));
    manager->get( QNetworkRequest( m_url ) );
}

void CheckForUpdatesJob::jobFinished( QNetworkReply* reply )
{
    if ( reply->error() ) {
        const QString errorString = tr( "Could not download update information from %1: %2" ).arg( m_url.toString() ).arg( reply->errorString() );
        m_jobData.errorString = errorString;
        m_jobData.error = reply->error();
    } else {
        QByteArray data = reply->readAll();
        parseXmlData( data );
    }

    reply->deleteLater();
    emit finished( m_jobData );
    deleteLater();
}

void CheckForUpdatesJob::setVerbose( bool verbose )
{
    m_jobData.verbose = verbose;
}

void CheckForUpdatesJob::parseXmlData( const QByteArray& data )
{
    QBuffer buffer;
    buffer.setData( data );
    buffer.open( QIODevice::ReadOnly );

    QDomDocument document;
    QString errorMessage;
    int errorLine = 0;
    int errorColumn = 0;
    if ( !document.setContent( &buffer, &errorMessage, &errorLine, &errorColumn ) )
    {
        m_jobData.errorString = tr( "Invalid XML: [%1:%2] %3" ).arg( QString::number( errorLine ), QString::number( errorColumn ), errorMessage );
        m_jobData.error = 999; // this value is just to have an and does not mean anything - error != 0
        return;
    }

    QDomElement element = document.documentElement();
    QDomElement versionElement = element.firstChildElement( QLatin1String( "version" ) );
    QDomElement linkElement = versionElement.nextSiblingElement( QLatin1String( "link" ) );
    const QString releaseVersion = versionElement.text();
    m_jobData.releaseVersion = releaseVersion;
    QUrl link( linkElement.text() );
    m_jobData.link = link;
    QString releaseInfoLink( linkElement.nextSiblingElement( QLatin1String( "releaseinfolink" ) ).text() );
    m_jobData.releaseInformationLink = releaseInfoLink;

}

void CheckForUpdatesJob::setUrl( const QUrl& url )
{
    m_url = url;
}

#include "moc_CheckForUpdatesJob.cpp"
