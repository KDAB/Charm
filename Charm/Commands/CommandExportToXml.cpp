#include <QDomDocument>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include <Core/CharmExceptions.h>
#include <Core/ControllerInterface.h>

#include "MainWindow.h"
#include "CommandExportToXml.h"

CommandExportToXml::CommandExportToXml( QString filename, QObject* parent )
    : CharmCommand( parent )
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
        }
    }  catch ( XmlSerializationException& ) {
        m_error = true;
    }
    return true;
}

bool CommandExportToXml::finalize()
{
    // any errors?
    if ( m_error ) {
        MainWindow* view = dynamic_cast<MainWindow*>( owner() );
        Q_ASSERT( view ); // this command is "owned" by the MainWindow
        QMessageBox::critical( view, tr( "Error exporting Database to XML" ),
                               tr("The database could not be exported, sorry." ) );
    }
    return true;
}

#include "CommandExportToXml.moc"
