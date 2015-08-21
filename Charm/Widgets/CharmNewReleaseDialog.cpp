/*
  CharmNewReleaseDialog.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "CharmNewReleaseDialog.h"
#include "ui_CharmNewReleaseDialog.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>


CharmNewReleaseDialog::CharmNewReleaseDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::CharmNewReleaseDialog )
{
    m_ui->setupUi( this );
    m_skipUpdate = new QPushButton( tr( "Skip Update" ) );
    connect( m_skipUpdate, SIGNAL(clicked()), SLOT(slotSkipVersion()) );
    m_remindMeLater = new QPushButton( tr( "Remind Me Later" ) );
    connect( m_remindMeLater, SIGNAL(clicked()), SLOT(slotRemindMe()) );
    m_update = new QPushButton( tr( "Update" ) );
    connect( m_update, SIGNAL(clicked()), SLOT(slotLaunchBrowser()) );

    m_ui->buttonBox->addButton( m_skipUpdate, QDialogButtonBox::NoRole );
    m_ui->buttonBox->addButton( m_remindMeLater, QDialogButtonBox::RejectRole );
    m_ui->buttonBox->addButton( m_update, QDialogButtonBox::AcceptRole );

}

void CharmNewReleaseDialog::setVersion( const QString& newVersion, const QString& localVersion )
{
    QString versionText = m_ui->infoLB->text();
    versionText.replace( "NEW", newVersion );
    versionText.replace( "CURRENT", localVersion );
    m_ui->infoLB->setText( versionText );
    m_version = newVersion;
}

void CharmNewReleaseDialog::setDownloadLink( const QUrl& link )
{
    m_link = link;
}

void CharmNewReleaseDialog::setReleaseInformationLink( const QString& link )
{
    QString hyperlink = m_ui->releaseInfoLabel->text();
    hyperlink.replace( "LINK", link );
    m_ui->releaseInfoLabel->setText( hyperlink );
}

void CharmNewReleaseDialog::slotLaunchBrowser()
{
    if ( !QDesktopServices::openUrl( m_link ) )
        QMessageBox::warning( this, tr( "Warning" ), tr( "Could not open url: %1 in your browser, please go to the Charm download page manually!" ).arg( m_link.toString() ) );
    accept();

}

void CharmNewReleaseDialog::slotSkipVersion()
{
    QSettings settings;
    settings.beginGroup( QLatin1String( "UpdateChecker" ) );
    settings.setValue( QLatin1String( "skip-version" ), m_version );
    settings.endGroup();
    accept();
}

void CharmNewReleaseDialog::slotRemindMe()
{
    reject();
}

CharmNewReleaseDialog::~CharmNewReleaseDialog()
{

}

#include "moc_CharmNewReleaseDialog.cpp"
