#include <QtDebug>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBrowser>

#include "ViewHelpers.h"
#include "ReportPreviewWindow.h"
#include "ui_ReportPreviewWindow.h"

ReportPreviewWindow::ReportPreviewWindow( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::ReportPreviewWindow )
    , m_document( 0 )
{
    m_ui->setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );
    connect( m_ui->pushButtonClose, SIGNAL( clicked() ),
             SLOT( slotClose() ) );
    connect( m_ui->pushButtonUpdate, SIGNAL( clicked() ),
             SLOT( slotUpdate() ) );
    connect( m_ui->pushButtonSave, SIGNAL( clicked() ),
             SLOT( slotSaveToXml() ) );
    connect( m_ui->pushButtonPrint, SIGNAL( clicked() ),
             SLOT( slotPrint() ) );
}

ReportPreviewWindow::~ReportPreviewWindow()
{
    delete m_document; m_document = 0;
    delete m_ui; m_ui = 0;
}

void ReportPreviewWindow::setDocument( const QTextDocument* document )
{
    if ( document != 0 ) {
        // we keep a copy, to be able to show different versions of the same document
        m_document = document->clone();
        m_ui->textBrowser->setDocument( m_document );
    } else {
        m_ui->textBrowser->setDocument( 0 );
        delete m_document;
        m_document = 0;
    }
}

QDomDocument ReportPreviewWindow::createReportTemplate()
{
    // create XHTML v1.0 structure:
    QDomDocument doc( "html" );
    // FIXME this is only a rudimentary subset of a valid xhtml 1 document

    // html element
    QDomElement html = doc.createElement( "html" );
    html.setAttribute( "xmlns", "http://www.w3.org/1999/xhtml" );
    doc.appendChild( html );

    // head and body, children of html
    QDomElement head = doc.createElement( "head" );
    html.appendChild( head );
    QDomElement body = doc.createElement( "body" );
    html.appendChild( body );

    return doc;
}

QDomDocument ReportPreviewWindow::createExportTemplate( QString reportType )
{
    QDomDocument doc( "charmreport" );

    // root element:
    QDomElement root = doc.createElement( "charmreport" );
    root.setAttribute( "type", reportType );
    doc.appendChild( root );

    // metadata:
    {
        QDomElement metadata = doc.createElement( "metadata" );
        root.appendChild( metadata );
        QDomElement username = doc.createElement( "username" );
        metadata.appendChild( username );
        QDomText text = doc.createTextNode( CONFIGURATION.user.name() );
        username.appendChild( text );
        QDomElement creationTime = doc.createElement( "creation-time" );
        metadata.appendChild( creationTime );
        QDomText time = doc.createTextNode(
            QDateTime::currentDateTime().toUTC().toString( Qt::ISODate ) );
        creationTime.appendChild( time );
        // FIXME installation id and stuff are probably necessary
    }

    QDomElement report = doc.createElement( "report" );
    root.appendChild( report );

    return doc;
}

void ReportPreviewWindow::slotSaveToXml()
{
}

void ReportPreviewWindow::slotPrint()
{
    QPrinter printer;
    QPrintDialog dialog( &printer, this );

    if ( dialog.exec() ) {
        m_document->print( &printer );
    }
}

void ReportPreviewWindow::slotUpdate()
{}

void ReportPreviewWindow::slotClose()
{
    close();
}

#include "ReportPreviewWindow.moc"
