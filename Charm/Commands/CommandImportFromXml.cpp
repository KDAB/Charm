#include <QDomDocument>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <Core/ControllerInterface.h>

#include "CharmWindow.h"
#include "CommandImportFromXml.h"

CommandImportFromXml::CommandImportFromXml( QString filename, QObject* parent )
    : CharmCommand( parent )
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
        if ( document.setContent( &file ) ) {
            m_error = controller->importDatabaseFromXml( document );
        } else {
            m_error = tr( "Cannot read the XML syntax of the specified file." );
        }
    } else {
        m_error = tr( "Cannot opened the specified file" );;
    }
    return true;
}

bool CommandImportFromXml::finalize()
{
    // any errors?
    if ( ! m_error.isEmpty() ) {
        CharmWindow* view = dynamic_cast<CharmWindow*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by a CharmWindow
        QMessageBox::critical( view, tr( "Error importing the Database" ),
                               tr("An error has occurred:\n%1" ).arg( m_error ) );
    }
    return true;
}

#include "CommandImportFromXml.moc"
