/*
  ConfigurationDialog.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "ConfigurationDialog.h"
#include "Core/CharmConstants.h"

#include <QFileDialog>

ConfigurationDialog::ConfigurationDialog( const Configuration& config,
                                          QWidget* parent )
    : QDialog( parent )
    , m_config( config )
{
    m_ui.setupUi( this );
    m_ui.nameLineEdit->setText( config.user.name() );
    m_ui.databaseLocation->setText( config.localStorageDatabase );
    connect( m_ui.buttonBox, SIGNAL(rejected()), SLOT(reject()) );
    connect( m_ui.buttonBox, SIGNAL(accepted()), SLOT(accept()) );
#ifdef Q_OS_ANDROID
    setWindowState(windowState() | Qt::WindowMaximized);
#endif
}

Configuration ConfigurationDialog::configuration() const
{
    return m_config;
}

void ConfigurationDialog::on_databaseLocation_textChanged( const QString& text )
{
    checkInput();
}

void ConfigurationDialog::accept()
{
    m_config.installationId = 1;
    m_config.user.setId( 1 );
    m_config.user.setName( m_ui.nameLineEdit->text() );
    m_config.localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_config.localStorageDatabase = m_ui.databaseLocation->text();
    m_config.newDatabase = true;
    // m_config.failure = false; currently set by application
    QDialog::accept();
}

void ConfigurationDialog::on_databaseLocationButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName( this, tr( "Choose Database Location..." ) );
    if ( ! filename.isNull() )
    {
        m_ui.databaseLocation->setText( filename );
    }
}

void ConfigurationDialog::on_nameLineEdit_textChanged( const QString& text )
{
    checkInput();
}

void ConfigurationDialog::checkInput()
{
    const bool ok = ! m_ui.databaseLocation->text().isEmpty() && ! m_ui.nameLineEdit->text().isEmpty();
    m_ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ok );
}

#include "moc_ConfigurationDialog.cpp"
