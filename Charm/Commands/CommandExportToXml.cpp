/*
  CommandExportToXml.cpp

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

#include "CommandExportToXml.h"

#include "Core/CharmExceptions.h"
#include "Core/ControllerInterface.h"

#include <QDomDocument>
#include <QFile>
#include <QTextStream>

CommandExportToXml::CommandExportToXml( QString filename, QObject* parent )
    : CharmCommand( tr("Export to XML"), parent )
    , m_error( false )
    , m_filename( filename )
{
}

CommandExportToXml::~CommandExportToXml()
{
}

bool CommandExportToXml::prepare()
{
    return true;
}

bool CommandExportToXml::execute( ControllerInterface* controller )
{
    try {
        QDomDocument document = controller->exportDatabasetoXml();
        QFile file( m_filename );
        if ( file.open( QIODevice::WriteOnly ) ) {
            QTextStream stream( &file );
            stream << document.toString( 4 );
        } else {
            m_error = true;
            m_errorString = tr( "Could not open %1 for writing: %2" ).arg( m_filename, file.errorString() );
        }
    }  catch ( const XmlSerializationException& e ) {
        m_error = true;
        m_errorString = e.what();
    }
    return true;
}

bool CommandExportToXml::finalize()
{
    // any errors?
    if ( m_error ) {
        showCritical( tr( "Error exporting Database to XML" ), tr("The database could not be exported:\n%1" ).arg( m_errorString ) );
    }
    return !m_error;
}

#include "moc_CommandExportToXml.cpp"
