#include <QDomDocument>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <Core/CharmExceptions.h>
#include <Core/ControllerInterface.h>

#include "CharmWindow.h"
#include "CommandExportToXml.h"

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
        CharmWindow* view = dynamic_cast<CharmWindow*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by a CharmWindow
        QMessageBox::critical( view, tr( "Error exporting Database to XML" ),
                               tr("The database could not be exported:\n%1" ).arg( m_errorString ) );
    }
    return true;
}

#include "moc_CommandExportToXml.cpp"
