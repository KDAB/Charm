#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "ViewHelpers.h"
#include "ReportPreviewWindow.h"
#include "ui_ReportPreviewWindow.h"

ReportPreviewWindow::ReportPreviewWindow( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::ReportPreviewWindow )
    , m_document()
{
    m_ui->setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );
    connect( m_ui->pushButtonClose, SIGNAL( clicked() ),
             SLOT( slotClose() ) );
    connect( m_ui->pushButtonUpdate, SIGNAL( clicked() ),
             SLOT( slotUpdate() ) );
    connect( m_ui->pushButtonSave, SIGNAL( clicked() ),
             SLOT( slotSaveToXml() ) );
    connect( m_ui->pushButtonSaveTotals, SIGNAL( clicked() ),
             SLOT( slotSaveToText() ) );
#ifndef QT_NO_PRINTER
    connect( m_ui->pushButtonPrint, SIGNAL( clicked() ),
             SLOT( slotPrint() ) );
#else
    m_ui->pushButtonPrint->setEnabled(false);
#endif
    resize(600, 600);
}

ReportPreviewWindow::~ReportPreviewWindow()
{
}

void ReportPreviewWindow::setDocument( const QTextDocument* document )
{
    if ( document != 0 ) {
        // we keep a copy, to be able to show different versions of the same document
        QScopedPointer<QTextDocument> docClone( document->clone() );
        m_document.swap( docClone );
        m_ui->textBrowser->setDocument( m_document.data() );
    } else {
        m_ui->textBrowser->setDocument( 0 );
        m_document.reset();
    }
}

QDomDocument ReportPreviewWindow::createReportTemplate() const
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

QPushButton* ReportPreviewWindow::saveToXmlButton() const
{
    return m_ui->pushButtonSave;
}

QPushButton* ReportPreviewWindow::saveToTextButton() const
{
    return m_ui->pushButtonSaveTotals;
}

QPushButton* ReportPreviewWindow::uploadButton() const
{
    return m_ui->pushButtonUpload;
}

void ReportPreviewWindow::slotSaveToXml()
{
}

void ReportPreviewWindow::slotSaveToText()
{
}

void ReportPreviewWindow::slotPrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintDialog dialog( &printer, this );

    if ( dialog.exec() ) {
        m_document->print( &printer );
    }
#endif
}

void ReportPreviewWindow::slotUpdate()
{}

void ReportPreviewWindow::slotClose()
{
    close();
}

#include "moc_ReportPreviewWindow.cpp"
