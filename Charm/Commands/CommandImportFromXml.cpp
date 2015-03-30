/*
  CommandImportFromXml.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
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

#include "CommandImportFromXml.h"
#include "Core/ControllerInterface.h"

#include <QDomDocument>
#include <QFile>

CommandImportFromXml::CommandImportFromXml( QString filename, QObject* parent )
    : CharmCommand( tr("Import from XML"), parent )
    , m_filename( filename )
{
}

CommandImportFromXml::~CommandImportFromXml()
{
}

bool CommandImportFromXml::prepare()
{
    return true;
}

bool CommandImportFromXml::execute( ControllerInterface* controller )
{
    QFile file( m_filename );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QDomDocument document;
        QString errorMessage;
        int errorLine = 0;
        int errorColumn = 0;
        if ( document.setContent( &file, &errorMessage, &errorLine, &errorColumn ) ) {
            m_error = controller->importDatabaseFromXml( document );
        } else {
            m_error = tr( "Cannot read the XML syntax of the specified file: [%1:%2] %3" ).arg( QString::number( errorLine ), QString::number( errorColumn ), errorMessage );
        }
    } else {
        m_error = tr( "Cannot open the specified file: %1" ).arg( file.errorString() );
    }
    return true;
}

bool CommandImportFromXml::finalize()
{
    // any errors?
    if ( ! m_error.isEmpty() ) {
        showCritical( tr( "Error importing the Database" ), tr("An error has occurred:\n%1" ).arg( m_error ) );
    }
    return true;
}

#include "moc_CommandImportFromXml.cpp"
