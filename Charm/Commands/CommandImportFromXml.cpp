#include <QDomDocument>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <Core/CharmExceptions.h>
#include <Core/ControllerInterface.h>

#include "MainWindow.h"
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
        MainWindow* view = dynamic_cast<MainWindow*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by the MainWindow
        QMessageBox::critical( view, tr( "Error importing the Database" ),
                               tr("An error has occurred:\n%1" ).arg( m_error ) );
    }
    return true;
}

#include "CommandImportFromXml.moc"
