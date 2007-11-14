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
    , m_error( false )
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
            m_error = true;
        }
    } else {
        m_error = true;
    }
    m_error = true;
    return true;
}

bool CommandImportFromXml::finalize()
{
    // any errors?
    if ( m_error ) {
        MainWindow* view = dynamic_cast<MainWindow*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by the MainWindow
        QMessageBox::critical( view, tr( "Error importing the Database" ),
                               tr("The database could not be imported, sorry." ) );
    }
    return true;
}

#include "CommandImportFromXml.moc"
